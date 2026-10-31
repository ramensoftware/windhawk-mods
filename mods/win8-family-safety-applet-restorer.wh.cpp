// ==WindhawkMod==
// @id              win8-family-safety-applet-restorer
// @name            Windows 8/8.1 Family Safety Applet Restorer
// @description     This mod restores the legacy Family Safety Control Panel applet on Windows 10 and Windows 11
// @version         1.0.0
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
## Credits 

- AdministratoX - Original implementation and screenshots 
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

static const wchar_t* GetWpcString(uint32_t id, MuiLanguage language) {
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

static bool BuildWpcStringBlockBytes(uint32_t blockId, MuiLanguage language, Bytes& output) {
    using namespace pepatch::detail;
    output.clear();
    for (uint32_t index = 0; index < 16; ++index) {
        const uint32_t id = (blockId - 1) * 16 + index;
        const wchar_t* text = GetWpcString(id, language);
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
                         const wchar_t* langTag, uint16_t primaryLcid) {
    using namespace pepatch;
    if (!langTag || !*langTag || primaryLcid == 0) return {};
    Bytes lnRc = ExtractResource(wpcCpl, "MUI", 0, 1);
    if (lnRc.size() < 60 || memcmp(lnRc.data() + 28, kExpectedMuiChecksums, 32) != 0) return {};
    Bytes muiRc = BuildMuiRcFromLn(lnRc, langTag);
    Bytes version;
    if (muiRc.empty() || !BuildEmbeddedVersionResourceBytes(version) || version.empty()) return {};

    MuiResourceTypeGroup strings{6, {}};
    if (language == MuiLanguage::EN_US) {
        // Preserve the exact English output and its long-standing hash assertion.
        for (const EmbeddedStringBlock& row : kEmbeddedStringBlocks) {
            Bytes block;
            if (!BuildEmbeddedStringBlockBytes(row, block) || block.empty()) return {};
            strings.items.push_back({row.id, std::move(block)});
        }
    } else {
        for (const EmbeddedStringBlock& row : kEmbeddedStringBlocks) {
            Bytes block;
            if (!BuildWpcStringBlockBytes(row.id, language, block) || block.empty()) return {};
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
    switch (GetCurrentEmbeddedLanguage()) {
        case MuiLanguage::IT_IT:
            task1 = L"Imposta Sicurezza famiglia";
            task2 = L"Modifica le impostazioni di Sicurezza famiglia";
            break;
        case MuiLanguage::FA_IR:
            task1 = L"\u062A\u0646\u0638\u06CC\u0645 \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647";
            task2 = L"\u0645\u062F\u06CC\u0631\u06CC\u062A \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u0627\u06CC\u0645\u0646\u06CC \u062E\u0627\u0646\u0648\u0627\u062F\u0647";
            break;
        case MuiLanguage::ES_ES:
            task1 = L"Configurar Protecci\u00F3n infantil";
            task2 = L"Cambiar la configuraci\u00F3n de Protecci\u00F3n infantil";
            break;
        case MuiLanguage::FR_FR:
            task1 = L"Configurer le Contr\u00F4le parental";
            task2 = L"Modifier les param\u00E8tres du Contr\u00F4le parental";
            break;
        case MuiLanguage::PT_BR:
            task1 = L"Configurar a Prote\u00E7\u00E3o para a Fam\u00EDlia";
            task2 = L"Alterar as configura\u00E7\u00F5es da Prote\u00E7\u00E3o para a Fam\u00EDlia";
            break;
        default:
            task1 = L"Set up Family Safety";
            task2 = L"Change Family Safety settings";
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
    return g_localizedResourceLanguage.load() == static_cast<int>(GetCurrentEmbeddedLanguage());
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
        g_localizedResourceLanguage.store(static_cast<int>(selected));
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
    const int wanted = static_cast<int>(GetCurrentEmbeddedLanguage());
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
    const std::wstring tasksUrl = binDir + L"\\FamilySafety.tasks.xml";
    add(applet,
        {S(L"", REG_SZ, force ? GetLocalizedDisplayName() : L"Family Safety"),
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
        {S(L"", REG_SZ, force ? GetLocalizedDisplayName() : L"Family Safety")}, {});
    if (nameOnly) return t;

    add(applet + L"\\defaulticon", {S(L"", REG_EXPAND_SZ, cpl + L",-1")}, {});
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
        {D(L"Enabled", 1), S(L"IconReference", REG_EXPAND_SZ, L"@" + cpl + L",-2")}, {});
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
    if (outs.size() != 11 || Sha256Hex(outs[0].data) != kPatchedWpcCplSha256 ||
        !englishMui || Sha256Hex(englishMui->data) != kGeneratedMuiSha256) {
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
    LoadLanguageSetting();
    const bool newForce = Wh_GetIntSetting(L"forceTranslations") != 0;
    g_forceTranslations.store(newForce);
    const int newMode = g_selectedLanguageMode.load();
    const bool languageChanged = oldMode != newMode;
    const bool forceChanged = oldForce != newForce;
    if (reload) *reload = languageChanged ? TRUE : FALSE;

    const std::wstring dir = StorageDir();
    if (dir.empty()) return TRUE;
    const std::wstring dlDir = dir + L"\\dl";
    const std::wstring binDir = dir + L"\\bin";
    if (g_role == ProcessRole::Explorer && (languageChanged || forceChanged)) {
        RefreshGeneratedMuiFiles(dlDir, binDir);
        if (newForce) {
            std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
            if (GetFileAttributesW((binDir + L"\\wpccpl.dll").c_str()) != INVALID_FILE_ATTRIBUTES)
                BuildLocalizedResourceModule(binDir + L"\\wpccpl.dll", binDir);
        } else {
            ReleaseLocalizedResourceModule();
        }
    }
    if (languageChanged || forceChanged) {
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
            static const wchar_t* bins[] = {L"wpccpl.dll", L"wpc.dll", L"DUI76.dll"};
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
