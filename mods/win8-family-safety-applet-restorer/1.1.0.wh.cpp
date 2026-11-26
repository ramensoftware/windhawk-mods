// ==WindhawkMod==
// @id              win8-family-safety-applet-restorer
// @name            Windows 7/8/8.1 Parental Controls / Family Safety Applet Restorer
// @description     This mod restores the legacy Family Safety Control Panel applet on Windows 10 and Windows 11, with an optional Windows 7 skin (Parental Controls)
// @version         1.1.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         control.exe
// @include         dllhost.exe
// @include         svchost.exe
// @architecture    x86-64
// @compilerOptions -lwininet -ladvapi32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8/8.1 Family Safety Applet Restorer

This mod restores the classic Windows 8/8.1 Family Safety (Parental Controls) Control Panel applet that was removed from modern versions of Windows.

**IMPORTANT:** `svchost.exe` must be included in the mod's target processes.

## Screenshots 

![family1](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/familysafety1.png)

![family2](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/familysafety2.png)

## About 


The mod downloads the original Windows binaries from Microsoft’s public symbol server on first use, verifies them with pinned SHA-256 hashes, applies the necessary compatibility patches in memory, and registers the applet virtually so that no permanent system files are modified.

It supports 6 languages: English, Italian, Persian, Spanish, French and Portuguese.  
The language can follow the Windows UI language or be forced to a specific locale.

The applet appears under **User Accounts** in Control Panel Category view and can also be opened directly.

Note: it might take a minute to load the applet because the mod downloads the required dependencies.

## Skins

The **Visual style (skin)** setting chooses how the applet looks:

- **Windows 8/8.1** (default): the original *Family Safety* applet.
- **Windows 7**: the applet is called *Parental Controls* (*Controllo genitori*, *Control parental*, *Contrôle parental*, *Controle dos Pais*, *کنترل والدین*) and uses the Windows 7 icon in Control Panel. In English the word "PC" also becomes "computer", as in Windows 7. The page itself is the Windows 8.1 one, only the name and the icon change.

The Windows 7 icon is stored inside the mod and written to a separate resource-only file, so the Windows binaries downloaded from Microsoft are never modified. Changing the skin reloads the mod; if *Force translations* is off, a restart of Explorer may be needed for the new name to show everywhere.

## Credits 

- AdministratoX - Original implementation and screenshots 
- RandomGuy5747 - Windows 7 edit (icon and "Parental Controls" naming) that the Windows 7 skin is based on
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Page language
  $description: >
    This setting modifies the embedded language for the Family Safety page. "Automatic" follows the Windows UI language.
    An explicit choice with Force translations also regenerates all MUI folders with that catalog.
    With Force translations off, native Windows UI-language MUI selection applies. Requires a mod reload.
  $options:
  - auto: Automatic (Windows UI language)
  - en-US: English (United States)
  - it-IT: Italiano (Italia)
  - fa-IR: فارسی (ایران)
  - es-ES: Español (España)
  - fr-FR: Français (France)
  - pt-BR: Português (Brasil)
- skin: win8
  $name: Visual style (skin)
  $description: >
    This setting selects the visual style. "Windows 8/8.1" keeps the original Family Safety look. "Windows 7" renames the applet to Parental Controls
    (translated in all six languages) and uses the Windows 7 icon in Control Panel. Requires a mod reload.
  $options:
  - win8: Windows 8/8.1 (Family Safety)
  - win7: Windows 7 (Parental Controls)
- forceTranslations: true
  $name: Force translations
  $description: >
    This setting makes the mod use embedded WPCCPL translations instead of native resource lookup, including
    the localized Control Panel name and tooltip. Disable for native WPCCPL loading
    and native per-locale MUI selection. Takes effect immediately.
- keepFilesOnDisable: true
  $name: Keep downloaded files when the mod is disabled
  $description: >
    This setting allows to keep private downloaded and generated files, including all eight MUI folders, for
    offline reuse. No language pack is installed. Turn off to remove mod-owned files
    on unload (locked files are left for later cleanup).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <wininet.h>
#include <wincrypt.h>
#include <objbase.h>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Lightweight stability helpers (based on Windhawk development tips)
// - Catch secondary C++ exceptions so they never escape hooks
// - Simple error budget to reduce work after repeated failures (anti-lag / anti-cascade)
static std::atomic<int> g_secondaryErrorCount{0};
static constexpr int kMaxSecondaryErrors = 12;

static bool StabilityBudgetExceeded() {
    return g_secondaryErrorCount.load(std::memory_order_relaxed) >= kMaxSecondaryErrors;
}

static void NoteSecondaryError(const wchar_t* context) {
    const int n = g_secondaryErrorCount.fetch_add(1, std::memory_order_relaxed) + 1;
    if (n <= kMaxSecondaryErrors) {
        Wh_Log(L"[stability] secondary error (%d/%d): %s", n, kMaxSecondaryErrors, context ? context : L"");
    }
}

namespace pepatch {

using Bytes = std::vector<uint8_t>;

namespace detail {
inline uint16_t R16(const Bytes& b, size_t o) { uint16_t v = 0; if (o + 2 <= b.size()) memcpy(&v, &b[o], 2); return v; }
inline uint32_t R32(const Bytes& b, size_t o) { uint32_t v = 0; if (o + 4 <= b.size()) memcpy(&v, &b[o], 4); return v; }
inline void W16(Bytes& b, size_t o, uint16_t v) { memcpy(&b[o], &v, 2); }
inline void W32(Bytes& b, size_t o, uint32_t v) { memcpy(&b[o], &v, 4); }
inline void W64(Bytes& b, size_t o, uint64_t v) { memcpy(&b[o], &v, 8); }
inline uint32_t Align(uint32_t v, uint32_t a) { return (v + a - 1) / a * a; }
inline bool IEq(const char* a, const char* b) {
    for (; *a && *b; ++a, ++b)
        if ((*a | 0x20) != (*b | 0x20) && *a != *b) return false;   // ASCII case insensitive
    return *a == *b;
}
}  // namespace detail

// Renames every import descriptor that imports from oldName
inline int PatchImportName(Bytes& pe, const char* oldName, const char* newName) {
    using namespace detail;
    const size_t oldLen = strlen(oldName);
    if (strlen(newName) > oldLen) return -1;
    if (pe.size() < 0x40 || pe[0] != 'M' || pe[1] != 'Z') return -1;
    const size_t nt = R32(pe, 0x3c);
    if (nt + 24 > pe.size() || memcmp(&pe[nt], "PE\0\0", 4)) return -1;
    const unsigned nsec = R16(pe, nt + 6);
    const size_t opt = nt + 24;
    const uint16_t magic = R16(pe, opt);
    if (magic != 0x20b && magic != 0x10b) return -1;
    const size_t dd = opt + (magic == 0x20b ? 112 : 96);
    const size_t sec = opt + R16(pe, nt + 20);

    auto rva2off = [&](uint32_t rva) -> size_t {
        for (unsigned i = 0; i < nsec; i++) {
            size_t s = sec + 40u * i;
            uint32_t vs = R32(pe, s + 8), va = R32(pe, s + 12), rs = R32(pe, s + 16), rp = R32(pe, s + 20);
            if (rva >= va && rva - va < std::max(vs, rs)) return (size_t)rp + (rva - va);
        }
        return rva < pe.size() ? rva : (size_t)-1;
    };

    const uint32_t irva = R32(pe, dd + 8);
    if (!irva) return 0;
    int patched = 0;
    for (size_t io = rva2off(irva); io + 20 <= pe.size(); io += 20) {
        const uint32_t oft = R32(pe, io), nm = R32(pe, io + 12), ft = R32(pe, io + 16);
        if (!oft && !nm && !ft) break;
        const size_t no = rva2off(nm);
        if (no == (size_t)-1 || no + oldLen + 1 > pe.size()) continue;
        const char* cur = reinterpret_cast<const char*>(&pe[no]);
        if (strnlen(cur, oldLen + 1) != oldLen || !IEq(cur, oldName)) continue;
        memset(&pe[no], 0, oldLen + 1);
        memcpy(&pe[no], newName, strlen(newName));
        W32(pe, io + 4, 0);   // clear TimeDateStamp: this import is no longer bound to the old DLL
        patched++;
    }
    return patched;
}

struct ShimExport {
    std::string name;
    std::string forward;   // Module.Function: export is a forwarder
    Bytes code;            
};

// Machine code for the two kinds of stub we need
inline Bytes StubReturn0() { return {0x31, 0xC0, 0xC3}; }               
inline Bytes StubReturnArg1() { return {0x48, 0x89, 0xC8, 0xC3}; }      

// Builds a complete x64 DLL image 

inline Bytes BuildShimDll(const std::string& dllName, std::vector<ShimExport> ex) {
    using namespace detail;
    // The loader binary-searches export names, so they must be sorted bytewise
    std::sort(ex.begin(), ex.end(), [](const ShimExport& a, const ShimExport& b) { return a.name < b.name; });
    const uint32_t n = (uint32_t)ex.size();

    Bytes text;
    std::vector<uint32_t> codeOff(n, 0);
    for (uint32_t i = 0; i < n; i++) {
        if (!ex[i].forward.empty()) continue;
        while (text.size() % 16) text.push_back(0xCC);
        codeOff[i] = (uint32_t)text.size();
        text.insert(text.end(), ex[i].code.begin(), ex[i].code.end());
    }
    const bool hasText = !text.empty();

    const uint32_t kHdr = 0x200, kSecAlign = 0x1000, kFileAlign = 0x200;
    const uint32_t textVA = kSecAlign;
    const uint32_t edataVA = hasText ? textVA + Align((uint32_t)text.size(), kSecAlign) : kSecAlign;

    // edata export directory, address table, name pointers, ordinals, then all strings
    const uint32_t eatOff = 40, namesOff = eatOff + 4 * n, ordOff = namesOff + 4 * n, strOff = ordOff + 2 * n;
    Bytes ed(strOff, 0);
    auto addStr = [&](const std::string& s) { uint32_t o = (uint32_t)ed.size(); ed.insert(ed.end(), s.begin(), s.end()); ed.push_back(0); return o; };
    const uint32_t dllNameOff = addStr(dllName);
    for (uint32_t i = 0; i < n; i++) {
        W32(ed, namesOff + 4 * i, edataVA + addStr(ex[i].name));
        W16(ed, ordOff + 2 * i, (uint16_t)i);
        W32(ed, eatOff + 4 * i, ex[i].forward.empty() ? textVA + codeOff[i] : edataVA + addStr(ex[i].forward));
    }
    W32(ed, 12, edataVA + dllNameOff);   // Name
    W32(ed, 16, 1);                      // Base
    W32(ed, 20, n);                      // NumberOfFunctions
    W32(ed, 24, n);                      // NumberOfNames
    W32(ed, 28, edataVA + eatOff);
    W32(ed, 32, edataVA + namesOff);
    W32(ed, 36, edataVA + ordOff);

    const uint32_t nsec = hasText ? 2 : 1;
    const uint32_t textRaw = hasText ? Align((uint32_t)text.size(), kFileAlign) : 0;
    const uint32_t edRaw = Align((uint32_t)ed.size(), kFileAlign);
    const uint32_t sizeOfImage = Align(edataVA + (uint32_t)ed.size(), kSecAlign);

    Bytes b(kHdr + textRaw + edRaw, 0);
    b[0] = 'M'; b[1] = 'Z';
    const uint32_t nt = 0x40;
    W32(b, 0x3c, nt);
    memcpy(&b[nt], "PE\0\0", 4);
    W16(b, nt + 4, 0x8664);              // AMD64
    W16(b, nt + 6, (uint16_t)nsec);
    W16(b, nt + 20, 240);                // SizeOfOptionalHeader
    W16(b, nt + 22, 0x2022);             // EXECUTABLE_IMAGE | LARGE_ADDRESS_AWARE | DLL
    const uint32_t opt = nt + 24;
    W16(b, opt + 0, 0x20b);
    W32(b, opt + 4, hasText ? textRaw : 0);                  // SizeOfCode
    W32(b, opt + 8, edRaw);                                  // SizeOfInitializedData
    W32(b, opt + 20, hasText ? textVA : edataVA);            // BaseOfCode
    W64(b, opt + 24, 0x180000000ull);                        // ImageBase
    W32(b, opt + 32, kSecAlign);
    W32(b, opt + 36, kFileAlign);
    W16(b, opt + 40, 6); W16(b, opt + 48, 6);                // OS / subsystem version 6.0
    W32(b, opt + 56, sizeOfImage);
    W32(b, opt + 60, kHdr);
    W16(b, opt + 68, 2);                                     // WINDOWS_GUI
    W16(b, opt + 70, 0x8160);                                // HIGH_ENTROPY_VA | DYNAMIC_BASE | NX_COMPAT | TERMINAL_SERVER_AWARE
    W64(b, opt + 72, 0x100000); W64(b, opt + 80, 0x1000);
    W64(b, opt + 88, 0x100000); W64(b, opt + 96, 0x1000);
    W32(b, opt + 108, 16);                                   // NumberOfRvaAndSizes
    W32(b, opt + 112, edataVA);                              // export directory
    W32(b, opt + 116, (uint32_t)ed.size());

    uint32_t sh = opt + 240, raw = kHdr;
    auto section = [&](const char* nm, uint32_t vs, uint32_t va, uint32_t rs, uint32_t flags, const Bytes& data) {
        memcpy(&b[sh], nm, strlen(nm));
        W32(b, sh + 8, vs); W32(b, sh + 12, va); W32(b, sh + 16, rs); W32(b, sh + 20, raw);
        W32(b, sh + 36, flags);
        memcpy(&b[raw], data.data(), data.size());
        sh += 40; raw += rs;
    };
    if (hasText) section(".text", (uint32_t)text.size(), textVA, textRaw, 0x60000020, text);
    section(".edata", (uint32_t)ed.size(), edataVA, edRaw, 0x40000040, ed);
    return b;
}

// Read-only helpers: list a PE file's named imports from one module, and its named exports.
// (Same logic as pelist.c, but on an in-memory image.)
namespace detail {
struct PeInfo { size_t nt, opt, dd, sec; unsigned nsec; bool is64; };
inline bool ParsePe(const Bytes& pe, PeInfo& p) {
    if (pe.size() < 0x40 || pe[0] != 'M' || pe[1] != 'Z') return false;
    p.nt = R32(pe, 0x3c);
    if (p.nt + 24 > pe.size() || memcmp(&pe[p.nt], "PE\0\0", 4)) return false;
    p.nsec = R16(pe, p.nt + 6);
    p.opt = p.nt + 24;
    const uint16_t magic = R16(pe, p.opt);
    if (magic != 0x20b && magic != 0x10b) return false;
    p.is64 = magic == 0x20b;
    p.dd = p.opt + (p.is64 ? 112 : 96);
    p.sec = p.opt + R16(pe, p.nt + 20);
    return true;
}
inline size_t RvaToOff(const Bytes& pe, const PeInfo& p, uint32_t rva) {
    for (unsigned i = 0; i < p.nsec; i++) {
        size_t s = p.sec + 40u * i;
        uint32_t vs = R32(pe, s + 8), va = R32(pe, s + 12), rs = R32(pe, s + 16), rp = R32(pe, s + 20);
        if (rva >= va && rva - va < std::max(vs, rs)) return (size_t)rp + (rva - va);
    }
    return rva < pe.size() ? rva : (size_t)-1;
}
inline bool CStrAt(const Bytes& pe, size_t off, std::string& out) {
    if (off >= pe.size()) return false;
    size_t e = off;
    while (e < pe.size() && pe[e]) e++;
    if (e >= pe.size()) return false;
    out.assign(reinterpret_cast<const char*>(&pe[off]), e - off);
    return true;
}
}  // namespace detail

// Names not ordinals that the image imports from module
inline std::vector<std::string> ListNamedImports(const Bytes& pe, const char* module) {
    using namespace detail;
    std::vector<std::string> names;
    PeInfo p;
    if (!ParsePe(pe, p)) return names;
    const uint32_t irva = R32(pe, p.dd + 8);
    if (!irva) return names;
    for (size_t io = RvaToOff(pe, p, irva); io != (size_t)-1 && io + 20 <= pe.size(); io += 20) {
        const uint32_t oft = R32(pe, io), nm = R32(pe, io + 12), ft = R32(pe, io + 16);
        if (!oft && !nm && !ft) break;
        std::string dll;
        if (!CStrAt(pe, RvaToOff(pe, p, nm), dll) || !IEq(dll.c_str(), module)) continue;
        const size_t step = p.is64 ? 8 : 4;
        for (size_t to = RvaToOff(pe, p, oft ? oft : ft); to != (size_t)-1 && to + step <= pe.size(); to += step) {
            uint64_t e = 0;
            if (p.is64) memcpy(&e, &pe[to], 8); else e = R32(pe, to);
            if (!e) break;
            if (e & (p.is64 ? (1ull << 63) : (1ull << 31))) continue;   // ordinal-only import: no name
            std::string fn;
            if (CStrAt(pe, RvaToOff(pe, p, (uint32_t)e + 2), fn)) names.push_back(fn);   // skip 2-byte hint
        }
    }
    return names;
}

// Named exports of the image
inline std::vector<std::string> ListNamedExports(const Bytes& pe) {
    using namespace detail;
    std::vector<std::string> names;
    PeInfo p;
    if (!ParsePe(pe, p)) return names;
    const uint32_t erva = R32(pe, p.dd);
    if (!erva) return names;
    const size_t eo = RvaToOff(pe, p, erva);
    if (eo == (size_t)-1) return names;
    const uint32_t nn = R32(pe, eo + 24);
    const size_t ao = RvaToOff(pe, p, R32(pe, eo + 32));
    if (ao == (size_t)-1) return names;
    for (uint32_t i = 0; i < nn; i++) {
        std::string fn;
        if (CStrAt(pe, RvaToOff(pe, p, R32(pe, ao + 4u * i)), fn)) names.push_back(fn);
    }
    return names;
}


// Resource extraction and .mui PE generation helpers no Windows API used

namespace detail {
inline void Push16(Bytes& b, uint16_t v) {
    uint8_t tmp[2];
    memcpy(tmp, &v, 2);
    b.insert(b.end(), tmp, tmp + 2);
}
inline void Push32(Bytes& b, uint32_t v) {
    uint8_t tmp[4];
    memcpy(tmp, &v, 4);
    b.insert(b.end(), tmp, tmp + 4);
}
inline void PushWStrZ(Bytes& b, const wchar_t* s) {
    if (!s) {
        Push16(b, 0);
        return;
    }
    for (; *s; ++s) {
        Push16(b, static_cast<uint16_t>(*s));
    }
    Push16(b, 0);
}
}  // namespace detail

// Extract a raw resource blob from a PE image by named type 
inline Bytes ExtractResource(const Bytes& pe, const char* namedType,
                             uint32_t idType, uint32_t resId) {
    using namespace detail;
    PeInfo p;
    if (!ParsePe(pe, p)) return {};
    const uint32_t rsrcRva = R32(pe, p.dd + 16);
    if (!rsrcRva) return {};
    const size_t rsrcBase = RvaToOff(pe, p, rsrcRva);
    if (rsrcBase == (size_t)-1 || rsrcBase + 16 > pe.size()) return {};

    const uint16_t named1 = R16(pe, rsrcBase + 12), idc1 = R16(pe, rsrcBase + 14);
    for (uint32_t i = 0; i < (uint32_t)named1 + idc1; ++i) {
        size_t e1 = rsrcBase + 16 + i * 8;
        if (e1 + 8 > pe.size()) break;
        uint32_t nid = R32(pe, e1), o2 = R32(pe, e1 + 4);
        if (!(o2 & 0x80000000u)) continue;
        if (namedType) {
            if (!(nid & 0x80000000u)) continue;
            size_t noff = rsrcBase + (nid & 0x7fffffffu);
            if (noff + 2 > pe.size()) continue;
            uint16_t nlen = R16(pe, noff);
            if (nlen != strlen(namedType) || noff + 2 + nlen * 2 > pe.size()) continue;
            bool match = true;
            for (size_t k = 0; k < nlen; ++k) {
                if (R16(pe, noff + 2 + k * 2) != static_cast<uint8_t>(namedType[k])) {
                    match = false;
                    break;
                }
            }
            if (!match) continue;
        } else {
            if (nid != idType) continue;
        }
        size_t d2 = rsrcBase + (o2 & 0x7fffffffu);
        if (d2 + 16 > pe.size()) break;
        uint16_t named2 = R16(pe, d2 + 12), idc2 = R16(pe, d2 + 14);
        for (uint32_t j = 0; j < (uint32_t)named2 + idc2; ++j) {
            size_t e2 = d2 + 16 + j * 8;
            if (e2 + 8 > pe.size()) break;
            uint32_t iid = R32(pe, e2), o3 = R32(pe, e2 + 4);
            if (iid != resId || !(o3 & 0x80000000u)) continue;
            size_t d3 = rsrcBase + (o3 & 0x7fffffffu);
            if (d3 + 24 > pe.size()) break;
            uint32_t ode = R32(pe, d3 + 20);
            if (ode & 0x80000000u) continue;
            size_t de = rsrcBase + ode;
            if (de + 16 > pe.size()) break;
            uint32_t dataRva = R32(pe, de), dataSize = R32(pe, de + 4);
            size_t rawOff = RvaToOff(pe, p, dataRva);
            if (rawOff == (size_t)-1 || rawOff + dataSize > pe.size()) return {};
            return Bytes(pe.begin() + rawOff, pe.begin() + rawOff + dataSize);
        }
    }
    return {};
}

// Convert a language-neutral (LN) PE's "MUI" RC-config resource (fileType 0x11)
// into a .mui file's "MUI" RC-config resource (fileType 0x12) with the matching
// 16-byte checksum and 16-byte serviceChecksum.
inline Bytes BuildMuiRcFromLn(const Bytes& lnRc, const wchar_t* langTag) {
    using namespace detail;
    if (lnRc.size() < 0x88 || R32(lnRc, 0) != 0xFECDFECDu) return {};
    const uint32_t muiNameOff = R32(lnRc, 84 + 16);
    const uint32_t muiNameSize = R32(lnRc, 84 + 20);
    const uint32_t muiIdOff = R32(lnRc, 84 + 24);
    const uint32_t muiIdSize = R32(lnRc, 84 + 28);
    if (muiNameOff + muiNameSize > lnRc.size() || muiIdOff + muiIdSize > lnRc.size())
        return {};

    Bytes nameBlob(lnRc.begin() + muiNameOff, lnRc.begin() + muiNameOff + muiNameSize);
    Bytes idBlob(lnRc.begin() + muiIdOff, lnRc.begin() + muiIdOff + muiIdSize);
    Bytes langBlob;
    PushWStrZ(langBlob, langTag);
    const uint32_t langSize = (uint32_t)langBlob.size();

    auto pad8 = [](Bytes& b) {
        while (b.size() % 8) b.push_back(0);
    };
    pad8(nameBlob);
    pad8(idBlob);
    pad8(langBlob);

    const uint32_t hdrSize = 0x88;
    const uint32_t nameOff = hdrSize;
    const uint32_t idOff = nameOff + (uint32_t)nameBlob.size();
    const uint32_t langOff = idOff + (uint32_t)idBlob.size();
    const uint32_t totalSize = langOff + (uint32_t)langBlob.size();

    Bytes out(hdrSize, 0);
    W32(out, 0, 0xFECDFECDu);
    W32(out, 4, totalSize);
    W32(out, 8, 0x00010000u);
    W32(out, 12, 0);
    W32(out, 16, 0x12u); // MUI resource fileType
    W32(out, 20, 0);
    W32(out, 24, 0);
    memcpy(&out[28], &lnRc[28], 32); // checksum[16] + serviceChecksum[16]
    W32(out, 84 + 0, nameOff);
    W32(out, 84 + 4, muiNameSize);
    W32(out, 84 + 8, idOff);
    W32(out, 84 + 12, muiIdSize);
    W32(out, 84 + 32, langOff);
    W32(out, 84 + 36, langSize);
    out.insert(out.end(), nameBlob.begin(), nameBlob.end());
    out.insert(out.end(), idBlob.begin(), idBlob.end());
    out.insert(out.end(), langBlob.begin(), langBlob.end());
    return out;
}

struct MuiResourceTypeGroup {
    uint32_t typeId;
    std::vector<std::pair<uint32_t, Bytes>> items;
};

// Build a complete PE32 resource-only .mui DLL containing the "MUI" RC-config
// resource and the specified ID-typed resource groups.
inline Bytes BuildMuiDll(const Bytes& muiRc,
                         const std::vector<MuiResourceTypeGroup>& idTypes,
                         const std::vector<uint16_t>& langIds) {
    using namespace detail;
    const uint32_t kHdr = 0x200, kSecAlign = 0x1000, kFileAlign = 0x200;
    const uint32_t rsrcVA = kSecAlign;

    const uint32_t numTypes = 1 + (uint32_t)idTypes.size();
    const uint32_t rootSize = 16 + 8 * numTypes;
    const uint32_t l2Off = rootSize;

    uint32_t l2Total = 16 + 8 * 1;
    uint32_t totalItems = 1;
    for (const auto& g : idTypes) {
        l2Total += 16 + 8 * (uint32_t)g.items.size();
        totalItems += (uint32_t)g.items.size();
    }
    const uint32_t l3Off = l2Off + l2Total;
    const uint32_t numLangs = (uint32_t)langIds.size();
    const uint32_t l3Total = totalItems * (16 + 8 * numLangs);
    const uint32_t deOff = l3Off + l3Total;
    const uint32_t totalDe = totalItems * numLangs;
    const uint32_t namesOff = deOff + 16 * totalDe;

    Bytes namesBuf;
    Push16(namesBuf, 3);
    Push16(namesBuf, 'M');
    Push16(namesBuf, 'U');
    Push16(namesBuf, 'I');
    while (namesBuf.size() % 8) namesBuf.push_back(0);
    const uint32_t dataStart = namesOff + (uint32_t)namesBuf.size();

    Bytes rootBuf, l2Buf, l3Buf, deBuf, rawBuf;
    auto pushDirHdr = [](Bytes& b, uint16_t named, uint16_t ids) {
        Push32(b, 0);
        Push32(b, 0);
        Push16(b, 4);
        Push16(b, 0);
        Push16(b, named);
        Push16(b, ids);
    };
    pushDirHdr(rootBuf, 1, (uint16_t)idTypes.size());

    auto addType = [&](bool isNamed, uint32_t typeKey,
                       const std::vector<std::pair<uint32_t, Bytes>>& items) {
        const uint32_t curL2 = l2Off + (uint32_t)l2Buf.size();
        Push32(rootBuf, isNamed ? (0x80000000u | namesOff) : typeKey);
        Push32(rootBuf, 0x80000000u | curL2);
        pushDirHdr(l2Buf, 0, (uint16_t)items.size());
        for (const auto& it : items) {
            const uint32_t curL3 = l3Off + (uint32_t)l3Buf.size();
            Push32(l2Buf, it.first);
            Push32(l2Buf, 0x80000000u | curL3);
            pushDirHdr(l3Buf, 0, (uint16_t)numLangs);

            while (rawBuf.size() % 4) rawBuf.push_back(0);
            const uint32_t blobRva = rsrcVA + dataStart + (uint32_t)rawBuf.size();
            const uint32_t blobSize = (uint32_t)it.second.size();
            rawBuf.insert(rawBuf.end(), it.second.begin(), it.second.end());

            for (uint16_t lid : langIds) {
                const uint32_t curDe = deOff + (uint32_t)deBuf.size();
                Push32(l3Buf, lid);
                Push32(l3Buf, curDe);
                Push32(deBuf, blobRva);
                Push32(deBuf, blobSize);
                Push32(deBuf, 1252);
                Push32(deBuf, 0);
            }
        }
    };

    addType(true, 0, {{1u, muiRc}});
    for (const auto& g : idTypes) addType(false, g.typeId, g.items);
    while (rawBuf.size() % 4) rawBuf.push_back(0);

    Bytes rsrc;
    rsrc.insert(rsrc.end(), rootBuf.begin(), rootBuf.end());
    rsrc.insert(rsrc.end(), l2Buf.begin(), l2Buf.end());
    rsrc.insert(rsrc.end(), l3Buf.begin(), l3Buf.end());
    rsrc.insert(rsrc.end(), deBuf.begin(), deBuf.end());
    rsrc.insert(rsrc.end(), namesBuf.begin(), namesBuf.end());
    rsrc.insert(rsrc.end(), rawBuf.begin(), rawBuf.end());

    const uint32_t rsrcRaw = Align((uint32_t)rsrc.size(), kFileAlign);
    const uint32_t rsrcVirt = Align((uint32_t)rsrc.size(), kSecAlign);
    const uint32_t sizeOfImage = rsrcVA + rsrcVirt;

    Bytes pe(kHdr + rsrcRaw, 0);
    pe[0] = 'M';
    pe[1] = 'Z';
    const uint32_t nt = 0x40;
    W32(pe, 0x3c, nt);
    memcpy(&pe[nt], "PE\0\0", 4);
    W16(pe, nt + 4, 0x014c);        // IMAGE_FILE_MACHINE_I386
    W16(pe, nt + 6, 1);             // 1 section (.rsrc)
    W16(pe, nt + 20, 224);          // sizeof(IMAGE_OPTIONAL_HEADER32)
    W16(pe, nt + 22, 0x2102);       // EXECUTABLE_IMAGE | 32BIT_MACHINE | DLL
    const uint32_t opt = nt + 24;
    W16(pe, opt + 0, 0x10b);        // PE32
    W32(pe, opt + 8, rsrcRaw);
    W32(pe, opt + 20, rsrcVA);
    W32(pe, opt + 24, rsrcVA);
    W32(pe, opt + 28, 0x10000000u);
    W32(pe, opt + 32, kSecAlign);
    W32(pe, opt + 36, kFileAlign);
    W16(pe, opt + 40, 10);
    W16(pe, opt + 44, 10);
    W16(pe, opt + 48, 6);
    W32(pe, opt + 56, sizeOfImage);
    W32(pe, opt + 60, kHdr);
    W16(pe, opt + 68, 2);           // IMAGE_SUBSYSTEM_WINDOWS_GUI
    W16(pe, opt + 70, 0x0540);      // DYNAMIC_BASE | NX_COMPAT | NO_SEH
    W32(pe, opt + 72, 0x100000);
    W32(pe, opt + 76, 0x1000);
    W32(pe, opt + 80, 0x100000);
    W32(pe, opt + 84, 0x1000);
    W32(pe, opt + 92, 16);
    W32(pe, opt + 96 + 16, rsrcVA);
    W32(pe, opt + 96 + 20, (uint32_t)rsrc.size());

    const uint32_t sh = opt + 224;
    memcpy(&pe[sh], ".rsrc", 5);
    W32(pe, sh + 8, rsrcVirt);
    W32(pe, sh + 12, rsrcVA);
    W32(pe, sh + 16, rsrcRaw);
    W32(pe, sh + 20, kHdr);
    W32(pe, sh + 36, 0x40000040u);  // CNT_INITIALIZED_DATA | MEM_READ
    memcpy(&pe[kHdr], rsrc.data(), rsrc.size());
    return pe;
}

// Build a complete PE32 resource-only DLL holding the given ID-typed resource groups, without
// any "MUI" configuration resource (used for the Windows 7 skin icons). Type ids, and the item
// ids inside each group, must already be sorted ascending. Every item gets a single language.
inline Bytes BuildResourceOnlyDll(const std::vector<MuiResourceTypeGroup>& types, uint16_t langId) {
    using namespace detail;
    const uint32_t kHdr = 0x200, kSecAlign = 0x1000, kFileAlign = 0x200;
    const uint32_t rsrcVA = kSecAlign;

    uint32_t l2Total = 0, totalItems = 0;
    for (const auto& g : types) {
        l2Total += 16 + 8 * (uint32_t)g.items.size();
        totalItems += (uint32_t)g.items.size();
    }
    if (types.empty() || totalItems == 0) return {};
    const uint32_t l2Off = 16 + 8 * (uint32_t)types.size();
    const uint32_t l3Off = l2Off + l2Total;
    const uint32_t deOff = l3Off + totalItems * (16 + 8);
    const uint32_t dataStart = deOff + 16 * totalItems;

    Bytes rootBuf, l2Buf, l3Buf, deBuf, rawBuf;
    auto pushDirHdr = [](Bytes& b, uint16_t ids) {
        Push32(b, 0);
        Push32(b, 0);
        Push16(b, 4);
        Push16(b, 0);
        Push16(b, 0);      // named entries
        Push16(b, ids);    // id entries
    };
    pushDirHdr(rootBuf, (uint16_t)types.size());
    for (const auto& g : types) {
        const uint32_t curL2 = l2Off + (uint32_t)l2Buf.size();
        Push32(rootBuf, g.typeId);
        Push32(rootBuf, 0x80000000u | curL2);
        pushDirHdr(l2Buf, (uint16_t)g.items.size());
        for (const auto& it : g.items) {
            const uint32_t curL3 = l3Off + (uint32_t)l3Buf.size();
            Push32(l2Buf, it.first);
            Push32(l2Buf, 0x80000000u | curL3);
            pushDirHdr(l3Buf, 1);
            const uint32_t curDe = deOff + (uint32_t)deBuf.size();
            Push32(l3Buf, langId);
            Push32(l3Buf, curDe);

            while (rawBuf.size() % 4) rawBuf.push_back(0);
            Push32(deBuf, rsrcVA + dataStart + (uint32_t)rawBuf.size());
            Push32(deBuf, (uint32_t)it.second.size());
            Push32(deBuf, 1252);
            Push32(deBuf, 0);
            rawBuf.insert(rawBuf.end(), it.second.begin(), it.second.end());
        }
    }
    while (rawBuf.size() % 4) rawBuf.push_back(0);

    Bytes rsrc;
    rsrc.insert(rsrc.end(), rootBuf.begin(), rootBuf.end());
    rsrc.insert(rsrc.end(), l2Buf.begin(), l2Buf.end());
    rsrc.insert(rsrc.end(), l3Buf.begin(), l3Buf.end());
    rsrc.insert(rsrc.end(), deBuf.begin(), deBuf.end());
    rsrc.insert(rsrc.end(), rawBuf.begin(), rawBuf.end());

    const uint32_t rsrcRaw = Align((uint32_t)rsrc.size(), kFileAlign);
    const uint32_t rsrcVirt = Align((uint32_t)rsrc.size(), kSecAlign);
    const uint32_t sizeOfImage = rsrcVA + rsrcVirt;

    Bytes pe(kHdr + rsrcRaw, 0);
    pe[0] = 'M';
    pe[1] = 'Z';
    const uint32_t nt = 0x40;
    W32(pe, 0x3c, nt);
    memcpy(&pe[nt], "PE\0\0", 4);
    W16(pe, nt + 4, 0x014c);        // IMAGE_FILE_MACHINE_I386
    W16(pe, nt + 6, 1);             // 1 section (.rsrc)
    W16(pe, nt + 20, 224);          // sizeof(IMAGE_OPTIONAL_HEADER32)
    W16(pe, nt + 22, 0x2102);       // EXECUTABLE_IMAGE | 32BIT_MACHINE | DLL
    const uint32_t opt = nt + 24;
    W16(pe, opt + 0, 0x10b);        // PE32
    W32(pe, opt + 8, rsrcRaw);
    W32(pe, opt + 20, rsrcVA);
    W32(pe, opt + 24, rsrcVA);
    W32(pe, opt + 28, 0x10000000u);
    W32(pe, opt + 32, kSecAlign);
    W32(pe, opt + 36, kFileAlign);
    W16(pe, opt + 40, 10);
    W16(pe, opt + 44, 10);
    W16(pe, opt + 48, 6);
    W32(pe, opt + 56, sizeOfImage);
    W32(pe, opt + 60, kHdr);
    W16(pe, opt + 68, 2);           // IMAGE_SUBSYSTEM_WINDOWS_GUI
    W16(pe, opt + 70, 0x0540);      // DYNAMIC_BASE | NX_COMPAT | NO_SEH
    W32(pe, opt + 72, 0x100000);
    W32(pe, opt + 76, 0x1000);
    W32(pe, opt + 80, 0x100000);
    W32(pe, opt + 84, 0x1000);
    W32(pe, opt + 92, 16);
    W32(pe, opt + 96 + 16, rsrcVA);
    W32(pe, opt + 96 + 20, (uint32_t)rsrc.size());

    const uint32_t sh = opt + 224;
    memcpy(&pe[sh], ".rsrc", 5);
    W32(pe, sh + 8, rsrcVirt);
    W32(pe, sh + 12, rsrcVA);
    W32(pe, sh + 16, rsrcRaw);
    W32(pe, sh + 20, kHdr);
    W32(pe, sh + 36, 0x40000040u);  // CNT_INITIALIZED_DATA | MEM_READ
    memcpy(&pe[kHdr], rsrc.data(), rsrc.size());
    return pe;
}

}  // namespace pepatch
// =============================================================================
// PURE LOGIC: pinned payloads, embedded resources, and generated private files.
// =============================================================================
// ---- PURE-BEGIN ----
struct FileSpec {
    const wchar_t* name;
    const wchar_t* key;
    const char* sha256;
    uint32_t size;
};

// Original files returned by the Microsoft symbol server. WPCCPL's patched
// SHA-256 is checked separately after its DirectUI import is renamed.
static const FileSpec kFiles[] = {
    {L"wpccpl.dll", L"wpccpl.dll/545037D62C5000/wpccpl.dll",
     "240b4d1abb4330006ecf03ed81c219fdc1c29954ebd696507985fc2e38a39b1b", 2880000},
    {L"wpc.dll", L"wpc.dll/5450445E321000/wpc.dll",
     "a733c32737cca79f5c33f1b775fc6347cf4ab91e4e6531c32024652568ccd9c8", 3256320},
    {L"dui70.dll", L"dui70.dll/9ECBA8E71AB000/dui70.dll",
     "11e0fd294026eca9261d4fb6d39625ac5de84c85ec3d6fcf031bc72cfb9463d3", 1723392},
};
static const char kPatchedWpcCplSha256[] =
    "e3c72f5234031bdc9f7c20e0f029d22e769c57bd8c38f564dc4f82898e154c41";
static const char kGeneratedMuiSha256[] =
    "1a372a37244c5eda2002c2f51b26bf1070aecca90ec3230001fd9473313c171b";

using Bytes = pepatch::Bytes;

// The 32 bytes consist of the MUI checksum and serviceChecksum shared by the
// language-neutral WPCCPL and its matching en-US MUI.
static const uint8_t kExpectedMuiChecksums[32] = {
    0xb1,0x68,0xcd,0x00,0x83,0x59,0x27,0x15,0xb8,0xe3,0xf0,0xf2,0xec,0xf7,0x6c,0xeb,
    0x89,0xf8,0x24,0x22,0x07,0x6f,0x3e,0xea,0xaa,0xa8,0xfe,0x99,0x0e,0x2c,0x03,0xd3,
};

// Original en-US MUI resources, stored as text for review.
// Source en-US MUI SHA-256: f4d4999855f38ab81d46515b95728ca390d4a89ed8d8ffbeba04d5d9e523f97b
// The MUI RC checksum is checked against the downloaded WPCCPL at build time.
// Keep all 16 slots per RT_STRING block, including empty strings. These are
// the original resource texts, not replacements from the translation catalog.
struct EmbeddedStringBlock { uint32_t id; const wchar_t* strings[16]; };
static const EmbeddedStringBlock kEmbeddedStringBlocks[] = {
    {7u, {
        L"",  // 96
        L"",  // 97
        L"",  // 98
        L"",  // 99
        L"Family Safety",  // 100
        L"Change Family Safety settings.",  // 101
        L"",  // 102
        L"",  // 103
        L"",  // 104
        L"",  // 105
        L"",  // 106
        L"",  // 107
        L"",  // 108
        L"",  // 109
        L"Password protected",  // 110
        L"",  // 111
    }},
    {8u, {
        L"Standard user",  // 112
        L"Unknown account type",  // 113
        L"PC administrator",  // 114
        L"Standard user - Family Safety on",  // 115
        L"Can't block this website. Web filtering hasn't been configured for %s.",  // 116
        L"Can't allow this website. Web filtering hasn't been configured for %s.",  // 117
        L"PC administrator - Family Safety on",  // 118
        L"",  // 119
        L"No password",  // 120
        L"Set up how %s will use the PC",  // 121
        L"Control which games and Windows Store apps %s can use",  // 122
        L"Can %s play games?",  // 123
        L"Which ratings are OK for %s?",  // 124
        L"",  // 125
        L"Choose which apps %s can use",  // 126
        L"Family Safety can't monitor an administrator account. To monitor this account with Family Safety, you must change it to a child account.",  // 127
    }},
    {9u, {
        L"",  // 128
        L"Up to %s",  // 129
        L"Allowed website",  // 130
        L"Block website",  // 131
        L"Selected website(s) added to the Allowed list:\n%1",  // 132
        L"Selected website(s) added to the Blocked list:\n%1",  // 133
        L"OK",  // 134
        L"Cancel",  // 135
        L"Get help",  // 136
        L"No rating provided",  // 137
        L"You already have some settings on individual games, which might override these changes. To review specific games you've allowed or blocked, see the \"Allow or Block\" settings.",  // 138
        L"Rated for: {0}+",  // 139
        L"",  // 140
        L"",  // 141
        L"Account picture",  // 142
        L"Picture",  // 143
    }},
    {10u, {
        L"Password",  // 144
        L"Name",  // 145
        L"Type",  // 146
        L"",  // 147
        L"",  // 148
        L"",  // 149
        L"",  // 150
        L"",  // 151
        L"",  // 152
        L"",  // 153
        L"",  // 154
        L"",  // 155
        L"",  // 156
        L"",  // 157
        L"",  // 158
        L"",  // 159
    }},
    {11u, {
        L"",  // 160
        L"",  // 161
        L"",  // 162
        L"",  // 163
        L"You need to be a member of at least one local group to view Family Safety.",  // 164
        L"Family Safety is currently unavailable.",  // 165
        L"",  // 166
        L"You don't have permission to open Family Safety. You must be an administrator on this PC to make changes to Family Safety settings.",  // 167
        L"",  // 168
        L"",  // 169
        L"",  // 170
        L"",  // 171
        L"",  // 172
        L"",  // 173
        L"Control when %s uses the PC",  // 174
        L"Control the websites %s can access online",  // 175
    }},
    {12u, {
        L"%s can play all games and view all Windows Store apps",  // 176
        L"%s can only use games and Windows Store apps I allow",  // 177
        L"Control when %s can use the PC",  // 178
        L"Set the number of hours %s can use the PC per day",  // 179
        L"Set the time of day %s can use the PC",  // 180
        L"Control how long %s can use the PC",  // 181
        L"%s can use the PC all day",  // 182
        L"%s can only use the PC for the amount of time I allow",  // 183
        L"When can %s use the PC?",  // 184
        L"%s can use the PC all day",  // 185
        L"%s can only use the PC during the time ranges I allow",  // 186
        L"Set times when %s can't use PC at all",  // 187
        L"Which websites can %s view?",  // 188
        L"%s can use all websites",  // 189
        L"%s can only use the websites I allow",  // 190
        L"Allow or block specific websites for %s",  // 191
    }},
    {13u, {
        L"Which websites can %s visit?",  // 192
        L"",  // 193
        L"",  // 194
        L"",  // 195
        L"",  // 196
        L"",  // 197
        L"",  // 198
        L"Which apps can %s use?",  // 199
        L"%s can use all apps",  // 200
        L"%s can only use the apps I allow",  // 201
        L"",  // 202
        L"",  // 203
        L"",  // 204
        L"",  // 205
        L"",  // 206
        L"",  // 207
    }},
    {14u, {
        L"",  // 208
        L"{0}'s PC activity for %1 from %2 - %3",  // 209
        L"No games",  // 210
        L"Other parental control settings",  // 211
        L"Family Safety can't monitor this account because it's the only local administrator account. Do you want to create a standard user account and monitor it with Family Safety?",  // 212
        L"",  // 213
        L"%1 can't be added to the list. This app is never allowed to run.",  // 214
        L"Family Safety can't monitor Guest accounts. It's a good idea to turn off Guest accounts if you're using Family Safety for other accounts.",  // 215
        L"Duplicate entry",  // 216
        L"URL is invalid and can't be added.",  // 217
        L"Current settings for %s",  // 218
        L"Web Allow Block Lists|*.WebAllowBlockList|All Files|*.*||",  // 219
        L"WebAllowBlockList",  // 220
        L"There was an error trying to export the website Allow/Block list.",  // 221
        L"There was an error trying to import the website Allow/Block list.",  // 222
        L"The selected app is already in the list.",  // 223
    }},
    {15u, {
        L"There was an error and the selected app wasn't added to the list.",  // 224
        L"%1 can't be added to the list. This app is always allowed to run.",  // 225
        L"Unknown user",  // 226
        L"Sorry, we couldn't make changes to Family Safety settings.",  // 227
        L"An account with that name already exists.\n\nPlease enter a different name.",  // 228
        L"Account names can't contain the following characters: /\\[]\":;|<>+=,?*\n\nPlease enter a different name.",  // 229
        L"",  // 230
        L"Allow website",  // 231
        L"Block website",  // 232
        L"Allow %s to visit selected website?",  // 233
        L"Allow %s to visit selected websites?",  // 234
        L"Block %s from visiting selected website?",  // 235
        L"Block %s from visiting selected websites?",  // 236
        L", including unrated games",  // 237
        L"None",  // 238
        L"All",  // 239
    }},
    {16u, {
        L"Can play",  // 240
        L"Can't play",  // 241
        L"Use rating",  // 242
        L"Always allow",  // 243
        L"Always block",  // 244
        L"",  // 245
        L"Summary of system event PC activity from %1 - %2",  // 246
        L"Visit the %1 website",  // 247
        L"Rating system row",  // 248
        L"logo",  // 249
        L"",  // 250
        L"%1 can't be added to this list because it is a game. \n\nUse game and Windows Store restrictions to block it.",  // 251
        L"",  // 252
        L"New password",  // 253
        L"Confirm new password",  // 254
        L"Enter a password hint",  // 255
    }},
    {17u, {
        L"System recovery account password",  // 256
        L"The System Recovery Account is an administrator account built into Windows Vista to assist if problems arise with your own account. This account could also be used to bypass or turn off Family Safety. You are setting a password to prevent unwanted access to this account.",  // 257
        L"Your account password",  // 258
        L"Your account is an administrator account. Because your account has no password, anyone can use it to bypass or turn off Family Safety. Setting a password will help prevent unwanted access to your account.",  // 259
        L"Your account and the system recovery password",  // 260
        L"The System Recovery Account is an administrator account built into Windows Vista to assist if problems arise with your own account. Your account is also an administrator account. Because these accounts have no passwords, anyone can use them to bypass or turn off Family Safety. Setting passwords will help prevent unwanted access to these accounts.",  // 261
        L"One or more administrator accounts don't have a password. Administrator accounts without passwords allow anyone to bypass or turn off Family Safety. Add passwords to these accounts.",  // 262
        L"",  // 263
        L"",  // 264
        L"The passwords you entered don't match. Enter the new password in each box.",  // 265
        L"",  // 266
        L"Windows can't set the password for your account.",  // 267
        L"Windows can't set the password for the system recovery account.",  // 268
        L"",  // 269
        L"",  // 270
        L"",  // 271
    }},
    {18u, {
        L"",  // 272
        L"",  // 273
        L"",  // 274
        L"",  // 275
        L"",  // 276
        L"",  // 277
        L"",  // 278
        L"",  // 279
        L"Executable File Types|*.exe;*.com;*.cmd;*.bat|All Files (*.*)|*.*||",  // 280
        L"",  // 281
        L"",  // 282
        L"",  // 283
        L"",  // 284
        L"",  // 285
        L"",  // 286
        L"",  // 287
    }},
    {19u, {
        L"",  // 288
        L"",  // 289
        L"",  // 290
        L"",  // 291
        L"",  // 292
        L"",  // 293
        L"",  // 294
        L"",  // 295
        L"",  // 296
        L"",  // 297
        L"",  // 298
        L"",  // 299
        L"Accounts to Monitor",  // 300
        L"User Activities",  // 301
        L"User Settings",  // 302
        L"Game and Windows Store Restrictions",  // 303
    }},
    {20u, {
        L"Rating Level",  // 304
        L"Allow or Block",  // 305
        L"Rating Systems",  // 306
        L"Curfew",  // 307
        L"App Restrictions",  // 308
        L"Web filter settings",  // 309
        L"Web Restrictions",  // 310
        L"Allow or Block Websites",  // 311
        L"",  // 312
        L"",  // 313
        L"User Accounts",  // 314
        L"Time Limits",  // 315
        L"Time Allowance",  // 316
        L"Web Filtering",  // 317
        L"User Activities",  // 318
        L"Websites visited",  // 319
    }},
    {21u, {
        L"Apps used",  // 320
        L"Games played",  // 321
        L"File downloads",  // 322
        L"",  // 323
        L"",  // 324
        L"",  // 325
        L"",  // 326
        L"",  // 327
        L"",  // 328
        L"",  // 329
        L"",  // 330
        L"How do I install additional controls?",  // 331
        L"How do I turn on additional controls?",  // 332
        L"Use Family Safety to get reports of your kids' PC activities, choose what they see online, and set time limits, app restrictions, and more. You can manage these settings on this PC, or on the Family Safety website.",  // 333
        L"Use Family Safety to get reports of your kids’ PC activities, choose what they see online, and set time limits, apps restrictions, and more.\n\nTo turn on Family Safety, add a new child’s account, or change an existing account to a child’s account in <a>Accounts</a>.\n\nTo turn off Family Safety for an account, change the account type from Child to Standard in <a>Accounts</a>.",  // 334
        L"Only web addresses starting with http:// or https:// can be added to the Allowed or Blocked lists.",  // 335
    }},
    {26u, {
        L"Family Safety",  // 400
        L"",  // 401
        L"",  // 402
        L"",  // 403
        L"",  // 404
        L"",  // 405
        L"",  // 406
        L"",  // 407
        L"",  // 408
        L"",  // 409
        L"Unratable content",  // 410
        L"Allow all",  // 411
        L"Allow list only",  // 412
        L"Designed for children",  // 413
        L"Custom",  // 414
        L"Block by list only",  // 415
    }},
    {27u, {
        L"Custom",  // 416
        L"Online communication",  // 417
        L"General interest",  // 418
        L"Warn on adult",  // 419
        L"",  // 420
        L"",  // 421
        L"",  // 422
        L"",  // 423
        L"",  // 424
        L"",  // 425
        L"",  // 426
        L"",  // 427
        L"",  // 428
        L"",  // 429
        L"",  // 430
        L"",  // 431
    }},
    {32u, {
        L"",  // 496
        L"",  // 497
        L"",  // 498
        L"",  // 499
        L"Web address",  // 500
        L"Action taken",  // 501
        L"Category",  // 502
        L"Last visited",  // 503
        L"Visits",  // 504
        L"Name",  // 505
        L"Times used",  // 506
        L"Last used",  // 507
        L"Duration",  // 508
        L"Name",  // 509
        L"File name",  // 510
        L"Local path",  // 511
    }},
    {33u, {
        L"Download date",  // 512
        L"Source",  // 513
        L"Times played",  // 514
        L"Rating",  // 515
        L"",  // 516
        L"",  // 517
        L"",  // 518
        L"",  // 519
        L"{0} visits",  // 520
        L"No activity",  // 521
        L"Blocked",  // 522
        L"Warned, then allowed",  // 523
        L"Allowed",  // 524
        L"{0}, {1}",  // 525
        L"",  // 526
        L"",  // 527
    }},
    {34u, {
        L"",  // 528
        L"",  // 529
        L"See a more comprehensive activity report for {0} on the <a>Family Safety website</a>.",  // 530
        L"See more details in this activity report by <a>managing settings on the Family Safety website</a>.",  // 531
        L"{0}'s PC activities, {1} - {2}",  // 532
        L"",  // 533
        L"",  // 534
        L"On allow list",  // 535
        L"On block list",  // 536
        L"",  // 537
        L"",  // 538
        L"",  // 539
        L"",  // 540
        L"App path",  // 541
        L"",  // 542
        L"",  // 543
    }},
    {35u, {
        L"",  // 544
        L"",  // 545
        L"",  // 546
        L"",  // 547
        L"",  // 548
        L"",  // 549
        L"Activity reporting is on",  // 550
        L"Activity reporting is off",  // 551
        L"",  // 552
        L"",  // 553
        L"",  // 554
        L"",  // 555
        L"",  // 556
        L"",  // 557
        L"",  // 558
        L"",  // 559
    }},
    {36u, {
        L"",  // 560
        L"",  // 561
        L"",  // 562
        L"",  // 563
        L"",  // 564
        L"",  // 565
        L"",  // 566
        L"",  // 567
        L"",  // 568
        L"",  // 569
        L"Duration",  // 570
        L"Account name",  // 571
        L"",  // 572
        L"",  // 573
        L"",  // 574
        L"",  // 575
    }},
    {38u, {
        L"",  // 592
        L"",  // 593
        L"",  // 594
        L"",  // 595
        L"",  // 596
        L"",  // 597
        L"",  // 598
        L"Unknown",  // 599
        L"",  // 600
        L"",  // 601
        L"",  // 602
        L"",  // 603
        L"",  // 604
        L"",  // 605
        L"",  // 606
        L"",  // 607
    }},
    {39u, {
        L"",  // 608
        L"",  // 609
        L"",  // 610
        L"",  // 611
        L"",  // 612
        L"",  // 613
        L"",  // 614
        L"",  // 615
        L"Changed by",  // 616
        L"Account changed",  // 617
        L"Setting affected",  // 618
        L"New setting",  // 619
        L"Old setting",  // 620
        L"New date",  // 621
        L"New time",  // 622
        L"Name",  // 623
    }},
    {40u, {
        L"",  // 624
        L"Type",  // 625
        L"Duration/Blocked",  // 626
        L"",  // 627
        L"",  // 628
        L"",  // 629
        L"%1 sign-in(s) - total time: %2 : %3",  // 630
        L"",  // 631
        L"",  // 632
        L"",  // 633
        L"",  // 634
        L"",  // 635
        L"",  // 636
        L"",  // 637
        L"",  // 638
        L"",  // 639
    }},
    {41u, {
        L"Source",  // 640
        L"Player name",  // 641
        L"Player version",  // 642
        L"Explicit",  // 643
        L"",  // 644
        L"",  // 645
        L"",  // 646
        L"",  // 647
        L"",  // 648
        L"",  // 649
        L"True",  // 650
        L"False",  // 651
        L"",  // 652
        L"",  // 653
        L"",  // 654
        L"",  // 655
    }},
    {43u, {
        L"",  // 672
        L"",  // 673
        L"",  // 674
        L"URL path",  // 675
        L"Category list",  // 676
        L"",  // 677
        L"",  // 678
        L"",  // 679
        L"",  // 680
        L"",  // 681
        L"",  // 682
        L"",  // 683
        L"",  // 684
        L"",  // 685
        L"Path",  // 686
        L"",  // 687
    }},
    {44u, {
        L"Rating",  // 688
        L"",  // 689
        L"",  // 690
        L"",  // 691
        L"",  // 692
        L"",  // 693
        L"",  // 694
        L"",  // 695
        L"",  // 696
        L"",  // 697
        L"",  // 698
        L"",  // 699
        L"",  // 700
        L"",  // 701
        L"",  // 702
        L"",  // 703
    }},
    {45u, {
        L"",  // 704
        L"",  // 705
        L"",  // 706
        L"",  // 707
        L"",  // 708
        L"",  // 709
        L"",  // 710
        L"",  // 711
        L"",  // 712
        L"Unknown User",  // 713
        L"On",  // 714
        L"Off",  // 715
        L"Unknown password or user name",  // 716
        L"Outside allowed time",  // 717
        L"Account disabled",  // 718
        L"Account expired",  // 719
    }},
    {46u, {
        L"Workstation restricted",  // 720
        L"Sign-in type restricted",  // 721
        L"Password expired",  // 722
        L"",  // 723
        L"",  // 724
        L"",  // 725
        L"",  // 726
        L"",  // 727
        L"",  // 728
        L"",  // 729
        L"",  // 730
        L"",  // 731
        L"",  // 732
        L"",  // 733
        L"",  // 734
        L"",  // 735
    }},
    {47u, {
        L"",  // 736
        L"",  // 737
        L"",  // 738
        L"",  // 739
        L"",  // 740
        L"",  // 741
        L"",  // 742
        L"",  // 743
        L"",  // 744
        L"",  // 745
        L"",  // 746
        L"",  // 747
        L"",  // 748
        L"Status",  // 749
        L"File name",  // 750
        L"Local path",  // 751
    }},
    {48u, {
        L"Allowed",  // 752
        L"Blocked",  // 753
        L"%1 files downloaded: %2",  // 754
        L"none",  // 755
        L"%1 - %2 time(s): %3",  // 756
        L"Old date",  // 757
        L"Old time",  // 758
        L"%1 account - made %2 changes to %3 account: %4",  // 759
        L"%1 account - %2 %3 account(s): %4",  // 760
        L"%1 account - %2 failed sign-in attempt(s) - %3: %4",  // 761
        L"%1 account - changed time from %2 %3 to %4 %5",  // 762
        L"Limited users cannot access activity reports.",  // 763
        L"Created",  // 764
        L"Deleted",  // 765
        L"",  // 766
        L"",  // 767
    }},
    {49u, {
        L"Add",  // 768
        L"Remove",  // 769
        L"Still signed in",  // 770
        L"System crash or power down",  // 771
        L"Elevated",  // 772
        L"Demoted",  // 773
        L"Less than one second",  // 774
        L"%dh %dm %ds",  // 775
        L"%dm %ds",  // 776
        L"%ds",  // 777
        L"",  // 778
        L"",  // 779
        L"",  // 780
        L"",  // 781
        L"",  // 782
        L"",  // 783
    }},
    {51u, {
        L"",  // 800
        L"",  // 801
        L"",  // 802
        L"",  // 803
        L"",  // 804
        L"",  // 805
        L"",  // 806
        L"",  // 807
        L"",  // 808
        L"Title",  // 809
        L"Album name",  // 810
        L"",  // 811
        L"",  // 812
        L"Contact name",  // 813
        L"Contact address",  // 814
        L"Change type",  // 815
    }},
    {52u, {
        L"Host name",  // 816
        L"Participants",  // 817
        L"Host IP",  // 818
        L"Sender",  // 819
        L"Recipients",  // 820
        L"Game name",  // 821
        L"",  // 822
        L"Message",  // 823
        L"Game play",  // 824
        L"File exchange",  // 825
        L"Link exchange",  // 826
        L"SMS message",  // 827
        L"%1 - %2 media played",  // 828
        L"%1 - %2 time(s)",  // 829
        L"%s",  // 830
        L"%1 from %2",  // 831
    }},
    {53u, {
        L"%1 sent to %2",  // 832
        L"Error parsing log files",  // 833
        L"%1 - %2 conversations - total time: %3 : %4",  // 834
        L"%1 include %2 - total time: %3",  // 835
        L"%1 - %2 link(s): %3",  // 836
        L"%1 - %2 link(s)",  // 837
        L"%1 - total games played: %2: %3",  // 838
        L"%1 - game(s) played: %2",  // 839
        L"%1 - %2 file(s): %3",  // 840
        L"%1 - %2 file(s)",  // 841
        L"%1 - %2 message(s): %3",  // 842
        L"%1 - %2 message(s)",  // 843
        L"%1 - sent video - %2 time(s), received video - %3 times",  // 844
        L"Sent video to",  // 845
        L"Received video from",  // 846
        L"%1 - sent audio - %2 time(s), received audio - %3 times",  // 847
    }},
    {54u, {
        L"Sent audio to",  // 848
        L"Received audio from",  // 849
        L"Modify",  // 850
        L"",  // 851
        L"",  // 852
        L"",  // 853
        L"",  // 854
        L"",  // 855
        L"",  // 856
        L"",  // 857
        L"",  // 858
        L"",  // 859
        L"TV",  // 860
        L"DVD",  // 861
        L"Other",  // 862
        L"Audio",  // 863
    }},
    {55u, {
        L"CD audio",  // 864
        L"Video ",  // 865
        L"Picture",  // 866
        L"",  // 867
        L"",  // 868
        L"",  // 869
        L"",  // 870
        L"Still running",  // 871
        L"",  // 872
        L"The %s defines these ratings.",  // 873
        L"Rating",  // 874
        L"Descriptor row",  // 875
        L"Rating icon",  // 876
        L"",  // 877
        L"",  // 878
        L"",  // 879
    }},
    {56u, {
        L"Allow list only",  // 880
        L"Kids only",  // 881
        L"Medium",  // 882
        L"Low",  // 883
        L"Custom",  // 884
        L"No restriction",  // 885
        L"Allow list only",  // 886
        L"Designed for children",  // 887
        L"General interest",  // 888
        L"Online communication",  // 889
        L"Warn on adult",  // 890
        L"Webpage|*.html||",  // 891
        L"HTML",  // 892
        L"Failed to export the data to HTML.",  // 893
        L"Product name",  // 894
        L"Folder",  // 895
    }},
    {57u, {
        L"Description",  // 896
        L"File",  // 897
        L"",  // 898
        L"",  // 899
        L"040904B0",  // 900
        L"Manage settings on the Family Safety website",  // 901
        L"Sorry, the service isn't available right now. Please try again later.",  // 902
        L"Time allowance unexpanded icon",  // 903
        L"Time allowance expanded icon",  // 904
        L"Varies",  // 905
        L"{0} Hours",  // 906
        L"{0} Minutes",  // 907
        L"People who can manage these settings online: {0}",  // 908
        L", ",  // 909
        L"You need a Microsoft account to manage these settings online",  // 910
        L"Switch to a Microsoft account now",  // 911
    }},
    {58u, {
        L"Continue using a local account",  // 912
        L"",  // 913
        L"",  // 914
        L"",  // 915
        L"",  // 916
        L"",  // 917
        L"",  // 918
        L"",  // 919
        L"",  // 920
        L"",  // 921
        L"",  // 922
        L"",  // 923
        L"",  // 924
        L"",  // 925
        L"",  // 926
        L"",  // 927
    }},
    {61u, {
        L"",  // 960
        L"To manage this PC online, you need to verify your Microsoft account. Go to PC settings, select Accounts, and then select Your account.",  // 961
        L"",  // 962
        L"",  // 963
        L"",  // 964
        L"",  // 965
        L"",  // 966
        L"",  // 967
        L"",  // 968
        L"",  // 969
        L"",  // 970
        L"",  // 971
        L"",  // 972
        L"",  // 973
        L"",  // 974
        L"",  // 975
    }},
    {63u, {
        L"",  // 992
        L"",  // 993
        L"",  // 994
        L"",  // 995
        L"",  // 996
        L"",  // 997
        L"",  // 998
        L"",  // 999
        L"Extension path",  // 1000
        L"Extension subsystem",  // 1001
        L"Extension image path",  // 1002
        L"Extension name",  // 1003
        L"Extension subtitle",  // 1004
        L"Current rating system",  // 1005
        L"Last Log View",  // 1006
        L"Log view reminder interval",  // 1007
    }},
    {64u, {
        L"",  // 1008
        L"HTTP exemption list",  // 1009
        L"URL exemption list",  // 1010
        L"Custom web filter ID",  // 1011
        L"Custom web filter name",  // 1012
        L"Locale",  // 1013
        L"Allow/Block",  // 1014
        L"Game and Windows Store restrictions",  // 1015
        L"Game allow unrated",  // 1016
        L"Game max allowed",  // 1017
        L"Game category - %s",  // 1018
        L"User WPC enabled",  // 1019
        L"User logging required",  // 1020
        L"User hourly restrictions",  // 1021
        L"Access requests",  // 1022
        L"User logon hours",  // 1023
    }},
    {65u, {
        L"User app restrictions",  // 1024
        L"Web filtering",  // 1025
        L"Web download blocked",  // 1026
        L"Web filter level",  // 1027
        L"Web category - %s",  // 1028
        L"Web block unrated",  // 1029
        L"WPC enabled",  // 1030
        L"Logging required",  // 1031
        L"Rating system path",  // 1032
        L"Unsuccessful sign-in",  // 1033
        L"Net log-in not started",  // 1034
        L"Account locked",  // 1035
        L"Interactive",  // 1036
        L"Remote interactive",  // 1037
        L"Unlock",  // 1038
        L"%s (%dh)",  // 1039
    }},
    {66u, {
        L"None",  // 1040
        L"%s, %s",  // 1041
        L"%s - %s",  // 1042
        L"URL domain",  // 1043
        L"All users",  // 1044
        L"%s days",  // 1045
        L"Provider logo picture",  // 1046
        L"Time limits picture",  // 1047
        L"Games picture",  // 1048
        L"Apps picture",  // 1049
        L"",  // 1050
        L"Never",  // 1051
        L"Yes",  // 1052
        L"No",  // 1053
        L"By rating",  // 1054
        L"Family Safety",  // 1055
    }},
    {67u, {
        L"Warning: Disabling notification area icon",  // 1056
        L"Turning off notifications also means users won't receive warnings before their time restrictions are enforced.",  // 1057
        L"",  // 1058
        L"",  // 1059
        L"",  // 1060
        L"",  // 1061
        L"",  // 1062
        L"",  // 1063
        L"",  // 1064
        L"",  // 1065
        L"%1 can't be added to the list because it's on a removable device or a FAT partition. This app is always allowed to run.",  // 1066
        L"%2 can write to the file %1.\n\n%2 might use this to tamper with and run any app.",  // 1067
        L"Time zone changes by other user",  // 1068
        L"",  // 1069
        L"",  // 1070
        L"",  // 1071
    }},
    {68u, {
        L"%1 application override(s): %2",  // 1072
        L"%1 website(s) allowed: %2",  // 1073
        L"%1 file downloads blocked: %2",  // 1074
        L"Allowed in Allow/Block list",  // 1075
        L"Blocked in Allow/Block list",  // 1076
        L"Not in Allow/Block list",  // 1077
        L"Web filter picture",  // 1078
        L"",  // 1079
        L"Activity viewer picture",  // 1080
        L"Importing this list will replace your old list. Additionally, importing malicious Allow/Block lists can effectively disable your web restriction settings. You shouldn't import this file unless you know who created it. Do you want to import this file?",  // 1081
        L"Hour",  // 1082
        L"Microsoft Family Safety",  // 1083
        L"Windows Store apps",  // 1084
        L"Other",  // 1085
        L"%1 - %2 event(s): %3",  // 1086
        L"%1 - none",  // 1087
    }},
    {69u, {
        L"There are no instant messaging activities for this user. This might be because the instant messaging app being used doesn't support Family Safety reporting, or this user hasn't used instant messaging during this time period.",  // 1088
        L"There are no email activities for this user. This might be because the email app being used doesn't support Family Safety, or this user hasn't used email during this time period.",  // 1089
        L"There are no media player activites for this user. This might be because the media player app being used doesn't support Family Safety reporting, or this user hasn't used a media player during this time period.",  // 1090
        L"Switch back to the Family Safety web filter",  // 1091
        L"The web filter has been switched back to the Family Safety web filter.",  // 1092
        L"Failed to switched the web filter back to the Family Safety web filter.",  // 1093
        L"%s can only use allowed apps",  // 1094
        L"Check the apps that can be used:",  // 1095
        L"The following apps can be used:",  // 1096
        L"",  // 1097
        L"",  // 1098
        L"",  // 1099
        L"",  // 1100
        L"",  // 1101
        L"",  // 1102
        L"",  // 1103
    }},
    {70u, {
        L"",  // 1104
        L"",  // 1105
        L"",  // 1106
        L"Choose a user and set up Family Safety",  // 1107
        L"",  // 1108
        L"",  // 1109
        L"If you want to apply Family Safety to someone who isn't in this list, <a>create a new user account</a> for them to use.",  // 1110
        L"",  // 1111
        L"",  // 1112
        L"",  // 1113
        L"",  // 1114
        L"",  // 1115
        L"",  // 1116
        L"Family Safety:",  // 1117
        L"On, enforce current settings",  // 1118
        L"Off",  // 1119
    }},
    {71u, {
        L"Activity reporting:",  // 1120
        L"On, collect information about PC usage",  // 1121
        L"Off",  // 1122
        L"",  // 1123
        L"",  // 1124
        L"",  // 1125
        L"Windows settings:",  // 1126
        L"Time limits",  // 1127
        L"Game and Windows Store app restrictions",  // 1128
        L"Control by rating or title",  // 1129
        L"Desktop app restrictions",  // 1130
        L"Control the apps allowed on your PC",  // 1131
        L"View activity reports",  // 1132
        L"",  // 1133
        L"",  // 1134
        L"",  // 1135
    }},
    {72u, {
        L"Web filtering:",  // 1136
        L"On",  // 1137
        L"Time limits:",  // 1138
        L"On",  // 1139
        L"Game restrictions:",  // 1140
        L"On",  // 1141
        L"",  // 1142
        L"",  // 1143
        L"Desktop app restrictions:",  // 1144
        L"On",  // 1145
        L"Family Safety:",  // 1146
        L"Off",  // 1147
        L"",  // 1148
        L"Control which types of games and Windows Store apps %username% can use",  // 1149
        L"",  // 1150
        L"",  // 1151
    }},
    {73u, {
        L"",  // 1152
        L"Allow or block games and Windows Store apps by rating",  // 1153
        L"Set game and Windows Store ratings",  // 1154
        L"Set game and Windows Store ratings",  // 1155
        L"Maximum allowed rating: ",  // 1156
        L"",  // 1157
        L"Allow or block any game on your PC by name",  // 1158
        L"Allow or block specific games",  // 1159
        L"",  // 1160
        L"",  // 1161
        L"Control which types of games %username% can play",  // 1162
        L"",  // 1163
        L"",  // 1164
        L"",  // 1165
        L"Which ratings are OK for %username% to play?",  // 1166
        L"The ESRB defines these ratings.",  // 1167
    }},
    {74u, {
        L"",  // 1168
        L"",  // 1169
        L"",  // 1170
        L"Control specific games %username% can and can't play",  // 1171
        L"Allowed ratings:",  // 1172
        L"",  // 1173
        L"Title/Rating",  // 1174
        L"Status",  // 1175
        L"User rating setting",  // 1176
        L"Always allow",  // 1177
        L"Always block",  // 1178
        L"Set times when %username% can't use PC at all",  // 1179
        L"Monday",  // 1180
        L"Tuesday",  // 1181
        L"Wednesday",  // 1182
        L"Thursday",  // 1183
    }},
    {75u, {
        L"Friday",  // 1184
        L"Saturday",  // 1185
        L"Sunday",  // 1186
        L"Midnight (AM)",  // 1187
        L" Noon (PM)",  // 1188
        L"Allowed - Unchecked",  // 1189
        L"Allowed ",  // 1190
        L"Blocked - Checked",  // 1191
        L"Blocked ",  // 1192
        L"",  // 1193
        L"",  // 1194
        L"",  // 1195
        L"",  // 1196
        L"Which rating system do you want to use?",  // 1197
        L"This rating system will apply to all users.",  // 1198
        L"",  // 1199
    }},
    {76u, {
        L"Which apps can %username% use",  // 1200
        L"Use all apps",  // 1201
        L"Only use the apps I allow in this list",  // 1202
        L"Check the apps that can be used:",  // 1203
        L"Add an app to this list:",  // 1204
        L"Browse...",  // 1205
        L"Check all",  // 1206
        L"Uncheck all",  // 1207
        L"Web filter settings",  // 1208
        L"",  // 1209
        L"",  // 1210
        L"",  // 1211
        L"",  // 1212
        L"",  // 1213
        L"Allow or block specific websites",  // 1214
        L"Enter a website to allow or block.",  // 1215
    }},
    {77u, {
        L"",  // 1216
        L"Allow",  // 1217
        L"Block",  // 1218
        L"Allowed websites:",  // 1219
        L"Blocked websites:",  // 1220
        L"Remove",  // 1221
        L"",  // 1222
        L"",  // 1223
        L"",  // 1224
        L"",  // 1225
        L"",  // 1226
        L"",  // 1227
        L"",  // 1228
        L"",  // 1229
        L"Choose a web restriction level:",  // 1230
        L"Allow list only",  // 1231
    }},
    {78u, {
        L"Designed for children",  // 1232
        L"General interest",  // 1233
        L"Online communication",  // 1234
        L"Warn on adult",  // 1235
        L"",  // 1236
        L"The child can view websites on the Allow list and websites designed for children. Adult sites are blocked.",  // 1237
        L"",  // 1238
        L"",  // 1239
        L"",  // 1240
        L"",  // 1241
        L"",  // 1242
        L"",  // 1243
        L"",  // 1244
        L"Block file downloads",  // 1245
        L"",  // 1246
        L"",  // 1247
    }},
    {79u, {
        L"",  // 1248
        L"",  // 1249
        L"",  // 1250
        L"",  // 1251
        L"",  // 1252
        L"",  // 1253
        L"",  // 1254
        L"",  // 1255
        L"",  // 1256
        L"",  // 1257
        L"",  // 1258
        L"",  // 1259
        L"",  // 1260
        L"",  // 1261
        L"",  // 1262
        L"",  // 1263
    }},
    {80u, {
        L"",  // 1264
        L"",  // 1265
        L"Web Browsing",  // 1266
        L"Top 10 Websites Visited",  // 1267
        L"1. ",  // 1268
        L"2. ",  // 1269
        L"3. ",  // 1270
        L"4. ",  // 1271
        L"5. ",  // 1272
        L"6. ",  // 1273
        L"7. ",  // 1274
        L"8. ",  // 1275
        L"9. ",  // 1276
        L"10.",  // 1277
        L"Most Recent 10 Websites Blocked",  // 1278
        L"File Downloads",  // 1279
    }},
    {81u, {
        L"",  // 1280
        L"File Downloads Blocked",  // 1281
        L"",  // 1282
        L"Web Overrides",  // 1283
        L"",  // 1284
        L"System",  // 1285
        L"Logon Times",  // 1286
        L"",  // 1287
        L"Applications Run",  // 1288
        L"Applications",  // 1289
        L"Application Overrides",  // 1290
        L"",  // 1291
        L"Gaming",  // 1292
        L"Games Played",  // 1293
        L"",  // 1294
        L"",  // 1295
    }},
    {82u, {
        L"",  // 1296
        L"",  // 1297
        L"",  // 1298
        L"",  // 1299
        L"",  // 1300
        L"",  // 1301
        L"",  // 1302
        L"",  // 1303
        L"",  // 1304
        L"",  // 1305
        L"",  // 1306
        L"General System Information",  // 1307
        L"Changes to Parental Controls Settings",  // 1308
        L"Account Changes",  // 1309
        L"System Clock Changes",  // 1310
        L"Failed Logon Attempts",  // 1311
    }},
    {83u, {
        L"",  // 1312
        L"",  // 1313
        L"",  // 1314
        L"",  // 1315
        L"",  // 1316
        L"",  // 1317
        L"",  // 1318
        L"User Summary",  // 1319
        L"",  // 1320
        L"",  // 1321
        L"More settings:",  // 1322
        L"Web Address Edit Control",  // 1323
        L"",  // 1324
        L"",  // 1325
        L"",  // 1326
        L"",  // 1327
    }},
    {84u, {
        L"",  // 1328
        L"The child can view websites on the Allow list, websites designed for children, and websites from the general interest, social networking, web chat, and web mail categories. Adult sites are blocked.",  // 1329
        L"",  // 1330
        L"",  // 1331
        L"",  // 1332
        L"",  // 1333
        L"",  // 1334
        L"Current settings:",  // 1335
        L"",  // 1336
        L"",  // 1337
        L"Switch back to the Family Safety web filter",  // 1338
        L"Switch",  // 1339
        L"",  // 1340
        L"Controls",  // 1341
        L"",  // 1342
        L"",  // 1343
    }},
    {85u, {
        L"",  // 1344
        L"Provider",  // 1345
        L"Select a provider:",  // 1346
        L"",  // 1347
        L"",  // 1348
        L"",  // 1349
        L"",  // 1350
        L"",  // 1351
        L"Jump",  // 1352
        L"Jump",  // 1353
        L"Jump",  // 1354
        L"",  // 1355
        L"",  // 1356
        L"",  // 1357
        L"",  // 1358
        L"Jump",  // 1359
    }},
    {86u, {
        L"",  // 1360
        L"",  // 1361
        L"",  // 1362
        L"",  // 1363
        L"",  // 1364
        L"",  // 1365
        L"",  // 1366
        L"",  // 1367
        L"",  // 1368
        L"",  // 1369
        L"",  // 1370
        L"",  // 1371
        L"",  // 1372
        L"",  // 1373
        L"",  // 1374
        L"",  // 1375
    }},
    {87u, {
        L"The child can view websites on the Allow list, websites designed for children, and websites from the general interest category. Adult sites are blocked.",  // 1376
        L"",  // 1377
        L"",  // 1378
        L"",  // 1379
        L"",  // 1380
        L"",  // 1381
        L"",  // 1382
        L"The child can view all websites, but receives a warning when a site contains suspected adult content.",  // 1383
        L"",  // 1384
        L"",  // 1385
        L"",  // 1386
        L"",  // 1387
        L"",  // 1388
        L"",  // 1389
        L"",  // 1390
        L"",  // 1391
    }},
    {88u, {
        L"Jump",  // 1392
        L"",  // 1393
        L"",  // 1394
        L"",  // 1395
        L"",  // 1396
        L"",  // 1397
        L"Manage settings on the Family Safety website",  // 1398
        L"",  // 1399
        L"You can manage this PC's Family Safety settings on the website.",  // 1400
        L"To manage this PC's Family Safety settings locally (only on this PC), <a>remove it from the Family Safety website.</a>",  // 1401
        L"",  // 1402
        L"Remove machine from the website",  // 1403
        L"",  // 1404
        L"Toggle Expando",  // 1405
        L"Show or hide extra information/elements",  // 1406
        L"",  // 1407
    }},
    {89u, {
        L"",  // 1408
        L"No games installed",  // 1409
        L"12",  // 1410
        L" 1",  // 1411
        L" 2",  // 1412
        L" 3",  // 1413
        L" 4",  // 1414
        L" 5",  // 1415
        L" 6",  // 1416
        L" 7",  // 1417
        L" 8",  // 1418
        L" 9",  // 1419
        L"10",  // 1420
        L"11",  // 1421
        L"12",  // 1422
        L" 1",  // 1423
    }},
    {90u, {
        L" 2",  // 1424
        L" 3",  // 1425
        L" 4",  // 1426
        L" 5",  // 1427
        L" 6",  // 1428
        L" 7",  // 1429
        L" 8",  // 1430
        L" 9",  // 1431
        L"10",  // 1432
        L"11",  // 1433
        L"",  // 1434
        L"No name",  // 1435
        L"No website",  // 1436
        L"No name",  // 1437
        L"",  // 1438
        L"",  // 1439
    }},
    {91u, {
        L"",  // 1440
        L"",  // 1441
        L"",  // 1442
        L"",  // 1443
        L"",  // 1444
        L"",  // 1445
        L"",  // 1446
        L"",  // 1447
        L"",  // 1448
        L"Set the number of hours %username% can use the PC a day",  // 1449
        L"Set time allowance",  // 1450
        L"Jump",  // 1451
        L"Set curfew",  // 1452
        L"Jump",  // 1453
        L"Set the time of day %username% can use the PC",  // 1454
        L"Which websites can %username% view?",  // 1455
    }},
    {92u, {
        L"",  // 1456
        L"%username% can use all websites",  // 1457
        L"%username% can only use the websites I allow",  // 1458
        L"",  // 1459
        L"",  // 1460
        L"",  // 1461
        L"",  // 1462
        L"",  // 1463
        L"",  // 1464
        L"%username% can play all games and view all Windows Store apps",  // 1465
        L"%username% can use only the games and the Windows Store apps I allow",  // 1466
        L"",  // 1467
        L"",  // 1468
        L"",  // 1469
        L"",  // 1470
        L"",  // 1471
    }},
    {93u, {
        L"",  // 1472
        L"",  // 1473
        L"",  // 1474
        L"The child can view websites on the Allow List. Adult sites are blocked.",  // 1475
        L"Click here to change Allow List.",  // 1476
        L"Jump",  // 1477
        L"",  // 1478
        L"",  // 1479
        L"Control how long %username% can use the PC",  // 1480
        L"%username% can use the PC without a limit on the number of hours",  // 1481
        L"%username% can only use the PC for the amount of time I allow",  // 1482
        L"",  // 1483
        L"",  // 1484
        L"",  // 1485
        L"",  // 1486
        L"",  // 1487
    }},
    {94u, {
        L"",  // 1488
        L"",  // 1489
        L"",  // 1490
        L"%username% can use the PC all day",  // 1491
        L"%username% can only use the PC during the time ranges I allow",  // 1492
        L"",  // 1493
        L"",  // 1494
        L"",  // 1495
        L"",  // 1496
        L"",  // 1497
        L"",  // 1498
        L"",  // 1499
        L"",  // 1500
        L"",  // 1501
        L"",  // 1502
        L"",  // 1503
    }},
    {95u, {
        L"",  // 1504
        L"",  // 1505
        L"",  // 1506
        L"",  // 1507
        L"",  // 1508
        L"",  // 1509
        L"Weekdays:",  // 1510
        L"More information",  // 1511
        L"",  // 1512
        L"",  // 1513
        L"hours",  // 1514
        L"Web filtering",  // 1515
        L"Jump",  // 1516
        L"minutes",  // 1517
        L"",  // 1518
        L"",  // 1519
    }},
    {96u, {
        L"",  // 1520
        L"",  // 1521
        L"",  // 1522
        L"Weekend:",  // 1523
        L"",  // 1524
        L"",  // 1525
        L"Mon - Fri",  // 1526
        L"Sat - Sun",  // 1527
        L"Allow or block websites by rating and content types",  // 1528
        L"Set web filtering level",  // 1529
        L"Jump",  // 1530
        L"Set web filtering level",  // 1531
        L"Allow or block all websites",  // 1532
        L"Allow or block specific websites",  // 1533
        L"Manage settings on the Family Safety website",  // 1534
        L"You already have some settings on individual games, which might override these changes. To review specific games you've allowed or blocked, see the \"Allow or Block Games\" settings",  // 1535
    }},
    {97u, {
        L"You've chosen Allow list only, but haven't specified any allowed websites. This user won't be able to browse the web at all.",  // 1536
        L"This person can only run apps needed by Windows.",  // 1537
        L"",  // 1538
        L"",  // 1539
        L"",  // 1540
        L"",  // 1541
        L"",  // 1542
        L"Most popular websites",  // 1543
        L"Latest blocked pages",  // 1544
        L"PC time used",  // 1545
        L"Most used apps and games",  // 1546
        L"Add new user",  // 1547
        L"You can turn on additional controls by selecting a provider from this list.",  // 1548
        L"One or more administrator accounts don't have a password. Administrator accounts without passwords allow anyone to bypass or turn off Family Safety. <a>Add passwords to these accounts</a>.",  // 1549
        L"Add passwords",  // 1550
        L"Family Safety Off",  // 1551
    }},
    {98u, {
        L"Activity Reporting Off",  // 1552
        L"",  // 1553
        L"",  // 1554
        L"",  // 1555
        L"",  // 1556
        L"Weekdays Toggle Expando",  // 1557
        L"Show or hide extra information/elements",  // 1558
        L"Weekend Toggle Expando",  // 1559
        L"Show or hide extra information/elements",  // 1560
        L"Weekdays Hours Allowed",  // 1561
        L"Weekdays Minutes Allowed",  // 1562
        L"Weekend Hours Allowed",  // 1563
        L"Weekend Minutes Allowed",  // 1564
        L"Manage settings on the Family Safety website",  // 1565
        L"12 AM",  // 1566
        L"1 AM",  // 1567
    }},
    {99u, {
        L"2 AM",  // 1568
        L"3 AM",  // 1569
        L"4 AM",  // 1570
        L"5 AM",  // 1571
        L"6 AM",  // 1572
        L"7 AM",  // 1573
        L"8 AM",  // 1574
        L"9 AM",  // 1575
        L"10 AM",  // 1576
        L"11 AM",  // 1577
        L"12 PM",  // 1578
        L"1 PM",  // 1579
        L"2 PM",  // 1580
        L"3 PM",  // 1581
        L"4 PM",  // 1582
        L"5 PM",  // 1583
    }},
    {100u, {
        L"6 PM",  // 1584
        L"7 PM",  // 1585
        L"8 PM",  // 1586
        L"9 PM",  // 1587
        L"10 PM",  // 1588
        L"11 PM",  // 1589
        L"",  // 1590
        L"",  // 1591
        L"",  // 1592
        L"",  // 1593
        L"",  // 1594
        L"",  // 1595
        L"",  // 1596
        L"",  // 1597
        L"",  // 1598
        L"Turning on web restrictions also turns on SafeSearch settings for Bing, Google, Yahoo! and other popular search engines. Adult images are also blocked.",  // 1599
    }},
    {101u, {
        L"Set up Family Safety",  // 1600
        L"",  // 1601
        L"",  // 1602
        L"",  // 1603
        L"",  // 1604
        L"",  // 1605
        L"",  // 1606
        L"",  // 1607
        L"",  // 1608
        L"",  // 1609
        L"",  // 1610
        L"",  // 1611
        L"",  // 1612
        L"",  // 1613
        L"",  // 1614
        L"",  // 1615
    }},
};

struct EmbeddedVersionString { const wchar_t* key; const wchar_t* value; };
struct EmbeddedVersionResource {
    uint32_t fixedFileInfo[13];
    const wchar_t* stringTable;
    EmbeddedVersionString strings[9];
    uint16_t language;
    uint16_t codePage;
};

// Original VS_VERSION_INFO fields and strings. A null value preserves the
// zero-length OleSelfRegister entry (distinct from a NUL-terminated empty value).
static const EmbeddedVersionResource kEmbeddedVersionResource = {
    {
        0xFEEF04BDu,              // dwSignature
        0x00010000u,              // dwStrucVersion
        (6u << 16) | 3u,          // dwFileVersionMS: 6.3
        (9600u << 16) | 17135u,    // dwFileVersionLS: 9600.17135
        (6u << 16) | 3u,          // dwProductVersionMS: 6.3
        (9600u << 16) | 17135u,    // dwProductVersionLS: 9600.17135
        0x0000003Fu,              // dwFileFlagsMask
        0x00000000u,              // dwFileFlags
        0x00040004u,              // dwFileOS: VOS_NT_WINDOWS32
        0x00000002u,              // dwFileType: VFT_DLL
        0x00000000u,              // dwFileSubtype
        0x00000000u,              // dwFileDateMS
        0x00000000u,              // dwFileDateLS
    },
    L"040904B0",                  // English (United States), Unicode code page
    {
        {L"CompanyName", L"Microsoft Corporation"},
        {L"FileDescription", L"Parental Controls Control Panel"},
        {L"FileVersion", L"6.3.9600.17135 (winblue_gdr.140525-1850)"},
        {L"InternalName", L"WPCCPL"},
        {L"LegalCopyright", L"© Microsoft Corporation. All rights reserved."},
        {L"OriginalFilename", L"WPCCPL.DLL.MUI"},
        {L"ProductName", L"Microsoft® Windows® Operating System"},
        {L"ProductVersion", L"6.3.9600.17135"},
        {L"OleSelfRegister", nullptr},
    },
    0x0409,                      // Translation language: en-US
    1200,                        // Translation code page: Unicode (0x04B0)
};

static bool BuildEmbeddedStringBlockBytes(const EmbeddedStringBlock& block, Bytes& output) {
    using namespace pepatch::detail;
    output.clear();
    for (const wchar_t* text : block.strings) {
        const size_t length = wcslen(text);
        if (length > 0xFFFF) return false;
        Push16(output, static_cast<uint16_t>(length));
        for (size_t i = 0; i < length; ++i) Push16(output, static_cast<uint16_t>(text[i]));
    }
    return true;
}

static bool BuildEmbeddedVersionResourceBytes(Bytes& output) {
    using namespace pepatch::detail;
    output.clear();
    const EmbeddedVersionResource& resource = kEmbeddedVersionResource;
    // Version blocks start on DWORD boundaries. Their lengths include internal
    // padding, but not the padding before the next sibling block.
    auto beginBlock = [&](const wchar_t* key, uint16_t valueLength, uint16_t type) {
        while (output.size() % 4) output.push_back(0);
        const size_t start = output.size();
        Push16(output, 0);  // wLength, filled by endBlock
        Push16(output, valueLength);
        Push16(output, type);
        PushWStrZ(output, key);
        while (output.size() % 4) output.push_back(0);
        return start;
    };
    auto endBlock = [&](size_t start) {
        const size_t length = output.size() - start;
        if (length > 0xFFFF) return false;
        W16(output, start, static_cast<uint16_t>(length));
        return true;
    };

    const size_t root = beginBlock(L"VS_VERSION_INFO", 52, 0);
    for (uint32_t value : resource.fixedFileInfo) Push32(output, value);
    const size_t stringFileInfo = beginBlock(L"StringFileInfo", 0, 1);
    const size_t stringTable = beginBlock(resource.stringTable, 0, 1);
    for (const EmbeddedVersionString& row : resource.strings) {
        const size_t length = row.value ? wcslen(row.value) + 1 : 0;
        if (length > 0xFFFF) return false;
        const size_t entry = beginBlock(row.key, static_cast<uint16_t>(length), 1);
        if (row.value) PushWStrZ(output, row.value);
        if (!endBlock(entry)) return false;
    }
    if (!endBlock(stringTable) || !endBlock(stringFileInfo)) return false;
    const size_t varFileInfo = beginBlock(L"VarFileInfo", 0, 1);
    const size_t translation = beginBlock(L"Translation", 4, 0);
    Push16(output, resource.language);
    Push16(output, resource.codePage);
    return endBlock(translation) && endBlock(varFileInfo) && endBlock(root);
}

// Generated by tools/extract_translations.py; do not hand-edit.
// 649 string IDs in six languages. Persian ID 961 is the only supplemental translation.
enum class MuiLanguage { EN_US, IT_IT, FA_IR, ES_ES, FR_FR, PT_BR };

struct WpcStringTable {
    uint32_t id;
    const wchar_t* en;
    const wchar_t* it;
    const wchar_t* fa;
    const wchar_t* es;
    const wchar_t* fr;
    const wchar_t* pt;
};

static const WpcStringTable kWpcStrings[] = {
    {100, L"Family Safety", L"Sicurezza famiglia", L"\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647", L"Protecci\u00F3n infantil", L"Contr\u00F4le parental", L"Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {101, L"Change Family Safety settings.", L"Cambia le impostazioni di Sicurezza famiglia.", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0631\u0627 \u062A\u063A\u06CC\u06CC\u0631 \u062F\u0647\u06CC\u062F.", L"Cambiar la configuraci\u00F3n de Protecci\u00F3n infantil.", L"Modifiez les param\u00E8tres de contr\u00F4le parental.", L"Altere as configura\u00E7\u00F5es da Prote\u00E7\u00E3o para a Fam\u00EDlia."},
    {110, L"Password protected", L"Protetto da password", L"\u0645\u062D\u0627\u0641\u0638\u062A \u0634\u062F\u0647 \u0628\u0627 \u0631\u0645\u0632 \u0639\u0628\u0648\u0631", L"Protegida por contrase\u00F1a", L"Prot\u00E9g\u00E9 par mot de passe", L"Protegido por senha"},
    {112, L"Standard user", L"Utente standard", L"\u06A9\u0627\u0631\u0628\u0631 \u0645\u0639\u0645\u0648\u0644\u06CC", L"Usuario est\u00E1ndar", L"Utilisateur standard", L"Usu\u00E1rio padr\u00E3o"},
    {113, L"Unknown account type", L"Tipo di account sconosciuto", L"\u200F\u200F\u0646\u0648\u0639 \u062D\u0633\u0627\u0628 \u0646\u0627\u0634\u0646\u0627\u062E\u062A\u0647", L"Tipo de cuenta desconocido", L"Type de compte inconnu", L"Tipo de conta desconhecido"},
    {114, L"PC administrator", L"Amministratore del PC", L"\u0633\u0631\u067E\u0631\u0633\u062A \u0631\u0627\u06CC\u0627\u0646\u0647", L"Administrador de equipo", L"Administrateur de l\u2019ordinateur", L"Administrador do PC"},
    {115, L"Standard user - Family Safety on", L"Utente standard - Sicurezza famiglia attivata", L"\u06A9\u0627\u0631\u0628\u0631 \u0627\u0633\u062A\u0627\u0646\u062F\u0627\u0631\u062F - \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0641\u0639\u0627\u0644 \u0627\u0633\u062A", L"Usuario est\u00E1ndar: Protecci\u00F3n infantil activada", L"Utilisateur standard - Contr\u00F4le parental activ\u00E9", L"Usu\u00E1rio padr\u00E3o - Prote\u00E7\u00E3o para a Fam\u00EDlia ligada"},
    {116, L"Can't block this website. Web filtering hasn't been configured for %s.", L"Impossibile bloccare questo sito Web. Filtro Web non configurato per %s.", L"\u200F\u200F\u0627\u0645\u06A9\u0627\u0646 \u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u0627\u06CC\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0648\u062C\u0648\u062F \u0646\u062F\u0627\u0631\u062F.  \u0641\u06CC\u0644\u062A\u0631\u06CC\u0646\u06AF \u0648\u0628 \u0628\u0631\u0627\u06CC %s \u067E\u06CC\u06A9\u0631\u0628\u0646\u062F\u06CC \u0646\u0634\u062F\u0647 \u0627\u0633\u062A.", L"No se puede bloquear este sitio web. No se ha configurado el filtrado web para %s.", L"Impossible de bloquer ce site Web. Le filtrage Web n\u2019a pas encore \u00E9t\u00E9 configur\u00E9 pour %s.", L"N\u00E3o \u00E9 poss\u00EDvel bloquear este site. A filtragem de sites n\u00E3o foi configurada para %s."},
    {117, L"Can't allow this website. Web filtering hasn't been configured for %s.", L"Impossibile consentire questo sito Web. Filtro Web non configurato per %s.", L"\u200F\u200F\u0627\u0645\u06A9\u0627\u0646 \u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u0627\u06CC\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0648\u062C\u0648\u062F \u0646\u062F\u0627\u0631\u062F.  \u0641\u06CC\u0644\u062A\u0631\u06CC\u0646\u06AF \u0648\u0628 \u0628\u0631\u0627\u06CC %s \u067E\u06CC\u06A9\u0631\u0628\u0646\u062F\u06CC \u0646\u0634\u062F\u0647 \u0627\u0633\u062A.", L"No se puede permitir este sitio web. No se ha configurado el filtrado web para %s.", L"Impossible d\u2019autoriser ce site Web. Le filtrage Web n\u2019a pas encore \u00E9t\u00E9 configur\u00E9 pour %s.", L"N\u00E3o \u00E9 poss\u00EDvel permitir este site. A filtragem de sites n\u00E3o foi configurada para %s."},
    {118, L"PC administrator - Family Safety on", L"Amministratore PC - Sicurezza famiglia attivata", L"\u0633\u0631\u067E\u0631\u0633\u062A \u0631\u0627\u06CC\u0627\u0646\u0647 - \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0631\u0648\u0634\u0646", L"Administrador de equipo: Protecci\u00F3n infantil activada", L"Administrateur de l\u2019ordinateur - Contr\u00F4le parental activ\u00E9", L"Administrador do computador - Prote\u00E7\u00E3o para a Fam\u00EDlia ligada"},
    {120, L"No password", L"Nessuna password", L"\u200F\u200F\u0628\u062F\u0648\u0646 \u0631\u0645\u0632 \u0639\u0628\u0648\u0631", L"Sin contrase\u00F1a", L"Aucun mot de passe", L"Sem senha"},
    {121, L"Set up how %s will use the PC", L"Imposta le modalit\u00E0 di utilizzo del PC per %s", L"\u0646\u062D\u0648\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 %s \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0631\u0627 \u067E\u06CC\u06A9\u0631\u0628\u0646\u062F\u06CC \u06A9\u0646\u06CC\u062F", L"Configurar la forma en que %s usar\u00E1 el equipo", L"Configuration de l\u2019utilisation de l\u2019ordinateur par %s", L"Configurar como %s usar\u00E1 o PC"},
    {122, L"Control which games and Windows Store apps %s can use", L"Controlla i giochi e le app di Windows Store che possono essere eseguiti da %s", L"\u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %s \u0627\u0632 \u06A9\u062F\u0627\u0645 \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Controlar qu\u00E9 juegos y aplicaciones de la Tienda Windows %s puede usar", L"G\u00E9rer les jeux et les applications du Windows Store que %s peut utiliser", L"Controlar os jogos e os aplicativos da Windows Store que %s pode usar"},
    {123, L"Can %s play games?", L"Indica se %s pu\u00F2 eseguire giochi.", L"\u200F\u200F\u0622\u06CC\u0627 %s \u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u0631\u062F \u0628\u0627\u0632\u06CC \u0647\u0627 \u0631\u0627 \u0627\u062C\u0631\u0627 \u06A9\u0646\u062F\u061F", L"\u00BF%s puede jugar a cualquier juego?", L"Est-ce que %s peut jouer sur cet ordinateur\u00A0?", L"%s pode jogar?"},
    {124, L"Which ratings are OK for %s?", L"Indica le classificazioni accettabili per %s", L"\u200F\u200F\u06A9\u062F\u0627\u0645 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0647\u0627 \u0628\u0631\u0627\u06CC %s \u0645\u0646\u0627\u0633\u0628 \u0647\u0633\u062A\u0646\u062F\u061F", L"\u00BFQu\u00E9 clasificaci\u00F3n es adecuada para %s?", L"Quelles classifications conviennent \u00E0 %s\u00A0?", L"Que classifica\u00E7\u00F5es s\u00E3o adequadas para %s jogar?"},
    {126, L"Choose which apps %s can use", L"Scegli le app che pu\u00F2 utilizzare %s", L"\u200F\u200F\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC\u06CC \u0631\u0627 \u06A9\u0647 \u0645\u06CC \u062E\u0648\u0627\u0647\u06CC\u062F %s \u0627\u062C\u0627\u0632\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u0622\u0646\u0647\u0627 \u0631\u0627 \u062F\u0627\u0631\u0627 \u0628\u0627\u0634\u062F\u060C \u0627\u0646\u062A\u062E\u0627\u0628 \u06A9\u0646\u06CC\u062F", L"Elija las aplicaciones que %s puede usar", L"Choisir les applications que peut utiliser %s", L"Escolha quais aplicativos %s pode usar"},
    {127, L"Family Safety can't monitor an administrator account. To monitor this account with Family Safety, you must change it to a child account.", L"Impossibile monitorare un account amministratore con Sicurezza famiglia. Per monitorare tale account con Sicurezza famiglia, \u00E8 necessario convertirlo in un account per bambini.", L"\u200F\u200F\u0639\u0645\u0644\u06A9\u0631\u062F \u00AB\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647\u00BB \u0642\u0627\u062F\u0631 \u0628\u0647 \u06A9\u0646\u062A\u0631\u0644 \u06CC\u06A9 \u062D\u0633\u0627\u0628 \u0633\u0631\u067E\u0631\u0633\u062A \u0633\u06CC\u0633\u062A\u0645 \u0646\u0645\u06CC \u0628\u0627\u0634\u062F. \u0628\u0631\u0627\u06CC \u06A9\u0646\u062A\u0631\u0644 \u0627\u06CC\u0646 \u062D\u0633\u0627\u0628 \u0628\u0627 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u00AB\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647\u00BB\u060C \u0628\u0627\u06CC\u062F \u0622\u0646 \u0631\u0627 \u0628\u0647 \u06CC\u06A9 \u062D\u0633\u0627\u0628 \u0648\u06CC\u0698\u0647 \u0641\u0631\u0632\u0646\u062F\u0627\u0646 \u062A\u063A\u06CC\u06CC\u0631 \u062F\u0647\u06CC\u062F.", L"Protecci\u00F3n infantil no puede supervisar una cuenta de administrador. Para supervisar esta cuenta con Protecci\u00F3n infantil, debe cambiarla a una cuenta de un menor.", L"Impossible d\u2019appliquer le contr\u00F4le parental \u00E0 un compte administrateur. Pour appliquer le contr\u00F4le parental \u00E0 ce compte, vous devez le convertir en un compte enfant.", L"A Prote\u00E7\u00E3o para a Fam\u00EDlia n\u00E3o pode monitorar uma conta de administrador. Para monitorar esta conta com a Prote\u00E7\u00E3o para a Fam\u00EDlia, voc\u00EA deve alter\u00E1-la para uma conta de crian\u00E7a."},
    {129, L"Up to %s", L"Fino a %s", L"\u062D\u062F\u0627\u06A9\u062B\u0631 \u062A\u0627 %s", L"Hasta la clasificaci\u00F3n %s", L"Jusqu'\u00E0 %s", L"At\u00E9 %s"},
    {130, L"Allowed website", L"Consenti sito Web", L"\u0648\u0628 \u0633\u0627\u06CC\u062A \u0645\u062C\u0627\u0632", L"Sitio web permitido", L"Site Web autoris\u00E9", L"Site permitido"},
    {131, L"Block website", L"Blocca sito Web", L"\u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A", L"Bloquear sitio web", L"Bloquer le site Web", L"Bloquear site"},
    {132, L"Selected website(s) added to the Allowed list:\n%1", L"I siti Web selezionati sono stati aggiunti all'elenco degli indirizzi Web consentiti:\n%1", L"\u0648\u0628 \u0633\u0627\u06CC\u062A \u0627\u0646\u062A\u062E\u0627\u0628\u06CC \u0628\u0647 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632 \u0627\u0636\u0627\u0641\u0647 \u0634\u062F:\n%1", L"Se agregaron los sitios web seleccionados a la lista de permitidos:\n%1", L"Sites Web s\u00E9lectionn\u00E9s ajout\u00E9s \u00E0 la liste des sites autoris\u00E9s\u00A0:\n%1", L"Sites selecionados adicionados \u00E0 lista de Permiss\u00F5es:\n%1"},
    {133, L"Selected website(s) added to the Blocked list:\n%1", L"I siti Web selezionati sono stati aggiunti all'elenco degli indirizzi Web bloccati:\n%1", L"\u0648\u0628 \u0633\u0627\u06CC\u062A \u0627\u0646\u062A\u062E\u0627\u0628\u06CC \u0628\u0647 \u0644\u06CC\u0633\u062A \u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F \u0627\u0636\u0627\u0641\u0647 \u0634\u062F:\n%1", L"Se agregaron los sitios web seleccionados a la lista de bloqueados:\n%1", L"Sites Web s\u00E9lectionn\u00E9s ajout\u00E9s \u00E0 la liste des sites bloqu\u00E9s\u00A0:\n%1", L"Sites selecionados adicionados \u00E0 lista de Bloqueios:\n%1"},
    {134, L"OK", L"OK", L"\u062A\u0623\u06CC\u06CC\u062F", L"Aceptar", L"OK", L"OK"},
    {135, L"Cancel", L"Annulla", L"\u0644\u063A\u0648", L"Cancelar", L"Annuler", L"Cancelar"},
    {136, L"Get help", L"Ulteriori informazioni", L"\u062F\u0631\u06CC\u0627\u0641\u062A \u0631\u0627\u0647\u0646\u0645\u0627\u06CC\u06CC", L"Obtener ayuda", L"Obtenir de l\u2019aide", L"Obter ajuda"},
    {137, L"No rating provided", L"Nessuna classificazione fornita", L"\u200F\u200F\u0647\u06CC\u0686 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0627\u0631\u0627\u0626\u0647 \u0646\u0634\u062F\u0647 \u0627\u0633\u062A", L"No se proporcion\u00F3 ninguna clasificaci\u00F3n", L"Aucune classification n'est fournie", L"Nenhuma classifica\u00E7\u00E3o fornecida"},
    {138, L"You already have some settings on individual games, which might override these changes. To review specific games you've allowed or blocked, see the \"Allow or Block\" settings.", L"Esistono gi\u00E0 impostazioni per singoli giochi, che potrebbero essere prioritarie rispetto a queste modifiche. Per verificare quali giochi sono stati consentiti o bloccati, vedere le impostazioni \"Consenti o blocca\".", L"\u200F\u200F\u0634\u0645\u0627 \u0627\u0632 \u0642\u0628\u0644 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A\u06CC \u0631\u0627 \u062F\u0631 \u0645\u0648\u0631\u062F \u0628\u0631\u062E\u06CC \u0627\u0632 \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648\u0636\u0639 \u0646\u0645\u0648\u062F\u0647 \u0627\u06CC\u062F \u06A9\u0647 \u0645\u0645\u06A9\u0646 \u0627\u0633\u062A \u0627\u06CC\u0646 \u062A\u063A\u06CC\u06CC\u0631\u0627\u062A \u0631\u0627 \u06A9\u0646\u0627\u0631\u06AF\u0630\u0627\u0631\u06CC \u06A9\u0646\u0646\u062F. \u0628\u0631\u0627\u06CC \u0628\u0627\u0632\u0628\u06CC\u0646\u06CC \u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u062E\u0627\u0635\u06CC \u06A9\u0647 \u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0647 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0647 \u0627\u06CC\u062F\u060C \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u00AB\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646\u00BB \u0631\u0627 \u0628\u0628\u06CC\u0646\u06CC\u062F.", L"Ya se configuraron algunos juegos individuales, lo que podr\u00EDa invalidar estos cambios. Para revisar los juegos que se han permitido o bloqueado, vea la configuraci\u00F3n de \"Permitir o bloquear\".", L"Vous avez d\u00E9j\u00E0 d\u00E9fini des param\u00E8tres qui peuvent supplanter ces modifications sur certains jeux. Pour afficher les jeux que vous avez autoris\u00E9s ou bloqu\u00E9s, consultez les param\u00E8tres \u00AB\u00A0Autoriser ou bloquer\u00A0\u00BB.", L"Voc\u00EA j\u00E1 tem algumas configura\u00E7\u00F5es em jogos individuais que podem substituir estas altera\u00E7\u00F5es. Para verificar jogos espec\u00EDficos que voc\u00EA bloqueou ou permitiu, consulte as configura\u00E7\u00F5es de \"Permitir ou Bloquear\"."},
    {139, L"Rated for: {0}+", L"Classificazione per: {0}+", L"\u200F\u200F\u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0634\u062F\u0647 \u0628\u0631\u0627\u06CC: {0}+", L"Clasificado para: {0}+", L"\u00C2ge\u00A0: {0}+", L"Classificado para: {0}+"},
    {142, L"Account picture", L"Immagine account", L"\u062A\u0635\u0648\u06CC\u0631 \u062D\u0633\u0627\u0628", L"Imagen de cuenta", L"Avatar du compte", L"Imagem da conta"},
    {143, L"Picture", L"Immagine", L"\u062A\u0635\u0648\u06CC\u0631", L"Imagen", L"Image", L"Imagem"},
    {144, L"Password", L"Password", L"\u0631\u0645\u0632 \u0639\u0628\u0648\u0631", L"Contrase\u00F1a", L"Mot de passe", L"Senha"},
    {145, L"Name", L"Nome", L"\u0646\u0627\u0645", L"Nombre", L"Nom", L"Nome"},
    {146, L"Type", L"Tipo", L"\u0646\u0648\u0639", L"Tipo", L"Type", L"Tipo"},
    {164, L"You need to be a member of at least one local group to view Family Safety.", L"Per visualizzare Sicurezza famiglia l'utente deve essere membro di almeno un gruppo locale.", L"\u200F\u200F\u0628\u0627\u06CC\u062F \u062D\u062F\u0627\u0642\u0644 \u062F\u0631 \u06CC\u06A9\u06CC \u0627\u0632 \u06AF\u0631\u0648\u0647 \u0647\u0627\u06CC \u0645\u062D\u0644\u06CC \u0639\u0636\u0648 \u0628\u0627\u0634\u06CC\u062F \u062A\u0627 \u0628\u062A\u0648\u0627\u0646\u06CC\u062F \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0631\u0627 \u0645\u0634\u0627\u0647\u062F\u0647 \u06A9\u0646\u06CC\u062F.", L"Debe pertenecer al menos a un grupo local para ver la Protecci\u00F3n infantil.", L"Vous devez \u00EAtre membre d\u2019au moins un groupe local pour afficher le contr\u00F4le parental.", L"\u00C9 necess\u00E1rio ser membro de pelo menos um grupo local para exibir a Prote\u00E7\u00E3o para a Fam\u00EDlia."},
    {165, L"Family Safety is currently unavailable.", L"Sicurezza famiglia non \u00E8 attualmente disponibile.", L"\u200F\u200F\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u062F\u0631 \u062D\u0627\u0644 \u062D\u0627\u0636\u0631 \u062F\u0631 \u062F\u0633\u062A\u0631\u0633 \u0646\u06CC\u0633\u062A.", L"La Protecci\u00F3n infantil no est\u00E1 disponible actualmente.", L"Le contr\u00F4le parental est actuellement indisponible.", L"A Prote\u00E7\u00E3o para a Fam\u00EDlia no momento n\u00E3o est\u00E1 dispon\u00EDvel."},
    {167, L"You don't have permission to open Family Safety. You must be an administrator on this PC to make changes to Family Safety settings.", L"Non si dispone delle autorizzazioni necessarie per aprire Sicurezza famiglia. Per modificare le impostazioni di Sicurezza famiglia, l'utente deve essere un amministratore del PC in uso.", L"\u200F\u200F\u0634\u0645\u0627 \u0627\u062C\u0627\u0632\u0647 \u0628\u0627\u0632 \u06A9\u0631\u062F\u0646 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0631\u0627 \u0646\u062F\u0627\u0631\u06CC\u062F. \u0628\u0631\u0627\u06CC \u0627\u06CC\u062C\u0627\u062F \u062A\u063A\u06CC\u06CC\u0631\u0627\u062A \u062F\u0631 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647\u060C \u0628\u0627\u06CC\u062F \u06CC\u06A9 \u0633\u0631\u067E\u0631\u0633\u062A \u062F\u0631 \u0627\u06CC\u0646 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0628\u0627\u0634\u06CC\u062F.", L"No tiene suficientes permisos para abrir la Protecci\u00F3n infantil. Debe ser un administrador del equipo para realizar cualquier cambio en la configuraci\u00F3n de Protecci\u00F3n infantil.", L"Vous n\u2019\u00EAtes pas autoris\u00E9 \u00E0 ouvrir le contr\u00F4le parental. Vous devez \u00EAtre administrateur de cet ordinateur pour apporter des modifications aux param\u00E8tres du contr\u00F4le parental.", L"Voc\u00EA n\u00E3o tem permiss\u00E3o para abrir a Prote\u00E7\u00E3o para a Fam\u00EDlia. \u00C9 necess\u00E1rio ser administrador deste computador para alterar as configura\u00E7\u00F5es da Prote\u00E7\u00E3o para a Fam\u00EDlia."},
    {174, L"Control when %s uses the PC", L"Controlla quando %s utilizza il PC", L"\u0632\u0645\u0627\u0646 \u0647\u0627\u06CC\u06CC \u06A9\u0647 %s \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0645\u06CC \u06A9\u0646\u062F \u0631\u0627 \u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F", L"Controlar el tiempo que %s usa el equipo", L"Contr\u00F4ler l\u2019utilisation de l\u2019ordinateur par %s", L"Controlar quando %s usa o PC"},
    {175, L"Control the websites %s can access online", L"Determina i siti Web a cui %s pu\u00F2 accedere online", L"\u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %s \u0628\u0647 \u0686\u0647 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC\u06CC \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0628\u0635\u0648\u0631\u062A \u0622\u0646\u0644\u0627\u06CC\u0646 \u062F\u0633\u062A\u0631\u0633\u06CC \u062F\u0627\u0634\u062A\u0647 \u0628\u0627\u0634\u062F", L"Controlar los sitios web a los que %s puede obtener acceso en l\u00EDnea", L"G\u00E9rer les sites Web auxquels %s a le droit d\u2019acc\u00E9der", L"Controlar os sites que %s pode acessar online"},
    {176, L"%s can play all games and view all Windows Store apps", L"%s pu\u00F2 utilizzare tutti i giochi e tutte le app di Windows Store", L"%s \u0645\u06CC \u062A\u0648\u0627\u0646\u06CC\u062F \u062A\u0645\u0627\u0645 \u0628\u0627\u0632\u06CC \u0647\u0627 \u0631\u0627 \u0627\u0646\u062C\u0627\u0645 \u062F\u0627\u062F\u0647 \u0648 \u062A\u0645\u0627\u0645 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB \u0631\u0627 \u0645\u0634\u0627\u0647\u062F\u0647 \u06A9\u0646\u062F", L"%s puede jugar a todos los juegos y ver todas las aplicaciones de la Tienda Windows", L"%s peut jouer \u00E0 tous les jeux et afficher toutes les applications du Windows Store", L"%s pode jogar todos os jogos e exibir todos os aplicativos da Windows Store"},
    {177, L"%s can only use games and Windows Store apps I allow", L"%s pu\u00F2 utilizzare solo i giochi e le app di Windows Store consentiti", L"%s \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u06CC\u062F \u0627\u0632 \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB \u06A9\u0647 \u0645\u0646 \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u0645 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%s solo puede usar los juegos y las aplicaciones de la Tienda Windows que yo permita", L"%s peut utiliser uniquement les jeux et applications du Windows Store que j\u2019autorise", L"%s pode usar apenas os jogos e os aplicativos da Windows Store permitidos"},
    {178, L"Control when %s can use the PC", L"Determina quando %s pu\u00F2 utilizzare il PC", L"\u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %s \u0686\u0647 \u0632\u0645\u0627\u0646\u06CC \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Controlar el tiempo que %s puede usar el equipo", L"G\u00E9rer quand %s peut utiliser l\u2019ordinateur", L"Controlar quando %s pode usar o computador"},
    {179, L"Set the number of hours %s can use the PC per day", L"Imposta il numero di ore al giorno per cui %s pu\u00F2 utilizzare il PC", L"\u062A\u0639\u062F\u0627\u062F \u0633\u0627\u0639\u062A \u0647\u0627\u06CC\u06CC \u06A9\u0647 %s \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062F\u0631 \u0631\u0648\u0632 \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u06CC\u062F", L"Definir el n\u00FAmero de horas al d\u00EDa que %s puede usar el equipo", L"D\u00E9finir le nombre d\u2019heures par jour o\u00F9 %s peut utiliser l\u2019ordinateur", L"Definir o n\u00FAmero de horas em que %s pode usar o computador por dia"},
    {180, L"Set the time of day %s can use the PC", L"Imposta l'ora del giorno in cui %s pu\u00F2 utilizzare il PC", L"\u0632\u0645\u0627\u0646\u06CC \u0627\u0632 \u0631\u0648\u0632 \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %s \u0628\u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Definir la hora del d\u00EDa en que %s puede usar el equipo", L"D\u00E9finir le moment de la journ\u00E9e o\u00F9 %s peut utiliser l\u2019ordinateur", L"Definir a hora do dia em que %s pode usar o computador"},
    {181, L"Control how long %s can use the PC", L"Determina per quanto tempo %s pu\u00F2 utilizzare il PC", L"\u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %s \u062A\u0627 \u0686\u0647 \u0645\u062F\u062A\u06CC \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Controlar durante cu\u00E1nto tiempo %s puede usar el equipo", L"G\u00E9rer la dur\u00E9e pendant laquelle %s peut utiliser l\u2019ordinateur", L"Controlar por quanto tempo %s pode usar o computador"},
    {182, L"%s can use the PC all day", L"%s pu\u00F2 utilizzare il PC per tutto il giorno", L"%s \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062F\u0631 \u062A\u0645\u0627\u0645 \u0637\u0648\u0644 \u0631\u0648\u0632 \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%s puede usar el equipo todo el d\u00EDa", L"%s peut utiliser l\u2019ordinateur \u00E0 tout moment de la journ\u00E9e", L"%s pode usar o computador o dia todo"},
    {183, L"%s can only use the PC for the amount of time I allow", L"%s pu\u00F2 utilizzare il PC solo per il numero di ore consentito", L"%s \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0628\u0631\u0627\u06CC \u0645\u062F\u062A \u0632\u0645\u0627\u0646\u06CC \u06A9\u0647 \u0645\u0646 \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u0645 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%s solo puede usar el equipo durante la cantidad de tiempo que yo permita", L"%s peut utiliser l\u2019ordinateur uniquement pour la dur\u00E9e que j\u2019autorise", L"%s pode usar o computador somente no tempo permitido"},
    {184, L"When can %s use the PC?", L"Specificare quando %s pu\u00F2 utilizzare il PC.", L"%s \u0686\u0647 \u0632\u0645\u0627\u0646\u06CC \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F\u061F", L"\u00BFCu\u00E1ndo puede usar el equipo %s?", L"\u00C0 quel moment %s peut-il/elle utiliser l\u2019ordinateur\u00A0?", L"Quando %s pode usar o computador?"},
    {185, L"%s can use the PC all day", L"%s pu\u00F2 utilizzare il PC per tutto il giorno", L"%s \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062F\u0631 \u062A\u0645\u0627\u0645 \u0637\u0648\u0644 \u0631\u0648\u0632 \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%s puede usar el equipo todo el d\u00EDa", L"%s peut utiliser l\u2019ordinateur \u00E0 tout moment de la journ\u00E9e", L"%s pode usar o computador o dia todo"},
    {186, L"%s can only use the PC during the time ranges I allow", L"%s pu\u00F2 utilizzare il PC solo durante la fascia oraria consentita", L"%s \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u062F\u0631 \u0632\u0645\u0627\u0646 \u0647\u0627\u06CC\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F \u06A9\u0647 \u0645\u0646 \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u0645", L"%s solo puede usar el equipo durante los intervalos de tiempo que yo permita", L"%s peut utiliser l\u2019ordinateur uniquement pendant les horaires que j\u2019autorise", L"%s pode usar o computador somente durante os intervalos de tempo permitidos"},
    {187, L"Set times when %s can't use PC at all", L"Imposta gli orari in cui %s non pu\u00F2 utilizzare il PC", L"\u0632\u0645\u0627\u0646 \u0647\u0627\u06CC\u06CC \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %s \u062F\u0631 \u06A9\u0644 \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0628\u0647 \u0631\u0627\u06CC\u0627\u0646\u0647 \u062F\u0633\u062A\u0631\u0633\u06CC \u062F\u0627\u0634\u062A\u0647 \u0628\u0627\u0634\u062F", L"Definir las horas en que %s no puede usar el equipo", L"D\u00E9finir les heures auxquelles %s ne peut pas utiliser l\u2019ordinateur", L"Definir os hor\u00E1rios em que %s n\u00E3o pode usar o computador"},
    {188, L"Which websites can %s view?", L"Specificare i siti Web che possono essere visualizzati da %s.", L"%s \u0686\u0647 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC\u06CC \u0631\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0628\u0628\u06CC\u0646\u062F\u061F", L"\u00BFQu\u00E9 sitios web puede ver %s?", L"Quels sites Web %s peut-il/elle afficher\u00A0?", L"Quais sites %s pode exibir?"},
    {189, L"%s can use all websites", L"%s pu\u00F2 utilizzare tutti i siti Web", L"%s \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u062A\u0645\u0627\u0645 \u0633\u0627\u06CC\u062A \u0647\u0627 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%s puede usar todos los sitios web", L"%s peut utiliser tous les sites Web", L"%s pode usar todos os sites"},
    {190, L"%s can only use the websites I allow", L"%s pu\u00F2 utilizzare solo i siti Web consentiti", L"%s \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F \u06A9\u0647 \u0645\u0646 \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u0645", L"%s solo puede usar los sitios web que yo permita", L"%s peut utiliser uniquement les sites Web que j\u2019autorise", L"%s pode usar somente os sites permitidos"},
    {191, L"Allow or block specific websites for %s", L"Consenti o blocca siti Web specifici per %s", L"\u0645\u062C\u0627\u0632 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u062E\u0627\u0635 \u0628\u0631\u0627\u06CC %s", L"Permitir o bloquear sitios web espec\u00EDficos para %s", L"Autoriser ou bloquer des sites Web particuliers pour %s", L"Permitir ou bloquear sites espec\u00EDficos para %s"},
    {192, L"Which websites can %s visit?", L"Specificare i siti Web che possono essere visitati da %s.", L"%s \u0627\u0632 \u06A9\u062F\u0627\u0645 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0628\u0627\u0632\u062F\u06CC\u062F \u06A9\u0646\u062F\u061F", L"\u00BFQu\u00E9 sitios web puede visitar %s?", L"Quels sites Web %s peut-il/elle visiter\u00A0?", L"Quais sites %s pode visitar?"},
    {199, L"Which apps can %s use?", L"Specificare le app che possono essere utilizzate da %s.", L"%s \u0627\u0632 \u06A9\u062F\u0627\u0645 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F\u061F", L"\u00BFQu\u00E9 aplicaciones puede usar %s?", L"Quelles applications %s peut-il/elle utiliser\u00A0?", L"Quais aplicativos %s pode usar?"},
    {200, L"%s can use all apps", L"%s pu\u00F2 utilizzare tutte le app", L"%s \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0647\u0645\u0647 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%s puede usar todas las aplicaciones", L"%s peut utiliser toutes les applications", L"%s pode usar todos os aplicativos"},
    {201, L"%s can only use the apps I allow", L"%s pu\u00F2 utilizzare solo le app consentite", L"%s \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F \u06A9\u0647 \u0645\u0646 \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u0645", L"%s solo puede usar las aplicaciones que yo permita", L"%s peut utiliser uniquement les applications que j\u2019autorise", L"%s pode usar somente aplicativos permitidos"},
    {209, L"{0}'s PC activity for %1 from %2 - %3", L"Riepilogo dell'attivit\u00E0 del PC di {0} per %1 nel periodo %2 - %3", L"\u200F\u200F\u0641\u0639\u0627\u0644\u06CC\u062A \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u06CC {0} \u0628\u0631\u0627\u06CC %1 \u0627\u0632 %2 \u062A\u0627 %3", L"Actividad en el equipo de {0} para %1 del %2 al %3", L"Activit\u00E9 de l\u2019ordinateur de {0} pour %1 du %2 au %3", L"{0} atividades do PC para %1 de %2 a %3"},
    {210, L"No games", L"Nessun gioco", L"\u0647\u06CC\u0686 \u0628\u0627\u0632\u06CC \u0627\u062C\u0631\u0627 \u0646\u0634\u0648\u062F", L"Ning\u00FAn juego", L"Aucun jeu", L"Nenhum jogo"},
    {211, L"Other parental control settings", L"Altre impostazioni di Controllo genitori", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u06A9\u0646\u062A\u0631\u0644 \u0647\u0627\u06CC \u0648\u0627\u0644\u062F\u06CC\u0646\u06CC \u062F\u06CC\u06AF\u0631", L"Otras configuraciones de Control parental", L"Autres param\u00E8tres de contr\u00F4le parental", L"Outras configura\u00E7\u00F5es de controles dos pais"},
    {212, L"Family Safety can't monitor this account because it's the only local administrator account. Do you want to create a standard user account and monitor it with Family Safety?", L"Impossibile monitorare questo account con Sicurezza famiglia perch\u00E9 corrisponde all'unico account amministratore locale. Creare un account utente standard e monitorarlo con Sicurezza famiglia?", L"\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u06CC\u0646 \u062D\u0633\u0627\u0628 \u0631\u0627 \u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u062F \u0632\u06CC\u0631\u0627 \u062A\u0646\u0647\u0627 \u062D\u0633\u0627\u0628 \u0633\u0631\u067E\u0631\u0633\u062A \u0645\u062D\u0644\u06CC \u0627\u0633\u062A. \u0622\u06CC\u0627 \u0645\u06CC \u062E\u0648\u0627\u0647\u06CC\u062F \u06CC\u06A9 \u062D\u0633\u0627\u0628 \u06A9\u0627\u0631\u0628\u0631 \u0627\u0633\u062A\u0627\u0646\u062F\u0627\u0631\u062F \u0627\u06CC\u062C\u0627\u062F \u06A9\u0646\u06CC\u062F \u0648 \u0622\u0646 \u0631\u0627 \u0628\u0627 \u06A9\u0645\u06A9 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F\u061F", L"La Protecci\u00F3n infantil no puede supervisar esta cuenta porque es la \u00FAnica cuenta de administrador local. \u00BFDesea crear una cuenta de usuario est\u00E1ndar y supervisarla con la Protecci\u00F3n infantil?", L"Le contr\u00F4le parental ne peut pas \u00EAtre appliqu\u00E9 \u00E0 ce compte car il s\u2019agit du seul compte administrateur local. Voulez-vous cr\u00E9er un compte d\u2019utilisateur standard et lui appliquer le contr\u00F4le parental ?", L"A Prote\u00E7\u00E3o para a Fam\u00EDlia n\u00E3o pode monitorar esta conta porque ela \u00E9 a conta do administrador local. Deseja criar uma conta de usu\u00E1rio padr\u00E3o e monitor\u00E1-la com a Prote\u00E7\u00E3o para a Fam\u00EDlia?"},
    {214, L"%1 can't be added to the list. This app is never allowed to run.", L"Impossibile aggiungere %1 all'elenco. L'esecuzione di questa app non \u00E8 mai consentita.", L"\u200F\u200F\u0627\u0645\u06A9\u0627\u0646 \u0627\u0641\u0632\u0648\u062F\u0646 %1 \u0628\u0647 \u0644\u06CC\u0633\u062A \u0648\u062C\u0648\u062F \u0646\u062F\u0627\u0631\u062F. \u0627\u06CC\u0646 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u06CC\u0686\u06AF\u0627\u0647 \u0645\u062C\u0627\u0632 \u0628\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0646\u0645\u06CC \u0628\u0627\u0634\u062F.", L"No se puede agregar %1 a la lista. La ejecuci\u00F3n de esta aplicaci\u00F3n no est\u00E1 permitida nunca.", L"Impossible d\u2019ajouter %1 \u00E0 la liste. L\u2019ex\u00E9cution de cette application n\u2019est jamais autoris\u00E9e.", L"%1 n\u00E3o pode ser adicionado \u00E0 lista. A execu\u00E7\u00E3o desse aplicativo nunca \u00E9 permitida."},
    {215, L"Family Safety can't monitor Guest accounts. It's a good idea to turn off Guest accounts if you're using Family Safety for other accounts.", L"Impossibile monitorare account Guest con Sicurezza famiglia. Se si utilizza Sicurezza famiglia per altri account, \u00E8 consigliabile disattivare l'account Guest.", L"\u200F\u200F\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062D\u0633\u0627\u0628 \u0647\u0627\u06CC \u0645\u0647\u0645\u0627\u0646 \u0631\u0627 \u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u062F. \u0627\u06AF\u0631 \u0627\u0632 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0628\u0631\u0627\u06CC \u0633\u0627\u06CC\u0631 \u062D\u0633\u0627\u0628 \u0647\u0627 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0646\u0645\u06CC \u06A9\u0646\u06CC\u062F\u060C \u0628\u0647\u062A\u0631 \u0627\u0633\u062A \u06A9\u0647 \u062D\u0633\u0627\u0628 \u0647\u0627\u06CC \u0645\u0647\u0645\u0627\u0646 \u0631\u0627 \u062E\u0627\u0645\u0648\u0634 \u06A9\u0646\u06CC\u062F.", L"La Protecci\u00F3n infantil no puede supervisar las cuentas Invitado. Se recomienda desactivar las cuentas Invitado si est\u00E1 usando la Protecci\u00F3n infantil para las dem\u00E1s cuentas.", L"Impossible d\u2019appliquer le contr\u00F4le parental aux comptes Invit\u00E9. Il est conseill\u00E9 de d\u00E9sactiver ce type de compte si vous appliquez le contr\u00F4le parental \u00E0 d\u2019autres comptes.", L"A Prote\u00E7\u00E3o para a Fam\u00EDlia n\u00E3o pode monitorar as contas de Convidado. \u00C9 recomend\u00E1vel desligar as contas de Convidado se voc\u00EA estiver usando a Prote\u00E7\u00E3o para a Fam\u00EDlia para outras contas."},
    {216, L"Duplicate entry", L"Voce duplicata", L"\u0648\u0631\u0648\u062F\u06CC \u062A\u06A9\u0631\u0627\u0631\u06CC", L"Entrada duplicada", L"Entr\u00E9e dupliqu\u00E9e", L"Entrada duplicada"},
    {217, L"URL is invalid and can't be added.", L"L'URL non \u00E8 valido e non pu\u00F2 essere aggiunto.", L"\u200F\u200FURL \u0645\u0639\u062A\u0628\u0631 \u0646\u06CC\u0633\u062A \u0648 \u0627\u0645\u06A9\u0627\u0646 \u0627\u0641\u0632\u0648\u062F\u0646 \u0622\u0646 \u0648\u062C\u0648\u062F \u0646\u062F\u0627\u0631\u062F.", L"La direcci\u00F3n URL no es v\u00E1lida y no se puede agregar.", L"L\u2019URL n\u2019est pas valide et ne peut pas \u00EAtre ajout\u00E9e.", L"A URL \u00E9 inv\u00E1lida e n\u00E3o pode ser adicionada."},
    {218, L"Current settings for %s", L"Impostazioni correnti per %s", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0641\u0639\u0644\u06CC \u0628\u0631\u0627\u06CC %s", L"Configuraci\u00F3n actual para %s", L"Param\u00E8tres actuels pour %s", L"Configura\u00E7\u00F5es atuais para %s"},
    {219, L"Web Allow Block Lists|*.WebAllowBlockList|All Files|*.*||", L"Elenchi di indirizzi Web consentiti/bloccati|*.WebAllowBlockList|Tutti i file|*.*||", L"\u0644\u06CC\u0633\u062A \u0647\u0627\u06CC \u0645\u062C\u0627\u0632 \u0648 \u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F \u0648\u0628 \u0647\u0627|*.WebAllowBlockList|\u0647\u0645\u0647 \u067E\u0631\u0648\u0646\u062F\u0647 \u0647\u0627|*.*||", L"Listas de p\u00E1ginas web permitidas o bloqueadas|*.WebAllowBlockList|Todos los archivos|*.*||", L"Listes rouge/verte pour le Web|*.WebAllowBlockList|Tous les fichiers|*.*||", L"Listas de Permiss\u00F5es/Bloqueios da Web|*.WebAllowBlockList|Todos os Arquivos|*.*||"},
    {220, L"WebAllowBlockList", L"WebAllowBlockList", L"WebAllowBlockList", L"WebAllowBlockList", L"WebAllowBlockList", L"WebAllowBlockList"},
    {221, L"There was an error trying to export the website Allow/Block list.", L"Errore durante l'esportazione dell'elenco di indirizzi Web consentiti/bloccati.", L"\u200F\u200F\u062E\u0637\u0627\u06CC\u06CC \u062F\u0631 \u062D\u06CC\u0646 \u062A\u0644\u0627\u0634 \u0628\u0631\u0627\u06CC \u0635\u0627\u062F\u0631 \u06A9\u0631\u062F\u0646 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632/\u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F \u0648\u0628 \u0633\u0627\u06CC\u062A \u0648\u062C\u0648\u062F \u062F\u0627\u0634\u062A.", L"Error al intentar exportar la lista de p\u00E1ginas web permitidas o bloqueadas.", L"Erreur lors de l\u2019exportation de la liste rouge/verte de sites Web.", L"Erro ao tentar exportar a lista de Permiss\u00F5es/Bloqueios do site."},
    {222, L"There was an error trying to import the website Allow/Block list.", L"Errore durante l'importazione dell'elenco di indirizzi Web consentiti/bloccati.", L"\u200F\u200F\u062E\u0637\u0627\u06CC\u06CC \u062F\u0631 \u062D\u06CC\u0646 \u062A\u0644\u0627\u0634 \u0628\u0631\u0627\u06CC \u0648\u0627\u0631\u062F \u06A9\u0631\u062F\u0646 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632/\u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F \u0648\u0628 \u0633\u0627\u06CC\u062A \u0648\u062C\u0648\u062F \u062F\u0627\u0634\u062A.", L"Error al intentar importar la lista de p\u00E1ginas web permitidas o bloqueadas.", L"Erreur lors de l\u2019importation de la liste rouge/verte de sites Web.", L"Erro ao tentar importar a lista de Permiss\u00F5es/Bloqueios do site."},
    {223, L"The selected app is already in the list.", L"L'app selezionata \u00E8 gi\u00E0 inclusa nell'elenco.", L"\u200F\u200F\u0628\u0631\u0646\u0627\u0645\u0647 \u0627\u0646\u062A\u062E\u0627\u0628\u06CC \u0627\u0632 \u0642\u0628\u0644 \u062F\u0631 \u0644\u06CC\u0633\u062A \u0627\u0633\u062A.", L"La aplicaci\u00F3n seleccionada ya est\u00E1 en la lista.", L"L\u2019application s\u00E9lectionn\u00E9e est d\u00E9j\u00E0 dans la liste.", L"O aplicativo selecionado j\u00E1 est\u00E1 na lista."},
    {224, L"There was an error and the selected app wasn't added to the list.", L"Si \u00E8 verificato un errore. L'app selezionata non \u00E8 stata aggiunta all'elenco.", L"\u200F\u200F\u062E\u0637\u0627\u06CC\u06CC \u0648\u062C\u0648\u062F \u062F\u0627\u0634\u062A \u0648 \u0628\u0631\u0646\u0627\u0645\u0647 \u0627\u0646\u062A\u062E\u0627\u0628\u06CC \u0628\u0647 \u0644\u06CC\u0633\u062A \u0627\u0636\u0627\u0641\u0647 \u0646\u0634\u062F.", L"Error. No se agreg\u00F3 la aplicaci\u00F3n seleccionada a la lista.", L"Une erreur s\u2019est produite et l\u2019application s\u00E9lectionn\u00E9e n\u2019a pas \u00E9t\u00E9 ajout\u00E9e \u00E0 la liste.", L"Ocorreu um erro e o aplicativo selecionado n\u00E3o foi adicionado \u00E0 lista."},
    {225, L"%1 can't be added to the list. This app is always allowed to run.", L"Impossibile aggiungere %1 all'elenco. L'esecuzione di tale app \u00E8 sempre consentita.", L"\u200F\u200F\u0627\u0645\u06A9\u0627\u0646 \u0627\u0641\u0632\u0648\u062F\u0646 %1 \u0628\u0647 \u0644\u06CC\u0633\u062A \u0648\u062C\u0648\u062F \u0646\u062F\u0627\u0631\u062F. \u0627\u06CC\u0646 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0645\u0648\u0627\u0631\u0647 \u0645\u062C\u0627\u0632 \u0628\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0645\u06CC \u0628\u0627\u0634\u062F.", L"No se puede agregar %1 a la lista. La ejecuci\u00F3n de esta aplicaci\u00F3n est\u00E1 siempre permitida.", L"Impossible d\u2019ajouter\u00A0%1 \u00E0 la liste. L\u2019ex\u00E9cution de cette application est toujours autoris\u00E9e.", L"%1 n\u00E3o pode ser adicionado \u00E0 lista. Esse aplicativo sempre tem permiss\u00E3o para ser executado."},
    {226, L"Unknown user", L"Utente sconosciuto", L"\u200F\u200F\u06A9\u0627\u0631\u0628\u0631 \u0646\u0627\u0634\u0646\u0627\u0633", L"Usuario desconocido", L"Utilisateur inconnu", L"Usu\u00E1rio desconhecido"},
    {227, L"Sorry, we couldn't make changes to Family Safety settings.", L"Impossibile modificare le impostazioni di Sicurezza famiglia.", L"\u200F\u200F\u0645\u062A\u0623\u0633\u0641\u06CC\u0645\u060C \u0646\u062A\u0648\u0627\u0646\u0633\u062A\u06CC\u0645 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0631\u0627 \u062A\u063A\u06CC\u06CC\u0631 \u062F\u0647\u06CC\u0645.", L"No se pudieron efectuar los cambios en la configuraci\u00F3n de Protecci\u00F3n infantil.", L"Impossible de modifier les param\u00E8tres du contr\u00F4le parental.", L"Infelizmente n\u00E3o foi poss\u00EDvel fazer as altera\u00E7\u00F5es nas configura\u00E7\u00F5es da Prote\u00E7\u00E3o para a Fam\u00EDlia."},
    {228, L"An account with that name already exists.\n\nPlease enter a different name.", L"Esiste gi\u00E0 un account con lo stesso nome.\n\nDigitare un nome diverso.", L"\u200F\u200F\u06CC\u06A9 \u062D\u0633\u0627\u0628 \u0628\u0627 \u0627\u06CC\u0646 \u0646\u0627\u0645 \u0627\u0632 \u0642\u0628\u0644 \u0645\u0648\u062C\u0648\u062F \u0627\u0633\u062A.\n\n\u0644\u0637\u0641\u0627\u064B \u0646\u0627\u0645 \u062F\u06CC\u06AF\u0631\u06CC \u0631\u0627 \u0648\u0627\u0631\u062F \u06A9\u0646\u06CC\u062F.", L"Ya existe una cuenta con este nombre.\n\nEscriba un nombre distinto.", L"Un compte du m\u00EAme nom existe d\u00E9j\u00E0.\n\nEntrez un autre nom.", L"J\u00E1 existe uma conta com esse nome.\n\nInsira outro nome."},
    {229, L"Account names can't contain the following characters: /\\[]\":;|<>+=,?*\n\nPlease enter a different name.", L"I nomi di account non possono contenere i caratteri seguenti: /\\[]\":;|<>+=,?*\n\nDigitare un nome diverso.", L"\u200F\u200F\u0646\u0627\u0645 \u0647\u0627\u06CC \u062D\u0633\u0627\u0628 \u0646\u0628\u0627\u06CC\u062F \u062F\u0627\u0631\u0627\u06CC \u0646\u0648\u06CC\u0633\u0647 \u0647\u0627\u06CC \u0632\u06CC\u0631 \u0628\u0627\u0634\u0646\u062F: /\\[]\":;|<>+=,?*\n\n\u0644\u0637\u0641\u0627\u064B \u06CC\u06A9 \u0646\u0627\u0645 \u062F\u06CC\u06AF\u0631 \u0631\u0627 \u0648\u0627\u0631\u062F \u06A9\u0646\u06CC\u062F.", L"Los nombres de cuentas no pueden contener los siguientes caracteres: /\\[]\":;|<>+=,?*\n\nEscriba otro nombre.", L"Les noms de compte ne doivent pas contenir les caract\u00E8res suivants\u00A0: /\\[]\":;|<>+=,?*\n\nEntrez un autre nom.", L"Os nomes de conta n\u00E3o podem conter os seguintes caracteres: /\\[]\":;|<>+=,?*\n\nInsira outro nome."},
    {231, L"Allow website", L"Consenti sito Web", L"\u0648\u0628 \u0633\u0627\u06CC\u062A \u0645\u062C\u0627\u0632 \u0627\u0633\u062A", L"Permitir sitio web", L"Autoriser le site Web", L"Permitir site"},
    {232, L"Block website", L"Blocca sito Web", L"\u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A", L"Bloquear sitio web", L"Bloquer le site Web", L"Bloquear site"},
    {233, L"Allow %s to visit selected website?", L"Consentire a %s di visitare il sito Web selezionato?", L"\u0628\u0647 %s \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u06CC\u062F \u0627\u0632 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0627\u0646\u062A\u062E\u0627\u0628\u06CC \u0628\u0627\u0632\u062F\u06CC\u062F \u06A9\u0646\u062F\u061F", L"\u00BFDesea permitir que %s visite el sitio web seleccionado?", L"Autoriser %s \u00E0 acc\u00E9der au site Web s\u00E9lectionn\u00E9\u00A0?", L"Permitir que %s visite o site selecionado?"},
    {234, L"Allow %s to visit selected websites?", L"Consentire a %s di visitare i siti Web selezionati?", L"\u0628\u0647 %s \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u06CC\u062F \u0627\u0632 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0627\u0646\u062A\u062E\u0627\u0628\u06CC \u0628\u0627\u0632\u062F\u06CC\u062F \u06A9\u0646\u062F\u061F", L"\u00BFDesea permitir que %s visite los sitios web seleccionados?", L"Autoriser %s \u00E0 acc\u00E9der aux sites Web s\u00E9lectionn\u00E9s\u00A0?", L"Permitir que %s visite os sites selecionados?"},
    {235, L"Block %s from visiting selected website?", L"Impedire a %s di visitare il sito Web selezionato?", L"Block %s from visiting selected website?", L"\u00BFDesea bloquear las visitas de %s al sitio web seleccionado?", L"Interdire \u00E0 %s d\u2019acc\u00E9der au site Web s\u00E9lectionn\u00E9\u00A0?", L"Bloquear a visita de %s ao site selecionado?"},
    {236, L"Block %s from visiting selected websites?", L"Impedire a %s di visitare i siti Web selezionati?", L"%s \u0631\u0627 \u0627\u0632 \u0628\u0627\u0632\u062F\u06CC\u062F \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0627\u0646\u062A\u062E\u0627\u0628\u06CC \u0645\u0633\u062F\u0648\u062F \u0645\u06CC \u06A9\u0646\u06CC\u062F\u061F", L"\u00BFDesea bloquear las visitas de %s a los sitios web seleccionados?", L"Interdire \u00E0 %s d\u2019acc\u00E9der aux sites Web s\u00E9lectionn\u00E9s\u00A0?", L"Bloquear a visita de %s aos sites selecionados?"},
    {237, L", including unrated games", L", inclusi i giochi non classificati", L"\u060C \u0627\u0632 \u062C\u0645\u0644\u0647 \u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0646\u0634\u062F\u0647", L", incluidos los juegos sin clasificar", L", y compris les jeux non class\u00E9s", L", inclusive jogos sem classifica\u00E7\u00E3o"},
    {238, L"None", L"Nessuno", L"\u0647\u06CC\u0686\u06A9\u062F\u0627\u0645", L"Ninguno", L"Aucun", L"Nenhum"},
    {239, L"All", L"Tutti", L"\u0647\u0645\u0647", L"Todos", L"Tous", L"Todos"},
    {240, L"Can play", L"Pu\u00F2 giocare", L"\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0628\u0627\u0632\u06CC \u06A9\u0646\u062F", L"Puede jugar", L"Autoris\u00E9", L"Pode jogar"},
    {241, L"Can't play", L"Non pu\u00F2 giocare", L"\u0646\u0645\u06CC \u062A\u0648\u0627\u0646 \u0628\u0627\u0632\u06CC \u06A9\u0631\u062F", L"No puede jugar", L"Interdit", L"N\u00E3o pode jogar"},
    {242, L"Use rating", L"Usa classificazione", L"\u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Usar clasificaci\u00F3n", L"Utiliser la classification", L"Usar classifica\u00E7\u00E3o"},
    {243, L"Always allow", L"Consenti sempre", L"\u0647\u0645\u0648\u0627\u0631\u0647 \u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0647 \u0634\u0648\u062F", L"Permitir siempre", L"Toujours autoriser", L"Permitir sempre"},
    {244, L"Always block", L"Blocca sempre", L"\u0647\u0645\u0648\u0627\u0631\u0647 \u0645\u0633\u062F\u0648\u062F \u0634\u0648\u062F", L"Bloquear siempre", L"Toujours bloquer", L"Bloquear sempre"},
    {246, L"Summary of system event PC activity from %1 - %2", L"Riepilogo dell'attivit\u00E0 del computer per gli eventi di sistema nel periodo %1 - %2", L"\u062E\u0644\u0627\u0635\u0647 \u0641\u0639\u0627\u0644\u06CC\u062A \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u06CC \u0631\u0648\u06CC\u062F\u0627\u062F \u0633\u06CC\u0633\u062A\u0645 \u0627\u0632 %1 - %2", L"Resumen de la actividad de eventos del sistema en el equipo del %1 al %2", L"R\u00E9sum\u00E9 de l\u2019activit\u00E9 de l\u2019ordinateur en \u00E9v\u00E9nements syst\u00E8me du %1 au %2", L"Resumo das atividades de computador de eventos do sistema de %1 - %2"},
    {247, L"Visit the %1 website", L"Visitare il sito Web %1", L"\u0628\u0627\u0632\u062F\u06CC\u062F \u0627\u0632 \u0648\u0628 \u0633\u0627\u06CC\u062A %1", L"Visite el sitio web de %1", L"Consulter le site Web %1", L"Visitar o site %1"},
    {248, L"Rating system row", L"Riga sistema di classificazione", L"\u0627\u06A9\u0646\u0648\u0646 \u0633\u06CC\u0633\u062A\u0645 \u0631\u0627 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u06A9\u0646\u06CC\u062F", L"Fila de sistema de clasificaci\u00F3n", L"Ligne du syst\u00E8me de classification", L"Linha de sistema de classifica\u00E7\u00E3o"},
    {249, L"logo", L"logo", L"\u0622\u0631\u0645", L"logotipo", L"logo", L"logotipo"},
    {251, L"%1 can't be added to this list because it is a game. \n\nUse game and Windows Store restrictions to block it.", L"Impossibile aggiungere %1 all'elenco perch\u00E9 \u00E8 un gioco. \n\nUtilizzare le restrizioni per giochi e Windows Store per bloccarlo.", L"\u200F\u200F%1 \u0631\u0627 \u0646\u0645\u06CC \u062A\u0648\u0627\u0646 \u0628\u0647 \u0627\u06CC\u0646 \u0644\u06CC\u0633\u062A \u0627\u0636\u0627\u0641\u0647 \u06A9\u0631\u062F \u0632\u06CC\u0631\u0627 \u06CC\u06A9 \u0628\u0627\u0632\u06CC \u0627\u0633\u062A. \n\n\u0627\u0632 \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB \u0628\u0631\u0627\u06CC \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0622\u0646 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u06CC\u062F.", L"No se puede agregar %1 a esta lista porque es un juego. \n\nUse Restricciones de juegos y de la Tienda Windows para bloquearlo.", L"Impossible d\u2019ajouter %1 \u00E0 la liste car il s\u2019agit d\u2019un jeu.\n\nUtilisez les restrictions aux jeux et au Windows Store pour le bloquer.", L"%1 n\u00E3o pode ser adicionado \u00E0 lista por ser um jogo. \n\nUse as restri\u00E7\u00F5es da Windows Store para bloque\u00E1-lo."},
    {253, L"New password", L"Nuova password", L"\u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u062C\u062F\u06CC\u062F", L"Nueva contrase\u00F1a", L"Nouveau mot de passe", L"Nova senha"},
    {254, L"Confirm new password", L"Conferma nuova password", L"\u200F\u200F\u062A\u0623\u06CC\u06CC\u062F \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u062C\u062F\u06CC\u062F", L"Confirmar la nueva contrase\u00F1a", L"Confirmer le nouveau mot de passe", L"Confirme a nova senha"},
    {255, L"Enter a password hint", L"Immettere un suggerimento per la password", L"\u06CC\u06A9 \u0631\u0627\u0647\u0646\u0645\u0627\u06CC\u06CC \u0628\u0631\u0627\u06CC \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0648\u0627\u0631\u062F \u06A9\u0646\u06CC\u062F", L"Escriba un indicio de contrase\u00F1a", L"Entrez l\u2019indication de mot de passe", L"Insira uma dica de senha"},
    {256, L"System recovery account password", L"Password per l'account di ripristino del sistema", L"\u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u062D\u0633\u0627\u0628 \u0628\u0627\u0632\u06CC\u0627\u0628\u06CC \u0633\u06CC\u0633\u062A\u0645", L"Contrase\u00F1a de la cuenta de recuperaci\u00F3n del sistema", L"Mot de passe du compte de r\u00E9cup\u00E9ration syst\u00E8me", L"Senha da conta de recupera\u00E7\u00E3o do sistema"},
    {257, L"The System Recovery Account is an administrator account built into Windows Vista to assist if problems arise with your own account. This account could also be used to bypass or turn off Family Safety. You are setting a password to prevent unwanted access to this account.", L"L'account di ripristino del sistema \u00E8 un account amministratore incorporato in Windows Vista utilizzabile in caso di problemi con l'account in uso. \u00C8 inoltre possibile utilizzare questo account per eludere o disattivare Sicurezza famiglia. L'impostazione di una password impedisce l'accesso non autorizzato all'account.", L"\u200F\u200F\u062D\u0633\u0627\u0628 \u0628\u0627\u0632\u06CC\u0627\u0628\u06CC \u0633\u06CC\u0633\u062A\u0645 \u06CC\u06A9 \u062D\u0633\u0627\u0628 \u0633\u0631\u067E\u0631\u0633\u062A \u0627\u0633\u062A \u06A9\u0647 \u062F\u0631 Windows Vista \u0642\u0631\u0627\u0631 \u062F\u0627\u062F\u0647 \u0634\u062F\u0647 \u0627\u0633\u062A \u062A\u0627 \u062F\u0631 \u0635\u0648\u0631\u062A \u0628\u0631\u0648\u0632 \u0645\u0634\u06A9\u0644\u0627\u062A \u062F\u0631 \u0627\u0631\u062A\u0628\u0627\u0637 \u0628\u0627 \u062D\u0633\u0627\u0628 \u0634\u0645\u0627\u060C \u0628\u0647 \u0634\u0645\u0627 \u06A9\u0645\u06A9 \u06A9\u0646\u062F. \u0647\u0645\u0686\u0646\u06CC\u0646 \u0628\u0631\u0627\u06CC \u0631\u062F \u0634\u062F\u0646 \u0627\u0632 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u06CC\u0627 \u062E\u0627\u0645\u0648\u0634 \u06A9\u0631\u062F\u0646 \u0622\u0646 \u0645\u06CC \u062A\u0648\u0627\u0646\u06CC\u062F \u0627\u0632 \u0627\u06CC\u0646 \u062D\u0633\u0627\u0628 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u06CC\u062F. \u0634\u0645\u0627 \u0631\u0645\u0632 \u0639\u0628\u0648\u0631\u06CC \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0631\u062F\u0647 \u0627\u06CC\u062F \u062A\u0627 \u062F\u0633\u062A\u0631\u0633\u06CC \u0646\u0627\u062E\u0648\u0627\u0633\u062A\u0647 \u0628\u0647 \u0627\u06CC\u0646 \u062D\u0633\u0627\u0628 \u0631\u0627 \u0645\u0645\u0646\u0648\u0639 \u06A9\u0646\u062F.", L"La cuenta de recuperaci\u00F3n del sistema es una cuenta de administrador integrada en Windows Vista que le ayuda en caso de que se produzca alg\u00FAn problema con su cuenta propia. Esta cuenta podr\u00EDa tambi\u00E9n usarse para omitir o desactivar la Protecci\u00F3n infantil. Se va a establecer una contrase\u00F1a para impedir el acceso no deseado a esta cuenta.", L"Le compte de r\u00E9cup\u00E9ration du syst\u00E8me est un compte administrateur int\u00E9gr\u00E9 \u00E0 Windows Vista pour vous aider en cas de probl\u00E8me sur votre propre compte. Ce compte est cependant susceptible de permettre de contourner ou de d\u00E9sactiver le contr\u00F4le parental. D\u00E9finissez un mot de passe afin d\u2019emp\u00EAcher tout acc\u00E8s interdit \u00E0 ce compte.", L"A Conta de Recupera\u00E7\u00E3o do Sistema \u00E9 uma conta de administrador integrada ao Windows Vista que ajuda a resolver eventuais problemas na sua conta. Tamb\u00E9m pode ser usada para ignorar ou desligar a Prote\u00E7\u00E3o para a Fam\u00EDlia. Voc\u00EA est\u00E1 definindo uma senha para impedir o acesso n\u00E3o desejado a essa conta."},
    {258, L"Your account password", L"Password dell'account in uso", L"\u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u062D\u0633\u0627\u0628 \u0634\u0645\u0627", L"Contrase\u00F1a de su cuenta", L"Mot de passe de votre compte", L"Senha da conta"},
    {259, L"Your account is an administrator account. Because your account has no password, anyone can use it to bypass or turn off Family Safety. Setting a password will help prevent unwanted access to your account.", L"L'account in uso \u00E8 un account amministratore. Poich\u00E9 per questo account non \u00E8 impostata una password, chiunque pu\u00F2 utilizzarlo per eludere o disattivare Sicurezza famiglia. L'impostazione di una password impedisce l'accesso non autorizzato all'account.", L"\u200F\u200F\u062D\u0633\u0627\u0628 \u0634\u0645\u0627 \u06CC\u06A9 \u062D\u0633\u0627\u0628 \u0633\u0631\u067E\u0631\u0633\u062A \u0633\u06CC\u0633\u062A\u0645 \u0627\u0633\u062A. \u0627\u0632 \u0622\u0646\u062C\u0627\u0626\u06CC\u06A9\u0647 \u062D\u0633\u0627\u0628 \u0634\u0645\u0627 \u0647\u06CC\u0686 \u0631\u0645\u0632 \u0639\u0628\u0648\u0631\u06CC \u0646\u062F\u0627\u0631\u062F\u060C \u0647\u0631 \u06A9\u0633\u06CC \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0622\u0646 \u0628\u0631\u0627\u06CC \u0639\u0628\u0648\u0631 \u0627\u0632 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u06CC\u0627 \u062E\u0627\u0645\u0648\u0634 \u06A9\u0631\u062F\u0646 \u0622\u0646 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F. \u062A\u0646\u0638\u06CC\u0645 \u06CC\u06A9 \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0628\u0647 \u062C\u0644\u0648\u06AF\u06CC\u0631\u06CC \u0627\u0632 \u062F\u0633\u062A\u0631\u0633\u06CC \u0646\u0627\u062E\u0648\u0627\u0633\u062A\u0647 \u0628\u0647 \u062D\u0633\u0627\u0628 \u0634\u0645\u0627 \u06A9\u0645\u06A9 \u062E\u0648\u0627\u0647\u062F \u06A9\u0631\u062F.", L"Su cuenta es una cuenta de administrador. Dado que no tiene contrase\u00F1a, cualquier persona puede usarla para omitir o desactivar la Protecci\u00F3n infantil. Si establece una contrase\u00F1a, ayudar\u00E1 a impedir el acceso no deseado a la cuenta.", L"Votre compte est un compte administrateur. Comme aucun mot de passe ne lui est associ\u00E9, tout utilisateur de l\u2019ordinateur peut contourner ou d\u00E9sactiver le contr\u00F4le parental. En d\u00E9finissant un mot de passe, vous \u00E9vitez tout acc\u00E8s interdit \u00E0 votre compte.", L"Sua conta \u00E9 uma conta de administrador. Como ela n\u00E3o tem senha, qualquer um pode us\u00E1-la para ignorar ou desligar a Prote\u00E7\u00E3o para a Fam\u00EDlia. A defini\u00E7\u00E3o de uma senha ajudar\u00E1 a impedir o acesso n\u00E3o desejado \u00E0 sua conta."},
    {260, L"Your account and the system recovery password", L"Password dell'account in uso e dell'account di ripristino del sistema", L"\u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u062D\u0633\u0627\u0628 \u0648 \u0628\u0627\u0632\u06CC\u0627\u0628\u06CC \u0633\u06CC\u0633\u062A\u0645 \u0634\u0645\u0627", L"Contrase\u00F1a de su cuenta y de la cuenta de recuperaci\u00F3n del sistema", L"Mot de passe de votre compte et de la r\u00E9cup\u00E9ration syst\u00E8me", L"Sua conta e a senha de recupera\u00E7\u00E3o do sistema"},
    {261, L"The System Recovery Account is an administrator account built into Windows Vista to assist if problems arise with your own account. Your account is also an administrator account. Because these accounts have no passwords, anyone can use them to bypass or turn off Family Safety. Setting passwords will help prevent unwanted access to these accounts.", L"L'account di ripristino del sistema \u00E8 un account amministratore incorporato in Windows Vista utilizzabile in caso di problemi con l'account in uso. Anche l'account in uso \u00E8 un account amministratore. Poich\u00E9 per questi account non \u00E8 impostata una password, chiunque pu\u00F2 utilizzarli per eludere o disattivare Sicurezza famiglia. L'impostazione di password impedisce l'accesso non autorizzato agli account.", L"\u200F\u200F\u062D\u0633\u0627\u0628 \u0628\u0627\u0632\u06CC\u0627\u0628\u06CC \u0633\u06CC\u0633\u062A\u0645 \u06CC\u06A9 \u062D\u0633\u0627\u0628 \u0633\u0631\u067E\u0631\u0633\u062A \u0627\u0633\u062A \u06A9\u0647 \u062F\u0631 Windows Vista \u0642\u0631\u0627\u0631 \u062F\u0627\u062F\u0647 \u0634\u062F\u0647 \u0627\u0633\u062A \u062A\u0627 \u062F\u0631 \u0635\u0648\u0631\u062A \u0628\u0631\u0648\u0632 \u0645\u0634\u06A9\u0644\u0627\u062A \u062F\u0631 \u0627\u0631\u062A\u0628\u0627\u0637 \u0628\u0627 \u062D\u0633\u0627\u0628 \u0634\u0645\u0627\u060C \u0628\u0647 \u0634\u0645\u0627 \u06A9\u0645\u06A9 \u06A9\u0646\u062F. \u0647\u0645\u0686\u0646\u06CC\u0646 \u0628\u0631\u0627\u06CC \u0631\u062F \u0634\u062F\u0646 \u0627\u0632 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u06CC\u0627 \u062E\u0627\u0645\u0648\u0634 \u06A9\u0631\u062F\u0646 \u0622\u0646 \u0645\u06CC \u062A\u0648\u0627\u0646\u06CC\u062F \u0627\u0632 \u0627\u06CC\u0646 \u062D\u0633\u0627\u0628 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u06CC\u062F. \u0634\u0645\u0627 \u0631\u0645\u0632 \u0639\u0628\u0648\u0631\u06CC \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0631\u062F\u0647 \u0627\u06CC\u062F \u062A\u0627 \u062F\u0633\u062A\u0631\u0633\u06CC \u0646\u0627\u062E\u0648\u0627\u0633\u062A\u0647 \u0628\u0647 \u0627\u06CC\u0646 \u062D\u0633\u0627\u0628 \u0647\u0627 \u0631\u0627 \u0645\u0645\u0646\u0648\u0639 \u06A9\u0646\u062F.", L"La cuenta de recuperaci\u00F3n del sistema es una cuenta de administrador integrada en Windows Vista que le ayuda en caso de que se produzca alg\u00FAn problema con su cuenta propia. Su cuenta es tambi\u00E9n una cuenta de administrador. Dado que estas cuentas no tienen contrase\u00F1a, cualquier persona puede usarlas para omitir o desactivar la Protecci\u00F3n infantil. Si establece contrase\u00F1as, ayudar\u00E1 a impedir el acceso no deseado a las cuentas.", L"Le compte de r\u00E9cup\u00E9ration du syst\u00E8me est un compte administrateur int\u00E9gr\u00E9 \u00E0 Windows Vista pour vous aider en cas de probl\u00E8me sur votre propre compte. Votre compte est \u00E9galement un compte administrateur. Comme ce type de compte ne pr\u00E9voie aucun mot de passe, il peut \u00EAtre utilis\u00E9 pour contourner ou d\u00E9sactiver le contr\u00F4le parental. En d\u00E9finissant un mot de passe, vous \u00E9vitez tout acc\u00E8s interdit \u00E0 votre compte.", L"A Conta de Recupera\u00E7\u00E3o do Sistema \u00E9 uma conta de administrador integrada ao Windows Vista que ajuda a resolver eventuais problemas na sua conta. A sua conta tamb\u00E9m \u00E9 uma conta de administrador. Como essas contas n\u00E3o t\u00EAm senha, qualquer um pode us\u00E1-las para ignorar ou desligar a Prote\u00E7\u00E3o para a Fam\u00EDlia. A defini\u00E7\u00E3o de senhas ajudar\u00E1 a impedir o acesso n\u00E3o desejado a essas contas."},
    {262, L"One or more administrator accounts don't have a password. Administrator accounts without passwords allow anyone to bypass or turn off Family Safety. Add passwords to these accounts.", L"Uno o pi\u00F9 account amministratore sono privi di password. Utilizzando un account amministratore senza password, chiunque pu\u00F2 eludere o disattivare Sicurezza famiglia. Aggiungere la password agli account.", L"\u200F\u200F\u06CC\u06A9 \u06CC\u0627 \u062A\u0639\u062F\u0627\u062F \u0628\u06CC\u0634\u062A\u0631\u06CC \u0627\u0632 \u062D\u0633\u0627\u0628 \u0647\u0627\u06CC \u0633\u0631\u067E\u0631\u0633\u062A \u062F\u0627\u0631\u0627\u06CC \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0646\u06CC\u0633\u062A\u0646\u062F. \u062D\u0633\u0627\u0628 \u0647\u0627\u06CC \u0633\u0631\u067E\u0631\u0633\u062A \u0628\u062F\u0648\u0646 \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0628\u0647 \u0647\u0631 \u06A9\u0633\u06CC \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u0646\u062F \u062A\u0627 \u0627\u0632 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0639\u0628\u0648\u0631 \u06A9\u0631\u062F\u0647 \u06CC\u0627 \u0622\u0646 \u0631\u0627 \u063A\u06CC\u0631\u0641\u0639\u0627\u0644 \u06A9\u0646\u062F. \u0631\u0645\u0632\u0647\u0627\u06CC \u0639\u0628\u0648\u0631 \u0631\u0627 \u0628\u0647 \u0627\u06CC\u0646 \u062D\u0633\u0627\u0628 \u0647\u0627 \u0627\u0636\u0627\u0641\u0647 \u06A9\u0646\u06CC\u062F.", L"Una o varias cuentas de administrador no tienen contrase\u00F1a. Cualquier usuario podr\u00E1 omitir o desactivar la Protecci\u00F3n infantil mientras las cuentas de administrador carezcan de contrase\u00F1a. Agregue contrase\u00F1as a estas cuentas.", L"Un ou plusieurs comptes administrateur n\u2019ont pas de mot de passe. Les comptes administrateur sans mot de passe permettent \u00E0 n\u2019importe quel utilisateur de contourner ou de d\u00E9sactiver le contr\u00F4le parental. Ajoutez un mot de passe \u00E0 ces comptes.", L"Uma ou mais contas de administrador n\u00E3o t\u00EAm senha. Contas de administrador sem senhas permitem que qualquer pessoa ignore ou desligue a Prote\u00E7\u00E3o para a Fam\u00EDlia. Adicione senhas a essas contas."},
    {265, L"The passwords you entered don't match. Enter the new password in each box.", L"Le password immesse non corrispondono. Immettere la nuova password in entrambe le caselle.", L"\u200F\u200F\u0631\u0645\u0632 \u0639\u0628\u0648\u0631\u06CC \u06A9\u0647 \u0648\u0627\u0631\u062F \u06A9\u0631\u062F\u0647 \u0627\u06CC\u062F \u0645\u0637\u0627\u0628\u0642\u062A \u0646\u062F\u0627\u0631\u062F. \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u062C\u062F\u06CC\u062F \u0631\u0627 \u062F\u0631 \u0647\u0631 \u06A9\u0627\u062F\u0631 \u0648\u0627\u0631\u062F \u06A9\u0646\u06CC\u062F.", L"Las contrase\u00F1as escritas no coinciden. Vuelva a escribir la nueva contrase\u00F1a en cada cuadro.", L"Les mots de passe que vous avez entr\u00E9s ne correspondent pas. Entrez \u00E0 nouveau le nouveau mot de passe dans chaque zone de texte.", L"As senhas inseridas n\u00E3o s\u00E3o correspondentes. Insira a nova senha em cada caixa."},
    {267, L"Windows can't set the password for your account.", L"Impossibile impostare la password per l'account in uso.", L"\u200F\u200FWindows \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0631\u0627 \u0628\u0631\u0627\u06CC \u062D\u0633\u0627\u0628 \u0634\u0645\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u062F.", L"Windows no puede establecer la contrase\u00F1a para su cuenta.", L"Windows ne peut pas d\u00E9finir le mot de passe de votre compte.", L"O Windows n\u00E3o pode definir a senha da sua conta."},
    {268, L"Windows can't set the password for the system recovery account.", L"Impossibile impostare la password per l'account di ripristino del sistema.", L"\u200F\u200FWindows \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0631\u0627 \u0628\u0631\u0627\u06CC \u062D\u0633\u0627\u0628 \u0628\u0627\u0632\u06CC\u0627\u0628\u06CC \u0633\u06CC\u0633\u062A\u0645 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u062F.", L"Windows no puede establecer la contrase\u00F1a para la cuenta de recuperaci\u00F3n del sistema.", L"Windows ne peut pas d\u00E9finir le mot de passe du compte de r\u00E9cup\u00E9ration syst\u00E8me.", L"O Windows n\u00E3o pode definir a senha da conta de recupera\u00E7\u00E3o do sistema."},
    {280, L"Executable File Types|*.exe;*.com;*.cmd;*.bat|All Files (*.*)|*.*||", L"Tipi di file eseguibili|*.exe;*.com;*.cmd;*.bat|Tutti i file (*.*)|*.*||", L"\u067E\u0631\u0648\u0646\u062F\u0647 \u0642\u0627\u0628\u0644 \u0627\u062C\u0631\u0627 Types|*.exe;*.com;*.cmd;*.bat|\u062A\u0645\u0627\u0645 \u067E\u0631\u0648\u0646\u062F\u0647 \u0647\u0627 (*.*)|*.*||", L"Tipos de archivo ejecutables|*.exe;*.com;*.cmd;*.bat|Todos los archivos (*.*)|*.*||", L"Fichiers ex\u00E9cutables|*.exe;*.com;*.cmd;*.bat|Tous les fichiers (*.*)|*.*||", L"Tipos de Arquivos Execut\u00E1veis|*.exe;*.com;*.cmd;*.bat|Todos os Arquivos (*.*)|*.*||"},
    {300, L"Accounts to Monitor", L"Account da monitorare", L"\u062D\u0633\u0627\u0628 \u0647\u0627\u06CC \u0645\u0648\u0631\u062F \u0646\u0638\u0631 \u0628\u0631\u0627\u06CC \u06A9\u0646\u062A\u0631\u0644", L"Cuentas para supervisar", L"Comptes \u00E0 contr\u00F4ler", L"Contas que Ser\u00E3o Monitoradas"},
    {301, L"User Activities", L"Attivit\u00E0 utente", L"\u0641\u0639\u0627\u0644\u06CC\u062A \u0647\u0627\u06CC \u06A9\u0627\u0631\u0628\u0631", L"Actividades de usuario", L"Activit\u00E9s de l\u2019utilisateur", L"Visualizador de Atividades"},
    {302, L"User Settings", L"Impostazioni utente", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u06A9\u0627\u0631\u0628\u0631", L"Configuraci\u00F3n de usuario", L"Param\u00E8tres utilisateurs", L"Configura\u00E7\u00F5es do usu\u00E1rio"},
    {303, L"Game and Windows Store Restrictions", L"Restrizioni per giochi e Windows Store", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB", L"Restricciones de juego y de la Tienda Windows", L"Restrictions aux jeux et au Windows Store", L"Restri\u00E7\u00F5es de Jogo e da Windows Store"},
    {304, L"Rating Level", L"Livello di classificazione", L"\u0633\u0637\u062D \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Nivel de clasificaci\u00F3n", L"Niveau de classification", L"N\u00EDvel de Classifica\u00E7\u00E3o"},
    {305, L"Allow or Block", L"Consenti o blocca", L"\u200F\u200F\u0627\u062C\u0627\u0632\u0647 \u062F\u0647\u06CC\u062F \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0646\u06CC\u062F", L"Permitir o bloquear", L"Autoriser ou bloquer", L"Permitir ou Bloquear"},
    {306, L"Rating Systems", L"Sistema di classificazione", L"\u0633\u06CC\u0633\u062A\u0645 \u0647\u0627\u06CC \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Sistemas de clasificaci\u00F3n", L"Syst\u00E8mes de classification", L"Sistemas de Classifica\u00E7\u00E3o"},
    {307, L"Curfew", L"Orario non consentito", L"\u0632\u0645\u0627\u0646 \u0645\u0645\u0646\u0648\u0639\u0647", L"Horario restringido", L"Horaires non autoris\u00E9s", L"Restri\u00E7\u00E3o de Hor\u00E1rio"},
    {308, L"App Restrictions", L"Restrizioni per app", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0631\u0646\u0627\u0645\u0647", L"Restricciones de aplicaciones", L"Restrictions d\u2019applications", L"Restri\u00E7\u00F5es de Aplicativo"},
    {309, L"Web filter settings", L"Impostazioni filtro Web", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628", L"Configuraci\u00F3n de filtro web", L"Param\u00E8tres de filtrage Web", L"Configura\u00E7\u00F5es do filtro Web"},
    {310, L"Web Restrictions", L"Restrizioni Web", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0648\u0628", L"Restricciones web", L"Restrictions d\u2019acc\u00E8s au Web", L"Restri\u00E7\u00F5es na Web"},
    {311, L"Allow or Block Websites", L"Consenti/Blocca siti Web", L"\u0645\u062C\u0627\u0632 \u06A9\u0631\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0633\u0627\u06CC\u062A \u0647\u0627", L"Permitir o bloquear sitios web", L"Autoriser ou bloquer des sites Web", L"Permitir ou Bloquear Sites"},
    {314, L"User Accounts", L"Account utente", L"\u062D\u0633\u0627\u0628 \u0647\u0627\u06CC \u06A9\u0627\u0631\u0628\u0631\u06CC", L"Cuentas de usuario", L"Comptes d'utilisateur", L"Contas de Usu\u00E1rio"},
    {315, L"Time Limits", L"Restrizioni di orario", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0632\u0645\u0627\u0646\u06CC", L"L\u00EDmites de tiempo", L"Limites de temps", L"Limites de Tempo"},
    {316, L"Time Allowance", L"Tempo consentito", L"\u0645\u062F\u062A \u0632\u0645\u0627\u0646 \u0645\u062C\u0627\u0632 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647", L"Tiempo permitido", L"Dur\u00E9e autoris\u00E9e", L"Tempo Permitido"},
    {317, L"Web Filtering", L"Filtro Web", L"\u0641\u06CC\u0644\u062A\u0631\u06CC\u0646\u06AF \u0648\u0628", L"Filtrado web", L"Filtrage Web", L"Filtragem de Sites"},
    {318, L"User Activities", L"Attivit\u00E0 utente", L"\u0641\u0639\u0627\u0644\u06CC\u062A \u0647\u0627\u06CC \u06A9\u0627\u0631\u0628\u0631", L"Actividades de usuario", L"Activit\u00E9s de l\u2019utilisateur", L"Atividades do Usu\u00E1rio"},
    {319, L"Websites visited", L"Siti Web visitati", L"\u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0628\u0627\u0632\u062F\u06CC\u062F \u0634\u062F\u0647", L"Sitios web visitados", L"Sites Web visit\u00E9s", L"Sites visitados"},
    {320, L"Apps used", L"App utilizzate", L"\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0634\u062F\u0647", L"Aplicaciones usadas", L"Applications utilis\u00E9es", L"Aplicativos usados"},
    {321, L"Games played", L"Giochi utilizzati", L"\u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u0627\u0646\u062C\u0627\u0645 \u0634\u062F\u0647", L"Juegos jugados", L"Parties jou\u00E9es", L"Partidas jogadas"},
    {322, L"File downloads", L"Download di file", L"\u067E\u0631\u0648\u0646\u062F\u0647 \u0647\u0627\u06CC \u062F\u0627\u0646\u0644\u0648\u062F \u0634\u062F\u0647", L"Descargas de archivos", L"T\u00E9l\u00E9chargements de fichiers", L"Downloads de arquivo"},
    {331, L"How do I install additional controls?", L"In che modo \u00E8 possibile installare controlli aggiuntivi?", L"\u0645\u0646 \u0686\u0637\u0648\u0631 \u0645\u06CC \u062A\u0648\u0627\u0646\u0645 \u06A9\u0646\u062A\u0631\u0644 \u0647\u0627\u06CC \u0628\u06CC\u0634\u062A\u0631\u06CC \u0631\u0627 \u0646\u0635\u0628 \u06A9\u0646\u0645\u061F", L"\u00BFC\u00F3mo instalo controles adicionales?", L"Comment installer des contr\u00F4les suppl\u00E9mentaires ?", L"Como fa\u00E7o para instalar controles adicionais?"},
    {332, L"How do I turn on additional controls?", L"In che modo \u00E8 possibile attivare controlli aggiuntivi?", L"\u0645\u0646 \u0686\u0637\u0648\u0631 \u0645\u06CC \u062A\u0648\u0627\u0646\u0645 \u06A9\u0646\u062A\u0631\u0644 \u0647\u0627\u06CC \u0628\u06CC\u0634\u062A\u0631\u06CC \u0631\u0627 \u0631\u0648\u0634\u0646 \u06A9\u0646\u0645\u061F", L"\u00BFC\u00F3mo activo los controles adicionales?", L"Comment activer des contr\u00F4les suppl\u00E9mentaires ?", L"Como fa\u00E7o para ativar controles adicionais?"},
    {333, L"Use Family Safety to get reports of your kids' PC activities, choose what they see online, and set time limits, app restrictions, and more. You can manage these settings on this PC, or on the Family Safety website.", L"Con Sicurezza famiglia \u00E8 possibile ricevere rapporti sulle attivit\u00E0 eseguite dai ragazzi con il PC, scegliere i contenuti che possono visualizzare online, impostare limiti di tempo, restrizioni per app e molto altro ancora. Tali impostazioni possono essere gestite su questo PC o nel sito Web di Sicurezza famiglia.", L"\u0627\u0632 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0628\u0631\u0627\u06CC \u062F\u0631\u06CC\u0627\u0641\u062A \u06AF\u0632\u0627\u0631\u0634 \u0645\u0631\u0628\u0648\u0637 \u0628\u0647 \u0641\u0639\u0627\u0644\u06CC\u062A \u0647\u0627\u06CC \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u06CC \u0641\u0631\u0632\u0646\u062F \u062E\u0648\u062F \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u06CC\u062F\u060C \u0627\u0646\u062A\u062E\u0627\u0628 \u06A9\u0646\u06CC\u062F \u06A9\u0647 \u0622\u0646\u0647\u0627 \u0686\u0647 \u0645\u0648\u0627\u0631\u062F\u06CC \u0631\u0627 \u0628\u0635\u0648\u0631\u062A \u0622\u0646\u0644\u0627\u06CC\u0646 \u0645\u0634\u0627\u0647\u062F\u0647 \u06A9\u0646\u0646\u062F \u0648 \u0647\u0645\u0686\u0646\u06CC\u0646 \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0632\u0645\u0627\u0646\u06CC \u0631\u0627 \u0628\u0631\u0627\u06CC \u0622\u0646\u0647\u0627 \u062A\u0639\u06CC\u06CC\u0646 \u06A9\u0646\u06CC\u062F\u060C \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627 \u0631\u0627 \u0645\u062D\u062F\u0648\u062F \u06A9\u0631\u062F\u0647 \u0648 \u0645\u0648\u0627\u0631\u062F \u0628\u06CC\u0634\u062A\u0631. \u0627\u06CC\u0646 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0631\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u06CC\u062F \u062F\u0631 \u0627\u06CC\u0646 \u0631\u0627\u06CC\u0627\u0646\u0647 \u06CC\u0627 \u062F\u0631 \u0633\u0627\u06CC\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0645\u062F\u06CC\u0631\u06CC\u062A \u06A9\u0646\u06CC\u062F.", L"Use la Protecci\u00F3n infantil para obtener informes de las actividades de sus hijos con el equipo, elija qu\u00E9 pueden ver en l\u00EDnea y establezca l\u00EDmites de tiempo y restricciones en aplicaciones, entre otras opciones. Puede administrar esta configuraci\u00F3n en el equipo o en el sitio web de Protecci\u00F3n infantil.", L"Utilisez le contr\u00F4le parental pour obtenir des rapports concernant l\u2019activit\u00E9 de vos enfants sur l\u2019ordinateur, choisir ce qu\u2019ils peuvent consulter en ligne, d\u00E9finir des limites de temps et des restrictions sur les applications, et bien plus encore. Vous pouvez g\u00E9rer ces param\u00E8tres sur cet ordinateur ou sur le site Web Contr\u00F4le parental.", L"Use a Prote\u00E7\u00E3o para a Fam\u00EDlia para obter relat\u00F3rios das atividades dos seus filhos no computador, escolher o que eles veem online, definir limites de tempo e restri\u00E7\u00F5es de aplicativo e muito mais. Voc\u00EA pode gerenciar essas configura\u00E7\u00F5es neste computador ou no site de Prote\u00E7\u00E3o para a Fam\u00EDlia."},
    {334, L"Use Family Safety to get reports of your kids\u2019 PC activities, choose what they see online, and set time limits, apps restrictions, and more.\n\nTo turn on Family Safety, add a new child\u2019s account, or change an existing account to a child\u2019s account in <a>Accounts</a>.\n\nTo turn off Family Safety for an account, change the account type from Child to Standard in <a>Accounts</a>.",
        L"Con Sicurezza famiglia \u00E8 possibile ricevere rapporti sulle attivit\u00E0 eseguite dai ragazzi con il PC, scegliere i contenuti che possono visualizzare online, impostare limiti di tempo, restrizioni per app e molto altro ancora.\n\nPer attivare Sicurezza famiglia, aggiungi un nuovo account per bambini o converti un account esiste in account per bambini in <a>Account</a>.\n\nPer disattivare Sicurezza famiglia per un account, cambia il tipo di account da Figlio/a a Utente standard in <a>Account</a>.",
        L"\u200F\u200F\u0627\u0632 \u0639\u0645\u0644\u06A9\u0631\u062F \u00AB\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647\u00BB \u0628\u0631\u0627\u06CC \u062F\u0631\u06CC\u0627\u0641\u062A \u06AF\u0632\u0627\u0631\u0634 \u0647\u0627\u06CC \u0641\u0639\u0627\u0644\u06CC\u062A \u0647\u0627\u06CC \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u06CC \u0641\u0631\u0632\u0646\u062F \u062E\u0648\u062F\u060C \u0627\u0646\u062A\u062E\u0627\u0628 \u0686\u06CC\u0632\u0647\u0627\u06CC\u06CC \u06A9\u0647 \u0628\u0647 \u0635\u0648\u0631\u062A \u0622\u0646\u0644\u0627\u06CC\u0646 \u0645\u06CC \u0628\u06CC\u0646\u062F\u060C \u0648 \u062A\u0646\u0638\u06CC\u0645 \u062D\u062F\u0627\u06A9\u062B\u0631 \u0632\u0645\u0627\u0646 \u0647\u0627\u06CC \u0645\u062C\u0627\u0632\u060C \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u060C \u0648 \u063A\u06CC\u0631\u0647\u060C \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u06CC\u062F.\n\n\u0628\u0631\u0627\u06CC \u0641\u0639\u0627\u0644 \u06A9\u0631\u062F\u0646 \u0639\u0645\u0644\u06A9\u0631\u062F \u00AB\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647\u00BB\u060C \u062F\u0631 \u0646\u0627\u062D\u06CC\u0647 <a>\u062D\u0633\u0627\u0628 \u0647\u0627</a>\u060C \u06CC\u06A9 \u00AB\u062D\u0633\u0627\u0628 \u0648\u06CC\u0698\u0647 \u0641\u0631\u0632\u0646\u062F\u0627\u0646\u00BB \u062C\u062F\u06CC\u062F \u0627\u0636\u0627\u0641\u0647 \u06A9\u0646\u06CC\u062F\u060C \u06CC\u0627 \u06CC\u06A9 \u062D\u0633\u0627\u0628 \u0645\u0648\u062C\u0648\u062F \u0631\u0627 \u0628\u0647 \u06CC\u06A9 \u00AB\u062D\u0633\u0627\u0628 \u0648\u06CC\u0698\u0647 \u0641\u0631\u0632\u0646\u062F\u0627\u0646\u00BB \u062A\u063A\u06CC\u06CC\u0631 \u062F\u0647\u06CC\u062F.\n\n\u0628\u0631\u0627\u06CC \u063A\u06CC\u0631\u0641\u0639\u0627\u0644 \u06A9\u0631\u062F\u0646 \u0639\u0645\u0644\u06A9\u0631\u062F \u00AB\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647\u00BB \u0628\u0631\u0627\u06CC \u06CC\u06A9 \u062D\u0633\u0627\u0628\u060C \u062F\u0631 \u0646\u0627\u062D\u06CC\u0647 <a>\u062D\u0633\u0627\u0628 \u0647\u0627</a>\u060C \u0646\u0648\u0639 \u062D\u0633\u0627\u0628 \u0631\u0627 \u0627\u0632 \u06CC\u06A9 \u00AB\u062D\u0633\u0627\u0628 \u0648\u06CC\u0698\u0647 \u0641\u0631\u0632\u0646\u062F\u0627\u0646\u00BB \u0628\u0647 \u06CC\u06A9 \u00AB\u062D\u0633\u0627\u0628 \u0627\u0633\u062A\u0627\u0646\u062F\u0627\u0631\u062F\u00BB \u062A\u063A\u06CC\u06CC\u0631 \u062F\u0647\u06CC\u062F.",
        L"Use Protecci\u00F3n infantil para obtener informes de las actividades de sus hijos con el equipo, elija qu\u00E9 pueden ver en l\u00EDnea y establezca l\u00EDmites de tiempo y restricciones en aplicaciones, entre otras opciones.\n\nPara activar Protecci\u00F3n infantil, agregue la cuenta de un menor nuevo o cambie una cuenta existente por la cuenta de un menor en <a>Cuentas</a>.\n\nPara desactivar Protecci\u00F3n infantil para una cuenta, cambie el tipo de cuenta de Hijo a Est\u00E1ndar en <a>Cuentas</a>.",
        L"Utilisez le contr\u00F4le parental pour obtenir des rapports sur les activit\u00E9s de vos enfants sur l\u2019ordinateur, choisir ce qu\u2019ils peuvent voir en ligne et d\u00E9finir des limites horaires, des restrictions sur les applications, et autres.\n\nPour activer le contr\u00F4le parental, ajoutez un nouveau compte enfant ou convertissez un compte existant en compte enfant dans <a>Comptes</a>.\n\nPour d\u00E9sactiver le contr\u00F4le parental d\u2019un compte, convertissez le compte Enfant en compte Standard dans <a>Comptes</a>.",
        L"Use a Prote\u00E7\u00E3o para a Fam\u00EDlia para obter relat\u00F3rios das atividades de seus filhos no computador, escolher o que eles veem online e definir limites de tempo, restri\u00E7\u00F5es a aplicativos e muito mais.\n\nPara ligar a Prote\u00E7\u00E3o para a Fam\u00EDlia, adicione uma nova conta de uma crian\u00E7a ou altere uma conta existente em <a>Contas </a>.\n\nPara desligar a Prote\u00E7\u00E3o para a Fam\u00EDlia para uma conta, altere o tipo de conta de Crian\u00E7a para Padr\u00E3o em <a> Contas</a>."},
    {335, L"Only web addresses starting with http:// or https:// can be added to the Allowed or Blocked lists.", L"Agli elenchi dei siti Web consentiti o bloccati \u00E8 possibile aggiungere solo indirizzi Web che cominciano con http:// o https://.", L"\u0641\u0642\u0637 \u0622\u062F\u0631\u0633 \u0647\u0627\u06CC \u0648\u0628 \u06A9\u0647 \u0628\u0627 http:// \u06CC\u0627 https:// \u0634\u0631\u0648\u0639 \u0645\u06CC \u0634\u0648\u0646\u062F \u0631\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646 \u0628\u0647 \u0644\u06CC\u0633\u062A \u0645\u0648\u0627\u0631\u062F \u0645\u062C\u0627\u0632 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u0627\u0636\u0627\u0641\u0647 \u06A9\u0631\u062F.", L"Solo se pueden agregar a la lista de permitidas o bloqueadas las direcciones web que empiecen por http:// o https://.", L"Seules les adresses Web commen\u00E7ant par http:// ou https:// peuvent \u00EAtre ajout\u00E9es aux listes de sites Web autoris\u00E9s ou bloqu\u00E9s.", L"Somente os endere\u00E7os da Web que come\u00E7am com http:// ou https:// podem ser adicionados \u00E0s listas de Permiss\u00F5es ou Bloqueios."},
    {400, L"Family Safety", L"Sicurezza famiglia", L"\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647", L"Protecci\u00F3n infantil", L"Contr\u00F4le parental", L"Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {410, L"Unratable content", L"Contenuto non classificabile", L"\u0645\u062D\u062A\u0648\u0627\u06CC \u0628\u062F\u0648\u0646 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Contenido no clasificable", L"Contenu non class\u00E9", L"Conte\u00FAdo n\u00E3o classific\u00E1vel"},
    {411, L"Allow all", L"Consenti tutto", L"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u0647\u0645\u0647", L"Permitir todo", L"Autoriser tout", L"Permitir tudo"},
    {412, L"Allow list only", L"Solo elenco consentiti", L"\u0641\u0642\u0637 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632", L"Solo la lista de permitidos", L"Liste verte uniquement", L"Somente lista de permiss\u00F5es"},
    {413, L"Designed for children", L"Adatto ai bambini", L"\u0637\u0631\u0627\u062D\u06CC \u0634\u062F\u0647 \u0628\u0631\u0627\u06CC \u06A9\u0648\u062F\u06A9\u0627\u0646", L"Dise\u00F1ado para menores", L"Destin\u00E9s aux enfants", L"Criado para crian\u00E7as"},
    {414, L"Custom", L"Personalizzato", L"\u0633\u0641\u0627\u0631\u0634\u06CC", L"Personalizado", L"Personnaliser", L"Personalizado"},
    {415, L"Block by list only", L"Blocca solo in base a elenco", L"\u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0641\u0642\u0637 \u0628\u0631 \u0627\u0633\u0627\u0633 \u0644\u06CC\u0633\u062A", L"Solo la lista de bloqueados", L"Bloquer par liste uniquement", L"Bloquear somente por lista"},
    {416, L"Custom", L"Personalizzato", L"\u0633\u0641\u0627\u0631\u0634\u06CC", L"Personalizado", L"Personnaliser", L"Personalizado"},
    {417, L"Online communication", L"Comunicazione online", L"\u0627\u0631\u062A\u0628\u0627\u0637 \u0622\u0646\u0644\u0627\u06CC\u0646", L"Comunicaci\u00F3n en l\u00EDnea", L"Sites de communication", L"Comunica\u00E7\u00E3o online"},
    {418, L"General interest", L"Interesse generale", L"\u0639\u0644\u0627\u0642\u0647 \u0639\u0645\u0648\u0645\u06CC", L"Inter\u00E9s general", L"Grand public", L"Interesses gerais"},
    {419, L"Warn on adult", L"Avvisa in caso di contenuto per adulti", L"\u0647\u0634\u062F\u0627\u0631 \u0645\u062D\u062A\u0648\u0627\u06CC \u0628\u0632\u0631\u06AF\u0633\u0627\u0644", L"Advertir de contenido para adultos", L"Avertir en cas de contenu pour adultes", L"Aviso de conte\u00FAdo para adulto"},
    {500, L"Web address", L"Indirizzo Web", L"\u0646\u0634\u0627\u0646\u06CC \u0648\u0628", L"Direcci\u00F3n web", L"Adresse Web", L"Endere\u00E7o da Web"},
    {501, L"Action taken", L"Azione intrapresa", L"\u0627\u0642\u062F\u0627\u0645 \u0635\u0648\u0631\u062A\u200C \u06AF\u0631\u0641\u062A\u0647", L"Medida tomada", L"Action entreprise", L"A\u00E7\u00E3o executada"},
    {502, L"Category", L"Categoria", L"\u0631\u062F\u0647", L"Categor\u00EDa", L"Cat\u00E9gorie", L"Categoria"},
    {503, L"Last visited", L"Ultima visita", L"\u200F\u200F\u0622\u062E\u0631\u06CC\u0646 \u0628\u0627\u0632\u062F\u06CC\u062F", L"\u00DAltima visita", L"Derni\u00E8re visite", L"\u00DAltima visita"},
    {504, L"Visits", L"Visite", L"\u062A\u0639\u062F\u0627\u062F \u0628\u0627\u0632\u062F\u06CC\u062F\u0647\u0627", L"Visitas", L"Visites", L"Visitas"},
    {505, L"Name", L"Nome", L"\u0646\u0627\u0645", L"Nombre", L"Nom", L"Nome"},
    {506, L"Times used", L"Numero di utilizzi", L"\u0632\u0645\u0627\u0646 \u0647\u0627\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647", L"Frecuencia de uso", L"Nombre d\u2019utilisations", L"Vezes usadas"},
    {507, L"Last used", L"Ultimo utilizzo", L"\u0622\u062E\u0631\u06CC\u0646 \u0627\u0633\u062A\u0641\u0627\u062F\u0647", L"\u00DAltimo uso", L"Derni\u00E8re utilisation", L"\u00DAltima utiliza\u00E7\u00E3o"},
    {508, L"Duration", L"Durata", L"\u0645\u062F\u062A", L"Duraci\u00F3n", L"Dur\u00E9e", L"Dura\u00E7\u00E3o"},
    {509, L"Name", L"Nome", L"\u0646\u0627\u0645", L"Nombre", L"Nom", L"Nome"},
    {510, L"File name", L"Nome file", L"\u0646\u0627\u0645 \u067E\u0631\u0648\u0646\u062F\u0647", L"Nombre de archivo", L"Nom du fichier", L"Nome de arquivo"},
    {511, L"Local path", L"Percorso locale", L"\u0645\u0633\u06CC\u0631 \u0645\u062D\u0644\u06CC", L"Ruta de acceso local", L"Chemin d\u2019acc\u00E8s local\u00A0", L"Caminho local"},
    {512, L"Download date", L"Data download", L"\u062A\u0627\u0631\u06CC\u062E \u062F\u0627\u0646\u0644\u0648\u062F", L"Fecha de descarga", L"Date du t\u00E9l\u00E9chargement", L"Data de download"},
    {513, L"Source", L"Origine", L"\u0645\u0646\u0628\u0639", L"Origen", L"Source", L"Origem"},
    {514, L"Times played", L"Numero di utilizzi giochi", L"\u062A\u0639\u062F\u0627\u062F \u062F\u0641\u0639\u0627\u062A \u067E\u062E\u0634 \u0634\u062F\u0647", L"Veces que se jug\u00F3", L"Parties jou\u00E9es", L"Vezes jogadas"},
    {515, L"Rating", L"Classificazione", L"\u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Clasificaci\u00F3n", L"Classification", L"Classifica\u00E7\u00E3o"},
    {520, L"{0} visits", L"{0} visite", L"{0} \u0628\u0627\u0632\u062F\u06CC\u062F", L"{0} visitas", L"{0} visites", L"{0} visitas"},
    {521, L"No activity", L"Nessuna attivit\u00E0", L"\u0628\u062F\u0648\u0646 \u0641\u0639\u0627\u0644\u06CC\u062A", L"Sin actividad", L"Aucune activit\u00E9", L"Nenhuma atividade"},
    {522, L"Blocked", L"Bloccato", L"\u0645\u0633\u062F\u0648\u062F \u0634\u062F\u0647", L"Bloqueado", L"Bloqu\u00E9", L"Bloqueado"},
    {523, L"Warned, then allowed", L"Consentito dopo avviso", L"\u0647\u0634\u062F\u0627\u0631 \u062F\u0627\u062F\u0647 \u0634\u0648\u062F \u0648 \u0633\u067E\u0633 \u0645\u062C\u0627\u0632 \u0628\u0627\u0634\u062F", L"Permitido con advertencia previa", L"Averti mais autoris\u00E9", L"Avisada - e permitida"},
    {524, L"Allowed", L"Consentito", L"\u200F\u200F\u0645\u062C\u0627\u0632", L"Permitido", L"Autoris\u00E9", L"Permitido"},
    {525, L"{0}, {1}", L"{0}, {1}", L"{0}\u060C {1}", L"{0}, {1}", L"{0}, {1}", L"{0}, {1}"},
    {530, L"See a more comprehensive activity report for {0} on the <a>Family Safety website</a>.", L"Per un rapporto attivit\u00E0 pi\u00F9 completo per {0} visitare il <a>sito Web di Sicurezza famiglia</a>.", L"\u06CC\u06A9 \u06AF\u0632\u0627\u0631\u0634 \u0641\u0639\u0627\u0644\u06CC\u062A \u062C\u0627\u0645\u0639 \u062A\u0631 \u0628\u0631\u0627\u06CC {0} \u062F\u0631 <a>\u0648\u0628 \u0633\u0627\u06CC\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647</a> \u0628\u0628\u06CC\u0646\u06CC\u062F.", L"Vea un informe de actividades m\u00E1s exhaustivo para {0} en el <a>sitio web de Protecci\u00F3n infantil</a>.", L"Consultez un rapport d\u2019activit\u00E9 plus complet pour {0} sur le <a>site Web Contr\u00F4le parental</a>.", L"Veja um relat\u00F3rio de atividades mais abrangente para {0} no <a>site de Prote\u00E7\u00E3o para a Fam\u00EDlia</a>."},
    {531, L"See more details in this activity report by <a>managing settings on the Family Safety website</a>.", L"Per visualizzare un maggior numero di dettagli nel rapporto attivit\u00E0, <a>modificare le impostazioni nel sito Web di Sicurezza famiglia</a>.", L"\u0628\u0627 <a>\u0645\u062F\u06CC\u0631\u06CC\u062A \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u062F\u0631 \u0633\u0627\u06CC\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647</a>\u060C \u062C\u0632\u0626\u06CC\u0627\u062A \u0628\u06CC\u0634\u062A\u0631\u06CC \u0631\u0627 \u062F\u0631\u0628\u0627\u0631\u0647 \u0627\u06CC\u0646 \u06AF\u0632\u0627\u0631\u0634 \u0641\u0639\u0627\u0644\u06CC\u062A \u0645\u0634\u0627\u0647\u062F\u0647 \u06A9\u0646\u06CC\u062F.", L"Para ver m\u00E1s detalles en este informe de actividades, <a>administre la configuraci\u00F3n en el sitio web de Protecci\u00F3n infantil</a>.", L"Retrouvez de plus amples d\u00E9tails de ce rapport d\u2019activit\u00E9 en <a>g\u00E9rant les param\u00E8tres sur le site Web Contr\u00F4le parental</a>.", L"Veja mais detalhes neste relat\u00F3rio de atividades <a>gerenciando as configura\u00E7\u00F5es no site de Prote\u00E7\u00E3o para a Fam\u00EDlia</a>."},
    {532, L"{0}'s PC activities, {1} - {2}", L"Riepilogo dell'attivit\u00E0 del PC di {0} , {1} - {2}", L"\u0641\u0639\u0627\u0644\u06CC\u062A \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u06CC {0}\u060C \u0627\u0632 {1} \u062A\u0627 {2}", L"Actividad en el equipo de {0}, del {1} al {2}", L"Activit\u00E9 sur l\u2019ordinateur de {0} du {1} au {2}", L"Resumo das Atividades do PC para {0} de {1} - {2}"},
    {535, L"On allow list", L"Nell'elenco consentiti", L"\u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632", L"En la lista de permitidos", L"Sur liste verte", L"Na lista de permiss\u00F5es"},
    {536, L"On block list", L"Nell'elenco bloccati", L"\u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F", L"En la lista de bloqueados", L"Sur liste rouge", L"Na lista de bloqueios"},
    {541, L"App path", L"Percorso app", L"\u0645\u0633\u06CC\u0631 \u0628\u0631\u0646\u0627\u0645\u0647", L"Ruta de acceso de la aplicaci\u00F3n", L"Chemin de l\u2019application", L"Caminho do aplicativo"},
    {550, L"Activity reporting is on", L"Rapporto attivit\u00E0 attivato", L"\u06AF\u0632\u0627\u0631\u0634 \u0641\u0639\u0627\u0644\u06CC\u062A \u0631\u0648\u0634\u0646 \u0627\u0633\u062A", L"El informe de actividades est\u00E1 activado", L"Rapports d\u2019activit\u00E9 activ\u00E9s", L"Sistema de relat\u00F3rio de atividades ligado"},
    {551, L"Activity reporting is off", L"Rapporto attivit\u00E0 disattivato", L"\u06AF\u0632\u0627\u0631\u0634 \u0641\u0639\u0627\u0644\u06CC\u062A \u062E\u0627\u0645\u0648\u0634 \u0627\u0633\u062A", L"El informe de actividades est\u00E1 desactivado", L"Rapports d\u2019activit\u00E9 d\u00E9sactiv\u00E9s", L"Sistema de relat\u00F3rio de atividades desligado"},
    {570, L"Duration", L"Durata", L"\u0645\u062F\u062A \u0632\u0645\u0627\u0646", L"Duraci\u00F3n", L"Dur\u00E9e", L"Dura\u00E7\u00E3o"},
    {571, L"Account name", L"Nome account", L"\u0646\u0627\u0645 \u062D\u0633\u0627\u0628", L"Nombre de cuenta", L"Nom du compte", L"Nome da conta"},
    {599, L"Unknown", L"Sconosciuto", L"\u0646\u0627\u0634\u0646\u0627\u0633", L"Desconocido", L"Inconnu", L"Desconhecido"},
    {616, L"Changed by", L"Autore modifica", L"\u062A\u063A\u06CC\u06CC\u0631 \u06CC\u0627\u0641\u062A\u0647 \u062A\u0648\u0633\u0637", L"Cambiado por", L"Modifi\u00E9 par", L"Alterado por"},
    {617, L"Account changed", L"Account modificato", L"\u062D\u0633\u0627\u0628 \u062A\u063A\u06CC\u06CC\u0631 \u06CC\u0627\u0641\u062A", L"Cuenta cambiada", L"Compte modifi\u00E9", L"Conta alterada"},
    {618, L"Setting affected", L"Impostazione interessata", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0645\u0624\u062B\u0631 \u0648\u0627\u0642\u0639 \u0634\u062F", L"Configuraci\u00F3n afectada", L"Param\u00E8tre affect\u00E9", L"Configura\u00E7\u00E3o afetada"},
    {619, L"New setting", L"Nuova impostazione", L"\u062A\u0646\u0638\u06CC\u0645 \u062C\u062F\u06CC\u062F", L"Nueva configuraci\u00F3n", L"Nouveau param\u00E8tre", L"Nova configura\u00E7\u00E3o"},
    {620, L"Old setting", L"Impostazione precedente", L"\u062A\u0646\u0638\u06CC\u0645 \u0642\u062F\u06CC\u0645\u06CC", L"Configuraci\u00F3n anterior", L"Ancien param\u00E8tre", L"Configura\u00E7\u00E3o antiga"},
    {621, L"New date", L"Nuova data", L"\u062A\u0627\u0631\u06CC\u062E \u062C\u062F\u06CC\u062F", L"Fecha nueva", L"Nouvelle date", L"Nova data"},
    {622, L"New time", L"Nuova ora", L"\u0633\u0627\u0639\u062A \u062C\u062F\u06CC\u062F", L"Hora nueva", L"Nouvelle heure", L"Nova hora"},
    {623, L"Name", L"Nome", L"\u0646\u0627\u0645", L"Nombre", L"Nom", L"Nome"},
    {625, L"Type", L"Tipo", L"\u0646\u0648\u0639", L"Tipo", L"Type", L"Tipo"},
    {626, L"Duration/Blocked", L"Durata/Bloccato", L"\u0645\u062F\u062A \u0632\u0645\u0627\u0646/\u0645\u0633\u062F\u0648\u062F", L"Duraci\u00F3n o bloqueado", L"Dur\u00E9e/bloqu\u00E9", L"Dura\u00E7\u00E3o/Bloqueado"},
    {630, L"%1 sign-in(s) - total time: %2 : %3", L"Accessi: %1, tempo totale: %2, %3", L"%1 \u0648\u0631\u0648\u062F \u0628\u0647 \u0633\u06CC\u0633\u062A\u0645 - \u0632\u0645\u0627\u0646 \u06A9\u0644: %2 : %3", L"%1 inicios de sesi\u00F3n - Tiempo total: %2 : %3", L"%1 connexion(s) - Dur\u00E9e totale\u00A0: %2 : %3", L"%1 entrada(s) - tempo total: %2 : %3"},
    {640, L"Source", L"Origine", L"\u0645\u0646\u0628\u0639", L"Origen", L"Source", L"Origem"},
    {641, L"Player name", L"Nome lettore", L"\u0646\u0627\u0645 \u067E\u062E\u0634 \u06A9\u0646\u0646\u062F\u0647", L"Nombre del reproductor", L"Nom du lecteur", L"Nome do player"},
    {642, L"Player version", L"Versione lettore", L"\u0646\u0633\u062E\u0647 \u067E\u062E\u0634 \u06A9\u0646\u0646\u062F\u0647", L"Versi\u00F3n del reproductor", L"Version du lecteur", L"Vers\u00E3o do player"},
    {643, L"Explicit", L"Esplicito", L"\u0648\u0627\u0636\u062D", L"Expl\u00EDcita", L"Explicite", L"Expl\u00EDcita"},
    {650, L"True", L"VERO", L"\u0631\u0627\u0633\u062A", L"Verdadero", L"Vrai", L"Verdadeiro"},
    {651, L"False", L"FALSO", L"\u0627\u0634\u062A\u0628\u0627\u0647", L"Falso", L"Faux", L"Falso"},
    {675, L"URL path", L"Percorso URL", L"\u0645\u0633\u06CC\u0631 URL", L"Ruta de direcci\u00F3n URL", L"Chemin de l\u2019URL", L"Caminho da URL"},
    {676, L"Category list", L"Elenco categorie", L"\u0644\u06CC\u0633\u062A \u0645\u0642\u0648\u0644\u0647", L"Lista de categor\u00EDas", L"Liste de cat\u00E9gories", L"Lista de categorias"},
    {686, L"Path", L"Percorso", L"\u0645\u0633\u06CC\u0631", L"Ruta de acceso", L"Chemin d'acc\u00E8s", L"Caminho"},
    {688, L"Rating", L"Classificazione", L"\u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Clasificaci\u00F3n", L"Classification", L"Classifica\u00E7\u00E3o"},
    {713, L"Unknown User", L"Utente sconosciuto", L"\u200F\u200F\u06A9\u0627\u0631\u0628\u0631 \u0646\u0627\u0634\u0646\u0627\u0633", L"Usuario desconocido", L"Utilisateur inconnu", L"Usu\u00E1rio Desconhecido"},
    {714, L"On", L"Attivato", L"\u0631\u0648\u0634\u0646", L"Activado", L"Activ\u00E9", L"Ativado"},
    {715, L"Off", L"Disattivato", L"\u062E\u0627\u0645\u0648\u0634", L"Desact.", L"D\u00E9sactiv\u00E9", L"Desativado"},
    {716, L"Unknown password or user name", L"Password o nome utente sconosciuto", L"\u200F\u200F\u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u06CC\u0627 \u0646\u0627\u0645 \u06A9\u0627\u0631\u0628\u0631\u06CC \u0646\u0627\u0645\u0634\u062E\u0635 \u0627\u0633\u062A", L"Nombre de usuario o contrase\u00F1a desconocidos", L"Mot de passe ou nom d\u2019utilisateur inconnu", L"Senha ou nome de usu\u00E1rio desconhecido"},
    {717, L"Outside allowed time", L"Al di fuori dell'orario consentito", L"\u062E\u0627\u0631\u062C \u0627\u0632 \u0632\u0645\u0627\u0646 \u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0647 \u0634\u062F\u0647", L"Fuera del tiempo permitido", L"En dehors des horaires autoris\u00E9s", L"Fora do hor\u00E1rio permitido"},
    {718, L"Account disabled", L"Account disabilitato", L"\u200F\u200F\u062D\u0633\u0627\u0628 \u063A\u06CC\u0631\u0641\u0639\u0627\u0644 \u0627\u0633\u062A", L"Cuenta deshabilitada", L"Compte d\u00E9sactiv\u00E9", L"Conta desabilitada"},
    {719, L"Account expired", L"Account scaduto", L"\u062D\u0633\u0627\u0628 \u0645\u0646\u0642\u0636\u06CC \u0634\u062F", L"Cuenta expirada", L"Compte p\u00E9rim\u00E9", L"Conta expirada"},
    {720, L"Workstation restricted", L"Workstation con restrizioni", L"\u0641\u0636\u0627\u06CC \u06A9\u0627\u0631\u06CC \u0645\u062D\u062F\u0648\u062F \u0627\u0633\u062A", L"Estaci\u00F3n de trabajo restringida", L"Poste de travail limit\u00E9", L"Esta\u00E7\u00E3o de Trabalho restrita"},
    {721, L"Sign-in type restricted", L"Tipo di accesso con restrizioni", L"\u0646\u0648\u0639 \u0648\u0631\u0648\u062F \u0628\u0647 \u0628\u0631\u0646\u0627\u0645\u0647 \u0645\u062D\u062F\u0648\u062F \u0627\u0633\u062A", L"Tipo de inicio de sesi\u00F3n restringido", L"Type de connexion limit\u00E9", L"Tipo de entrada restrita"},
    {722, L"Password expired", L"Password scaduta", L"\u200F\u200F\u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0645\u0646\u0642\u0636\u06CC \u0634\u062F", L"Contrase\u00F1a expirada", L"Mot de passe p\u00E9rim\u00E9", L"Senha expirada"},
    {749, L"Status", L"Stato", L"\u0648\u0636\u0639\u06CC\u062A", L"Estado", L"Statut", L"Status"},
    {750, L"File name", L"Nome file", L"\u0646\u0627\u0645 \u067E\u0631\u0648\u0646\u062F\u0647", L"Nombre de archivo", L"Nom du fichier", L"Nome de arquivo"},
    {751, L"Local path", L"Percorso locale", L"\u0645\u0633\u06CC\u0631 \u0645\u062D\u0644\u06CC", L"Ruta de acceso local", L"Chemin d\u2019acc\u00E8s local\u00A0", L"Caminho local"},
    {752, L"Allowed", L"Consentito", L"\u200F\u200F\u0645\u062C\u0627\u0632 \u0627\u0633\u062A", L"Permitido", L"Autoris\u00E9", L"Permitido"},
    {753, L"Blocked", L"Bloccato", L"\u0645\u0633\u062F\u0648\u062F \u0634\u062F\u0647", L"Bloqueado", L"Bloqu\u00E9", L"Bloqueado"},
    {754, L"%1 files downloaded: %2", L"%1 file scaricati: %2", L"%1 \u067E\u0631\u0648\u0646\u062F\u0647 \u062F\u0627\u0646\u0644\u0648\u062F \u0634\u062F\u0647 \u0627\u0633\u062A: %2", L"%1 archivos descargados: %2", L"%1 fichiers t\u00E9l\u00E9charg\u00E9s\u00A0: %2", L"%1 arquivos baixados: %2"},
    {755, L"none", L"Nessuno", L"\u0647\u06CC\u0686\u06A9\u062F\u0627\u0645", L"ninguno", L"aucun", L"Nenhum"},
    {756, L"%1 - %2 time(s): %3", L"%1 - %2 volte: %3", L"%1 - %2 \u0628\u0627\u0631: %3", L"%1 - %2 veces: %3", L"%1\u00A0- %2 fois\u00A0: %3", L"%1 - %2 vez(es): %3"},
    {757, L"Old date", L"Data precedente", L"\u062A\u0627\u0631\u06CC\u062E \u0642\u062F\u06CC\u0645\u06CC", L"Fecha anterior", L"Ancienne date", L"Data antiga"},
    {758, L"Old time", L"Orario precedente", L"\u0633\u0627\u0639\u062A \u0642\u062F\u06CC\u0645\u06CC", L"Hora anterior", L"Ancienne heure", L"Hora antiga"},
    {759, L"%1 account - made %2 changes to %3 account: %4", L"Account %1 - Eseguite %2 modifiche all'account %3: %4", L"\u062D\u0633\u0627\u0628 %1 - %2 \u062A\u063A\u06CC\u06CC\u0631 \u0631\u0627 \u062F\u0631 \u062D\u0633\u0627\u0628 %3 \u0627\u0639\u0645\u0627\u0644 \u0646\u0645\u0648\u062F\u0647 \u0627\u0633\u062A: %4", L"La cuenta %1 realiz\u00F3 %2 cambios en la cuenta %3: %4", L"Compte %1\u00A0- A effectu\u00E9 %2 modifications sur le compte %3\u00A0: %4", L"A conta %1 efetuou %2 altera\u00E7\u00F5es na conta %3: %4"},
    {760, L"%1 account - %2 %3 account(s): %4", L"Account %1 - %3 modifiche %2 agli account: %4", L"%1 \u062D\u0633\u0627\u0628 - %2 %3 \u062D\u0633\u0627\u0628: %4", L"Cuenta %1 - Cuentas %2 %3: %4", L"Compte %1\u00A0- %2 %3 compte(s)\u00A0: %4", L"Conta %1 - %2 %3 conta(s): %4"},
    {761, L"%1 account - %2 failed sign-in attempt(s) - %3: %4", L"Account %1 - Tentativi di accesso non riusciti: %2, %3, %4", L"%1 \u062D\u0633\u0627\u0628 - %2 \u062A\u0644\u0627\u0634 \u0648\u0631\u0648\u062F \u0628\u0647 \u0633\u06CC\u0633\u062A\u0645 \u0646\u0627\u0645\u0648\u0641\u0642 - %3: %4", L"La cuenta %1 tuvo %2 errores de inicio de sesi\u00F3n - %3: %4", L"Compte %1 - %2 tentative(s) de connexion infructueuse(s) - %3\u00A0: %4", L"Conta %1 - falha em %2 tentativa(s) de conex\u00E3o - %3: %4"},
    {762, L"%1 account - changed time from %2 %3 to %4 %5", L"Account %1 - Orario modificato da %2 %3 a %4 %5", L"\u062D\u0633\u0627\u0628 %1 - \u0632\u0645\u0627\u0646 \u0631\u0627 \u0627\u0632 %2 %3 \u0628\u0647 %4 %5 \u062A\u063A\u06CC\u06CC\u0631 \u062F\u0627\u062F\u0647 \u0627\u0633\u062A", L"La cuenta %1 cambi\u00F3 la hora de %2 %3 a %4 %5", L"Compte %1\u00A0- A modifi\u00E9 l\u2019heure, de %2 %3 \u00E0 %4 %5", L"A conta %1 alterou a hora de %2 %3 para %4 %5"},
    {763, L"Limited users cannot access activity reports.", L"Gli utenti con restrizioni non possono accedere ai rapporti attivit\u00E0.", L"\u200F\u200F\u06A9\u0627\u0631\u0628\u0631\u0627\u0646 \u0645\u062D\u062F\u0648\u062F \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u0646\u062F \u0628\u0647 \u06AF\u0632\u0627\u0631\u0634 \u0647\u0627\u06CC \u0641\u0639\u0627\u0644\u06CC\u062A \u062F\u0633\u062A\u0631\u0633\u06CC \u062F\u0627\u0634\u062A\u0647 \u0628\u0627\u0634\u0646\u062F.", L"Los usuarios limitados no tienen acceso a los informes de actividades.", L"Les utilisateurs standard ne peuvent pas acc\u00E9der aux rapports d\u2019activit\u00E9.", L"Usu\u00E1rios com acesso limitado n\u00E3o podem acessar relat\u00F3rios de atividades."},
    {764, L"Created", L"Data creazione", L"\u0627\u06CC\u062C\u0627\u062F \u0634\u062F\u0647", L"Creado", L"Cr\u00E9\u00E9 le", L"Criado em"},
    {765, L"Deleted", L"Eliminato", L"\u062D\u0630\u0641 \u0634\u062F\u0647", L"Eliminado", L"Supprim\u00E9", L"Exclu\u00EDdo"},
    {768, L"Add", L"Aggiungi", L"\u0627\u0641\u0632\u0648\u062F\u0646", L"Agregar", L"Ajouter", L"Adicionar"},
    {769, L"Remove", L"Rimuovi", L"\u062D\u0630\u0641", L"Quitar", L"Supprimer", L"Remover"},
    {770, L"Still signed in", L"Ancora connesso", L"\u0647\u0645\u0686\u0646\u0627\u0646 \u062F\u0631 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0633\u062A\u06CC\u062F", L"Sesi\u00F3n a\u00FAn abierta", L"Encore connect\u00E9", L"Ainda conectado"},
    {771, L"System crash or power down", L"Arresto anomalo o spegnimento del sistema", L"\u0633\u06CC\u0633\u062A\u0645 \u062E\u0631\u0627\u0628 \u0634\u062F\u0647 \u06CC\u0627 \u0628\u0631\u0642 \u0642\u0637\u0639 \u0634\u062F\u0647 \u0627\u0633\u062A", L"Bloqueo o apagado del sistema", L"Panne ou mise hors tension du syst\u00E8me", L"Falha de sistema ou falta de energia"},
    {772, L"Elevated", L"Innalzato di livello", L"\u06AF\u0632\u0627\u0631\u0634 \u0634\u062F\u0647", L"Elevados", L"\u00C9lev\u00E9", L"Elevado"},
    {773, L"Demoted", L"Abbassato di livello", L"\u062A\u0646\u0632\u0644 \u0631\u062A\u0628\u0647", L"Reducidos", L"R\u00E9trograd\u00E9", L"Rebaixado"},
    {774, L"Less than one second", L"Meno di un secondo", L"\u06A9\u0645\u062A\u0631 \u0627\u0632 \u06CC\u06A9 \u062B\u0627\u0646\u06CC\u0647", L"Menos de un segundo", L"Moins d'une seconde", L"Menos de um segundo"},
    {775, L"%dh %dm %ds", L"%dh %dm %ds", L"%d\u0633\u0627\u0639\u062A %d\u062F\u0642\u06CC\u0642\u0647 %d\u062B\u0627\u0646\u06CC\u0647", L"%dh %dm %ds", L"%dh %dm %ds", L"%dh %dm %ds"},
    {776, L"%dm %ds", L"%dm %ds", L"%d\u062F\u0642\u06CC\u0642\u0647 %d\u062B\u0627\u0646\u06CC\u0647", L"%dm %ds", L"%dm %ds", L"%dm %ds"},
    {777, L"%ds", L"%ds", L"%ds", L"%ds", L"%ds", L"%ds"},
    {809, L"Title", L"Titolo", L"\u0639\u0646\u0648\u0627\u0646", L"T\u00EDtulo", L"Titre", L"T\u00EDtulo"},
    {810, L"Album name", L"Nome album", L"\u0646\u0627\u0645 \u0622\u0644\u0628\u0648\u0645", L"Nombre del \u00E1lbum", L"Nom de l\u2019album", L"Nome do \u00C1lbum"},
    {813, L"Contact name", L"Nome contatto", L"\u0646\u0627\u0645 \u0645\u062E\u0627\u0637\u0628", L"Nombre de contacto", L"Nom du contact", L"Nome do contato"},
    {814, L"Contact address", L"Indirizzo contatto", L"\u0622\u062F\u0631\u0633 \u0645\u062E\u0627\u0637\u0628", L"Direcci\u00F3n de contacto", L"Adresse du contact", L"Endere\u00E7o de contato"},
    {815, L"Change type", L"Modifica tipo", L"\u062A\u063A\u06CC\u06CC\u0631 \u0646\u0648\u0639", L"Cambiar tipo", L"Type de modification", L"Alterar tipo"},
    {816, L"Host name", L"Nome host", L"\u0646\u0627\u0645 \u0645\u06CC\u0632\u0628\u0627\u0646", L"Nombre de host", L"Nom de l\u2019h\u00F4te", L"Nome do host"},
    {817, L"Participants", L"Partecipanti", L"\u0634\u0631\u06A9\u062A \u06A9\u0646\u0646\u062F\u06AF\u0627\u0646", L"Participantes", L"Participants", L"Participantes"},
    {818, L"Host IP", L"IP host", L"IP \u0645\u06CC\u0632\u0628\u0627\u0646", L"Direcci\u00F3n IP del host", L"Adresse IP de l'h\u00F4te", L"IP do host"},
    {819, L"Sender", L"Mittente", L"\u0641\u0631\u0633\u062A\u0646\u062F\u0647", L"Remitente", L"Exp\u00E9diteur", L"Remetente"},
    {820, L"Recipients", L"Destinatari", L"\u06AF\u06CC\u0631\u0646\u062F\u06AF\u0627\u0646", L"Destinatarios", L"Destinataires", L"Destinat\u00E1rios"},
    {821, L"Game name", L"Nome gioco", L"\u0646\u0627\u0645 \u0628\u0627\u0632\u06CC", L"Nombre del juego", L"Nom du jeu", L"Nome do jogo"},
    {823, L"Message", L"Messaggio", L"\u067E\u06CC\u0627\u0645", L"Mensaje", L"Message", L"Mensagem"},
    {824, L"Game play", L"Gioco", L"\u0627\u0646\u062C\u0627\u0645 \u0628\u0627\u0632\u06CC", L"Juego", L"Utilisation des jeux", L"Partida"},
    {825, L"File exchange", L"Scambio file", L"\u062A\u0639\u0648\u06CC\u0636 \u067E\u0631\u0648\u0646\u062F\u0647", L"Intercambio de archivos", L"\u00C9change de fichier", L"Troca de arquivos"},
    {826, L"Link exchange", L"Scambio collegamenti", L"\u062A\u0639\u0648\u06CC\u0636 \u067E\u06CC\u0648\u0646\u062F", L"Intercambio de v\u00EDnculos", L"\u00C9change de lien", L"Troca de links"},
    {827, L"SMS message", L"Messaggio SMS", L"\u067E\u06CC\u0627\u0645 SMS", L"Mensaje SMS", L"Message SMS", L"Mensagem SMS"},
    {828, L"%1 - %2 media played", L"%1 - %2 file multimediali riprodotti", L"%1 - %2 \u0631\u0633\u0627\u0646\u0647 \u067E\u062E\u0634 \u0634\u062F\u0647 \u0627\u0633\u062A", L"%1 - %2 archivos multimedia reproducidos", L"%1\u00A0- %2 \u00E9l\u00E9ments multim\u00E9dias lus", L"%1 - %2 m\u00EDdia(s) executada(s)"},
    {829, L"%1 - %2 time(s)", L"%1 - %2 volte", L"%1 - %2 \u0628\u0627\u0631", L"%1 - %2 veces", L"%1\u00A0- %2 fois", L"%1 - %2 vez(es)"},
    {830, L"%s", L"%s", L"%s", L"%s", L"%s", L"%s"},
    {831, L"%1 from %2", L"%1 da %2", L"%1 \u0627\u0632 %2", L"%1 de %2", L"%1 en provenance de %2", L"%1 de %2"},
    {832, L"%1 sent to %2", L"%1 inviato a %2", L"%1 \u0627\u0631\u0633\u0627\u0644 \u0634\u062F\u0647 \u0628\u0647 %2", L"%1 se envi\u00F3 a %2", L"%1 envoy\u00E9 \u00E0 %2", L"%1 enviado para %2"},
    {833, L"Error parsing log files", L"Errore durante l'analisi dei file di registro", L"\u200F\u200F\u062E\u0637\u0627 \u062F\u0631 \u062A\u062C\u0632\u06CC\u0647 \u067E\u0631\u0648\u0646\u062F\u0647 \u0647\u0627\u06CC \u0648\u0627\u0642\u0639\u0647 \u0646\u06AF\u0627\u0631", L"Error al analizar archivos de registro", L"Erreur lors de l\u2019analyse des fichiers journaux", L"Erro ao analisar arquivos de log"},
    {834, L"%1 - %2 conversations - total time: %3 : %4", L"%1 - %2 conversazioni - Tempo totale: %3, %4", L"%1 - %2 \u062A\u0628\u062F\u06CC\u0644 - \u0645\u062F\u062A \u0632\u0645\u0627\u0646 \u06A9\u0644: %3 : %4", L"%1 - %2 conversaciones; tiempo total: %3 : %4", L"%1\u00A0- %2 conversations. Dur\u00E9e totale\u00A0: %3\u00A0: %4", L"%1 - %2 conversas - tempo total: %3 : %4"},
    {835, L"%1 include %2 - total time: %3", L"%1 include %2 - Tempo totale: %3", L"%1 \u0634\u0627\u0645\u0644 %2 - \u0645\u062F\u062A \u0632\u0645\u0627\u0646 \u06A9\u0644: %3", L"%1 incluye %2; tiempo total: %3", L"%1 avec %2. Dur\u00E9e totale\u00A0: %3", L"%1 inclui(em) %2 - tempo total: %3"},
    {836, L"%1 - %2 link(s): %3", L"%1 - %2 collegamento/i: %3", L"%1 - %2 \u067E\u06CC\u0648\u0646\u062F: %3", L"%1 - %2 v\u00EDnculos: %3", L"%1\u00A0- %2 lien(s)\u00A0: %3", L"%1 - %2 link(s): %3"},
    {837, L"%1 - %2 link(s)", L"%1 - %2 collegamenti", L"%1 - %2 \u067E\u06CC\u0648\u0646\u062F", L"%1 - %2 v\u00EDnculos", L"%1\u00A0- %2 lien(s)", L"%1 - %2 link(s)"},
    {838, L"%1 - total games played: %2: %3", L"%1 - Totale partite giocate: %2: %3", L"%1 - \u062A\u0639\u062F\u0627\u062F \u06A9\u0644 \u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u0627\u0646\u062C\u0627\u0645 \u0634\u062F\u0647: %2: %3", L"%1 - Total de juegos jugados: %2: %3", L"%1\u00A0- Total de parties jou\u00E9es\u00A0: %2\u00A0: %3", L"%1 - total de partidas jogadas: %2: %3"},
    {839, L"%1 - game(s) played: %2", L"%1 - Partite giocate: %2", L"%1 - \u0628\u0627\u0632\u06CC \u0627\u0646\u062C\u0627\u0645 \u0634\u062F\u0647: %2", L"%1 - Juegos jugados: %2", L"%1\u00A0- Jeu(x) jou\u00E9(s)\u00A0: %2", L"%1 - partida(s) jogada(s): %2"},
    {840, L"%1 - %2 file(s): %3", L"%1 - %2 file: %3", L"%1 - %2 \u067E\u0631\u0648\u0646\u062F\u0647: %3", L"%1 - %2 archivos: %3", L"%1\u00A0- %2 fichier(s)\u00A0: %3", L"%1 - %2 arquivo(s): %3"},
    {841, L"%1 - %2 file(s)", L"%1 - %2 file", L"%1 - %2 \u067E\u0631\u0648\u0646\u062F\u0647", L"%1 - %2 archivos", L"%1\u00A0- %2 fichier(s)", L"%1 - %2 arquivo(s)"},
    {842, L"%1 - %2 message(s): %3", L"%1 - %2 messaggi/o: %3", L"%1 - %2 \u067E\u06CC\u0627\u0645: %3", L"%1 - %2 mensajes: %3", L"%1 - %2 message(s) : %3", L"%1 - %2 mensagem(ns): %3"},
    {843, L"%1 - %2 message(s)", L"%1 - %2 messaggi", L"%1 - %2 \u067E\u06CC\u0627\u0645", L"%1 - %2 mensajes", L"%1 - %2 message(s)", L"%1 - %2 mensagem(ns)"},
    {844, L"%1 - sent video - %2 time(s), received video - %3 times", L"%1 - Video inviato %2 volta/e, video ricevuto %3 volta/e", L"%1 - \u0648\u06CC\u062F\u06CC\u0648\u06CC \u0627\u0631\u0633\u0627\u0644 \u0634\u062F\u0647 - %2 \u0628\u0627\u0631\u060C \u0648\u06CC\u062F\u06CC\u0648\u06CC \u062F\u0631\u06CC\u0627\u0641\u062A \u0634\u062F\u0647 - %3 \u0628\u0627\u0631", L"%1 envi\u00F3 v\u00EDdeo %2 veces y recibi\u00F3 v\u00EDdeo %3 veces", L"%1 - Envoi de vid\u00E9o - %2 fois, r\u00E9ception de vid\u00E9o - %3 fois", L"%1 - v\u00EDdeo enviado - %2 vez(es), v\u00EDdeo recebido - %3 vezes"},
    {845, L"Sent video to", L"Video inviato a", L"\u0648\u06CC\u062F\u06CC\u0648 \u0627\u0631\u0633\u0627\u0644 \u0634\u062F \u0628\u0647", L"V\u00EDdeo enviado a", L"Vid\u00E9o envoy\u00E9e \u00E0", L"Enviou v\u00EDdeo para"},
    {846, L"Received video from", L"Video ricevuto da", L"\u0648\u06CC\u062F\u06CC\u0648 \u062F\u0631\u06CC\u0627\u0641\u062A \u0634\u062F \u0627\u0632", L"V\u00EDdeo recibido de", L"Vid\u00E9o re\u00E7ue de", L"Recebeu v\u00EDdeo de"},
    {847, L"%1 - sent audio - %2 time(s), received audio - %3 times", L"%1 - Audio inviato %2 volta/e, audio ricevuto %3 volta/e", L"%1 - \u0635\u062F\u0627\u06CC \u0627\u0631\u0633\u0627\u0644 \u0634\u062F\u0647 - %2 \u0628\u0627\u0631\u060C \u0635\u062F\u0627\u06CC \u062F\u0631\u06CC\u0627\u0641\u062A \u0634\u062F\u0647 - %3 \u0628\u0627\u0631", L"%1 envi\u00F3 audio %2 veces y recibi\u00F3 audio %3 veces", L"%1 - Envoi de fichier audio - %2 fois, r\u00E9ception de fichier audio - %3 fois", L"%1 - \u00E1udio enviado - %2 vez(es), \u00E1udio recebido - %3 vezes"},
    {848, L"Sent audio to", L"Audio inviato a", L"\u0635\u062F\u0627 \u0627\u0631\u0633\u0627\u0644 \u0634\u062F \u0628\u0647", L"Audio enviado a", L"Fichier audio envoy\u00E9 \u00E0", L"Enviou \u00E1udio para"},
    {849, L"Received audio from", L"Audio ricevuto da", L"\u0635\u062F\u0627 \u062F\u0631\u06CC\u0627\u0641\u062A \u0634\u062F \u0627\u0632", L"Audio recibido de", L"Fichier audio re\u00E7u de", L"Recebeu \u00E1udio de"},
    {850, L"Modify", L"Modifica", L"\u0627\u0635\u0644\u0627\u062D", L"Modificar", L"Modifier", L"Modificar"},
    {860, L"TV", L"TV", L"\u062A\u0644\u0648\u06CC\u0632\u06CC\u0648\u0646", L"TV", L"TV", L"TV"},
    {861, L"DVD", L"DVD", L"DVD", L"DVD", L"DVD", L"DVD"},
    {862, L"Other", L"Altro", L"\u0633\u0627\u06CC\u0631 \u0645\u0648\u0627\u0631\u062F", L"Otros", L"Autre", L"Outro"},
    {863, L"Audio", L"Audio", L"\u0635\u0648\u062A\u06CC", L"Audio", L"Audio", L"\u00C1udio"},
    {864, L"CD audio", L"CD audio", L"\u0633\u06CC \u062F\u06CC \u0635\u0648\u062A\u06CC", L"CDs de audio", L"CD audio", L"CD de \u00E1udio"},
    {865, L"Video ", L"Video ", L"\u0648\u06CC\u062F\u06CC\u0648", L"V\u00EDdeo ", L"Vid\u00E9o ", L"V\u00EDdeo "},
    {866, L"Picture", L"Immagine", L"\u062A\u0635\u0648\u06CC\u0631", L"Imagen", L"Image", L"Imagem"},
    {871, L"Still running", L"Ancora in esecuzione", L"\u062F\u0631 \u062D\u0627\u0644 \u0627\u062C\u0631\u0627", L"A\u00FAn en ejecuci\u00F3n", L"Encore en cours d\u2019ex\u00E9cution", L"Ainda em execu\u00E7\u00E3o"},
    {873, L"The %s defines these ratings.", L"Classificazioni definite da %s.", L"%s \u0627\u06CC\u0646 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0647\u0627 \u0631\u0627 \u062A\u0639\u06CC\u06CC\u0646 \u0645\u06CC \u06A9\u0646\u062F.", L"El %s define esta clasificaci\u00F3n.", L"La norme %s d\u00E9finit ces classifications.", L"%s define as classifica\u00E7\u00F5es."},
    {874, L"Rating", L"Classificazione", L"\u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Clasificaci\u00F3n", L"Classification", L"Classifica\u00E7\u00E3o"},
    {875, L"Descriptor row", L"Riga descrittore", L"\u0631\u062F\u06CC\u0641 \u062A\u0648\u0635\u06CC\u0641 \u06AF\u0631", L"Fila de descriptor", L"Ligne de descripteur", L"Linha de descritor"},
    {876, L"Rating icon", L"Icona classificazione", L"\u0646\u0645\u0627\u062F \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Icono de clasificaci\u00F3n", L"Ic\u00F4ne de classification", L"\u00CDcone de classifica\u00E7\u00E3o"},
    {880, L"Allow list only", L"Solo elenco consentiti", L"\u0641\u0642\u0637 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632", L"Solo la lista de permitidos", L"Liste verte uniquement", L"Somente lista de permiss\u00F5es"},
    {881, L"Kids only", L"Solo ragazzi", L"\u0641\u0642\u0637 \u06A9\u0648\u062F\u06A9\u0627\u0646", L"Solo ni\u00F1os", L"Enfants uniquement", L"Crian\u00E7as somente"},
    {882, L"Medium", L"Medio", L"\u0645\u062A\u0648\u0633\u0637", L"Media", L"Moyen", L"M\u00E9dio"},
    {883, L"Low", L"Basso", L"\u06A9\u0645", L"Baja", L"Faible", L"Baixo"},
    {884, L"Custom", L"Personalizza", L"\u0633\u0641\u0627\u0631\u0634\u06CC", L"Personalizada", L"Personnalis\u00E9", L"Personalizado"},
    {885, L"No restriction", L"Nessuna restrizione", L"\u0628\u062F\u0648\u0646 \u0645\u062D\u062F\u0648\u062F\u06CC\u062A", L"Sin restricci\u00F3n", L"Aucune restriction", L"Sem restri\u00E7\u00E3o"},
    {886, L"Allow list only", L"Solo elenco consentiti", L"\u0641\u0642\u0637 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632", L"Solo la lista de permitidos", L"Liste verte uniquement", L"Somente lista de permiss\u00F5es"},
    {887, L"Designed for children", L"Adatto ai bambini", L"\u0628\u0631\u0627\u06CC \u06A9\u0648\u062F\u06A9\u0627\u0646 \u0637\u0631\u0627\u062D\u06CC \u0634\u062F\u0647 \u0627\u0633\u062A", L"Dise\u00F1ado para menores", L"Destin\u00E9s aux enfants", L"Criado para crian\u00E7as"},
    {888, L"General interest", L"Interesse generale", L"\u0639\u0644\u0627\u0642\u0647 \u0639\u0645\u0648\u0645\u06CC", L"Inter\u00E9s general", L"Grand public", L"Interesses gerais"},
    {889, L"Online communication", L"Comunicazione online", L"\u0627\u0631\u062A\u0628\u0627\u0637 \u0622\u0646\u0644\u0627\u06CC\u0646", L"Comunicaci\u00F3n en l\u00EDnea", L"Sites de communication", L"Comunica\u00E7\u00E3o online"},
    {890, L"Warn on adult", L"Avvisa in caso di contenuto per adulti", L"\u0647\u0634\u062F\u0627\u0631 \u0645\u062D\u062A\u0648\u0627\u06CC \u0628\u0632\u0631\u06AF\u0633\u0627\u0644", L"Advertir de contenido para adultos", L"Avertir en cas de contenu pour adultes", L"Aviso de conte\u00FAdo para adulto"},
    {891, L"Webpage|*.html||", L"Pagina Web|*.html||", L"\u0635\u0641\u062D\u0647 \u0648\u0628|*.html||", L"P\u00E1gina web|*.html||", L"Page Web|*.html||", L"P\u00E1gina da Web|*.html||"},
    {892, L"HTML", L"HTML", L"HTML", L"HTML", L"HTML", L"HTML"},
    {893, L"Failed to export the data to HTML.", L"Impossibile esportare i dati in HTML.", L"\u062F\u0627\u062F\u0647 \u0647\u0627 \u0628\u0647 HTML \u0635\u0627\u062F\u0631 \u0646\u0634\u062F\u0646\u062F.", L"No se pueden exportar los datos en formato HTML.", L"\u00C9chec de l\u2019exportation des donn\u00E9es en html.", L"Falha ao exportar os dados para HTML."},
    {894, L"Product name", L"Nome prodotto", L"\u0646\u0627\u0645 \u0645\u062D\u0635\u0648\u0644", L"Nombre producto", L"Nom du produit", L"Nome do produto"},
    {895, L"Folder", L"Cartella", L"\u067E\u0648\u0634\u0647", L"Carpeta", L"Dossier", L"Pasta"},
    {896, L"Description", L"Descrizione", L"\u0634\u0631\u062D", L"Descripci\u00F3n", L"Description", L"Descri\u00E7\u00E3o"},
    {897, L"File", L"File", L"\u067E\u0631\u0648\u0646\u062F\u0647", L"Archivo", L"Fichier", L"Arquivo"},
    {900, L"040904B0", L"040904B0", L"040904B0", L"040904B0", L"040904B0", L"040904B0"},
    {901, L"Manage settings on the Family Safety website", L"Gestisci impostazioni nel sito Web di Sicurezza famiglia", L"\u0645\u062F\u06CC\u0631\u06CC\u062A \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0645\u0648\u062C\u0648\u062F \u062F\u0631 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647", L"Administrar configuraci\u00F3n en el sitio web de Protecci\u00F3n infantil", L"G\u00E9rer les param\u00E8tres sur le site Web Contr\u00F4le parental", L"Gerenciar configura\u00E7\u00F5es no site Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {902, L"Sorry, the service isn't available right now. Please try again later.", L"Servizio attualmente non disponibile. Riprovare pi\u00F9 tardi.", L"\u0645\u062A\u0623\u0633\u0641\u06CC\u0645\u060C \u0627\u06CC\u0646 \u0633\u0631\u0648\u06CC\u0633 \u062F\u0631 \u062D\u0627\u0644 \u062D\u0627\u0636\u0631 \u062F\u0631 \u062F\u0633\u062A\u0631\u0633 \u0646\u06CC\u0633\u062A. \u0644\u0637\u0641\u0627\u064B \u0628\u0639\u062F\u0627\u064B \u062F\u0648\u0628\u0627\u0631\u0647 \u0627\u0645\u062A\u062D\u0627\u0646 \u06A9\u0646\u06CC\u062F.", L"El servicio no est\u00E1 disponible en este momento. Int\u00E9ntelo de nuevo m\u00E1s tarde.", L"Le service n\u2019est pas disponible actuellement. R\u00E9essayez ult\u00E9rieurement.", L"No momento, o servi\u00E7o n\u00E3o est\u00E1 dispon\u00EDvel. Tente novamente mais tarde."},
    {903, L"Time allowance unexpanded icon", L"Icona tempo consentito non espansa", L"\u0646\u0645\u0627\u062F \u0628\u062F\u0648\u0646 \u06AF\u0633\u062A\u0631\u0634 \u0645\u062D\u062F\u0648\u062F\u0647 \u0632\u0645\u0627\u0646\u06CC \u0645\u062C\u0627\u0632", L"Icono de tiempo permitido sin expandir", L"Ic\u00F4ne non d\u00E9velopp\u00E9e de la dur\u00E9e autoris\u00E9e", L"\u00CDcone de tempo permitido n\u00E3o expandido"},
    {904, L"Time allowance expanded icon", L"Icona tempo consentito espansa", L"\u0646\u0645\u0627\u062F \u0642\u0627\u0628\u0644 \u06AF\u0633\u062A\u0631\u0634 \u0645\u062D\u062F\u0648\u062F\u0647 \u0632\u0645\u0627\u0646\u06CC \u0645\u062C\u0627\u0632", L"Icono de tiempo permitido expandido", L"Ic\u00F4ne d\u00E9velopp\u00E9e de la dur\u00E9e autoris\u00E9e", L"\u00CDcone de tempo permitido expandido"},
    {905, L"Varies", L"Varia", L"\u0645\u062A\u0641\u0627\u0648\u062A \u0627\u0633\u062A", L"Var\u00EDa", L"Varie", L"Varia"},
    {906, L"{0} Hours", L"{0} ore", L"{0} \u0633\u0627\u0639\u062A", L"{0} horas", L"{0}\u00A0heures", L"{0} Horas"},
    {907, L"{0} Minutes", L"{0} minuti", L"{0} \u062F\u0642\u06CC\u0642\u0647", L"{0} minutos", L"{0} minutes", L"{0} Minutos"},
    {908, L"People who can manage these settings online: {0}", L"Persone autorizzate a gestire le impostazioni online: {0}", L"\u0627\u0641\u0631\u0627\u062F\u06CC \u06A9\u0647 \u0645\u06CC \u062A\u0648\u0627\u0646\u0646\u062F \u0627\u06CC\u0646 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0631\u0627 \u0628\u0635\u0648\u0631\u062A \u0622\u0646\u0644\u0627\u06CC\u0646 \u0645\u062F\u06CC\u0631\u06CC\u062A \u06A9\u0646\u0646\u062F: {0}", L"Personas que pueden administrar esta configuraci\u00F3n en l\u00EDnea: {0}", L"Personnes autoris\u00E9es \u00E0 g\u00E9rer ces param\u00E8tres en ligne\u00A0: {0}", L"Pessoas que podem gerenciar essas configura\u00E7\u00F5es online: {0}"},
    {909, L", ", L", ", L"\u060C ", L", ", L", ", L", "},
    {910, L"You need a Microsoft account to manage these settings online", L"Per gestire queste impostazioni online \u00E8 necessario un account Microsoft", L"\u0628\u0631\u0627\u06CC \u0645\u062F\u06CC\u0631\u06CC\u062A \u06A9\u0631\u062F\u0646 \u0627\u06CC\u0646 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0628\u0635\u0648\u0631\u062A \u0622\u0646\u0644\u0627\u06CC\u0646 \u0628\u0647 \u06CC\u06A9 \u062D\u0633\u0627\u0628 Microsoft \u0646\u06CC\u0627\u0632 \u062F\u0627\u0631\u06CC\u062F", L"Necesita una cuenta Microsoft para administrar esta configuraci\u00F3n en l\u00EDnea", L"Vous avez besoin d\u2019un compte Microsoft pour g\u00E9rer ces param\u00E8tres en ligne", L"Voc\u00EA precisa de uma conta da Microsoft para gerenciar estas configura\u00E7\u00F5es online"},
    {911, L"Switch to a Microsoft account now", L"Passa a un account Microsoft ora", L"\u0647\u0645 \u0627\u06A9\u0646\u0648\u0646 \u0628\u0647 \u06CC\u06A9 \u062D\u0633\u0627\u0628 Microsoft \u062A\u0639\u0648\u06CC\u0636 \u06A9\u0646\u06CC\u062F", L"Cambiar a una cuenta Microsoft ahora", L"Passer \u00E0 un compte Microsoft maintenant", L"Alternar para uma conta da Microsoft agora"},
    {912, L"Continue using a local account", L"Continua a utilizzare l'account locale", L"\u0627\u062F\u0627\u0645\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u06CC\u06A9 \u062D\u0633\u0627\u0628 \u0645\u062D\u0644\u06CC", L"Continuar usando una cuenta local", L"Continuer en utilisant un compte local", L"Continue a usar uma conta local"},
    {961, L"To manage this PC online, you need to verify your Microsoft account. Go to PC settings, select Accounts, and then select Your account.", L"Per gestire online questo PC, \u00E8 necessario verificare l'account Microsoft. Andare a Impostazioni PC, selezionare Account e quindi Il tuo account.", L"\u0628\u0631\u0627\u06CC \u0645\u062F\u06CC\u0631\u06CC\u062A \u0622\u0646\u0644\u0627\u06CC\u0646 \u0627\u06CC\u0646 \u0631\u0627\u06CC\u0627\u0646\u0647\u060C \u0628\u0627\u06CC\u062F \u062D\u0633\u0627\u0628 Microsoft \u062E\u0648\u062F \u0631\u0627 \u062A\u0623\u06CC\u06CC\u062F \u06A9\u0646\u06CC\u062F. \u0628\u0647 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0631\u0627\u06CC\u0627\u0646\u0647 \u0628\u0631\u0648\u06CC\u062F\u060C \u062D\u0633\u0627\u0628\u200C\u0647\u0627 \u0631\u0627 \u0627\u0646\u062A\u062E\u0627\u0628 \u06A9\u0646\u06CC\u062F \u0648 \u0633\u067E\u0633 \u062D\u0633\u0627\u0628 \u0634\u0645\u0627 \u0631\u0627 \u0627\u0646\u062A\u062E\u0627\u0628 \u06A9\u0646\u06CC\u062F.", L"Para administrar este PC en l\u00EDnea, es necesario verificar la cuenta de Microsoft. Ve a la configuraci\u00F3n del PC, selecciona Cuentas y, a continuaci\u00F3n, selecciona Tu cuenta.", L"Pour g\u00E9rer cet ordinateur en ligne, vous devez valider votre compte Microsoft. Acc\u00E9dez aux Param\u00E8tres du PC, s\u00E9lectionnez Comptes, puis Votre compte.", L"Para gerenciar esse computador online, voc\u00EA necessita confirmar sua conta da Microsoft. Acesse Configura\u00E7\u00F5es do computador, selecione Contas e, em seguida, selecione Sua conta."},
    {1000, L"Extension path", L"Percorso estensione", L"\u0645\u0633\u06CC\u0631 \u067E\u0633\u0648\u0646\u062F", L"Ruta de la extensi\u00F3n", L"Chemin d\u2019acc\u00E8s de l\u2019extension", L"Caminho da extens\u00E3o"},
    {1001, L"Extension subsystem", L"Sottosistema estensione", L"\u0632\u06CC\u0631\u0633\u06CC\u0633\u062A\u0645 \u067E\u0633\u0648\u0646\u062F", L"Subsistema de la extensi\u00F3n", L"Sous-syst\u00E8me d\u2019extension", L"Subsistema de extens\u00E3o"},
    {1002, L"Extension image path", L"Percorso immagine estensione", L"\u0645\u0633\u06CC\u0631 \u062A\u0635\u0648\u06CC\u0631 \u067E\u0633\u0648\u0646\u062F", L"Ruta de la imagen de la extensi\u00F3n", L"Chemin d\u2019acc\u00E8s \u00E0 l\u2019image de l\u2019extension", L"Caminho da imagem da extens\u00E3o"},
    {1003, L"Extension name", L"Nome estensione", L"\u0646\u0627\u0645 \u067E\u0633\u0648\u0646\u062F", L"Nombre de la extensi\u00F3n", L"Nom d\u2019extension", L"Nome da extens\u00E3o"},
    {1004, L"Extension subtitle", L"Sottotitolo estensione", L"\u0632\u06CC\u0631\u0646\u0648\u06CC\u0633 \u067E\u0633\u0648\u0646\u062F", L"Subt\u00EDtulo de la extensi\u00F3n", L"Sous-titre de l\u2019extension", L"Subt\u00EDtulo da extens\u00E3o"},
    {1005, L"Current rating system", L"Sistema di classificazione corrente", L"\u0633\u06CC\u0633\u062A\u0645 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0641\u0639\u0644\u06CC", L"Sistema de clasificaci\u00F3n actual", L"Syst\u00E8me de classification actuel", L"Sistema de classifica\u00E7\u00E3o atual"},
    {1006, L"Last Log View", L"Ultima visualizzazione registro", L"\u0622\u062E\u0631\u06CC\u0646 \u0645\u0634\u0627\u0647\u062F\u0647 \u0648\u0627\u0642\u0639\u0647 \u0646\u06AF\u0627\u0631", L"Vista de registro m\u00E1s reciente", L"Dernier affichage du journal", L"\u00DAltima Exibi\u00E7\u00E3o de Log"},
    {1007, L"Log view reminder interval", L"Intervallo promemoria visualizzazione registro", L"\u0641\u0627\u0635\u0644\u0647 \u0632\u0645\u0627\u0646\u06CC \u06CC\u0627\u062F\u0622\u0648\u0631\u06CC \u0645\u0634\u0627\u0647\u062F\u0647 \u0648\u0627\u0642\u0639\u0647 \u0646\u06AF\u0627\u0631", L"Intervalo de aviso de vista de registro", L"Intervalle de rappel pour l\u2019affichage du journal", L"Intervalo de lembrete de exibi\u00E7\u00E3o de log"},
    {1009, L"HTTP exemption list", L"Elenco esenzioni HTTP", L"\u0644\u06CC\u0633\u062A \u0627\u0633\u062A\u062B\u0646\u0627\u0621 HTTP", L"Lista de exenciones de HTTP", L"Liste des exemptions HTTP", L"Lista de isen\u00E7\u00E3o de HTTP"},
    {1010, L"URL exemption list", L"Elenco esenzioni URL", L"\u0644\u06CC\u0633\u062A \u0627\u0633\u062A\u062B\u0646\u0627\u0621 URL", L"Lista de exenciones de URL", L"Liste des exemptions d\u2019URL", L"Lista de isen\u00E7\u00E3o de URL"},
    {1011, L"Custom web filter ID", L"ID filtro Web personalizzato", L"\u0634\u0646\u0627\u0633\u0647 \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0633\u0641\u0627\u0631\u0634\u06CC", L"Id. de filtro web personalizado", L"Identificateur du filtre Web personnalis\u00E9", L"ID do filtro da Web personalizado"},
    {1012, L"Custom web filter name", L"Nome filtro Web personalizzato", L"\u0646\u0627\u0645 \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0633\u0641\u0627\u0631\u0634\u06CC", L"Nombre de filtro web personalizado", L"Nom du filtre Web personnalis\u00E9", L"Nome do filtro da Web personalizado"},
    {1013, L"Locale", L"Impostazioni locali", L"\u0645\u0646\u0637\u0642\u0647", L"Configuraci\u00F3n regional", L"Param\u00E8tres r\u00E9gionaux", L"Localidade"},
    {1014, L"Allow/Block", L"Consenti/Blocca", L"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646/\u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646", L"Permitir o bloquear", L"Autoriser/Bloquer", L"Permitir/Bloquear"},
    {1015, L"Game and Windows Store restrictions", L"Restrizioni per giochi e Windows Store", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB", L"Restricciones de juego y de la Tienda Windows", L"Restrictions aux jeux et au Windows Store", L"Restri\u00E7\u00F5es de Jogo e da Windows Store"},
    {1016, L"Game allow unrated", L"Giochi consentiti non classificati", L"\u0628\u0627\u0632\u06CC \u0645\u062C\u0627\u0632 \u0648 \u0628\u062F\u0648\u0646 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Permitir juegos sin clasificaci\u00F3n", L"Autoriser les jeux non class\u00E9s", L"Permitir jogo sem classifica\u00E7\u00E3o"},
    {1017, L"Game max allowed", L"Numero massimo di giochi consentiti", L"\u062D\u062F\u0627\u06A9\u062B\u0631 \u0628\u0627\u0632\u06CC \u0645\u062C\u0627\u0632", L"M\u00E1ximo de juegos permitidos", L"Nombre de jeux autoris\u00E9s", L"M\u00E1ximo permitido para jogo"},
    {1018, L"Game category - %s", L"Categoria gioco - %s", L"\u0631\u062F\u0647 \u0628\u0627\u0632\u06CC - %s", L"Categor\u00EDa de juego: %s", L"Cat\u00E9gorie du jeu - %s", L"Categoria do jogo - %s"},
    {1019, L"User WPC enabled", L"Controllo genitori di Windows abilitato per l'utente", L"WPC \u06A9\u0627\u0631\u0628\u0631 \u0641\u0639\u0627\u0644 \u0627\u0633\u062A", L"WPC de usuario habilitado", L"Contr\u00F4le parental Windows activ\u00E9 pour l\u2019utilisateur", L"Controle dos Pais do Windows habilitado para o usu\u00E1rio"},
    {1020, L"User logging required", L"Registrazione utente necessaria", L"\u0644\u0627\u0632\u0645 \u0627\u0633\u062A \u06A9\u0647 \u06A9\u0627\u0631\u0628\u0631 \u0648\u0627\u0631\u062F \u0633\u06CC\u0633\u062A\u0645 \u0634\u0648\u062F", L"Registro del usuario requerido", L"Journalisation requise pour l\u2019utilisateur", L"Logon de usu\u00E1rio necess\u00E1rio"},
    {1021, L"User hourly restrictions", L"Restrizioni di orario per l'utente", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0633\u0627\u0639\u062A\u06CC \u06A9\u0627\u0631\u0628\u0631", L"Restricciones por hora del usuario", L"Restrictions horaires pour l\u2019utilisateur", L"Restri\u00E7\u00F5es de hor\u00E1rio do usu\u00E1rio"},
    {1022, L"Access requests", L"Richieste di accesso", L"\u062F\u0631\u062E\u0648\u0627\u0633\u062A \u0647\u0627\u06CC \u062F\u0633\u062A\u0631\u0633\u06CC", L"Solicitudes de acceso", L"Demandes d\u2019acc\u00E8s", L"Solicita\u00E7\u00F5es de acesso"},
    {1023, L"User logon hours", L"Orario di accesso per l'utente", L"\u0633\u0627\u0639\u062A \u0647\u0627\u06CC \u0648\u0631\u0648\u062F \u0628\u0647 \u0633\u06CC\u0633\u062A\u0645 \u06A9\u0627\u0631\u0628\u0631", L"Horas de sesi\u00F3n del usuario", L"Heures de session de l\u2019utilisateur", L"Horas de logon do usu\u00E1rio"},
    {1024, L"User app restrictions", L"Restrizioni app per l'utente", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0631\u0646\u0627\u0645\u0647 \u0627\u06CC \u06A9\u0627\u0631\u0628\u0631", L"Restricciones de aplicaci\u00F3n del usuario", L"Restrictions des applications pour l\u2019utilisateur", L"Restri\u00E7\u00F5es de aplicativo do usu\u00E1rio"},
    {1025, L"Web filtering", L"Filtro Web", L"\u0641\u06CC\u0644\u062A\u0631\u06CC\u0646\u06AF \u0648\u0628", L"Filtrado web", L"Filtrage Web", L"Filtragem de sites"},
    {1026, L"Web download blocked", L"Download Web bloccato", L"\u0628\u0627\u0631\u06AF\u06CC\u0631\u06CC \u0648\u0628 \u0645\u0633\u062F\u0648\u062F \u0634\u062F", L"Descarga desde sitios web bloqueada", L"T\u00E9l\u00E9chargement Web bloqu\u00E9", L"Download da Web bloqueado"},
    {1027, L"Web filter level", L"Livello filtro Web", L"\u0633\u0637\u062D \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628", L"Nivel de filtro web", L"Niveau de filtre Web", L"N\u00EDvel de filtro da Web"},
    {1028, L"Web category - %s", L"Categoria Web - %s", L"\u0631\u062F\u0647 \u0648\u0628 - %s", L"Categor\u00EDa web: %s", L"Cat\u00E9gorie Web - %s", L"Categoria da Web - %s"},
    {1029, L"Web block unrated", L"Blocco Web non classificato", L"\u0627\u0646\u0633\u062F\u0627\u062F \u0648\u0628 \u0628\u062F\u0648\u0646 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Bloquear sitios web sin clasificaci\u00F3n", L"Bloquer le contenu Web non \u00E9valu\u00E9", L"Bloqueio da Web sem classifica\u00E7\u00E3o"},
    {1030, L"WPC enabled", L"Controllo genitori di Windows abilitato", L"WPC \u0641\u0639\u0627\u0644 \u0634\u062F", L"WPC habilitado", L"Contr\u00F4le parental Windows activ\u00E9", L"Habilitado para Controle dos Pais do Windows"},
    {1031, L"Logging required", L"Registrazione necessaria", L"\u0644\u0627\u0632\u0645 \u0627\u0633\u062A \u06A9\u0647 \u0648\u0627\u0631\u062F \u0633\u06CC\u0633\u062A\u0645 \u0634\u0648\u06CC\u062F", L"Registro requerido", L"Journalisation requise", L"Logon necess\u00E1rio"},
    {1032, L"Rating system path", L"Percorso sistema di classificazione", L"\u0645\u0633\u06CC\u0631 \u0633\u06CC\u0633\u062A\u0645 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Ruta del sistema de clasificaci\u00F3n", L"Chemin d\u2019acc\u00E8s du syst\u00E8me de classification", L"Caminho do sistema de classifica\u00E7\u00E3o"},
    {1033, L"Unsuccessful sign-in", L"Accesso non riuscito", L"\u0648\u0631\u0648\u062F \u0628\u0647 \u0628\u0631\u0646\u0627\u0645\u0647 \u0646\u0627\u0645\u0648\u0641\u0642", L"Inicio de sesi\u00F3n incorrecto", L"Connexion infructueuse", L"Entrada malsucedida"},
    {1034, L"Net log-in not started", L"Accesso rete non avviato", L"\u200F\u200F\u0648\u0631\u0648\u062F \u0628\u0647 \u0633\u06CC\u0633\u062A\u0645 \u0634\u0628\u06A9\u0647 \u0634\u0631\u0648\u0639 \u0646\u0634\u062F\u0647 \u0627\u0633\u062A", L"Inicio de sesi\u00F3n de red no iniciado", L"Connexion r\u00E9seau non d\u00E9marr\u00E9e", L"Logon de rede n\u00E3o iniciado"},
    {1035, L"Account locked", L"Account bloccato", L"\u200F\u200F\u062D\u0633\u0627\u0628 \u0642\u0641\u0644 \u0634\u062F", L"Cuenta bloqueada", L"Compte verrouill\u00E9", L"Conta bloqueada"},
    {1036, L"Interactive", L"Interattivo", L"\u062A\u0639\u0627\u0645\u0644\u06CC", L"Interactivo", L"Interactif", L"Interativo"},
    {1037, L"Remote interactive", L"Interattivo remoto", L"\u062A\u0639\u0627\u0645\u0644\u06CC \u0631\u0627\u0647 \u062F\u0648\u0631", L"Interactivo remoto", L"Interactif \u00E0 distance", L"Interativo remoto"},
    {1038, L"Unlock", L"Sblocco", L"\u0628\u0627\u0632 \u06A9\u0631\u062F\u0646 \u0642\u0641\u0644", L"Desbloquear", L"D\u00E9verrouiller", L"Desbloquear"},
    {1039, L"%s (%dh)", L"%s (%dh)", L"%s (%dh)", L"%s (%dh)", L"%s (%dh)", L"%s (%dh)"},
    {1040, L"None", L"Nessuno", L"\u0647\u06CC\u0686\u06A9\u062F\u0627\u0645", L"Ninguno", L"Aucun", L"Nenhum"},
    {1041, L"%s, %s", L"%s, %s", L"%s, %s", L"%s, %s", L"%s, %s", L"%s, %s"},
    {1042, L"%s - %s", L"%s - %s", L"%s - %s", L"%s - %s", L"%s - %s", L"%s - %s"},
    {1043, L"URL domain", L"Dominio URL", L"\u062F\u0627\u0645\u0646\u0647 URL", L"Dominio de direcci\u00F3n URL", L"Domaine d\u2019URL", L"Dom\u00EDnio da URL"},
    {1044, L"All users", L"Tutti gli utenti", L"\u0647\u0645\u0647 \u06A9\u0627\u0631\u0628\u0631\u0627\u0646", L"Todos los usuarios", L"Tous les utilisateurs", L"Todos os usu\u00E1rios"},
    {1045, L"%s days", L"%s giorni", L"%s \u0631\u0648\u0632", L"%s d\u00EDas", L"%s jours", L"%s dias"},
    {1046, L"Provider logo picture", L"Immagine logo provider", L"\u062A\u0635\u0648\u06CC\u0631 \u0622\u0631\u0645 \u0627\u0631\u0627\u0626\u0647 \u062F\u0647\u0646\u062F\u0647", L"Imagen de logotipo de proveedor", L"Image du logo du fournisseur", L"Imagem do logotipo do provedor"},
    {1047, L"Time limits picture", L"Immagine di Restrizioni di orario", L"\u062A\u0635\u0648\u06CC\u0631 \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0632\u0645\u0627\u0646\u06CC", L"Imagen de l\u00EDmites de tiempo", L"Image de dur\u00E9es maximales", L"Imagem de limites de tempo"},
    {1048, L"Games picture", L"Immagine di Giochi", L"\u062A\u0635\u0648\u06CC\u0631 \u0628\u0627\u0632\u06CC \u0647\u0627", L"Imagen de juegos", L"Image de jeux", L"Imagem de jogos"},
    {1049, L"Apps picture", L"Immagine di App", L"\u062A\u0635\u0648\u06CC\u0631 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627", L"Imagen de aplicaciones", L"Image d\u2019applications", L"Imagem de aplicativos"},
    {1051, L"Never", L"Mai", L"\u0647\u0631\u06AF\u0632", L"Nunca", L"Jamais", L"Nunca"},
    {1052, L"Yes", L"S\u00EC", L"\u0628\u0644\u0647", L"S\u00ED", L"Oui", L"Sim"},
    {1053, L"No", L"No", L"\u062E\u06CC\u0631", L"No", L"Non", L"N\u00E3o"},
    {1054, L"By rating", L"Per classificazione", L"\u0628\u0631 \u0627\u0633\u0627\u0633 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Por clasificaci\u00F3n", L"Par classification", L"Por classifica\u00E7\u00E3o"},
    {1055, L"Family Safety", L"Sicurezza famiglia", L"\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647", L"Protecci\u00F3n infantil", L"Contr\u00F4le parental", L"Controles dos Pais"},
    {1056, L"Warning: Disabling notification area icon", L"Avviso: disabilitazione dell'icona nell'area di notifica", L"\u0647\u0634\u062F\u0627\u0631: \u0646\u0645\u0627\u062F \u063A\u06CC\u0631\u0641\u0639\u0627\u0644 \u06A9\u0631\u062F\u0646 \u0642\u0633\u0645\u062A \u0627\u0639\u0644\u0627\u0646", L"Advertencia: se est\u00E1 deshabilitando el icono del \u00E1rea de notificaci\u00F3n", L"Avertissement\u00A0: d\u00E9sactivation de l\u2019ic\u00F4ne de la zone de notification", L"Aviso: desabilitando o \u00EDcone da \u00E1rea de notifica\u00E7\u00E3o"},
    {1057, L"Turning off notifications also means users won't receive warnings before their time restrictions are enforced.", L"Se si disattiva l'icona nell'area di notifica, gli utenti non verranno avvisati prima dell'attivazione delle restrizioni di orario.", L"\u200F\u200F\u062E\u0627\u0645\u0648\u0634 \u06A9\u0631\u062F\u0646 \u0627\u0639\u0644\u0627\u0646 \u0647\u0627 \u0646\u06CC\u0632 \u0628\u062F\u06CC\u0646 \u0645\u0639\u0646\u0627\u0633\u062A \u06A9\u0647 \u06A9\u0627\u0631\u0628\u0631\u0627\u0646 \u0642\u0628\u0644 \u0627\u0632 \u067E\u0627\u06CC\u0627\u0646 \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0632\u0645\u0627\u0646\u06CC \u062E\u0648\u062F\u060C \u0647\u0634\u062F\u0627\u0631\u0647\u0627 \u0631\u0627 \u062F\u0631\u06CC\u0627\u0641\u062A \u0646\u062E\u0648\u0627\u0647\u0646\u062F \u06A9\u0631\u062F.", L"Si desactiva las notificaciones, los usuarios no recibir\u00E1n ninguna advertencia antes de que se apliquen las restricciones de tiempo.", L"La d\u00E9sactivation des notifications implique aussi que les utilisateurs ne recevront aucun avertissement avant que leurs restrictions d\u2019horaire ne s\u2019appliquent.", L"O desligamento das notifica\u00E7\u00F5es tamb\u00E9m impede que os usu\u00E1rios recebam avisos antes de as restri\u00E7\u00F5es de tempo serem impostas."},
    {1066, L"%1 can't be added to the list because it's on a removable device or a FAT partition. This app is always allowed to run.", L"Impossibile aggiungere %1 all'elenco perch\u00E9 si trova in un dispositivo rimovibile o in una partizione FAT. L'esecuzione di questa app \u00E8 sempre consentita.", L"\u200F\u200F%1 \u0628\u0647 \u0644\u06CC\u0633\u062A \u0627\u0636\u0627\u0641\u0647 \u0646\u0645\u06CC \u0634\u0648\u062F \u0632\u06CC\u0631\u0627 \u062F\u0631 \u06CC\u06A9 \u062F\u0633\u062A\u06AF\u0627\u0647 \u062C\u062F\u0627\u0634\u062F\u0646\u06CC \u06CC\u0627 \u0642\u0633\u0645\u062A FAT \u0627\u0633\u062A. \u0627\u06CC\u0646 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0645\u06CC\u0634\u0647 \u0628\u0631\u0627\u06CC \u0627\u062C\u0631\u0627 \u0645\u062C\u0627\u0632 \u0627\u0633\u062A.", L"No se puede agregar %1 a la lista porque est\u00E1 en un dispositivo extra\u00EDble o en una partici\u00F3n FAT. Siempre se permite la ejecuci\u00F3n de esta aplicaci\u00F3n.", L"Impossible d\u2019ajouter\u00A0%1 \u00E0 la liste car il se trouve sur un p\u00E9riph\u00E9rique amovible ou une partition FAT. L\u2019ex\u00E9cution de cette application est toujours autoris\u00E9e.", L"%1 n\u00E3o pode ser adicionado \u00E0 lista porque est\u00E1 em um dispositivo remov\u00EDvel ou uma parti\u00E7\u00E3o FAT. Esse aplicativo sempre tem permiss\u00E3o para ser executado."},
    {1067, L"%2 can write to the file %1.\n\n%2 might use this to tamper with and run any app.", L"%2 pu\u00F2 scrivere nel file %1.\n\n%2 potrebbe sfruttare questa possibilit\u00E0 per alterare o eseguire qualsiasi app.", L"\u200F\u200F%2 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062F\u0631 \u067E\u0631\u0648\u0646\u062F\u0647 %1 \u0628\u0646\u0648\u06CC\u0633\u062F.\n\n%2 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0627\u06CC\u0646 \u0628\u0631\u0627\u06CC \u0645\u062F\u0627\u062E\u0644\u0647 \u0648 \u0627\u062C\u0631\u0627\u06CC \u0647\u0631 \u0628\u0631\u0646\u0627\u0645\u0647 \u0627\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F.", L"%2 puede escribir en el archivo %1.\n\nEs posible que %2 pueda usar esto para alterar y ejecutar cualquier aplicaci\u00F3n.", L"%2 peut \u00E9crire dans le fichier %1.\n\n%2 peut en profiter pour modifier une application et l\u2019ex\u00E9cuter.", L"%2 pode gravar no arquivo %1;\n\n%2 pode usar isso para adulterar e executar qualquer aplicativo."},
    {1068, L"Time zone changes by other user", L"Modifiche del fuso orario eseguite da un altro utente", L"\u200F\u200F\u062A\u063A\u06CC\u06CC\u0631\u0627\u062A \u0645\u0646\u0637\u0642\u0647 \u0632\u0645\u0627\u0646\u06CC \u062A\u0648\u0633\u0637 \u06CC\u06A9 \u06A9\u0627\u0631\u0628\u0631 \u062F\u06CC\u06AF\u0631", L"Cambios en la zona horaria por otro usuario", L"Modifications de fuseau horaire par un autre utilisateur", L"Altera\u00E7\u00F5es de fuso hor\u00E1rio feitas por outro usu\u00E1rio"},
    {1072, L"%1 application override(s): %2", L"%1 elevazioni diritti per applicazioni: %2", L"%1 \u0628\u0631\u0646\u0627\u0645\u0647 \u0631\u0648\u0646\u0648\u06CC\u0633\u06CC \u0634\u062F\u0647 \u0627\u0633\u062A: %2", L"%1 invalidaciones de aplicaci\u00F3n: %2", L"%1\u00A0autorisation(s) d'acc\u00E8s aux applications bloqu\u00E9es : %2", L"%1 substitui\u00E7\u00E3o(\u00F5es) de aplicativo(s): %2"},
    {1073, L"%1 website(s) allowed: %2", L"Siti Web consentiti %1, %2", L"%1 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0645\u062C\u0627\u0632 \u0627\u0633\u062A: %2", L"%1 sitios web permitidos: %2", L"%1 site(s) Web autoris\u00E9s\u00A0: %2", L"%1 site(s) permitido(s): %2"},
    {1074, L"%1 file downloads blocked: %2", L"%1 download di file bloccati: %2", L"%1 \u062F\u0627\u0646\u0644\u0648\u062F \u067E\u0631\u0648\u0646\u062F\u0647 \u0645\u0633\u062F\u0648\u062F \u0634\u062F\u0647 \u0627\u0633\u062A: %2", L"%1 descargas de archivo bloqueadas: %2", L"%1 t\u00E9l\u00E9chargement(s) de fichiers bloqu\u00E9s\u00A0: %2", L"%1 downloads de arquivos bloqueados: %2"},
    {1075, L"Allowed in Allow/Block list", L"Consentiti nell'elenco di indirizzi Web consentiti/bloccati", L"\u0645\u062C\u0627\u0632 \u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632/\u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F", L"Permitido en la lista de permitidos o bloqueados", L"Autoris\u00E9 dans la liste rouge/verte", L"Permitido na lista de Permiss\u00F5es/Bloqueios"},
    {1076, L"Blocked in Allow/Block list", L"Bloccati nell'elenco di indirizzi Web consentiti/bloccati", L"\u0645\u0633\u062F\u0648\u062F \u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632/\u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F", L"Bloqueado en la lista de permitidos o bloqueados", L"Bloqu\u00E9 dans la liste rouge/verte", L"Bloqueado na lista de Permiss\u00F5es/Bloqueios"},
    {1077, L"Not in Allow/Block list", L"Non inclusi elenco di indirizzi Web consentiti/bloccati", L"\u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632/\u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F \u0646\u06CC\u0633\u062A", L"No est\u00E1 en la lista de permitidos o bloqueados", L"Absent de la liste rouge/verte", L"N\u00E3o est\u00E1 na lista de Permiss\u00F5es/Bloqueios"},
    {1078, L"Web filter picture", L"Immagine di Filtro Web", L"\u062A\u0635\u0648\u06CC\u0631 \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628", L"Imagen de filtro web", L"Image de filtre Web", L"Imagem do filtro da Web"},
    {1080, L"Activity viewer picture", L"Immagine di Visualizzatore attivit\u00E0", L"\u062A\u0635\u0648\u06CC\u0631 \u0645\u0634\u0627\u0647\u062F\u0647 \u06AF\u0631 \u0641\u0639\u0627\u0644\u06CC\u062A", L"Imagen de visor de actividades", L"Images de la visionneuse d\u2019activit\u00E9", L"Imagem do visualizador de atividades"},
    {1081, L"Importing this list will replace your old list. Additionally, importing malicious Allow/Block lists can effectively disable your web restriction settings. You shouldn't import this file unless you know who created it. Do you want to import this file?", L"L'elenco che si sta per importare sostituir\u00E0 l'elenco precedente. Inoltre l'importazione di elenchi dannosi di indirizzi Web consentiti/bloccati pu\u00F2 disabilitare le impostazioni del filtro delle restrizioni Web. Non \u00E8 consigliabile importare questo file se non si conosce la persona che lo ha creato. Importare il file?", L"\u200F\u200F\u0648\u0627\u0631\u062F \u06A9\u0631\u062F\u0646 \u0627\u06CC\u0646 \u0644\u06CC\u0633\u062A \u0633\u0628\u0628 \u062C\u0627\u06CC\u06AF\u0632\u06CC\u0646 \u0634\u062F\u0646 \u0628\u0627 \u0644\u06CC\u0633\u062A \u0642\u062F\u06CC\u0645\u06CC \u0634\u0645\u0627 \u0645\u06CC \u0634\u0648\u062F. \u0639\u0644\u0627\u0648\u0647 \u0628\u0631 \u0622\u0646\u060C \u0648\u0627\u0631\u062F \u06A9\u0631\u062F\u0646 \u0644\u06CC\u0633\u062A \u0647\u0627\u06CC \u0645\u062C\u0627\u0632/\u0645\u0648\u0627\u0631\u062F \u0645\u0633\u062F\u0648\u062F \u0645\u0636\u0631 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0648\u0628 \u0634\u0645\u0627 \u0631\u0627 \u063A\u06CC\u0631\u0641\u0639\u0627\u0644 \u06A9\u0646\u062F. \u0634\u0645\u0627 \u0646\u0628\u0627\u06CC\u062F \u0627\u06CC\u0646 \u067E\u0631\u0648\u0646\u062F\u0647 \u0631\u0627 \u0648\u0627\u0631\u062F \u06A9\u0646\u06CC\u062F \u0645\u06AF\u0631 \u0627\u06CC\u0646\u06A9\u0647 \u0628\u062F\u0627\u0646\u06CC\u062F \u0686\u0647 \u06A9\u0633\u06CC \u0622\u0646 \u0631\u0627 \u0627\u06CC\u062C\u0627\u062F \u06A9\u0631\u062F\u0647 \u0627\u0633\u062A. \u0645\u06CC \u062E\u0648\u0627\u0647\u06CC\u062F \u0627\u06CC\u0646 \u067E\u0631\u0648\u0646\u062F\u0647 \u0631\u0627 \u0648\u0627\u0631\u062F \u06A9\u0646\u06CC\u062F\u061F", L"Si importa esta lista se reemplazar\u00E1 la lista anterior. Asimismo, la importaci\u00F3n de listas de permitidos o bloqueados malintencionadas puede desactivar de forma efectiva la configuraci\u00F3n de restricci\u00F3n web. No debe importar este archivo a menos que conozca a la persona que lo cre\u00F3. \u00BFDesea importar el archivo?", L"L\u2019importation de cette liste remplacera votre ancienne liste. Par ailleurs, l\u2019importation de listes rouge/verte malveillantes peut, de\u00A0facto, d\u00E9sactiver vos param\u00E8tres de restrictions Web. N\u2019importez pas ce fichier sauf si vous connaissez la personne qui l\u2019a cr\u00E9\u00E9. Voulez-vous importer ce fichier\u00A0?", L"Ao importar esta lista, voc\u00EA substitui a lista antiga. Al\u00E9m disso, importar listas de Permiss\u00F5es/Bloqueios mal-intencionadas pode desabilitar por completo as configura\u00E7\u00F5es de restri\u00E7\u00E3o da Web. Voc\u00EA n\u00E3o deve importar esse arquivo, a menos que confie na pessoa que o criou. Deseja importar esse arquivo?"},
    {1082, L"Hour", L"Ora", L"\u0633\u0627\u0639\u062A", L"Hora", L"Heures", L"Hora"},
    {1083, L"Microsoft Family Safety", L"Microsoft Sicurezza famiglia", L"\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 Microsoft", L"Protecci\u00F3n infantil de Microsoft", L"Contr\u00F4le parental Microsoft", L"Prote\u00E7\u00E3o para a Fam\u00EDlia da Microsoft"},
    {1084, L"Windows Store apps", L"App Windows Store", L"\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows", L"Aplicaciones de la Tienda Windows", L"Applications du Windows Store", L"Aplicativos da Windows Store"},
    {1085, L"Other", L"Altro", L"\u0633\u0627\u06CC\u0631 \u0645\u0648\u0627\u0631\u062F", L"Otros", L"Autre", L"Outros"},
    {1086, L"%1 - %2 event(s): %3", L"%1 - %2 eventi: %3", L"%1 - %2 \u0631\u0648\u06CC\u062F\u0627\u062F: %3", L"%1 - evento(s) de %2: %3", L"%1 - %2 \u00E9v\u00E9nement(s)\u00A0: %3", L"%1 - %2 evento(s): %3"},
    {1087, L"%1 - none", L"%1 - Nessuno", L"%1 - \u0647\u06CC\u0686\u06A9\u062F\u0627\u0645", L"%1 - ninguno", L"%1 - aucun", L"%1 - nenhum"},
    {1088, L"There are no instant messaging activities for this user. This might be because the instant messaging app being used doesn't support Family Safety reporting, or this user hasn't used instant messaging during this time period.", L"Nessuna attivit\u00E0 di messaggistica immediata per l'utente. L'app di messaggistica immediata utilizzata non supporta Sicurezza famiglia oppure l'utente non ha utilizzato la messaggistica immediata in questo periodo.", L"\u200F\u200F\u0647\u06CC\u0686 \u0641\u0639\u0627\u0644\u06CC\u062A \u067E\u06CC\u0627\u0645 \u0631\u0633\u0627\u0646\u06CC \u0641\u0648\u0631\u06CC \u0628\u0631\u0627\u06CC \u0627\u06CC\u0646 \u06A9\u0627\u0631\u0628\u0631 \u0648\u062C\u0648\u062F \u0646\u062F\u0627\u0631\u062F. \u0627\u06CC\u0646 \u0627\u0645\u0631 \u0645\u0645\u06A9\u0646 \u0627\u0633\u062A \u0628\u0647 \u0627\u06CC\u0646 \u062F\u0644\u06CC\u0644 \u0628\u0627\u0634\u062F \u06A9\u0647 \u0628\u0631\u0646\u0627\u0645\u0647 \u067E\u06CC\u0627\u0645 \u0631\u0633\u0627\u0646\u06CC \u0641\u0648\u0631\u06CC \u0645\u0648\u0631\u062F \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0634\u0645\u0627 \u0627\u0632 \u06AF\u0632\u0627\u0631\u0634 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u067E\u0634\u062A\u06CC\u0628\u0627\u0646\u06CC \u0646\u0645\u06CC \u06A9\u0646\u062F \u06CC\u0627 \u0627\u06CC\u0646\u06A9\u0647 \u0627\u06CC\u0646 \u06A9\u0627\u0631\u0628\u0631 \u0627\u0632 \u067E\u06CC\u0627\u0645 \u0631\u0633\u0627\u0646\u06CC \u0641\u0648\u0631\u06CC \u062F\u0631 \u0627\u06CC\u0646 \u062F\u0648\u0631\u0647 \u0632\u0645\u0627\u0646\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0646\u06A9\u0631\u062F\u0647 \u0627\u0633\u062A.", L"No hay actividades de mensajer\u00EDa instant\u00E1nea para este usuario. Esto se puede deber a que la aplicaci\u00F3n de mensajer\u00EDa instant\u00E1nea usada no es compatible con los informes de Protecci\u00F3n infantil o a que el usuario no us\u00F3 la mensajer\u00EDa instant\u00E1nea durante este per\u00EDodo.", L"Aucune activit\u00E9 de messagerie instantan\u00E9e pour cet utilisateur. Ceci peut \u00EAtre d\u00FB au fait que l\u2019application de messagerie instantan\u00E9e utilis\u00E9e ne prend pas en charge les rapports d\u2019activit\u00E9 du contr\u00F4le parental ou que l\u2019utilisateur ne s\u2019en est pas servi pendant cette p\u00E9riode.", L"N\u00E3o h\u00E1 nenhuma atividade de mensagem instant\u00E2nea para este usu\u00E1rio. A causa prov\u00E1vel disso \u00E9 que o aplicativo de mensagens instant\u00E2neas usado n\u00E3o d\u00E1 suporte a relat\u00F3rios de Prote\u00E7\u00E3o para a Fam\u00EDlia ou esse usu\u00E1rio n\u00E3o usou mensagens instant\u00E2neas durante esse per\u00EDodo."},
    {1089, L"There are no email activities for this user. This might be because the email app being used doesn't support Family Safety, or this user hasn't used email during this time period.", L"Nessuna attivit\u00E0 di posta elettronica per l'utente. L'app di posta elettronica utilizzata non supporta Sicurezza famiglia oppure l'utente non ha utilizzato la posta elettronica in questo periodo.", L"\u200F\u200F\u0647\u06CC\u0686 \u0641\u0639\u0627\u0644\u06CC\u062A \u067E\u0633\u062A \u0627\u0644\u06A9\u062A\u0631\u0648\u0646\u06CC\u06A9\u06CC \u0628\u0631\u0627\u06CC \u0627\u06CC\u0646 \u06A9\u0627\u0631\u0628\u0631 \u0648\u062C\u0648\u062F \u0646\u062F\u0627\u0631\u062F. \u0627\u06CC\u0646 \u0627\u0645\u0631 \u0645\u0645\u06A9\u0646 \u0627\u0633\u062A \u0628\u0647 \u0627\u06CC\u0646 \u062F\u0644\u06CC\u0644 \u0628\u0627\u0634\u062F \u06A9\u0647 \u0628\u0631\u0646\u0627\u0645\u0647 \u067E\u0633\u062A \u0627\u0644\u06A9\u062A\u0631\u0648\u0646\u06CC\u06A9\u06CC \u0645\u0648\u0631\u062F \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u067E\u0634\u062A\u06CC\u0628\u0627\u0646\u06CC \u0646\u0645\u06CC \u06A9\u0646\u062F \u06CC\u0627 \u0627\u06CC\u0646\u06A9\u0647 \u0627\u06CC\u0646 \u06A9\u0627\u0631\u0628\u0631 \u062F\u0631 \u0627\u06CC\u0646 \u062F\u0648\u0631\u0647 \u0632\u0645\u0627\u0646\u06CC \u0627\u0632 \u067E\u0633\u062A \u0627\u0644\u06A9\u062A\u0631\u0648\u0646\u06CC\u06A9\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0646\u06A9\u0631\u062F\u0647 \u0627\u0633\u062A.", L"No hay actividades de correo electr\u00F3nico para este usuario. Esto se puede deber a que la aplicaci\u00F3n de correo electr\u00F3nico usada no es compatible con la Protecci\u00F3n infantil o a que el usuario no us\u00F3 el correo electr\u00F3nico durante este per\u00EDodo.", L"Aucune activit\u00E9 de messagerie pour cet utilisateur. Ceci peut \u00EAtre d\u00FB au fait que l\u2019application de messagerie utilis\u00E9e ne prend pas en charge le contr\u00F4le parental ou que l\u2019utilisateur ne s\u2019en est pas servi pendant cette p\u00E9riode.", L"N\u00E3o h\u00E1 nenhuma atividade de email para este usu\u00E1rio. A causa prov\u00E1vel disso \u00E9 que o aplicativo de email usado n\u00E3o d\u00E1 suporte \u00E0 Prote\u00E7\u00E3o para a Fam\u00EDlia ou esse usu\u00E1rio n\u00E3o usou emails durante esse per\u00EDodo."},
    {1090, L"There are no media player activites for this user. This might be because the media player app being used doesn't support Family Safety reporting, or this user hasn't used a media player during this time period.", L"Nessuna attivit\u00E0 del lettore multimediale per l'utente. Il lettore multimediale utilizzato non supporta Sicurezza famiglia oppure l'utente non ha utilizzato un lettore multimediale in questo periodo.", L"\u200F\u200F\u0647\u06CC\u0686 \u0641\u0639\u0627\u0644\u06CC\u062A \u067E\u062E\u0634 \u06A9\u0646\u0646\u062F\u0647 \u0631\u0633\u0627\u0646\u0647 \u0627\u06CC \u0628\u0631\u0627\u06CC \u0627\u06CC\u0646 \u06A9\u0627\u0631\u0628\u0631 \u0648\u062C\u0648\u062F \u0646\u062F\u0627\u0631\u062F. \u0627\u06CC\u0646 \u0627\u0645\u0631 \u0645\u0645\u06A9\u0646 \u0627\u0633\u062A \u0628\u0647 \u0627\u06CC\u0646 \u062F\u0644\u06CC\u0644 \u0628\u0627\u0634\u062F \u06A9\u0647 \u0628\u0631\u0646\u0627\u0645\u0647 \u067E\u062E\u0634 \u06A9\u0646\u0646\u062F\u0647 \u0631\u0633\u0627\u0646\u0647 \u0645\u0648\u0631\u062F \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u06AF\u0632\u0627\u0631\u0634 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u067E\u0634\u062A\u06CC\u0628\u0627\u0646\u06CC \u0646\u0645\u06CC \u06A9\u0646\u062F \u06CC\u0627 \u0627\u06CC\u0646\u06A9\u0647 \u0627\u06CC\u0646 \u06A9\u0627\u0631\u0628\u0631 \u062F\u0631 \u0627\u06CC\u0646 \u062F\u0648\u0631\u0647 \u0632\u0645\u0627\u0646\u06CC \u0627\u0632 \u067E\u062E\u0634 \u06A9\u0646\u0646\u062F\u0647 \u0631\u0633\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0646\u06A9\u0631\u062F\u0647 \u0627\u0633\u062A.", L"No hay actividades de reproductor multimedia para este usuario. Esto se puede deber a que la aplicaci\u00F3n de reproductor multimedia usada no es compatible con los informes de Protecci\u00F3n infantil o a que el usuario no us\u00F3 ning\u00FAn reproductor multimedia durante este per\u00EDodo.", L"Aucune activit\u00E9 de lecture multim\u00E9dia pour cet utilisateur. Ceci peut \u00EAtre d\u00FB au fait que le lecteur multim\u00E9dia utilis\u00E9 ne prend pas en charge les rapports d\u2019activit\u00E9 du contr\u00F4le parental ou que l\u2019utilisateur ne s\u2019en est pas servi pendant cette p\u00E9riode.", L"N\u00E3o h\u00E1 nenhuma atividade de execu\u00E7\u00E3o de m\u00EDdia para este usu\u00E1rio. A causa prov\u00E1vel disso \u00E9 que o aplicativo de media player usado n\u00E3o d\u00E1 suporte a relat\u00F3rios de Prote\u00E7\u00E3o para a Fam\u00EDlia ou esse usu\u00E1rio n\u00E3o usou um media player durante esse per\u00EDodo."},
    {1091, L"Switch back to the Family Safety web filter", L"Reimposta il filtro Web di Sicurezza famiglia", L"\u0628\u0647 \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0628\u0627\u0632\u06AF\u0631\u062F\u06CC\u062F", L"Volver al filtro web de Protecci\u00F3n infantil", L"R\u00E9tablir le filtre Web du contr\u00F4le parental", L"Redefinir para o filtro de Web de Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {1092, L"The web filter has been switched back to the Family Safety web filter.", L"Il filtro Web \u00E8 stato reimpostato sul filtro Web di Sicurezza famiglia.", L"\u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0628\u0647 \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0628\u0627\u0632\u06AF\u0631\u062F\u0627\u0646\u062F\u0647 \u0634\u062F\u0647 \u0627\u0633\u062A.", L"Se volvi\u00F3 del filtro web al filtro web de Protecci\u00F3n infantil.", L"Le filtre de contenu du contr\u00F4le parental a \u00E9t\u00E9 r\u00E9tabli comme filtre de contenu Web.", L"O filtro de Web foi redefinido como Filtro de Web de Prote\u00E7\u00E3o para a Fam\u00EDlia."},
    {1093, L"Failed to switched the web filter back to the Family Safety web filter.", L"Impossibile reimpostare il filtro Web sul filtro Web di Sicurezza famiglia.", L"\u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0628\u0647 \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0628\u0627\u0632\u06AF\u0631\u062F\u0627\u0646\u062F\u0647 \u0646\u0634\u062F.", L"No se pudo volver del filtro web al filtro web de Protecci\u00F3n infantil.", L"Le filtre de contenu du contr\u00F4le parental n\u2019a pas pu \u00EAtre r\u00E9tabli comme filtre de contenu Web.", L"Falha ao redefinir o filtro de Web como Filtro de Conte\u00FAdo da Web de Prote\u00E7\u00E3o para a Fam\u00EDlia."},
    {1094, L"%s can only use allowed apps", L"%s pu\u00F2 utilizzare solo le app consentite", L"%s \u062A\u0646\u0647\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u0645\u062C\u0627\u0632 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%s solo puede usar aplicaciones permitidas", L"%s ne peut utiliser que les applications autoris\u00E9es", L"%s s\u00F3 pode usar aplicativos permitidos"},
    {1095, L"Check the apps that can be used:", L"Selezionare le app che possono essere utilizzate:", L"\u200F\u200F\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC\u06CC \u06A9\u0647 \u0642\u0627\u0628\u0644 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0647\u0633\u062A\u0646\u062F \u0631\u0627 \u0628\u0631\u0631\u0633\u06CC \u06A9\u0646\u06CC\u062F:", L"Comprobar las aplicaciones que se pueden usar:", L"Cochez les applications utilisables\u00A0:", L"Verificar quais aplicativos podem ser usados:"},
    {1096, L"The following apps can be used:", L"Le app seguenti possono essere utilizzate:", L"\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u0632\u06CC\u0631 \u0642\u0627\u0628\u0644 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0647\u0633\u062A\u0646\u062F:", L"Se pueden usar las siguientes aplicaciones:", L"Les applications suivantes peuvent \u00EAtre utilis\u00E9es\u00A0:", L"Os seguintes aplicativos podem ser usados:"},
    {1107, L"Choose a user and set up Family Safety", L"Scegliere un utente e impostare Sicurezza famiglia", L"\u200F\u200F\u06CC\u06A9 \u06A9\u0627\u0631\u0628\u0631 \u0631\u0627 \u0627\u0646\u062A\u062E\u0627\u0628 \u06A9\u0631\u062F\u0647 \u0648 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u06CC\u062F", L"Elegir un usuario y configurar la Protecci\u00F3n infantil", L"Choix d\u2019un utilisateur et configuration du contr\u00F4le parental", L"Escolher um usu\u00E1rio e configurar a Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {1110, L"If you want to apply Family Safety to someone who isn't in this list, <a>create a new user account</a> for them to use.", L"Per applicare Sicurezza famiglia a un utente non incluso nell'elenco, <a>creare un nuovo account utente</a> per l'utente in questione.", L"\u200F\u200F\u0627\u06AF\u0631 \u0645\u06CC \u062E\u0648\u0627\u0647\u06CC\u062F \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0631\u0627 \u0628\u0631\u0627\u06CC \u0641\u0631\u062F\u06CC \u0627\u0639\u0645\u0627\u0644 \u06A9\u0646\u06CC\u062F \u06A9\u0647 \u062F\u0631 \u0627\u06CC\u0646 \u0644\u06CC\u0633\u062A \u0646\u06CC\u0633\u062A\u060C <a>\u06CC\u06A9 \u062D\u0633\u0627\u0628 \u06A9\u0627\u0631\u0628\u0631\u06CC \u062C\u062F\u06CC\u062F \u0628\u0631\u0627\u06CC \u0622\u0646\u0647\u0627 \u0627\u06CC\u062C\u0627\u062F \u06A9\u0646\u06CC\u062F</a> \u062A\u0627 \u0627\u0632 \u0622\u0646 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u0646\u062F.", L"Si desea aplicar la Protecci\u00F3n infantil a alguien que no est\u00E1 en esta lista, <a>cree una nueva cuenta de usuario</a> para dicha persona.", L"Si vous voulez appliquer le contr\u00F4le parental \u00E0 une personne non inscrite dans cette liste, <a>cr\u00E9ez-lui un compte d\u2019utilisateur</a>.", L"Se quiser aplicar a Prote\u00E7\u00E3o para a Fam\u00EDlia em algu\u00E9m que n\u00E3o esteja na lista, <a>crie uma nova conta de usu\u00E1rio</a> para essa pessoa usar."},
    {1117, L"Family Safety:", L"Sicurezza famiglia:", L"\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647:", L"Protecci\u00F3n infantil:", L"Contr\u00F4le parental\u00A0:", L"Prote\u00E7\u00E3o para a Fam\u00EDlia:"},
    {1118, L"On, enforce current settings", L"Attivato, applica le impostazioni correnti", L"\u0631\u0648\u0634\u0646\u060C \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0641\u0639\u0644\u06CC \u062A\u062D\u0645\u06CC\u0644 \u0634\u0648\u062F", L"Activado, aplicar configuraci\u00F3n actual", L"Activ\u00E9, les param\u00E8tres actuels sont appliqu\u00E9s", L"Ativar, aplicar configura\u00E7\u00F5es atuais"},
    {1119, L"Off", L"Disattivato", L"\u062E\u0627\u0645\u0648\u0634", L"Desactivado", L"D\u00E9sactiv\u00E9", L"Desativar"},
    {1120, L"Activity reporting:", L"Rapporto attivit\u00E0:", L"\u06AF\u0632\u0627\u0631\u0634 \u0641\u0639\u0627\u0644\u06CC\u062A:", L"Informe de actividades:", L"Rapports d\u2019activit\u00E9\u00A0:", L"Sistema de relat\u00F3rio de atividades:"},
    {1121, L"On, collect information about PC usage", L"Attivato, raccogli informazioni sull'utilizzo del PC", L"\u0631\u0648\u0634\u0646\u060C \u0627\u0637\u0644\u0627\u0639\u0627\u062A \u0645\u0631\u0628\u0648\u0637 \u0628\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u062C\u0645\u0639 \u0622\u0648\u0631\u06CC \u0634\u0648\u062F", L"Activado, recopilar informaci\u00F3n sobre el uso del equipo", L"Activ\u00E9, recueillir des informations sur l\u2019utilisation de l\u2019ordinateur", L"Ligado; coletar informa\u00E7\u00F5es sobre o uso do computador"},
    {1122, L"Off", L"Disattivato", L"\u062E\u0627\u0645\u0648\u0634", L"Desactivado", L"D\u00E9sactiv\u00E9", L"Desativar"},
    {1126, L"Windows settings:", L"Impostazioni di Windows:", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A Windows:", L"Configuraci\u00F3n de Windows:", L"Param\u00E8tres Windows\u00A0:", L"Configura\u00E7\u00F5es do Windows:"},
    {1127, L"Time limits", L"Restrizioni di orario", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0632\u0645\u0627\u0646\u06CC", L"L\u00EDmites de tiempo", L"Limites horaires", L"Limites de tempo"},
    {1128, L"Game and Windows Store app restrictions", L"Restrizioni per giochi e app di Windows Store", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB", L"Restricciones de aplicaciones de juego y de la Tienda Windows", L"Restrictions d\u2019applications de jeu et du Windows Store", L"Restri\u00E7\u00F5es de Jogo e de aplicativo da Windows Store"},
    {1129, L"Control by rating or title", L"Consente di controllare in base alla classificazione o al titolo", L"\u06A9\u0646\u062A\u0631\u0644 \u0628\u0631 \u0627\u0633\u0627\u0633 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u06CC\u0627 \u0639\u0646\u0648\u0627\u0646", L"Controlar por clasificaci\u00F3n o t\u00EDtulo", L"G\u00E9rer par classification ou titre", L"Controlar jogos por classifica\u00E7\u00E3o ou t\u00EDtulo"},
    {1130, L"Desktop app restrictions", L"Restrizioni per app desktop", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u062A\u062D\u062A \u062F\u0633\u06A9 \u062A\u0627\u067E", L"Restricciones de aplicaciones de escritorio", L"Restrictions d\u2019applications de bureau", L"Restri\u00E7\u00F5es de aplicativo da \u00E1rea de trabalho"},
    {1131, L"Control the apps allowed on your PC", L"Determina le app consentite nel PC", L"\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u0645\u062C\u0627\u0632 \u062F\u0631 \u0631\u0627\u06CC\u0627\u0646\u0647 \u062E\u0648\u062F \u0631\u0627 \u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F", L"Controlar las aplicaciones permitidas en el equipo", L"G\u00E9rer les applications autoris\u00E9es sur votre ordinateur", L"Controlar os aplicativos permitidos no computador"},
    {1132, L"View activity reports", L"Visualizza rapporti attivit\u00E0", L"\u0645\u0634\u0627\u0647\u062F\u0647 \u06AF\u0632\u0627\u0631\u0634 \u0647\u0627\u06CC \u0641\u0639\u0627\u0644\u06CC\u062A", L"Ver informes de actividades", L"Afficher les rapports d\u2019activit\u00E9", L"Exibir relat\u00F3rios de atividades"},
    {1136, L"Web filtering:", L"Filtro Web:", L"\u0641\u06CC\u0644\u062A\u0631\u06CC\u0646\u06AF \u0648\u0628:", L"Filtrado web:", L"Filtrage Web\u00A0:", L"Filtragem de sites:"},
    {1137, L"On", L"Attivato", L"\u0631\u0648\u0634\u0646", L"Activado", L"Activ\u00E9", L"Ligar"},
    {1138, L"Time limits:", L"Restrizioni di orario:", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0632\u0645\u0627\u0646\u06CC:", L"L\u00EDmites de tiempo:", L"Limites des horaires\u00A0:", L"Limites de tempo:"},
    {1139, L"On", L"Attivato", L"\u0631\u0648\u0634\u0646", L"Activado", L"Activ\u00E9es", L"Ativado"},
    {1140, L"Game restrictions:", L"Restrizioni per giochi:", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0627\u0632\u06CC:", L"Restricciones de juego:", L"Restrictions de jeux\u00A0:", L"Restri\u00E7\u00F5es de jogo:"},
    {1141, L"On", L"Attivato", L"\u0631\u0648\u0634\u0646", L"Activado", L"Activ\u00E9es", L"Ativado"},
    {1144, L"Desktop app restrictions:", L"Restrizioni per app desktop:", L"\u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u062A\u062D\u062A \u062F\u0633\u06A9 \u062A\u0627\u067E:", L"Restricciones de aplicaciones de escritorio:", L"Restrictions d\u2019applications\u00A0:", L"Restri\u00E7\u00F5es de aplicativo da \u00E1rea de trabalho:"},
    {1145, L"On", L"Attivato", L"\u0631\u0648\u0634\u0646", L"Activado", L"Activ\u00E9", L"Ativado"},
    {1146, L"Family Safety:", L"Sicurezza famiglia:", L"\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647:", L"Protecci\u00F3n infantil:", L"Contr\u00F4le parental\u00A0:", L"Prote\u00E7\u00E3o para a Fam\u00EDlia:"},
    {1147, L"Off", L"Disattivato", L"\u062E\u0627\u0645\u0648\u0634", L"Desactivado", L"D\u00E9sactiv\u00E9", L"Desativado"},
    {1149, L"Control which types of games and Windows Store apps %username% can use", L"Determina i tipi di giochi o di app di Windows Store che %username% pu\u00F2 utilizzare", L"\u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %username% \u0627\u0632 \u0686\u0647 \u0646\u0648\u0639 \u0628\u0627\u0632\u06CC \u0648 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Controlar el tipo de juegos o aplicaciones de la Tienda Windows que puede usar %username%", L"G\u00E9rer les types de jeux et d\u2019applications du Windows Store utilisables par %username%", L"Controlar que tipos de jogos ou aplicativos da Windows Store %username% pode usar"},
    {1153, L"Allow or block games and Windows Store apps by rating", L"Consenti o blocca i giochi e le app di Windows Store in base alla classificazione", L"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB \u0628\u0631 \u0627\u0633\u0627\u0633 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"Permitir o bloquear juegos y las aplicaciones de la Tienda Windows por clasificaci\u00F3n", L"Autoriser ou bloquer les jeux et les applications du Windows Store en fonction de leur classification", L"Pemitir ou bloquear jogos e aplicativos da Windows Store por classifica\u00E7\u00E3o"},
    {1154, L"Set game and Windows Store ratings", L"Imposta classificazioni di giochi e Windows Store", L"\u062A\u0646\u0638\u06CC\u0645 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0647\u0627\u06CC \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB", L"Establecer clasificaci\u00F3n de juegos y la Tienda Windows", L"D\u00E9finir la classification des jeux et du Windows Store", L"Definir classifica\u00E7\u00F5es de jogo e da Windows Store"},
    {1155, L"Set game and Windows Store ratings", L"Imposta classificazioni di giochi e Windows Store", L"\u062A\u0646\u0638\u06CC\u0645 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0647\u0627\u06CC \u0628\u0627\u0632\u06CC \u0648 \u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows", L"Establecer clasificaci\u00F3n de juegos y la Tienda Windows", L"D\u00E9finir la classification des jeux et du Windows Store", L"Definir classifica\u00E7\u00F5es de jogo e da Windows Store"},
    {1156, L"Maximum allowed rating: ", L"Classificazione massima consentita: ", L"\u062D\u062F\u0627\u06A9\u062B\u0631 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0645\u062C\u0627\u0632: ", L"Clasificaci\u00F3n m\u00E1xima permitida: ", L"Classification maximale\u00A0autoris\u00E9e : ", L"Classifica\u00E7\u00E3o m\u00E1xima permitida: "},
    {1158, L"Allow or block any game on your PC by name", L"Consenti o blocca giochi nel PC in base al nome", L"\u200F\u200F\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0647\u0631 \u0628\u0627\u0632\u06CC \u0631\u0648\u06CC \u0631\u0627\u06CC\u0627\u0646\u0647 \u062E\u0648\u062F \u0628\u0631 \u0627\u0633\u0627\u0633 \u0646\u0627\u0645", L"Permitir o bloquear cualquier juego en el equipo por nombre", L"Autoriser ou bloquer les jeux sur votre ordinateur par leur nom", L"Bloquear ou permitir qualquer jogo no computador por nome"},
    {1159, L"Allow or block specific games", L"Consenti o blocca giochi specifici", L"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u062E\u0627\u0635", L"Permitir o bloquear juegos espec\u00EDficos", L"Autoriser ou bloquer des jeux", L"Bloquear ou permitir jogos espec\u00EDficos"},
    {1162, L"Control which types of games %username% can play", L"Controlla quali tipi di giochi %username% pu\u00F2 eseguire", L"\u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %username% \u0686\u0647 \u0646\u0648\u0639 \u0628\u0627\u0632\u06CC \u0647\u0627\u06CC\u06CC \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0646\u062C\u0627\u0645 \u062F\u0647\u062F", L"Controlar el tipo de juegos a los que %username% puede jugar", L"Contr\u00F4ler les types de jeux accessibles \u00E0 %username%", L"Controlar que tipos de jogos %username% pode jogar"},
    {1166, L"Which ratings are OK for %username% to play?", L"Selezionare la classificazione giochi adatta per %username%.", L"\u200F\u200F\u06A9\u062F\u0627\u0645 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0628\u0631\u0627\u06CC \u0628\u0627\u0632\u06CC \u062A\u0648\u0633\u0637 %username% \u0645\u0646\u0627\u0633\u0628 \u0627\u0633\u062A\u061F", L"\u00BFCu\u00E1les son las clasificaciones de juegos permitidas para %username%?", L"Quelles classifications de jeux sont autoris\u00E9es pour %username%\u00A0?", L"Que classifica\u00E7\u00F5es s\u00E3o adequadas para %username% jogar?"},
    {1167, L"The ESRB defines these ratings.", L"Classificazioni definite da ESRB.", L"\u0627\u06CC\u0646 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0647\u0627 \u062A\u0648\u0633\u0637 ESRB \u062A\u0639\u0631\u06CC\u0641 \u0645\u06CC \u0634\u0648\u062F.", L"ESRB define estas clasificaciones.", L"Le comit\u00E9 ESRB d\u00E9finit ces classifications.", L"O ESRB (Comit\u00EA americano de classifica\u00E7\u00E3o de entretenimento em software) define essas classifica\u00E7\u00F5es."},
    {1171, L"Control specific games %username% can and can't play", L"Controlla i giochi specifici che %username% pu\u00F2 o non pu\u00F2 eseguire", L"\u200F\u200F\u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u062E\u0627\u0635\u06CC \u06A9\u0647 %username% \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0622\u0646\u0647\u0627 \u0631\u0627 \u0627\u0646\u062C\u0627\u0645 \u062F\u0647\u062F\u060C \u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F", L"Controlar los juegos espec\u00EDficos a los que %username% puede o no puede jugar", L"G\u00E9rer les jeux sp\u00E9cifiques que %username% peut acc\u00E9der ou non", L"Controlar jogos espec\u00EDficos que %username% pode ou n\u00E3o jogar"},
    {1172, L"Allowed ratings:", L"Classificazioni consentite:", L"\u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0647\u0627\u06CC \u0645\u062C\u0627\u0632:", L"Clasificaciones permitidas:", L"Classifications autoris\u00E9es\u00A0:", L"Classifica\u00E7\u00F5es permitidas:"},
    {1174, L"Title/Rating", L"Titolo/Classificazione", L"\u0639\u0646\u0648\u0627\u0646/\u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC", L"T\u00EDtulo o clasificaci\u00F3n", L"Titre/Classification", L"T\u00EDtulo/Classifica\u00E7\u00E3o"},
    {1175, L"Status", L"Stato", L"\u0648\u0636\u0639\u06CC\u062A", L"Estado", L"Statut", L"Status"},
    {1176, L"User rating setting", L"Impostazione classificazione utente", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u06A9\u0627\u0631\u0628\u0631", L"Configuraci\u00F3n de clasificaci\u00F3n de usuario", L"Param\u00E8tre de classification utilisateur", L"Configura\u00E7\u00E3o de classifica\u00E7\u00E3o de usu\u00E1rio"},
    {1177, L"Always allow", L"Consenti sempre", L"\u0647\u0645\u06CC\u0634\u0647 \u0645\u062C\u0627\u0632", L"Permitir siempre", L"Toujours autoriser", L"Permitir sempre"},
    {1178, L"Always block", L"Blocca sempre", L"\u0647\u0645\u06CC\u0634\u0647 \u0645\u0633\u062F\u0648\u062F", L"Bloquear siempre", L"Toujours bloquer", L"Bloquear sempre"},
    {1179, L"Set times when %username% can't use PC at all", L"Imposta gli orari in cui %username% non pu\u00F2 utilizzare il PC", L"\u0632\u0645\u0627\u0646 \u0647\u0627\u06CC\u06CC \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %username% \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0628\u0637\u0648\u0631 \u06A9\u0644\u06CC \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Definir las horas en que %username% no puede usar el equipo", L"D\u00E9finir les horaires auxquels %username% ne peut pas utiliser du tout l\u2019ordinateur", L"Definir os hor\u00E1rios em que %username% n\u00E3o pode usar o computador"},
    {1180, L"Monday", L"Luned\u00EC", L"\u062F\u0648\u0634\u0646\u0628\u0647", L"Lunes", L"Lundi", L"Segunda-feira"},
    {1181, L"Tuesday", L"Marted\u00EC", L"\u0633\u0647 \u0634\u0646\u0628\u0647", L"Martes", L"Mardi", L"Ter\u00E7a-feira"},
    {1182, L"Wednesday", L"Mercoled\u00EC", L"\u0686\u0647\u0627\u0631\u0634\u0646\u0628\u0647", L"Mi\u00E9rcoles", L"Mercredi", L"Quarta-feira"},
    {1183, L"Thursday", L"Gioved\u00EC", L"\u067E\u0646\u062C \u0634\u0646\u0628\u0647", L"Jueves", L"Jeudi", L"Quinta-feira"},
    {1184, L"Friday", L"Venerd\u00EC", L"\u062C\u0645\u0639\u0647", L"Viernes", L"Vendredi", L"Sexta-feira"},
    {1185, L"Saturday", L"Sabato", L"\u0634\u0646\u0628\u0647", L"S\u00E1bado", L"Samedi", L"S\u00E1bado"},
    {1186, L"Sunday", L"Domenica", L"\u06CC\u06A9\u0634\u0646\u0628\u0647", L"Domingo", L"Dimanche", L"Domingo"},
    {1187, L"Midnight (AM)", L"Mezzanotte (AM)", L"\u0646\u06CC\u0645\u0647 \u0634\u0628 (\u0635\u0628\u062D)", L"Medianoche (a.m.)", L"Minuit (matin)", L"Meia-noite (AM)"},
    {1188, L" Noon (PM)", L"Mezzogiorno (PM)", L"\u0638\u0647\u0631 (\u0628\u0639\u062F\u0627\u0632\u0638\u0647\u0631)", L" Mediod\u00EDa (p.m.)", L" Midi (apr\u00E8s-midi)", L" Meio-dia (PM)"},
    {1189, L"Allowed - Unchecked", L"Consentito - Deselezionato", L"\u0645\u062C\u0627\u0632 - \u0628\u062F\u0648\u0646 \u0639\u0644\u0627\u0645\u062A", L"Permitido: sin comprobar", L"Autoris\u00E9 - D\u00E9coch\u00E9", L"Permitido - N\u00E3o Selecionado"},
    {1190, L"Allowed ", L"Consentita ", L"\u0645\u062C\u0627\u0632 ", L"Permitido ", L"Autoris\u00E9 ", L"Permitido "},
    {1191, L"Blocked - Checked", L"Bloccato - Selezionato", L"\u0645\u0633\u062F\u0648\u062F - \u0639\u0644\u0627\u0645\u062A \u062E\u0648\u0631\u062F\u0647", L"Bloqueado: comprobado", L"Bloqu\u00E9 - Coch\u00E9", L"Bloqueado - Selecionado"},
    {1192, L"Blocked ", L"Bloccata ", L"\u0645\u0633\u062F\u0648\u062F \u0634\u062F\u0647 ", L"Bloqueado ", L"Bloqu\u00E9 ", L"Bloqueado "},
    {1197, L"Which rating system do you want to use?", L"Selezionare il sistema di classificazione da utilizzare.", L"\u200F\u200F\u0645\u06CC \u062E\u0648\u0627\u0647\u06CC\u062F \u0627\u0632 \u06A9\u062F\u0627\u0645 \u0633\u06CC\u0633\u062A\u0645 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u06CC\u062F\u061F", L"\u00BFQu\u00E9 sistema de clasificaci\u00F3n desea usar?", L"Quel syst\u00E8me de classification voulez-vous utiliser\u00A0?", L"Que sistema de classifica\u00E7\u00E3o voc\u00EA deseja usar?"},
    {1198, L"This rating system will apply to all users.", L"Questo sistema di classificazione verr\u00E0 applicato a tutti gli utenti.", L"\u200F\u200F\u0627\u06CC\u0646 \u0633\u06CC\u0633\u062A\u0645 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0628\u0631\u0627\u06CC \u0647\u0645\u0647 \u06A9\u0627\u0631\u0628\u0631\u0627\u0646 \u0627\u0639\u0645\u0627\u0644 \u062E\u0648\u0627\u0647\u062F \u0634\u062F.", L"Este sistema de clasificaci\u00F3n se aplicar\u00E1 a todos los usuarios.", L"Ce syst\u00E8me de classification s'appliquera \u00E0 tous les utilisateurs.", L"Este sistema de classifica\u00E7\u00E3o se aplicar\u00E1 a todos os usu\u00E1rios."},
    {1200, L"Which apps can %username% use", L"Specificare le app che %username% pu\u00F2 utilizzare.", L"%username% \u0627\u0632 \u06A9\u062F\u0627\u0645 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Las aplicaciones que %username% puede usar", L"Applications utilisables par %username%", L"Quais aplicativos %username% pode usar"},
    {1201, L"Use all apps", L"Tutte le app", L"\u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u0647\u0645\u0647 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627", L"Usar todas las aplicaciones", L"Utiliser toutes les applications", L"Usar todos os aplicativos"},
    {1202, L"Only use the apps I allow in this list", L"Solo le app presenti nell'elenco", L"\u0641\u0642\u0637 \u0627\u0632 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0634\u0648\u062F \u06A9\u0647 \u0645\u0646 \u062F\u0631 \u0627\u06CC\u0646 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632 \u06A9\u0631\u062F\u0647 \u0627\u0645", L"Usar solo las aplicaciones permitidas en esta lista", L"N\u2019utiliser que les applications que j\u2019autorise dans cette liste", L"Usar somente os aplicativos permitidos nesta lista"},
    {1203, L"Check the apps that can be used:", L"Selezionare le app che possono essere utilizzate:", L"\u200F\u200F\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC\u06CC \u06A9\u0647 \u0642\u0627\u0628\u0644 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0647\u0633\u062A\u0646\u062F \u0631\u0627 \u0628\u0631\u0631\u0633\u06CC \u06A9\u0646\u06CC\u062F:", L"Activar las aplicaciones que se pueden usar:", L"Cochez les applications autoris\u00E9es\u00A0:", L"Verificar quais aplicativos podem ser usados:"},
    {1204, L"Add an app to this list:", L"Aggiungere un'app all'elenco:", L"\u0628\u0631\u0646\u0627\u0645\u0647 \u0627\u06CC \u0631\u0627 \u0628\u0647 \u0627\u06CC\u0646 \u0644\u06CC\u0633\u062A \u0627\u0636\u0627\u0641\u0647 \u06A9\u0646\u06CC\u062F:", L"Agregar una aplicaci\u00F3n a esta lista:", L"Ajoutez une application \u00E0 cette liste\u00A0:", L"Adicionar um aplicativo a esta lista:"},
    {1205, L"Browse...", L"Sfoglia...", L"\u0645\u0631\u0648\u0631...", L"Examinar...", L"Parcourir...", L"Procurar..."},
    {1206, L"Check all", L"Seleziona tutto", L"\u0639\u0644\u0627\u0645\u062A \u0632\u062F\u0646 \u0647\u0645\u0647", L"Activar todo", L"Tout cocher", L"Selecionar tudo"},
    {1207, L"Uncheck all", L"Deseleziona tutto", L"\u062D\u0630\u0641 \u0639\u0644\u0627\u0645\u062A \u0647\u0645\u0647", L"Desactivar todo", L"Tout d\u00E9cocher", L"Desmarcar tudo"},
    {1208, L"Web filter settings", L"Impostazioni filtro Web", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628", L"Configuraci\u00F3n del filtro web", L"Param\u00E8tres de filtrage Web", L"Configura\u00E7\u00F5es de filtro de Web"},
    {1214, L"Allow or block specific websites", L"Consenti o blocca siti Web specifici", L"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u062E\u0627\u0635", L"Permitir o bloquear sitios web espec\u00EDficos", L"Autoriser ou bloquer des sites Web sp\u00E9cifiques", L"Permitir ou bloquear sites espec\u00EDficos"},
    {1215, L"Enter a website to allow or block.", L"Immettere un sito Web da consentire o bloccare.", L"\u06CC\u06A9 \u0633\u0627\u06CC\u062A \u0631\u0627 \u0628\u0631\u0627\u06CC \u0645\u062C\u0627\u0632 \u06A9\u0631\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0648\u0627\u0631\u062F \u06A9\u0646\u06CC\u062F.", L"Escriba una sitio web que desee permitir o bloquear.", L"Entrez un site Web \u00E0 autoriser ou \u00E0 bloquer.", L"Insira um site que ser\u00E1 permitido ou bloqueado."},
    {1217, L"Allow", L"Consenti", L"\u0645\u062C\u0627\u0632 \u0627\u0633\u062A", L"Permitir", L"Autoriser", L"Permitir"},
    {1218, L"Block", L"Blocca", L"\u0645\u0633\u062F\u0648\u062F \u0627\u0633\u062A", L"Bloquear", L"Bloquer", L"Bloquear"},
    {1219, L"Allowed websites:", L"Siti Web consentiti:", L"\u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0645\u062C\u0627\u0632:", L"Sitios web permitidos:", L"Sites Web autoris\u00E9s\u00A0:", L"Sites permitidos:"},
    {1220, L"Blocked websites:", L"Siti Web bloccati:", L"\u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0645\u0633\u062F\u0648\u062F \u0634\u062F\u0647:", L"Sitios web bloqueados:", L"Sites Web bloqu\u00E9s\u00A0:", L"Sites bloqueados:"},
    {1221, L"Remove", L"Rimuovi", L"\u062D\u0630\u0641 \u06A9\u0631\u062F\u0646", L"Quitar", L"Supprimer", L"Remover"},
    {1230, L"Choose a web restriction level:", L"Scegliere un livello di restrizione Web:", L"\u06CC\u06A9 \u0633\u0637\u062D \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0648\u0628 \u0631\u0627 \u0627\u0646\u062A\u062E\u0627\u0628 \u06A9\u0646\u06CC\u062F:", L"Elegir un nivel de restricci\u00F3n web:", L"Choisissez un niveau de restriction d\u2019acc\u00E8s au Web\u00A0:", L"Selecione um n\u00EDvel de restri\u00E7\u00E3o da web:"},
    {1231, L"Allow list only", L"Solo elenco consentiti", L"\u0641\u0642\u0637 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632", L"Solo la lista de permitidos", L"Liste verte uniquement", L"Somente lista de permiss\u00F5es"},
    {1232, L"Designed for children", L"Adatto ai bambini", L"\u0637\u0631\u0627\u062D\u06CC \u0634\u062F\u0647 \u0628\u0631\u0627\u06CC \u06A9\u0648\u062F\u06A9\u0627\u0646", L"Dise\u00F1ado para menores", L"Destin\u00E9s aux enfants", L"Criado para crian\u00E7as"},
    {1233, L"General interest", L"Interesse generale", L"\u0639\u0644\u0627\u0642\u0647 \u0639\u0645\u0648\u0645\u06CC", L"Inter\u00E9s general", L"Grand public", L"Interesses gerais"},
    {1234, L"Online communication", L"Comunicazione online", L"\u0627\u0631\u062A\u0628\u0627\u0637 \u0622\u0646\u0644\u0627\u06CC\u0646", L"Comunicaci\u00F3n en l\u00EDnea", L"Communication en ligne", L"Comunica\u00E7\u00E3o online"},
    {1235, L"Warn on adult", L"Avvisa in caso di contenuto per adulti", L"\u0647\u0634\u062F\u0627\u0631 \u0645\u062D\u062A\u0648\u0627\u06CC \u0628\u0632\u0631\u06AF\u0633\u0627\u0644\u0627\u0646", L"Advertir de contenido para adultos", L"Avertir en cas de contenu pour adultes", L"Aviso de conte\u00FAdo para adulto"},
    {1237, L"The child can view websites on the Allow list and websites designed for children. Adult sites are blocked.", L"Il bambino pu\u00F2 visualizzare i siti Web inclusi nell'elenco degli indirizzi Web consentiti, nonch\u00E9 i siti Web adatti ai bambini. I siti per adulti sono bloccati.", L"\u06A9\u0648\u062F\u06A9 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0645\u0648\u062C\u0648\u062F \u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632 \u0648 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0637\u0631\u0627\u062D\u06CC \u0634\u062F\u0647 \u0628\u0631\u0627\u06CC \u06A9\u0648\u062F\u06A9\u0627\u0646 \u0631\u0627 \u0628\u0628\u06CC\u0646\u0646\u062F. \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u062F\u0627\u0631\u0627\u06CC \u0645\u062D\u062A\u0648\u0627\u06CC \u0628\u0632\u0631\u06AF\u0633\u0627\u0644\u0627\u0646 \u0645\u0633\u062F\u0648\u062F \u0645\u06CC \u0634\u0648\u062F.", L"El menor puede ver los sitios web incluidos en la lista de permitidos y en sitios web dise\u00F1ados para menores. Se bloquean los sitios para p\u00FAblico adulto.", L"L\u2019enfant peut acc\u00E9der aux sites Web de la liste verte et aux sites Web destin\u00E9s aux enfants. Les sites pour adultes sont bloqu\u00E9s.", L"As crian\u00E7as podem ver sites da lista de Permiss\u00F5es e os criados para crian\u00E7as. Os sites para adultos s\u00E3o bloqueados."},
    {1245, L"Block file downloads", L"Blocca download di file", L"\u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u067E\u0631\u0648\u0646\u062F\u0647 \u0647\u0627\u06CC \u0628\u0627\u0631\u06AF\u06CC\u0631\u06CC \u0634\u062F\u0647", L"Bloquear descargas de archivos", L"Bloquer les t\u00E9l\u00E9chargements de fichiers", L"Bloquear downloads de arquivo"},
    {1266, L"Web Browsing", L"Esplorazione Web", L"\u0645\u0631\u0648\u0631 \u062F\u0631 \u0648\u0628", L"Exploraci\u00F3n web", L"Navigation sur le Web", L"Navega\u00E7\u00E3o na Web"},
    {1267, L"Top 10 Websites Visited", L"10 siti Web pi\u00F9 visitati", L"10 \u0633\u0627\u06CC\u062A \u0628\u0631\u062A\u0631 \u0645\u0634\u0627\u0647\u062F\u0647 \u0634\u062F\u0647", L"Los 10 sitios web m\u00E1s visitados", L"10 premiers sites Web visit\u00E9s", L"10 Principais Sites Visitados"},
    {1268, L"1. ", L"1. ", L"1. ", L"1. ", L"1. ", L"1. "},
    {1269, L"2. ", L"2. ", L"2. ", L"2. ", L"2. ", L"2. "},
    {1270, L"3. ", L"3. ", L"3. ", L"3. ", L"3. ", L"3. "},
    {1271, L"4. ", L"4. ", L"4. ", L"4. ", L"4. ", L"4. "},
    {1272, L"5. ", L"5. ", L"5. ", L"5. ", L"5. ", L"5. "},
    {1273, L"6. ", L"6. ", L"6. ", L"6. ", L"6. ", L"6. "},
    {1274, L"7. ", L"7. ", L"7. ", L"7. ", L"7. ", L"7. "},
    {1275, L"8. ", L"8. ", L"8. ", L"8. ", L"8. ", L"8. "},
    {1276, L"9. ", L"9. ", L"9. ", L"9. ", L"9. ", L"9. "},
    {1277, L"10.", L"10.", L"10.", L"10.", L"10.", L"10."},
    {1278, L"Most Recent 10 Websites Blocked", L"Ultimi 10 siti Web bloccati", L"\u062C\u062F\u06CC\u062F\u062A\u0631\u06CC\u0646 10 \u0633\u0627\u06CC\u062A\u06CC \u06A9\u0647 \u0627\u062E\u06CC\u0631\u0627\u064B \u0645\u0633\u062F\u0648\u062F \u0634\u062F\u0647 \u0627\u0646\u062F", L"Los 10 sitios web bloqueados m\u00E1s recientes", L"10 sites Web bloqu\u00E9s les plus r\u00E9cents", L"10 Mais Recentes Sites Bloqueados"},
    {1279, L"File Downloads", L"Download di file", L"\u067E\u0631\u0648\u0646\u062F\u0647 \u0647\u0627\u06CC \u0628\u0627\u0631\u06AF\u06CC\u0631\u06CC \u0634\u062F\u0647", L"Descargas de archivos", L"T\u00E9l\u00E9chargements de fichiers", L"Downloads de Arquivo"},
    {1281, L"File Downloads Blocked", L"Download di file bloccati", L"\u067E\u0631\u0648\u0646\u062F\u0647 \u0647\u0627\u06CC \u0628\u0627\u0631\u06AF\u06CC\u0631\u06CC \u0634\u062F\u0647 \u0645\u0633\u062F\u0648\u062F \u0634\u062F\u0646\u062F", L"Descargas de archivos bloqueadas", L"T\u00E9l\u00E9chargements de fichiers bloqu\u00E9s", L"Downloads de Arquivos Bloqueados"},
    {1283, L"Web Overrides", L"Sostituzioni Web", L"\u0631\u0648\u0646\u0648\u06CC\u0633\u06CC \u0647\u0627\u06CC \u0648\u0628", L"Invalidaciones de sitios web", L"Substitutions Web", L"Substitui\u00E7\u00F5es na Web"},
    {1285, L"System", L"Sistema", L"\u0633\u06CC\u0633\u062A\u0645", L"Sistema", L"Syst\u00E8me", L"Sistema"},
    {1286, L"Logon Times", L"Orari di accesso", L"\u0632\u0645\u0627\u0646 \u0648\u0631\u0648\u062F \u0628\u0647 \u0633\u06CC\u0633\u062A\u0645", L"Horas de inicio de sesi\u00F3n", L"Temps de connexion", L"Hor\u00E1rios de Logon"},
    {1288, L"Applications Run", L"Esecuzione di applicazioni", L"\u0627\u062C\u0631\u0627\u06CC \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627", L"Aplicaciones ejecutadas", L"Applications ex\u00E9cut\u00E9es", L"Execu\u00E7\u00E3o de Aplicativos"},
    {1289, L"Applications", L"Applicazioni", L"\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627", L"Aplicaciones", L"Applications", L"Aplicativos"},
    {1290, L"Application Overrides", L"Sostituzioni applicazioni", L"\u0631\u0648\u0646\u0648\u06CC\u0633\u06CC \u0647\u0627\u06CC \u0628\u0631\u0646\u0627\u0645\u0647", L"Invalidaciones de aplicaciones", L"Substitutions d\u2019applications", L"Substitui\u00E7\u00F5es de Aplicativos"},
    {1292, L"Gaming", L"Gioco", L"\u0628\u0627\u0632\u06CC", L"Juegos", L"Jeux", L"Jogos"},
    {1293, L"Games Played", L"Partite giocate", L"\u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u0627\u0646\u062C\u0627\u0645 \u0634\u062F\u0647", L"Juegos jugados", L"Parties jou\u00E9es", L"Partidas Jogadas"},
    {1307, L"General System Information", L"Informazioni di sistema generali", L"\u0627\u0637\u0644\u0627\u0639\u0627\u062A \u06A9\u0644\u06CC \u0633\u06CC\u0633\u062A\u0645", L"Informaci\u00F3n general del sistema", L"Informations syst\u00E8me g\u00E9n\u00E9rales", L"Informa\u00E7\u00F5es Gerais do Sistema"},
    {1308, L"Changes to Parental Controls Settings", L"Passa a impostazioni di Controllo genitori", L"\u0628\u0647 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u06A9\u0646\u062A\u0631\u0644 \u0647\u0627\u06CC \u0648\u0627\u0644\u062F\u06CC\u0646 \u062A\u063A\u06CC\u06CC\u0631 \u0645\u06CC \u062F\u0647\u062F", L"Cambios en la configuraci\u00F3n de Control parental", L"Modifiez les param\u00E8tres de contr\u00F4le parental.", L"Alterar as Configura\u00E7\u00F5es de Controles dos Pais"},
    {1309, L"Account Changes", L"Modifiche agli account", L"\u062A\u063A\u06CC\u06CC\u0631\u0627\u062A \u062D\u0633\u0627\u0628", L"Cambios en la cuenta", L"Modifications du compte", L"Altera\u00E7\u00F5es de conta"},
    {1310, L"System Clock Changes", L"Modifiche al clock di sistema", L"\u062A\u063A\u06CC\u06CC\u0631\u0627\u062A \u0633\u0627\u0639\u062A \u0633\u06CC\u0633\u062A\u0645", L"Cambios en el reloj del sistema", L"Changements de l\u2019heure syst\u00E8me", L"Altera\u00E7\u00F5es no Rel\u00F3gio do Sistema"},
    {1311, L"Failed Logon Attempts", L"Tentativi di accesso non riusciti", L"\u062A\u0639\u062F\u0627\u062F \u062F\u0641\u0639\u0627\u062A \u0646\u0627\u0645\u0648\u0641\u0642 \u062F\u0631 \u0648\u0631\u0648\u062F \u0628\u0647 \u0633\u06CC\u0633\u062A\u0645", L"Errores de intentos de inicio de sesi\u00F3n", L"Tentatives de connexion infructueuses", L"Tentativas de Logon sem \u00CAxito"},
    {1319, L"User Summary", L"Riepilogo utente", L"\u062E\u0644\u0627\u0635\u0647 \u06A9\u0627\u0631\u0628\u0631", L"Resumen de usuario", L"R\u00E9sum\u00E9 utilisateur", L"Resumo do Usu\u00E1rio"},
    {1322, L"More settings:", L"Altre impostazioni:", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0628\u06CC\u0634\u062A\u0631:", L"M\u00E1s configuraciones:", L"Autres param\u00E8tres\u00A0:", L"Mais configura\u00E7\u00F5es:"},
    {1323, L"Web Address Edit Control", L"Controllo modifica indirizzi Web", L"\u06A9\u0646\u062A\u0631\u0644 \u0648\u06CC\u0631\u0627\u06CC\u0634 \u0622\u062F\u0631\u0633 \u0648\u0628", L"Control de edici\u00F3n de direcciones web", L"Contr\u00F4le de modification d\u2019adresse Web", L"Controle de Edi\u00E7\u00E3o de Endere\u00E7os da Web"},
    {1329, L"The child can view websites on the Allow list, websites designed for children, and websites from the general interest, social networking, web chat, and web mail categories. Adult sites are blocked.", L"Il bambino pu\u00F2 visualizzare i siti Web inclusi nell'elenco di indirizzi Web consentiti, quelli adatti ai bambini, nonch\u00E9 quelli appartenenti alle categorie di interesse generale, social network, chat Web e posta elettronica Web. I siti per adulti sono bloccati.", L"\u06A9\u0648\u062F\u06A9 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0645\u0648\u062C\u0648\u062F \u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632 \u0648 \u0647\u0645\u0686\u0646\u06CC\u0646 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0637\u0631\u0627\u062D\u06CC \u0634\u062F\u0647 \u0628\u0631\u0627\u06CC \u06A9\u0648\u062F\u06A9\u0627\u0646\u060C \u0639\u0644\u0627\u0642\u0647 \u0647\u0627\u06CC \u0639\u0645\u0648\u0645\u06CC\u060C \u0634\u0628\u06A9\u0647 \u0627\u062C\u062A\u0645\u0627\u0639\u06CC\u060C \u0686\u062A \u0648\u0628 \u0648 \u0646\u0627\u0645\u0647 \u0648\u0628 \u0631\u0627 \u0628\u0628\u06CC\u0646\u062F. \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u062F\u0627\u0631\u0627\u06CC \u0645\u062D\u062A\u0648\u0627\u06CC \u0628\u0632\u0631\u06AF\u0633\u0627\u0644\u0627\u0646 \u0645\u0633\u062F\u0648\u062F \u0645\u06CC \u0634\u0648\u062F.", L"El menor puede ver los sitios web incluidos en la lista de permitidos y los dise\u00F1ados para menores, adem\u00E1s de los sitios web de las categor\u00EDas de inter\u00E9s general, redes sociales, chat en web y correo web. Se bloquean los sitios para p\u00FAblico adulto.", L"L\u2019enfant peut acc\u00E9der aux sites Web de la liste verte, aux sites Web destin\u00E9s aux enfants ou au grand public, aux r\u00E9seaux sociaux, \u00E0 la discussion Web et \u00E0 la messagerie Web. Les sites pour adultes sont bloqu\u00E9s.", L"As crian\u00E7as podem ver sites da lista de Permiss\u00F5es e das categorias de sites criados para crian\u00E7as, sites de interesses gerais, rede social, webchat e webmail. Os sites para adultos s\u00E3o bloqueados."},
    {1335, L"Current settings:", L"Impostazioni correnti:", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0641\u0639\u0644\u06CC:", L"Configuraci\u00F3n actual:", L"Param\u00E8tres actifs\u00A0:", L"Configura\u00E7\u00F5es atuais:"},
    {1338, L"Switch back to the Family Safety web filter", L"Reimposta il filtro Web di Sicurezza famiglia", L"\u0628\u0647 \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0628\u0627\u0632\u06AF\u0631\u062F\u06CC\u062F", L"Volver al filtro web de Protecci\u00F3n infantil", L"R\u00E9tablir le filtre Web du contr\u00F4le parental", L"Redefinir para o filtro de Web de Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {1339, L"Switch", L"Cambia", L"\u062C\u0627\u0628\u062C\u0627\u06CC\u06CC", L"Cambiar", L"Changer", L"Alternar"},
    {1341, L"Controls", L"Controlli", L"\u06A9\u0646\u062A\u0631\u0644 \u0647\u0627", L"Controles", L"Contr\u00F4les", L"Controles"},
    {1345, L"Provider", L"Provider", L"\u0627\u0631\u0627\u0626\u0647 \u062F\u0647\u0646\u062F\u0647", L"Proveedor", L"Fournisseur", L"Provedor"},
    {1346, L"Select a provider:", L"Seleziona un provider:", L"\u06CC\u06A9 \u0627\u0631\u0627\u0626\u0647 \u062F\u0647\u0646\u062F\u0647 \u0631\u0627 \u0627\u0646\u062A\u062E\u0627\u0628 \u06A9\u0646\u06CC\u062F:", L"Seleccionar un proveedor:", L"S\u00E9lectionnez un fournisseur :", L"Selecione um provedor:"},
    {1352, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1353, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1354, L"Jump", L"Passa a", L"\u067E\u0631\u0634", L"Saltar", L"Passer", L"Saltar"},
    {1359, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1376, L"The child can view websites on the Allow list, websites designed for children, and websites from the general interest category. Adult sites are blocked.", L"Il bambino pu\u00F2 visualizzare i siti Web inclusi nell'elenco di indirizzi Web consentiti, quelli adatti ai bambini e quelli appartenenti alla categoria di interesse generale. I siti per adulti sono bloccati.", L"\u06A9\u0648\u062F\u06A9 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0645\u0648\u062C\u0648\u062F \u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632 \u0648 \u0647\u0645\u0686\u0646\u06CC\u0646 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0637\u0631\u0627\u062D\u06CC \u0634\u062F\u0647 \u0628\u0631\u0627\u06CC \u06A9\u0648\u062F\u06A9\u0627\u0646 \u0648 \u0639\u0644\u0627\u0642\u0647 \u0647\u0627\u06CC \u0639\u0645\u0648\u0645\u06CC \u0631\u0627 \u0628\u0628\u06CC\u0646\u0646\u062F. \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u062F\u0627\u0631\u0627\u06CC \u0645\u062D\u062A\u0648\u0627\u06CC \u0628\u0632\u0631\u06AF\u0633\u0627\u0644\u0627\u0646 \u0645\u0633\u062F\u0648\u062F \u0645\u06CC \u0634\u0648\u062F.", L"El menor puede ver los sitios web incluidos en la lista de permitidos y los dise\u00F1ados para menores, adem\u00E1s de los sitios web de la categor\u00EDa de inter\u00E9s general. Se bloquean los sitios para p\u00FAblico adulto.", L"L\u2019enfant peut acc\u00E9der aux sites Web de la liste verte et aux sites Web destin\u00E9s aux enfants ou au grand public. Les sites pour adultes sont bloqu\u00E9s.", L"As crian\u00E7as podem ver sites da lista de Permiss\u00F5es e das categorias de sites criados para crian\u00E7as e sites de interesses gerais. Os sites para adultos s\u00E3o bloqueados."},
    {1383, L"The child can view all websites, but receives a warning when a site contains suspected adult content.", L"Il bambino pu\u00F2 visualizzare tutti i siti Web, ma riceve un avviso quando il sito contiene materiale potenzialmente destinato a un pubblico adulto.", L"\u06A9\u0648\u062F\u06A9\u0627\u0646 \u0645\u06CC \u062A\u0648\u0627\u0646\u0646\u062F \u0647\u0645\u0647 \u0633\u0627\u06CC\u062A \u0647\u0627 \u0631\u0627 \u0645\u0634\u0627\u0647\u062F\u0647 \u06A9\u0646\u0646\u062F \u0627\u0645\u0627 \u0627\u06AF\u0631 \u0633\u0627\u06CC\u062A\u06CC \u0627\u062D\u062A\u0645\u0627\u0644\u0627\u064B \u062F\u0627\u0631\u0627\u06CC \u0645\u062D\u062A\u0648\u0627\u06CC \u0645\u0631\u0628\u0648\u0637 \u0628\u0647 \u0628\u0632\u0631\u06AF\u0633\u0627\u0644\u0627\u0646 \u0628\u0627\u0634\u062F\u060C \u06CC\u06A9 \u0647\u0634\u062F\u0627\u0631 \u062F\u0631\u06CC\u0627\u0641\u062A \u0645\u06CC \u06A9\u0646\u0646\u062F.", L"El menor puede ver todos los sitios web, pero recibe una advertencia cuando un sitio contiene contenido supuestamente dirigido a adultos.", L"L\u2019enfant peut acc\u00E9der \u00E0 tous les sites Web, mais un avertissement s\u2019affiche lorsque le contenu d\u2019un site semble \u00EAtre r\u00E9serv\u00E9 aux adultes.", L"As crian\u00E7as podem ver todos os sites, mas recebem um aviso quando um site tem conte\u00FAdo suspeito para adulto."},
    {1392, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1398, L"Manage settings on the Family Safety website", L"Gestisci impostazioni nel sito Web di Sicurezza famiglia", L"\u0645\u062F\u06CC\u0631\u06CC\u062A \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0645\u0648\u062C\u0648\u062F \u062F\u0631 \u0633\u0627\u06CC\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647", L"Administrar configuraci\u00F3n en el sitio web de Protecci\u00F3n infantil", L"G\u00E9rer les param\u00E8tres sur le site Web Contr\u00F4le parental", L"Gerenciar configura\u00E7\u00F5es no site Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {1400, L"You can manage this PC's Family Safety settings on the website.", L"\u00C8 possibile gestire le impostazioni di Sicurezza famiglia del PC dal sito Web.", L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0627\u06CC\u0646 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0631\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u06CC\u062F \u062F\u0631 \u0633\u0627\u06CC\u062A \u0645\u062F\u06CC\u0631\u06CC\u062A \u06A9\u0646\u06CC\u062F.", L"Puede administrar la configuraci\u00F3n de Protecci\u00F3n infantil de este equipo en el sitio web.", L"Vous pouvez g\u00E9rer les param\u00E8tres du contr\u00F4le parental de cet ordinateur sur le site Web.", L"Voc\u00EA pode gerenciar as configura\u00E7\u00F5es de Prote\u00E7\u00E3o para a Fam\u00EDlia deste computador no site."},
    {1401, L"To manage this PC's Family Safety settings locally (only on this PC), <a>remove it from the Family Safety website.</a>", L"Per gestire le impostazioni di Sicurezza famiglia del PC localmente (solo sul PC in uso), <a>rimuoverle dal sito Web di Sicurezza famiglia</a>.", L"\u0628\u0631\u0627\u06CC \u0645\u062F\u06CC\u0631\u06CC\u062A \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0627\u06CC\u0646 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0628\u0635\u0648\u0631\u062A \u0645\u062D\u0644\u06CC (\u0641\u0642\u0637 \u062F\u0631 \u0627\u06CC\u0646 \u0631\u0627\u06CC\u0627\u0646\u0647)\u060C <a>\u0622\u0646 \u0631\u0627 \u0627\u0632 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u062D\u0630\u0641 \u06A9\u0646\u06CC\u062F.</a>", L"Para administrar la configuraci\u00F3n de Protecci\u00F3n infantil de este equipo localmente (solo en este equipo), <a>qu\u00EDtela del sitio web de Protecci\u00F3n infantil.</a>", L"Pour g\u00E9rer localement (uniquement sur cet ordinateur) les param\u00E8tres du contr\u00F4le parental de cet ordinateur, <a>retirez-le du site Web du contr\u00F4le parental</a>.", L"Para gerenciar as configura\u00E7\u00F5es de Prote\u00E7\u00E3o para a Fam\u00EDlia deste computador localmente (somente neste computador), <a>remova-as do site de Prote\u00E7\u00E3o para a Fam\u00EDlia.</a>"},
    {1403, L"Remove machine from the website", L"Rimuovi computer da sito Web", L"\u062D\u0630\u0641 \u062F\u0633\u062A\u06AF\u0627\u0647 \u0627\u0632 \u0648\u0628 \u0633\u0627\u06CC\u062A", L"Quitar equipo del sitio web", L"Retirer l\u2019ordinateur du site Web", L"Remover m\u00E1quina do site"},
    {1405, L"Toggle Expando", L"Espandi/Comprimi", L"\u06AF\u0633\u062A\u0631\u0634 \u062C\u0627\u0628\u062C\u0627\u06CC\u06CC", L"Alternar expando", L"Basculer Expando", L"Alternar Expandir"},
    {1406, L"Show or hide extra information/elements", L"Mostra o nasconde informazioni o elementi aggiuntivi", L"\u0627\u0637\u0644\u0627\u0639\u0627\u062A/\u0639\u0646\u0627\u062B\u0631 \u0628\u06CC\u0634\u062A\u0631 \u0631\u0627 \u0646\u0645\u0627\u06CC\u0634 \u062F\u0627\u062F\u0647 \u06CC\u0627 \u067E\u0646\u0647\u0627\u0646 \u06A9\u0646\u06CC\u062F", L"Mostrar u ocultar informaci\u00F3n o elementos adicionales", L"Afficher ou masquer les informations/\u00E9l\u00E9ments suppl\u00E9mentaires", L"Mostrar ou ocultar informa\u00E7\u00F5es/elementos extras"},
    {1409, L"No games installed", L"Nessun gioco installato", L"\u0647\u06CC\u0686 \u0628\u0627\u0632\u06CC \u0646\u0635\u0628 \u0646\u0634\u062F\u0647 \u0627\u0633\u062A", L"No hay juegos instalados", L"Aucun jeu install\u00E9", L"Nenhum jogo instalado"},
    {1410, L"12", L"12", L"12", L"12", L"12", L"12"},
    {1411, L" 1", L" 1", L" 1", L" 1", L" 1", L" 1"},
    {1412, L" 2", L" 2", L" 2", L" 2", L" 2", L" 2"},
    {1413, L" 3", L" 3", L" 3", L" 3", L" 3", L" 3"},
    {1414, L" 4", L" 4", L" 4", L" 4", L" 4", L" 4"},
    {1415, L" 5", L" 5", L" 5", L" 5", L" 5", L" 5"},
    {1416, L" 6", L" 6", L" 6", L" 6", L" 6", L" 6"},
    {1417, L" 7", L" 7", L" 7", L" 7", L" 7", L" 7"},
    {1418, L" 8", L" 8", L" 8", L" 8", L" 8", L" 8"},
    {1419, L" 9", L" 9", L" 9", L" 9", L" 9", L" 9"},
    {1420, L"10", L"10", L"10", L"10", L"10", L"10"},
    {1421, L"11", L"11", L"11", L"11", L"11", L"11"},
    {1422, L"12", L"12", L"12", L"12", L"12", L"12"},
    {1423, L" 1", L" 1", L" 1", L" 1", L" 1", L" 1"},
    {1424, L" 2", L" 2", L" 2", L" 2", L" 2", L" 2"},
    {1425, L" 3", L" 3", L" 3", L" 3", L" 3", L" 3"},
    {1426, L" 4", L" 4", L" 4", L" 4", L" 4", L" 4"},
    {1427, L" 5", L" 5", L" 5", L" 5", L" 5", L" 5"},
    {1428, L" 6", L" 6", L" 6", L" 6", L" 6", L" 6"},
    {1429, L" 7", L" 7", L" 7", L" 7", L" 7", L" 7"},
    {1430, L" 8", L" 8", L" 8", L" 8", L" 8", L" 8"},
    {1431, L" 9", L" 9", L" 9", L" 9", L" 9", L" 9"},
    {1432, L"10", L"10", L"10", L"10", L"10", L"10"},
    {1433, L"11", L"11", L"11", L"11", L"11", L"11"},
    {1435, L"No name", L"Senza nome", L"\u0628\u062F\u0648\u0646 \u0646\u0627\u0645", L"Sin nombre", L"Sans nom", L"Sem nome"},
    {1436, L"No website", L"Nessun sito Web", L"\u0628\u062F\u0648\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A", L"Sin sitio web", L"Absence de site Web", L"Nenhum site"},
    {1437, L"No name", L"Senza nome", L"\u0628\u062F\u0648\u0646 \u0646\u0627\u0645", L"Sin nombre", L"Sans nom", L"Sem nome"},
    {1449, L"Set the number of hours %username% can use the PC a day", L"Impostare il numero di ore al giorno per cui %username% pu\u00F2 utilizzare il PC", L"\u062A\u0639\u062F\u0627\u062F \u0633\u0627\u0639\u062A \u0647\u0627\u06CC\u06CC \u06A9\u0647 %username% \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u062F\u0631 \u0631\u0648\u0632 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u06CC\u062F", L"Definir el n\u00FAmero de horas al d\u00EDa que %username% puede usar el equipo", L"D\u00E9finir le nombre d\u2019heures par jour o\u00F9 %username% peut utiliser l\u2019ordinateur", L"Definir o n\u00FAmero de horas em que %username% pode usar o computador por dia"},
    {1450, L"Set time allowance", L"Imposta tempo consentito", L"\u062A\u0646\u0638\u06CC\u0645 \u0632\u0645\u0627\u0646 \u0645\u062C\u0627\u0632", L"Establecer tiempo permitido", L"D\u00E9finir la dur\u00E9e autoris\u00E9e", L"Definir tempo permitido"},
    {1451, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1452, L"Set curfew", L"Imposta orario non consentito", L"\u062A\u0646\u0638\u06CC\u0645 \u0632\u0645\u0627\u0646 \u0645\u0645\u0646\u0648\u0639\u0647", L"Establecer horario restringido", L"D\u00E9finir les horaires non autoris\u00E9s", L"Definir restri\u00E7\u00E3o de hor\u00E1rio"},
    {1453, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1454, L"Set the time of day %username% can use the PC", L"Impostare l'ora del giorno in cui %username% pu\u00F2 utilizzare il PC", L"\u0633\u0627\u0639\u062A\u06CC \u0627\u0632 \u0631\u0648\u0632 \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %username% \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Definir la hora del d\u00EDa en que %username% puede usar el equipo", L"D\u00E9finir le moment de la journ\u00E9e o\u00F9 %username% peut utiliser l\u2019ordinateur", L"Definir a hora do dia em que %username% pode usar o computador"},
    {1455, L"Which websites can %username% view?", L"Specificare i siti Web che possono essere visualizzati da %username%.", L"%username% \u06A9\u062F\u0627\u0645 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627 \u0631\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0645\u0634\u0627\u0647\u062F\u0647 \u06A9\u0646\u062F\u061F", L"\u00BFQu\u00E9 sitios web puede ver %username%?", L"Quels sites Web %username% peut-il/elle afficher\u00A0?", L"Quais sites %username% pode exibir?"},
    {1457, L"%username% can use all websites", L"%username% pu\u00F2 utilizzare tutti i siti Web", L"%username% \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u062A\u0645\u0627\u0645 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%username% puede usar todos los sitios web", L"%username% peut utiliser tous les sites Web", L"%username% pode usar todos os sites"},
    {1458, L"%username% can only use the websites I allow", L"%username% pu\u00F2 utilizzare solo i siti Web consentiti", L"%username% \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F \u06A9\u0647 \u0645\u0646 \u0645\u062C\u0627\u0632 \u0645\u06CC \u06A9\u0646\u0645", L"%username% solo puede usar los sitios web que yo permita", L"%username% peut utiliser uniquement les sites Web que j\u2019autorise", L"%username% pode usar somente os sites permitidos"},
    {1465, L"%username% can play all games and view all Windows Store apps", L"%username% pu\u00F2 utilizzare tutti i giochi e tutte le app di Windows Store", L"%username% \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062A\u0645\u0627\u0645 \u0628\u0627\u0632\u06CC \u0647\u0627 \u0631\u0627 \u0627\u0646\u062C\u0627\u0645 \u062F\u0627\u062F\u0647 \u0648 \u0647\u0645\u0647 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB \u0631\u0627 \u0645\u0634\u0627\u0647\u062F\u0647 \u06A9\u0646\u062F", L"%username% puede jugar a todos los juegos y ver todas las aplicaciones de la Tienda Windows", L"%username% peut jouer \u00E0 tous les jeux et afficher toutes les applications du Windows Store", L"%username% pode jogar todos os jogos exibir todos os aplicativos da Windows Store"},
    {1466, L"%username% can use only the games and the Windows Store apps I allow", L"%username% pu\u00F2 utilizzare solo i giochi e le app di Windows Store consentiti", L"%username% \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0628\u0627\u0632\u06CC \u0647\u0627 \u0648 \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC \u00AB\u0641\u0631\u0648\u0634\u06AF\u0627\u0647 Windows\u00BB \u06A9\u0647 \u0645\u0646 \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u0645 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%username% solo puede usar los juegos y las aplicaciones de la Tienda Windows que yo permita", L"%username% peut utiliser uniquement les applications du Windows Store et les jeux que j\u2019autorise", L"%username% pode usar apenas os jogos e os aplicativos da Windows Store permitidos"},
    {1475, L"The child can view websites on the Allow List. Adult sites are blocked.", L"Il bambino pu\u00F2 visualizzare i siti Web inclusi nell'elenco degli indirizzi Web consentiti. I siti per adulti sono bloccati.", L"\u06A9\u0648\u062F\u06A9 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u0645\u0648\u062C\u0648\u062F \u062F\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632 \u0631\u0627 \u0645\u0634\u0627\u0647\u062F\u0647 \u06A9\u0646\u062F. \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u062D\u0627\u0648\u06CC \u0645\u062D\u062A\u0648\u0627\u06CC \u0628\u0632\u0631\u06AF\u0633\u0627\u0644 \u0645\u0633\u062F\u0648\u062F \u0645\u06CC \u0634\u0648\u062F.", L"El menor puede ver los sitios web en la lista de permitidos. Se bloquean los sitios para p\u00FAblico adulto.", L"L\u2019enfant peut consulter les sites Web de la liste verte. Les sites pour adultes sont bloqu\u00E9s.", L"A crian\u00E7a pode exibir os sites da Lista de Permiss\u00F5es. Os sites para adultos s\u00E3o bloqueados."},
    {1476, L"Click here to change Allow List.", L"Fare clic qui per modificare l'elenco degli indirizzi Web consentiti.", L"\u0628\u0631\u0627\u06CC \u062A\u063A\u06CC\u06CC\u0631 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632\u060C \u0627\u06CC\u0646 \u0642\u0633\u0645\u062A \u0631\u0627 \u06A9\u0644\u06CC\u06A9 \u06A9\u0646\u06CC\u062F.", L"Haga clic aqu\u00ED para cambiar la lista de permitidos.", L"Cliquez ici pour modifier la liste verte.", L"Clique aqui para alterar a Lista de Permiss\u00F5es."},
    {1477, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1480, L"Control how long %username% can use the PC", L"Determina per quanto tempo %username% pu\u00F2 utilizzare il PC", L"\u06A9\u0646\u062A\u0631\u0644 \u06A9\u0646\u06CC\u062F \u06A9\u0647 %username% \u062A\u0627 \u0686\u0647 \u0645\u062F\u062A\u06CC \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"Controlar durante cu\u00E1nto tiempo %username% puede usar el equipo", L"G\u00E9rer la dur\u00E9e pendant laquelle %username% peut utiliser l\u2019ordinateur", L"Controlar por quanto tempo %username% pode usar o computador"},
    {1481, L"%username% can use the PC without a limit on the number of hours", L"%username% pu\u00F2 utilizzare il PC per un numero di ore illimitato", L"%username% \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0628\u062F\u0648\u0646 \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u062A\u0639\u062F\u0627\u062F \u0633\u0627\u0639\u062A \u0647\u0627 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%username% puede usar el equipo sin un l\u00EDmite de n\u00FAmero de horas", L"%username% peut utiliser l\u2019ordinateur sans limite de dur\u00E9e", L"%username% pode usar o computador sem um limite de n\u00FAmero de horas"},
    {1482, L"%username% can only use the PC for the amount of time I allow", L"%username% pu\u00F2 utilizzare il PC solo per il numero di ore consentito", L"%username% \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0641\u0642\u0637 \u0628\u0631\u0627\u06CC \u0645\u062F\u062A \u0632\u0645\u0627\u0646\u06CC \u06A9\u0647 \u0645\u0646 \u0645\u062C\u0627\u0632 \u0645\u06CC \u06A9\u0646\u0645 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%username% solo puede usar el equipo durante la cantidad de tiempo que yo permita", L"%username% peut utiliser l\u2019ordinateur uniquement pendant la dur\u00E9e que j\u2019autorise", L"%username% pode usar o computador somente no tempo permitido"},
    {1491, L"%username% can use the PC all day", L"%username% pu\u00F2 utilizzare il PC per tutto il giorno", L"%username% \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u062F\u0631 \u062A\u0645\u0627\u0645 \u0637\u0648\u0644 \u0631\u0648\u0632 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F", L"%username% puede usar el equipo todo el d\u00EDa", L"%username% peut utiliser l\u2019ordinateur toute la journ\u00E9e", L"%username% pode usar o computador o dia todo"},
    {1492, L"%username% can only use the PC during the time ranges I allow", L"%username% pu\u00F2 utilizzare il PC solo durante la fascia oraria consentita", L"%username% \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647 \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062F\u0631 \u0645\u062D\u062F\u0648\u062F\u0647 \u0632\u0645\u0627\u0646\u06CC \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u06A9\u0646\u062F \u06A9\u0647 \u0645\u0646 \u0645\u062C\u0627\u0632 \u0645\u06CC \u06A9\u0646\u0645", L"%username% solo puede usar el equipo durante los intervalos de tiempo que yo permita", L"%username% peut utiliser l\u2019ordinateur uniquement pendant les horaires que j\u2019autorise", L"%username% pode usar o computador somente durante os intervalos de tempo permitidos"},
    {1510, L"Weekdays:", L"Giorni feriali:", L"\u0631\u0648\u0632\u0647\u0627\u06CC \u0647\u0641\u062A\u0647:", L"D\u00EDas laborables:", L"Jours de la semaine\u00A0:", L"Dias \u00FAteis:"},
    {1511, L"More information", L"Ulteriori informazioni", L"\u0627\u0637\u0644\u0627\u0639\u0627\u062A \u0628\u06CC\u0634\u062A\u0631", L"M\u00E1s informaci\u00F3n", L"Informations suppl\u00E9mentaires", L"Mais informa\u00E7\u00F5es"},
    {1514, L"hours", L"ore", L"\u0633\u0627\u0639\u062A", L"horas", L"heures", L"horas"},
    {1515, L"Web filtering", L"Filtro Web", L"\u0641\u06CC\u0644\u062A\u0631\u06CC\u0646\u06AF \u0648\u0628", L"Filtrado web", L"Filtrage Web", L"Filtragem de sites"},
    {1516, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1517, L"minutes", L"minuti", L"\u062F\u0642\u06CC\u0642\u0647", L"minutos", L"minutes", L"minutos"},
    {1523, L"Weekend:", L"Fine settimana:", L"\u0622\u062E\u0631 \u0647\u0641\u062A\u0647:", L"Fin de semana:", L"Week-end\u00A0:", L"Fim de Semana:"},
    {1526, L"Mon - Fri", L"Lun - Ven", L"\u062F\u0648\u0634\u0646\u0628\u0647 - \u062C\u0645\u0639\u0647", L"Lun - Vier", L"Lun - Ven", L"Seg - Sex"},
    {1527, L"Sat - Sun", L"Sab - Dom", L"\u0634\u0646\u0628\u0647 - \u06CC\u06A9\u0634\u0646\u0628\u0647", L"S\u00E1b - Dom", L"Sam - Dim", L"S\u00E1b - Dom"},
    {1528, L"Allow or block websites by rating and content types", L"Consenti o blocca siti Web in base alla classificazione e ai tipi di contenuto", L"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627 \u0628\u0631 \u0627\u0633\u0627\u0633 \u0631\u062A\u0628\u0647 \u0628\u0646\u062F\u06CC \u0648 \u0646\u0648\u0639 \u0645\u062D\u062A\u0648\u0627", L"Permitir o bloquear sitios web por clasificaci\u00F3n y tipos de contenido", L"Autoriser ou bloquer les sites Web par type de classification et de contenu", L"Bloquear ou permitir sites por classifica\u00E7\u00E3o e tipos de conte\u00FAdo"},
    {1529, L"Set web filtering level", L"Imposta livello filtro Web", L"\u0633\u0637\u062D \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628 \u0631\u0627 \u062A\u0646\u0638\u06CC\u0645 \u06A9\u0646\u06CC\u062F", L"Establecer nivel de filtrado web", L"D\u00E9finir le niveau de filtrage Web", L"Definir n\u00EDvel de filtragem de sites"},
    {1530, L"Jump", L"Passa a", L"\u067E\u0631\u06CC\u062F\u0646", L"Saltar", L"Passer", L"Saltar"},
    {1531, L"Set web filtering level", L"Imposta livello filtro Web", L"\u062A\u0646\u0638\u06CC\u0645 \u0633\u0637\u062D \u0641\u06CC\u0644\u062A\u0631 \u0648\u0628", L"Establecer nivel de filtrado web", L"D\u00E9finir le niveau de filtrage Web", L"Definir n\u00EDvel de filtragem de sites"},
    {1532, L"Allow or block all websites", L"Consenti o blocca tutti i siti Web", L"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627", L"Permitir o bloquear todos los sitios web", L"Autoriser ou bloquer tous les sites Web", L"Bloquear ou permitir todos os sites"},
    {1533, L"Allow or block specific websites", L"Consenti o blocca siti Web specifici", L"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627\u06CC \u062E\u0627\u0635", L"Permitir o bloquear sitios web espec\u00EDficos", L"Autoriser ou bloquer des sites Web sp\u00E9cifiques", L"Permitir ou bloquear sites espec\u00EDficos"},
    {1534, L"Manage settings on the Family Safety website", L"Gestisci impostazioni nel sito Web di Sicurezza famiglia", L"\u0645\u062F\u06CC\u0631\u06CC\u062A \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0645\u0648\u062C\u0648\u062F \u062F\u0631 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647", L"Administrar configuraci\u00F3n en el sitio web de Protecci\u00F3n infantil", L"G\u00E9rer les param\u00E8tres sur le site Web Contr\u00F4le parental", L"Gerenciar configura\u00E7\u00F5es no site Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {1535, L"You already have some settings on individual games, which might override these changes. To review specific games you've allowed or blocked, see the \"Allow or Block Games\" settings", L"Alcuni giochi includono impostazioni che consentono di ignorare queste modifiche. Per verificare quali giochi sono stati consentiti o bloccati, vedere le impostazioni \"Consenti o blocca giochi specifici\"", L"\u200F\u200F\u0634\u0645\u0627 \u0627\u0632 \u0642\u0628\u0644 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A\u06CC \u0631\u0627 \u0628\u0635\u0648\u0631\u062A \u062C\u062F\u0627\u06AF\u0627\u0646\u0647 \u0628\u0631\u0627\u06CC \u0628\u0627\u0632\u06CC \u0647\u0627 \u0627\u0639\u0645\u0627\u0644 \u06A9\u0631\u062F\u0647 \u0627\u06CC\u062F \u06A9\u0647 \u0645\u0645\u06A9\u0646 \u0627\u0633\u062A \u0627\u06CC\u0646 \u062A\u063A\u06CC\u06CC\u0631\u0627\u062A \u0631\u0627 \u0631\u0648\u0646\u0648\u06CC\u0633\u06CC \u06A9\u0646\u0646\u062F. \u0628\u0631\u0627\u06CC \u0628\u0627\u0632\u0628\u06CC\u0646\u06CC \u0628\u0631\u062E\u06CC \u0627\u0632 \u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u062E\u0627\u0635 \u06A9\u0647 \u0645\u062C\u0627\u0632 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0647 \u0627\u06CC\u062F\u060C \u0628\u0647 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \"\u0627\u062C\u0627\u0632\u0647 \u062F\u0627\u062F\u0646 \u06CC\u0627 \u0645\u0633\u062F\u0648\u062F \u06A9\u0631\u062F\u0646 \u0628\u0627\u0632\u06CC \u0647\u0627\" \u0645\u0631\u0627\u062C\u0639\u0647 \u0646\u0645\u0627\u06CC\u06CC\u062F", L"Ya se configuraron algunos juegos individuales, lo que podr\u00EDa invalidar estos cambios. Para revisar los juegos que se han permitido o bloqueado, vea la configuraci\u00F3n de \"Permitir o bloquear juegos\"", L"Vous avez d\u00E9j\u00E0 d\u00E9fini pour certains jeux des param\u00E8tres qui peuvent supplanter ces modifications. Pour afficher les jeux qui sont autoris\u00E9s ou bloqu\u00E9s, consultez les param\u00E8tres \u00AB\u00A0Autoriser ou bloquer des jeux\u00A0\u00BB", L"Voc\u00EA j\u00E1 tem algumas configura\u00E7\u00F5es em jogos individuais que podem substituir estas altera\u00E7\u00F5es. Para verificar jogos espec\u00EDficos que voc\u00EA bloqueou ou permitiu, consulte as configura\u00E7\u00F5es de \"Bloquear ou Permitir jogos espec\u00EDficos\""},
    {1536, L"You've chosen Allow list only, but haven't specified any allowed websites. This user won't be able to browse the web at all.", L"\u00C8 stata scelta l'opzione Solo elenco consentiti ma non sono stati specificati siti Web consentiti. L'utente non sar\u00E0 in grado di visitare alcun sito Web.", L"\u0634\u0645\u0627 \u0641\u0642\u0637 \u0644\u06CC\u0633\u062A \u0645\u062C\u0627\u0632 \u0631\u0627 \u0627\u0646\u062A\u062E\u0627\u0628 \u06A9\u0631\u062F\u0647 \u0627\u06CC\u062F \u0627\u0645\u0627 \u0647\u06CC\u0686 \u0633\u0627\u06CC\u062A \u0645\u062C\u0627\u0632\u06CC \u0631\u0627 \u0645\u0634\u062E\u0635 \u0646\u06A9\u0631\u062F\u0647 \u0627\u06CC\u062F. \u0627\u06CC\u0646 \u06A9\u0627\u0631\u0628\u0631 \u0628\u0637\u0648\u0631 \u06A9\u0644\u06CC \u0646\u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u062F\u0631 \u0627\u06CC\u0646\u062A\u0631\u0646\u062A \u0645\u0631\u0648\u0631 \u06A9\u0646\u062F.", L"Eligi\u00F3 Solo la lista de permitidos, pero no especific\u00F3 ning\u00FAn sitio web permitido. Este usuario no podr\u00E1 explorar la Web en absoluto.", L"Vous avez choisi l\u2019option Liste verte uniquement mais n\u2019avez indiqu\u00E9 aucun site Web autoris\u00E9. Cet utilisateur ne sera pas du tout en mesure de naviguer sur le Web.", L"Voc\u00EA escolheu Somente lista de permiss\u00F5es, mas n\u00E3o especificou nenhum site permitido. Esse usu\u00E1rio n\u00E3o poder\u00E1 navegar na Internet."},
    {1537, L"This person can only run apps needed by Windows.", L"L'utente pu\u00F2 eseguire solo app necessarie per Windows.", L"\u0627\u06CC\u0646 \u0641\u0631\u062F \u0641\u0642\u0637 \u0645\u06CC \u062A\u0648\u0627\u0646\u062F \u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627\u06CC\u06CC \u0631\u0627 \u0627\u062C\u0631\u0627 \u06A9\u0646\u062F \u06A9\u0647 \u0645\u0648\u0631\u062F \u0646\u06CC\u0627\u0632 Windows \u0627\u0633\u062A.", L"Esta persona solo puede ejecutar las aplicaciones que requiere Windows.", L"Cette personne est limit\u00E9e \u00E0 ex\u00E9cuter les applications n\u00E9cessaires \u00E0 Windows.", L"Esta pessoa pode executar somente aplicativos necess\u00E1rios no Windows."},
    {1543, L"Most popular websites", L"Siti Web pi\u00F9 noti", L"\u067E\u0631\u0628\u0627\u0632\u062F\u06CC\u062F\u062A\u0631\u06CC\u0646 \u0648\u0628 \u0633\u0627\u06CC\u062A \u0647\u0627", L"Sitios web m\u00E1s populares", L"Sites Web les plus populaires", L"Sites mais populares"},
    {1544, L"Latest blocked pages", L"Ultime pagine bloccate", L"\u0622\u062E\u0631\u06CC\u0646 \u0635\u0641\u062D\u0627\u062A \u0645\u0633\u062F\u0648\u062F \u0634\u062F\u0647", L"\u00DAltimas p\u00E1ginas bloqueadas", L"Derni\u00E8res pages bloqu\u00E9es", L"Sites bloqueados mais recentes"},
    {1545, L"PC time used", L"Tempo di utilizzo del PC", L"\u0632\u0645\u0627\u0646 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0627\u0632 \u0631\u0627\u06CC\u0627\u0646\u0647", L"Tiempo de uso del equipo", L"Temps PC utilis\u00E9", L"Tempo de computador usado"},
    {1546, L"Most used apps and games", L"App e giochi pi\u00F9 utilizzati", L"\u0628\u0631\u0646\u0627\u0645\u0647 \u0647\u0627 \u0648 \u0628\u0627\u0632\u06CC \u0647\u0627\u06CC \u0628\u06CC\u0634\u062A\u0631 \u0627\u0633\u062A\u0641\u0627\u062F\u0647 \u0634\u062F\u0647", L"Aplicaciones y juegos m\u00E1s usados", L"Applications et jeux les plus utilis\u00E9s", L"Aplicativos e jogos mais usados"},
    {1547, L"Add new user", L"Aggiungi nuovo utente", L"\u0627\u0641\u0632\u0648\u062F\u0646 \u06A9\u0627\u0631\u0628\u0631 \u062C\u062F\u06CC\u062F", L"Agregar nuevo usuario", L"Ajout d\u2019un nouvel utilisateur", L"Adicionar novo usu\u00E1rio"},
    {1548, L"You can turn on additional controls by selecting a provider from this list.", L"\u00C8 possibile attivare ulteriori controlli selezionando un provider nell'elenco.", L"\u0634\u0645\u0627 \u0645\u06CC \u062A\u0648\u0627\u0646\u06CC\u062F \u06A9\u0646\u062A\u0631\u0644 \u0647\u0627\u06CC \u0627\u0636\u0627\u0641\u06CC \u0631\u0627 \u0627\u0632 \u0637\u0631\u06CC\u0642 \u0627\u0646\u062A\u062E\u0627\u0628 \u06CC\u06A9 \u0627\u0631\u0627\u0626\u0647 \u062F\u0647\u0646\u062F\u0647 \u0627\u0632 \u0627\u06CC\u0646 \u0644\u06CC\u0633\u062A \u0641\u0639\u0627\u0644 \u06A9\u0646\u06CC\u062F.", L"Si selecciona un proveedor de esta lista, puede activar controles adicionales.", L"Vous pouvez activer d\u2019autres commandes en s\u00E9lectionnant un fournisseur dans cette liste.", L"Voc\u00EA pode ligar controles adicionais selecionando um provedor nesta lista."},
    {1549, L"One or more administrator accounts don't have a password. Administrator accounts without passwords allow anyone to bypass or turn off Family Safety. <a>Add passwords to these accounts</a>.", L"Uno o pi\u00F9 account amministratore sono privi di password. Utilizzando un account amministratore senza password, chiunque pu\u00F2 eludere o disattivare Sicurezza famiglia. <a>Aggiungere la password a tali account</a>.", L"\u200F\u200F\u06CC\u06A9 \u06CC\u0627 \u062A\u0639\u062F\u0627\u062F \u0628\u06CC\u0634\u062A\u0631\u06CC \u0627\u0632 \u062D\u0633\u0627\u0628 \u0647\u0627\u06CC \u0633\u0631\u067E\u0631\u0633\u062A \u062F\u0627\u0631\u0627\u06CC \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0646\u06CC\u0633\u062A\u0646\u062F. \u062D\u0633\u0627\u0628 \u0647\u0627\u06CC \u0633\u0631\u067E\u0631\u0633\u062A \u0628\u062F\u0648\u0646 \u0631\u0645\u0632 \u0639\u0628\u0648\u0631 \u0628\u0647 \u0647\u0631 \u06A9\u0633\u06CC \u0627\u062C\u0627\u0632\u0647 \u0645\u06CC \u062F\u0647\u0646\u062F \u062A\u0627 \u0627\u0632 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u0639\u0628\u0648\u0631 \u06A9\u0631\u062F\u0647 \u06CC\u0627 \u0622\u0646 \u0631\u0627 \u063A\u06CC\u0631\u0641\u0639\u0627\u0644 \u06A9\u0646\u062F. <a>\u0631\u0645\u0632\u0647\u0627\u06CC \u0639\u0628\u0648\u0631 \u0631\u0627 \u0628\u0647 \u0627\u06CC\u0646 \u062D\u0633\u0627\u0628 \u0647\u0627 \u0627\u0636\u0627\u0641\u0647 \u06A9\u0646\u06CC\u062F</a>.", L"Una o varias cuentas de administrador no tienen contrase\u00F1a. Cualquier usuario podr\u00E1 omitir o desactivar la Protecci\u00F3n infantil mientras las cuentas de administrador carezcan de contrase\u00F1a. <a>Agregue contrase\u00F1as a estas cuentas</a>.", L"Un ou plusieurs comptes administrateur n\u2019ont pas de mot de passe. Les comptes administrateur sans mot de passe permettent \u00E0 n\u2019importe quel utilisateur de contourner ou de d\u00E9sactiver le contr\u00F4le parental. <a>Ajoutez un mot de passe \u00E0 ces comptes</a>.", L"Uma ou mais contas de administrador n\u00E3o t\u00EAm senha. Contas de administrador sem senhas permitem que qualquer pessoa ignore ou desligue a Prote\u00E7\u00E3o para a Fam\u00EDlia. <a>Adicione senhas a essas contas</a>."},
    {1550, L"Add passwords", L"Aggiungi password", L"\u0627\u0641\u0632\u0648\u062F\u0646 \u0631\u0645\u0632\u0647\u0627\u06CC \u0639\u0628\u0648\u0631", L"Agregar contrase\u00F1as", L"Ajouter des mots de passe", L"Adicionar senhas"},
    {1551, L"Family Safety Off", L"Sicurezza famiglia disattivata", L"\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647 \u062E\u0627\u0645\u0648\u0634", L"Protecci\u00F3n infantil desactivada", L"Contr\u00F4le parental d\u00E9sactiv\u00E9", L"Prote\u00E7\u00E3o para a Fam\u00EDlia desativada"},
    {1552, L"Activity Reporting Off", L"Rapporto attivit\u00E0 disattivato", L"\u06AF\u0632\u0627\u0631\u0634 \u0641\u0639\u0627\u0644\u06CC\u062A \u062E\u0627\u0645\u0648\u0634", L"Informe de actividades desactivado", L"Rapports d\u2019activit\u00E9 d\u00E9sactiv\u00E9s", L"Relat\u00F3rio de Atividades ativado"},
    {1557, L"Weekdays Toggle Expando", L"Espandi/Comprimi giorni feriali", L"\u06AF\u0633\u062A\u0631\u0634 \u062C\u0627\u0628\u062C\u0627\u06CC\u06CC \u0631\u0648\u0632\u0647\u0627\u06CC \u0647\u0641\u062A\u0647", L"Alternar expando de d\u00EDas laborables", L"Basculer Expando pour les jours ouvr\u00E9s", L"Alternar Expandir de Dias de Semana"},
    {1558, L"Show or hide extra information/elements", L"Mostra o nasconde informazioni o elementi aggiuntivi", L"\u0627\u0637\u0644\u0627\u0639\u0627\u062A/\u0639\u0646\u0627\u062B\u0631 \u0628\u06CC\u0634\u062A\u0631 \u0631\u0627 \u0646\u0645\u0627\u06CC\u0634 \u062F\u0627\u062F\u0647 \u06CC\u0627 \u067E\u0646\u0647\u0627\u0646 \u06A9\u0646\u06CC\u062F", L"Mostrar u ocultar informaci\u00F3n o elementos adicionales", L"Afficher ou masquer les informations/\u00E9l\u00E9ments suppl\u00E9mentaires", L"Mostrar ou ocultar informa\u00E7\u00F5es/elementos extras"},
    {1559, L"Weekend Toggle Expando", L"Espandi/Comprimi fine settimana", L"\u06AF\u0633\u062A\u0631\u0634 \u062C\u0627\u0628\u062C\u0627\u06CC\u06CC \u0622\u062E\u0631 \u0647\u0641\u062A\u0647", L"Alternar expando de d\u00EDas laborables", L"Basculer Expando pour les week-ends", L"Alternar Expandir de Dias de Semana"},
    {1560, L"Show or hide extra information/elements", L"Mostra o nasconde informazioni o elementi aggiuntivi", L"\u0627\u0637\u0644\u0627\u0639\u0627\u062A/\u0639\u0646\u0627\u062B\u0631 \u0628\u06CC\u0634\u062A\u0631 \u0631\u0627 \u0646\u0645\u0627\u06CC\u0634 \u062F\u0627\u062F\u0647 \u06CC\u0627 \u067E\u0646\u0647\u0627\u0646 \u06A9\u0646\u06CC\u062F", L"Mostrar u ocultar informaci\u00F3n o elementos adicionales", L"Afficher ou masquer les informations/\u00E9l\u00E9ments suppl\u00E9mentaires", L"Mostrar ou ocultar informa\u00E7\u00F5es/elementos extras"},
    {1561, L"Weekdays Hours Allowed", L"Ore consentite giorni feriali", L"\u0633\u0627\u0639\u062A \u0647\u0627\u06CC \u0645\u062C\u0627\u0632 \u062F\u0631 \u0631\u0648\u0632\u0647\u0627\u06CC \u0647\u0641\u062A\u0647", L"Horas de d\u00EDas laborables permitidas", L"Heures autoris\u00E9es pendant les jours ouvr\u00E9s", L"Horas de Dias de Semana Permitidas"},
    {1562, L"Weekdays Minutes Allowed", L"Minuti consentiti giorni feriali", L"\u062F\u0642\u06CC\u0642\u0647 \u0647\u0627\u06CC \u0645\u062C\u0627\u0632 \u062F\u0631 \u0631\u0648\u0632\u0647\u0627\u06CC \u0647\u0641\u062A\u0647", L"Minutos de d\u00EDas laborables permitidos", L"Minutes autoris\u00E9es pendant les jours ouvr\u00E9s", L"Minutos de Dias de Semana Permitidas"},
    {1563, L"Weekend Hours Allowed", L"Ore consentite fine settimana", L"\u0633\u0627\u0639\u062A \u0647\u0627\u06CC \u0645\u062C\u0627\u0632 \u062F\u0631 \u0622\u062E\u0631 \u0647\u0641\u062A\u0647", L"Horas de fin de semana permitidas", L"Heures autoris\u00E9es pendant le week-end", L"Horas de Dias de Semana Permitidas"},
    {1564, L"Weekend Minutes Allowed", L"Minuti consentiti fine settimana", L"\u062F\u0642\u06CC\u0642\u0647 \u0647\u0627\u06CC \u0645\u062C\u0627\u0632 \u062F\u0631 \u0622\u062E\u0631 \u0647\u0641\u062A\u0647", L"Minutos de fin de semana permitidos", L"Minutes autoris\u00E9es pendant le week-end", L"Minutos de Dias de Semana Permitidas"},
    {1565, L"Manage settings on the Family Safety website", L"Gestisci impostazioni nel sito Web di Sicurezza famiglia", L"\u0645\u062F\u06CC\u0631\u06CC\u062A \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0645\u0648\u062C\u0648\u062F \u062F\u0631 \u0633\u0627\u06CC\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647", L"Administrar configuraci\u00F3n en el sitio web de Protecci\u00F3n infantil", L"G\u00E9rer les param\u00E8tres sur le site Web Contr\u00F4le parental", L"Gerenciar configura\u00E7\u00F5es no site Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {1566, L"12 AM", L"0.00", L"12 \u0635\u0628\u062D", L"12 a.m.", L"Minuit", L"0:00"},
    {1567, L"1 AM", L"1.00", L"1 \u0635\u0628\u062D", L"1 a.m.", L"01:00", L"01:00"},
    {1568, L"2 AM", L"2.00", L"2 \u0635\u0628\u062D", L"2 a.m.", L"02:00", L"02:00"},
    {1569, L"3 AM", L"3.00", L"3 \u0635\u0628\u062D", L"3 a.m.", L"03:00", L"03:00"},
    {1570, L"4 AM", L"4.00", L"4 \u0635\u0628\u062D", L"4 a.m.", L"04:00", L"04:00"},
    {1571, L"5 AM", L"5.00", L"5 \u0635\u0628\u062D", L"5 a.m.", L"05:00", L"05:00"},
    {1572, L"6 AM", L"6.00", L"6 \u0635\u0628\u062D", L"6 a.m.", L"06:00", L"06:00"},
    {1573, L"7 AM", L"7.00", L"7 \u0635\u0628\u062D", L"7 a.m.", L"07:00", L"07:00"},
    {1574, L"8 AM", L"8.00", L"8 \u0635\u0628\u062D", L"8 a.m.", L"08:00", L"08:00"},
    {1575, L"9 AM", L"9.00", L"9 \u0635\u0628\u062D", L"9 a.m.", L"09:00", L"09:00"},
    {1576, L"10 AM", L"10.00", L"10 \u0635\u0628\u062D", L"10 a.m.", L"10:00", L"10:00"},
    {1577, L"11 AM", L"11.00", L"11 \u0635\u0628\u062D", L"11 a.m.", L"11:00", L"11:00"},
    {1578, L"12 PM", L"12.00", L"12 \u0628\u0639\u062F\u0627\u0632\u0638\u0647\u0631", L"12 p.m.", L"Midi", L"12:00"},
    {1579, L"1 PM", L"13.00", L"1 \u0639\u0635\u0631", L"1 p.m.", L"13:00", L"13:00"},
    {1580, L"2 PM", L"14.00", L"2 \u0639\u0635\u0631", L"2 p.m.", L"14:00", L"14:00"},
    {1581, L"3 PM", L"15.00", L"3 \u0639\u0635\u0631", L"3 p.m.", L"15:00", L"15:00"},
    {1582, L"4 PM", L"16.00", L"4 \u0639\u0635\u0631", L"4 p.m.", L"16:00", L"16:00"},
    {1583, L"5 PM", L"17.00", L"5 \u0639\u0635\u0631", L"5 p.m.", L"17:00", L"17:00"},
    {1584, L"6 PM", L"18.00", L"6 \u0639\u0635\u0631", L"6 p.m.", L"18:00", L"18:00"},
    {1585, L"7 PM", L"19.00", L"7 \u0639\u0635\u0631", L"7 p.m.", L"19:00", L"19:00"},
    {1586, L"8 PM", L"20.00", L"8 \u0634\u0628", L"8 p.m.", L"20:00", L"20:00"},
    {1587, L"9 PM", L"21.00", L"9 \u0634\u0628", L"9 p.m.", L"21:00", L"21:00"},
    {1588, L"10 PM", L"22.00", L"10 \u0634\u0628", L"10 p.m.", L"22:00", L"22:00"},
    {1589, L"11 PM", L"23.00", L"11 \u0634\u0628", L"11 p.m.", L"23:00", L"23:00"},
    {1599, L"Turning on web restrictions also turns on SafeSearch settings for Bing, Google, Yahoo! and other popular search engines. Adult images are also blocked.", L"L'attivazione delle restrizioni Web comporta l'attivazione delle impostazioni di Ricerca Sicura in Bing, Google, Yahoo! e altri noti motori di ricerca. Vengono inoltre bloccate le immagini per adulti.", L"\u0641\u0639\u0627\u0644 \u06A9\u0631\u062F\u0646 \u0645\u062D\u062F\u0648\u062F\u06CC\u062A \u0647\u0627\u06CC \u0648\u0628\u060C \u062A\u0646\u0638\u06CC\u0645\u0627\u062A SafeSearch \u0631\u0627 \u0628\u0631\u0627\u06CC Bing\u060C Google\u060C Yahoo! \u0648 \u0633\u0627\u06CC\u0631 \u0645\u0648\u062A\u0648\u0631\u0647\u0627\u06CC \u062C\u0633\u062A\u062C\u0648\u06CC \u0645\u0639\u0631\u0648\u0641 \u0646\u06CC\u0632 \u0641\u0639\u0627\u0644 \u0645\u06CC \u06A9\u0646\u062F. \u062A\u0635\u0627\u0648\u06CC\u0631 \u0645\u0646\u0627\u0633\u0628 \u0627\u0641\u0631\u0627\u062F \u0628\u0627\u0644\u063A \u0646\u06CC\u0632 \u0645\u0633\u062F\u0648\u062F \u062E\u0648\u0627\u0647\u062F \u0634\u062F.", L"La activaci\u00F3n de las restricciones web tambi\u00E9n activa la configuraci\u00F3n de B\u00FAsqueda segura de Bing, Google, Yahoo! y otros motores de b\u00FAsqueda conocidos. Tambi\u00E9n se bloquean las im\u00E1genes para adultos.", L"L\u2019activation de restrictions d\u2019acc\u00E8s au Web active \u00E9galement les param\u00E8tres de recherche s\u00E9curis\u00E9e pour Bing, Google, Yahoo! et d\u2019autres moteurs de recherche connus. Les images pour adultes sont \u00E9galement bloqu\u00E9es.", L"A ativa\u00E7\u00E3o de restri\u00E7\u00F5es na Internet tamb\u00E9m ativa configura\u00E7\u00F5es de PesquisaSegura para Bing, Google, Yahoo! e outros mecanismos de pesquisa populares. As imagens para adultos tamb\u00E9m s\u00E3o bloqueadas."},
    {1600, L"Set up Family Safety", L"Configura Sicurezza famiglia", L"\u0639\u0645\u0644\u06A9\u0631\u062F \u00AB\u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647\u00BB \u0631\u0627 \u067E\u06CC\u06A9\u0631\u0628\u0646\u062F\u06CC \u06A9\u0646\u06CC\u062F", L"Configurar Protecci\u00F3n infantil", L"Configurer le contr\u00F4le parental", L"Configurar a Prote\u00E7\u00E3o para a Fam\u00EDlia"},
    {0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
};

// =============================================================================
// Visual style (skin)
//   Win8: the untouched Windows 8/8.1 "Family Safety" applet (default; output is byte-identical
//         to the previous versions of this mod, so every pinned SHA-256 still applies).
//   Win7: the Windows 7 "Parental Controls" look: the applet is renamed in all six languages
//         and the Windows 7 icon is used (see BuildWin7SkinIconDll). Only the product name (and
//         the "PC" -> "computer" wording in English) changes; the page itself is not altered.
// =============================================================================
enum class Skin : int { Win8 = 0, Win7 = 1 };
static std::atomic<int> g_skin{static_cast<int>(Skin::Win8)};
static Skin CurrentSkin() { return static_cast<Skin>(g_skin.load(std::memory_order_relaxed)); }

namespace skin7 {

static bool IsLetter(wchar_t c) {
    return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') || c >= 0x00C0;
}

static void ReplaceAll(std::wstring& s, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) return;
    for (size_t pos = 0; (pos = s.find(from, pos)) != std::wstring::npos; pos += to.size())
        s.replace(pos, from.size(), to);
}

// Like ReplaceAll, but a match must not be glued to a preceding letter, so that the short
// article rules ("a ", "la ") never fire in the middle of another word.
static void ReplaceAllBounded(std::wstring& s, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) return;
    for (size_t pos = 0; (pos = s.find(from, pos)) != std::wstring::npos;) {
        if (pos > 0 && IsLetter(s[pos - 1])) { pos += 1; continue; }
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
}

// Windows 7 calls it "the computer"; the 8.1 text says "PC". "PC settings" (the Windows 8
// Settings app) is a different thing and is kept as it is.
static void ReplacePcEnglish(std::wstring& s) {
    for (size_t pos = 0; (pos = s.find(L"PC", pos)) != std::wstring::npos;) {
        const size_t end = pos + 2;
        const bool leftOk = pos == 0 || !IsLetter(s[pos - 1]);
        const bool rightOk = end >= s.size() || !IsLetter(s[end]);
        if (!leftOk || !rightOk || s.compare(end, 9, L" settings") == 0) { pos = end; continue; }
        const bool capital = pos == 0 ||
            (pos >= 2 && s[pos - 1] == L' ' && (s[pos - 2] == L'.' || s[pos - 2] == L':'));
        const wchar_t* word = capital ? L"Computer" : L"computer";
        s.replace(pos, 2, word);
        pos += wcslen(word);
    }
}

static std::wstring ApplyWin7SkinText(const wchar_t* base, MuiLanguage language) {
    std::wstring s(base);
    switch (language) {
        case MuiLanguage::EN_US:
            ReplacePcEnglish(s);
            ReplaceAll(s, L"Family Safety", L"Parental Controls");
            break;
        case MuiLanguage::IT_IT: {
            const std::wstring brand = L"Sicurezza famiglia", neu = L"Controllo genitori";
            ReplaceAll(s, brand + L" attivata", neu + L" attivato");
            ReplaceAll(s, brand + L" disattivata", neu + L" disattivato");
            ReplaceAll(s, brand, neu);
            break;
        }
        case MuiLanguage::ES_ES: {
            // "la Protección infantil" (feminine) becomes "el Control parental" (masculine)
            const std::wstring brand = L"Protección infantil", neu = L"Control parental";
            ReplaceAll(s, brand + L" activada", neu + L" activado");
            ReplaceAll(s, brand + L" desactivada", neu + L" desactivado");
            ReplaceAllBounded(s, L"de la " + brand, L"del " + neu);
            ReplaceAllBounded(s, L"a la " + brand, L"al " + neu);
            ReplaceAllBounded(s, L"la " + brand, L"el " + neu);
            ReplaceAllBounded(s, L"La " + brand, L"El " + neu);
            ReplaceAll(s, brand, neu);
            break;
        }
        case MuiLanguage::PT_BR: {
            // "a Proteção para a Família" (feminine) becomes "o Controle dos Pais" (masculine)
            const std::wstring brand = L"Proteção para a Família", neu = L"Controle dos Pais";
            ReplaceAll(s, brand + L" ligada", neu + L" ligado");
            ReplaceAll(s, brand + L" desativada", neu + L" desativado");
            ReplaceAllBounded(s, L"da " + brand, L"do " + neu);
            ReplaceAllBounded(s, L"à " + brand, L"ao " + neu);
            ReplaceAllBounded(s, L"a " + brand, L"o " + neu);
            ReplaceAllBounded(s, L"A " + brand, L"O " + neu);
            ReplaceAll(s, brand, neu);
            break;
        }
        case MuiLanguage::FA_IR:
            ReplaceAll(s, L"ایمنی خانواده",
                       L"کنترل والدین");
            break;
        case MuiLanguage::FR_FR:
        default:
            break;   // the French 8.1 name is already the Windows 7 one: "Contrôle parental"
    }
    return s;
}

}  // namespace skin7

static std::mutex g_skinTextMutex;
static std::unordered_map<uint64_t, std::wstring> g_skinTextCache;   // node based: c_str() stays valid

static const wchar_t* GetWpcStringBase(uint32_t id, MuiLanguage language) {
    for (const WpcStringTable* row = kWpcStrings; row->en; ++row) {
        if (row->id != id) continue;
        switch (language) {
            case MuiLanguage::IT_IT: return row->it;
            case MuiLanguage::FA_IR: return row->fa;
            case MuiLanguage::ES_ES: return row->es;
            case MuiLanguage::FR_FR: return row->fr;
            case MuiLanguage::PT_BR: return row->pt;
            default: return row->en;
        }
    }
    return nullptr;
}

static const wchar_t* GetWpcString(uint32_t id, MuiLanguage language, Skin skin) {
    const wchar_t* base = GetWpcStringBase(id, language);
    if (!base || skin != Skin::Win7 || !*base) return base;
    const uint64_t key = (static_cast<uint64_t>(id) << 8) | static_cast<uint64_t>(language);
    std::lock_guard<std::mutex> lock(g_skinTextMutex);
    auto it = g_skinTextCache.find(key);
    if (it == g_skinTextCache.end())
        it = g_skinTextCache.emplace(key, skin7::ApplyWin7SkinText(base, language)).first;
    return it->second.c_str();
}

static const wchar_t* GetWpcString(uint32_t id, MuiLanguage language) {
    return GetWpcString(id, language, CurrentSkin());
}

[[maybe_unused]] static MuiLanguage DetectMuiLanguage(const wchar_t* locale) {
    if (!locale || !locale[0]) return MuiLanguage::EN_US;
    auto starts = [](const wchar_t* text, const wchar_t* prefix) {
        while (*prefix) {
            wchar_t a = *text++, b = *prefix++;
            if (a >= L'A' && a <= L'Z') a += L'a' - L'A';
            if (b >= L'A' && b <= L'Z') b += L'a' - L'A';
            if (a != b) return false;
        }
        return true;
    };
    if (starts(locale, L"it")) return MuiLanguage::IT_IT;
    if (starts(locale, L"fa")) return MuiLanguage::FA_IR;
    if (starts(locale, L"es")) return MuiLanguage::ES_ES;
    if (starts(locale, L"fr")) return MuiLanguage::FR_FR;
    if (starts(locale, L"pt")) return MuiLanguage::PT_BR;
    return MuiLanguage::EN_US;
}

static uint16_t GetEmbeddedLanguageId(MuiLanguage language) {
    switch (language) {
        case MuiLanguage::IT_IT: return 0x0410;
        case MuiLanguage::FA_IR: return 0x0429;
        case MuiLanguage::ES_ES: return 0x0C0A;
        case MuiLanguage::FR_FR: return 0x040C;
        case MuiLanguage::PT_BR: return 0x0416;
        default: return 0x0409;
    }
}

static bool BuildWpcStringBlockBytes(uint32_t blockId, MuiLanguage language, Bytes& output,
                                     Skin skin = CurrentSkin()) {
    using namespace pepatch::detail;
    output.clear();
    for (uint32_t index = 0; index < 16; ++index) {
        const uint32_t id = (blockId - 1) * 16 + index;
        const wchar_t* text = GetWpcString(id, language, skin);
        const size_t length = text ? wcslen(text) : 0;
        if (length > 0xFFFF) return false;
        Push16(output, static_cast<uint16_t>(length));
        for (size_t i = 0; i < length; ++i) Push16(output, static_cast<uint16_t>(text[i]));
    }
    return true;
}

struct MuiLocaleTarget { MuiLanguage lang; const wchar_t* folder; uint16_t lcid; };
static const MuiLocaleTarget kMuiLocaleTargets[] = {
    {MuiLanguage::EN_US, L"en-US", 0x0409},
    {MuiLanguage::IT_IT, L"it-IT", 0x0410},
    {MuiLanguage::FA_IR, L"fa-IR", 0x0429},
    {MuiLanguage::ES_ES, L"es-ES", 0x0C0A},
    {MuiLanguage::ES_ES, L"es-MX", 0x080A},
    {MuiLanguage::FR_FR, L"fr-FR", 0x040C},
    {MuiLanguage::PT_BR, L"pt-BR", 0x0416},
    {MuiLanguage::PT_BR, L"pt-PT", 0x0816},
};

static Bytes BuildWpcMui(const Bytes& wpcCpl, MuiLanguage language,
                         const wchar_t* langTag, uint16_t primaryLcid, Skin skin = CurrentSkin()) {
    using namespace pepatch;
    if (!langTag || !*langTag || primaryLcid == 0) return {};
    Bytes lnRc = ExtractResource(wpcCpl, "MUI", 0, 1);
    if (lnRc.size() < 60 || memcmp(lnRc.data() + 28, kExpectedMuiChecksums, 32) != 0) return {};
    Bytes muiRc = BuildMuiRcFromLn(lnRc, langTag);
    Bytes version;
    if (muiRc.empty() || !BuildEmbeddedVersionResourceBytes(version) || version.empty()) return {};

    MuiResourceTypeGroup strings{6, {}};
    if (language == MuiLanguage::EN_US && skin == Skin::Win8) {
        // Preserve the exact English output and its long-standing hash assertion.
        for (const EmbeddedStringBlock& row : kEmbeddedStringBlocks) {
            Bytes block;
            if (!BuildEmbeddedStringBlockBytes(row, block) || block.empty()) return {};
            strings.items.push_back({row.id, std::move(block)});
        }
    } else {
        for (const EmbeddedStringBlock& row : kEmbeddedStringBlocks) {
            Bytes block;
            if (!BuildWpcStringBlockBytes(row.id, language, block, skin) || block.empty()) return {};
            strings.items.push_back({row.id, std::move(block)});
        }
    }
    if (strings.items.size() != 81 || strings.items.front().first != 7 ||
        strings.items.back().first != 101) return {};
    const uint16_t nativeLcid = GetEmbeddedLanguageId(language);
    std::vector<uint16_t> lcids = {0x0409, nativeLcid, primaryLcid};
    std::sort(lcids.begin(), lcids.end());
    lcids.erase(std::unique(lcids.begin(), lcids.end()), lcids.end());
    MuiResourceTypeGroup versions{16, {{1u, std::move(version)}}};
    return BuildMuiDll(muiRc, {strings, versions}, lcids);
}

// ---- Windows 7 skin: icons ----------------------------------------------------------------
// The Windows 7 Parental Controls icon: group 1 is the applet icon (people in front of a monitor,
// with the UAC shield); group 2 is the same artwork without the shield, used for the elevation
// entry. Both come from the Windows 7 edit of WPCCPL.DLL posted on the WinClassic forum by
// RandomGuy5747. Stored as Base64 of the raw RT_ICON payloads (PNG for 256x256, 32-bit DIB for
// the other sizes) and wrapped at runtime into a tiny resource-only DLL, so the Windows 8.1
// WPCCPL downloaded from Microsoft is never modified.
// kWin7ShieldPng256: 71161 bytes
static const char kWin7ShieldPng256[] =
    "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";

// kWin7ShieldBmp64: 16936 bytes
static const char kWin7ShieldBmp64[] =
    "KAAAAEAAAACAAAAAAQAgAAAAAAAAQgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAIAAAAGAAAABQAAAAQAAAACAAAAAAAAAAAAAAAA////Af///wH///8C////Av///wL///8B////AQAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAI"
    "AAIEIQERHT4DHjVcBChHdgUsToQGOGKVBjpnmwY6ZpwFOGKZBC1OigMnRH4CHTRoBhQgTRQVFzE0NDQbu7u7C////wj///8H////Bf///wP///"
    "8CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQYLCQMVJD"
    "8HMlZ9DVOMtA9nsNkSdsnzEn/Y/xKC3/8Rg+L/EoTl/xGE4/8Qg+P/EIDf/w582f8LeNL/DHLH/w1ptvYRXJreGE9+vyRAV486QUhaZWVlL0xM"
    "TGhHR0eTPDw8myoqKpkvLy+PMjIyTSsrKwoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAh83"
    "SBFYkbMdhdfuIpTw/yGW8v8flO//HZHs/xyO6v8ajOj/GIrm/xeH5P8VhOH/FIPf/xOB3f8Qftr/EHvU/xF40P8Wec3/HnzL/yR+yv8ofML/I1"
    "iE80FMVeexsbH10NLT+9/g4P/c3Nz/1tbW95uameQKCgqKAAAAHQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACER"
    "4DEE6AeSyV6fgxpf7/M6L3/zSg8v8ynu//Lpzu/ymY7v8jlOv/H5Lq/x6Q6f8bjeb/GInj/xeG4f8UhN//FIPd/xWB2v8Zf9X/I4DQ/y+Fzv86"
    "iMr/KVN4/2lrbf/V2Nr/r8XQ/1Wqxf8ynLb/p3pZ/7meiv/h3tz/1NPS9UJCQrQAAAA6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAABQAAAAcAAAAIAAAACAAAAAgAAAAIAAAACAAA"
    "AAgAAAAABSE9QT2m9PhWtvv/P6fw/0ap8f9HqvH/Q6nx/z6m8f84ovD/Lp3u/yeZ7f8klev/IJPp/x6P5v8bi+T/GIbh/xmG3/8chtz/JYja/z"
    "KM2P9FlNf/M12A/3h6e//a3d//fau//xu0zP8AwOP/DMLe/8SFP/+2eUD/oIZv/8vDv//c29r3WlpavQAAADoAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAVBAkHMwkTDlIOHRZrFCoegBYvI4wXMCSTFzAjlBcwI5QXMCOUFz"
    "AjlBYwI5QWLyOTGCkZiRtSaLVgvP3/Z7v3/1Kx8/9ct/T/Xbb0/1i18/9PsfP/Ravy/zqm8f8woe//Kpzs/yaZ7P8jlen/H5Dn/xyL4/8cieD/"
    "I4ve/zOQ3v9Hmt7/RXei/3Byc//f4OH/cKS1/yy4zP88zd7/Wt3v/23X6v/hmkX/0ptU/8l/Of+2dUf/xr23/9zb2vhBQUG1AAAAJgAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcPCzUcOSqHLVtEwjl0V+NAg2L0Q4pn/UWOav9GkWz/RZBs/0WQa/9F"
    "j2v/RY9p/0SPav9Dj2j/S5Ft/3KWef9AjoL/a731/3vG+v9nvPX/ccL3/3LC9/9swPb/Ybz1/1O19f9FrvT/Oajy/zGj8P8uoe//Kpzt/yaW6v"
    "8jkub/J4/j/zOU4/9Em+P/T4/C/1ZbX//Y2tv/fKSv/yu5yv9VyuD/b9Xw/33g9v912e7/2Y89/8ONUP+9i1D/z3gp/79tN/+5s67/0tLS9QwM"
    "DJYAAAATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwYEEiRJNo9ChmTuTp52/1CheP9Qn3j/UJ52/06bdf9OmnT/Tpp0/0"
    "2Zc/9NmXP/TZhy/0yXcv9Ml3H/R5Vu/1ieef+gppj/V5aA/2u95/+T0v3/eMb3/4HL+P+DzPj/fMj5/27E+P9fvfb/T7b1/0Gv9P84qfH/M6Tt"
    "/y+c5/8rl+H/K5Ld/zCQ2/9BmN7/WKXj/zhETf/M0NL/l7bC/xi2zP9Gw+D/c9X3/33a+P923/7/XtP2/9ePPv/Bikr/vopI/72EPv/NbBv/qn"
    "BJ/9XQzv+2trbrAgEAfwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAChUQDzVpT69XqoL/UqF7/1GdeP9Tn3n/Up94/1Ofef9ToHn/"
    "VJ95/1Oeef9Un3n/U595/1KeeP9RnXf/UZx3/0mYcf9rp4T/uq+p/2qVfP9juMv/o9v//4nP+P+Jz/n/jNL5/4TN+v92yfn/Z8L4/1a79v9Gsf"
    "H/OqTl/zKY2P8ukMz/LYnH/zGIxv8+jsr/UZnR/zVTa/+sra3/vsrS/xqvyv8zyer/Z8/y/3La/f9s1/7/YtT//zvJ9v/MiDv/wIlJ/8SMRv/F"
    "iEH/vn84/7hgGf+ihHH/393c/m9ubc0AAABNAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACpTQHNhtY7/XaeE/1WhfP9WoXv/V6N9/1ejff"
    "9ZpX//WaZ//1qlgP9apYD/WaV//1ikfv9Wonz/VaF7/1Whe/9Km3P/g7OV/868tv98k4L/Wqyf/5rY/P+h2vz/idL6/43U+v+F0Pr/ecz6/2rF"
    "+P9Utez/N53V/yiIv/8lfrP/JXyw/yp+s/80hLr/RY/C/1KHr/9hZGb/4OTn/z2rwv8Q0Pj/Yc32/2rR+P9o1v7/W9L//1bR//8xx/X/yIM1/7"
    "6DQv/EjUr/xoxF/8KDPf+0ci//oVEZ/7Gnn//d3Nz6BwcHnQAAABMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABLj2+7abSS/2Gqif9ap4H/W6aA"
    "/1yogv9cqIL/XamD/16qhP9eqoT/XqqE/1ypgv9cqIL/W6eB/1ikfv9YpH7/S552/5/CqP/Yxr3/g5CG/16og/9yv87/seX//5TY+/+I0/r/g9"
    "L7/3fM+/9buez/TqDL/1+Wsv96lqn/iZmj/42Yof+KmKH/h5up/4OjuP85REz/wMLD/5a5x/8Hx/P/TtD6/23W/v9s1v3/YNP//1XQ/f9W0f//"
    "M8j1/8V+MP+5fz3/v4lH/8WNTP/DhkP/unk5/61gIf+KYkb/1tPR/5OTk98AAABpAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAV599ym+0lP9lr4"
    "3/X6uG/1+rhf9grIb/Ya2H/2KuiP9iroj/Yq6I/2KuiP9grIb/X6uF/16qhP9bp4H/WKZ//1OjfP+/0br/283B/4ePif9ssIz/VqaG/4nN4f+q"
    "5P//itT8/33Q+/9qxfL/iLnT/8nCwf/ayL7/xrm1/7SrrP+spqr/qKSm/7Gpp/+Yj4r/bWxs/9vg5f8lsM3/LNf//2rW/P9s2f7/Z9f+/1nQ/v"
    "9S0P3/V9L//zPG9f/Ceyz/t3s4/7yFQ//Bi03/xYxI/8KAO/+0bi3/pE4P/5+Vjf/Qz8/5KCgorAAAAB0AAAAAAAAAAAAAAAAAAAAAAAAAAFug"
    "gcR1upr/arOQ/2Swi/9jr4r/ZLCK/2Wxi/9msoz/ZrKM/2ayjP9lsYv/Y6+J/2KuiP9iroj/X6qE/1enf/9mrIj/2t/L/9fRwf+KkYr/dbWW/1"
    "upff9cqIr/gMzb/5Te//9z0P//k8nk//DYzf/Kxsr/iqXA/3SZvf9qkrn/Z4+2/2qPsf9zlLD/OEFI/8rMzv+Rt8L/Bcv2/1LU//9u2f7/b9n/"
    "/2HU/v9T0f3/UtD9/1fS//8zyPX/v3cp/7J1Mv+5gD//vodJ/8OLTP/HgUH/wXYv/65bGv+EXEL/1M/M/6OioeQAAABfAAAAAgAAAAAAAAAAAA"
    "AAAAAAAABgpoXEfL+g/3C5mP9ptZH/aLSP/2m1j/9qtpD/a7eR/2u3kf9rt5H/araQ/2ezjf9msoz/ZLCK/2OuiP9Vpn//hbua/+nq1f/X2MX/"
    "jpOM/3i2mP9hsIn/X6qA/1qng/9qurr/ZMXv/6PQ6v/t2db/eJ/C/2WSvf9qkrr/Zo6z/2OKrP9ojKz/aIOd/1VXWP/k6Or/NKvE/yrX//9r2P"
    "3/ctr//2/Z/v9b1P3/TtD9/0/P/f9U0P//Msf1/7xyJf+tbyz/tXs6/7qCRf++hU3/xIVC/8l4Mv+3ZyX/mEgU/62hmP/Rz876MjIynwAAABEA"
    "AAAAAAAAAAAAAAAAAAAAYKWFu4LEpv97v6H/cbuZ/265lP9uupP/brqU/2+7lf9vu5X/brqU/225k/9qtpD/aLSO/2ezjf9ksIr/VaeA/67Qs/"
    "/s79n/3OPM/46TjP97uZz/Y7OL/2KtiP9grIL/W6V7/1OehP9Xo6T/l7e7/3ufwf9pkrn/Y4qw/1yEqf9cgqT/ZYin/1Bjdf+pqqr/zdLU/zy9"
    "1P9a1vv/c9r9/3bb/f9t2P3/V9L8/07Q/P9O0Pz/UdH+/y3G9f+4cCH/pmgp/61zNP+0ez//uYJJ/72CSv/BeDX/vmkm/6RND/+SeGj/2dfV/4"
    "GBgcoAAAAwAAAAAAAAAAAAAAAAAAAAAFWPc6WHy6z/hMWo/3jBoP90vpr/cb2X/3K+mP9zv5n/c7+Z/3C8lv9vu5X/bbmT/2y4kv9ptY//YK+I"
    "/2myjP/V583/4u7V/9zpz/+Pl43/f72g/2e2jf9lsYr/XqqF/1yngv9VnXX/SI9l/zmDXv9aipj/ZYu2/1eApf9ReJ7/VHmd/2GDov9CTlj/xs"
    "bG/7XBvv895ef/ZOP9/2/g/v924P//Y9///07Z//9H1///Sdj//0za//8jzv//xWkU/8BoFf/EcSD/yXoq/81+M//QgTn/2X4s/9dxHP/MZBj/"
    "il9E/9DPz/+5urrrAAAAVAAAAAAAAAAAAAAAAAAAAABMgGeTic+u/4vJrv+Ax6f/esOg/3XBm/90wJr/c7+Z/3O/mf9zv5n/cr6Y/2+7lf9tuZ"
    "P/abWP/1ushP+czav/5/TY/97w1P/b7ND/kp6S/37Aov9nto7/ZrKM/1+qhP9ZpX//VJ13/02Sb/9Ah13/R394/1V9qf9Ndp3/Unme/1yBo/9t"
    "jqv/T1Zc/+Dg3/+mqar/s5ps/72Ybf+5mG//sYZQ/6RxNf+cbDf/nGs3/5xrN/+fbDf/lGw//zq5z/86u9j/P8DZ/0fC2v9MxNv/UcLZ/1S61P"
    "9Qssr/PKnG/yaRqf+6w8n/yMnK8wAAAHEAAAAAAAAAAAAAAAAAAAAAP2lUYITNq/+QzbH/hsqr/37Gpf93wZ3/c8CZ/3O/mf9zv5n/c7+Z/3O/"
    "mf9xvZf/bbmT/2Szjf9rtI//zOfI/9vw0f/Z8NL/1erM/5Gilf96vp7/Z7aO/2ayjP9grIb/WqSA/1Kbdv9KkW3/QIVc/z97af9Ufab/WIGn/1"
    "+Fqf9ojK3/fJq4/1NWWf/m5uX/wZmI/96VU//TmVz/05le/8+QTP/CcRb/rVsM/6hZDP+pXAz/rV0M/65eD/9M2v//S93//07e//9R4P//Vdz+"
    "/1nb/v9Z1fr/WMzz/0PC7P8puuH/g7LA/9bZ2vkAAACBAAAAAAAAAAAAAAAAAAAAAClFNiJ1u5nik9O0/4rNrf+Ax6j/e8Oh/3XAm/9zv5n/c7"
    "+Z/3O/mf9zv5n/cLyW/225k/9hsYj/nM6n/8zgw/+5z7X/v9K2/7nKrv+GmYz/bq+Q/2CphP9hrIf/XquE/1qkf/9Rm3b/SI9p/0CDYP9Wh5X/"
    "Y4u0/2CIrv9liq7/cJKy/4Ceuf9TVVf/6uro/8mdhv/Wn2j/0aBq/8+dZ//Nll7/yIpJ/795Kf+xZxD/p14M/6RdDP+lYBf/TtP2/03W/v9O1v"
    "z/UdT7/1TS+v9Z0Pr/XM33/1zF8P9Lvev/L7Lk/2Gjvv/N19z7AAAAlwAAAAAAAAAAAAAAAAAAAAAcLyYBT4Nqh43Wsv+PzK//g8mp/37GpP94"
    "wp7/dMCa/3O/mf9zv5n/cr6Z/2+7lf9otIz/ZqeL/4uuuv+Epbr/fZ+0/4Ggsf+Bl6D/cYd//16Sd/9SknH/Vpx5/1qkfv9ZpH7/U5x2/0iQaP"
    "9bjpb/cJXC/2iQt/9nj7T/bJG0/3aXuP+Go7//U1VX/+rp5v/Qn4T/2KZx/9Okb//RoGz/zZhi/8mMUP/GiEX/xYI6/752JP+0ZxH/p14W/0nR"
    "9v9L1P3/TdP6/07Q+f9Tzvn/V8z3/1nL9v9Ywe7/Sbnq/yys4/9Nl7f/ydbe/AAAAJoAAAAAAAAAAAAAAAAAAAAAAAAAACxLPBVxtpTOj9Ky/4"
    "nJrP9/xqX/esSg/3XBm/9zv5n/c7+Z/3G9mP9rtpD/XaV7/2WYm/9vk8L/bJO9/2yTvP9qkrv/Zomv/2WCgf9Tgmj/SIFl/0uKav9SmHT/VaB7"
    "/0+bcv9Nhnz/dZvD/3Sbwf9ulrv/bpW6/3KXuv97nbz/i6jD/1NVWP/q6eb/zp+H/9qqef/Vp3b/06R0/9Ghb//OmWP/y5FY/8qQVP/JjVD/yY"
    "Y//7d4Nf9d2vb/UNT8/0zS+v9Nzfn/Ucr4/1TH9/9Vw/L/Ur3s/0Sw5v8nqeH/QpO0/8HV4f0AAQKeAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "PGRQOHe8meSO0bH/g8em/3zEof91wZv/c7+Z/3K+mP9suJP/YamD/1eXeP9rlbL/cZa//22Tuf9rkbf/aY+2/2eKsP9jhH//T35j/0N9YP9Dfm"
    "D/SIpp/1Kedf9Bg2r/Vnyb/4Knz/94oMT/dpzB/3WbwP94nMD/gaPC/5Ctx/9TVlj/6enm/8+hiP/dsX//2K18/9esf//ZroX/16yB/9Sld//T"
    "onD/0J1n/86VW/+/i1P/jfH5/4Lu//956P7/beH9/2bZ+/9l1fn/ZM71/13D7P9Ks+T/Lanf/z2Rtf++1eL9AQMFoAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAABLe2NEfcij64nQrv97xaL/cr6X/3C8lv9tupP/Y62J/1eadP9ckov/b5S9/2uRt/9njbP/ZYux/2OJsf9ih63/XIF3/0p6"
    "Xv9Bel3/Pnda/0KAYP9KkW7/NGBt/3SXvP+Ptdn/gKfL/3yiyP97oMX/fqLF/4anxv+UsMv/VFZY/+rq5//QpIz/4biJ/922iP/cto3/4LuZ/+"
    "G/n//gu5r/3riQ/9qwiP/WpXb/xpVl/5D1+v+L9P//ifH//4Tt/v+C6v7/gOb7/3ze9v92z+z/ZsHl/0W24P9DlbT/vtbj/QADBaEAAAACAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEx9ZURrsI7eftOr/3HBm/9st5H/Z7GM/1qee/9OjGr/YZCg/2uQuf9li7H/YYet/1+Fq/9dg6v/XI"
    "Kn/1R8cP9FeFv/PXZa/zt1WP9BgV7/PXNr/zZZdf+NsdD/nsLj/4601v+Ir9D/hKrM/4Snyv+Jqsr/lbLO/1RWWP/r6+j/1aqP/+S9j//euZD/"
    "4LuX/+TFqP/ny7L/6M62/+bKrf/hwaP/37aQ/82ieP+Q9vv/ivT//4bw//+B7P3/fuj8/3zj9/962/H/c8/n/3DE3/9Nu97/UZau/8nW3vwAAQ"
    "KfAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOmFNK12gfr1wxZz/ariR/1uif/9NjGr/SoRs/2WNrP9mi7P/YIas/1uBp/9Y"
    "fqT/WH6m/1h+ov9Me2z/QXpZ/z15Wv86cVLvNWNPvjRcbu07X3b/ncDb/7LT8P+ewuD/l7vb/5G21/+NsNH/jq/P/5Wz0P9UVlj/8vHv/8uklf"
    "/frH//37B//+G6k//lya3/6dS+/+3YxP/s1sH/5820/+PAnv/RqoX/jvj7/4r0//+E7v3/f+r6/3zl9v944PL/cdXt/13R6v9Qxd//QbzT/3Ok"
    "rv/N0NL8AAAAnQAAAAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAbFUNU45wfVqffO5RknD/QXtY/0qBcP9ljLH/YYau/1"
    "l/pf9TeZ//T3Wb/1F3nv9Ue6D/QnVm+DZnSsgwVkWVDhkRURYiKkM8Y33vPWF2/3ibsf/B4vr/uNr0/6TI5P+dwd//nL7d/5m62f+Zt9X/OT5F"
    "//7+/v/i3t7/y723/8Kgh//brYz/58Kl/+3Xvv/w3Mz/8NzL/+zVvf/kxKX/0q6K/4r6+/+D9P3/gu77/4Lp9/9z4PL/Wtru/0bI3v8+prz/Vq"
    "C2/53I0P/M0tL/r6+v8QAAAJwAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAArSDcpNFpJyDtvXf9CcXX/"
    "T3qV/1F8nP9Ve6L/VXuh/1V7of9XfaP/W4Ss/yY7R6cGDAQFITc8SB40RrolN0SnTHSP/UpviP9DZnv/Wn2T/5i6z/+73fX/r9Lr/57B3f+StN"
    "H/kLDM/3GIm/+doaT/y8vM/+De3P/7+vn/59/X/860n//kx6//8tbB/+3Wwv/s1sL/5seq/86qhP9/+fr/fvT7/33q9v9p5PL/StXn/z+ovv9m"
    "p8T/v9Tg/7fEzPifo6Xml5eX10VFRX8AAAAhAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMVIj1FgS5RZe"
    "E1XnP/M110/zViev9Xf6L/aY+2/2uRt/9skrj/apG2/26Wvf9ScJHYCxAXWB42RKQ3ZH3/PWJ5/VV8l/5Kc43/SW+H/zxheP83XHL/ocPY/6DE"
    "2v9NcIb/TnGG/1FyhP9VdIb/XnmJ/1RqdP9IV2D/VVpe/6elo//49/X/5drQ/9e6of/mw6n/5MGh/+C3kv/LoXr/e/n6/3Lz+P9a6/L/PsPR/1"
    "GgvP/B1+T/tcXN/2Zrbtk+P0B/GRkZNAAAAAYAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHCwwZL1Ni"
    "vDxthP85Z37/OGV9/zRgdv9Sepr/dZrB/3Wbwf91m8H/dZvB/3OYv/9vlLr/d53F/2CBo/A2X3r5Mlty/0Nshf9chaD/U32W/011jv9HcIn/QW"
    "V+/3WXq/95mq7/O11z/ztedP87XXH/QGFz/0hoev9PbX7/VHCB/196iv9XaHL/LTU6/8LBv//08u//xbar/9Kpkf/ZmWf/xpNo/0nt8P837fb/"
    "M6/L/5DA2f/G1Nz/g4mN/4GBgv+BgYGo////ENbW1gYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAKEhUokJ1jP88bYT/PnGH/zhpf/9AbIT/c5e9/3qgx/97ocf/fKLI/3yiyP96oMb/eJ7E/3KYvf97oMT/VHqd/yxTbP9AZ3//Yo2o/16Hov9b"
    "g53/Vn2X/1Z7k/9tjaD/Y4id/0NogP86X3b/PF91/zpdc/8+YHX/RGV5/0ZleP9FY3X/WXOE/1Nref9sdHn/eXl5/+jo5//Rzs3/tJue/4V5gv"
    "8ppLH/RqnM/67R5f+lsrr/UVBQ/6Wmpv3FxcX/sbGxu9/f3w////8DAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAEiEmR0J1iv5Dd43/QHWM/z9ziv82Znv/Xoak/4CmzP+Cp87/g6nP/4Sq0P+Fq9H/gqjO/3+ly/94nsT/eZ7D/3GUuP81XXn/NVtx/2"
    "mTrf9vmLL/bZWt/2uQqf9ojqf/a4yg/1R6kf9NdIz/Q2mB/0Bje/88YHb/Ol1z/zxfdf8+YXX/Nlhs/0Vhdf9GZnj/mKWs/6musf9fYmL/s7Ky"
    "/93c3P/b3eD/09ze/7LDzf+Hj5b/VjIx/9Kalv/Q2dr9xMPD/7GxscPX19cQ////AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAChMWAStPW59Nh5//RnyT/0J4jv89cIT/P2yC/3ibvv+HrNT/ia/W/4uy1/+Mstj/jLLY/4mv1f+Fq9H/f6XL/3mexf99oMT/"
    "TXOT/ylPZv9ghp//gKfB/32iuv9+obn/d5y0/3WXrv9ghJv/WH+Y/01zjf9Fa4L/P2V7/zxgd/86XnX/Ol50/zRYbP8uU2n/SmZ2/7menP/Rpa"
    "D/1pqT/41ZVP9rXFr/b2ho/2xmZv9fPjz/fzo0/7hDP/+9c2v/ztfY/8XFxf+qqqrUt7e3Fv///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAB43PxZBcYTUVJCm/0Z/lf9DeY//N2p9/0t2jv+Gqc//jbPZ/5G33f+Tud//lLrg/5S64P+Pttz/i7HX/4Sq0P"
    "98osj/fqLH/2SIqv8rUWn/PV9z/4Omv/+Mr8b/ja7E/4epwP98orn/bpOr/2GJo/9WfZf/TXSM/0Rqgf9AZHv/PGB3/ztfdv81W3L/Hk9n/385"
    "Nv/BLRr/tS8g/7Q1J/+3QDL/uUs9/71RRf++VUf/vlNF/7xNP/+5Oij/vmVY/8vT1f/Gx8j/qqqq6qmpqSUAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAlQkwuTYSZ6VaRp/9Gf5X/QniN/zNmeP9ahJ7/j7LZ/5W64f+Zv+X/nMLo/5zC6P+awOb/lrzi"
    "/5G23P+KsNb/gafN/32iyP9ylrj/MVh0/zZOXP+Jm6f/lsHX/5a5zf+OrsT/g6e+/3Watf9nj6r/WoOe/1N6kv9Jb4f/Qmd//0Bjef9CaID/KV"
    "96/1Q0O/+nHQz/qSgX/6svH/+yNib/uD4u/7tEM/+9Rzf/vkc3/75FNf++QjD/vDMf/75TQ//Kzc7/yMrL/6urq/qmpqY+AAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMlpnR1SOo/pXk6n/RoCV/z1zhv9Kd4//gaXJ/5S64P+dw+n/ocft/6TJ8P+jyf"
    "D/ocft/5zC6P+Vu+H/jrTa/4Wr0f99o8r/eZ3A/zljgP9hcXj/oJST/52Unv+UtMj/jrrR/4Opwf92nLb/aZGs/16Gof9XfZf/TnOM/0luhv9H"
    "dY7/P2+H/1s4QP+iEwL/rygW/7IyIP+1NCL/tzcm/7o8Kv+9PSv/v0Ev/8JCMP/DQS//w0Au/8U5I//CRzP/ycTD/8fNzf+rqqr/paWlXAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADJbZ0tWkaf8Wpet/0SAk/89dIf/b5a1/5O33f+awef/o8ju/6nO"
    "8v+s0vP/rNLz/6fN8v+hx+3/mb/l/5C23P+IrtT/f6TL/3ihx/9PWG7/cycn/4oZF/+ICwn/hjQ4/4eJmv9+q8T/cqjE/2Wdu/9bk7D/VYuo/0"
    "+Dn/9NepP/V2By/34rLf+lDAD/qhkH/7MzI/+4Oin/uTsq/749K//BPy3/w0Eu/8VDMP/IRTH/ykYw/8pGMf/MQiz/yUMt/8q4tv/K0tP/ra2t"
    "/6SkpHcAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyXWlLWJSp+1+bsP9Df5L/TH6U/4Snyv+YvOL/pc"
    "nu/6zR8/+z1/T/tdz0/7Ta9P+u1PP/pcvw/5zC6P+Tud//ia/V/3+mzf9zncT/XTpD/3kAAP+HBwD/gwgA/4QAAP+EBwP/gS82/3tQXf92Vmb/"
    "dVFf/3NPXP91RVD/hCss/5wRBv+qDAD/qBEA/60iD/+8Py//wUc2/8NGNv/GSDb/yUo3/8tMOP/NTjn/0E86/9FQO//SUTv/1FA6/9FGLv/Kqa"
    "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"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAKExUYXy+0P2iz9//isTS/3/Azv96vMv/X56x/0iEmv8/eY7/"
    "OnKG/zdvgf8/dIj/Tn+W/1iGn/9VgZr/TniR/z5nff8pVGn/NkJN/3Q6NP+jSkD/mh0S/5cLAP+eEQD/pRMA/6kUAP+tFQD/sxYA/7kZAP+/Gg"
    "D/xRsA/8kaAP/YSTH/7JmK/+6jlv/wopT/8qOV//Wllv/2ppf/96iX//mpmP/6qZn/+6ua//yrm//9rZ3//q2d//uPef/bwrv/2d/g/7Cvr/+a"
    "mpqNAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABIkJxNoorHOuOTx/6DP3v+Kx9X/jMrY/4zL2v"
    "9+vcz/Z6W4/1SPo/9HgJb/P3SL/zlshP81Z33/NGR6/zNgdv80YHb/N2N7/yxed/87SFX/fC8m/5IPAv+cDgD/ohIA/6gUAP+rFQD/sRYA/7cY"
    "AP+9GgD/xBwA/8ccAP/NHAD/311G/+6om//yraD/8qyf//WtoP/4rqH/+a+i//mwov/8sqL//bOk//60pP//tKb//7am//+3qP/9moT/4L+4/9"
    "vi5P+4t7f/mZmZp56engIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQGBpW7Tl8f643uv/pNTj"
    "/5vR3/+Z0N//mNHe/5LP3f+JyNf/fr3N/3Kxwf9no7b/W5ar/1OLoP9KgJb/Q3eO/z9wiP89aYP/LFx2/0MsMf+FCQD/oBEA/6YTAP+pFQD/rh"
    "UA/7UYAP+7GQD/wRsA/8gdAP/MHAD/1SMF/+d3Yf/1tqv/+LWp//m2qf/7t6r//Lmq//27rP/9vKz//r2t//++rv//wK///8Gv///CsP//xLP/"
    "/amQ/9+8sf/c4+b/v76+/5qamsGenp4KAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACQ3PASGr7"
    "mlxe74/7zg7f+r2eb/otbj/5zS4P+Vztz/jsvZ/4nI1v+FxdP/gsTS/33Az/94vMz/crfH/2qwwP9iprf/WJ+z/0WYrv9DS1j/iwgA/6MSAP+p"
    "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"
    "8CMA//prTv//3dX//+jj///n4P//6OH//+ji///p4///6+P//+zk///t5v//7uf//+7n///t5f//7OH//+fb///j1f/+3Mz/97ia/+HJv//4/f"
    "//vr29/5WVlcCcnJwGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACysrRXytbX/pNST/+ZAgD/phQA/6oUAP+0FwD/vRkA/8IcAP/LHgD/0yAA/9oiAP"
    "/iJQD/6iUA//IpA//9jHT//+vn///s5///6eP//+jg///l2///4dX//9zM///WxP//z7r//six//nCqv/yvaT/7Lig/+m3o//muab/5rup/+G2"
    "o//e0cz//v///8fHx/+enp7ItbW1BwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAr6+yPcbR0vOabWv/lgQA/6cTAP+sFAD/tRgA/70bAP/EHQD/zR8A"
    "/9ggAP/fIAD/6B8A//AbAP/4JQD//oNp//zHuf/6va3/+bSh//eumP/wp4//6aWN/+OnkP/cq5n/3bKh/9+6qv/bwLX/1sbA/9nRzv/f3Nv/5O"
    "Tm/+ns7f/v9fj69Pf39Pv7++3X19fcu7u7XQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAKamqCbAycvlooWE/5cEAP+pDAD/rg0A/7cNAP+/Dg"
    "D/xA8A/8oRAP/OFwD/0CAL/9cqGP/ZNib/00s2/9R+bP/Uo5n/0a2k/9a2sP/Wvrj/0sXD/9DNzP/U1tf/1dvd/9ne4fvZ3+Lo29/h3d7h487f"
    "4eO54uPkm+Hj44Hi4+Nj4ODgT+/v7z7w8PAu4+PjFwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACWlpoSuL7B06yZmf+LEQ7/mB0U/6Aw"
    "JP+tQjf/rVFL/6tlYf+wfXn/s5SQ/7uqqf/Du7r/xsjI/8bQ0v/K1Nf8z9XY8tDX2ubP19nT0NfZwNPZ2avU19iT0tPSdtPT1FnQz89Az87PKN"
    "LR0Bvb29oS2NjZCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAj4+SAq+vssG7urv/o5"
    "uc/62vsP+7wMH/wMrL/8PQ0f3E0NLuw8/R3cPMz8fDy82yyM7PksjMzofNz9BtzM3NUczMzTnKycklyMfHDsnIyAIAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACp"
    "qKmBv7/Bwbe6vaOytbaKsrO1cK+vsV21tbZJvb28Lry7vBy+vr8Ovb2+BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAD///////////////8HAf//////wAAA//////4AAAAP/////AAAAAf////wAAAAA///wBAAAAAB//wAAAAAAAD/8AAAAAAAAH/AAAAA"
    "AAAAf4AAAAAAAAA/gAAAAAAAAB+AAAAAAAAAH4AAAAAAAAAPgAAAAAAAAAeAAAAAAAAAB4AAAAAAAAAHgAAAAAAAAAeAAAAAAAAAB4AAAAAAAA"
    "AHgAAAAAAAAAeAAAAAAAAAB8AAAAAAAAAH4AAAAAAAAAfwAAAAAAAAA/gAAAAAAAAD/AAAAAAAAAP+AAAAAAAAA/+AAAAAAAAD/wAAAAAAAA/+"
    "AAAAAAAAP/4AAAAAAAA//AAAAAAAAD/4AAAAAAAAP/gAAAAAAAB/+AAAAAAAAH/4AAAAAAAAf/gAAAAAAAB/+AAAAAAAAH/4AAAAAAAAf/gAAA"
    "AAAAA/+AAAAAAAAD/4AAAAAAAAP/wAAAAAAAA//AAAAAAAAD/8AAAAAAAAP/4AAAAAAAA//gAAAAAAAB//AAAAAAAAH/8AAAAAAAAf/4AAAAAA"
    "AB//wAAAAAAAH//wAAAAAAAf//8YAAAAAB////gAAAAAH////AAAAAAf///8AAAAAA////wAAAAAD////AAAAAAf///8AAAAAD////wAAAAf//"
    "///AAAP//////+AD///////////////w==";

// kWin7ShieldBmp48: 9640 bytes
static const char kWin7ShieldBmp48[] =
    "KAAAADAAAABgAAAAAQAgAAAAAACAJQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAP///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAkAAAAcAAABKwEK"
    "Ej0BDBVDAQsTQQACBDIAAAAlDQ0NFf///wL///8D////A////wT///8D////Av///wEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAHEDkFOWGSDFeUyA5suOwQedH/EH7a/xGA3v8Qf97/D37Z/w151P8McMT/CmCo8wpOhdMONlmkGyAkVZqamhH///8IQkJCPD8/P55BQUGzQU"
    "FBsz09PZ0zMzM4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAABHDJNFGWmyyGT7f8koP//Ip7//x+Y+P8clPT/GZHw/xeM7v8Vi+r/E4bm/xCE5P8Rgd3/En7Y/xR+1f8We8"
    "7/Fm63/xpPedE0NzqpgYGBy9/f3/nu7+//7+/v/9HR0fJycnLEODg4cAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEYLzI6qv//O7T//z2r+/89pvX/N6Ly/y6d7/8kl+3/IZPr/x6P6P"
    "8aiuX/Fobi/xWE3v8Vgtr/GYDU/x9/zv8kfcj/JnvD/zZRaP+QnKb/5ubn/67Hz/9Jn73/n08v/7hkTv/Qx8D/k5OT0zk5OXAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHQAAADkGDQlbChgRcAsYEncLGBJ4CxgSeAsYEXgKFg50BQ0BZCBfhsFszv//U7P3/1"
    "m19P9XtfP/TrDz/z+p8f8xoe//KJvt/yWX7P8gken/G4rk/xqH4P8cht7/JYnb/y6J1f82ic//PVVp/6mqq//j5OT/ibPA/02wyP9azeH/0oE1"
    "/96CN/+3Wzj/6enp/5OTk9M4ODhwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGhgxJH8uXEXJOnlZ9ESLZ/9GkWv/SJJt/0iTbf9Gk2z/Rp"
    "Jq/0OQav9HkWv/aJFt/0eYn/+BzP//bMD4/3XE+P90w/f/Zr72/1K19P8/rfT/M6by/y+j8f8pnO7/I5Ln/yCN4/8mjeH/NJLf/0KW3P9DZ4X/"
    "m52e/+Lj4/+Drrn/SK7L/1vF6P9i1e3/xX4u/9KEOP+3fUH/xz4S/+bl4/9ubm7DNTU1TwAAAAAAAAAAAAAAAAAAAAAMGhRFQ4dl8lWrf/9Wq4"
    "H/Vad+/1Sjff9ToXr/U6F6/1GheP9Sn3n/UJ53/0uddf9RoHf/oaqV/2CjmP+L0f//hc7+/4jP+P+Gzvn/dsj5/2C/+P9Lt/j/O630/zGi6v8r"
    "ld3/JovV/yeI0v8wjdb/Qpne/1CJt/9pbnH/5ebm/53CyP9Hq8r/V8Tp/13P9P9T0fL/yXop/9CFNf/AhkT/vncz/7BOKf/WzMf9U1NTujExMS"
    "IAAAAAAAAAAAQIBhxXpoD/YLSM/1alff9Wonz/WKR+/1mkf/9YpH//WKR9/1ekfv9Wo33/VaJ7/02dd/9apX7/xLSq/2yXgP+Fzun/pd///4zT"
    "+v+K0vv/e877/2PE+/8+rej/JI7L/x5/uP8derL/HXi1/yN9u/83iMT/VZrR/1lmcf/c3Nz/v9PX/0apy/9Wxef/WM3x/1bP9v9Iz/D/w3Qe/8"
    "2BMf/Dikj/xIdA/7dpK/+0hXf/ycbG7zs7O4YAAAAAAAAAACFAMWpyxJ//Yq+L/1mngf9bp4H/XamD/16qhP9fqoX/XqqE/12pg/9dqIL/WKV+"
    "/0mddf9/t5b/18C6/3ONd/9jt63/rOP//5XZ//+G0/z/csz8/1Wz4/9nn73/g5yp/46Znv+MlZz/g5Sd/3WTqf9hlLn/VHaO/6GipP/d4OD/bL"
    "LK/0273/9Z0PL/VtH2/1LQ+P9HzvD/wGoW/8h6Kf+9h0b/xYpJ/8N9Ov+qTBb/1sG1/3Nzc8QzMzNDAAAAAClMO3R9yqX/ZrKO/1+rhv9hrYf/"
    "Y6+J/2Swiv9ksIr/Y6+J/2KuiP9grIb/WqiC/0mgeP+pyq7/4crC/3yQgv9Vqnr/dLzA/6Pj//+C1///csbx/8fKy//iycD/vre5/6WmsP+XnK"
    "j/l5ad/6memf+6qaD/YGJk/+Li4/+zzdP/PKrN/1zP8f9a0vf/UtD4/1DR9/9G0fT/vWMN/8R0Iv+6gUD/wYlL/8iFQP/IbCj/uXxn/8zKx/Q8"
    "PDyRAAAAACpMPHCF0a7/bbeV/2Wzjf9ns4z/abWP/2m1j/9ptY//aLSO/2ayjP9jr4n/XKuF/1Omfv/N3sX/3tLF/4GSiP9htIf/VqV2/2q3r/"
    "9uzf3/otb4/+7V1f97nsL/ZZO9/1uKtf9RgKr/THei/1J7oP99g4z/fX18/9/g3/9us8r/TL3h/2LZ9v9Z1vn/T9L5/0vT+P9D0/j/tVwD/79s"
    "G/+zeTj/u4RI/8WERv/ReDH/tEcS/9rMwv9SUlK6MDAwIilLO2eN1rX/esKi/226lf9suZL/brqU/3C8lv9vu5X/bLiS/2m1j/9ns43/W6yE/2"
    "aviv/o7db/4eDN/3+Phv9ouZH/X6yC/1umd/9Ro4f/Yamn/5Outv9qk7//You0/1V+pP9LdJr/SXOX/1d+ov9ianL9sLCw/snS0P9Dvtj/XNLz"
    "/2Te+f9Y4f7/R9v//0ba//8y4P3/vVAE/7toF/+7bCD/w3gw/8x9OP/feCX/3FgM/8KKfv+Hh4fYNzc3WiI6L1CO17X/iMut/3jBn/9zv5n/c8"
    "CZ/3TAmv9zv5n/b7uV/2y4kv9otI7/VaiA/5fKqP/w+N3/3ubO/4GUif9tvZb/Y7KK/16qgv9XoHX/RI5g/zmBXP9chqT/QWyU/0Rulv9AaZD/"
    "R26S/2KGqf9eYWPn0dHR/7PAvf87vtL/Z9Hn/27X8v9f3fj/R9f6/0bb/v8x4///tUUA/71QBP+1Xwz/v2wd/8dxJv/cbRP/3FIA/8J1V/+srK"
    "zuOzs7exQkHCqL1LH/k9O1/4HIp/92wZz/cr+Y/3O/mf9zv5n/c7+Z/2+8lf9ks4z/abWO/9zy0f/r/dv/3evO/4Kbjf9twJf/ZbSM/16rhf9W"
    "n3v/S5Fr/zqBU/9HeIb/UXmm/1B5nv9Ue6D/XoOm/2GGpv9iY2Tr5+Xk/9Z9SP/Vczn/0IlL/8yANv+7XwD/qUwA/6dNAP+1RQD/NOL//zTi//"
    "9B2f7/SNf8/1DU9v9Uz+v/TcTd/zCcu//Q09P7Pj4+kQAAAAB3vZrtmty9/4fMrP97xKH/c76Z/3O/mf9zv5n/dMCa/2+8lf9hsoj/mcym/9Hh"
    "xv/F2rz/vMiu/3iTg/9ls43/Yq6H/1+shP9VoHr/R49l/0GCZP9ch6P/X4Wx/1uCqP9dg6j/ao2u/3eYtf9iYmP+8vLx/9Z8Rv/LhlL/zJdg/8"
    "uRWf/EgTz/uW8b/6tdBP+pTQD/Rdb//0XW//9OzfX/Ucrz/1XI7f9WxOT/TrvX/zGVtP/Q09P+QUFBswAAAAA7YU9sm+nD/5DStP9/yKb/d8Kd"
    "/3K/mf90wJr/c7+Z/2u4j/9ip4X/gaW7/3mavP93mrb/epCh/2iDdf9RjW3/U5h2/1qmgP9Wo3f/SZBo/2eSrv9zlsP/aZC1/2OKsP9mjK//bp"
    "Gw/3+duv9fYWP/8/Ly/9V7Rv/NkV7/z5xo/82YYv/IjE//xYZE/8OAOf/CaBP/RNT//0XU/v9JyPP/TMTw/1HE7f9Uv+T/S7bT/zOWs//T1tb/"
    "QkJCswAAAAAAAAAAV4pxppvlwP+Hzaz/e8Sh/3S/mv9zv5n/bruU/12nfP9hk5j/b5LE/2qQvP9njrz/ZYiy/2OEdv9Delv/RIBi/1CXcf9LmG"
    "3/S3yE/36hz/91ncP/b5e8/22Uuf9uk7f/dpi4/4Oivf9hY2b/8vLy/9V7Rv/Tm2b/0qV0/9apff/UpXf/0Z5r/86YYf/NhkP/eOr9/3br//9r"
    "2/b/YNTy/17P8P9Zx+b/VLrZ/zaVtP/Y29z/Q0NDswAAAAAAAAAAAAAAAGqqisWU47z/fcql/3K+mf9wvZX/Y66G/1OVcv9qkrP/bZG8/2eNtP"
    "9ki7X/Y4it/1qBbf9BeVr/PXda/0eMZf85dGf/YIGp/5C03f9+pMr/d53E/3SZv/92m73/fZ6+/4mmwv9iZWf/8vHx/9Z8Rv/XpW//2a6B/964"
    "lf/iwKD/4LyZ/9uzjP/Xm2f/iu/2/4Tx/P995/n/dOD0/27X7/9kzen/WsPd/z+buf/c4eH/RUVFswAAAAAAAAAAAAAAAAAAAABYkXWwheC1/3"
    "bLov9otI3/U5dw/1GJev9skbv/ZYqx/1+Fq/9cgqv/XIKl/097YP9AfFr/PXta/z9+Xv8wV2z/h6rL/6fK6/+Ptdb/havO/32jyP98ocP/gaPD"
    "/4yqxv9kZ2n/8O/v/9mHUv/Ti1L/1qNy/9+9nP/nz7X/6tG6/+XJrf/groX/i+jw/3/t+v9+6ff/d+Pz/2zb6/9awNn/TazH/0mXuf/W29z/RU"
    "VFswAAAAAAAAAAAAAAAAAAAAAAAAAAM1dFcGi2j/pfqIH/Q4JZ/1WGhv9ojLv/XIGo/1R6of9TeKL/VX2g/0V8XP83bE3jHzsljChGTJszWXT/"
    "gaO8/8vs//+pzer/mb7d/5C12P+HrM7/h6nL/4+vzf9kZ2n/9/f3/+LZ1//l0MH/2bOW/92rg//qy67/79fE/+vTu//jtJH/h+To/3rp8/904u"
    "//YtHj/06zy/9ss8f/ttXZ/9bj4//Z2tr9QkJCswAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQGAAQ4YUrIPXFa/0x5hv9WfaL/VHqh/1F3nf9T"
    "eaL/V4Go/xcqJYQBAwEYEB0qgStBVrJJcov/RWl+/4Smu/+22fH/t9r1/57D3/+Qs9P/hqjK/4Skwf9lbXT/bnBx/7Gysv/h4d//6+no/+3j3v"
    "/hy73/5r6f/+XHqv/erIf/f9rh/2ra5/9Rv9L/W7HE/6/N0//f5+j/ysvL8YyMjM5TU1OuOTk5ewAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAsU"
    "HVksT2TjMl1y/y5bcf9Sep3/bJG4/2ySuP9skrn/cJjC/0NWcrgQHymINGF9/0pxi/9UfJf/Q2qC/yxSaf9skKT/rdDl/0Zof/9AYnb/QGF0/0"
    "Jjdv9HZnX/U259/2Bwef9hZmr/fHx8/+Df3//u6+n/49fO/82WbP/Kb0j/T8nY/0y+0f+Evsr/0N/g/8DBwetiYmLBRkZGgDk5OTgAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAGSw0dkF0jf89cIj/MmJ4/0JsiP93m8T/eqDG/3mfxf94nsT/dZzD/32iy/9Uep7/K1Zt/095k/9eiKX/Un"
    "uV/0Vuh/9bfpP/hKe6/zNYbv8xV2r/MlVq/zlcbv8/XnH/QF9x/09sfv9geor/y8vL/3d3d/+jo6P/7evq/+bd2//bsJb/dMLF/6HR1v/d4OH/"
    "jIyM/1paWqGTk5Mo////BwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEBgcZQHKG/0V8lP8/c4r/N2d8/22Ss/+EqNH/g6nP/4Sq0P+CqM"
    "7/faTK/3qhyf91mL//L1Vw/0dvhv9wmrf/a5Kr/2KJo/9pjKH/W32T/0RrhP8+Ynn/O151/zpccv89Ynf/N1pu/zhab/9Qc4T/ucfL/8nV2P+V"
    "mJn/eHh4/+Lh4f/o6Or/6Ozp/8/Pz/9sbW3/hYWF/8jIyFf///8Q////BQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAApR1SXU5Cp/0V8k/"
    "85boL/SHOL/4ap0P+Lstn/jbPZ/4602v+Mstj/hqzS/36ly/+BpMr/UXaW/ytSZ/92nrn/hqvD/4CkvP96nrX/ZIee/1R9lv9LcYn/Qmd9/zxh"
    "eP87X3b/N1tx/x9Sa/9wcn3/0ZCJ/9KBdf/MbmL/rFxT/25XU/9dVFT/XFNT/2hWVf+vuLn/r6+w/7S0tH3///8L////BAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAABAb4DQV5au/0N9k/8wZXf/Vn6Z/5K23v+UuuH/l73j/5i+5P+UuuD/jLLY/4Wr0f+Bpsv/ao6w/x5FW/9qi5//mMDY"
    "/5O3zv+IqsP/dZuy/2GKpP9Ue5b/SW6G/z9lfP89Y3v/L2SA/y1HW/+jJBb/txoP/60gGP+xMiP/tkIz/7xNP//BVET/wUk5/8VtYf/T3N7/rr"
    "Cw/6WlpqX///8H////AwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABUjqP0V5Su/z94jf83anz/c5u7/5nA5/+fxez/osju/6LI7v+dw+n/"
    "lbvh/4ux1/+Ap87/dJzB/ylVbf99hYf/prrH/5nH3/+LudT/eaW//2WTr/9YhKD/THeQ/0Jwif88eJT/OVxz/4oaEP+1Ig3/sS0d/7Q1Iv+4PC"
    "r/vkMw/8JIOP/FTTv/xUYy/8dXRv/S29v/r7Ky/6CgoMf///8E////AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABYlqz/Wpiv/zlzhv9i"
    "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"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAn5+jgLjAw9iywcStssDEjbK+w3G+yMpPu8"
    "HFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAP////+//wAA///4AAf/AAD//8AAAH8AAP//AAAAPwAA//4AAAAfAAD4AAAAAA8AAMAAAAAABwAAgAAAAAADAAAAAA"
    "AAAAMAAAAAAAAAAQAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAgAAAAAAAAADAAAAAAAAAAOAAAAAA"
    "AAAA8AAAAAAAAAD4AAAAAAAAAPwAAAAAAAAA/AAAAAADAAD4AAAAAAcAAPAAAAAABwAA8AAAAAAHAADwAAAAAAcAAPAAAAAABwAA8AAAAAAPAA"
    "DwAAAAAB8AAPAAAAAADwAA8AAAAAAPAADwAAAAAA8AAPAAAAAADwAA+AAAAAAPAAD4AAAAAA8AAPgAAAAADwAA/AAAAAAHAAD+AAAAAAcAAP8A"
    "AAAABwAA/+EAAAAHAAD//wAAAAcAAP//AAAABwAA//8AAAAHAAD//wAAAAcAAP//gAAAPwAA//+AAD//AAD//4D///8AAA==";

// kWin7ShieldBmp32: 4264 bytes
static const char kWin7ShieldBmp32[] =
    "KAAAACAAAABAAAAAAQAgAAAAAACAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////Bf///wqAgIApenp6s39/f+x/f3/venp6rnJyciIAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFLVEBD1SHBwAiPSoIQ3NRBCVCagYwVnwGNlyCBS1OfA8zUm"
    "1bfJhnhIaIqH9/f+rPz8/419fX/NfX1/vIyMj4fn5+53V1dUoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGDwsBBxEMAQcQ"
    "CwEAAAAAAAQHAwYtUDoYbq2vHYHQ7B2Q7P8alvr/FpP5/xSO9P8Ti+//KY/k/3GPqP9/f4D/1dXW/Yewwv84hq//tWY1/7yWZf/Pz8/4f39/6n"
    "R0dEoAAAAAAAAAAAAAAAAAAAAAAAAAACZMOQYtW0QLAwYEHAkUDysKFhAvCRUPLwgOBSgLGyJGOZPO2E7E//9Is/7/Oqf2/ymb7/8hlOv/G43m"
    "/ySN4/9joNH/fn+A/9fX1/9VsMz/Tb/b/z3K6v/Zfx7/z3Y4/7d0QP/Q0ND4fn5+23FxcRgAAAAAAAAAAA0YEwMKGBIsKVQ+iTRpTsY5c1bfPH"
    "pa6Tt6W+w5eFjsO3VS6VODde5sv+v/eMn//3HC9P9bufb/PKz1/y2j8v8nmev/RJ/n/3x/gv/Y2Nj/TarH/1DU7f9X4vb/P9vx/9l/F//1pCr/"
    "6pMs/8F+Qf/W1tb3eXl5pgAAAAAAAAAAEiIbPU2WcupasIT/Wq2E/1uthf9aq4L/WaqC/1Oofv9fqX//mbCe/3/A1P+Y3P//iNH6/2zH+v9Br+"
    "//KpXV/y2MyP9nmL3/kJGS/9fX1/82sdv/U933/03k9/8/2vH/2HsT//eIEP/1nSH/0Hcu/9PBqvqPj4/OcXFxHAAAAAA+cFmccMWd/1qkgP9d"
    "qIL/X6qF/16qhP9cqIP/UqN7/3Wtjf+1sKP/bauf/5zg+P+J2P//ccPv/3upwv+CmKP/iZqk/4mNkf++vr7/QKPS/zzH6f9L4vf/Run3/z/a8f"
    "/YehH/7n4H//eNGP/wmR7/zYUn/7q6uud2dnZyAAAAAE2AaKB2xaD/Yq6J/2ezjf9otI7/ZrKM/2GuiP9WqYH/msKm/8fAtP9bm3f/aL2u/4LT"
    "8/+xzuX/w7e+/4ebtf+GmrL/g4SF/9TU1P8rj6T/N8fl/z7Y7/8+2O//Pdnw/9Z5Ef/XdQr/2HkQ/9qAH/+5dDz/0NDQ93p6eqUAAAAATHxmlY"
    "jTsv9vupT/b7yV/3C8lv9tuZP/ZLKM/2Gwh//C28D/zs6//2Oigv9XrXz/WKWK/3SnpP9oj7H/RHWm/12Gqf9+f3//3t7e/819Dv/Sgh//x3of"
    "/8NyIv/GciT/Ir/l/ya85f8pvOP/Jrbd/zacyf/X19f6fX190gAAAABBZlR5meG//3zEov9zwJn/dMCa/3K+mP9mtY3/gsOa/+r31//C0bj/aK"
    "mL/2K3i/9SnGv/N4JV/0N4g/9McaD/a4ys/39/f//d3d3/2oYS/9yXJv/dkSH/zXcs/8FtKP8iw+b/JMvs/yjP7/8q1vL/M6HI/9fX1/x+fn7j"
    "AAAAADVZRz+O07L1i9Gw/3S+mv9zv5n/cr+Z/2e0jf+WwbD/ucvG/5OonP9fmX3/W6yD/1Shdf9LinP/Xoim/2GGs/94mbf/f39//9zc3P/ahh"
    "L/67Za/+iuSv/ckC7/xXYs/yq+5P9V6/7/ZOj9/x/n/f81msT/19fX/H9/f+MAAAAAN1tKBGCWfImS2rj6fsql/3O/mf9suo//YZ+M/2+Rv/9j"
    "iLv/YYGW/059X/9Ij2j/Tpd0/2OPpv99n9H/apK3/4Wkwf9/f4D/3d3d/+aSHf/msVr/7smC/+y1WP/Ohif/M7vi/3To+v806f7/KcLe/ziayP"
    "/X19f8f39/4wAAAAAAAAAAS3ljBXW6l5x9y6b0csSa/1ujev9fjZj/aYu+/16Gsf9ag5T/R31V/zd1UvJEd3f7h6jT/5O43v92ncP/i6vJ/35+"
    "f//Nzc3/2NjY/9uQI//Qhyj/7ciC/9OPK/8esdz/H+P5/y+y0f9JqMb/z8/P+s3Nzfp8fHzcAAAAAAAAAAAAAAAAaauJAlucentVlnHaSoZo/1"
    "yDn/9dgK3/U3qi/0x1gvMqVjO9IkQ7rEFje+2avdj/tdn0/5O52v+Mr9D/jZmm/4qLjP+Kior/0NDQ/9HR0f/HkUX/z5E1/zSiwf9Esc//wMDA"
    "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";

// kWin7ShieldBmp16: 1128 bytes
static const char kWin7ShieldBmp16[] =
    "KAAAABAAAAAgAAAAAQAgAAAAAABABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACNlnLQjZZzoI2Wc6CNlnPMjZZz4Mmua7GZ/k9yLjY"
    "77jY6O/I6OjpyOjo4zAAAAAAAAAAAAAAAAAAAAAClnkadCndf5NaT2/h+W8v4ThuX/KHm+/4+Pjv9zo6X/TMXs/6tdLv+ib0j/kI2K/46Ojl0A"
    "AAAAAAAAAESNYsVTkbb/lN39/2vG+v84ls//In/D/2KIp/9oqrL/ctv6/5bp/P/YsFX/xn4z/6toOf+PjYz/jo6OCUuPastXpnz/XK2E/1WWsf"
    "9todL/YZLG/29+i/9+mpj/btn5/4/m+/953fr/xYxD/8+QOv/Bcy//nXha/46OjoFemn/XcLGM/26ui/9vror/nMOs/4623f+IjI7/Ur/h/4rk"
    "+/953fr/XdL5/7BkMP+/byz/yII1/69cKv+Ojo7qYZ6BxbPixf9xs43/iL6r/6XL6/+fxej/jo6O/7NWI//fsUj/zY5B/7FmMv87xPf/Uc34/3"
    "Ha+v9Jyvf/jo6O/wAAAABioIKzZa2H6GGKqv+ew+L/r9Hv/46Ojv+0WCb/6Mdy/+C4gf/Ik2T/U874/13S+f953fr/Scr3/46Ojv8AAAAAAAAA"
    "ADReYrVVhbL/U4Oq/2qTs/9/iJD/rWc8/9utff/u17X/3byK/3nc+v9r1/n/XtL4/1y2zP+Xl5f6AAAAACNATXdUhqT/lbvj/2GTwP9wlbX/jI"
    "eU/5KHif+ghW7/w5Bv/71/W/9Ly/f/WLnR/4SXkv+Zl5b/rLCw/wAAAAA6aHagfrHP/8vr//+w2Pz/gHyF/7hQS//Mfnr/rW9v/5R6ev+Ojo7/"
    "jo6O/5qEgv/AiYH/3M7J/7e/wfgAAAAALVFbT3Kuw/+axdb/lb3e/2dodP+xOTL/t0E5/8laUf/Ydmz/6KSe/+2xq//uqaH/7ZaL/9THwf+ttb"
    "j4AAAAAAAAAAB+rLfJcrTE/12esf9BcIP/liQf/68WC//BIxX/0Tkp/+aGff/plIv/6o+G/+2OgP/Ux8H/q7S29wAAAAAAAAAAyOnuM5DE0qCg"
    "2ej9lamw/6UNBv+6EgD/zBIA/900I//okYz/6JeQ/+ick//soZf/1M7K/621tvcAAAAAAAAAAAAAAAAAAAAAytfZ6qF4d/+0AAD/yAgA/9wFAP"
    "/oSzn/6JyQ/+iVh//ol4f/7JiC/9bJw/+wuLr3AAAAAAAAAAAAAAAAAAAAAMrS1O+elJX/pXN0/6x3df+2eXb/vJqS/8G2rv/EtKv/xrar/8ey"
    "p//HyMf/try9+wAAAAAAAAAAAAAAAAAAAADN09WGx8/Q98bW2PfE1dj3wtXY98LP0/fBy873wczP98HM0PfAztD3xdDS97i9vqjgAwAAwAEAAI"
    "AAAAAAAAAAAAAAAAAAAACAAAAAwAAAAIAAAACAAAAAgAAAAMAAAADAAAAA8AAAAPAAAADwAAAA";

// kWin7PlainBmp32: 4264 bytes
static const char kWin7PlainBmp32[] =
    "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";

// kWin7PlainBmp16: 1128 bytes
static const char kWin7PlainBmp16[] =
    "KAAAABAAAAAgAAAAAQAgAAAAAABABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACNlnHUjZZyzI2WcsyNlnLMjZZ"
    "yzI2WcdQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAClnkWlCndfYNaT28B+W8vAThuXwCXPN8AJEe7wABQkjAAAAAAAAAAAA"
    "AAAAAAAAAESNYsVFj2TlRY9j5kKLX+VFh3j2lN39/2vG+v84ls//In/D/xF91P8DT47fAAMGIAAAAAAAAAAAAAAAAEuPastXpnz/XK2E/0mXaf"
    "93rpD/XKyE/0uNd/+PtsrmbYun/1BuiPsdaaPLARktLAAAAAAAAAAAAAAAAAAAAABemn/XcsOY/2y6k/9wu5H/8ffy/2y6k/9CjF7/pbG48k54"
    "ov9TdJP6nqGh6p2goOyfoaHtoKGhYQAAAAAAAAAAYZ6BxbPixf90yJn/iL6r/8vb1f9erIP/Q4tk/I6uyuRymsH/Y4qw/32Kkf+ipqbxn6Cgka"
    "ChoQMAAAAAAAAAAAAAAABioIKzZa2H6GGKqv9PdIz5SHJV0JWkrXdylq//i6/O/1Z+nv8xUmb/mpub1qCgn1+fn59fnZ2dYaCgoDUAAAAAAAAA"
    "ADReYrVhiK//Y4Kk+mR1e/+5z9X/ZIqi/1V6kP8vU2n/P2F0/7fKzP+zyMr/ssfK/7G8vv+io6PdAAAAACNATXdUhqT/lbvj/5W95v9MaH//mj"
    "9B/5V/i/9tjKX/WWZ5/5pVUP/DZ1n/xmZX/8NeSv/DtbD/pq+w/wAAAAA6aHagfrHP/8vr//+t1/z/bm13/5IOBf+TDQH/mwEA/6oEAP/JQSz/"
    "3WhT/+VnUv/eVDb/w6yj/6aws/cAAAAALVFbT3Kuw/+axdb/lLze/2Blcv+jMyj/nxsN/64SAP/AHAb/4nhn//Ccjf/2m4r/9Y13/8u7tP+mr7"
    "L3AAAAAAAAAAB+rLfJcrTE/12esf8/cob/iC8o/6gSAv++FQD/1CoP//Sgkf/6uav//ben//+vnP/Sxb//qrO19wAAAAAAAAAAyOnuM5DE0qCg"
    "2ej9lKy0/58SCf+8GgD/1hoA/+9KMv//0Mj//9nP///f0///1sj/1M7K/621tvcAAAAAAAAAAAAAAAAAAAAAytfZ6qF6ef+0AAD/0QsA/+0HAP"
    "//a1L//+DO///Wwf//2cH//8qt/9bJw/+wuLr3AAAAAAAAAAAAAAAAAAAAAMrS1O+elJX/pXN0/6x3df+2eXb/vJqS/8G2rv/EtKv/xrar/8ey"
    "p//HyMf/try9+wAAAAAAAAAAAAAAAAAAAADN09WGx8/Q98bW2PfE1dj3wtXY98LP0/fBy873wczP98HM0PfAztD3xdDS97i9vqj8DwAA+AcAAI"
    "AHAAAADwAAAAMAAAADAACAAAAAwAAAAIAAAACAAAAAgAAAAMAAAADAAAAA8AAAAPAAAADwAAAA";

static const wchar_t kWin7SkinDllName[] = L"wpccpl.win7skin.dll";

static Bytes DecodeBase64(const char* text) {
    Bytes out;
    uint32_t acc = 0;
    int bits = 0;
    for (; *text; ++text) {
        const char c = *text;
        int v;
        if (c >= 'A' && c <= 'Z') v = c - 'A';
        else if (c >= 'a' && c <= 'z') v = c - 'a' + 26;
        else if (c >= '0' && c <= '9') v = c - '0' + 52;
        else if (c == '+') v = 62;
        else if (c == '/') v = 63;
        else if (c == '=') break;
        else continue;
        acc = (acc << 6) | static_cast<uint32_t>(v);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<uint8_t>((acc >> bits) & 0xFF));
        }
    }
    return out;
}

struct Win7IconImage { uint16_t resId; uint8_t size; const char* data; };   // size 0 means 256x256
static const Win7IconImage kWin7ShieldIcons[] = {
    {1, 0, kWin7ShieldPng256}, {2, 64, kWin7ShieldBmp64}, {3, 48, kWin7ShieldBmp48},
    {4, 32, kWin7ShieldBmp32}, {5, 16, kWin7ShieldBmp16},
};
static const Win7IconImage kWin7PlainIcons[] = {
    {6, 32, kWin7PlainBmp32}, {7, 16, kWin7PlainBmp16},
};

// Resource-only DLL with RT_GROUP_ICON 1 (applet icon, referenced as ",-1") and 2 (",-2"),
// the same resource ids that WPCCPL itself uses for them.
static Bytes BuildWin7SkinIconDll() {
    using namespace pepatch;
    using namespace pepatch::detail;
    MuiResourceTypeGroup icons{3, {}}, groups{14, {}};
    auto addGroup = [&](uint32_t groupId, const Win7IconImage* images, size_t count) -> bool {
        Bytes dir;
        Push16(dir, 0);                      // reserved
        Push16(dir, 1);                      // type: icon
        Push16(dir, static_cast<uint16_t>(count));
        for (size_t i = 0; i < count; ++i) {
            Bytes data = DecodeBase64(images[i].data);
            if (data.empty()) return false;
            dir.push_back(images[i].size);   // width  (0 = 256)
            dir.push_back(images[i].size);   // height (0 = 256)
            dir.push_back(0);                // colour count
            dir.push_back(0);                // reserved
            Push16(dir, 1);                  // planes
            Push16(dir, 32);                 // bits per pixel
            Push32(dir, static_cast<uint32_t>(data.size()));
            Push16(dir, images[i].resId);
            icons.items.push_back({images[i].resId, std::move(data)});
        }
        groups.items.push_back({groupId, std::move(dir)});
        return true;
    };
    if (!addGroup(1, kWin7ShieldIcons, sizeof(kWin7ShieldIcons) / sizeof(kWin7ShieldIcons[0])) ||
        !addGroup(2, kWin7PlainIcons, sizeof(kWin7PlainIcons) / sizeof(kWin7PlainIcons[0]))) {
        return {};
    }
    return BuildResourceOnlyDll({icons, groups}, 0x0409);
}

struct OutFile { std::wstring rel; Bytes data; };

static bool BuildOutputs(const std::map<std::wstring, Bytes>& in,
                         std::vector<OutFile>& out, std::string& err) {
    auto get = [&](const wchar_t* name) -> const Bytes* {
        auto it = in.find(name);
        return it == in.end() ? nullptr : &it->second;
    };
    const Bytes* originalCpl = get(L"wpccpl.dll");
    const Bytes* wpc = get(L"wpc.dll");
    const Bytes* dui = get(L"dui70.dll");
    if (!originalCpl || !wpc || !dui) { err = "an input file is missing"; return false; }

    Bytes cpl = *originalCpl;
    if (pepatch::PatchImportName(cpl, "DUI70.dll", "DUI76.dll") != 1) {
        err = "WPCCPL: expected exactly one DUI70.dll import descriptor";
        return false;
    }
    out.clear();
    out.push_back({L"wpccpl.dll", std::move(cpl)});
    out.push_back({L"wpc.dll", *wpc});
    out.push_back({L"DUI76.dll", *dui});
    // Windows 7 skin icons: always generated (tiny), the registry view decides whether to use them.
    Bytes skinDll = BuildWin7SkinIconDll();
    if (skinDll.empty()) { err = "Windows 7 skin icon module generation failed"; return false; }
    out.push_back({kWin7SkinDllName, std::move(skinDll)});
    for (const MuiLocaleTarget& target : kMuiLocaleTargets) {
        Bytes mui = BuildWpcMui(*originalCpl, target.lang, target.folder, target.lcid);
        if (mui.empty()) { err = "WPCCPL: matching localized MUI generation failed"; return false; }
        out.push_back({std::wstring(target.folder) + L"\\wpccpl.dll.mui", std::move(mui)});
    }
    return true;
}
// ---- PURE-END ----

// =============================================================================
// PLATFORM PART: storage, verified downloads/loading, and virtual registration.
// =============================================================================
static const wchar_t* const kAppletClsid = L"{96AE8D84-A250-4520-95A5-A47A7E3C548B}";
static const wchar_t* const kVirtualFactoryClsid = L"{A2D8CFE7-7BA4-4BAD-B86B-851376B59134}";
static const wchar_t* const kLayoutFolderClsid = L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}";
static const wchar_t* const kWpcCplClass1 = L"{A167B942-CD17-4D00-BDD9-8FDC2DC158F2}";
static const wchar_t* const kWpcCplClass2 = L"{A6F1684D-AED6-401B-9786-A3E3B53C6641}";
static const wchar_t* const kPsFactoryClsid = L"{D2ED260C-38F1-4ABE-8B2B-D4A088C54416}";

struct WpcClassSpec {
    const wchar_t* clsid;
    const wchar_t* name;
    const wchar_t* threading;
    bool hasOwnAppId;
};
static const WpcClassSpec kWpcClasses[] = {
    {L"{E77E1B8B-ECF3-4AF0-8B5E-EB13739D5344}", nullptr, L"Apartment", false},
    {L"{6C482A9C-033D-45FE-A01C-9722EF35E255}", L"Activator", L"Both", true},
    {L"{5323BCB5-0431-42BE-A482-5EE76F243A28}", nullptr, L"Apartment", false},
    {L"{B016C9D5-4801-424E-89EB-DA6EB1DF53B7}", L"SecureActivator", L"Both", false},
    {L"{BB18C7A0-2186-4BE0-97D8-04847B628E02}", L"WpcProviderSupport Class", L"Apartment", false},
    {L"{D2ED260C-38F1-4ABE-8B2B-D4A088C54416}", L"PSFactoryBuffer", L"Both", false},
    {L"{D5307D61-F9BC-4DE1-94EA-1DEF24DF4BB2}", nullptr, L"Apartment", false},
    {L"{E77CC89B-7401-4C04-8CED-149DB35ADD04}", L"WindowsParentalControls Class", L"Both", false},
    {L"{355DFFAA-3B9F-435C-B428-5D44290BC5F2}", L"WpcSettingsProvider Class", L"Apartment", false},
};

struct InterfaceSpec { const wchar_t* iid; const wchar_t* name; };
static const InterfaceSpec kWpcInterfaces[] = {
    {L"{5E1D24C6-7887-410F-AF47-363C0FDD1203}", L"ISystemSettings"},
    {L"{15F125DF-C67F-4BBD-B084-093600179F0A}", L"ILogParser"},
    {L"{22243BD6-7F2A-4BEA-8A75-110A1FB95518}", L"IUserSettingsTransactionFactory"},
    {L"{F6433826-4E3A-41A3-AD8D-1A02BDCF1CB1}", L"IActivator"},
};

static const wchar_t* const kVirtualServerObjects[] = {
    L"{FC79209B-7814-49A1-A462-FCD2107EE6C1}",
    L"{E77E1B8B-ECF3-4AF0-8B5E-EB13739D5344}",
    L"{60664CAF-AF0D-0005-A300-5C7D25FF22A0}",
    L"{D5307D61-F9BC-4DE1-94EA-1DEF24DF4BB2}",
    L"{6C482A9C-033D-45FE-A01C-9722EF35E255}",
    L"{4F9183D4-ED7D-468A-A983-3F58D50566CE}",
    L"{9A5EA990-3034-4D6F-9128-01F3C61022BC}",
    L"{E77CC89B-7401-4C04-8CED-149DB35ADD04}",
    L"{29D365AE-D23B-4004-8658-5ED2A59CD77C}",
};

static std::atomic<HMODULE> g_hWpcCpl{nullptr};
static std::atomic<HMODULE> g_hWpc{nullptr};

static const DWORD kDownloadTimeoutMs = 20000;
static const int kMaxDownloadAttempts = 3;
static const DWORD kRetryDelayMs = 3000;
static const size_t kMaxDownloadBytes = 16u * 1024 * 1024;

static std::atomic<bool> g_shuttingDown{false};
static HANDLE g_stopEvent = nullptr;
[[clang::no_destroy]] static std::optional<std::thread> g_setupThread;
static std::mutex g_netMutex;
static HINTERNET g_hNet = nullptr;
static HINTERNET g_hUrl = nullptr;

static std::wstring StorageDir() {
    std::vector<wchar_t> buf(32768);
    const size_t n = Wh_GetModStoragePath(buf.data(), buf.size());
    if (n == 0 || n >= buf.size()) return L"";
    return std::wstring(buf.data(), n);
}

static bool IsRunningAsAmd64() {
    typedef BOOL(WINAPI * IsWow64Process2_t)(HANDLE, USHORT*, USHORT*);
    USHORT proc = 0, native = 0;
    HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
    auto f = k32 ? reinterpret_cast<IsWow64Process2_t>(
                       reinterpret_cast<void*>(GetProcAddress(k32, "IsWow64Process2")))
                 : nullptr;
    if (f && f(GetCurrentProcess(), &proc, &native))
        return (proc ? proc : native) == IMAGE_FILE_MACHINE_AMD64;
    SYSTEM_INFO si{};
    GetNativeSystemInfo(&si);
    return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
}

// ---- files ---------------------------------------------------------------------------------
static bool ReadWholeFile(const std::wstring& path, Bytes& out) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER sz{};
    bool ok = GetFileSizeEx(h, &sz) && sz.QuadPart >= 0 && (size_t)sz.QuadPart <= kMaxDownloadBytes;
    if (ok) {
        out.resize((size_t)sz.QuadPart);
        size_t done = 0;
        while (ok && done < out.size()) {
            DWORD rd = 0;
            ok = ReadFile(h, &out[done], (DWORD)std::min<size_t>(out.size() - done, 1u << 20), &rd, nullptr) && rd > 0;
            done += rd;
        }
    }
    CloseHandle(h);
    return ok;
}

static void EnsureDirs(const std::wstring& path) {   // creates every folder level of `path`
    for (size_t i = 3; i < path.size(); ++i)
        if (path[i] == L'\\') CreateDirectoryW(path.substr(0, i).c_str(), nullptr);
    CreateDirectoryW(path.c_str(), nullptr);
}

static bool WriteFileAtomic(const std::wstring& path, const Bytes& data) {
    const std::wstring tmp = path + L".tmp" + std::to_wstring(GetCurrentProcessId());
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    size_t done = 0;
    bool ok = true;
    while (ok && done < data.size()) {
        DWORD wr = 0;
        ok = WriteFile(h, &data[done], (DWORD)std::min<size_t>(data.size() - done, 1u << 20), &wr, nullptr) && wr > 0;
        done += wr;
    }
    CloseHandle(h);
    if (ok) ok = MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE;
    if (!ok) DeleteFileW(tmp.c_str());
    return ok;
}

// ---- SHA-256 (CryptoAPI) -------------------------------------------------------------------
static std::string Sha256Hex(const Bytes& data) {
    std::string hex;
    HCRYPTPROV prov = 0;
    HCRYPTHASH hash = 0;
    if (CryptAcquireContextW(&prov, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(prov, CALG_SHA_256, 0, 0, &hash)) {
            bool ok = true;
            for (size_t off = 0; ok && off < data.size(); off += (1u << 20))
                ok = CryptHashData(hash, &data[off], (DWORD)std::min<size_t>(data.size() - off, 1u << 20), 0) != FALSE;
            BYTE d[32];
            DWORD cb = 32;
            if (ok && CryptGetHashParam(hash, HP_HASHVAL, d, &cb, 0) && cb == 32) {
                static const char* digits = "0123456789abcdef";
                for (BYTE b : d) { hex.push_back(digits[b >> 4]); hex.push_back(digits[b & 15]); }
            }
            CryptDestroyHash(hash);
        }
        CryptReleaseContext(prov, 0);
    }
    return hex;
}

// ---- download (WinInet, time-limited, cancellable from Wh_ModUninit) ------------------------
static void CancelInFlightDownload() {
    std::lock_guard<std::mutex> l(g_netMutex);
    if (g_hUrl) { InternetCloseHandle(g_hUrl); g_hUrl = nullptr; }
    if (g_hNet) { InternetCloseHandle(g_hNet); g_hNet = nullptr; }
}

static bool Download(const std::wstring& url, Bytes& out) {
    out.clear();
    HINTERNET net = InternetOpenW(L"Windhawk Family Safety Applet Restorer", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!net) return false;
    { std::lock_guard<std::mutex> l(g_netMutex); g_hNet = net; }
    if (g_shuttingDown.load()) { CancelInFlightDownload(); return false; }
    DWORD to = kDownloadTimeoutMs;
    InternetSetOptionW(net, INTERNET_OPTION_CONNECT_TIMEOUT, &to, sizeof(to));
    InternetSetOptionW(net, INTERNET_OPTION_SEND_TIMEOUT, &to, sizeof(to));
    InternetSetOptionW(net, INTERNET_OPTION_RECEIVE_TIMEOUT, &to, sizeof(to));
    HINTERNET h = InternetOpenUrlW(net, url.c_str(), nullptr, 0,
                                   INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_UI, 0);
    if (!h) { CancelInFlightDownload(); return false; }
    {
        std::lock_guard<std::mutex> l(g_netMutex);
        if (!g_hNet) { InternetCloseHandle(h); return false; }   // cancelled meanwhile
        g_hUrl = h;
    }
    bool ok = false;
    DWORD status = 0, len = sizeof(status), idx = 0;
    if (HttpQueryInfoW(h, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &len, &idx) && status == HTTP_STATUS_OK) {
        const ULONGLONG start = GetTickCount64();
        std::vector<BYTE> buf(65536);
        ok = true;
        for (;;) {
            if (g_shuttingDown.load() || GetTickCount64() - start > 4 * kDownloadTimeoutMs) { ok = false; break; }
            DWORD rd = 0;
            if (!InternetReadFile(h, buf.data(), (DWORD)buf.size(), &rd)) { ok = false; break; }
            if (rd == 0) break;   // end of file
            out.insert(out.end(), buf.begin(), buf.begin() + rd);
            if (out.size() > kMaxDownloadBytes) { ok = false; break; }
        }
    }
    // close whatever the cancel path has not already closed
    {
        std::lock_guard<std::mutex> l(g_netMutex);
        if (g_hUrl == h) g_hUrl = nullptr; else h = nullptr;
        if (g_hNet == net) g_hNet = nullptr; else net = nullptr;
    }
    if (h) InternetCloseHandle(h);
    if (net) InternetCloseHandle(net);
    if (!ok) out.clear();
    return ok;
}

// Returns the verified bytes of one original file: from <dir>\dl if already there and correct,
// otherwise downloaded. Anything whose size or SHA-256 differs from the pinned values is rejected.
static bool GetVerifiedFile(const std::wstring& dlDir, const FileSpec& f, Bytes& data) {
    const std::wstring path = dlDir + L"\\" + f.name;
    auto good = [&](const Bytes& b) { return b.size() == f.size && Sha256Hex(b) == f.sha256; };
    if (ReadWholeFile(path, data) && good(data)) return true;
    const std::wstring url = std::wstring(L"https://msdl.microsoft.com/download/symbols/") + f.key;
    for (int attempt = 1; attempt <= kMaxDownloadAttempts && !g_shuttingDown.load(); ++attempt) {
        Wh_Log(L"Downloading %s (attempt %d)", f.name, attempt);
        if (Download(url, data) && good(data)) {
            if (!WriteFileAtomic(path, data)) Wh_Log(L"Could not save %s (continuing in memory)", f.name);
            return true;
        }
        Wh_Log(L"%s: download failed or did not match the pinned SHA-256", f.name);
        if (attempt < kMaxDownloadAttempts && g_stopEvent &&
            WaitForSingleObject(g_stopEvent, kRetryDelayMs) == WAIT_OBJECT_0) break;
    }
    data.clear();
    return false;
}

// ---- write generated files (skip if identical; a locked, different file is left alone) ------
static bool WriteOutputs(const std::wstring& binDir, const std::vector<OutFile>& files) {
    for (const OutFile& f : files) {
        const std::wstring path = binDir + L"\\" + f.rel;
        Bytes cur;
        if (ReadWholeFile(path, cur) && cur == f.data) continue;
        const size_t slash = path.find_last_of(L'\\');
        EnsureDirs(path.substr(0, slash));
        if (!WriteFileAtomic(path, f.data)) {
            Wh_Log(L"Could not write %s (err %u)", path.c_str(), GetLastError());
            if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) return false;
            Wh_Log(L"Keeping the existing file (it may be in use)");
        }
    }
    return true;
}


static HMODULE LoadVerified(const std::wstring& path, const Bytes& expected, DWORD flags,
                            HMODULE(WINAPI* loader)(LPCWSTR, HANDLE, DWORD)) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Cannot open %s for verification (error %u)", path.c_str(), GetLastError());
        return nullptr;
    }
    BY_HANDLE_FILE_INFORMATION info{};
    bool ok = GetFileInformationByHandle(h, &info) &&
              !(info.dwFileAttributes & (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DIRECTORY)) &&
              info.nFileSizeHigh == 0 && info.nFileSizeLow == expected.size() && !expected.empty();
    if (ok) {
        Bytes data(expected.size());
        size_t done = 0;
        while (ok && done < data.size()) {
            DWORD rd = 0;
            ok = ReadFile(h, &data[done], (DWORD)std::min<size_t>(data.size() - done, 1u << 20), &rd, nullptr) && rd > 0;
            done += rd;
        }
        ok = ok && memcmp(data.data(), expected.data(), data.size()) == 0;
    }
    HMODULE m = nullptr;
    if (ok) {
        m = loader(path.c_str(), nullptr, flags);   // handle still open
    } else {
        Wh_Log(L"%s does not match the expected contents; not loading it (restart explorer.exe to regenerate it)", path.c_str());
    }
    CloseHandle(h);
    return m;
}

// ---- Embedded six-language runtime and resource hooks -----------------------
// This code is inserted after the shared file/setup helpers and before vreg.
enum class LanguageSettingMode { Auto, EN_US, IT_IT, FA_IR, ES_ES, FR_FR, PT_BR };

static std::atomic<bool> g_forceTranslations{true};
static std::atomic<int> g_selectedLanguageMode{static_cast<int>(LanguageSettingMode::Auto)};
static std::atomic<HMODULE> g_hLocalizedResources{nullptr};
static std::mutex g_localizedResourceMutex;
static std::wstring g_localizedResourcePath;
static std::atomic<int> g_localizedResourceLanguage{-1};
static std::atomic<uint32_t> g_localizedResourceSeq{0};

struct ScopedResourceUpdateHandle {
    HANDLE handle = nullptr;
    ScopedResourceUpdateHandle() = default;
    explicit ScopedResourceUpdateHandle(HANDLE h) : handle(h) {}
    ScopedResourceUpdateHandle(const ScopedResourceUpdateHandle&) = delete;
    ScopedResourceUpdateHandle& operator=(const ScopedResourceUpdateHandle&) = delete;
    ~ScopedResourceUpdateHandle() { if (handle) EndUpdateResourceW(handle, TRUE); }
    HANDLE Get() const { return handle; }
    explicit operator bool() const { return handle != nullptr; }
    bool Commit() {
        if (!handle) return false;
        HANDLE h = handle;
        handle = nullptr;
        return EndUpdateResourceW(h, FALSE) != FALSE;
    }
};

static MuiLanguage DetectWindowsUiLanguage() {
    ULONG numLangs = 0, cchBuf = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLangs, nullptr, &cchBuf) && cchBuf > 0) {
        std::vector<wchar_t> buf(cchBuf, L'\0');
        if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLangs, buf.data(), &cchBuf) && buf[0])
            return DetectMuiLanguage(buf.data());
    }
    const LANGID langId = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(langId)) {
        case LANG_ITALIAN: return MuiLanguage::IT_IT;
        case LANG_PERSIAN: return MuiLanguage::FA_IR;
        case LANG_SPANISH: return MuiLanguage::ES_ES;
        case LANG_FRENCH: return MuiLanguage::FR_FR;
        case LANG_PORTUGUESE: return MuiLanguage::PT_BR;
        default: return MuiLanguage::EN_US;
    }
}

static MuiLanguage GetCurrentEmbeddedLanguage() {
    switch (static_cast<LanguageSettingMode>(g_selectedLanguageMode.load())) {
        case LanguageSettingMode::EN_US: return MuiLanguage::EN_US;
        case LanguageSettingMode::IT_IT: return MuiLanguage::IT_IT;
        case LanguageSettingMode::FA_IR: return MuiLanguage::FA_IR;
        case LanguageSettingMode::ES_ES: return MuiLanguage::ES_ES;
        case LanguageSettingMode::FR_FR: return MuiLanguage::FR_FR;
        case LanguageSettingMode::PT_BR: return MuiLanguage::PT_BR;
        case LanguageSettingMode::Auto:
        default: return DetectWindowsUiLanguage();
    }
}

// Identifies the built private resource module: embedded language plus skin.
static int CurrentResourceVariant() {
    return static_cast<int>(GetCurrentEmbeddedLanguage()) | (static_cast<int>(CurrentSkin()) << 8);
}

static void LoadSkinSetting() {
    PCWSTR raw = Wh_GetStringSetting(L"skin");
    Skin skin = Skin::Win8;
    if (raw) {
        if (_wcsicmp(raw, L"win7") == 0) skin = Skin::Win7;
        Wh_FreeStringSetting(raw);
    }
    g_skin.store(static_cast<int>(skin));
}

static void LoadLanguageSetting() {
    PCWSTR raw = Wh_GetStringSetting(L"language");
    LanguageSettingMode mode = LanguageSettingMode::Auto;
    if (raw) {
        if (_wcsicmp(raw, L"en-US") == 0 || _wcsicmp(raw, L"en") == 0) mode = LanguageSettingMode::EN_US;
        else if (_wcsicmp(raw, L"it-IT") == 0 || _wcsicmp(raw, L"it") == 0) mode = LanguageSettingMode::IT_IT;
        else if (_wcsicmp(raw, L"fa-IR") == 0 || _wcsicmp(raw, L"fa") == 0) mode = LanguageSettingMode::FA_IR;
        else if (_wcsicmp(raw, L"es-ES") == 0 || _wcsicmp(raw, L"es") == 0) mode = LanguageSettingMode::ES_ES;
        else if (_wcsicmp(raw, L"fr-FR") == 0 || _wcsicmp(raw, L"fr") == 0) mode = LanguageSettingMode::FR_FR;
        else if (_wcsicmp(raw, L"pt-BR") == 0 || _wcsicmp(raw, L"pt-PT") == 0 ||
                 _wcsicmp(raw, L"pt") == 0) mode = LanguageSettingMode::PT_BR;
        Wh_FreeStringSetting(raw);
    }
    g_selectedLanguageMode.store(static_cast<int>(mode));
}

static const wchar_t* GetEmbeddedTranslation(UINT id) {
    if (!g_forceTranslations.load()) return nullptr;
    const MuiLanguage language = GetCurrentEmbeddedLanguage();
    if (const wchar_t* text = GetWpcString(id, language)) return text;
    if (language != MuiLanguage::EN_US) return GetWpcString(id, MuiLanguage::EN_US);
    return nullptr;
}

static const wchar_t* GetLocalizedDisplayName() {
    if (const wchar_t* text = GetEmbeddedTranslation(100)) return text;
    return GetWpcString(100, MuiLanguage::EN_US);
}

static const wchar_t* GetLocalizedInfoTip() {
    if (const wchar_t* text = GetEmbeddedTranslation(101)) return text;
    return GetWpcString(101, MuiLanguage::EN_US);
}

// Classic Windows 7/8 task link names for Family Safety (category 9)
static void GetLocalizedTaskNames(const wchar_t*& task1, const wchar_t*& task2) {
    const bool win7 = CurrentSkin() == Skin::Win7;
    switch (GetCurrentEmbeddedLanguage()) {
        case MuiLanguage::IT_IT:
            task1 = win7 ? L"Imposta Controllo genitori" : L"Imposta Sicurezza famiglia";
            task2 = win7 ? L"Modifica le impostazioni di Controllo genitori"
                         : L"Modifica le impostazioni di Sicurezza famiglia";
            break;
        case MuiLanguage::FA_IR:
            if (win7) {
                task1 = L"تنظیم کنترل والدین";
                task2 = L"مدیریت تنظیمات کنترل والدین";
            } else {
                task1 = L"تنظیم ایمنی خانواده";
                task2 = L"مدیریت تنظیمات ایمنی خانواده";
            }
            break;
        case MuiLanguage::ES_ES:
            task1 = win7 ? L"Configurar Control parental" : L"Configurar Protección infantil";
            task2 = win7 ? L"Cambiar la configuración de Control parental"
                         : L"Cambiar la configuración de Protección infantil";
            break;
        case MuiLanguage::FR_FR:
            task1 = L"Configurer le Contrôle parental";
            task2 = L"Modifier les paramètres du Contrôle parental";
            break;
        case MuiLanguage::PT_BR:
            task1 = win7 ? L"Configurar o Controle dos Pais" : L"Configurar a Proteção para a Família";
            task2 = win7 ? L"Alterar as configurações do Controle dos Pais"
                         : L"Alterar as configurações da Proteção para a Família";
            break;
        default:
            task1 = win7 ? L"Set up Parental Controls" : L"Set up Family Safety";
            task2 = win7 ? L"Change Parental Controls settings" : L"Change Family Safety settings";
            break;
    }
}

static bool WriteTasksXml(const std::wstring& binDir) {
    try {
        const wchar_t* task1 = nullptr;
        const wchar_t* task2 = nullptr;
        GetLocalizedTaskNames(task1, task2);

        // Simple XML for classic Control Panel task links (category 9 = User Accounts)
        std::wstring xml =
            L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n"
            L"<applications xmlns=\"http://schemas.microsoft.com/windows/cpltasks/v1\" "
            L"xmlns:sh=\"http://schemas.microsoft.com/windows/tasks/v1\">\r\n"
            L"  <application id=\"{96AE8D84-A250-4520-95A5-A47A7E3C548B}\">\r\n"
            L"    <sh:task id=\"{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}\">\r\n"
            L"      <sh:name>" + std::wstring(task1) + L"</sh:name>\r\n"
            L"      <sh:keywords>family safety parental controls</sh:keywords>\r\n"
            L"      <sh:command>explorer.exe shell:::{96AE8D84-A250-4520-95A5-A47A7E3C548B}</sh:command>\r\n"
            L"    </sh:task>\r\n"
            L"    <sh:task id=\"{B2C3D4E5-F6A7-8901-BCDE-F12345678901}\">\r\n"
            L"      <sh:name>" + std::wstring(task2) + L"</sh:name>\r\n"
            L"      <sh:keywords>family safety settings manage</sh:keywords>\r\n"
            L"      <sh:command>explorer.exe shell:::{96AE8D84-A250-4520-95A5-A47A7E3C548B}</sh:command>\r\n"
            L"    </sh:task>\r\n"
            L"    <category id=\"9\">\r\n"
            L"      <sh:task idref=\"{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}\"/>\r\n"
            L"      <sh:task idref=\"{B2C3D4E5-F6A7-8901-BCDE-F12345678901}\"/>\r\n"
            L"    </category>\r\n"
            L"  </application>\r\n"
            L"</applications>\r\n";

        const std::wstring path = binDir + L"\\FamilySafety.tasks.xml";
        // Write as UTF-16 LE with BOM
        Bytes data;
        data.push_back(0xFF);
        data.push_back(0xFE);
        for (wchar_t c : xml) {
            data.push_back(static_cast<uint8_t>(c & 0xFF));
            data.push_back(static_cast<uint8_t>((c >> 8) & 0xFF));
        }
        return WriteFileAtomic(path, data);
    } catch (...) {
        Wh_Log(L"WriteTasksXml failed");
        return false;
    }
}

// A private resource module preserves every WPCCPL resource but neutralizes its
// external-MUI marker and carries the selected embedded RT_STRING blocks.
static bool DisableMuiConfigInPrivateCopy(const std::wstring& path) {
    Bytes buf;
    if (!ReadWholeFile(path, buf) || buf.size() < sizeof(IMAGE_DOS_HEADER)) return false;
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(buf.data());
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return false;
    const size_t ntOff = static_cast<size_t>(dos->e_lfanew);
    if (ntOff + sizeof(IMAGE_NT_HEADERS64) > buf.size()) return false;
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(buf.data() + ntOff);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        return false;
    const IMAGE_DATA_DIRECTORY& resDir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
    if (!resDir.VirtualAddress || resDir.Size < sizeof(IMAGE_RESOURCE_DIRECTORY)) return false;
    const auto* sections = IMAGE_FIRST_SECTION(nt);
    const WORD numSections = nt->FileHeader.NumberOfSections;
    auto rvaToOffset = [&](DWORD rva) -> size_t {
        for (WORD i = 0; i < numSections; ++i) {
            const DWORD va = sections[i].VirtualAddress;
            const DWORD rawSize = sections[i].SizeOfRawData;
            const DWORD virtSize = sections[i].Misc.VirtualSize;
            const DWORD span = rawSize > virtSize ? rawSize : virtSize;
            if (rva >= va && rva < va + span) {
                const DWORD delta = rva - va;
                if (delta >= rawSize) return static_cast<size_t>(-1);
                const size_t fileOff = static_cast<size_t>(sections[i].PointerToRawData) + delta;
                return fileOff < buf.size() ? fileOff : static_cast<size_t>(-1);
            }
        }
        return static_cast<size_t>(-1);
    };
    const size_t resBase = rvaToOffset(resDir.VirtualAddress);
    if (resBase == static_cast<size_t>(-1) || resBase + sizeof(IMAGE_RESOURCE_DIRECTORY) > buf.size())
        return false;
    const auto* root = reinterpret_cast<const IMAGE_RESOURCE_DIRECTORY*>(buf.data() + resBase);
    const size_t entriesOff = resBase + sizeof(IMAGE_RESOURCE_DIRECTORY);
    const size_t namedCount = root->NumberOfNamedEntries;
    if (entriesOff + namedCount * sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) > buf.size()) return false;
    const auto* entries = reinterpret_cast<const IMAGE_RESOURCE_DIRECTORY_ENTRY*>(buf.data() + entriesOff);
    for (size_t i = 0; i < namedCount; ++i) {
        if (!entries[i].NameIsString) continue;
        const size_t strOff = resBase + entries[i].NameOffset;
        if (strOff + sizeof(WORD) + 3 * sizeof(WCHAR) > buf.size()) continue;
        const WORD len = *reinterpret_cast<const WORD*>(buf.data() + strOff);
        auto* chars = reinterpret_cast<WCHAR*>(buf.data() + strOff + sizeof(WORD));
        if (len == 3 && chars[0] == L'M' && chars[1] == L'U' && chars[2] == L'I') {
            chars[0] = L'C';
            return WriteFileAtomic(path, buf);
        }
    }
    return false;
}

static void CleanupStaleLocalizedResourceModules(const std::wstring& dir, const std::wstring& keepPath) {
    if (dir.empty()) return;
    WIN32_FIND_DATAW fd{};
    HANDLE find = FindFirstFileW((dir + L"\\wpccpl.resources*.dll").c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const std::wstring full = dir + L"\\" + fd.cFileName;
        if (!keepPath.empty() && _wcsicmp(full.c_str(), keepPath.c_str()) == 0) continue;
        DeleteFileW(full.c_str());
    } while (FindNextFileW(find, &fd));
    FindClose(find);
}

static void ReleaseLocalizedResourceModuleLocked() {
    // Do not unload a resource image that DirectUI may still reference.
    g_hLocalizedResources.store(nullptr);
    g_localizedResourceLanguage.store(-1);
    g_localizedResourcePath.clear();
}

static void ReleaseLocalizedResourceModule() {
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    ReleaseLocalizedResourceModuleLocked();
}

static bool LocalizedResourceModuleUpToDateLocked() {
    if (g_localizedResourcePath.empty() ||
        GetFileAttributesW(g_localizedResourcePath.c_str()) == INVALID_FILE_ATTRIBUTES) return false;
    return g_localizedResourceLanguage.load() == CurrentResourceVariant();
}

static bool BuildLocalizedResourceModule(const std::wstring& sourceDllPath, const std::wstring& dir) {
    try {
        if (sourceDllPath.empty() || dir.empty()) return false;
        const MuiLanguage selected = GetCurrentEmbeddedLanguage();
        const uint32_t seq = g_localizedResourceSeq.fetch_add(1, std::memory_order_relaxed) + 1;
        wchar_t fileName[128] = {};
        swprintf_s(fileName, ARRAYSIZE(fileName), L"\\wpccpl.resources-v1-%d-%lu-%u.dll",
                   static_cast<int>(selected), GetCurrentProcessId(), seq);
        const std::wstring targetPath = dir + fileName;
        CleanupStaleLocalizedResourceModules(dir, L"");
        if (!CopyFileW(sourceDllPath.c_str(), targetPath.c_str(), FALSE)) return false;
        SetFileAttributesW(targetPath.c_str(), FILE_ATTRIBUTE_NORMAL);
        if (!DisableMuiConfigInPrivateCopy(targetPath)) {
            Wh_Log(L"Localized WPCCPL resource module could not neutralize MUI config");
            DeleteFileW(targetPath.c_str());
            return false;
        }
        ScopedResourceUpdateHandle update(BeginUpdateResourceW(targetPath.c_str(), FALSE));
        if (!update) { DeleteFileW(targetPath.c_str()); return false; }
        const WORD targetLangIds[] = {0x0409, GetEmbeddedLanguageId(selected)};
        for (const EmbeddedStringBlock& row : kEmbeddedStringBlocks) {
            Bytes block;
            if (!BuildWpcStringBlockBytes(row.id, selected, block)) {
                DeleteFileW(targetPath.c_str());
                return false;
            }
            for (size_t i = 0; i < ARRAYSIZE(targetLangIds); ++i) {
                if (i > 0 && targetLangIds[i] == targetLangIds[0]) continue;
                if (!UpdateResourceW(update.Get(), MAKEINTRESOURCEW(6), MAKEINTRESOURCEW(row.id), targetLangIds[i],
                                     block.data(), static_cast<DWORD>(block.size()))) {
                    DeleteFileW(targetPath.c_str());
                    return false;
                }
            }
        }
        if (!update.Commit()) { DeleteFileW(targetPath.c_str()); return false; }
        g_localizedResourcePath = targetPath;
        g_localizedResourceLanguage.store(CurrentResourceVariant());
        CleanupStaleLocalizedResourceModules(dir, targetPath);
        return true;
    } catch (const std::exception& e) {
        Wh_Log(L"BuildLocalizedResourceModule exception: %S", e.what());
        return false;
    } catch (...) {
        Wh_Log(L"BuildLocalizedResourceModule unknown exception");
        return false;
    }
}

static HMODULE EnsureLocalizedResourceModuleLoaded() {
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    const int wanted = CurrentResourceVariant();
    if (HMODULE existing = g_hLocalizedResources.load()) {
        if (g_localizedResourceLanguage.load() == wanted) return existing;
        ReleaseLocalizedResourceModuleLocked();
    }
    if (!LocalizedResourceModuleUpToDateLocked()) {
        const std::wstring dir = StorageDir();
        if (dir.empty()) return nullptr;
        const std::wstring binDir = dir + L"\\bin";
        const std::wstring cplPath = binDir + L"\\wpccpl.dll";
        if (GetFileAttributesW(cplPath.c_str()) == INVALID_FILE_ATTRIBUTES ||
            !BuildLocalizedResourceModule(cplPath, binDir)) return nullptr;
    }
    HMODULE loaded = LoadLibraryExW(g_localizedResourcePath.c_str(), nullptr,
                                    LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!loaded) {
        Wh_Log(L"Failed to load localized WPCCPL resource module (error=%lu)", GetLastError());
        return nullptr;
    }
    g_hLocalizedResources.store(loaded);
    return loaded;
}

using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
using LoadStringA_t = int(WINAPI*)(HINSTANCE, UINT, LPSTR, int);
using XResourceProviderCreate_t = HRESULT(__fastcall*)(HINSTANCE, const wchar_t*, const wchar_t*, const wchar_t*, void**);
using FindResourceExW_t = HRSRC(WINAPI*)(HMODULE, LPCWSTR, LPCWSTR, WORD);
using LoadResource_t = HGLOBAL(WINAPI*)(HMODULE, HRSRC);
static LoadStringW_t LoadStringWOriginalUser32 = nullptr;
static LoadStringW_t LoadStringWOriginalKernelBase = nullptr;
static LoadStringA_t LoadStringAOriginalUser32 = nullptr;
static LoadStringA_t LoadStringAOriginalKernelBase = nullptr;
static XResourceProviderCreate_t XResourceProviderCreateOriginalDui76 = nullptr;
static FindResourceExW_t FindResourceExWOriginal = nullptr;
static LoadResource_t LoadResourceOriginal = nullptr;

static std::wstring g_binDirLower;
static std::mutex g_resModuleCacheMutex;
static std::map<uintptr_t, bool> g_resModuleCache;

static bool IsModWpcCplModule(uintptr_t base) {
    if (g_binDirLower.empty()) return false;
    {
        std::lock_guard<std::mutex> lock(g_resModuleCacheMutex);
        auto it = g_resModuleCache.find(base);
        if (it != g_resModuleCache.end()) return it->second;
    }
    bool match = false;
    wchar_t path[MAX_PATH] = {};
    if (GetModuleFileNameW(reinterpret_cast<HMODULE>(base), path, ARRAYSIZE(path))) {
        std::wstring value(path);
        for (wchar_t& c : value) c = towlower(c);
        if (value.compare(0, g_binDirLower.size(), g_binDirLower) == 0) {
            const wchar_t* name = value.c_str() + g_binDirLower.size();
            match = wcscmp(name, L"wpccpl.dll") == 0 || wcsncmp(name, L"wpccpl.resources", 16) == 0;
        }
    }
    std::lock_guard<std::mutex> lock(g_resModuleCacheMutex);
    g_resModuleCache[base] = match;
    return match;
}

static bool IsTargetResourceModule(HINSTANCE instance) {
    if (!instance) return false;
    const uintptr_t base = reinterpret_cast<uintptr_t>(instance) & ~static_cast<uintptr_t>(3);
    const auto same = [base](HMODULE module) {
        return module && (reinterpret_cast<uintptr_t>(module) & ~static_cast<uintptr_t>(3)) == base;
    };
    return same(g_hWpcCpl.load()) || same(g_hLocalizedResources.load()) || IsModWpcCplModule(base);
}

static int HandleLoadStringW(LoadStringW_t original, HINSTANCE instance, UINT id,
                             LPWSTR buffer, int bufferChars) {
    try {
        if (!StabilityBudgetExceeded() && IsTargetResourceModule(instance)) {
            if (bufferChars == 0 && buffer && g_forceTranslations.load()) {
                if (HMODULE localized = EnsureLocalizedResourceModuleLoaded())
                    return original(localized, id, buffer, 0);
            }
            if (buffer && bufferChars > 0) {
                if (const wchar_t* translated = GetEmbeddedTranslation(id)) {
                    const size_t copyLen = std::min<size_t>(wcslen(translated), static_cast<size_t>(bufferChars - 1));
                    wmemcpy(buffer, translated, copyLen);
                    buffer[copyLen] = L'\0';
                    return static_cast<int>(copyLen);
                }
            }
        }
    } catch (const std::exception&) {
        NoteSecondaryError(L"HandleLoadStringW");
    } catch (...) {
        NoteSecondaryError(L"HandleLoadStringW (unknown)");
    }
    return original(instance, id, buffer, bufferChars);
}

static int HandleLoadStringA(LoadStringA_t original, HINSTANCE instance, UINT id,
                             LPSTR buffer, int bufferChars) {
    if (buffer && bufferChars > 0 && IsTargetResourceModule(instance)) {
        if (const wchar_t* translated = GetEmbeddedTranslation(id)) {
            const int converted = WideCharToMultiByte(CP_ACP, 0, translated, -1,
                                                       buffer, bufferChars, nullptr, nullptr);
            if (converted > 0) return converted - 1;
            buffer[bufferChars - 1] = '\0';
            return bufferChars - 1;
        }
    }
    return original(instance, id, buffer, bufferChars);
}

static int WINAPI LoadStringWHookUser32(HINSTANCE i, UINT id, LPWSTR b, int c) {
    return HandleLoadStringW(LoadStringWOriginalUser32, i, id, b, c);
}
static int WINAPI LoadStringWHookKernelBase(HINSTANCE i, UINT id, LPWSTR b, int c) {
    return HandleLoadStringW(LoadStringWOriginalKernelBase, i, id, b, c);
}
static int WINAPI LoadStringAHookUser32(HINSTANCE i, UINT id, LPSTR b, int c) {
    return HandleLoadStringA(LoadStringAOriginalUser32, i, id, b, c);
}
static int WINAPI LoadStringAHookKernelBase(HINSTANCE i, UINT id, LPSTR b, int c) {
    return HandleLoadStringA(LoadStringAOriginalKernelBase, i, id, b, c);
}

static HRESULT __fastcall XResourceProviderCreateHookDui76(
    HINSTANCE instance, const wchar_t* r1, const wchar_t* r2, const wchar_t* r3, void** out) {
    if (g_forceTranslations.load() && IsTargetResourceModule(instance)) {
        if (HMODULE localized = EnsureLocalizedResourceModuleLoaded()) instance = localized;
    }
    return XResourceProviderCreateOriginalDui76(instance, r1, r2, r3, out);
}

static HRSRC WINAPI FindResourceExWHook(HMODULE module, LPCWSTR type, LPCWSTR name, WORD language) {
    if (type == MAKEINTRESOURCEW(6) && g_forceTranslations.load() && module && IsTargetResourceModule(module)) {
        if (HMODULE localized = EnsureLocalizedResourceModuleLoaded()) {
            const WORD wanted = GetEmbeddedLanguageId(GetCurrentEmbeddedLanguage());
            if (HRSRC resource = FindResourceExWOriginal(localized, type, name, wanted)) return resource;
            if (HRSRC resource = FindResourceExWOriginal(localized, type, name, language)) return resource;
        }
    }
    return FindResourceExWOriginal(module, type, name, language);
}

static HGLOBAL WINAPI LoadResourceHook(HMODULE module, HRSRC resourceInfo) {
    if (g_forceTranslations.load() && module && resourceInfo && IsTargetResourceModule(module)) {
        if (HMODULE localized = g_hLocalizedResources.load()) {
            MEMORY_BASIC_INFORMATION mbiRes{}, mbiLoc{};
            if (VirtualQuery(resourceInfo, &mbiRes, sizeof(mbiRes)) &&
                VirtualQuery(reinterpret_cast<LPCVOID>(reinterpret_cast<uintptr_t>(localized) & ~uintptr_t(3)),
                             &mbiLoc, sizeof(mbiLoc)) && mbiRes.AllocationBase == mbiLoc.AllocationBase) {
                if (HGLOBAL resource = LoadResourceOriginal(localized, resourceInfo)) return resource;
            }
        }
    }
    return LoadResourceOriginal(module, resourceInfo);
}

static void InstallTranslationHook() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) user32 = LoadLibraryW(L"user32.dll");
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    void* loadWUser = user32 ? reinterpret_cast<void*>(GetProcAddress(user32, "LoadStringW")) : nullptr;
    void* loadWKB = kernelBase ? reinterpret_cast<void*>(GetProcAddress(kernelBase, "LoadStringW")) : nullptr;
    if (loadWUser) WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringW_t>(loadWUser),
                                                  LoadStringWHookUser32, &LoadStringWOriginalUser32);
    if (loadWKB && loadWKB != loadWUser)
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringW_t>(loadWKB),
                                       LoadStringWHookKernelBase, &LoadStringWOriginalKernelBase);
    void* loadAUser = user32 ? reinterpret_cast<void*>(GetProcAddress(user32, "LoadStringA")) : nullptr;
    void* loadAKB = kernelBase ? reinterpret_cast<void*>(GetProcAddress(kernelBase, "LoadStringA")) : nullptr;
    if (loadAUser) WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringA_t>(loadAUser),
                                                  LoadStringAHookUser32, &LoadStringAOriginalUser32);
    if (loadAKB && loadAKB != loadAUser)
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringA_t>(loadAKB),
                                       LoadStringAHookKernelBase, &LoadStringAOriginalKernelBase);
    if (kernelBase) {
        if (auto* find = reinterpret_cast<FindResourceExW_t>(reinterpret_cast<void*>(GetProcAddress(kernelBase, "FindResourceExW"))))
            WindhawkUtils::SetFunctionHook(find, FindResourceExWHook, &FindResourceExWOriginal);
        if (auto* load = reinterpret_cast<LoadResource_t>(reinterpret_cast<void*>(GetProcAddress(kernelBase, "LoadResource"))))
            WindhawkUtils::SetFunctionHook(load, LoadResourceHook, &LoadResourceOriginal);
    }
}

static void HookDui76(HMODULE module) {
    static std::atomic<bool> hooked{false};
    if (!module || g_shuttingDown.load() || hooked.exchange(true)) return;
    if (auto* create = reinterpret_cast<XResourceProviderCreate_t>(reinterpret_cast<void*>(GetProcAddress(
            module, "?Create@XResourceProvider@DirectUI@@SAJPEAUHINSTANCE__@@PEBG11PEAPEAV12@@Z")))) {
        WindhawkUtils::SetFunctionHook(create, XResourceProviderCreateHookDui76,
                                       &XResourceProviderCreateOriginalDui76);
        Wh_ApplyHookOperations();
    } else {
        Wh_Log(L"XResourceProvider::Create not found in DUI76.dll");
    }
}

static void RefreshGeneratedMuiFiles(const std::wstring& dlDir, const std::wstring& binDir) {
    Bytes original;
    if (!ReadWholeFile(dlDir + L"\\wpccpl.dll", original)) return;
    const auto mode = static_cast<LanguageSettingMode>(g_selectedLanguageMode.load());
    const bool overrideAll = g_forceTranslations.load() && mode != LanguageSettingMode::Auto;
    const MuiLanguage forced = GetCurrentEmbeddedLanguage();
    std::vector<OutFile> files;
    for (const MuiLocaleTarget& target : kMuiLocaleTargets) {
        const MuiLanguage use = overrideAll ? forced : target.lang;
        Bytes mui = BuildWpcMui(original, use, target.folder, target.lcid);
        if (!mui.empty()) files.push_back({std::wstring(target.folder) + L"\\wpccpl.dll.mui", std::move(mui)});
    }
    if (files.size() == ARRAYSIZE(kMuiLocaleTargets)) WriteOutputs(binDir, files);
}

namespace vreg {

struct Val { std::wstring name; DWORD type; std::wstring str; DWORD dw; };
struct Key { std::wstring rel; std::vector<Val> values; std::vector<std::wstring> subkeys; };   // rel: lower case
struct Table { std::vector<Key> keys; };

static std::mutex g_tableMutex;
static std::shared_ptr<const Table> g_table;             // null until the applet is published

static std::shared_ptr<const Table> GetTable() {
    std::lock_guard<std::mutex> l(g_tableMutex);
    return g_table;
}
static void SetTable(std::shared_ptr<const Table> t) {
    std::lock_guard<std::mutex> l(g_tableMutex);
    g_table = std::move(t);
}

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = towlower(c);
    return s;
}

static bool ContainsKeywordI(LPCWSTR s) {
    if (!s) return false;
    for (; *s; ++s) {
        if (_wcsnicmp(s, L"clsid", 5) == 0 || _wcsnicmp(s, L"appid", 5) == 0 ||
            _wcsnicmp(s, L"interface", 9) == 0 || _wcsnicmp(s, L"controlpanel", 12) == 0 ||
            _wcsnicmp(s, L"shell extensions", 16) == 0) return true;
    }
    return false;
}

static const wchar_t* PredefinedName(HKEY h) {
    if (h == HKEY_CLASSES_ROOT) return L"hkey_classes_root";
    if (h == HKEY_CURRENT_USER) return L"hkey_current_user";
    if (h == HKEY_LOCAL_MACHINE) return L"hkey_local_machine";
    if (h == HKEY_USERS) return L"hkey_users";
    if (h == HKEY_CURRENT_CONFIG) return L"hkey_current_config";
    return L"";
}

static std::wstring JoinPath(const std::wstring& base, LPCWSTR sub) {
    std::wstring p = base;
    if (sub && *sub) {
        if (!p.empty()) p += L'\\';
        p += Lower(sub);
    }
    return p;
}

static bool EndsWithKey(const std::wstring& path, const std::wstring& rel) {   // whole-component suffix match
    if (path.size() < rel.size()) return false;
    if (path.compare(path.size() - rel.size(), rel.size(), rel) != 0) return false;
    return path.size() == rel.size() || path[path.size() - rel.size() - 1] == L'\\';
}

// The applet is 64-bit only: the 32-bit registry view (WOW6432Node) must not show it, otherwise the shell
// treats it as a 32-bit item.
static const Key* FindKey(const Table& t, const std::wstring& lowerPath) {
    if (lowerPath.find(L"wow6432node") != std::wstring::npos) return nullptr;
    for (const Key& k : t.keys)
        if (EndsWithKey(lowerPath, k.rel)) return &k;
    return nullptr;
}

static const Val* FindValue(const Key& k, LPCWSTR name) {
    const wchar_t* n = name ? name : L"";
    for (const Val& v : k.values)
        if (_wcsicmp(v.name.c_str(), n) == 0) return &v;
    return nullptr;
}

// The NameSpace key whose enumeration gets the applet appended: the machine-wide one, not the per-user one.
static bool IsNamespaceParent(const std::wstring& lowerPath) {
    return EndsWithKey(lowerPath, L"controlpanel\\namespace") &&
           lowerPath.compare(0, 17, L"hkey_current_user") != 0;
}

// ---- original functions ----
using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
using RegOpenKeyW_t = decltype(&RegOpenKeyW);
using RegCreateKeyExW_t = decltype(&RegCreateKeyExW);
using RegCloseKey_t = decltype(&RegCloseKey);
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
using RegGetValueW_t = decltype(&RegGetValueW);
using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
using RegEnumKeyW_t = decltype(&RegEnumKeyW);
using RegQueryInfoKeyW_t = decltype(&RegQueryInfoKeyW);
static RegOpenKeyExW_t RegOpenKeyExW_Orig;
static RegOpenKeyW_t RegOpenKeyW_Orig;
static RegCreateKeyExW_t RegCreateKeyExW_Orig;
static RegCloseKey_t RegCloseKey_Orig;
static RegQueryValueExW_t RegQueryValueExW_Orig;
static RegGetValueW_t RegGetValueW_Orig;
static RegEnumKeyExW_t RegEnumKeyExW_Orig;
static RegEnumKeyW_t RegEnumKeyW_Orig;
static RegQueryInfoKeyW_t RegQueryInfoKeyW_Orig;

// ---- the private hive that backs every virtual handle ----
static void* RegFunc(const char* name);   // defined below

static std::mutex g_scratchMutex;
static HKEY g_scratchRoot = nullptr;       // root of the private hive
static bool g_scratchFailed = false;       // loading the hive failed once: don't retry on every open
static std::wstring g_hiveFile;            // <storage>\vreg-<pid>.hiv

static void DeleteHiveFiles(const std::wstring& file) {
    DeleteFileW(file.c_str());
    DeleteFileW((file + L".LOG1").c_str());
    DeleteFileW((file + L".LOG2").c_str());
}

// Leftovers of earlier runs that crashed. Files of a hive that is still loaded can't be deleted, so another
// running process is not affected.
static void CleanupStaleHives() {
    const std::wstring dir = StorageDir();
    if (dir.empty()) return;
    WIN32_FIND_DATAW fd{};
    HANDLE h = FindFirstFileW((dir + L"\\vreg-*.hiv*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) DeleteFileW((dir + L"\\" + fd.cFileName).c_str());
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

// A real, empty registry root that is not part of the user's registry. There is no fallback: if it can't be
// loaded, the virtual keys are unavailable.
static bool EnsureScratchRoot() {
    std::lock_guard<std::mutex> l(g_scratchMutex);
    if (g_scratchRoot) return true;
    if (g_scratchFailed) return false;

    using RegLoadAppKeyW_t = LSTATUS(WINAPI*)(LPCWSTR, PHKEY, REGSAM, DWORD, DWORD);
    const std::wstring dir = StorageDir();
    auto loadAppKey = reinterpret_cast<RegLoadAppKeyW_t>(RegFunc("RegLoadAppKeyW"));
    if (!loadAppKey || dir.empty()) {
        g_scratchFailed = true;
        Wh_Log(L"Private registry hive is not available; the applet will not be shown");
        return false;
    }
    wchar_t name[64];
    swprintf_s(name, ARRAYSIZE(name), L"\\vreg-%lu.hiv", GetCurrentProcessId());
    g_hiveFile = dir + name;
    DeleteHiveFiles(g_hiveFile);
    HKEY k = nullptr;
    const LSTATUS st = loadAppKey(g_hiveFile.c_str(), &k, KEY_READ | KEY_WRITE, REG_PROCESS_APPKEY, 0);
    if (st != ERROR_SUCCESS || !k) {
        g_scratchFailed = true;
        Wh_Log(L"Private registry hive could not be loaded (error %ld); the applet will not be shown", st);
        return false;
    }
    g_scratchRoot = k;
    return true;
}

static void ReleaseScratchRoot() {
    HKEY root;
    {
        std::lock_guard<std::mutex> l(g_scratchMutex);
        root = g_scratchRoot;
        g_scratchRoot = nullptr;
        g_scratchFailed = false;
    }
    if (root && RegCloseKey_Orig) RegCloseKey_Orig(root);
    if (!g_hiveFile.empty()) DeleteHiveFiles(g_hiveFile);   // fails while a caller still holds a virtual handle; removed at the next start
}

// ---- handle bookkeeping ----
static std::mutex g_handleMutex;
static std::unordered_map<HKEY, std::wstring> g_paths;   // tracked real handles and virtual handles -> lower-case path
static std::unordered_set<HKEY> g_fake;                  // virtual handles (real handles to the scratch key)

static void LogVirtual(const wchar_t* what, const std::wstring& path) {
    static std::atomic<int> count{0};
    if (count.fetch_add(1) < 80) Wh_Log(L"[vreg] %s: %s", what, path.c_str());
}

static bool Lookup(HKEY h, std::wstring& path, bool& fake) {
    std::lock_guard<std::mutex> l(g_handleMutex);
    auto it = g_paths.find(h);
    if (it == g_paths.end()) return false;
    path = it->second;
    fake = g_fake.count(h) > 0;
    return true;
}

static bool IsFake(HKEY h) {
    std::lock_guard<std::mutex> l(g_handleMutex);
    return g_fake.count(h) > 0;
}

static bool IsTrackedOrFake(HKEY h) {
    std::lock_guard<std::mutex> l(g_handleMutex);
    return g_paths.count(h) > 0;
}

static void Track(HKEY h, const std::wstring& path) {
    std::lock_guard<std::mutex> l(g_handleMutex);
    g_paths[h] = path;
}

static void Untrack(HKEY h) {
    std::lock_guard<std::mutex> l(g_handleMutex);
    g_paths.erase(h);
}

static HKEY NewFake(const std::wstring& path) {
    if (!EnsureScratchRoot()) return nullptr;
    HKEY root;
    {
        std::lock_guard<std::mutex> l(g_scratchMutex);
        root = g_scratchRoot;
    }
    HKEY h = nullptr;
    if (!root || RegOpenKeyExW_Orig(root, L"", 0, KEY_READ, &h) != ERROR_SUCCESS) return nullptr;   // a new handle to the hive root
    std::lock_guard<std::mutex> l(g_handleMutex);
    g_paths[h] = path;
    g_fake.insert(h);
    return h;
}

static void FreeFake(HKEY h) {
    {
        std::lock_guard<std::mutex> l(g_handleMutex);
        g_fake.erase(h);
        g_paths.erase(h);
    }
    RegCloseKey_Orig(h);
}

// Forget everything without closing anything: the handles belong to their callers. They stay valid handles to the
// scratch key, which is deleted right after, so a later use just fails instead of touching anything else.
static void ForgetAllHandles() {
    std::lock_guard<std::mutex> l(g_handleMutex);
    g_fake.clear();
    g_paths.clear();
}

static bool IsWriteAccess(REGSAM sam) {
    return (sam & (KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_CREATE_LINK)) != 0;
}

// ---- value delivery (RegQueryValueEx / RegGetValue semantics) ----
static void ValueBytes(const Val& v, std::vector<BYTE>& out) {
    if (v.type == REG_DWORD) {
        out.resize(sizeof(DWORD));
        memcpy(out.data(), &v.dw, sizeof(DWORD));
    } else {
        const size_t n = (v.str.size() + 1) * sizeof(wchar_t);
        out.resize(n);
        memcpy(out.data(), v.str.c_str(), n);
    }
}

static LSTATUS Deliver(DWORD type, const std::vector<BYTE>& bytes, LPDWORD pType, void* data, LPDWORD cb) {
    if (pType) *pType = type;
    if (!cb) return data ? ERROR_INVALID_PARAMETER : ERROR_SUCCESS;
    const DWORD need = static_cast<DWORD>(bytes.size());
    if (!data) { *cb = need; return ERROR_SUCCESS; }
    if (*cb < need) { *cb = need; return ERROR_MORE_DATA; }
    memcpy(data, bytes.data(), need);
    *cb = need;
    return ERROR_SUCCESS;
}

static LSTATUS DeliverGetValue(const Val& v, DWORD flags, LPDWORD pType, void* data, LPDWORD cb) {
    if ((flags & 0xffff) == 0) flags |= RRF_RT_ANY;
    Val cur = v;
    if (cur.type == REG_EXPAND_SZ && (flags & RRF_RT_REG_SZ) && !(flags & RRF_NOEXPAND)) {
        wchar_t buf[2048];
        const DWORD n = ExpandEnvironmentStringsW(v.str.c_str(), buf, ARRAYSIZE(buf));
        if (n > 0 && n <= ARRAYSIZE(buf)) cur.str = buf;
        cur.type = REG_SZ;
    }
    const DWORD mask = cur.type == REG_SZ ? RRF_RT_REG_SZ
                       : cur.type == REG_EXPAND_SZ ? RRF_RT_REG_EXPAND_SZ : RRF_RT_REG_DWORD;
    if (!(flags & mask)) return ERROR_UNSUPPORTED_TYPE;
    std::vector<BYTE> bytes;
    ValueBytes(cur, bytes);
    return Deliver(cur.type, bytes, pType, data, cb);
}

static LSTATUS DeliverName(const std::wstring& name, LPWSTR lpName, LPDWORD cch) {
    if (!cch || !lpName) return ERROR_INVALID_PARAMETER;
    if (*cch < name.size() + 1) { *cch = static_cast<DWORD>(name.size() + 1); return ERROR_MORE_DATA; }
    wcscpy_s(lpName, *cch, name.c_str());
    *cch = static_cast<DWORD>(name.size());
    return ERROR_SUCCESS;
}

// ---- the virtual keys ----
// nameOnly is used by control.exe: enough to resolve Microsoft.ParentalControls,
// without exposing any server that could make control.exe load the legacy DLLs.
static std::shared_ptr<const Table> BuildTable(const std::wstring& binDir, bool nameOnly = false) {
    const std::wstring cpl = binDir + L"\\wpccpl.dll";
    const std::wstring wpc = binDir + L"\\wpc.dll";
    auto S = [](const wchar_t* n, DWORD type, const std::wstring& s) { return Val{n, type, s, 0}; };
    auto D = [](const wchar_t* n, DWORD d) { return Val{n, REG_DWORD, L"", d}; };
    auto t = std::make_shared<Table>();
    auto add = [&](const std::wstring& rel, std::vector<Val> vals, std::vector<std::wstring> subs) {
        t->keys.push_back(Key{rel, std::move(vals), std::move(subs)});
    };

    const std::wstring applet = L"clsid\\" + Lower(kAppletClsid);
    const bool force = g_forceTranslations.load();
    // Windows 7 skin: name (when not taken from the generated strings) and icon module.
    const bool win7 = CurrentSkin() == Skin::Win7;
    const wchar_t* fallbackName = win7 ? L"Parental Controls" : L"Family Safety";
    const std::wstring skinIcons = binDir + L"\\" + kWin7SkinDllName;
    const std::wstring iconModule =
        (win7 && GetFileAttributesW(skinIcons.c_str()) != INVALID_FILE_ATTRIBUTES) ? skinIcons : cpl;
    const std::wstring tasksUrl = binDir + L"\\FamilySafety.tasks.xml";
    add(applet,
        {S(L"", REG_SZ, force ? GetLocalizedDisplayName() : fallbackName),
         S(L"System.Software.TasksFileUrl", REG_SZ, tasksUrl),
         S(L"InfoTip", force ? REG_SZ : REG_EXPAND_SZ,
           force ? std::wstring(GetLocalizedInfoTip()) : L"@" + cpl + L",-101"),
         S(L"LocalizedString", force ? REG_SZ : REG_EXPAND_SZ,
           force ? std::wstring(GetLocalizedDisplayName()) : L"@" + cpl + L",-100"),
         S(L"System.ApplicationName", REG_SZ, L"Microsoft.ParentalControls"),
         D(L"System.ControlPanel.Category", 9)},
        nameOnly ? std::vector<std::wstring>{}
                 : std::vector<std::wstring>{L"DefaultIcon", L"InProcServer32", L"Instance", L"ShellFolder"});
    add(L"controlpanel\\namespace\\" + Lower(kAppletClsid),
        {S(L"", REG_SZ, force ? GetLocalizedDisplayName() : fallbackName)}, {});
    if (nameOnly) return t;

    add(applet + L"\\defaulticon", {S(L"", REG_EXPAND_SZ, iconModule + L",-1")}, {});
    add(applet + L"\\inprocserver32",
        {S(L"", REG_EXPAND_SZ, L"%SystemRoot%\\System32\\shdocvw.dll"),
         S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});
    add(applet + L"\\instance", {S(L"CLSID", REG_SZ, kLayoutFolderClsid)}, {L"InitPropertyBag"});
    add(applet + L"\\instance\\initpropertybag",
        {D(L"ResourceID", 100), S(L"ResourceDLL", REG_EXPAND_SZ, cpl)}, {});
    add(applet + L"\\shellfolder",
        {D(L"Attributes", 0xa0000000), S(L"WantsParseDisplayName", REG_SZ, L"")}, {});

    for (const wchar_t* clsid : {kWpcCplClass1, kWpcCplClass2}) {
        const std::wstring root = L"clsid\\" + Lower(clsid);
        std::vector<Val> values;
        if (clsid == kWpcCplClass2) values.push_back(S(L"", REG_SZ, L"Parental Controls Control Panel"));
        add(root, std::move(values), {L"InProcServer32"});
        add(root + L"\\inprocserver32",
            {S(L"", REG_SZ, cpl), S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});
    }

    const std::wstring factory = L"clsid\\" + Lower(kVirtualFactoryClsid);
    add(factory,
        {S(L"", REG_SZ, L"Windows Parental Controls"),
         S(L"AppId", REG_SZ, kVirtualFactoryClsid),
         S(L"LocalizedString", REG_EXPAND_SZ, L"@" + cpl + L",-400")},
        {L"Elevation", L"InProcServer32", L"VirtualServerObjects"});
    add(factory + L"\\elevation",
        {D(L"Enabled", 1), S(L"IconReference", REG_EXPAND_SZ, L"@" + iconModule + L",-2")}, {});
    add(factory + L"\\inprocserver32",
        {S(L"", REG_EXPAND_SZ, L"%SystemRoot%\\System32\\shpafact.dll"),
         S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});
    std::vector<Val> objects;
    for (const wchar_t* clsid : kVirtualServerObjects) objects.push_back(S(clsid, REG_SZ, L""));
    add(factory + L"\\virtualserverobjects", std::move(objects), {});
    add(L"appid\\" + Lower(kVirtualFactoryClsid),
        {S(L"", REG_SZ, L"Windows Parental Controls"), S(L"DllSurrogate", REG_SZ, L"")}, {});

    for (const WpcClassSpec& item : kWpcClasses) {
        const std::wstring root = L"clsid\\" + Lower(item.clsid);
        std::vector<Val> values;
        if (item.name) values.push_back(S(L"", REG_SZ, item.name));
        if (item.hasOwnAppId) values.push_back(S(L"AppID", REG_SZ, item.clsid));
        add(root, std::move(values), {L"InProcServer32"});
        add(root + L"\\inprocserver32",
            {S(L"", REG_SZ, wpc), S(L"ThreadingModel", REG_SZ, item.threading)}, {});
    }
    for (const InterfaceSpec& item : kWpcInterfaces) {
        const std::wstring root = L"interface\\" + Lower(item.iid);
        add(root, {S(L"", REG_SZ, item.name)}, {L"ProxyStubClsid32"});
        add(root + L"\\proxystubclsid32", {S(L"", REG_SZ, kPsFactoryClsid)}, {});
    }
    return t;
}

// ---- hooks ----
static bool ChildExists(HKEY k) {   // does the NameSpace key already contain the applet?
    HKEY h = nullptr;
    if (RegOpenKeyExW_Orig(k, kAppletClsid, 0, KEY_READ, &h) != ERROR_SUCCESS) return false;
    RegCloseKey_Orig(h);
    return true;
}

static DWORD RealSubKeyCount(HKEY hKey) {
    DWORD n = 0;
    if (RegQueryInfoKeyW_Orig(hKey, nullptr, nullptr, nullptr, &n, nullptr, nullptr, nullptr, nullptr, nullptr,
                              nullptr, nullptr) != ERROR_SUCCESS)
        return 0xFFFFFFFF;
    return n;
}

static LSTATUS OpenKeyImpl(HKEY hKey, LPCWSTR sub, DWORD opt, REGSAM sam, PHKEY phk, bool oldApi) {
    auto realOpen = [&]() -> LSTATUS {
        return oldApi ? RegOpenKeyW_Orig(hKey, sub, phk) : RegOpenKeyExW_Orig(hKey, sub, opt, sam, phk);
    };
    std::wstring parentPath;
    bool fake = false;
    const bool tracked = Lookup(hKey, parentPath, fake);
    if (!tracked && !ContainsKeywordI(sub)) return realOpen();

    const auto table = (sam & KEY_WOW64_32KEY) ? nullptr : GetTable();
    if (!tracked) parentPath = PredefinedName(hKey);
    const std::wstring full = JoinPath(parentPath, sub);
    const bool virt = table && FindKey(*table, full);

    if (fake) {   // the parent is virtual: answer from the table only
        if (!virt) return ERROR_FILE_NOT_FOUND;
        if (IsWriteAccess(sam)) return ERROR_ACCESS_DENIED;
        HKEY f = NewFake(full);
        if (!f) return ERROR_OUTOFMEMORY;
        LogVirtual(L"open (child of virtual key)", full);
        if (phk) *phk = f;
        return ERROR_SUCCESS;
    }
    const LSTATUS st = realOpen();
    if (st == ERROR_SUCCESS && phk && *phk) {
        if (ContainsKeywordI(full.c_str())) Track(*phk, full);
    } else if (st == ERROR_FILE_NOT_FOUND && phk && virt) {
        if (IsWriteAccess(sam)) return ERROR_ACCESS_DENIED;
        HKEY f = NewFake(full);
        if (!f) return ERROR_OUTOFMEMORY;
        LogVirtual(L"open (virtual)", full);
        *phk = f;
        return ERROR_SUCCESS;
    }
    return st;
}

static LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey, LPCWSTR sub, DWORD opt, REGSAM sam, PHKEY phk) {
    try {
        if (StabilityBudgetExceeded()) return RegOpenKeyExW_Orig(hKey, sub, opt, sam, phk);
        return OpenKeyImpl(hKey, sub, opt, sam, phk, false);
    } catch (...) {
        NoteSecondaryError(L"RegOpenKeyExW");
        return RegOpenKeyExW_Orig(hKey, sub, opt, sam, phk);
    }
}

static LSTATUS WINAPI RegOpenKeyW_Hook(HKEY hKey, LPCWSTR sub, PHKEY phk) {
    try {
        if (StabilityBudgetExceeded()) return RegOpenKeyW_Orig(hKey, sub, phk);
        return OpenKeyImpl(hKey, sub, 0, MAXIMUM_ALLOWED, phk, true);
    } catch (...) {
        NoteSecondaryError(L"RegOpenKeyW");
        return RegOpenKeyW_Orig(hKey, sub, phk);
    }
}

// Writes into the virtual tree are refused; everything else goes through.
static LSTATUS WINAPI RegCreateKeyExW_Hook(HKEY hKey, LPCWSTR sub, DWORD reserved, LPWSTR cls, DWORD opt, REGSAM sam,
                                           LPSECURITY_ATTRIBUTES sa, PHKEY phk, LPDWORD disp) {
    try {
        std::wstring parentPath;
        bool fake = false;
        const bool tracked = Lookup(hKey, parentPath, fake);
        if (tracked || ContainsKeywordI(sub)) {
            if (!tracked) parentPath = PredefinedName(hKey);
            const auto table = GetTable();
            const std::wstring full = JoinPath(parentPath, sub);
            if (table && FindKey(*table, full)) {
                if (phk) *phk = nullptr;
                return ERROR_ACCESS_DENIED;
            }
            if (fake) return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
    }
    return RegCreateKeyExW_Orig(hKey, sub, reserved, cls, opt, sam, sa, phk, disp);
}

static LSTATUS WINAPI RegCloseKey_Hook(HKEY hKey) {
    if (IsFake(hKey)) {
        FreeFake(hKey);
        return ERROR_SUCCESS;
    }
    if (IsTrackedOrFake(hKey)) Untrack(hKey);   // before closing, so a reused handle value can't be erased by mistake
    return RegCloseKey_Orig(hKey);
}

static LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data,
                                            LPDWORD cb) {
    try {
        std::wstring path;
        bool fake = false;
        if (Lookup(hKey, path, fake)) {
            if (const auto table = GetTable()) {
                if (const Key* k = FindKey(*table, path)) {
                    if (const Val* v = FindValue(*k, name)) {
                        std::vector<BYTE> bytes;
                        ValueBytes(*v, bytes);
                        return Deliver(v->type, bytes, type, data, cb);
                    }
                }
            }
            if (fake) return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
        Wh_Log(L"exception in RegQueryValueExW hook");
    }
    return RegQueryValueExW_Orig(hKey, name, reserved, type, data, cb);
}

static LSTATUS WINAPI RegGetValueW_Hook(HKEY hkey, LPCWSTR subKey, LPCWSTR value, DWORD flags, LPDWORD pType,
                                        PVOID data, LPDWORD cb) {
    try {
        std::wstring path;
        bool fake = false;
        const bool tracked = Lookup(hkey, path, fake);
        if (tracked || ContainsKeywordI(subKey)) {
            if (!tracked) path = PredefinedName(hkey);
            path = JoinPath(path, subKey);
            if (const auto table = (flags & RRF_SUBKEY_WOW6432KEY) ? nullptr : GetTable()) {
                if (const Key* k = FindKey(*table, path)) {
                    if (const Val* v = FindValue(*k, value)) return DeliverGetValue(*v, flags, pType, data, cb);
                }
            }
            if (fake) return ERROR_FILE_NOT_FOUND;
        }
    } catch (...) {
        Wh_Log(L"exception in RegGetValueW hook");
    }
    return RegGetValueW_Orig(hkey, subKey, value, flags, pType, data, cb);
}

static LSTATUS WINAPI RegEnumKeyExW_Hook(HKEY hKey, DWORD idx, LPWSTR name, LPDWORD cchName, LPDWORD reserved,
                                         LPWSTR cls, LPDWORD cchCls, PFILETIME ft) {
    try {
        std::wstring path;
        bool fake = false;
        if (Lookup(hKey, path, fake)) {
            const auto table = GetTable();
            if (fake) {
                const Key* k = table ? FindKey(*table, path) : nullptr;
                if (!k || idx >= k->subkeys.size()) return ERROR_NO_MORE_ITEMS;
                const LSTATUS st = DeliverName(k->subkeys[idx], name, cchName);
                if (st == ERROR_SUCCESS) {
                    if (cchCls) { if (cls && *cchCls) cls[0] = 0; *cchCls = 0; }
                    if (ft) GetSystemTimeAsFileTime(ft);
                }
                return st;
            }
            if (table && IsNamespaceParent(path)) {
                const LSTATUS st = RegEnumKeyExW_Orig(hKey, idx, name, cchName, reserved, cls, cchCls, ft);
                if (st != ERROR_NO_MORE_ITEMS) return st;
                if (ChildExists(hKey) || idx != RealSubKeyCount(hKey)) return ERROR_NO_MORE_ITEMS;
                const LSTATUS inj = DeliverName(kAppletClsid, name, cchName);   // the applet goes after the real entries
                if (inj == ERROR_SUCCESS) {
                    if (cchCls) { if (cls && *cchCls) cls[0] = 0; *cchCls = 0; }
                    if (ft) GetSystemTimeAsFileTime(ft);
                }
                return inj;
            }
        }
    } catch (...) {
        Wh_Log(L"exception in RegEnumKeyExW hook");
    }
    return RegEnumKeyExW_Orig(hKey, idx, name, cchName, reserved, cls, cchCls, ft);
}

static LSTATUS WINAPI RegEnumKeyW_Hook(HKEY hKey, DWORD idx, LPWSTR name, DWORD cchName) {
    try {
        std::wstring path;
        bool fake = false;
        if (Lookup(hKey, path, fake)) {
            const auto table = GetTable();
            if (fake) {
                const Key* k = table ? FindKey(*table, path) : nullptr;
                if (!k || idx >= k->subkeys.size()) return ERROR_NO_MORE_ITEMS;
                DWORD cch = cchName;
                return DeliverName(k->subkeys[idx], name, &cch);
            }
            if (table && IsNamespaceParent(path)) {
                const LSTATUS st = RegEnumKeyW_Orig(hKey, idx, name, cchName);
                if (st != ERROR_NO_MORE_ITEMS) return st;
                if (ChildExists(hKey) || idx != RealSubKeyCount(hKey)) return ERROR_NO_MORE_ITEMS;
                DWORD cch = cchName;
                return DeliverName(kAppletClsid, name, &cch);
            }
        }
    } catch (...) {
        Wh_Log(L"exception in RegEnumKeyW hook");
    }
    return RegEnumKeyW_Orig(hKey, idx, name, cchName);
}

static LSTATUS WINAPI RegQueryInfoKeyW_Hook(HKEY hKey, LPWSTR cls, LPDWORD cchCls, LPDWORD reserved,
                                            LPDWORD cSubKeys, LPDWORD maxSubKeyLen, LPDWORD maxClassLen,
                                            LPDWORD cValues, LPDWORD maxValueNameLen, LPDWORD maxValueLen,
                                            LPDWORD secDescLen, PFILETIME ft) {
    try {
        std::wstring path;
        bool fake = false;
        if (Lookup(hKey, path, fake)) {
            const auto table = GetTable();
            if (fake) {
                const Key* k = table ? FindKey(*table, path) : nullptr;
                if (cSubKeys) *cSubKeys = k ? static_cast<DWORD>(k->subkeys.size()) : 0;
                if (cValues) *cValues = k ? static_cast<DWORD>(k->values.size()) : 0;
                if (maxSubKeyLen) *maxSubKeyLen = 32;
                if (maxClassLen) *maxClassLen = 0;
                if (maxValueNameLen) *maxValueNameLen = 64;
                if (maxValueLen) *maxValueLen = 512;
                if (secDescLen) *secDescLen = 0;
                if (cls && cchCls) { if (*cchCls > 0) cls[0] = 0; *cchCls = 0; }
                if (ft) GetSystemTimeAsFileTime(ft);
                return ERROR_SUCCESS;
            }
            if (table && IsNamespaceParent(path)) {
                const LSTATUS st = RegQueryInfoKeyW_Orig(hKey, cls, cchCls, reserved, cSubKeys, maxSubKeyLen,
                                                         maxClassLen, cValues, maxValueNameLen, maxValueLen,
                                                         secDescLen, ft);
                if (st == ERROR_SUCCESS && cSubKeys && !ChildExists(hKey)) {
                    ++*cSubKeys;
                    const DWORD need = static_cast<DWORD>(wcslen(kAppletClsid) + 1);
                    if (maxSubKeyLen && *maxSubKeyLen < need) *maxSubKeyLen = need;
                }
                return st;
            }
        }
    } catch (...) {
        Wh_Log(L"exception in RegQueryInfoKeyW hook");
    }
    return RegQueryInfoKeyW_Orig(hKey, cls, cchCls, reserved, cSubKeys, maxSubKeyLen, maxClassLen, cValues,
                                 maxValueNameLen, maxValueLen, secDescLen, ft);
}

// ---- COM activation of the private classes ----
// The Shell's applet class still comes from shdocvw. WPCCPL and Wpc classes are
// answered directly from their verified private modules. PSFactoryBuffer is not
// intercepted: COM must request IPSFactoryBuffer through the normal proxy path.
struct ComClassMap { GUID clsid; int module; };  // 0=shdocvw, 1=WPCCPL, 2=Wpc
static const ComClassMap kComClassMap[] = {
    {{0x96AE8D84, 0xA250, 0x4520, {0x95, 0xA5, 0xA4, 0x7A, 0x7E, 0x3C, 0x54, 0x8B}}, 0},
    {{0xA167B942, 0xCD17, 0x4D00, {0xBD, 0xD9, 0x8F, 0xDC, 0x2D, 0xC1, 0x58, 0xF2}}, 1},
    {{0xA6F1684D, 0xAED6, 0x401B, {0x97, 0x86, 0xA3, 0xE3, 0xB5, 0x3C, 0x66, 0x41}}, 1},
    {{0xE77E1B8B, 0xECF3, 0x4AF0, {0x8B, 0x5E, 0xEB, 0x13, 0x73, 0x9D, 0x53, 0x44}}, 2},
    {{0x6C482A9C, 0x033D, 0x45FE, {0xA0, 0x1C, 0x97, 0x22, 0xEF, 0x35, 0xE2, 0x55}}, 2},
    {{0x5323BCB5, 0x0431, 0x42BE, {0xA4, 0x82, 0x5E, 0xE7, 0x6F, 0x24, 0x3A, 0x28}}, 2},
    {{0xB016C9D5, 0x4801, 0x424E, {0x89, 0xEB, 0xDA, 0x6E, 0xB1, 0xDF, 0x53, 0xB7}}, 2},
    {{0xBB18C7A0, 0x2186, 0x4BE0, {0x97, 0xD8, 0x04, 0x84, 0x7B, 0x62, 0x8E, 0x02}}, 2},
    {{0xD5307D61, 0xF9BC, 0x4DE1, {0x94, 0xEA, 0x1D, 0xEF, 0x24, 0xDF, 0x4B, 0xB2}}, 2},
    {{0xE77CC89B, 0x7401, 0x4C04, {0x8C, 0xED, 0x14, 0x9D, 0xB3, 0x5A, 0xDD, 0x04}}, 2},
    {{0x355DFFAA, 0x3B9F, 0x435C, {0xB4, 0x28, 0x5D, 0x44, 0x29, 0x0B, 0xC5, 0xF2}}, 2},
};
static const GUID kIidClassFactory = {0x00000001, 0x0000, 0x0000, {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};

static HRESULT GetOurFactory(REFCLSID clsid, REFIID riid, void** ppv) {
    int wanted = -1;
    for (const ComClassMap& item : kComClassMap) {
        if (IsEqualGUID(clsid, item.clsid)) { wanted = item.module; break; }
    }
    if (wanted < 0) return REGDB_E_CLASSNOTREG;
    HMODULE mod = nullptr;
    if (wanted == 0) {
        mod = GetModuleHandleW(L"shdocvw.dll");
        if (!mod) mod = LoadLibraryExW(L"shdocvw.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    } else if (wanted == 1) {
        mod = g_hWpcCpl.load();
    } else {
        mod = g_hWpc.load();
    }
    if (!mod) return REGDB_E_CLASSNOTREG;
    using DllGetClassObject_t = HRESULT(STDAPICALLTYPE*)(REFCLSID, REFIID, LPVOID*);
    auto fn = reinterpret_cast<DllGetClassObject_t>(reinterpret_cast<void*>(GetProcAddress(mod, "DllGetClassObject")));
    return fn ? fn(clsid, riid, ppv) : REGDB_E_CLASSNOTREG;
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstanceCombase_Orig;
static CoCreateInstance_t CoCreateInstanceOle32_Orig;

static HRESULT HandleCoCreateInstance(REFCLSID rclsid, LPUNKNOWN outer, DWORD ctx, REFIID riid, LPVOID* ppv,
                                      CoCreateInstance_t original) {
    try {
        if (!StabilityBudgetExceeded() && ppv && (ctx & CLSCTX_INPROC_SERVER) && GetTable()) {
            IClassFactory* factory = nullptr;
            HRESULT hr = GetOurFactory(rclsid, kIidClassFactory, reinterpret_cast<void**>(&factory));
            if (SUCCEEDED(hr) && factory) {
                *ppv = nullptr;
                hr = factory->CreateInstance(outer, riid, ppv);
                factory->Release();
                Wh_Log(L"[com] private class activation -> 0x%08lX", static_cast<unsigned long>(hr));
                return hr;
            }
        }
    } catch (const std::exception&) {
        NoteSecondaryError(L"HandleCoCreateInstance");
    } catch (...) {
        NoteSecondaryError(L"HandleCoCreateInstance (unknown)");
    }
    return original(rclsid, outer, ctx, riid, ppv);
}

static HRESULT WINAPI CoCreateInstanceCombase_Hook(REFCLSID c, LPUNKNOWN o, DWORD ctx, REFIID r, LPVOID* p) {
    return HandleCoCreateInstance(c, o, ctx, r, p, CoCreateInstanceCombase_Orig);
}
static HRESULT WINAPI CoCreateInstanceOle32_Hook(REFCLSID c, LPUNKNOWN o, DWORD ctx, REFIID r, LPVOID* p) {
    return HandleCoCreateInstance(c, o, ctx, r, p, CoCreateInstanceOle32_Orig);
}

static void* RegFunc(const char* name) {
    if (HMODULE kb = GetModuleHandleW(L"kernelbase.dll")) {
        if (void* p = reinterpret_cast<void*>(GetProcAddress(kb, name))) return p;
    }
    HMODULE adv = GetModuleHandleW(L"advapi32.dll");
    if (!adv) adv = LoadLibraryW(L"advapi32.dll");
    return adv ? reinterpret_cast<void*>(GetProcAddress(adv, name)) : nullptr;
}

static bool InstallHooks(bool withCom = true) {
    struct H { const char* name; void* hook; void** orig; bool required; };
    const H hooks[] = {
        {"RegOpenKeyExW", (void*)RegOpenKeyExW_Hook, (void**)&RegOpenKeyExW_Orig, true},
        {"RegOpenKeyW", (void*)RegOpenKeyW_Hook, (void**)&RegOpenKeyW_Orig, false},
        {"RegCreateKeyExW", (void*)RegCreateKeyExW_Hook, (void**)&RegCreateKeyExW_Orig, true},
        {"RegCloseKey", (void*)RegCloseKey_Hook, (void**)&RegCloseKey_Orig, true},
        {"RegQueryValueExW", (void*)RegQueryValueExW_Hook, (void**)&RegQueryValueExW_Orig, true},
        {"RegGetValueW", (void*)RegGetValueW_Hook, (void**)&RegGetValueW_Orig, true},
        {"RegEnumKeyExW", (void*)RegEnumKeyExW_Hook, (void**)&RegEnumKeyExW_Orig, true},
        {"RegEnumKeyW", (void*)RegEnumKeyW_Hook, (void**)&RegEnumKeyW_Orig, true},
        {"RegQueryInfoKeyW", (void*)RegQueryInfoKeyW_Hook, (void**)&RegQueryInfoKeyW_Orig, true},
    };
    for (const H& h : hooks) {
        void* target = RegFunc(h.name);
        if (!target || !Wh_SetFunctionHook(target, h.hook, h.orig)) {
            if (!h.required) continue;
            Wh_Log(L"Could not hook %S", h.name);
            return false;
        }
    }
    if (!withCom) return true;
    HMODULE combase = GetModuleHandleW(L"combase.dll");
    if (!combase) combase = LoadLibraryExW(L"combase.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
    if (!ole32) ole32 = LoadLibraryExW(L"ole32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    void* pCombase = combase ? reinterpret_cast<void*>(GetProcAddress(combase, "CoCreateInstance")) : nullptr;
    void* pOle32 = ole32 ? reinterpret_cast<void*>(GetProcAddress(ole32, "CoCreateInstance")) : nullptr;
    if (pCombase) Wh_SetFunctionHook(pCombase, (void*)CoCreateInstanceCombase_Hook, (void**)&CoCreateInstanceCombase_Orig);
    if (pOle32 && pOle32 != pCombase)
        Wh_SetFunctionHook(pOle32, (void*)CoCreateInstanceOle32_Hook, (void**)&CoCreateInstanceOle32_Orig);
    return true;
}

}  // namespace vreg

static void PublishVirtualRegistration(const std::wstring& binDir, bool nameOnly = false) {
    vreg::SetTable(vreg::BuildTable(binDir, nameOnly));
    Wh_Log(L"Family Safety registration published virtually (native x64; no registry merge)");
}

static void UnpublishVirtualRegistration() {
    vreg::SetTable(nullptr);
    vreg::ForgetAllHandles();
    vreg::ReleaseScratchRoot();
}

static const OutFile* FindOutFile(const std::vector<OutFile>& outs, const std::wstring& rel) {
    for (const OutFile& file : outs) if (_wcsicmp(file.rel.c_str(), rel.c_str()) == 0) return &file;
    return nullptr;
}

static HMODULE LoadPrivateModule(const std::wstring& binDir, const OutFile& file) {
    return LoadVerified(binDir + L"\\" + file.rel, file.data,
                        LOAD_WITH_ALTERED_SEARCH_PATH, LoadLibraryExW);
}

static bool LoadExplorerModules(const std::wstring& binDir, const std::vector<OutFile>& outs) {
    try {
        static const wchar_t* order[] = {L"DUI76.dll", L"wpc.dll", L"wpccpl.dll"};
        for (const wchar_t* name : order) {
            const OutFile* expected = FindOutFile(outs, name);
            if (!expected) return false;
            HMODULE module = LoadPrivateModule(binDir, *expected);
            if (!module) {
                Wh_Log(L"Could not load the verified private module %s", name);
                return false;
            }
            if (_wcsicmp(name, L"DUI76.dll") == 0) HookDui76(module);
            if (_wcsicmp(name, L"wpc.dll") == 0) g_hWpc.store(module);
            if (_wcsicmp(name, L"wpccpl.dll") == 0) g_hWpcCpl.store(module);
        }
        if (g_forceTranslations.load()) {
            std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
            if (!LocalizedResourceModuleUpToDateLocked() &&
                !BuildLocalizedResourceModule(binDir + L"\\wpccpl.dll", binDir)) {
                Wh_Log(L"Could not prebuild the private localized WPCCPL resource module");
            }
        }
        return true;
    } catch (const std::exception& e) {
        Wh_Log(L"LoadExplorerModules exception: %S", e.what());
        return false;
    } catch (...) {
        Wh_Log(L"LoadExplorerModules unknown exception");
        return false;
    }
}

static void RunSetup() {
    const std::wstring dir = StorageDir();
    if (dir.empty()) { Wh_Log(L"No mod storage path; giving up"); return; }
    const std::wstring dlDir = dir + L"\\dl", binDir = dir + L"\\bin";
    HANDLE mtx = CreateMutexW(nullptr, FALSE, L"Windhawk.FamilySafetyAppletRestorer.Setup");
    if (mtx) {
        HANDLE handles[] = {mtx, g_stopEvent};
        const DWORD wait = WaitForMultipleObjects(g_stopEvent ? 2 : 1, handles, FALSE, 120000);
        if (wait == WAIT_OBJECT_0 + 1) { CloseHandle(mtx); return; }
    }
    auto done = [&]() { if (mtx) { ReleaseMutex(mtx); CloseHandle(mtx); } };
    EnsureDirs(dlDir);
    EnsureDirs(binDir);

    std::map<std::wstring, Bytes> inputs;
    for (const FileSpec& f : kFiles) {
        Bytes data;
        if (g_shuttingDown.load() || !GetVerifiedFile(dlDir, f, data)) {
            Wh_Log(L"Setup stopped: %s is unavailable; check the connection and restart Explorer", f.name);
            done();
            return;
        }
        inputs[f.name] = std::move(data);
    }

    std::vector<OutFile> outs;
    std::string err;
    if (!BuildOutputs(inputs, outs, err)) {
        Wh_Log(L"Building WPC files failed: %S", err.c_str());
        done();
        return;
    }
    const OutFile* englishMui = FindOutFile(outs, L"en-US\\wpccpl.dll.mui");
    // The pinned English MUI hash always refers to the untouched Windows 8 text. With the Windows 7
    // skin the shipped English MUI legitimately differs, so the baseline is rebuilt and checked instead.
    const Bytes baselineMui = BuildWpcMui(inputs[L"wpccpl.dll"], MuiLanguage::EN_US, L"en-US", 0x0409, Skin::Win8);
    if (outs.size() != 12 || Sha256Hex(outs[0].data) != kPatchedWpcCplSha256 ||
        !englishMui || baselineMui.empty() || Sha256Hex(baselineMui) != kGeneratedMuiSha256 ||
        (CurrentSkin() == Skin::Win8 && englishMui->data != baselineMui)) {
        Wh_Log(L"Generated WPCCPL or English MUI does not match its pinned SHA-256");
        done();
        return;
    }
    if (!WriteOutputs(binDir, outs)) { done(); return; }
    if (g_forceTranslations.load() &&
        static_cast<LanguageSettingMode>(g_selectedLanguageMode.load()) != LanguageSettingMode::Auto) {
        RefreshGeneratedMuiFiles(dlDir, binDir);
    }
    WriteTasksXml(binDir);   // generate classic task links XML
    done();
    if (g_shuttingDown.load()) return;
    if (!LoadExplorerModules(binDir, outs)) return;
    PublishVirtualRegistration(binDir);
}

// ---- Shell32 compatibility: hide-list string and WPC install state ----
static const wchar_t kHideGuidStr[] = L"::{96AE8D84-A250-4520-95A5-A47A7E3C548B}";
struct StringPatch { BYTE* addr; std::vector<BYTE> orig; };
static std::vector<StringPatch> g_patches;

static BYTE* FindWideInModule(HMODULE m, const wchar_t* needle, bool ignoreCase) {
    BYTE* base = reinterpret_cast<BYTE*>(m);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;
    BYTE* end = base + nt->OptionalHeader.SizeOfImage;
    const size_t nlen = wcslen(needle), nb = nlen * sizeof(wchar_t);
    for (BYTE* p = base; p < end;) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(p, &mbi, sizeof(mbi))) break;
        BYTE* rs = reinterpret_cast<BYTE*>(mbi.BaseAddress);
        BYTE* re = rs + mbi.RegionSize;
        if (re > end) re = end;
        const DWORD readable = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ |
                               PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
        if (mbi.State == MEM_COMMIT && !(mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) && (mbi.Protect & readable)) {
            for (BYTE* q = reinterpret_cast<BYTE*>((reinterpret_cast<uintptr_t>(rs) + 1) & ~uintptr_t(1));
                 q + nb <= re; q += 2) {
                const wchar_t* w = reinterpret_cast<const wchar_t*>(q);
                if (w[0] != L':') continue;
                if (ignoreCase ? _wcsnicmp(w, needle, nlen) == 0 : wcsncmp(w, needle, nlen) == 0) return q;
            }
        }
        p = re;
    }
    return nullptr;
}

static void BlankHideString(HMODULE m) {
    if (!m) { Wh_Log(L"[unhide] shell32.dll is not loaded"); return; }
    BYTE* a = FindWideInModule(m, kHideGuidStr, false);
    if (!a) a = FindWideInModule(m, kHideGuidStr, true);
    if (!a) { Wh_Log(L"[unhide] Family Safety string not found (already blanked or unsupported build)"); return; }
    const size_t nb = wcslen(kHideGuidStr) * sizeof(wchar_t);
    DWORD old = 0;
    if (!VirtualProtect(a, nb, PAGE_READWRITE, &old)) {
        Wh_Log(L"[unhide] VirtualProtect failed (%lu)", GetLastError());
        return;
    }
    StringPatch sp;
    sp.addr = a;
    sp.orig.assign(a, a + nb);
    memset(a, 0, nb);
    DWORD tmp = 0;
    VirtualProtect(a, nb, old, &tmp);
    g_patches.push_back(std::move(sp));
    Wh_Log(L"[unhide] Family Safety string blanked at RVA 0x%llx",
           (unsigned long long)(a - reinterpret_cast<BYTE*>(m)));
}

static void RestoreHideStrings() {
    for (StringPatch& sp : g_patches) {
        DWORD old = 0, tmp = 0;
        if (VirtualProtect(sp.addr, sp.orig.size(), PAGE_READWRITE, &old)) {
            memcpy(sp.addr, sp.orig.data(), sp.orig.size());
            VirtualProtect(sp.addr, sp.orig.size(), old, &tmp);
        }
    }
    g_patches.clear();
}

using WpcInstallState_t = HRESULT(WINAPI*)(DWORD* state);
static WpcInstallState_t WpcInstallState_Original = nullptr;

static HRESULT WINAPI WpcInstallState_Hook(DWORD* state) {
    if (state) *state = 0;
    return S_FALSE;  // non-negative, and preserves the previously tested bypass semantics
}

static bool InstallWpcInstallStateHook() {
    HMODULE shell = GetModuleHandleW(L"shell32.dll");
    if (!shell) shell = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!shell) return false;
    auto target = reinterpret_cast<WpcInstallState_t>(
        reinterpret_cast<void*>(GetProcAddress(shell, MAKEINTRESOURCEA(859))));
    static const BYTE expected[] = {0xC7,0x01,0x01,0x00,0x00,0x00,0x33,0xC0,0xC3};
    if (!target || memcmp(reinterpret_cast<const void*>(target), expected, sizeof(expected)) != 0) {
        Wh_Log(L"[wpc] Shell32 ordinal 859 does not match the VM-verified WPC_InstallState; not hooked");
        return false;
    }
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(target), reinterpret_cast<void*>(WpcInstallState_Hook),
                            reinterpret_cast<void**>(&WpcInstallState_Original))) {
        Wh_Log(L"[wpc] Could not hook WPC_InstallState");
        return false;
    }
    Wh_Log(L"[wpc] VM-verified WPC_InstallState bypass installed");
    return true;
}

// =============================================================================
// Windhawk entry points
// =============================================================================
// svchost.exe is a functional requirement, not an optional UI host. The
// RpcSs, DcomLaunch and Appinfo service hosts participate in COM activation
// and elevation, and need the same virtual WPC registration view as Explorer.
// Explorer-only registration can expose the Control Panel item and even create
// an initial COM object without completing activation of the Family Safety page.
//
// Regression note from the author's runtime testing: the applet did not open
// after svchost.exe support was removed. Earlier apparently successful tests
// still had the svchost-enabled version active, so they did not establish that
// the configuration without svchost.exe worked. Removing @include svchost.exe,
// the service-host selection below, or the ComService registration branch can
// therefore introduce silent, previously undocumented opening regressions.
// Preserve all three together; any change needs an end-to-end opening test in
// a fresh session, without service-side hooks left over from an earlier version.
enum class ProcessRole { Other, Explorer, Control, DllHost, ComService };
static ProcessRole g_role = ProcessRole::Other;

static std::wstring LowerText(std::wstring value) {
    for (wchar_t& c : value) c = towlower(c);
    return value;
}

static bool ReadPinnedFile(const std::wstring& path, const FileSpec& spec, Bytes& data) {
    return ReadWholeFile(path, data) && data.size() == spec.size && Sha256Hex(data) == spec.sha256;
}

static const FileSpec* FindFileSpec(const wchar_t* name) {
    for (const FileSpec& f : kFiles) if (_wcsicmp(f.name, name) == 0) return &f;
    return nullptr;
}

static bool LoadDllHostPayload(const std::wstring& binDir, const std::wstring& dlDir) {
    const FileSpec* spec = FindFileSpec(L"wpc.dll");
    if (!spec) return false;
    Bytes expected;
    if (!ReadPinnedFile(dlDir + L"\\wpc.dll", *spec, expected) &&
        !ReadPinnedFile(binDir + L"\\wpc.dll", *spec, expected)) {
        Wh_Log(L"[dllhost] pinned wpc.dll is not ready; elevated WPC objects are not published");
        return false;
    }
    HMODULE module = LoadVerified(binDir + L"\\wpc.dll", expected,
                                  LOAD_WITH_ALTERED_SEARCH_PATH, LoadLibraryExW);
    if (!module) return false;
    g_hWpc.store(module);
    Wh_Log(L"[dllhost] verified private wpc.dll loaded; elevated WPC path ready");
    return true;
}

BOOL Wh_ModInit() {
    if (!IsRunningAsAmd64()) {
        Wh_Log(L"Not an x64 process; mod not started");
        return FALSE;
    }
    LoadLanguageSetting();
    LoadSkinSetting();
    g_forceTranslations.store(Wh_GetIntSetting(L"forceTranslations") != 0);

    wchar_t exe[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    const wchar_t* exeName = wcsrchr(exe, L'\\');
    exeName = exeName ? exeName + 1 : exe;
    const std::wstring commandLine = LowerText(GetCommandLineW() ? GetCommandLineW() : L"");

    if (_wcsicmp(exeName, L"explorer.exe") == 0) g_role = ProcessRole::Explorer;
    else if (_wcsicmp(exeName, L"control.exe") == 0) g_role = ProcessRole::Control;
    else if (_wcsicmp(exeName, L"dllhost.exe") == 0 ||
             _wcsicmp(exeName, L"wpc-initprobe.exe") == 0) g_role = ProcessRole::DllHost;
    else if (_wcsicmp(exeName, L"svchost.exe") == 0 &&
             (commandLine.find(L"rpcss") != std::wstring::npos ||
              commandLine.find(L"dcomlaunch") != std::wstring::npos ||
              commandLine.find(L"appinfo") != std::wstring::npos)) {
        g_role = ProcessRole::ComService;
    } else {
        return FALSE;
    }

    const std::wstring dir = StorageDir();
    if (dir.empty()) return FALSE;
    const std::wstring dlDir = dir + L"\\dl";
    const std::wstring binDir = dir + L"\\bin";
    g_binDirLower = LowerText(binDir + L"\\");

    if (g_role == ProcessRole::ComService) {
        // Required service-side registration only: keep the existing service
        // filter above. Do not load WPCCPL/Wpc/DUI payloads or install the
        // Explorer COM/translation/Shell32 hooks in these service hosts.
        // Registration must exist before Explorer's first download finishes.
        // This process never loads a payload and never changes Shell32.
        vreg::CleanupStaleHives();
        if (!vreg::InstallHooks(false)) return FALSE;
        PublishVirtualRegistration(binDir);
        Wh_Log(L"[com-service] read-only WPC registration view published");
        return TRUE;
    }

    if (g_role == ProcessRole::Control) {
        if (GetFileAttributesW((binDir + L"\\wpccpl.dll").c_str()) == INVALID_FILE_ATTRIBUTES) return FALSE;
        vreg::CleanupStaleHives();
        if (!vreg::InstallHooks(false)) return FALSE;
        PublishVirtualRegistration(binDir, true);
        return TRUE;
    }

    if (g_role == ProcessRole::DllHost) {
        if (!LoadDllHostPayload(binDir, dlDir)) return FALSE;
        InstallWpcInstallStateHook();
        vreg::CleanupStaleHives();
        if (!vreg::InstallHooks(true)) return FALSE;
        PublishVirtualRegistration(binDir);
        return TRUE;
    }

    // Explorer: asynchronous payload setup, then full UI registration.
    vreg::CleanupStaleHives();
    InstallTranslationHook();
    g_shuttingDown.store(false);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    BlankHideString(GetModuleHandleW(L"shell32.dll"));
    if (!InstallWpcInstallStateHook()) {
        Wh_Log(L"[wpc] continuing without the install-state bypass is not supported on this VM");
        RestoreHideStrings();
        return FALSE;
    }
    if (!vreg::InstallHooks(true)) {
        RestoreHideStrings();
        return FALSE;
    }
    try {
        g_setupThread.emplace([] {
            try { RunSetup(); } catch (...) { Wh_Log(L"Setup threw an exception"); }
        });
    } catch (...) {
        Wh_Log(L"Could not start the setup thread");
        RestoreHideStrings();
        return FALSE;
    }
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    const int oldMode = g_selectedLanguageMode.load();
    const bool oldForce = g_forceTranslations.load();
    const int oldSkin = g_skin.load();
    LoadLanguageSetting();
    LoadSkinSetting();
    const bool newForce = Wh_GetIntSetting(L"forceTranslations") != 0;
    g_forceTranslations.store(newForce);
    const int newMode = g_selectedLanguageMode.load();
    const bool languageChanged = oldMode != newMode;
    const bool forceChanged = oldForce != newForce;
    const bool skinChanged = oldSkin != g_skin.load();
    if (reload) *reload = (languageChanged || skinChanged) ? TRUE : FALSE;

    const std::wstring dir = StorageDir();
    if (dir.empty()) return TRUE;
    const std::wstring dlDir = dir + L"\\dl";
    const std::wstring binDir = dir + L"\\bin";
    if (g_role == ProcessRole::Explorer && (languageChanged || forceChanged || skinChanged)) {
        RefreshGeneratedMuiFiles(dlDir, binDir);
        if (newForce) {
            std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
            if (GetFileAttributesW((binDir + L"\\wpccpl.dll").c_str()) != INVALID_FILE_ATTRIBUTES)
                BuildLocalizedResourceModule(binDir + L"\\wpccpl.dll", binDir);
        } else {
            ReleaseLocalizedResourceModule();
        }
    }
    if (languageChanged || forceChanged || skinChanged) {
        WriteTasksXml(binDir);   // refresh localized task names
        if (g_role == ProcessRole::Explorer || g_role == ProcessRole::DllHost ||
            g_role == ProcessRole::ComService || g_role == ProcessRole::Control) {
            PublishVirtualRegistration(binDir, g_role == ProcessRole::Control);
        }
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    if (g_role != ProcessRole::Explorer) return;
    g_shuttingDown.store(true);
    if (g_stopEvent) SetEvent(g_stopEvent);
    CancelInFlightDownload();
    if (g_setupThread && g_setupThread->joinable()) g_setupThread->join();
    g_setupThread.reset();
}

void Wh_ModUninit() {
    bool keepFiles = true;
    std::wstring binDir;
    std::wstring resourceToKeep;
    if (g_role == ProcessRole::Explorer) {
        keepFiles = Wh_GetIntSetting(L"keepFilesOnDisable") != 0;
        RestoreHideStrings();
        if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
        const std::wstring dir = StorageDir();
        if (!dir.empty()) binDir = dir + L"\\bin";
        {
            std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
            resourceToKeep = g_localizedResourcePath;
            ReleaseLocalizedResourceModuleLocked();
        }
        if (!binDir.empty())
            CleanupStaleLocalizedResourceModules(binDir, keepFiles ? resourceToKeep : L"");
        {
            std::lock_guard<std::mutex> lock(g_resModuleCacheMutex);
            g_resModuleCache.clear();
        }
        g_binDirLower.clear();
    }
    UnpublishVirtualRegistration();

    // Only Explorer owns setup files. Helper/service processes never delete them.
    if (g_role == ProcessRole::Explorer && !keepFiles) {
        const std::wstring dir = StorageDir();
        if (!dir.empty() && !binDir.empty()) {
            const std::wstring dlDir = dir + L"\\dl";
            static const wchar_t* bins[] = {L"wpccpl.dll", L"wpc.dll", L"DUI76.dll", kWin7SkinDllName};
            for (const wchar_t* name : bins) DeleteFileW((binDir + L"\\" + name).c_str());
            for (const MuiLocaleTarget& target : kMuiLocaleTargets) {
                const std::wstring folder = binDir + L"\\" + target.folder;
                DeleteFileW((folder + L"\\wpccpl.dll.mui").c_str());
                RemoveDirectoryW(folder.c_str());
            }
            for (const FileSpec& f : kFiles) DeleteFileW((dlDir + L"\\" + f.name).c_str());
            RemoveDirectoryW(binDir.c_str());
            RemoveDirectoryW(dlDir.c_str());
        }
    }
    g_role = ProcessRole::Other;
}
