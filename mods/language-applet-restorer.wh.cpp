// ==WindhawkMod==
// @id              language-applet-restorer
// @name            Language Applet Restorer
// @description     Brings back the Control Panel "Language" applet on later versions of Windows downloading the original files from Microsoft's symbol server
// @version         1.0.0
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @include         rundll32.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lwininet -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Language Applet Restorer

Restores the classic Control Panel **Language** applet from windows 10 1709 and optionally the classic **Region** applet (`intl.cpl`) of Windows 10 1709 on later versions of Windows.

## Preview:

![mainpage](https://raw.githubusercontent.com/AdmXP8/assets/main/Screenshot%202026-10-21%20151430.png)

![advancepage](https://raw.githubusercontent.com/AdmXP8/assets/main/Screenshot%202026-10-21%20151455.png)

![addlang](https://raw.githubusercontent.com/AdmXP8/assets/main/Screenshot%202026-10-21%20151500.png)

![intl.cpl](https://raw.githubusercontent.com/AdmXP8/assets/main/intl1709.png)

## What is downloaded, from where, and when

The mod does not contain any Microsoft binaries. The first time it runs inside `explorer.exe`, it downloads five files of Windows 10 1709 (build 16299), about 3.7 MB in total, from Microsoft's public symbol server (`https://msdl.microsoft.com/download/symbols/`):

`intl.cpl`, `UserLanguagesCpl.dll`, `WinLangdb.dll`, `Bcp47Langs.dll` and `dui70.dll`.

The size and SHA-256 of every file are checked against values pinned in the mod, and a file that doesn't match is discarded. The originals are kept in the mod's storage folder (`dl`); patched copies and small compatibility shims are generated from them into `bin`. Nothing is downloaded again as long as `dl` is present.

**Offline:** if the files are not on disk and there is no internet connection, the applet is simply not added and the reason is written to the mod's log. Connect to the internet and restart `explorer.exe` once. After a successful first run the mod works offline, provided *Keep downloaded files* is on.

## Settings

- **Page language**: language of the restored Language page and its Control Panel tooltip. *Automatic* follows the Windows UI language. Available: English, Italian, Persian, Spanish, French, Portuguese.
- **Force translations**: use the embedded translations for the six languages above, instead of relying only on the generated `.mui` files.
- **Keep downloaded files when the mod is disabled**: keep `dl` and `bin` in the storage folder so the mod works offline and doesn't download again. If off, the files are deleted when the mod is unloaded.
- **Redirect Control Panel's intl.cpl to the restored one**: when enabled, opening Region (`intl.cpl`) from Control Panel, the Run dialog or `control.exe` loads the restored 1709 `intl.cpl` instead of the system one. This is done in `explorer.exe`, `rundll32.exe` and `control.exe`; the mod does nothing in other processes. It only affects windows opened after the setting is changed. The restored file is checked against the expected contents right before it is loaded; if that fails, or the files are not downloaded yet, the system `intl.cpl` is used.

## Notes

- Only **x64** is supported (no ARM64).
- The 2017 components run inside `explorer.exe`. A crash in them restarts Explorer.
- The mod writes nothing to the user's registry. The applet's registry entries are provided virtually by registry hooks inside `explorer.exe` (`control.exe` only gets the part needed to find the applet by name), and the applet's COM classes are created from inside `explorer.exe`. Every virtual key is handed out as a real, read-only handle to an empty private registry hive (`vreg-<pid>.hiv` in the mod's storage folder, loaded with `RegLoadAppKeyW`), so that registry APIs that are not hooked cannot crash on it. The hive is not part of the user's registry, holds no data, and is deleted when the mod is unloaded (leftovers after a crash are removed at the next start). If the hive cannot be loaded, the applet is simply not shown.
- **Compatibility:** `intl.cpl` redirection overlaps with the `win7-intl-control-panel-restorer` mod. Don't enable the redirect in both mods at the same time: whichever hook runs first decides which `intl.cpl` is opened.
- While the mod is enabled, one string in `shell32.dll`'s memory (the applet's entry in Windows' built-in hide list) is blanked so that Control Panel lists the applet. It is restored when the mod is disabled; nothing on disk is changed.
- Links in the applet that open the Settings app are not changed by this mod. The `settings-to-control-panel` mod can redirect them.

## Special Thanks to:
babamohammed2022 for adding various languages.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Page language
  $description: This setting is used to select the language used by the restored Language page and its Control Panel tooltip. "Automatic" follows the current Windows UI language.
  $options:
    - auto: Automatic (Windows UI language)
    - en-US: English (United States)
    - it-IT: Italian (Italiano)
    - fa-IR: Persian (فارسی)
    - es-ES: Spanish (Español)
    - fr-FR: French (Français)
    - pt-BR: Portuguese (Português)

- forceTranslations: true
  $name: Force translations
  $description: If this setting is enabled, the mod forces the use of the embedded translations within the page and generated .mui files for the 6 available languages.

- keepFilesOnDisable: true
  $name: Keep downloaded files when the mod is disabled
  $description: If on, the downloaded and generated files stay in the mod's storage folder so re-enabling works offline. If off, they are deleted on the next unload (files still in use are left and retried later).

- redirectIntlCpl: false
  $name: Redirect Control Panel's intl.cpl to the restored one
  $description: When enabled, opening Region / intl.cpl (from Control Panel, Run dialog or control.exe) loads the Windows 10 1709 intl.cpl from the mod's storage folder instead of the system one. Takes effect immediately for newly opened windows.
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
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
// PURE LOGIC (no Windows API): file table, base64, and "turn the downloaded
// originals into the files the applet needs".
// =============================================================================
// ---- PURE-BEGIN ----
struct FileSpec {
    const wchar_t* name;     // original file name
    const wchar_t* key;      // path on the symbol server: name/TimeDateStamp+SizeOfImage/name
    const char* sha256;      // lower-case hex, of the exact file the server returns
    uint32_t size;
};
// Windows 10 1709 (10.0.16299.x), x64. The hashes come from a real download of each URL.
static const FileSpec kFiles[] = {
    {L"intl.cpl", L"intl.cpl/3FD92BED80000/intl.cpl", "fe8ddf8f29f45b262c4f2bb5e24aa1aa194ff338a53aca10f09c83507ec467c4", 499712},
    {L"UserLanguagesCpl.dll", L"UserLanguagesCpl.dll/07613E6FA3000/UserLanguagesCpl.dll", "96ebcb260336bb84bfeddaeeafb7a9032a8159980e50b5249c4fe577f11067ae", 648704},
    {L"WinLangdb.dll", L"WinLangdb.dll/56BE3A926C000/WinLangdb.dll", "377125f90ceed99024612bd456d1b6848a12c9f107321a09f153414c3d7ecb5e", 424960},
    {L"Bcp47Langs.dll", L"Bcp47Langs.dll/B60E05F56A000/Bcp47Langs.dll", "23bb3a8bcfc6a043fa8410d29199a33e1e0fef5ab57fa257c0ecf90c831f5629", 425152},
    {L"dui70.dll", L"dui70.dll/9ECBA8E71AB000/dui70.dll", "11e0fd294026eca9261d4fb6d39625ac5de84c85ec3d6fcf031bc72cfb9463d3", 1723392},
};


using Bytes = pepatch::Bytes;

// =============================================================================
// Embedded MUI String Tables (English, Italian, Persian, Spanish, French, Portuguese)
// Built directly in source code like performance-info-tools-restorer (no Base64 .mui blobs)
// =============================================================================

enum class MuiLanguage {
    EN_US,
    IT_IT,
    FA_IR,
    ES_ES,
    FR_FR,
    PT_BR
};

struct MuiStringTable {
    UINT id;
    const wchar_t* en;
    const wchar_t* it;
    const wchar_t* fa;
    const wchar_t* es;
    const wchar_t* fr;
    const wchar_t* pt;
};

// Complete UserLanguagesCpl.dll.mui string table (all 181 strings, IDs 1..681)
static const MuiStringTable kMuiStrings[] = {
    {1, L"Language", L"Lingua", L"\u0632\u0628\u0627\u0646", L"Idioma", L"Langue", L"Idioma"},
    {2, L"Customize your language preferences and international settings", L"Personalizza le preferenze della lingua e le impostazioni internazionali", L"\u0633\u0641\u0627\u0631\u0634\u06cc\u200c\u0633\u0627\u0632\u06cc \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0628\u0631\u06af\u0632\u06cc\u062f\u0647 \u0632\u0628\u0627\u0646 \u0648 \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0628\u06cc\u0646\u200c\u0627\u0644\u0645\u0644\u0644\u06cc", L"Personalice sus preferencias de idioma y configuraci\u00f3n internacional", L"Personnaliser vos pr\u00e9f\u00e9rences linguistiques et vos param\u00e8tres internationaux", L"Personalize as suas prefer\u00eancias de idioma e configura\u00e7\u00f5es internacionais"},
    {3, L"This program is blocked by group policy. For more information, contact your system administrator.", L"Questo programma \u00e8 bloccato dai Criteri di gruppo. Per ulteriori informazioni, contattare l'amministratore di sistema.", L"\u0627\u06cc\u0646 \u0628\u0631\u0646\u0627\u0645\u0647 \u062a\u0648\u0633\u0637 \u062e\u0637\u200c\u0645\u0634\u06cc \u06af\u0631\u0648\u0647 \u0645\u0633\u062f\u0648\u062f \u0634\u062f\u0647 \u0627\u0633\u062a. \u0628\u0631\u0627\u06cc \u0627\u0637\u0644\u0627\u0639\u0627\u062a \u0628\u06cc\u0634\u062a\u0631 \u0628\u0627 \u0645\u062f\u06cc\u0631 \u0633\u06cc\u0633\u062a\u0645 \u062e\u0648\u062f \u062a\u0645\u0627\u0633 \u0628\u06af\u06cc\u0631\u06cc\u062f.", L"Este programa est\u00e1 bloqueado por una directiva de grupo. Para obtener m\u00e1s informaci\u00f3n, p\u00f3ngase en contacto con el administrador del sistema.", L"Ce programme est bloqu\u00e9 par une strat\u00e9gie de groupe. Pour plus d'informations, contactez votre administrateur syst\u00e8me.", L"Este programa est\u00e1 bloqueado pela pol\u00edtica de grupo. Para obter mais informa\u00e7\u00f5es, contate o administrador do sistema."},
    {4, L"Help", L"Guida", L"\u0631\u0627\u0647\u0646\u0645\u0627", L"Ayuda", L"Aide", L"Ajuda"},
    {5, L"Add languages", L"Aggiungi lingue", L"\u0627\u0641\u0632\u0648\u062f\u0646 \u0632\u0628\u0627\u0646\u200c\u0647\u0627", L"Agregar idiomas", L"Ajouter des langues", L"Adicionar idiomas"},
    {6, L"Regional variants", L"Varianti regionali", L"\u06af\u0648\u0646\u0647\u200c\u0647\u0627\u06cc \u0645\u0646\u0637\u0642\u0647\u200c\u0627\u06cc", L"Variantes regionales", L"Variantes r\u00e9gionales", L"Variantes regionais"},
    {7, L"Advanced settings", L"Impostazioni avanzate", L"\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u067e\u06cc\u0634\u0631\u0641\u062a\u0647", L"Configuraci\u00f3n avanzada", L"Param\u00e8tres avanc\u00e9s", L"Configura\u00e7\u00f5es avan\u00e7adas"},
    {8, L"Language options", L"Opzioni lingua", L"\u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0632\u0628\u0627\u0646", L"Opciones de idioma", L"Options de langue", L"Op\u00e7\u00f5es de idioma"},
    {9, L"Input method", L"Metodo di input", L"\u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc", L"M\u00e9todo de entrada", L"M\u00e9thode d'entr\u00e9e", L"M\u00e9todo de entrada"},
    {10, L"Options", L"Opzioni", L"\u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627", L"Opciones", L"Options", L"Op\u00e7\u00f5es"},
    {11, L"Remove", L"Rimuovi", L"\u062d\u0630\u0641", L"Quitar", L"Supprimer", L"Remover"},
    {12, L"Move up", L"Sposta su", L"\u0627\u0646\u062a\u0642\u0627\u0644 \u0628\u0647 \u0628\u0627\u0644\u0627", L"Subir", L"Monter", L"Mover para cima"},
    {13, L"Move down", L"Sposta gi\u00f9", L"\u0627\u0646\u062a\u0642\u0627\u0644 \u0628\u0647 \u067e\u0627\u06cc\u06cc\u0646", L"Bajar", L"Descendre", L"Mover para baixo"},
    {14, L"Language name", L"Nome lingua", L"\u0646\u0627\u0645 \u0632\u0628\u0627\u0646", L"Nombre de idioma", L"Nom de la langue", L"Nome do idioma"},
    {16, L"Writing system", L"Sistema di scrittura", L"\u062e\u0637 \u0646\u0648\u0634\u062a\u0627\u0631\u06cc", L"Sistema de escritura", L"Syst\u00e8me d'\u00e9criture", L"Sistema de escrita"},
    {17, L"Common language settings", L"Impostazioni comuni della lingua", L"\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0645\u0634\u062a\u0631\u06a9 \u0632\u0628\u0627\u0646", L"Configuraci\u00f3n com\u00fan de idioma", L"Param\u00e8tres de langue communs", L"Configura\u00e7\u00f5es comuns de idioma"},
    {18, L"Add", L"Aggiungi", L"\u0627\u0641\u0632\u0648\u062f\u0646", L"Agregar", L"Ajouter", L"Adicionar"},
    {19, L"Open", L"Apri", L"\u0628\u0627\u0632 \u06a9\u0631\u062f\u0646", L"Abrir", L"Ouvrir", L"Abrir"},
    {20, L"Number of languages: ", L"Numero di lingue: ", L"\u062a\u0639\u062f\u0627\u062f \u0632\u0628\u0627\u0646\u200c\u0647\u0627: ", L"N\u00famero de idiomas: ", L"Nombre de langues : ", L"N\u00famero de idiomas: "},
    {21, L"Change date, time, or number formats", L"Modifica formati di data, ora o numeri", L"\u062a\u063a\u06cc\u06cc\u0631 \u0642\u0627\u0644\u0628\u200c\u0647\u0627\u06cc \u062a\u0627\u0631\u06cc\u062e\u060c \u0632\u0645\u0627\u0646 \u06cc\u0627 \u0627\u0639\u062f\u0627\u062f", L"Cambiar formatos de fecha, hora o n\u00famero", L"Modifier les formats de date, d'heure ou de nombre", L"Alterar formatos de data, hora ou n\u00famero"},
    {22, L"Administrative", L"Opzioni di amministrazione", L"\u0645\u062f\u06cc\u0631\u06cc\u062a\u06cc", L"Administrativo", L"Administration", L"Administrativo"},
    {23, L"Fonts", L"Caratteri", L"\u0642\u0644\u0645\u200c\u0647\u0627", L"Fuentes", L"Polices", L"Fontes"},
    {24, L"Group languages by", L"Raggruppa lingue per", L"\u06af\u0631\u0648\u0647\u200c\u0628\u0646\u062f\u06cc \u0632\u0628\u0627\u0646\u200c\u0647\u0627 \u0628\u0631 \u0627\u0633\u0627\u0633", L"Agrupar idiomas por", L"Regrouper les langues par", L"Agrupar idiomas por"},
    {25, L"Add a language", L"Aggiungi una lingua", L"\u0627\u0641\u0632\u0648\u062f\u0646 \u06cc\u06a9 \u0632\u0628\u0627\u0646", L"Agregar un idioma", L"Ajouter une langue", L"Adicionar um idioma"},
    {26, L"Common settings", L"Impostazioni comuni", L"\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0645\u0634\u062a\u0631\u06a9", L"Configuraci\u00f3n com\u00fan", L"Param\u00e8tres communs", L"Configura\u00e7\u00f5es comuns"},
    {27, L"Language options", L"Opzioni lingua", L"\u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0632\u0628\u0627\u0646", L"Opciones de idioma", L"Options de langue", L"Op\u00e7\u00f5es de idioma"},
    {28, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {29, L"Expand input methods", L"Espandi metodi di input", L"\u0628\u0627\u0632 \u06a9\u0631\u062f\u0646 \u0631\u0648\u0634\u200c\u0647\u0627\u06cc \u0648\u0631\u0648\u062f\u06cc", L"Expandir m\u00e9todos de entrada", L"D\u00e9velopper les m\u00e9thodes d'entr\u00e9e", L"Expandir m\u00e9todos de entrada"},
    {30, L"Collapse input methods", L"Comprimi metodi di input", L"\u0628\u0633\u062a\u0646 \u0631\u0648\u0634\u200c\u0647\u0627\u06cc \u0648\u0631\u0648\u062f\u06cc", L"Contraer m\u00e9todos de entrada", L"R\u00e9duire les m\u00e9thodes d'entr\u00e9e", L"Recolher m\u00e9todos de entrada"},
    {31, L"Add an input method", L"Aggiungi un metodo di input", L"\u0627\u0641\u0632\u0648\u062f\u0646 \u06cc\u06a9 \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc", L"Agregar un m\u00e9todo de entrada", L"Ajouter une m\u00e9thode d'entr\u00e9e", L"Adicionar um m\u00e9todo de entrada"},
    {32, L"Save", L"Salva", L"\u0630\u062e\u06cc\u0631\u0647", L"Guardar", L"Enregistrer", L"Salvar"},
    {33, L"Expand text services", L"Espandi servizi di testo", L"\u0628\u0627\u0632 \u06a9\u0631\u062f\u0646 \u062e\u062f\u0645\u0627\u062a \u0645\u062a\u0646\u06cc", L"Expandir servicios de texto", L"D\u00e9velopper les services de texte", L"Expandir servi\u00e7os de texto"},
    {34, L"Collapse text servives", L"Comprimi servizi di testo", L"\u0628\u0633\u062a\u0646 \u062e\u062f\u0645\u0627\u062a \u0645\u062a\u0646\u06cc", L"Contraer servicios de texto", L"R\u00e9duire les services de texte", L"Recolher servi\u00e7os de texto"},
    {35, L"Spellchecking preferences:", L"Preferenze controllo ortografico:", L"\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0628\u0631\u06af\u0632\u06cc\u062f\u0647 \u063a\u0644\u0637\u200c\u06af\u06cc\u0631 \u0627\u0645\u0644\u0627\u06cc\u06cc:", L"Preferencias de revisi\u00f3n ortogr\u00e1fica:", L"Pr\u00e9f\u00e9rences de v\u00e9rification orthographique :", L"Prefer\u00eancias de verifica\u00e7\u00e3o ortogr\u00e1fica:"},
    {36, L"Installed", L"Installato", L"\u0646\u0635\u0628\u200c\u0634\u062f\u0647", L"Instalado", L"Install\u00e9", L"Instalado"},
    {38, L"Display language list", L"Elenco lingue di visualizzazione", L"\u0641\u0647\u0631\u0633\u062a \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634", L"Lista de idiomas para mostrar", L"Liste des langues d'affichage", L"Lista de idiomas de exibi\u00e7\u00e3o"},
    {39, L"Text Input List", L"Elenco input di testo", L"\u0641\u0647\u0631\u0633\u062a \u0648\u0631\u0648\u062f\u06cc \u0645\u062a\u0646", L"Lista de entrada de texto", L"Liste de saisie de texte", L"Lista de entrada de texto"},
    {40, L"Click to open language bar options", L"Fai clic per aprire le opzioni della barra della lingua", L"\u0628\u0631\u0627\u06cc \u0628\u0627\u0632 \u06a9\u0631\u062f\u0646 \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0646\u0648\u0627\u0631 \u0632\u0628\u0627\u0646 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f", L"Haga clic para abrir las opciones de la barra de idioma", L"Cliquez pour ouvrir les opzioni de la barre de langue", L"Clique para abrir as op\u00e7\u00f5es da barra de idiomas"},
    {41, L"Log off now", L"Disconnetti ora", L"\u062e\u0631\u0648\u062c \u0627\u0632 \u0633\u06cc\u0633\u062a\u0645 \u0627\u06a9\u0646\u0648\u0646", L"Cerrar sesi\u00f3n ahora", L"Se d\u00e9connecter maintenant", L"Fazer logoff agora"},
    {42, L"Change display language", L"Modifica lingua di visualizzazione", L"\u062a\u063a\u06cc\u06cc\u0631 \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634", L"Cambiar idioma para mostrar", L"Modifier la langue d'affichage", L"Alterar idioma de exibi\u00e7\u00e3o"},
    {43, L"You must log off for display language changes to take effect", L"\u00c8 necessario disconnettersi per rendere effettive le modifiche alla lingua di visualizzazione", L"\u0628\u0631\u0627\u06cc \u0627\u0639\u0645\u0627\u0644 \u062a\u063a\u06cc\u06cc\u0631\u0627\u062a \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0628\u0627\u06cc\u062f \u0627\u0632 \u0633\u06cc\u0633\u062a\u0645 \u062e\u0627\u0631\u062c \u0634\u0648\u06cc\u062f", L"Debe cerrar la sesi\u00f3n para que los cambios de idioma para mostrar surtan efecto", L"Vous devez vous d\u00e9connecter pour que les modifications de la langue d'affichage prennent effet", L"Voc\u00ea deve fazer logoff para que as altera\u00e7\u00f5es no idioma de exibi\u00e7\u00e3o entrem em vigor"},
    {44, L"Make sure you save your work and close all open programs before you log off.", L"Assicurati di salvare il lavoro e chiudere tutti i programmi aperti prima di disconnetterti.", L"\u067e\u06cc\u0634 \u0627\u0632 \u062e\u0631\u0648\u062c \u0627\u0632 \u0633\u06cc\u0633\u062a\u0645\u060c \u0645\u0637\u0645\u0626\u0646 \u0634\u0648\u06cc\u062f \u06a9\u0627\u0631 \u062e\u0648\u062f \u0631\u0627 \u0630\u062e\u06cc\u0631\u0647 \u06a9\u0631\u062f\u0647 \u0648 \u0647\u0645\u0647 \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627\u06cc \u0628\u0627\u0632 \u0631\u0627 \u0628\u0633\u062a\u0647\u200c\u0627\u06cc\u062f.", L"Aseg\u00farese de guardar su trabajo y cerrar todos los programas abiertos antes de cerrar la sesi\u00f3n.", L"Assurez-vous d'enregistrer votre travail et de fermer tous les programmes ouverts avant de vous d\u00e9connecter.", L"Certifique-se de salvar seu trabalho e fechar todos os programas abertos antes de fazer logoff."},
    {45, L"Administrative privileges required", L"Privilegi di amministratore richiesti", L"\u0646\u06cc\u0627\u0632\u0645\u0646\u062f \u062f\u0633\u062a\u0631\u0633\u06cc \u0645\u062f\u06cc\u0631 \u0633\u06cc\u0633\u062a\u0645", L"Se requieren privilegios administrativos", L"Privil\u00e8ges d'administrateur requis", L"Privil\u00e9gios administrativos necess\u00e1rios"},
    {46, L"Languages enabled explorer", L"Esplora lingue abilitate", L"\u06a9\u0627\u0648\u0634\u06af\u0631 \u0632\u0628\u0627\u0646\u200c\u0647\u0627\u06cc \u0641\u0639\u0627\u0644", L"Explorador de idiomas habilitados", L"Explorateur des langues activ\u00e9es", L"Explorador de idiomas habilitados"},
    {47, L"Languages choices explorer", L"Esplora scelte lingua", L"\u06a9\u0627\u0648\u0634\u06af\u0631 \u0627\u0646\u062a\u062e\u0627\u0628\u200c\u0647\u0627\u06cc \u0632\u0628\u0627\u0646", L"Explorador de opciones de idioma", L"Explorateur de choix de langues", L"Explorador de op\u00e7\u00f5es de idioma"},
    {48, L"Input method explorer", L"Esplora metodi di input", L"\u06a9\u0627\u0648\u0634\u06af\u0631 \u0631\u0648\u0634\u200c\u0647\u0627\u06cc \u0648\u0631\u0648\u062f\u06cc", L"Explorador de m\u00e9todos de entrada", L"Explorateur de m\u00e9thodes d'entr\u00e9e", L"Explorador de m\u00e9todos de entrada"},
    {49, L"Search languages", L"Cerca lingue", L"\u062c\u0633\u062a\u062c\u0648\u06cc \u0632\u0628\u0627\u0646\u200c\u0647\u0627", L"Buscar idiomas", L"Rechercher des langues", L"Pesquisar idiomas"},
    {50, L"Add", L"Aggiungi", L"\u0627\u0641\u0632\u0648\u062f\u0646", L"Agregar", L"Ajouter", L"Adicionar"},
    {51, L"Enabled", L"Abilitato", L"\u0641\u0639\u0627\u0644", L"Habilitado", L"Activ\u00e9", L"Habilitado"},
    {52, L"Enabled (override)", L"Abilitato (sostituzione)", L"\u0641\u0639\u0627\u0644 (\u062c\u0627\u06cc\u06af\u0632\u06cc\u0646\u200c\u0634\u062f\u0647)", L"Habilitado (invalidaci\u00f3n)", L"Activ\u00e9 (substitution)", L"Habilitado (substitui\u00e7\u00e3o)"},
    {53, L"Change override", L"Modifica sostituzione", L"\u062a\u063a\u06cc\u06cc\u0631 \u062c\u0627\u06cc\u06af\u0632\u06cc\u0646\u06cc", L"Cambiar invalidaci\u00f3n", L"Modifier la substitution", L"Alterar substitui\u00e7\u00e3o"},
    {54, L"Available", L"Disponibile", L"\u062f\u0631 \u062f\u0633\u062a\u0631\u0633", L"Disponible", L"Disponible", L"Dispon\u00edvel"},
    {55, L"Make %1 your primary language (you\u2019ll see it at the top of your list)", L"Imposta %1 come lingua principale (verr\u00e0 visualizzata in cima all'elenco)", L"\u062a\u0646\u0638\u06cc\u0645 %1 \u0628\u0647 \u0639\u0646\u0648\u0627\u0646 \u0632\u0628\u0627\u0646 \u0627\u0635\u0644\u06cc \u0634\u0645\u0627 (\u062f\u0631 \u0628\u0627\u0644\u0627\u06cc \u0641\u0647\u0631\u0633\u062a \u0634\u0645\u0627 \u0646\u0645\u0627\u06cc\u0634 \u062f\u0627\u062f\u0647 \u0645\u06cc\u200c\u0634\u0648\u062f)", L"Hacer que %1 sea su idioma principal (lo ver\u00e1 en la parte superior de la lista)", L"Faire de %1 votre langue principale (elle appara\u00eetra en haut de votre liste)", L"Tornar %1 seu idioma principal (voc\u00ea o ver\u00e1 no topo da sua lista)"},
    {56, L"Make this the primary language", L"Imposta come lingua principale", L"\u062a\u0646\u0638\u06cc\u0645 \u0628\u0647 \u0639\u0646\u0648\u0627\u0646 \u0632\u0628\u0627\u0646 \u0627\u0635\u0644\u06cc", L"Establecer como idioma principal", L"D\u00e9finir comme langue principale", L"Tornar este o idioma principal"},
    {57, L"Will be enabled on next sign-in", L"Verr\u00e0 abilitato al prossimo accesso", L"\u062f\u0631 \u0648\u0631\u0648\u062f \u0628\u0639\u062f\u06cc \u0641\u0639\u0627\u0644 \u062e\u0648\u0627\u0647\u062f \u0634\u062f", L"Se habilitar\u00e1 en el pr\u00f3ximo inicio de sesi\u00f3n", L"Sera activ\u00e9 \u00e0 la prochaine connexion", L"Ser\u00e1 habilitado no pr\u00f3ximo login"},
    {58, L"Will be enabled on next sign-in", L"Verr\u00e0 abilitato al prossimo accesso", L"\u062f\u0631 \u0648\u0631\u0648\u062f \u0628\u0639\u062f\u06cc \u0641\u0639\u0627\u0644 \u062e\u0648\u0627\u0647\u062f \u0634\u062f", L"Se habilitar\u00e1 en el pr\u00f3ximo inicio de sesi\u00f3n", L"Sera activ\u00e9 \u00e0 la prochaine connexion", L"Ser\u00e1 habilitado no pr\u00f3ximo login"},
    {59, L"Change override", L"Modifica sostituzione", L"\u062a\u063a\u06cc\u06cc\u0631 \u062c\u0627\u06cc\u06af\u0632\u06cc\u0646\u06cc", L"Cambiar invalidaci\u00f3n", L"Modifier la substitution", L"Alterar substitui\u00e7\u00e3o"},
    {60, L"Available for download", L"Disponibile per il download", L"\u0622\u0645\u0627\u062f\u0647 \u0628\u0631\u0627\u06cc \u062f\u0627\u0646\u0644\u0648\u062f", L"Disponible para descargar", L"Disponible au t\u00e9l\u00e9chargement", L"Dispon\u00edvel para download"},
    {61, L"A language pack for %1 is available for download", L"\u00c8 disponibile per il download un Language Pack per %1", L"\u0628\u0633\u062a\u0647 \u0632\u0628\u0627\u0646\u06cc \u0628\u0631\u0627\u06cc %1 \u062c\u0647\u062a \u062f\u0627\u0646\u0644\u0648\u062f \u062f\u0631 \u062f\u0633\u062a\u0631\u0633 \u0627\u0633\u062a", L"Hay un paquete de idioma para %1 disponible para descargar", L"Un module linguistique pour %1 est disponible au t\u00e9l\u00e9chargement", L"Um pacote de idiomas para %1 est\u00e1 dispon\u00edvel para download"},
    {62, L"Download and install language pack", L"Scarica e installa Language Pack", L"\u062f\u0627\u0646\u0644\u0648\u062f \u0648 \u0646\u0635\u0628 \u0628\u0633\u062a\u0647 \u0632\u0628\u0627\u0646", L"Descargar e instalar paquete de idioma", L"T\u00e9l\u00e9charger et installer le module linguistique", L"Baixar e instalar pacote de idiomas"},
    {63, L"If you want to change the display language, talk to the person who manages this account", L"Se desideri modificare la lingua di visualizzazione, rivolgiti alla persona che gestisce questo account", L"\u0627\u06af\u0631 \u0645\u06cc\u200c\u062e\u0648\u0627\u0647\u06cc\u062f \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0631\u0627 \u062a\u063a\u06cc\u06cc\u0631 \u062f\u0647\u06cc\u062f\u060c \u0628\u0627 \u0641\u0631\u062f\u06cc \u06a9\u0647 \u0627\u06cc\u0646 \u062d\u0633\u0627\u0628 \u0631\u0627 \u0645\u062f\u06cc\u0631\u06cc\u062a \u0645\u06cc\u200c\u06a9\u0646\u062f \u0635\u062d\u0628\u062a \u06a9\u0646\u06cc\u062f", L"Si desea cambiar el idioma para mostrar, hable con la persona que administra esta cuenta", L"Si vous souhaitez modifier la langue d'affichage, adressez-vous \u00e0 la personne qui g\u00e8re ce compte", L"Se voc\u00ea quiser alterar o idioma de exibi\u00e7\u00e3o, fale com a pessoa que gerencia esta conta"},
    {66, L"A language pack isn't available", L"Language Pack non disponibile", L"\u0628\u0633\u062a\u0647 \u0632\u0628\u0627\u0646 \u062f\u0631 \u062f\u0633\u062a\u0631\u0633 \u0646\u06cc\u0633\u062a", L"No hay ning\u00fan paquete de idioma disponible", L"Aucun module linguistique n'est disponible", L"Um pacote de idiomas n\u00e3o est\u00e1 dispon\u00edvel"},
    {67, L"The display language has been set to another language using override", L"La lingua di visualizzazione \u00e8 stata impostata su un'altra lingua tramite sostituzione", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0628\u0627 \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0627\u0632 \u062c\u0627\u06cc\u06af\u0632\u06cc\u0646\u06cc \u0631\u0648\u06cc \u0632\u0628\u0627\u0646 \u062f\u06cc\u06af\u0631\u06cc \u062a\u0646\u0638\u06cc\u0645 \u0634\u062f\u0647 \u0627\u0633\u062a", L"El idioma para mostrar se ha establecido en otro idioma mediante invalidaci\u00f3n", L"La langue d'affichage a \u00e9t\u00e9 d\u00e9finie sur une autre langue via une substitution", L"O idioma de exibi\u00e7\u00e3o foi definido para outro idioma usando substitui\u00e7\u00e3o"},
    {68, L"Advanced settings", L"Impostazioni avanzate", L"\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u067e\u06cc\u0634\u0631\u0641\u062a\u0647", L"Configuraci\u00f3n avanzada", L"Param\u00e8tres avanc\u00e9s", L"Configura\u00e7\u00f5es avan\u00e7adas"},
    {69, L"Location", L"Posizione", L"\u0645\u06a9\u0627\u0646", L"Ubicaci\u00f3n", L"Emplacement", L"Local"},
    {70, L"Windows display language: %1\nKeyboard layout: %2", L"Lingua di visualizzazione di Windows: %1\nLayout di tastiera: %2", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632: %1\n\u0686\u06cc\u062f\u0645\u0627\u0646 \u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f: %2", L"Idioma para mostrar de Windows: %1\nDistribuci\u00f3n del teclado: %2", L"Langue d'affichage de Windows : %1\nDisposition du clavier : %2", L"Idioma de exibi\u00e7\u00e3o do Windows: %1\nLayout do teclado: %2"},
    {71, L"%3. Windows display language: %1. Keyboard layout: %2. Click for more options.", L"%3. Lingua di visualizzazione di Windows: %1. Layout di tastiera: %2. Fai clic per altre opzioni.", L"%3. \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632: %1. \u0686\u06cc\u062f\u0645\u0627\u0646 \u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f: %2. \u0628\u0631\u0627\u06cc \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%3. Idioma para mostrar de Windows: %1. Distribuci\u00f3n del teclado: %2. Haga clic para ver m\u00e1s opciones.", L"%3. Langue d'affichage de Windows : %1. Disposition du clavier : %2. Cliquez pour plus d'options.", L"%3. Idioma de exibi\u00e7\u00e3o do Windows: %1. Layout do teclado: %2. Clique para mais op\u00e7\u00f5es."},
    {72, L"Keyboard layout: %1", L"Layout di tastiera: %1", L"\u0686\u06cc\u062f\u0645\u0627\u0646 \u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f: %1", L"Distribuci\u00f3n del teclado: %1", L"Disposition du clavier : %1", L"Layout do teclado: %1"},
    {73, L"%3. Keyboard layout: %2. Click for more options.", L"%3. Layout di tastiera: %2. Fai clic per altre opzioni.", L"%3. \u0686\u06cc\u062f\u0645\u0627\u0646 \u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f: %2. \u0628\u0631\u0627\u06cc \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%3. Distribuci\u00f3n del teclado: %2. Haga clic para ver m\u00e1s opciones.", L"%3. Disposition du clavier : %2. Cliquez pour plus d'options.", L"%3. Layout do teclado: %2. Clique para mais op\u00e7\u00f5es."},
    {74, L"Windows display language: %1\nInput method: %2", L"Lingua di visualizzazione di Windows: %1\nMetodo di input: %2", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632: %1\n\u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc: %2", L"Idioma para mostrar de Windows: %1\nM\u00e9todo de entrada: %2", L"Langue d'affichage de Windows : %1\nM\u00e9thode d'entr\u00e9e : %2", L"Idioma de exibi\u00e7\u00e3o do Windows: %1\nM\u00e9todo de entrada: %2"},
    {75, L"%3. Windows display language: %1. Input method: %2. Click for more options.", L"%3. Lingua di visualizzazione di Windows: %1. Metodo di input: %2. Fai clic per altre opzioni.", L"%3. \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632: %1. \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc: %2. \u0628\u0631\u0627\u06cc \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%3. Idioma para mostrar de Windows: %1. M\u00e9todo de entrada: %2. Haga clic para ver m\u00e1s opciones.", L"%3. Langue d'affichage de Windows : %1. M\u00e9thode d'entr\u00e9e : %2. Cliquez pour plus d'options.", L"%3. Idioma de exibi\u00e7\u00e3o do Windows: %1. M\u00e9todo de entrada: %2. Clique para mais op\u00e7\u00f5es."},
    {76, L"Input method: %1", L"Metodo di input: %1", L"\u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc: %1", L"M\u00e9todo de entrada: %1", L"M\u00e9thode d'entr\u00e9e : %1", L"M\u00e9todo de entrada: %1"},
    {77, L"%3. Input method: %2. Click for more options.", L"%3. Metodo di input: %2. Fai clic per altre opzioni.", L"%3. \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc: %2. \u0628\u0631\u0627\u06cc \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%3. M\u00e9todo de entrada: %2. Haga clic para ver m\u00e1s opciones.", L"%3. M\u00e9thode d'entr\u00e9e : %2. Cliquez pour plus d'options.", L"%3. M\u00e9todo de entrada: %2. Clique para mais op\u00e7\u00f5es."},
    {78, L"%1\n%2", L"%1\n%2", L"%1\n%2", L"%1\n%2", L"%1\n%2", L"%1\n%2"},
    {79, L"%1. %2. Click for preview.", L"%1. %2. Fai clic per l'anteprima.", L"%1. %2. \u0628\u0631\u0627\u06cc \u067e\u06cc\u0634\u200c\u0646\u0645\u0627\u06cc\u0634 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%1. %2. Haga clic para obtener una vista previa.", L"%1. %2. Cliquez pour un aper\u00e7u.", L"%1. %2. Clique para visualizar."},
    {80, L"Preview", L"Anteprima", L"\u067e\u06cc\u0634\u200c\u0646\u0645\u0627\u06cc\u0634", L"Vista previa", L"Aper\u00e7u", L"Visualizar"},
    {81, L"Standard", L"Standard", L"\u0627\u0633\u062a\u0627\u0646\u062f\u0627\u0631\u062f", L"Est\u00e1ndar", L"Standard", L"Padr\u00e3o"},
    {82, L"Touch keyboard layout", L"Layout tastiera virtuale", L"\u0686\u06cc\u062f\u0645\u0627\u0646 \u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f \u0644\u0645\u0633\u06cc", L"Distribuci\u00f3n del teclado t\u00e1ctil", L"Disposition du clavier tactile", L"Layout do teclado virtual"},
    {83, L"IME", L"IME", L"IME", L"IME", L"IME", L"IME"},
    {84, L"Search regional variants", L"Cerca varianti regionali", L"\u062c\u0633\u062a\u062c\u0648\u06cc \u06af\u0648\u0646\u0647\u200c\u0647\u0627\u06cc \u0645\u0646\u0637\u0642\u0647\u200c\u0627\u06cc", L"Buscar variantes regionales", L"Rechercher des variantes r\u00e9gionales", L"Pesquisar variantes regionais"},
    {85, L"Search input methods", L"Cerca metodi di input", L"\u062c\u0633\u062a\u062c\u0648\u06cc \u0631\u0648\u0634\u200c\u0647\u0627\u06cc \u0648\u0631\u0648\u062f\u06cc", L"Buscar m\u00e9todos de entrada", L"Rechercher des m\u00e9thodes d'entr\u00e9e", L"Pesquisar m\u00e9todos de entrada"},
    {88, L"Change language bar hot keys", L"Modifica tasti di scelta rapida della barra della lingua", L"\u062a\u063a\u06cc\u06cc\u0631 \u06a9\u0644\u06cc\u062f\u0647\u0627\u06cc \u0645\u06cc\u0627\u0646\u0628\u0631 \u0646\u0648\u0627\u0631 \u0632\u0628\u0627\u0646", L"Cambiar teclas de acceso r\u00e1pido de la barra de idioma", L"Modifier les touches d'acc\u00e8s rapide de la barre de langue", L"Alterar teclas de atalho da barra de idiomas"},
    {91, L"none available", L"nessuno disponibile", L"\u0645\u0648\u0631\u062f\u06cc \u062f\u0631 \u062f\u0633\u062a\u0631\u0633 \u0646\u06cc\u0633\u062a", L"ninguno disponible", L"aucun disponible", L"nenhum dispon\u00edvel"},
    {92, L"|", L"|", L"|", L"|", L"|", L"|"},
    {93, L"Windows display language: %1\nKeyboard layout: %2...", L"Lingua di visualizzazione di Windows: %1\nLayout di tastiera: %2...", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632: %1\n\u0686\u06cc\u062f\u0645\u0627\u0646 \u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f: %2...", L"Idioma para mostrar de Windows: %1\nDistribuci\u00f3n del teclado: %2...", L"Langue d'affichage de Windows : %1\nDisposition du clavier : %2...", L"Idioma de exibi\u00e7\u00e3o do Windows: %1\nLayout do teclado: %2..."},
    {94, L"Keyboard layout: %2...", L"Layout di tastiera: %2...", L"\u0686\u06cc\u062f\u0645\u0627\u0646 \u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f: %2...", L"Distribuci\u00f3n del teclado: %2...", L"Disposition du clavier : %2...", L"Layout do teclado: %2..."},
    {95, L"Windows display language: %1\nInput method: %2...", L"Lingua di visualizzazione di Windows: %1\nMetodo di input: %2...", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632: %1\n\u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc: %2...", L"Idioma para mostrar de Windows: %1\nM\u00e9todo de entrada: %2...", L"Langue d'affichage de Windows : %1\nM\u00e9thode d'entr\u00e9e : %2...", L"Idioma de exibi\u00e7\u00e3o do Windows: %1\nM\u00e9todo de entrada: %2..."},
    {96, L"Input method: %2...", L"Metodo di input: %2...", L"\u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc: %2...", L"M\u00e9todo de entrada: %2...", L"M\u00e9thode d'entr\u00e9e : %2...", L"M\u00e9todo de entrada: %2..."},
    {97, L"Checking availability...", L"Verifica disponibilit\u00e0 in corso...", L"\u062f\u0631 \u062d\u0627\u0644 \u0628\u0631\u0631\u0633\u06cc \u062f\u0631 \u062f\u0633\u062a\u0631\u0633 \u0628\u0648\u062f\u0646...", L"Comprobando disponibilidad...", L"V\u00e9rification de la disponibilit\u00e9...", L"Verificando disponibilidade..."},
    {98, L"Enabled (will change after next sign-in)", L"Abilitato (cambier\u00e0 al prossimo accesso)", L"\u0641\u0639\u0627\u0644 (\u067e\u0633 \u0627\u0632 \u0648\u0631\u0648\u062f \u0628\u0639\u062f\u06cc \u062a\u063a\u06cc\u06cc\u0631 \u062e\u0648\u0627\u0647\u062f \u06a9\u0631\u062f)", L"Habilitado (cambiar\u00e1 despu\u00e9s del pr\u00f3ximo inicio de sesi\u00f3n)", L"Activ\u00e9 (changera apr\u00e8s la prochaine connexion)", L"Habilitado (mudar\u00e1 ap\u00f3s o pr\u00f3ximo login)"},
    {99, L"Enabled (will change after next sign-in)", L"Abilitato (cambier\u00e0 al prossimo accesso)", L"\u0641\u0639\u0627\u0644 (\u067e\u0633 \u0627\u0632 \u0648\u0631\u0648\u062f \u0628\u0639\u062f\u06cc \u062a\u063a\u06cc\u06cc\u0631 \u062e\u0648\u0627\u0647\u062f \u06a9\u0631\u062f)", L"Habilitado (cambiar\u00e1 despu\u00e9s del pr\u00f3ximo inicio de sesi\u00f3n)", L"Activ\u00e9 (changera apr\u00e8s la prochaine connexion)", L"Habilitado (mudar\u00e1 ap\u00f3s o pr\u00f3ximo login)"},
    {100, L"Use language list (recommended)", L"Usa elenco lingue (consigliato)", L"\u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0627\u0632 \u0641\u0647\u0631\u0633\u062a \u0632\u0628\u0627\u0646 (\u062a\u0648\u0635\u06cc\u0647\u200c\u0634\u062f\u0647)", L"Usar lista de idiomas (recomendado)", L"Utiliser la liste des langues (recommand\u00e9)", L"Usar lista de idiomas (recomendado)"},
    {101, L"_stack", L"_stack", L"_stack", L"_stack", L"_stack", L"_stack"},
    {102, L"Install or uninstall display languages. Administrative privileges required", L"Installa o disinstalla lingue di visualizzazione. Privilegi di amministratore richiesti", L"\u0646\u0635\u0628 \u06cc\u0627 \u062d\u0630\u0641 \u0632\u0628\u0627\u0646\u200c\u0647\u0627\u06cc \u0646\u0645\u0627\u06cc\u0634. \u0646\u06cc\u0627\u0632\u0645\u0646\u062f \u062f\u0633\u062a\u0631\u0633\u06cc \u0645\u062f\u06cc\u0631 \u0633\u06cc\u0633\u062a\u0645", L"Instalar o desinstalar idiomas para mostrar. Se requieren privilegios administrativos", L"Installer ou d\u00e9sinstaller des langues d'affichage. Privil\u00e8ges d'administrateur requis", L"Instalar ou desinstalar idiomas de exibi\u00e7\u00e3o. Privil\u00e9gios administrativos necess\u00e1rios"},
    {103, L"What can I do to get more languages?", L"Come posso ottenere altre lingue?", L"\u0686\u06af\u0648\u0646\u0647 \u0645\u06cc\u200c\u062a\u0648\u0627\u0646\u0645 \u0632\u0628\u0627\u0646\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631\u06cc \u062f\u0631\u06cc\u0627\u0641\u062a \u06a9\u0646\u0645\u061f", L"\u00bfQu\u00e9 puedo hacer para obtener m\u00e1s idiomas?", L"Que puis-je faire pour obtenir plus de langues ?", L"O que posso fazer para obter mais idiomas?"},
    {105, L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2", L"%1 - %2"},
    {106, L"Keyboard", L"Tastiera", L"\u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f", L"Teclado", L"Clavier", L"Teclado"},
    {107, L"Uninstall language pack", L"Disinstalla Language Pack", L"\u062d\u0630\u0641 \u0646\u0635\u0628 \u0628\u0633\u062a\u0647 \u0632\u0628\u0627\u0646", L"Desinstalar paquete de idioma", L"D\u00e9sinstaller le module linguistique", L"Desinstalar pacote de idiomas"},
    {108, L"%1 (not available)", L"%1 (non disponibile)", L"%1 (\u062f\u0631 \u062f\u0633\u062a\u0631\u0633 \u0646\u06cc\u0633\u062a)", L"%1 (no disponible)", L"%1 (non disponible)", L"%1 (n\u00e3o dispon\u00edvel)"},
    {109, L"/u %1 /w", L"/u %1 /w", L"/u %1 /w", L"/u %1 /w", L"/u %1 /w", L"/u %1 /w"},
    {110, L"Apply language settings to the welcome screen, system accounts, and new user accounts", L"Applica le impostazioni della lingua alla schermata iniziale, agli account di sistema e ai nuovi account utente", L"\u0627\u0639\u0645\u0627\u0644 \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0632\u0628\u0627\u0646 \u0628\u0647 \u0635\u0641\u062d\u0647 \u062e\u0648\u0634\u200c\u0622\u0645\u062f\u06af\u0648\u06cc\u06cc\u060c \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u0633\u06cc\u0633\u062a\u0645 \u0648 \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u062c\u062f\u06cc\u062f", L"Aplicar la configuraci\u00f3n de idioma a la pantalla de bienvenida, a las cuentas del sistema y a las cuentas de usuario nuevas", L"Appliquer les param\u00e8tres de langue \u00e0 l'\u00e9cran d'accueil, aux comptes syst\u00e8me et aux nouveaux comptes d'utilisateurs", L"Aplicar configura\u00e7\u00f5es de idioma \u00e0 tela de boas-vindas, contas do sistema e novas contas de usu\u00e1rio"},
    {111, L"Unavailable input method", L"Metodo di input non disponibile", L"\u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc \u0646\u0627\u0645\u0648\u062c\u0648\u062f", L"M\u00e9todo de entrada no disponible", L"M\u00e9thode d'entr\u00e9e non disponible", L"M\u00e9todo de entrada indispon\u00edvel"},
    {112, L"Restore defaults", L"Ripristina predefiniti", L"\u0628\u0627\u0632\u06af\u0631\u062f\u0627\u0646\u06cc \u067e\u06cc\u0634\u200c\u0641\u0631\u0636\u200c\u0647\u0627", L"Restaurar valores predeterminados", L"Restaurer les param\u00e8tres par d\u00e9faut", L"Restaurar padr\u00f5es"},
    {113, L"Click to preview %1 input method", L"Fai clic per visualizzare l'anteprima del metodo di input %1", L"\u0628\u0631\u0627\u06cc \u067e\u06cc\u0634\u200c\u0646\u0645\u0627\u06cc\u0634 \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc %1 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f", L"Haga clic para obtener una vista previa del m\u00e9todo de entrada %1", L"Cliquez pour pr\u00e9visualiser la m\u00e9thode d'entr\u00e9e %1", L"Clique para visualizar o m\u00e9todo de entrada %1"},
    {114, L"Click to open options for %1 input method", L"Fai clic per aprire le opzioni del metodo di input %1", L"\u0628\u0631\u0627\u06cc \u0628\u0627\u0632 \u06a9\u0631\u062f\u0646 \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc %1 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f", L"Haga clic para abrir las opciones del m\u00e9todo de entrada %1", L"Cliquez pour ouvrir les options de la m\u00e9thode d'entr\u00e9e %1", L"Clique para abrir as op\u00e7\u00f5es do m\u00e9todo de entrada %1"},
    {115, L"Click to remove %1 input method", L"Fai clic per rimuovere il metodo di input %1", L"\u0628\u0631\u0627\u06cc \u062d\u0630\u0641 \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc %1 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f", L"Haga clic para quitar el m\u00e9todo de entrada %1", L"Cliquez pour supprimer la m\u00e9thode d'entr\u00e9e %1", L"Clique para remover o m\u00e9todo de entrada %1"},
    {116, L"No items match your search", L"Nessun elemento corrisponde alla ricerca", L"\u0647\u06cc\u0686 \u0645\u0648\u0631\u062f\u06cc \u0628\u0627 \u062c\u0633\u062a\u062c\u0648\u06cc \u0634\u0645\u0627 \u0645\u0637\u0627\u0628\u0642\u062a \u0646\u062f\u0627\u0631\u062f", L"Ning\u00fan elemento coincide con su b\u00fasqueda", L"Aucun \u00e9l\u00e9ment ne correspond \u00e0 votre recherche", L"Nenhum item corresponde \u00e0 sua pesquisa"},
    {117, L"No items match your search", L"Nessun elemento corrisponde alla ricerca", L"\u0647\u06cc\u0686 \u0645\u0648\u0631\u062f\u06cc \u0628\u0627 \u062c\u0633\u062a\u062c\u0648\u06cc \u0634\u0645\u0627 \u0645\u0637\u0627\u0628\u0642\u062a \u0646\u062f\u0627\u0631\u062f", L"Ning\u00fan elemento coincide con su b\u00fasqueda", L"Aucun \u00e9l\u00e9ment ne correspond \u00e0 votre recherche", L"Nenhum item corresponde \u00e0 sua pesquisa"},
    {118, L"Connect to the Internet to check for language packs", L"Connettiti a Internet per verificare la disponibilit\u00e0 di Language Pack", L"\u0628\u0631\u0627\u06cc \u0628\u0631\u0631\u0633\u06cc \u0628\u0633\u062a\u0647\u200c\u0647\u0627\u06cc \u0632\u0628\u0627\u0646 \u0628\u0647 \u0627\u06cc\u0646\u062a\u0631\u0646\u062a \u0645\u062a\u0635\u0644 \u0634\u0648\u06cc\u062f", L"Con\u00e9ctese a Internet para buscar paquetes de idioma", L"Connectez-vous \u00e0 Internet pour rechercher des modules linguistiques", L"Conecte-se \u00e0 Internet para verificar se h\u00e1 pacotes de idiomas"},
    {119, L"%1\nOnly works on the desktop", L"%1\nFunziona solo sul desktop", L"%1\n\u0641\u0642\u0637 \u062f\u0631 \u062f\u0633\u06a9\u062a\u0627\u067e \u06a9\u0627\u0631 \u0645\u06cc\u200c\u06a9\u0646\u062f", L"%1\nSolo funciona en el escritorio", L"%1\nFonctionne uniquement sur le Bureau", L"%1\nFunciona apenas na \u00e1rea de trabalho"},
    {120, L"A language pack isn't available", L"Language Pack non disponibile", L"\u0628\u0633\u062a\u0647 \u0632\u0628\u0627\u0646 \u062f\u0631 \u062f\u0633\u062a\u0631\u0633 \u0646\u06cc\u0633\u062a", L"No hay ning\u00fan paquete de idioma disponible", L"Aucun module linguistique n'est disponible", L"Um pacote de idiomas n\u00e3o est\u00e1 dispon\u00edvel"},
    {121, L"Enabled", L"Abilitato", L"\u0641\u0639\u0627\u0644", L"Habilitado", L"Activ\u00e9", L"Habilitado"},
    {122, L"Will be enabled on next sign-in (override)", L"Verr\u00e0 abilitato al prossimo accesso (sostituzione)", L"\u062f\u0631 \u0648\u0631\u0648\u062f \u0628\u0639\u062f\u06cc \u0641\u0639\u0627\u0644 \u062e\u0648\u0627\u0647\u062f \u0634\u062f (\u062c\u0627\u06cc\u06af\u0632\u06cc\u0646\u200c\u0634\u062f\u0647)", L"Se habilitar\u00e1 en el pr\u00f3ximo inicio de sesi\u00f3n (invalidaci\u00f3n)", L"Sera activ\u00e9 \u00e0 la prochaine connexion (substitution)", L"Ser\u00e1 habilitado no pr\u00f3ximo login (substitui\u00e7\u00e3o)"},
    {123, L"%1", L"%1", L"%1", L"%1", L"%1", L"%1"},
    {124, L"%1. Click for preview.", L"%1. Fai clic per l'anteprima.", L"%1. \u0628\u0631\u0627\u06cc \u067e\u06cc\u0634\u200c\u0646\u0645\u0627\u06cc\u0634 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%1. Haga clic para obtener una vista previa.", L"%1. Cliquez pour un aper\u00e7u.", L"%1. Clique para visualizar."},
    {125, L"Privacy statement", L"Informativa sulla privacy", L"\u0628\u06cc\u0627\u0646\u06cc\u0647 \u062d\u0631\u06cc\u0645 \u062e\u0635\u0648\u0635\u06cc", L"Declaraci\u00f3n de privacidad", L"D\u00e9claration de confidentialit\u00e9", L"Pol\u00edtica de privacidade"},
    {126, L"If you want to change the display language, talk to the person who manages this account", L"Se desideri modificare la lingua di visualizzazione, rivolgiti alla persona che gestisce questo account", L"\u0627\u06af\u0631 \u0645\u06cc\u200c\u062e\u0648\u0627\u0647\u06cc\u062f \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0631\u0627 \u062a\u063a\u06cc\u06cc\u0631 \u062f\u0647\u06cc\u062f\u060c \u0628\u0627 \u0641\u0631\u062f\u06cc \u06a9\u0647 \u0627\u06cc\u0646 \u062d\u0633\u0627\u0628 \u0631\u0627 \u0645\u062f\u06cc\u0631\u06cc\u062a \u0645\u06cc\u200c\u06a9\u0646\u062f \u0635\u062d\u0628\u062a \u06a9\u0646\u06cc\u062f", L"Si desea cambiar el idioma para mostrar, hable con la persona que administra esta cuenta", L"Si vous souhaitez modifier la langue d'affichage, adressez-vous \u00e0 la personne qui g\u00e8re ce compte", L"Se voc\u00ea quiser alterar o idioma de exibi\u00e7\u00e3o, fale com a pessoa que gerencia esta conta"},
    {127, L"This setting is managed by your system administrator", L"Questa impostazione \u00e8 gestita dall'amministratore di sistema", L"\u0627\u06cc\u0646 \u062a\u0646\u0638\u06cc\u0645 \u062a\u0648\u0633\u0637 \u0645\u062f\u06cc\u0631 \u0633\u06cc\u0633\u062a\u0645 \u0634\u0645\u0627 \u0645\u062f\u06cc\u0631\u06cc\u062a \u0645\u06cc\u200c\u0634\u0648\u062f", L"Esta configuraci\u00f3n la administra el administrador del sistema", L"Ce param\u00e8tre est g\u00e9r\u00e9 par votre administrateur syst\u00e8me", L"Esta configura\u00e7\u00e3o \u00e9 gerenciada pelo administrador do sistema"},
    {128, L"Windows display language: %1", L"Lingua di visualizzazione di Windows: %1", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632: %1", L"Idioma para mostrar de Windows: %1", L"Langue d'affichage de Windows : %1", L"Idioma de exibi\u00e7\u00e3o do Windows: %1"},
    {129, L", ", L", ", L"\u060c ", L", ", L", ", L", "},
    {130, L"%3\n%1", L"%3\n%1", L"%3\n%1", L"%3\n%1", L"%3\n%1", L"%3\n%1"},
    {131, L"%2. %3. %1. Click for more options.", L"%2. %3. %1. Fai clic per altre opzioni.", L"%2. %3. %1. \u0628\u0631\u0627\u06cc \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%2. %3. %1. Haga clic para ver m\u00e1s opciones.", L"%2. %3. %1. Cliquez pour plus d'options.", L"%2. %3. %1. Clique para mais op\u00e7\u00f5es."},
    {132, L"%1", L"%1", L"%1", L"%1", L"%1", L"%1"},
    {133, L"%2. %1. Click for more options.", L"%2. %1. Fai clic per altre opzioni.", L"%2. %1. \u0628\u0631\u0627\u06cc \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%2. %1. Haga clic para ver m\u00e1s opciones.", L"%2. %1. Cliquez pour plus d'options.", L"%2. %1. Clique para mais op\u00e7\u00f5es."},
    {134, L"%3\n%1\nDate, time, and number formatting", L"%3\n%1\nFormattazione di data, ora e numeri", L"%3\n%1\n\u0642\u0627\u0644\u0628\u200c\u0628\u0646\u062f\u06cc \u062a\u0627\u0631\u06cc\u062e\u060c \u0632\u0645\u0627\u0646 \u0648 \u0627\u0639\u062f\u0627\u062f", L"%3\n%1\nFormato de fecha, hora y n\u00famero", L"%3\n%1\nFormat de date, d'heure et de nombre", L"%3\n%1\nFormata\u00e7\u00e3o de data, hora e n\u00famero"},
    {135, L"%2. %3. %1. Used for date, time, and number formatting. Click for more options.", L"%2. %3. %1. Usato per la formattazione di data, ora e numeri. Fai clic per altre opzioni.", L"%2. %3. %1. \u0628\u0631\u0627\u06cc \u0642\u0627\u0644\u0628\u200c\u0628\u0646\u062f\u06cc \u062a\u0627\u0631\u06cc\u062e\u060c \u0632\u0645\u0627\u0646 \u0648 \u0627\u0639\u062f\u0627\u062f \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0645\u06cc\u200c\u0634\u0648\u062f. \u0628\u0631\u0627\u06cc \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%2. %3. %1. Se usa para el formato de fecha, hora y n\u00famero. Haga clic para ver m\u00e1s opciones.", L"%2. %3. %1. Utilis\u00e9 pour le format de date, d'heure et de nombre. Cliquez pour plus d'options.", L"%2. %3. %1. Usado para formata\u00e7\u00e3o de data, hora e n\u00famero. Clique para mais op\u00e7\u00f5es."},
    {136, L"%1\nDate, time, and number formatting", L"%1\nFormattazione di data, ora e numeri", L"%1\n\u0642\u0627\u0644\u0628\u200c\u0628\u0646\u062f\u06cc \u062a\u0627\u0631\u06cc\u062e\u060c \u0632\u0645\u0627\u0646 \u0648 \u0627\u0639\u062f\u0627\u062f", L"%1\nFormato de fecha, hora y n\u00famero", L"%1\nFormat de date, d'heure et de nombre", L"%1\nFormata\u00e7\u00e3o de data, hora e n\u00famero"},
    {137, L"%2. %1. Used for date, time, and number formatting. Click for more options.", L"%2. %1. Usato per la formattazione di data, ora e numeri. Fai clic per altre opzioni.", L"%2. %1. \u0628\u0631\u0627\u06cc \u0642\u0627\u0644\u0628\u200c\u0628\u0646\u062f\u06cc \u062a\u0627\u0631\u06cc\u062e\u060c \u0632\u0645\u0627\u0646 \u0648 \u0627\u0639\u062f\u0627\u062f \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0645\u06cc\u200c\u0634\u0648\u062f. \u0628\u0631\u0627\u06cc \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"%2. %1. Se usa para el formato de fecha, hora y n\u00famero. Haga clic para ver m\u00e1s opciones.", L"%2. %1. Utilis\u00e9 pour le format de date, d'heure et de nombre. Cliquez pour plus d'options.", L"%2. %1. Usado para formata\u00e7\u00e3o de data, hora e n\u00famero. Clique para mais op\u00e7\u00f5es."},
    {138, L"Open overflow menu", L"Apri menu di overflow", L"\u0628\u0627\u0632 \u06a9\u0631\u062f\u0646 \u0645\u0646\u0648\u06cc \u0633\u0631\u0631\u06cc\u0632", L"Abrir men\u00fa de desbordamiento", L"Ouvrir le menu de d\u00e9passement", L"Abrir menu de estouro"},
    {139, L"Unavailable in this edition of Windows", L"Non disponibile in questa edizione di Windows", L"\u062f\u0631 \u0627\u06cc\u0646 \u0646\u0633\u062e\u0647 \u0627\u0632 \u0648\u06cc\u0646\u062f\u0648\u0632 \u062f\u0631 \u062f\u0633\u062a\u0631\u0633 \u0646\u06cc\u0633\u062a", L"No disponible en esta edici\u00f3n de Windows", L"Non disponible dans cette \u00e9dition de Windows", L"Indispon\u00edvel nesta edi\u00e7\u00e3o do Windows"},
    {140, L"Log off later", L"Disconnetti pi\u00f9 tardi", L"\u062e\u0631\u0648\u062c \u0627\u0632 \u0633\u06cc\u0633\u062a\u0645 \u062f\u0631 \u0632\u0645\u0627\u0646 \u062f\u06cc\u06af\u0631", L"Cerrar sesi\u00f3n m\u00e1s tarde", L"Se d\u00e9connecter plus tard", L"Fazer logoff mais tarde"},
    {141, L"Recognize rarely used Chinese, Kanji, or Hanja characters when converting handwriting to typed text.", L"Riconosci caratteri cinesi, Kanji o Hanja usati raramente durante la conversione della calligrafia in testo digitato.", L"\u062a\u0634\u062e\u06cc\u0635 \u0646\u0648\u06cc\u0633\u0647\u200c\u0647\u0627\u06cc \u06a9\u0645\u200c\u06a9\u0627\u0631\u0628\u0631\u062f \u0686\u06cc\u0646\u06cc\u060c \u06a9\u0627\u0646\u062c\u06cc \u06cc\u0627 \u0647\u0627\u0646\u062c\u0627 \u0647\u0646\u06af\u0627\u0645 \u062a\u0628\u062f\u06cc\u0644 \u062f\u0633\u062a\u200c\u062e\u0637 \u0628\u0647 \u0645\u062a\u0646 \u062a\u0627\u06cc\u067e\u200c\u0634\u062f\u0647.", L"Reconocer caracteres chinos, kanji o hanja poco usados al convertir escritura a mano en texto escrito.", L"Reconna\u00eetre les caract\u00e8res chinois, kanji ou hanja rarement utilis\u00e9s lors de la conversion de l'\u00e9criture manuscrite en texte tap\u00e9.", L"Reconhecer caracteres chineses, Kanji ou Hanja raramente usados ao converter escrita \u00e0 m\u00e3o em texto digitado."},
    {142, L"There are no handwriting options for this language", L"Non sono disponibili opzioni per la calligrafia per questa lingua", L"\u06af\u0632\u06cc\u0646\u0647\u200c\u0627\u06cc \u0628\u0631\u0627\u06cc \u062f\u0633\u062a\u200c\u062e\u0637 \u062f\u0631 \u0627\u06cc\u0646 \u0632\u0628\u0627\u0646 \u0648\u062c\u0648\u062f \u0646\u062f\u0627\u0631\u062f", L"No hay opciones de escritura a mano para este idioma", L"Il n'y a pas d'options d'\u00e9criture manuscrite pour cette langue", L"N\u00e3o h\u00e1 op\u00e7\u00f5es de escrita \u00e0 m\u00e3o para este idioma"},
    {594, L"Change your language preferences", L"Modifica le preferenze relative alla lingua", L"\u062a\u063a\u06cc\u06cc\u0631 \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0628\u0631\u06af\u0632\u06cc\u062f\u0647 \u0632\u0628\u0627\u0646 \u0634\u0645\u0627", L"Cambiar las preferencias de idioma", L"Modifier vos pr\u00e9f\u00e9rences linguistiques", L"Alterar suas prefer\u00eancias de idioma"},
    {595, L"You can type in any language you add to the list. Windows, apps and websites will appear in the first language in the list that they support.", L"Puoi digitare in qualsiasi lingua aggiunta all'elenco. Windows, le app e i siti Web verranno visualizzati nella prima lingua dell'elenco supportata.", L"\u0645\u06cc\u200c\u062a\u0648\u0627\u0646\u06cc\u062f \u0628\u0647 \u0647\u0631 \u0632\u0628\u0627\u0646\u06cc \u06a9\u0647 \u0628\u0647 \u0641\u0647\u0631\u0633\u062a \u0627\u0636\u0627\u0641\u0647 \u0645\u06cc\u200c\u06a9\u0646\u06cc\u062f \u062a\u0627\u06cc\u067e \u06a9\u0646\u06cc\u062f. \u0648\u06cc\u0646\u062f\u0648\u0632\u060c \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627 \u0648 \u0648\u0628\u200c\u0633\u0627\u06cc\u062a\u200c\u0647\u0627 \u0628\u0647 \u0627\u0648\u0644\u06cc\u0646 \u0632\u0628\u0627\u0646\u06cc \u06a9\u0647 \u062f\u0631 \u0641\u0647\u0631\u0633\u062a \u067e\u0634\u062a\u06cc\u0628\u0627\u0646\u06cc \u0645\u06cc\u200c\u06a9\u0646\u0646\u062f \u0646\u0645\u0627\u06cc\u0634 \u062f\u0627\u062f\u0647 \u0645\u06cc\u200c\u0634\u0648\u0646\u062f.", L"Puede escribir en cualquier idioma que agregue a la lista. Windows, las aplicaciones y los sitios web aparecer\u00e1n en el primer idioma de la lista que admitan.", L"Vous pouvez taper dans n'importe quelle langue ajout\u00e9e \u00e0 la liste. Windows, les applications et les sites web s'afficheront dans la premi\u00e8re langue prise en charge de la liste.", L"Voc\u00ea pode digitar em qualquer idioma que adicionar \u00e0 lista. O Windows, os aplicativos e os sites aparecer\u00e3o no primeiro idioma da lista que eles suportarem."},
    {600, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {601, L"Add", L"Aggiungi", L"\u0627\u0641\u0632\u0648\u062f\u0646", L"Agregar", L"Ajouter", L"Adicionar"},
    {603, L"Add a language", L"Aggiungi una lingua", L"\u0627\u0641\u0632\u0648\u062f\u0646 \u06cc\u06a9 \u0632\u0628\u0627\u0646", L"Agregar un idioma", L"Ajouter une langue", L"Adicionar um idioma"},
    {608, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {609, L"Save", L"Salva", L"\u0630\u062e\u06cc\u0631\u0647", L"Guardar", L"Enregistrer", L"Salvar"},
    {611, L"Group languages by:", L"Raggruppa lingue per:", L"\u06af\u0631\u0648\u0647\u200c\u0628\u0646\u062f\u06cc \u0632\u0628\u0627\u0646\u200c\u0647\u0627 \u0628\u0631 \u0627\u0633\u0627\u0633:", L"Agrupar idiomas por:", L"Regrouper les langues par :", L"Agrupar idiomas por:"},
    {612, L"Use the search box to find more languages.", L"Usa la casella di ricerca per trovare altre lingue.", L"\u0628\u0631\u0627\u06cc \u06cc\u0627\u0641\u062a\u0646 \u0632\u0628\u0627\u0646\u200c\u0647\u0627\u06cc \u0628\u06cc\u0634\u062a\u0631 \u0627\u0632 \u06a9\u0627\u062f\u0631 \u062c\u0633\u062a\u062c\u0648 \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u06a9\u0646\u06cc\u062f.", L"Use el cuadro de b\u00fasqueda para encontrar m\u00e1s idiomas.", L"Utilisez la zone de recherche pour trouver d'autres langues.", L"Use a caixa de pesquisa para encontrar mais idiomas."},
    {618, L"Input method", L"Metodo di input", L"\u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc", L"M\u00e9todo de entrada", L"M\u00e9thode d'entr\u00e9e", L"M\u00e9todo de entrada"},
    {620, L"Add an input method", L"Aggiungi un metodo di input", L"\u0627\u0641\u0632\u0648\u062f\u0646 \u06cc\u06a9 \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc", L"Agregar un m\u00e9todo de entrada", L"Ajouter une m\u00e9thode d'entr\u00e9e", L"Adicionar um m\u00e9todo de entrada"},
    {621, L"Text services", L"Servizi di testo", L"\u062e\u062f\u0645\u0627\u062a \u0645\u062a\u0646\u06cc", L"Servicios de texto", L"Services de texte", L"Servi\u00e7os de texto"},
    {624, L"Add an input method", L"Aggiungi un metodo di input", L"\u0627\u0641\u0632\u0648\u062f\u0646 \u06cc\u06a9 \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc", L"Agregar un m\u00e9todo de entrada", L"Ajouter une m\u00e9thode d'entr\u00e9e", L"Adicionar um m\u00e9todo de entrada"},
    {626, L"Add", L"Aggiungi", L"\u0627\u0641\u0632\u0648\u062f\u0646", L"Agregar", L"Ajouter", L"Adicionar"},
    {627, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {628, L"Advanced settings", L"Impostazioni avanzate", L"\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u067e\u06cc\u0634\u0631\u0641\u062a\u0647", L"Configuraci\u00f3n avanzada", L"Param\u00e8tres avanc\u00e9s", L"Configura\u00e7\u00f5es avan\u00e7adas"},
    {629, L"Override for Windows display language", L"Sostituzione per la lingua di visualizzazione di Windows", L"\u062c\u0627\u06cc\u06af\u0632\u06cc\u0646 \u0628\u0631\u0627\u06cc \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632", L"Invalidaci\u00f3n del idioma para mostrar de Windows", L"Substitution de la langue d'affichage de Windows", L"Substitui\u00e7\u00e3o do idioma de exibi\u00e7\u00e3o do Windows"},
    {630, L"Override for default input method", L"Sostituzione per il metodo di input predefinito", L"\u062c\u0627\u06cc\u06af\u0632\u06cc\u0646 \u0628\u0631\u0627\u06cc \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc \u067e\u06cc\u0634\u200c\u0641\u0631\u0636", L"Invalidaci\u00f3n del m\u00e9todo de entrada predeterminado", L"Substitution de la m\u00e9thode d'entr\u00e9e par d\u00e9faut", L"Substitui\u00e7\u00e3o do m\u00e9todo de entrada padr\u00e3o"},
    {631, L"Switching input methods", L"Cambio dei metodi di input", L"\u062a\u063a\u06cc\u06cc\u0631 \u062f\u0627\u062f\u0646 \u0631\u0648\u0634\u200c\u0647\u0627\u06cc \u0648\u0631\u0648\u062f\u06cc", L"Cambio de m\u00e9todos de entrada", L"Changement des m\u00e9thodes d'entr\u00e9e", L"Alternando m\u00e9todos de entrada"},
    {632, L"Personalization data", L"Dati di personalizzazione", L"\u062f\u0627\u062f\u0647\u200c\u0647\u0627\u06cc \u0634\u062e\u0635\u06cc\u200c\u0633\u0627\u0632\u06cc", L"Datos de personalizaci\u00f3n", L"Donn\u00e9es de personnalisation", L"Dados de personaliza\u00e7\u00e3o"},
    {633, L"Save", L"Salva", L"\u0630\u062e\u06cc\u0631\u0647", L"Guardar", L"Enregistrer", L"Salvar"},
    {634, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {635, L"If you want to use a display language that's different than the one determined by the order of your language list, choose it here.", L"Se desideri usare una lingua di visualizzazione diversa da quella determinata dall'ordine dell'elenco delle lingue, sceglila qui.", L"\u0627\u06af\u0631 \u0645\u06cc\u200c\u062e\u0648\u0627\u0647\u06cc\u062f \u0627\u0632 \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634\u06cc \u0645\u062a\u0641\u0627\u0648\u062a \u0628\u0627 \u0632\u0628\u0627\u0646\u06cc \u06a9\u0647 \u0628\u0631 \u0627\u0633\u0627\u0633 \u062a\u0631\u062a\u06cc\u0628 \u0641\u0647\u0631\u0633\u062a \u0632\u0628\u0627\u0646 \u0634\u0645\u0627 \u062a\u0639\u06cc\u06cc\u0646 \u0634\u062f\u0647 \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u06a9\u0646\u06cc\u062f\u060c \u0622\u0646 \u0631\u0627 \u062f\u0631 \u0627\u06cc\u0646\u062c\u0627 \u0627\u0646\u062a\u062e\u0627\u0628 \u06a9\u0646\u06cc\u062f.", L"Si desea usar un idioma para mostrar diferente al determinado por el orden de la lista de idiomas, el\u00edjalo aqu\u00ed.", L"Si vous souhaitez utiliser une langue d'affichage diff\u00e9rente de celle d\u00e9termin\u00e9e par l'ordre de votre liste de langues, choisissez-la ici.", L"Se voc\u00ea quiser usar um idioma de exibi\u00e7\u00e3o diferente daquele determinado pela ordem da sua lista de idiomas, escolha-o aqui."},
    {636, L"If you want to use an input method that's different than the first one in your language list, choose it here.", L"Se desideri usare un metodo di input diverso dal primo nell'elenco delle lingue, sceglilo qui.", L"\u0627\u06af\u0631 \u0645\u06cc\u200c\u062e\u0648\u0627\u0647\u06cc\u062f \u0627\u0632 \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc \u0645\u062a\u0641\u0627\u0648\u062a\u06cc \u0646\u0633\u0628\u062a \u0628\u0647 \u0627\u0648\u0644\u06cc\u0646 \u0645\u0648\u0631\u062f \u062f\u0631 \u0641\u0647\u0631\u0633\u062a \u0632\u0628\u0627\u0646 \u062e\u0648\u062f \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u06a9\u0646\u06cc\u062f\u060c \u0622\u0646 \u0631\u0627 \u062f\u0631 \u0627\u06cc\u0646\u062c\u0627 \u0627\u0646\u062a\u062e\u0627\u0628 \u06a9\u0646\u06cc\u062f.", L"Si desea usar un m\u00e9todo de entrada diferente al primero de la lista de idiomas, el\u00edjalo aqu\u00ed.", L"Si vous souhaitez utiliser une m\u00e9thode d'entr\u00e9e diff\u00e9rente de la premi\u00e8re de votre liste de langues, choisissez-la ici.", L"Se voc\u00ea quiser usar um m\u00e9todo de entrada diferente do primeiro da sua lista de idiomas, escolha-o aqui."},
    {637, L"Let me set a different input method for each app window", L"Consenti di impostare un metodo di input diverso per ogni finestra dell'app", L"\u0627\u062c\u0627\u0632\u0647 \u062a\u0646\u0638\u06cc\u0645 \u06cc\u06a9 \u0631\u0648\u0634 \u0648\u0631\u0648\u062f\u06cc \u0645\u062a\u0641\u0627\u0648\u062a \u0628\u0631\u0627\u06cc \u0647\u0631 \u067e\u0646\u062c\u0631\u0647 \u0628\u0631\u0646\u0627\u0645\u0647", L"Permitirme establecer un m\u00e9todo de entrada diferente para cada ventana de aplicaci\u00f3n", L"Me permettre de d\u00e9finir une m\u00e9thode d'entr\u00e9e diff\u00e9rente pour chaque fen\u00eatre d'application", L"Permitir-me definir um m\u00e9todo de entrada diferente para cada janela de aplicativo"},
    {638, L"Use the desktop language bar when it's available", L"Usa la barra della lingua del desktop quando disponibile", L"\u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0627\u0632 \u0646\u0648\u0627\u0631 \u0632\u0628\u0627\u0646 \u062f\u0633\u06a9\u062a\u0627\u067e \u062f\u0631 \u0635\u0648\u0631\u062a \u062f\u0631 \u062f\u0633\u062a\u0631\u0633 \u0628\u0648\u062f\u0646", L"Usar la barra de idioma del escritorio cuando est\u00e9 disponible", L"Utiliser la barre de langue du Bureau lorsqu'elle est disponible", L"Usar a barra de idiomas da \u00e1rea de trabalho quando estiver dispon\u00edvel"},
    {640, L"Use automatic learning (recommended)", L"Usa apprendimento automatico (consigliato)", L"\u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0627\u0632 \u06cc\u0627\u062f\u06af\u06cc\u0631\u06cc \u062e\u0648\u062f\u06a9\u0627\u0631 (\u062a\u0648\u0635\u06cc\u0647\u200c\u0634\u062f\u0647)", L"Usar aprendizaje autom\u00e1tico (recomendado)", L"Utiliser l'apprentissage automatique (recommand\u00e9)", L"Usar aprendizado autom\u00e1tico (recomendado)"},
    {641, L"Don't use automatic learning and delete all previously collected data", L"Non usare l'apprendimento automatico ed elimina tutti i dati raccolti in precedenza", L"\u0639\u062f\u0645 \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0627\u0632 \u06cc\u0627\u062f\u06af\u06cc\u0631\u06cc \u062e\u0648\u062f\u06a9\u0627\u0631 \u0648 \u062d\u0630\u0641 \u062a\u0645\u0627\u0645 \u062f\u0627\u062f\u0647\u200c\u0647\u0627\u06cc \u062c\u0645\u0639\u200c\u0622\u0648\u0631\u06cc\u200c\u0634\u062f\u0647 \u0642\u0628\u0644\u06cc", L"No usar el aprendizaje autom\u00e1tico y eliminar todos los datos recopilados anteriormente", L"Ne pas utiliser l'apprentissage automatique et supprimer toutes les donn\u00e9es collect\u00e9es pr\u00e9c\u00e9demment", L"N\u00e3o usar aprendizado autom\u00e1tico e excluir todos os dados coletados anteriormente"},
    {642, L"This data is only used to improve handwriting recognition and text prediction results for languages without IMEs on this PC. No info is sent to Microsoft. <a>Privacy statement</a>", L"Questi dati vengono usati solo per migliorare il riconoscimento della calligrafia e i risultati di previsione del testo per le lingue senza IME in questo PC. Nessuna informazione viene inviata a Microsoft. <a>Informativa sulla privacy</a>", L"\u0627\u06cc\u0646 \u062f\u0627\u062f\u0647\u200c\u0647\u0627 \u0641\u0642\u0637 \u0628\u0631\u0627\u06cc \u0628\u0647\u0628\u0648\u062f \u062a\u0634\u062e\u06cc\u0635 \u062f\u0633\u062a\u200c\u062e\u0637 \u0648 \u0646\u062a\u0627\u06cc\u062c \u067e\u06cc\u0634\u200c\u0628\u06cc\u0646\u06cc \u0645\u062a\u0646 \u0628\u0631\u0627\u06cc \u0632\u0628\u0627\u0646\u200c\u0647\u0627\u06cc \u0628\u062f\u0648\u0646 IME \u062f\u0631 \u0627\u06cc\u0646 \u0631\u0627\u06cc\u0627\u0646\u0647 \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0645\u06cc\u200c\u0634\u0648\u0646\u062f. \u0647\u06cc\u0686 \u0627\u0637\u0644\u0627\u0639\u0627\u062a\u06cc \u0628\u0647 \u0645\u0627\u06cc\u06a9\u0631\u0648\u0633\u0627\u0641\u062a \u0627\u0631\u0633\u0627\u0644 \u0646\u0645\u06cc\u200c\u0634\u0648\u062f. <a>\u0628\u06cc\u0627\u0646\u06cc\u0647 \u062d\u0631\u06cc\u0645 \u062e\u0635\u0648\u0635\u06cc</a>", L"Estos datos solo se usan para mejorar el reconocimiento de escritura a mano y los resultados de predicci\u00f3n de texto para idiomas sin IME en este equipo. No se env\u00eda informaci\u00f3n a Microsoft. <a>Declaraci\u00f3n de privacidad</a>", L"Ces donn\u00e9es sont uniquement utilis\u00e9es pour am\u00e9liorer la reconnaissance de l'\u00e9criture manuscrite et la pr\u00e9diction de texte pour les langues sans IME sur ce PC. Aucune info n'est envoy\u00e9e \u00e0 Microsoft. <a>D\u00e9claration de confidentialit\u00e9</a>", L"Esses dados s\u00e3o usados apenas para melhorar o reconhecimento de escrita \u00e0 m\u00e3o e os resultados de previs\u00e3o de texto para idiomas sem IMEs neste PC. Nenhuma informa\u00e7\u00e3o \u00e9 enviada \u00e0 Microsoft. <a>Pol\u00edtica de privacidade</a>"},
    {645, L"Options", L"Opzioni", L"\u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627", L"Opciones", L"Options", L"Op\u00e7\u00f5es"},
    {646, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {647, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {648, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {650, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {651, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {652, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {653, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {654, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {655, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {656, L"Handwriting", L"Calligrafia", L"\u062f\u0633\u062a\u200c\u062e\u0637", L"Escritura a mano", L"\u00c9criture manuscrite", L"Escrita \u00e0 m\u00e3o"},
    {661, L"Windows display language", L"Lingua di visualizzazione di Windows", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632", L"Idioma para mostrar de Windows", L"Langue d'affichage de Windows", L"Idioma de exibi\u00e7\u00e3o do Windows"},
    {673, L"Personalize handwriting recognition", L"Personalizza riconoscimento calligrafia", L"\u0634\u062e\u0635\u06cc\u200c\u0633\u0627\u0632\u06cc \u062a\u0634\u062e\u06cc\u0635 \u062f\u0633\u062a\u200c\u062e\u0637", L"Personalizar el reconocimiento de escritura a mano", L"Personnaliser la reconnaissance de l'\u00e9criture manuscrite", L"Personalizar o reconhecimento de escrita \u00e0 m\u00e3o"},
    {674, L"Personalize handwriting recognition.", L"Personalizza il riconoscimento della calligrafia.", L"\u0634\u062e\u0635\u06cc\u200c\u0633\u0627\u0632\u06cc \u062a\u0634\u062e\u06cc\u0635 \u062f\u0633\u062a\u200c\u062e\u0637.", L"Personalizar el reconocimiento de escritura a mano.", L"Personnaliser la reconnaissance de l'\u00e9criture manuscrite.", L"Personalizar o reconhecimento de escrita \u00e0 m\u00e3o."},
    {677, L"Change language bar hot keys", L"Modifica tasti di scelta rapida della barra della lingua", L"\u062a\u063a\u06cc\u06cc\u0631 \u06a9\u0644\u06cc\u062f\u0647\u0627\u06cc \u0645\u06cc\u0627\u0646\u0628\u0631 \u0646\u0648\u0627\u0631 \u0632\u0628\u0627\u0646", L"Cambiar teclas de acceso r\u00e1pido de la barra de idioma", L"Modifier les touches d'acc\u00e8s rapide de la barre de langue", L"Alterar teclas de atalho da barra de idiomas"},
    {678, L"click", L"clic", L"\u06a9\u0644\u06cc\u06a9", L"clic", L"clic", L"clique"},
    {679, L"Spellchecking engine to use: ", L"Motore di controllo ortografico da usare: ", L"\u0645\u0648\u062a\u0648\u0631 \u063a\u0644\u0637\u200c\u06af\u06cc\u0631 \u0627\u0645\u0644\u0627\u06cc\u06cc \u0645\u0648\u0631\u062f \u0627\u0633\u062a\u0641\u0627\u062f\u0647: ", L"Motor de revisi\u00f3n ortogr\u00e1fica para usar: ", L"Moteur de v\u00e9rification orthographique \u00e0 utiliser : ", L"Mecanismo de verifica\u00e7\u00e3o ortogr\u00e1fica a ser usado: "},
    {680, L"Language for web content", L"Lingua per il contenuto Web", L"\u0632\u0628\u0627\u0646 \u0628\u0631\u0627\u06cc \u0645\u062d\u062a\u0648\u0627\u06cc \u0648\u0628", L"Idioma para el contenido web", L"Langue du contenu web", L"Idioma para conte\u00fado da Web"},
    {681, L"Don't let websites access my language list. The language of my date, time, and number formatting will be used instead.", L"Non consentire ai siti Web di accedere all'elenco delle lingue. Verr\u00e0 invece usata la lingua di formattazione di data, ora e numeri.", L"\u0628\u0647 \u0648\u0628\u200c\u0633\u0627\u06cc\u062a\u200c\u0647\u0627 \u0627\u062c\u0627\u0632\u0647 \u062f\u0633\u062a\u0631\u0633\u06cc \u0628\u0647 \u0641\u0647\u0631\u0633\u062a \u0632\u0628\u0627\u0646 \u0645\u0646 \u062f\u0627\u062f\u0647 \u0646\u0634\u0648\u062f. \u062f\u0631 \u0639\u0648\u0636 \u0627\u0632 \u0632\u0628\u0627\u0646 \u0642\u0627\u0644\u0628\u200c\u0628\u0646\u062f\u06cc \u062a\u0627\u0631\u06cc\u062e\u060c \u0632\u0645\u0627\u0646 \u0648 \u0627\u0639\u062f\u0627\u062f \u0645\u0646 \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u062e\u0648\u0627\u0647\u062f \u0634\u062f.", L"No permitir que los sitios web accedan a mi lista de idiomas. En su lugar, se usar\u00e1 el idioma de mi formato de fecha, hora y n\u00famero.", L"Ne pas autoriser les sites web \u00e0 acc\u00e9der \u00e0 ma liste de langues. La langue de mise en forme de la date, de l'heure et des nombres sera utilis\u00e9e \u00e0 la place.", L"N\u00e3o permitir que os sites acessem minha lista de idiomas. O idioma da minha formata\u00e7\u00e3o de data, hora e n\u00famero ser\u00e1 usado em vez disso."},
    {0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
};

// Complete intl.cpl.mui string table (all 69 strings, IDs 1..902)
static const MuiStringTable kIntlMuiStrings[] = {
    {1, L"Region", L"Area geografica", L"\u0645\u0646\u0637\u0642\u0647", L"Regi\u00f3n", L"R\u00e9gion", L"Regi\u00e3o"},
    {2, L"Customize settings for the display of languages, numbers, times, and dates.", L"Personalizza le impostazioni per la visualizzazione di lingue, numeri, ore e date.", L"\u0633\u0641\u0627\u0631\u0634\u06cc\u200c\u0633\u0627\u0632\u06cc \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0628\u0631\u0627\u06cc \u0646\u0645\u0627\u06cc\u0634 \u0632\u0628\u0627\u0646\u200c\u0647\u0627\u060c \u0627\u0639\u062f\u0627\u062f\u060c \u0632\u0645\u0627\u0646\u200c\u0647\u0627 \u0648 \u062a\u0627\u0631\u06cc\u062e\u200c\u0647\u0627.", L"Personalice la configuraci\u00f3n para mostrar idiomas, n\u00fameros, horas y fechas.", L"Personnaliser les param\u00e8tres d'affichage des langues, des nombres, des heures et des dates.", L"Personalize as configura\u00e7\u00f5es de exibi\u00e7\u00e3o de idiomas, n\u00fameros, horas e datas."},
    {3, L"Region", L"Area geografica", L"\u0645\u0646\u0637\u0642\u0647", L"Regi\u00f3n", L"R\u00e9gion", L"Regi\u00e3o"},
    {4, L"Customize Format", L"Personalizza formato", L"\u0633\u0641\u0627\u0631\u0634\u06cc\u200c\u0633\u0627\u0632\u06cc \u0642\u0627\u0644\u0628", L"Personalizar formato", L"Personnaliser le format", L"Personalizar Formato"},
    {5, L"One or more of your regional settings are invalid. To fix this problem review and correct the customizable settings.", L"Una o pi\u00f9 impostazioni internazionali non sono valide. Per risolvere il problema, rivedi e correggi le impostazioni personalizzabili.", L"\u06cc\u06a9 \u06cc\u0627 \u0686\u0646\u062f \u0645\u0648\u0631\u062f \u0627\u0632 \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0645\u0646\u0637\u0642\u0647\u200c\u0627\u06cc \u0634\u0645\u0627 \u0646\u0627\u0645\u0639\u062a\u0628\u0631 \u0627\u0633\u062a. \u0628\u0631\u0627\u06cc \u0631\u0641\u0639 \u0627\u06cc\u0646 \u0645\u0634\u06a9\u0644\u060c \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0642\u0627\u0628\u0644 \u0633\u0641\u0627\u0631\u0634\u06cc\u200c\u0633\u0627\u0632\u06cc \u0631\u0627 \u0628\u0631\u0631\u0633\u06cc \u0648 \u0627\u0635\u0644\u0627\u062d \u06a9\u0646\u06cc\u062f.", L"Una o m\u00e1s de sus configuraciones regionales no son v\u00e1lidas. Para solucionar este problema, revise y corrija la configuraci\u00f3n personalizable.", L"Un ou plusieurs de vos param\u00e8tres r\u00e9gionaux ne sont pas valides. Pour r\u00e9soudre ce probl\u00e8me, v\u00e9rifiez et corrigez les param\u00e8tres personnalisables.", L"Uma ou mais das suas configura\u00e7\u00f5es regionais s\u00e3o inv\u00e1lidas. Para corrigir esse problema, revise e corrija as configura\u00e7\u00f5es personaliz\u00e1veis."},
    {8, L".7", L",7", L".7", L",7", L",7", L",7"},
    {9, L"0.7", L"0,7", L"0.7", L"0,7", L"0,7", L"0,7"},
    {10, L"Metric", L"Metrico", L"\u0645\u062a\u0631\u06cc\u06a9", L"M\u00e9trico", L"M\u00e9trique", L"M\u00e9trico"},
    {11, L"U.S.", L"Stati Uniti", L"\u0627\u06cc\u0627\u0644\u0627\u062a \u0645\u062a\u062d\u062f\u0647", L"EE. UU.", L"\u00c9tats-Unis", L"EUA"},
    {12, L"One or more of the characters you entered in this field are invalid. Try using different characters.", L"Uno o pi\u00f9 caratteri immessi in questo campo non sono validi. Prova a usare caratteri diversi.", L"\u06cc\u06a9 \u06cc\u0627 \u0686\u0646\u062f \u0646\u0648\u06cc\u0633\u0647 \u0648\u0627\u0631\u062f\u0634\u062f\u0647 \u062f\u0631 \u0627\u06cc\u0646 \u0641\u06cc\u0644\u062f \u0646\u0627\u0645\u0639\u062a\u0628\u0631 \u0627\u0633\u062a. \u0646\u0648\u06cc\u0633\u0647\u200c\u0647\u0627\u06cc \u062f\u06cc\u06af\u0631\u06cc \u0631\u0627 \u0627\u0645\u062a\u062d\u0627\u0646 \u06a9\u0646\u06cc\u062f.", L"Uno o m\u00e1s de los caracteres especificados en este campo no son v\u00e1lidos. Intente usar caracteres diferentes.", L"Un ou plusieurs des caract\u00e8res saisis dans ce champ ne sont pas valides. Essayez d'utiliser d'autres caract\u00e8res.", L"Um ou mais dos caracteres inseridos neste campo s\u00e3o inv\u00e1lidos. Tente usar caracteres diferentes."},
    {13, L"One or more of the characters you entered for %s are invalid. Try using a different character or enter a blank space.", L"Uno o pi\u00f9 caratteri immessi per %s non sono validi. Prova a usare un carattere diverso o immetti uno spazio vuoto.", L"\u06cc\u06a9 \u06cc\u0627 \u0686\u0646\u062f \u0646\u0648\u06cc\u0633\u0647 \u0648\u0627\u0631\u062f\u0634\u062f\u0647 \u0628\u0631\u0627\u06cc %s \u0646\u0627\u0645\u0639\u062a\u0628\u0631 \u0627\u0633\u062a. \u0646\u0648\u06cc\u0633\u0647 \u062f\u06cc\u06af\u0631\u06cc \u0631\u0627 \u0627\u0645\u062a\u062d\u0627\u0646 \u06a9\u0646\u06cc\u062f \u06cc\u0627 \u06cc\u06a9 \u0641\u0627\u0635\u0644\u0647 \u062e\u0627\u0644\u06cc \u0648\u0627\u0631\u062f \u06a9\u0646\u06cc\u062f.", L"Uno o m\u00e1s de los caracteres especificados para %s no son v\u00e1lidos. Intente usar un car\u00e1cter diferente o escriba un espacio en blanco.", L"Un ou plusieurs des caract\u00e8res saisis pour %s ne sont pas valides. Essayez d'utiliser un autre caract\u00e8re ou entrez un espace.", L"Um ou mais dos caracteres inseridos para %s s\u00e3o inv\u00e1lidos. Tente usar um caractere diferente ou insira um espa\u00e7o em branco."},
    {14, L"Decimal Symbol", L"Separatore decimale", L"\u0646\u0645\u0627\u062f \u0627\u0639\u0634\u0627\u0631", L"S\u00edmbolo decimal", L"Symbole d\u00e9cimal", L"S\u00edmbolo decimal"},
    {15, L"Negative Sign", L"Segno negativo", L"\u0639\u0644\u0627\u0645\u062a \u0645\u0646\u0641\u06cc", L"Signo negativo", L"Signe n\u00e9gatif", L"Sinal negativo"},
    {16, L"Grouping Symbol", L"Simbolo raggruppamento cifre", L"\u0646\u0645\u0627\u062f \u06af\u0631\u0648\u0647\u200c\u0628\u0646\u062f\u06cc \u0627\u0631\u0642\u0627\u0645", L"S\u00edmbolo de separaci\u00f3n de miles", L"Symbole de groupement", L"S\u00edmbolo de agrupamento"},
    {18, L"AM Symbol", L"Simbolo AM", L"\u0646\u0645\u0627\u062f \u0642.\u0638", L"S\u00edmbolo a. m.", L"Symbole AM", L"S\u00edmbolo AM"},
    {19, L"PM Symbol", L"Simbolo PM", L"\u0646\u0645\u0627\u062f \u0628.\u0638", L"S\u00edmbolo p. m.", L"Symbole PM", L"S\u00edmbolo PM"},
    {21, L"Currency Symbol", L"Simbolo valuta", L"\u0646\u0645\u0627\u062f \u0648\u0627\u062d\u062f \u067e\u0648\u0644", L"S\u00edmbolo de moneda", L"Symbole mon\u00e9taire", L"S\u00edmbolo da moeda"},
    {22, L"Currency Decimal Symbol", L"Separatore decimale valuta", L"\u0646\u0645\u0627\u062f \u0627\u0639\u0634\u0627\u0631 \u0648\u0627\u062d\u062f \u067e\u0648\u0644", L"S\u00edmbolo decimal de moneda", L"Symbole d\u00e9cimal mon\u00e9taire", L"S\u00edmbolo decimal da moeda"},
    {23, L"Currency Grouping Symbol", L"Simbolo raggruppamento valuta", L"\u0646\u0645\u0627\u062f \u06af\u0631\u0648\u0647\u200c\u0628\u0646\u062f\u06cc \u0648\u0627\u062d\u062f \u067e\u0648\u0644", L"S\u00edmbolo de separaci\u00f3n de miles de moneda", L"Symbole de groupement mon\u00e9taire", L"S\u00edmbolo de agrupamento da moeda"},
    {24, L"One or more of the characters you entered for the %s format are invalid. Try using different characters.", L"Uno o pi\u00f9 caratteri immessi per il formato %s non sono validi. Prova a usare caratteri diversi.", L"\u06cc\u06a9 \u06cc\u0627 \u0686\u0646\u062f \u0646\u0648\u06cc\u0633\u0647 \u0648\u0627\u0631\u062f\u0634\u062f\u0647 \u0628\u0631\u0627\u06cc \u0642\u0627\u0644\u0628 %s \u0646\u0627\u0645\u0639\u062a\u0628\u0631 \u0627\u0633\u062a. \u0646\u0648\u06cc\u0633\u0647\u200c\u0647\u0627\u06cc \u062f\u06cc\u06af\u0631\u06cc \u0631\u0627 \u0627\u0645\u062a\u062d\u0627\u0646 \u06a9\u0646\u06cc\u062f.", L"Uno o m\u00e1s de los caracteres especificados para el formato %s no son v\u00e1lidos. Intente usar caracteres diferentes.", L"Un ou plusieurs des caract\u00e8res saisis pour le format %s ne sont pas valides. Essayez d'utiliser d'autres caract\u00e8res.", L"Um ou mais dos caracteres inseridos para o formato %s s\u00e3o inv\u00e1lidos. Tente usar caracteres diferentes."},
    {25, L"Long Time", L"Ora estesa", L"\u0632\u0645\u0627\u0646 \u0637\u0648\u0644\u0627\u0646\u06cc", L"Hora larga", L"Heure compl\u00e8te", L"Hora por extenso"},
    {26, L"Short Date", L"Data breve", L"\u062a\u0627\u0631\u06cc\u062e \u06a9\u0648\u062a\u0627\u0647", L"Fecha corta", L"Date courte", L"Data abreviada"},
    {27, L"Long Date", L"Data estesa", L"\u062a\u0627\u0631\u06cc\u062e \u0637\u0648\u0644\u0627\u0646\u06cc", L"Fecha larga", L"Date compl\u00e8te", L"Data por extenso"},
    {28, L"The value in this field must be a number between 99 and 9999. Try using a different number.", L"Il valore in questo campo deve essere un numero compreso tra 99 e 9999. Prova a usare un numero diverso.", L"\u0645\u0642\u062f\u0627\u0631 \u0627\u06cc\u0646 \u0641\u06cc\u0644\u062f \u0628\u0627\u06cc\u062f \u0639\u062f\u062f\u06cc \u0628\u06cc\u0646 \u06f9\u06f9 \u062a\u0627 \u06f9\u06f9\u06f9\u06f9 \u0628\u0627\u0634\u062f. \u0639\u062f\u062f \u062f\u06cc\u06af\u0631\u06cc \u0631\u0627 \u0627\u0645\u062a\u062d\u0627\u0646 \u06a9\u0646\u06cc\u062f.", L"El valor de este campo debe ser un n\u00famero entre 99 y 9999. Intente usar un n\u00famero diferente.", L"La valeur de ce champ doit \u00eatre un nombre compris entre 99 et 9999. Essayez d'utiliser un autre nombre.", L"O valor neste campo deve ser um n\u00famero entre 99 e 9999. Tente usar um n\u00famero diferente."},
    {29, L"Short Time", L"Ora breve", L"\u0632\u0645\u0627\u0646 \u06a9\u0648\u062a\u0627\u0647", L"Hora corta", L"Heure courte", L"Hora abreviada"},
    {30, L"&Format:", L"&Formato:", L"&\u0642\u0627\u0644\u0628:", L"&Formato:", L"&Format :", L"&Formato:"},
    {31, L"&Format: (* Custom Locale)", L"&Formato: (* Impostazioni locali personalizzate)", L"&\u0642\u0627\u0644\u0628: (* \u0645\u062d\u0644\u06cc \u0633\u0641\u0627\u0631\u0634\u06cc)", L"&Formato: (* Configuraci\u00f3n regional personalizada)", L"&Format : (* Param\u00e8tres r\u00e9gionaux personnalis\u00e9s)", L"&Formato: (* Localidade personalizada)"},
    {32, L"Match Windows display language (recommended)", L"Come la lingua di visualizzazione di Windows (consigliato)", L"\u0645\u0637\u0627\u0628\u0642 \u0628\u0627 \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0648\u06cc\u0646\u062f\u0648\u0632 (\u062a\u0648\u0635\u06cc\u0647\u200c\u0634\u062f\u0647)", L"Coincidir con el idioma para mostrar de Windows (recomendado)", L"Identique \u00e0 la langue d'affichage de Windows (recommand\u00e9)", L"Coincidir com o idioma de exibi\u00e7\u00e3o do Windows (recomendado)"},
    {33, L"&Format: %1", L"&Formato: %1", L"&\u0642\u0627\u0644\u0628: %1", L"&Formato: %1", L"&Format : %1", L"&Formato: %1"},
    {55, L"H", L"H", L"H", L"H", L"H", L"H"},
    {56, L"h", L"h", L"h", L"h", L"h", L"h"},
    {57, L"M", L"M", L"M", L"M", L"M", L"M"},
    {58, L"m", L"m", L"m", L"m", L"m", L"m"},
    {59, L"s", L"s", L"s", L"s", L"s", L"s"},
    {60, L"t", L"t", L"t", L"t", L"t", L"t"},
    {61, L"d", L"g", L"d", L"d", L"j", L"d"},
    {62, L"y", L"a", L"y", L"a", L"a", L"a"},
    {69, L"System locale has been changed. You must restart Windows for the changes to take effect.", L"Le impostazioni locali del sistema sono state modificate. \u00c8 necessario riavviare Windows per rendere effettive le modifiche.", L"\u0632\u0628\u0627\u0646 \u0645\u062d\u0644\u06cc \u0633\u06cc\u0633\u062a\u0645 \u062a\u063a\u06cc\u06cc\u0631 \u06a9\u0631\u062f\u0647 \u0627\u0633\u062a. \u0628\u0631\u0627\u06cc \u0627\u0639\u0645\u0627\u0644 \u062a\u063a\u06cc\u06cc\u0631\u0627\u062a \u0628\u0627\u06cc\u062f \u0648\u06cc\u0646\u062f\u0648\u0632 \u0631\u0627 \u0645\u062c\u062f\u062f\u0627\u064b \u0631\u0627\u0647\u200c\u0627\u0646\u062f\u0627\u0632\u06cc \u06a9\u0646\u06cc\u062f.", L"Se ha cambiado la configuraci\u00f3n regional del sistema. Debe reiniciar Windows para que los cambios surtan efecto.", L"Les param\u00e8tres r\u00e9gionaux du syst\u00e8me ont \u00e9t\u00e9 modifi\u00e9s. Vous devez red\u00e9marrer Windows pour que les modifications prennent effet.", L"A localidade do sistema foi alterada. Voc\u00ea deve reiniciar o Windows para que as altera\u00e7\u00f5es entrem em vigor."},
    {70, L"Change Regional Options", L"Modifica opzioni internazionali", L"\u062a\u063a\u06cc\u06cc\u0631 \u06af\u0632\u06cc\u0646\u0647\u200c\u0647\u0627\u06cc \u0645\u0646\u0637\u0642\u0647\u200c\u0627\u06cc", L"Cambiar opciones regionales", L"Modifier les options r\u00e9gionales", L"Alterar op\u00e7\u00f5es regionais"},
    {71, L"Setup was unable to install the chosen locale.  Please contact your system Administrator.", L"Impossibile installare le impostazioni locali scelte. Contattare l'amministratore di sistema.", L"\u0646\u0635\u0628 \u0632\u0628\u0627\u0646 \u0645\u062d\u0644\u06cc \u0627\u0646\u062a\u062e\u0627\u0628\u200c\u0634\u062f\u0647 \u0627\u0645\u06a9\u0627\u0646\u200c\u067e\u0630\u06cc\u0631 \u0646\u0628\u0648\u062f. \u0644\u0637\u0641\u0627\u064b \u0628\u0627 \u0645\u062f\u06cc\u0631 \u0633\u06cc\u0633\u062a\u0645 \u062e\u0648\u062f \u062a\u0645\u0627\u0633 \u0628\u06af\u06cc\u0631\u06cc\u062f.", L"El programa de instalaci\u00f3n no pudo instalar la configuraci\u00f3n regional elegida. P\u00f3ngase en contacto con el administrador del sistema.", L"Le programme d'installation n'a pas pu installer les param\u00e8tres r\u00e9gionaux choisis. Contactez votre administrateur syst\u00e8me.", L"A Instala\u00e7\u00e3o n\u00e3o p\u00f4de instalar a localidade escolhida. Contate o Administrador do sistema."},
    {72, L"System display language has been changed. You must restart Windows for the changes to take effect.", L"La lingua di visualizzazione del sistema \u00e8 stata modificata. \u00c8 necessario riavviare Windows per rendere effettive le modifiche.", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0633\u06cc\u0633\u062a\u0645 \u062a\u063a\u06cc\u06cc\u0631 \u06a9\u0631\u062f\u0647 \u0627\u0633\u062a. \u0628\u0631\u0627\u06cc \u0627\u0639\u0645\u0627\u0644 \u062a\u063a\u06cc\u06cc\u0631\u0627\u062a \u0628\u0627\u06cc\u062f \u0648\u06cc\u0646\u062f\u0648\u0632 \u0631\u0627 \u0645\u062c\u062f\u062f\u0627\u064b \u0631\u0627\u0647\u200c\u0627\u0646\u062f\u0627\u0632\u06cc \u06a9\u0646\u06cc\u062f.", L"Se ha cambiado el idioma para mostrar del sistema. Debe reiniciar Windows para que los cambios surtan efecto.", L"La langue d'affichage du syst\u00e8me a \u00e9t\u00e9 modifi\u00e9e. Vous devez red\u00e9marrer Windows pour que les modifications prennent effet.", L"O idioma de exibi\u00e7\u00e3o do sistema foi alterado. Voc\u00ea deve reiniciar o Windows para que as altera\u00e7\u00f5es entrem em vigor."},
    {77, L"Remove all customizations for the current format?", L"Rimuovere tutte le personalizzazioni per il formato corrente?", L"\u0622\u06cc\u0627 \u0647\u0645\u0647 \u0633\u0641\u0627\u0631\u0634\u06cc\u200c\u0633\u0627\u0632\u06cc\u200c\u0647\u0627 \u0628\u0631\u0627\u06cc \u0642\u0627\u0644\u0628 \u0641\u0639\u0644\u06cc \u062d\u0630\u0641 \u0634\u0648\u0646\u062f\u061f", L"\u00bfDesea quitar todas las personalizaciones del formato actual?", L"Supprimer toutes les personnalisations du format actuel ?", L"Remover todas as personaliza\u00e7\u00f5es do formato atual?"},
    {78, L"Would you like to apply your region and language changes?", L"Applicare le modifiche relative ad area geografica e lingua?", L"\u0622\u06cc\u0627 \u0645\u06cc\u200c\u062e\u0648\u0627\u0647\u06cc\u062f \u062a\u063a\u06cc\u06cc\u0631\u0627\u062a \u0645\u0646\u0637\u0642\u0647 \u0648 \u0632\u0628\u0627\u0646 \u062e\u0648\u062f \u0631\u0627 \u0627\u0639\u0645\u0627\u0644 \u06a9\u0646\u06cc\u062f\u061f", L"\u00bfDesea aplicar los cambios de regi\u00f3n e idioma?", L"Voulez-vous appliquer vos modifications de r\u00e9gion et de langue ?", L"Deseja aplicar as altera\u00e7\u00f5es de regi\u00e3o e idioma?"},
    {79, L"Restart now", L"Riavvia ora", L"\u0631\u0627\u0647\u200c\u0627\u0646\u062f\u0627\u0632\u06cc \u0645\u062c\u062f\u062f \u0627\u06a9\u0646\u0648\u0646", L"Reiniciar ahora", L"Red\u00e9marrer maintenant", L"Reiniciar agora"},
    {80, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {81, L"Make sure you save your work and close all open programs before restarting.", L"Assicurati di salvare il lavoro e chiudere tutti i programmi aperti prima di riavviare.", L"\u067e\u06cc\u0634 \u0627\u0632 \u0631\u0627\u0647\u200c\u0627\u0646\u062f\u0627\u0632\u06cc \u0645\u062c\u062f\u062f\u060c \u0645\u0637\u0645\u0626\u0646 \u0634\u0648\u06cc\u062f \u06a9\u0627\u0631 \u062e\u0648\u062f \u0631\u0627 \u0630\u062e\u06cc\u0631\u0647 \u06a9\u0631\u062f\u0647 \u0648 \u0647\u0645\u0647 \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627\u06cc \u0628\u0627\u0632 \u0631\u0627 \u0628\u0633\u062a\u0647\u200c\u0627\u06cc\u062f.", L"Aseg\u00farese de guardar su trabajo y cerrar todos los programas abiertos antes de reiniciar.", L"Assurez-vous d'enregistrer votre travail et de fermer tous les programmes ouverts avant de red\u00e9marrer.", L"Certifique-se de salvar seu trabalho e fechar todos os programas abertos antes de reiniciar."},
    {82, L"Change System Locale", L"Modifica impostazioni locali del sistema", L"\u062a\u063a\u06cc\u06cc\u0631 \u0632\u0628\u0627\u0646 \u0645\u062d\u0644\u06cc \u0633\u06cc\u0633\u062a\u0645", L"Cambiar configuraci\u00f3n regional del sistema", L"Modifier les param\u00e8tres r\u00e9gionaux du syst\u00e8me", L"Alterar localidade do sistema"},
    {83, L"Windows could not properly load the %s keyboard layout.", L"Impossibile caricare correttamente il layout di tastiera %s.", L"\u0648\u06cc\u0646\u062f\u0648\u0632 \u0646\u062a\u0648\u0627\u0646\u0633\u062a \u0686\u06cc\u062f\u0645\u0627\u0646 \u0635\u0641\u062d\u0647\u200c\u06a9\u0644\u06cc\u062f %s \u0631\u0627 \u0628\u0647 \u062f\u0631\u0633\u062a\u06cc \u0628\u0627\u0631\u06af\u0630\u0627\u0631\u06cc \u06a9\u0646\u062f.", L"Windows no pudo cargar correctamente la distribuci\u00f3n de teclado %s.", L"Windows n'a pas pu charger correctement la disposition de clavier %s.", L"O Windows n\u00e3o p\u00f4de carregar corretamente o layout de teclado %s."},
    {96, L"Spanish (Spain)", L"Spagnolo (Spagna)", L"\u0627\u0633\u067e\u0627\u0646\u06cc\u0627\u06cc\u06cc (\u0627\u0633\u067e\u0627\u0646\u06cc\u0627)", L"Espa\u00f1ol (Espa\u00f1a)", L"Espagnol (Espagne)", L"Espanhol (Espanha)"},
    {100, L"You must log off for display language changes to take effect", L"\u00c8 necessario disconnettersi per rendere effettive le modifiche alla lingua di visualizzazione", L"\u0628\u0631\u0627\u06cc \u0627\u0639\u0645\u0627\u0644 \u062a\u063a\u06cc\u06cc\u0631\u0627\u062a \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0628\u0627\u06cc\u062f \u0627\u0632 \u0633\u06cc\u0633\u062a\u0645 \u062e\u0627\u0631\u062c \u0634\u0648\u06cc\u062f", L"Debe cerrar la sesi\u00f3n para que los cambios de idioma para mostrar surtan efecto", L"Vous devez vous d\u00e9connecter pour que les modifications de la langue d'affichage prennent effet", L"Voc\u00ea deve fazer logoff para que as altera\u00e7\u00f5es no idioma de exibi\u00e7\u00e3o entrem em vigor"},
    {201, L"Make sure you save your work and close all open programs before you log off.", L"Assicurati di salvare il lavoro e chiudere tutti i programmi aperti prima di disconnetterti.", L"\u067e\u06cc\u0634 \u0627\u0632 \u062e\u0631\u0648\u062c \u0627\u0632 \u0633\u06cc\u0633\u062a\u0645\u060c \u0645\u0637\u0645\u0626\u0646 \u0634\u0648\u06cc\u062f \u06a9\u0627\u0631 \u062e\u0648\u062f \u0631\u0627 \u0630\u062e\u06cc\u0631\u0647 \u06a9\u0631\u062f\u0647 \u0648 \u0647\u0645\u0647 \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627\u06cc \u0628\u0627\u0632 \u0631\u0627 \u0628\u0633\u062a\u0647\u200c\u0627\u06cc\u062f.", L"Aseg\u00farese de guardar su trabajo y cerrar todos los programas abiertos antes de cerrar la sesi\u00f3n.", L"Assurez-vous d'enregistrer votre travail et de fermer tous les programmes ouverts avant de vous d\u00e9connecter.", L"Certifique-se de salvar seu trabalho e fechar todos os programas abertos antes de fazer logoff."},
    {202, L"Log off now", L"Disconnetti ora", L"\u062e\u0631\u0648\u062c \u0627\u0632 \u0633\u06cc\u0633\u062a\u0645 \u0627\u06a9\u0646\u0648\u0646", L"Cerrar sesi\u00f3n ahora", L"Se d\u00e9connecter maintenant", L"Fazer logoff agora"},
    {203, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {204, L"Change Display Language", L"Modifica lingua di visualizzazione", L"\u062a\u063a\u06cc\u06cc\u0631 \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634", L"Cambiar idioma para mostrar", L"Modifier la langue d'affichage", L"Alterar idioma de exibi\u00e7\u00e3o"},
    {205, L"To ensure that the computer reflects these changes we recommend that you apply them before making any further system changes.", L"Per assicurarsi che il computer rifletta queste modifiche, si consiglia di applicarle prima di apportare ulteriori modifiche al sistema.", L"\u0628\u0631\u0627\u06cc \u0627\u0637\u0645\u06cc\u0646\u0627\u0646 \u0627\u0632 \u0627\u0639\u0645\u0627\u0644 \u0627\u06cc\u0646 \u062a\u063a\u06cc\u06cc\u0631\u0627\u062a \u062f\u0631 \u0631\u0627\u06cc\u0627\u0646\u0647\u060c \u062a\u0648\u0635\u06cc\u0647 \u0645\u06cc\u200c\u06a9\u0646\u06cc\u0645 \u067e\u06cc\u0634 \u0627\u0632 \u0627\u0646\u062c\u0627\u0645 \u0647\u0631\u06af\u0648\u0646\u0647 \u062a\u063a\u06cc\u06cc\u0631 \u062f\u06cc\u06af\u0631 \u062f\u0631 \u0633\u06cc\u0633\u062a\u0645 \u0622\u0646\u200c\u0647\u0627 \u0631\u0627 \u0627\u0639\u0645\u0627\u0644 \u06a9\u0646\u06cc\u062f.", L"Para asegurarse de que el equipo refleje estos cambios, le recomendamos que los aplique antes de realizar m\u00e1s cambios en el sistema.", L"Pour vous assurer que l'ordinateur prend en compte ces modifications, nous vous recommandons de les appliquer avant d'effectuer d'autres modifications syst\u00e8me.", L"Para garantir que o computador reflita essas altera\u00e7\u00f5es, recomendamos que voc\u00ea as aplique antes de fazer outras altera\u00e7\u00f5es no sistema."},
    {206, L"Apply", L"Applica", L"\u0627\u0639\u0645\u0627\u0644", L"Aplicar", L"Appliquer", L"Aplicar"},
    {207, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {208, L"The task cannot be completed", L"Impossibile completare l'attivit\u00e0", L"\u0627\u0646\u062c\u0627\u0645 \u0627\u06cc\u0646 \u06a9\u0627\u0631 \u0627\u0645\u06a9\u0627\u0646\u200c\u067e\u0630\u06cc\u0631 \u0646\u06cc\u0633\u062a", L"No se puede completar la tarea", L"La t\u00e2che ne peut pas \u00eatre effectu\u00e9e", L"A tarefa n\u00e3o pode ser conclu\u00edda"},
    {250, L"Current user", L"Utente corrente", L"\u06a9\u0627\u0631\u0628\u0631 \u0641\u0639\u0644\u06cc", L"Usuario actual", L"Utilisateur actuel", L"Usu\u00e1rio atual"},
    {251, L"Welcome screen", L"Schermata iniziale", L"\u0635\u0641\u062d\u0647 \u062e\u0648\u0634\u200c\u0622\u0645\u062f\u06af\u0648\u06cc\u06cc", L"Pantalla de bienvenida", L"\u00c9cran d'accueil", L"Tela de boas-vindas"},
    {252, L"New user accounts", L"Nuovi account utente", L"\u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u062c\u062f\u06cc\u062f", L"Cuentas de usuario nuevas", L"Nouveaux comptes d'utilisateurs", L"Novas contas de usu\u00e1rio"},
    {253, L"Display language:", L"Lingua di visualizzazione:", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634:", L"Idioma para mostrar:", L"Langue d'affichage :", L"Idioma de exibi\u00e7\u00e3o:"},
    {254, L"Input language:", L"Lingua di input:", L"\u0632\u0628\u0627\u0646 \u0648\u0631\u0648\u062f\u06cc:", L"Idioma de entrada:", L"Langue d'entr\u00e9e :", L"Idioma de entrada:"},
    {255, L"Format:", L"Formato:", L"\u0642\u0627\u0644\u0628:", L"Formato:", L"Format :", L"Formato:"},
    {256, L"Location:", L"Posizione:", L"\u0645\u06a9\u0627\u0646:", L"Ubicaci\u00f3n:", L"Emplacement :", L"Local:"},
    {257, L"Setting could not be read", L"Impossibile leggere l'impostazione", L"\u062e\u0648\u0627\u0646\u062f\u0646 \u062a\u0646\u0638\u06cc\u0645 \u0627\u0645\u06a9\u0627\u0646\u200c\u067e\u0630\u06cc\u0631 \u0646\u0628\u0648\u062f", L"No se pudo leer la configuraci\u00f3n", L"Impossible de lire le param\u00e8tre", L"N\u00e3o foi poss\u00edvel ler a configura\u00e7\u00e3o"},
    {900, L"Context", L"Contesto", L"\u0645\u062a\u0646", L"Contexto", L"Contexte", L"Contexto"},
    {901, L"Never", L"Mai", L"\u0647\u0631\u06af\u0632", L"Nunca", L"Jamais", L"Nunca"},
    {902, L"National", L"Nazionale", L"\u0645\u0644\u06cc", L"Nacional", L"National", L"Nacional"},
    {0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
};

// Complete intl.cpl.mui dialog caption/control string table (all 83 strings)
static const MuiStringTable kIntlDlgStrings[] = {
    {1, L"Formats", L"Formati", L"\u0642\u0627\u0644\u0628\u200c\u0647\u0627", L"Formatos", L"Formats", L"Formatos"},
    {2, L"<A>Change sorting method</A>", L"<A>Modifica metodo di ordinamento</A>", L"<A>\u062a\u063a\u06cc\u06cc\u0631 \u0631\u0648\u0634 \u0645\u0631\u062a\u0628\u200c\u0633\u0627\u0632\u06cc</A>", L"<A>Cambiar el m\u00e9todo de ordenaci\u00f3n</A>", L"<A>Modifier la m\u00e9thode de tri</A>", L"<A>Alterar m\u00e9todo de classifica\u00e7\u00e3o</A>"},
    {3, L"<A>Language preferences</A>", L"<A>Preferenze lingua</A>", L"<A>\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0628\u0631\u06af\u0632\u06cc\u062f\u0647 \u0632\u0628\u0627\u0646</A>", L"<A>Preferencias de idioma</A>", L"<A>Pr\u00e9f\u00e9rences linguistiques</A>", L"<A>Prefer\u00eancias de idioma</A>"},
    {4, L"Date and time formats", L"Formati data e ora", L"\u0642\u0627\u0644\u0628\u200c\u0647\u0627\u06cc \u062a\u0627\u0631\u06cc\u062e \u0648 \u0632\u0645\u0627\u0646", L"Formatos de fecha y hora", L"Formats de date et d'heure", L"Formatos de data e hora"},
    {5, L"&Short date:", L"Data &breve:", L"\u062a\u0627\u0631\u06cc\u062e &\u06a9\u0648\u062a\u0627\u0647:", L"Fecha &corta:", L"Date &courte :", L"Data &abreviada:"},
    {6, L"&Long date:", L"Data &estesa:", L"\u062a\u0627\u0631\u06cc\u062e &\u0637\u0648\u0644\u0627\u0646\u06cc:", L"Fecha &larga:", L"Date &compl\u00e8te :", L"Data por &extenso:"},
    {7, L"S&hort time:", L"&Ora breve:", L"\u0632\u0645\u0627\u0646 \u06a9\u0648&\u062a\u0627\u0647:", L"&Hora corta:", L"&Heure courte :", L"&Hora abreviada:"},
    {8, L"L&ong time:", L"O&ra estesa:", L"\u0632\u0645\u0627\u0646 \u0637\u0648&\u0644\u0627\u0646\u06cc:", L"H&ora larga:", L"H&eure compl\u00e8te :", L"H&ora por extenso:"},
    {9, L"First day of &week:", L"&Primo giorno della settimana:", L"&\u0627\u0648\u0644\u06cc\u0646 \u0631\u0648\u0632 \u0647\u0641\u062a\u0647:", L"&Primer d\u00eda de la semana:", L"&Premier jour de la semaine :", L"&Primeiro dia da semana:"},
    {10, L"Examples", L"Esempi", L"\u0646\u0645\u0648\u0646\u0647\u200c\u0647\u0627", L"Ejemplos", L"Exemples", L"Exemplos"},
    {11, L"Short date:", L"Data breve:", L"\u062a\u0627\u0631\u06cc\u062e \u06a9\u0648\u062a\u0627\u0647:", L"Fecha corta:", L"Date courte :", L"Data abreviada:"},
    {12, L"Long date:", L"Data estesa:", L"\u062a\u0627\u0631\u06cc\u062e \u0637\u0648\u0644\u0627\u0646\u06cc:", L"Fecha larga:", L"Date compl\u00e8te :", L"Data por extenso:"},
    {13, L"Short time:", L"Ora breve:", L"\u0632\u0645\u0627\u0646 \u06a9\u0648\u062a\u0627\u0647:", L"Hora corta:", L"Heure courte :", L"Hora abreviada:"},
    {14, L"Long time:", L"Ora estesa:", L"\u0632\u0645\u0627\u0646 \u0637\u0648\u0644\u0627\u0646\u06cc:", L"Hora larga:", L"Heure compl\u00e8te :", L"Hora por extenso:"},
    {15, L"A&dditional settings...", L"&Impostazioni aggiuntive...", L"&\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0627\u0636\u0627\u0641\u06cc...", L"&Configuraci\u00f3n adicional...", L"&Param\u00e8tres suppl\u00e9mentaires...", L"&Configura\u00e7\u00f5es adicionais..."},
    {16, L"Administrative", L"Opzioni di amministrazione", L"\u0645\u062f\u06cc\u0631\u06cc\u062a\u06cc", L"Administrativo", L"Administration", L"Administrativo"},
    {17, L"Welcome screen and new user accounts", L"Schermata iniziale e nuovi account utente", L"\u0635\u0641\u062d\u0647 \u062e\u0648\u0634\u200c\u0622\u0645\u062f\u06af\u0648\u06cc\u06cc \u0648 \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u062c\u062f\u06cc\u062f", L"Pantalla de bienvenida y cuentas de usuario nuevas", L"\u00c9cran d'accueil et nouveaux comptes d'utilisateurs", L"Tela de boas-vindas e novas contas de usu\u00e1rio"},
    {18, L"View and copy your international settings to the welcome screen, system accounts and new user accounts.", L"Visualizza e copia le impostazioni internazionali nella schermata iniziale, negli account di sistema e nei nuovi account utente.", L"\u0645\u0634\u0627\u0647\u062f\u0647 \u0648 \u06a9\u067e\u06cc \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0628\u06cc\u0646\u200c\u0627\u0644\u0645\u0644\u0644\u06cc \u0634\u0645\u0627 \u0628\u0647 \u0635\u0641\u062d\u0647 \u062e\u0648\u0634\u200c\u0622\u0645\u062f\u06af\u0648\u06cc\u06cc\u060c \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u0633\u06cc\u0633\u062a\u0645 \u0648 \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u062c\u062f\u06cc\u062f.", L"Vea y copie su configuraci\u00f3n internacional en la pantalla de bienvenida, cuentas del sistema y cuentas de usuario nuevas.", L"Afficher et copier vos param\u00e8tres internationaux sur l'\u00e9cran d'accueil, les comptes syst\u00e8me et les nouveaux comptes d'utilisateurs.", L"Exiba e copie suas configura\u00e7\u00f5es internacionais para a tela de boas-vindas, contas do sistema e novas contas de usu\u00e1rio."},
    {19, L"C&opy settings...", L"&Copia impostazioni...", L"&\u06a9\u067e\u06cc \u062a\u0646\u0638\u06cc\u0645\u0627\u062a...", L"&Copiar configuraci\u00f3n...", L"&Copier les param\u00e8tres...", L"&Copiar configura\u00e7\u00f5es..."},
    {20, L"Language for non-Unicode programs", L"Lingua per programmi non Unicode", L"\u0632\u0628\u0627\u0646 \u0628\u0631\u0627\u06cc \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627\u06cc \u063a\u06cc\u0631 \u06cc\u0648\u0646\u06cc\u06a9\u062f", L"Idioma para programas no Unicode", L"Langue pour les programmes non Unicode", L"Idioma para programas n\u00e3o Unicode"},
    {21, L"This setting (system locale) controls the language used when displaying text in programs that do not support Unicode.", L"Questa impostazione (impostazioni locali del sistema) controlla la lingua usata per la visualizzazione del testo nei programmi che non supportano Unicode.", L"\u0627\u06cc\u0646 \u062a\u0646\u0638\u06cc\u0645 (\u0632\u0628\u0627\u0646 \u0645\u062d\u0644\u06cc \u0633\u06cc\u0633\u062a\u0645) \u0632\u0628\u0627\u0646 \u0645\u0648\u0631\u062f \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0647\u0646\u06af\u0627\u0645 \u0646\u0645\u0627\u06cc\u0634 \u0645\u062a\u0646 \u062f\u0631 \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627\u06cc\u06cc \u06a9\u0647 \u0627\u0632 \u06cc\u0648\u0646\u06cc\u06a9\u062f \u067e\u0634\u062a\u06cc\u0628\u0627\u0646\u06cc \u0646\u0645\u06cc\u200c\u06a9\u0646\u0646\u062f \u0631\u0627 \u06a9\u0646\u062a\u0631\u0644 \u0645\u06cc\u200c\u06a9\u0646\u062f.", L"Esta configuraci\u00f3n (configuraci\u00f3n regional del sistema) controla el idioma que se usa al mostrar texto en programas que no admiten Unicode.", L"Ce param\u00e8tre (param\u00e8tres r\u00e9gionaux du syst\u00e8me) contr\u00f4le la langue utilis\u00e9e lors de l'affichage du texte dans les programmes qui ne prennent pas en charge Unicode.", L"Esta configura\u00e7\u00e3o (localidade do sistema) controla o idioma usado ao exibir texto em programas que n\u00e3o oferecem suporte a Unicode."},
    {22, L"Current language for non-Unicode programs:", L"Lingua corrente per programmi non Unicode:", L"\u0632\u0628\u0627\u0646 \u0641\u0639\u0644\u06cc \u0628\u0631\u0627\u06cc \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627\u06cc \u063a\u06cc\u0631 \u06cc\u0648\u0646\u06cc\u06a9\u062f:", L"Idioma actual para programas no Unicode:", L"Langue actuelle pour les programmes non Unicode :", L"Idioma atual para programas n\u00e3o Unicode:"},
    {23, L"<systemLocale>", L"<systemLocale>", L"<systemLocale>", L"<systemLocale>", L"<systemLocale>", L"<systemLocale>"},
    {24, L"&Change system locale...", L"&Modifica impostazioni locali del sistema...", L"&\u062a\u063a\u06cc\u06cc\u0631 \u0632\u0628\u0627\u0646 \u0645\u062d\u0644\u06cc \u0633\u06cc\u0633\u062a\u0645...", L"C&ambiar configuraci\u00f3n regional del sistema...", L"&Modifier les param\u00e8tres r\u00e9gionaux du syst\u00e8me...", L"&Alterar localidade do sistema..."},
    {25, L"Numbers", L"Numeri", L"\u0627\u0639\u062f\u0627\u062f", L"N\u00fameros", L"Nombres", L"N\u00fameros"},
    {26, L"Example", L"Esempio", L"\u0646\u0645\u0648\u0646\u0647", L"Ejemplo", L"Exemple", L"Exemplo"},
    {27, L"Positive:", L"Positivo:", L"\u0645\u062b\u0628\u062a:", L"Positivo:", L"Positif :", L"Positivo:"},
    {28, L"Negative:", L"Negativo:", L"\u0645\u0646\u0641\u06cc:", L"Negativo:", L"N\u00e9gatif :", L"Negativo:"},
    {29, L"&Decimal symbol:", L"Separatore &decimale:", L"\u0646\u0645\u0627\u062f &\u0627\u0639\u0634\u0627\u0631:", L"S\u00edmbolo &decimal:", L"Symbole &d\u00e9cimal :", L"S\u00edmbolo &decimal:"},
    {30, L"&No. of digits after decimal:", L"&N. di cifre decimali:", L"&\u062a\u0639\u062f\u0627\u062f \u0627\u0631\u0642\u0627\u0645 \u0628\u0639\u062f \u0627\u0632 \u0627\u0639\u0634\u0627\u0631:", L"&N\u00ba de d\u00edgitos despu\u00e9s del decimal:", L"&Nombre de chiffres apr\u00e8s la virgule :", L"&N\u00ba de d\u00edgitos ap\u00f3s decimal:"},
    {31, L"D&igit grouping symbol:", L"Simbolo raggruppamento &cifre:", L"\u0646\u0645\u0627\u062f &\u06af\u0631\u0648\u0647\u200c\u0628\u0646\u062f\u06cc \u0627\u0631\u0642\u0627\u0645:", L"S\u00edmbolo de &separaci\u00f3n de miles:", L"Symbole de &groupement des chiffres :", L"S\u00edmbolo de a&grupamento de d\u00edgitos:"},
    {32, L"Di&git grouping:", L"&Raggruppamento cifre:", L"\u06af\u0631\u0648\u0647\u200c&\u0628\u0646\u062f\u06cc \u0627\u0631\u0642\u0627\u0645:", L"Agrupaci\u00f3n de &d\u00edgitos:", L"G&roupement des chiffres :", L"Agrupamento de &d\u00edgitos:"},
    {33, L"N&egative sign symbol:", L"Simbolo segno &negativo:", L"\u0646\u0645\u0627\u062f \u0639\u0644\u0627\u0645\u062a &\u0645\u0646\u0641\u06cc:", L"S\u00edmbolo de signo &negativo:", L"Symbole du signe &n\u00e9gatif :", L"S\u00edmbolo de sinal &negativo:"},
    {34, L"Nega&tive number format:", L"&Formato numeri negativi:", L"&\u0642\u0627\u0644\u0628 \u0639\u062f\u062f \u0645\u0646\u0641\u06cc:", L"Formato de n\u00famero ne&gativo:", L"&Format de nombre n\u00e9gatif :", L"Formato de n\u00famero ne&gativo:"},
    {35, L"Dis&play leading zeros:", L"&Visualizza zeri iniziali:", L"&\u0646\u0645\u0627\u06cc\u0634 \u0635\u0641\u0631\u0647\u0627\u06cc \u0627\u0628\u062a\u062f\u0627\u06cc\u06cc:", L"&Mostrar ceros a la izquierda:", L"&Afficher les z\u00e9ros en t\u00eate :", L"&Exibir zeros \u00e0 esquerda:"},
    {36, L"&List separator:", L"&Separatore di elenco:", L"&\u062c\u062f\u0627\u06a9\u0646\u0646\u062f\u0647 \u0641\u0647\u0631\u0633\u062a:", L"Separador de &listas:", L"S\u00e9parateur de &liste :", L"Separador de &listas:"},
    {37, L"&Measurement system:", L"Sistema di &misura:", L"\u0633\u06cc\u0633\u062a\u0645 &\u0627\u0646\u062f\u0627\u0632\u0647\u200c\u06af\u06cc\u0631\u06cc:", L"Sistema de &medida:", L"Syst\u00e8me de &mesure :", L"Sistema de &medidas:"},
    {38, L"&Standard digits:", L"Cifre &standard:", L"\u0627\u0631\u0642\u0627\u0645 &\u0627\u0633\u062a\u0627\u0646\u062f\u0627\u0631\u062f:", L"D\u00edgitos &est\u00e1ndar:", L"Chiffres &standard :", L"D\u00edgitos &padr\u00e3o:"},
    {39, L"&Use native digits:", L"&Usa cifre native:", L"&\u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0627\u0632 \u0627\u0631\u0642\u0627\u0645 \u0628\u0648\u0645\u06cc:", L"&Usar d\u00edgitos nativos:", L"&Utiliser les chiffres natifs :", L"&Usar d\u00edgitos nativos:"},
    {40, L"Click Reset to restore the system default settings for numbers, currency, time, and date.", L"Fai clic su Reimposta per ripristinare le impostazioni predefinite del sistema per numeri, valuta, ora e data.", L"\u0628\u0631\u0627\u06cc \u0628\u0627\u0632\u06af\u0631\u062f\u0627\u0646\u06cc \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u067e\u06cc\u0634\u200c\u0641\u0631\u0636 \u0633\u06cc\u0633\u062a\u0645 \u0628\u0631\u0627\u06cc \u0627\u0639\u062f\u0627\u062f\u060c \u0648\u0627\u062d\u062f \u067e\u0648\u0644\u060c \u0632\u0645\u0627\u0646 \u0648 \u062a\u0627\u0631\u06cc\u062e \u0631\u0648\u06cc \u0628\u0627\u0632\u0646\u0634\u0627\u0646\u06cc \u06a9\u0644\u06cc\u06a9 \u06a9\u0646\u06cc\u062f.", L"Haga clic en Restablecer para restaurar la configuraci\u00f3n predeterminada del sistema para n\u00fameros, moneda, hora y fecha.", L"Cliquez sur R\u00e9initialiser pour restaurer les param\u00e8tres syst\u00e8me par d\u00e9faut pour les nombres, la devise, l'heure et la date.", L"Clique em Redefinir para restaurar as configura\u00e7\u00f5es padr\u00e3o do sistema para n\u00fameros, moeda, hora e data."},
    {41, L"&Reset", L"R&eimposta", L"&\u0628\u0627\u0632\u0646\u0634\u0627\u0646\u06cc", L"&Restablecer", L"&R\u00e9initialiser", L"&Redefinir"},
    {42, L"Currency", L"Valuta", L"\u0648\u0627\u062d\u062f \u067e\u0648\u0644", L"Moneda", L"Symbole mon\u00e9taire", L"Moeda"},
    {43, L"&Currency symbol:", L"Simbolo &valuta:", L"\u0646\u0645\u0627\u062f &\u0648\u0627\u062d\u062f \u067e\u0648\u0644:", L"S\u00edmbolo de &moneda:", L"Symbole &mon\u00e9taire :", L"S\u00edmbolo da &moeda:"},
    {44, L"&Positive currency format:", L"Formato valuta &positiva:", L"\u0642\u0627\u0644\u0628 \u0648\u0627\u062d\u062f \u067e\u0648\u0644 &\u0645\u062b\u0628\u062a:", L"Formato de moneda &positiva:", L"Format de devise &positif :", L"Formato de moeda &positiva:"},
    {45, L"&Negative currency format:", L"Formato valuta &negativa:", L"\u0642\u0627\u0644\u0628 \u0648\u0627\u062d\u062f \u067e\u0648\u0644 &\u0645\u0646\u0641\u06cc:", L"Formato de moneda &negativa:", L"Format de devise &n\u00e9gatif :", L"Formato de moeda &negativa:"},
    {46, L"N&o. of digits after decimal:", L"&N. di cifre decimali:", L"&\u062a\u0639\u062f\u0627\u062f \u0627\u0631\u0642\u0627\u0645 \u0628\u0639\u062f \u0627\u0632 \u0627\u0639\u0634\u0627\u0631:", L"&N\u00ba de d\u00edgitos despu\u00e9s del decimal:", L"&Nombre de chiffres apr\u00e8s la virgule :", L"&N\u00ba de d\u00edgitos ap\u00f3s decimal:"},
    {47, L"Di&git grouping symbol:", L"Simbolo raggruppamento &cifre:", L"\u0646\u0645\u0627\u062f &\u06af\u0631\u0648\u0647\u200c\u0628\u0646\u062f\u06cc \u0627\u0631\u0642\u0627\u0645:", L"S\u00edmbolo de &separaci\u00f3n de miles:", L"Symbole de &groupement des chiffres :", L"S\u00edmbolo de a&grupamento de d\u00edgitos:"},
    {48, L"D&igit grouping:", L"&Raggruppamento cifre:", L"\u06af\u0631\u0648\u0647\u200c&\u0628\u0646\u062f\u06cc \u0627\u0631\u0642\u0627\u0645:", L"Agrupaci\u00f3n de &d\u00edgitos:", L"G&roupement des chiffres :", L"Agrupamento de &d\u00edgitos:"},
    {49, L"Time", L"Ora", L"\u0632\u0645\u0627\u0646", L"Hora", L"Heure", L"Hora"},
    {50, L"Time formats", L"Formati ora", L"\u0642\u0627\u0644\u0628\u200c\u0647\u0627\u06cc \u0632\u0645\u0627\u0646", L"Formatos de hora", L"Formats d'heure", L"Formatos de hora"},
    {51, L"&Short time:", L"&Ora breve:", L"\u0632\u0645\u0627\u0646 &\u06a9\u0648\u062a\u0627\u0647:", L"Hora &corta:", L"Heure &courte :", L"Hora &abreviada:"},
    {52, L"&Long time:", L"O&ra estesa:", L"\u0632\u0645\u0627\u0646 &\u0637\u0648\u0644\u0627\u0646\u06cc:", L"Hora &larga:", L"Heure &compl\u00e8te :", L"Hora por &extenso:"},
    {53, L"A&M symbol:", L"Simbolo &AM:", L"\u0646\u0645\u0627\u062f &\u0642.\u0638:", L"S\u00edmbolo &a. m.:", L"Symbole &AM :", L"S\u00edmbolo &AM:"},
    {54, L"&PM symbol:", L"Simbolo &PM:", L"\u0646\u0645\u0627\u062f &\u0628.\u0638:", L"S\u00edmbolo &p. m.:", L"Symbole &PM :", L"S\u00edmbolo &PM:"},
    {55, L"What the notations mean:\n\nh = hour   m = minute\ns = second (long time only)\ntt = A.M. or P.M.\n\nh/H = 12/24 hour\n\nhh, mm, ss = display leading zero\nh, m, s = do not display leading zero", L"Significato della notazione:\n\nh = ora   m = minuto\ns = secondo (solo ora estesa)\ntt = AM o PM\n\nh/H = 12/24 ore\n\nhh, mm, ss = visualizza zero iniziale\nh, m, s = non visualizzare zero iniziale", L"\u0645\u0639\u0646\u0627\u06cc \u0646\u0645\u0627\u062f\u0647\u0627:\n\nh = \u0633\u0627\u0639\u062a   m = \u062f\u0642\u06cc\u0642\u0647\ns = \u062b\u0627\u0646\u06cc\u0647 (\u0641\u0642\u0637 \u0632\u0645\u0627\u0646 \u0637\u0648\u0644\u0627\u0646\u06cc)\ntt = \u0642.\u0638 \u06cc\u0627 \u0628.\u0638\n\nh/H = \u06f1\u06f2/\u06f2\u06f4 \u0633\u0627\u0639\u062a\u0647\n\nhh, mm, ss = \u0646\u0645\u0627\u06cc\u0634 \u0635\u0641\u0631 \u0627\u0628\u062a\u062f\u0627\u06cc\u06cc\nh, m, s = \u0639\u062f\u0645 \u0646\u0645\u0627\u06cc\u0634 \u0635\u0641\u0631 \u0627\u0628\u062a\u062f\u0627\u06cc\u06cc", L"Significado de las notaciones:\n\nh = hora   m = minuto\ns = segundo (solo hora larga)\ntt = A. M. o P. M.\n\nh/H = 12/24 horas\n\nhh, mm, ss = mostrar cero inicial\nh, m, s = no mostrar cero inicial", L"Signification des notations :\n\nh = heure   m = minute\ns = seconde (heure compl\u00e8te uniquement)\ntt = AM ou PM\n\nh/H = 12/24 heures\n\nhh, mm, ss = afficher le z\u00e9ro initial\nh, m, s = ne pas afficher le z\u00e9ro initial", L"Significado das nota\u00e7\u00f5es:\n\nh = hora   m = minuto\ns = segundo (apenas hora por extenso)\ntt = AM ou PM\n\nh/H = 12/24 horas\n\nhh, mm, ss = exibir zero \u00e0 esquerda\nh, m, s = n\u00e3o exibir zero \u00e0 esquerda"},
    {56, L"Date", L"Data", L"\u062a\u0627\u0631\u06cc\u062e", L"Fecha", L"Date", L"Data"},
    {57, L"Date formats", L"Formati data", L"\u0642\u0627\u0644\u0628\u200c\u0647\u0627\u06cc \u062a\u0627\u0631\u06cc\u062e", L"Formatos de fecha", L"Formats de date", L"Formatos de data"},
    {58, L"What the notations mean:\nd, dd = day;  ddd, dddd = day of week;  M = month;  y = year", L"Significato della notazione:\nd, dd = giorno;  ddd, dddd = giorno della settimana;  M = mese;  y = anno", L"\u0645\u0639\u0646\u0627\u06cc \u0646\u0645\u0627\u062f\u0647\u0627:\nd, dd = \u0631\u0648\u0632\u061b  ddd, dddd = \u0631\u0648\u0632 \u0647\u0641\u062a\u0647\u061b  M = \u0645\u0627\u0647\u061b  y = \u0633\u0627\u0644", L"Significado de las notaciones:\nd, dd = d\u00eda;  ddd, dddd = d\u00eda de la semana;  M = mes;  y = a\u00f1o", L"Signification des notations :\nd, dd = jour ;  ddd, dddd = jour de la semaine ;  M = mois ;  y = ann\u00e9e", L"Significado das nota\u00e7\u00f5es:\nd, dd = dia;  ddd, dddd = dia da semana;  M = m\u00eas;  y = ano"},
    {59, L"Calendar", L"Calendario", L"\u062a\u0642\u0648\u06cc\u0645", L"Calendario", L"Calendrier", L"Calend\u00e1rio"},
    {60, L"W&hen a two-digit year is entered, interpret it as a year between:", L"&Quando viene immesso un anno a due cifre, interpretalo come un anno compreso tra:", L"&\u0647\u0646\u06af\u0627\u0645 \u0648\u0627\u0631\u062f \u06a9\u0631\u062f\u0646 \u0633\u0627\u0644 \u062f\u0648 \u0631\u0642\u0645\u06cc\u060c \u0622\u0646 \u0631\u0627 \u0633\u0627\u0644\u06cc \u0628\u06cc\u0646 \u0627\u06cc\u0646 \u062f\u0648 \u062f\u0631 \u0646\u0638\u0631 \u0628\u06af\u06cc\u0631:", L"&Cuando se escriba un a\u00f1o de dos d\u00edgitos, interpretarlo como un a\u00f1o entre:", L"&Lorsqu'une ann\u00e9e \u00e0 deux chiffres est saisie, l'interpr\u00e9ter comme une ann\u00e9e comprise entre :", L"&Quando um ano de dois d\u00edgitos for inserido, interpret\u00e1-lo como um ano entre:"},
    {61, L"and", L"e", L"\u0648", L"y", L"et", L"e"},
    {62, L"&First day of week:", L"&Primo giorno della settimana:", L"&\u0627\u0648\u0644\u06cc\u0646 \u0631\u0648\u0632 \u0647\u0641\u062a\u0647:", L"&Primer d\u00eda de la semana:", L"&Premier jour de la semaine :", L"&Primeiro dia da semana:"},
    {63, L"&Calendar type:", L"&Tipo di calendario:", L"&\u0646\u0648\u0639 \u062a\u0642\u0648\u06cc\u0645:", L"&Tipo de calendario:", L"&Type de calendrier :", L"&Tipo de calend\u00e1rio:"},
    {64, L"A&djust Hijri date to:", L"&Regola data Hijri su:", L"&\u062a\u0646\u0638\u06cc\u0645 \u062a\u0627\u0631\u06cc\u062e \u0647\u062c\u0631\u06cc \u0642\u0645\u0631\u06cc \u0628\u0647:", L"&Ajustar fecha Hijri a:", L"&Ajuster la date Hijri \u00e0 :", L"&Ajustar data Hijri para:"},
    {65, L"Sorting", L"Ordinamento", L"\u0645\u0631\u062a\u0628\u200c\u0633\u0627\u0632\u06cc", L"Ordenaci\u00f3n", L"Tri", L"Classifica\u00e7\u00e3o"},
    {66, L"You can control the way some programs sort characters, words, files, and folders.", L"\u00c8 possibile controllare il modo in cui alcuni programmi ordinano caratteri, parole, file e cartelle.", L"\u0645\u06cc\u200c\u062a\u0648\u0627\u0646\u06cc\u062f \u0646\u062d\u0648\u0647 \u0645\u0631\u062a\u0628\u200c\u0633\u0627\u0632\u06cc \u0646\u0648\u06cc\u0633\u0647\u200c\u0647\u0627\u060c \u0648\u0627\u0698\u0647\u200c\u0647\u0627\u060c \u067e\u0631\u0648\u0646\u062f\u0647\u200c\u0647\u0627 \u0648 \u067e\u0648\u0634\u0647\u200c\u0647\u0627 \u0631\u0627 \u062f\u0631 \u0628\u0631\u062e\u06cc \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627 \u06a9\u0646\u062a\u0631\u0644 \u06a9\u0646\u06cc\u062f.", L"Puede controlar la forma en que algunos programas ordenan caracteres, palabras, archivos y carpetas.", L"Vous pouvez contr\u00f4ler la fa\u00e7on dont certains programmes trient les caract\u00e8res, les mots, les fichiers et les dossiers.", L"Voc\u00ea pode controlar a maneira como alguns programas classificam caracteres, palavras, arquivos e pastas."},
    {67, L"&Select the sorting method:", L"&Seleziona il metodo di ordinamento:", L"&\u0627\u0646\u062a\u062e\u0627\u0628 \u0631\u0648\u0634 \u0645\u0631\u062a\u0628\u200c\u0633\u0627\u0632\u06cc:", L"&Seleccionar el m\u00e9todo de ordenaci\u00f3n:", L"&S\u00e9lectionner la m\u00e9thode de tri :", L"&Selecionar o m\u00e9todo de classifica\u00e7\u00e3o:"},
    {68, L"Location", L"Posizione", L"\u0645\u06a9\u0627\u0646", L"Ubicaci\u00f3n", L"Emplacement", L"Local"},
    {69, L"Some software, including Windows, may provide you with additional content for a particular location. Some services provide local information such as news and weather.", L"Alcuni software, incluso Windows, potrebbero fornire contenuti aggiuntivi per una posizione specifica. Alcuni servizi forniscono informazioni locali come notizie e meteo.", L"\u0628\u0631\u062e\u06cc \u0646\u0631\u0645\u200c\u0627\u0641\u0632\u0627\u0631\u0647\u0627 \u0627\u0632 \u062c\u0645\u0644\u0647 \u0648\u06cc\u0646\u062f\u0648\u0632 \u0645\u0645\u06a9\u0646 \u0627\u0633\u062a \u0645\u062d\u062a\u0648\u0627\u06cc \u0627\u0636\u0627\u0641\u06cc \u0628\u0631\u0627\u06cc \u0645\u06a9\u0627\u0646 \u062e\u0627\u0635\u06cc \u0627\u0631\u0627\u0626\u0647 \u062f\u0647\u0646\u062f. \u0628\u0631\u062e\u06cc \u062e\u062f\u0645\u0627\u062a \u0627\u0637\u0644\u0627\u0639\u0627\u062a \u0645\u062d\u0644\u06cc \u0645\u0627\u0646\u0646\u062f \u0627\u062e\u0628\u0627\u0631 \u0648 \u0622\u0628\u200c\u0648\u0647\u0648\u0627 \u0631\u0627 \u0641\u0631\u0627\u0647\u0645 \u0645\u06cc\u200c\u06a9\u0646\u0646\u062f.", L"Algunos programas, incluido Windows, pueden proporcionarle contenido adicional para una ubicaci\u00f3n determinada. Algunos servicios proporcionan informaci\u00f3n local, como noticias y el tiempo.", L"Certains logiciels, dont Windows, peuvent vous fournir du contenu suppl\u00e9mentaire pour un emplacement particulier. Certains services fournissent des informations locales telles que les actualit\u00e9s et la m\u00e9t\u00e9o.", L"Alguns softwares, incluindo o Windows, podem fornecer conte\u00fado adicional para um local espec\u00edfico. Alguns servi\u00e7os fornecem informa\u00e7\u00f5es locais, como not\u00edcias e clima."},
    {70, L"&Home location:", L"&Posizione corrente:", L"&\u0645\u06a9\u0627\u0646 \u0627\u0635\u0644\u06cc:", L"&Ubicaci\u00f3n principal:", L"&Emplacement principal :", L"&Localiza\u00e7\u00e3o inicial:"},
    {71, L"Welcome screen and new user accounts settings", L"Impostazioni schermata iniziale e nuovi account utente", L"\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0635\u0641\u062d\u0647 \u062e\u0648\u0634\u200c\u0622\u0645\u062f\u06af\u0648\u06cc\u06cc \u0648 \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u062c\u062f\u06cc\u062f", L"Configuraci\u00f3n de la pantalla de bienvenida y cuentas de usuario nuevas", L"Param\u00e8tres de l'\u00e9cran d'accueil et des nouveaux comptes d'utilisateurs", L"Configura\u00e7\u00f5es da tela de boas-vindas e novas contas de usu\u00e1rio"},
    {72, L"The &settings for the current user, welcome screen (system accounts) and new user accounts are displayed below.", L"Di seguito sono visualizzate le &impostazioni per l'utente corrente, la schermata iniziale (account di sistema) e i nuovi account utente.", L"&\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0645\u0631\u0628\u0648\u0637 \u0628\u0647 \u06a9\u0627\u0631\u0628\u0631 \u0641\u0639\u0644\u06cc\u060c \u0635\u0641\u062d\u0647 \u062e\u0648\u0634\u200c\u0622\u0645\u062f\u06af\u0648\u06cc\u06cc (\u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u0633\u06cc\u0633\u062a\u0645) \u0648 \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u062c\u062f\u06cc\u062f \u062f\u0631 \u0632\u06cc\u0631 \u0646\u0645\u0627\u06cc\u0634 \u062f\u0627\u062f\u0647 \u0634\u062f\u0647 \u0627\u0633\u062a.", L"A continuaci\u00f3n se muestra la &configuraci\u00f3n para el usuario actual, la pantalla de bienvenida (cuentas del sistema) y las cuentas de usuario nuevas.", L"Les &param\u00e8tres de l'utilisateur actuel, de l'\u00e9cran d'accueil (comptes syst\u00e8me) et des nouveaux comptes d'utilisateurs sont affich\u00e9s ci-dessous.", L"As &configura\u00e7\u00f5es para o usu\u00e1rio atual, tela de boas-vindas (contas do sistema) e novas contas de usu\u00e1rio s\u00e3o exibidas abaixo."},
    {73, L"* Custom locale", L"* Impostazioni locali personalizzate", L"* \u0645\u062d\u0644\u06cc \u0633\u0641\u0627\u0631\u0634\u06cc", L"* Configuraci\u00f3n regional personalizada", L"* Param\u00e8tres r\u00e9gionaux personnalis\u00e9s", L"* Localidade personalizada"},
    {74, L"Copy your current settings to:", L"Copia le impostazioni correnti in:", L"\u06a9\u067e\u06cc \u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0641\u0639\u0644\u06cc \u0634\u0645\u0627 \u0628\u0647:", L"Copiar la configuraci\u00f3n actual en:", L"Copier vos param\u00e8tres actuels vers :", L"Copiar suas configura\u00e7\u00f5es atuais para:"},
    {75, L"&Welcome screen and system accounts", L"&Schermata iniziale e account di sistema", L"&\u0635\u0641\u062d\u0647 \u062e\u0648\u0634\u200c\u0622\u0645\u062f\u06af\u0648\u06cc\u06cc \u0648 \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u0633\u06cc\u0633\u062a\u0645", L"&Pantalla de bienvenida y cuentas del sistema", L"&\u00c9cran d'accueil et comptes syst\u00e8me", L"&Tela de boas-vindas e contas do sistema"},
    {76, L"&New user accounts", L"&Nuovi account utente", L"&\u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u062c\u062f\u06cc\u062f", L"&Cuentas de usuario nuevas", L"&Nouveaux comptes d'utilisateurs", L"&Novas contas de usu\u00e1rio"},
    {77, L"The new user accounts display language is currently inherited from the welcome screen display language.", L"La lingua di visualizzazione dei nuovi account utente \u00e8 attualmente ereditata dalla lingua di visualizzazione della schermata iniziale.", L"\u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u062c\u062f\u06cc\u062f \u062f\u0631 \u062d\u0627\u0644 \u062d\u0627\u0636\u0631 \u0627\u0632 \u0632\u0628\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0635\u0641\u062d\u0647 \u062e\u0648\u0634\u200c\u0622\u0645\u062f\u06af\u0648\u06cc\u06cc \u0628\u0647 \u0627\u0631\u062b \u0645\u06cc\u200c\u0631\u0633\u062f.", L"El idioma para mostrar de las cuentas de usuario nuevas se hereda actualmente del idioma para mostrar de la pantalla de bienvenida.", L"La langue d'affichage des nouveaux comptes d'utilisateurs est actuellement h\u00e9rit\u00e9e de la langue d'affichage de l'\u00e9cran d'accueil.", L"O idioma de exibi\u00e7\u00e3o das novas contas de usu\u00e1rio \u00e9 herdado atualmente do idioma de exibi\u00e7\u00e3o da tela de boas-vindas."},
    {78, L"OK", L"OK", L"\u062a\u0623\u06cc\u06cc\u062f", L"Aceptar", L"OK", L"OK"},
    {79, L"Cancel", L"Annulla", L"\u0627\u0646\u0635\u0631\u0627\u0641", L"Cancelar", L"Annuler", L"Cancelar"},
    {80, L"Region Settings", L"Impostazioni area geografica", L"\u062a\u0646\u0638\u06cc\u0645\u0627\u062a \u0645\u0646\u0637\u0642\u0647", L"Configuraci\u00f3n de regi\u00f3n", L"Param\u00e8tres de r\u00e9gion", L"Configura\u00e7\u00f5es de Regi\u00e3o"},
    {81, L"Select which language (system locale) to use when displaying text in programs that do not support Unicode. This setting affects all user accounts on the computer.", L"Seleziona la lingua (impostazioni locali del sistema) da usare per la visualizzazione del testo nei programmi che non supportano Unicode. Questa impostazione ha effetto su tutti gli account utente del computer.", L"\u0632\u0628\u0627\u0646 (\u0645\u062d\u0644\u06cc \u0633\u06cc\u0633\u062a\u0645) \u0645\u0648\u0631\u062f \u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0647\u0646\u06af\u0627\u0645 \u0646\u0645\u0627\u06cc\u0634 \u0645\u062a\u0646 \u062f\u0631 \u0628\u0631\u0646\u0627\u0645\u0647\u200c\u0647\u0627\u06cc\u06cc \u06a9\u0647 \u0627\u0632 \u06cc\u0648\u0646\u06cc\u06a9\u062f \u067e\u0634\u062a\u06cc\u0628\u0627\u0646\u06cc \u0646\u0645\u06cc\u200c\u06a9\u0646\u0646\u062f \u0631\u0627 \u0627\u0646\u062a\u062e\u0627\u0628 \u06a9\u0646\u06cc\u062f. \u0627\u06cc\u0646 \u062a\u0646\u0638\u06cc\u0645 \u0628\u0631 \u062a\u0645\u0627\u0645 \u062d\u0633\u0627\u0628\u200c\u0647\u0627\u06cc \u06a9\u0627\u0631\u0628\u0631\u06cc \u0631\u0627\u06cc\u0627\u0646\u0647 \u062a\u0623\u062b\u06cc\u0631 \u0645\u06cc\u200c\u06af\u0630\u0627\u0631\u062f.", L"Seleccione el idioma (configuraci\u00f3n regional del sistema) que desea usar al mostrar texto en programas que no admiten Unicode. Esta configuraci\u00f3n afecta a todas las cuentas de usuario del equipo.", L"S\u00e9lectionnez la langue (param\u00e8tres r\u00e9gionaux du syst\u00e8me) \u00e0 utiliser lors de l'affichage du texte dans les programmes qui ne prennent pas en charge Unicode. Ce param\u00e8tre affecte tous les comptes d'utilisateurs de l'ordinateur.", L"Selecione qual idioma (localidade do sistema) usar ao exibir texto em programas que n\u00e3o oferecem suporte a Unicode. Esta configura\u00e7\u00e3o afeta todas as contas de usu\u00e1rio no computador."},
    {82, L"&Current system locale:", L"&Impostazioni locali del sistema correnti:", L"&\u0632\u0628\u0627\u0646 \u0645\u062d\u0644\u06cc \u0641\u0639\u0644\u06cc \u0633\u06cc\u0633\u062a\u0645:", L"&Configuraci\u00f3n regional del sistema actual:", L"&Param\u00e8tres r\u00e9gionaux actuels du syst\u00e8me :", L"&Localidade atual do sistema:"},
    {83, L"Beta: &Use Unicode UTF-8 for worldwide language support", L"Beta: &Utilizza Unicode UTF-8 per il supporto della lingua a livello mondiale", L"\u0622\u0632\u0645\u0627\u06cc\u0634\u06cc: &\u0627\u0633\u062a\u0641\u0627\u062f\u0647 \u0627\u0632 Unicode UTF-8 \u0628\u0631\u0627\u06cc \u067e\u0634\u062a\u06cc\u0628\u0627\u0646\u06cc \u0627\u0632 \u0632\u0628\u0627\u0646\u200c\u0647\u0627\u06cc \u062c\u0647\u0627\u0646\u06cc", L"Beta: &Usar Unicode UTF-8 para la compatibilidad de idioma mundial", L"B\u00eata : &Utiliser Unicode UTF-8 pour une prise en charge des langues \u00e0 l'\u00e9chelle mondiale", L"Beta: &Usar Unicode UTF-8 para suporte de linguagem mundial"},
    {0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
};

struct IntlDlgItemSpec {
    uint32_t exStyle;
    uint32_t style;
    int16_t x, y, cx, cy;
    uint32_t id;
    uint16_t clsOrd;
    const wchar_t* clsName;
    uint16_t titleId;
};

struct IntlDlgSpec {
    uint16_t dlgId;
    uint32_t style;
    int16_t x, y, cx, cy;
    uint16_t titleId;
    const IntlDlgItemSpec* items;
    size_t itemCount;
};

static const IntlDlgItemSpec kIntlDlgItems_101[] = {
    {0x0u, 0x50020000u, 7, 7, 235, 8, 0x40fu, 130, nullptr, 0},
    {0x0u, 0x50210003u, 7, 17, 238, 60, 0x3fdu, 133, nullptr, 0},
    {0x0u, 0x50010000u, 7, 32, 119, 8, 0x409u, 0, L"SysLink", 2},
    {0x0u, 0x50010000u, 7, 42, 119, 8, 0x40au, 0, L"SysLink", 3},
    {0x0u, 0x50000007u, 7, 54, 238, 99, 0xffffffffu, 128, nullptr, 4},
    {0x0u, 0x50020000u, 12, 68, 64, 8, 0xffffffffu, 130, nullptr, 5},
    {0x0u, 0x50210003u, 77, 66, 163, 60, 0x404u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 12, 83, 64, 8, 0xffffffffu, 130, nullptr, 6},
    {0x0u, 0x50210003u, 77, 81, 163, 60, 0x405u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 12, 98, 64, 8, 0xffffffffu, 130, nullptr, 7},
    {0x0u, 0x50210003u, 77, 96, 163, 60, 0x406u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 12, 113, 64, 8, 0xffffffffu, 130, nullptr, 8},
    {0x0u, 0x50210003u, 77, 111, 163, 60, 0x407u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 12, 128, 64, 8, 0xffffffffu, 130, nullptr, 9},
    {0x0u, 0x50210003u, 77, 126, 163, 60, 0x408u, 133, nullptr, 0},
    {0x0u, 0x50000007u, 7, 156, 238, 60, 0xffffffffu, 128, nullptr, 10},
    {0x0u, 0x50020000u, 12, 166, 64, 8, 0xffffffffu, 130, nullptr, 11},
    {0x0u, 0x50020000u, 77, 166, 163, 8, 0x40bu, 130, nullptr, 0},
    {0x0u, 0x50020000u, 12, 178, 64, 8, 0xffffffffu, 130, nullptr, 12},
    {0x0u, 0x50020000u, 77, 178, 163, 8, 0x40cu, 130, nullptr, 0},
    {0x0u, 0x50020000u, 12, 190, 64, 8, 0xffffffffu, 130, nullptr, 13},
    {0x0u, 0x50020000u, 77, 190, 163, 8, 0x40du, 130, nullptr, 0},
    {0x0u, 0x50020000u, 12, 202, 64, 8, 0xffffffffu, 130, nullptr, 14},
    {0x0u, 0x50020000u, 77, 202, 163, 8, 0x40eu, 130, nullptr, 0},
    {0x0u, 0x50010000u, 131, 220, 115, 13, 0x400u, 128, nullptr, 15},
};
static const IntlDlgItemSpec kIntlDlgItems_104[] = {
    {0x0u, 0x50000007u, 5, 8, 242, 73, 0xffffffffu, 128, nullptr, 17},
    {0x0u, 0x50020000u, 14, 20, 223, 23, 0x83bu, 130, nullptr, 18},
    {0x0u, 0x50030000u, 160, 51, 80, 14, 0x838u, 128, nullptr, 19},
    {0x0u, 0x50000007u, 5, 83, 242, 91, 0xffffffffu, 128, nullptr, 20},
    {0x0u, 0x50020000u, 14, 95, 225, 26, 0x41au, 130, nullptr, 21},
    {0x0u, 0x50020000u, 14, 120, 223, 10, 0x41bu, 130, nullptr, 22},
    {0x0u, 0x50020000u, 25, 132, 200, 10, 0x834u, 130, nullptr, 23},
    {0x0u, 0x50030000u, 140, 144, 100, 14, 0x836u, 128, nullptr, 24},
};
static const IntlDlgItemSpec kIntlDlgItems_105[] = {
    {0x0u, 0x50000007u, 5, 7, 242, 48, 0x3f1u, 128, nullptr, 26},
    {0x0u, 0x50020000u, 10, 20, 30, 10, 0x3efu, 130, nullptr, 27},
    {0x0u, 0x50800880u, 42, 18, 80, 14, 0x3edu, 129, nullptr, 0},
    {0x0u, 0x50020000u, 128, 20, 31, 10, 0x3f0u, 130, nullptr, 28},
    {0x0u, 0x50800880u, 162, 18, 80, 14, 0x3eeu, 129, nullptr, 0},
    {0x0u, 0x50020000u, 10, 38, 30, 10, 0x3f4u, 130, nullptr, 27},
    {0x3000u, 0x50800882u, 42, 36, 80, 14, 0x3f2u, 129, nullptr, 0},
    {0x0u, 0x50020000u, 128, 38, 31, 10, 0x3f5u, 130, nullptr, 28},
    {0x3000u, 0x50800882u, 162, 36, 80, 14, 0x3f3u, 129, nullptr, 0},
    {0x0u, 0x50020000u, 20, 61, 96, 9, 0xffffffffu, 130, nullptr, 29},
    {0x0u, 0x50210102u, 128, 59, 100, 100, 0x42eu, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 77, 96, 9, 0xffffffffu, 130, nullptr, 30},
    {0x0u, 0x50210103u, 128, 75, 100, 100, 0x430u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 93, 96, 9, 0xffffffffu, 130, nullptr, 31},
    {0x0u, 0x50210102u, 128, 91, 100, 100, 0x431u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 109, 96, 9, 0xffffffffu, 130, nullptr, 32},
    {0x0u, 0x50210103u, 128, 107, 100, 100, 0x432u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 125, 96, 9, 0xffffffffu, 130, nullptr, 33},
    {0x0u, 0x50210102u, 128, 123, 100, 100, 0x434u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 141, 96, 9, 0xffffffffu, 130, nullptr, 34},
    {0x0u, 0x50210103u, 128, 139, 100, 100, 0x436u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 157, 96, 9, 0xffffffffu, 130, nullptr, 35},
    {0x0u, 0x50210103u, 128, 155, 100, 100, 0x438u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 173, 96, 9, 0xffffffffu, 130, nullptr, 36},
    {0x0u, 0x50210102u, 128, 171, 100, 100, 0x437u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 189, 96, 9, 0xffffffffu, 130, nullptr, 37},
    {0x0u, 0x50210103u, 128, 187, 100, 100, 0x439u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 205, 96, 9, 0x43au, 130, nullptr, 38},
    {0x0u, 0x50210103u, 128, 203, 100, 100, 0x43bu, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 221, 96, 9, 0x43cu, 130, nullptr, 39},
    {0x0u, 0x50210103u, 128, 219, 100, 100, 0x43du, 133, nullptr, 0},
    {0x0u, 0x50020000u, 7, 236, 185, 16, 0x417u, 130, nullptr, 40},
    {0x0u, 0x50010000u, 196, 237, 50, 13, 0x416u, 128, nullptr, 41},
};
static const IntlDlgItemSpec kIntlDlgItems_106[] = {
    {0x0u, 0x50000007u, 5, 7, 242, 40, 0x3f1u, 128, nullptr, 26},
    {0x0u, 0x50020000u, 10, 25, 30, 10, 0x3efu, 130, nullptr, 27},
    {0x0u, 0x50800880u, 42, 23, 80, 14, 0x3edu, 129, nullptr, 0},
    {0x0u, 0x50020000u, 128, 25, 31, 10, 0x3f0u, 130, nullptr, 28},
    {0x0u, 0x50800880u, 162, 23, 80, 14, 0x3eeu, 129, nullptr, 0},
    {0x0u, 0x50020000u, 20, 61, 96, 9, 0xffffffffu, 130, nullptr, 43},
    {0x0u, 0x50210102u, 128, 59, 100, 100, 0x42fu, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 78, 96, 9, 0xffffffffu, 130, nullptr, 44},
    {0x0u, 0x50210103u, 128, 76, 100, 100, 0x435u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 95, 96, 9, 0xffffffffu, 130, nullptr, 45},
    {0x0u, 0x50210103u, 128, 93, 100, 100, 0x436u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 121, 96, 9, 0xffffffffu, 130, nullptr, 29},
    {0x0u, 0x50210102u, 128, 119, 100, 100, 0x42eu, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 138, 96, 9, 0xffffffffu, 130, nullptr, 46},
    {0x0u, 0x50210103u, 128, 136, 100, 100, 0x430u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 166, 96, 9, 0xffffffffu, 130, nullptr, 47},
    {0x0u, 0x50210102u, 128, 164, 100, 100, 0x431u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 20, 183, 96, 9, 0xffffffffu, 130, nullptr, 48},
    {0x0u, 0x50210103u, 128, 181, 100, 100, 0x432u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 7, 236, 185, 16, 0x417u, 130, nullptr, 40},
    {0x0u, 0x50010000u, 196, 237, 50, 13, 0x416u, 128, nullptr, 41},
};
static const IntlDlgItemSpec kIntlDlgItems_107[] = {
    {0x0u, 0x50000007u, 5, 7, 242, 47, 0xffffffffu, 128, nullptr, 10},
    {0x0u, 0x50020000u, 13, 20, 54, 10, 0xffffffffu, 130, nullptr, 13},
    {0x0u, 0x50800880u, 68, 18, 84, 14, 0x3eeu, 129, nullptr, 0},
    {0x2000u, 0x50800882u, 156, 18, 84, 14, 0x3f3u, 129, nullptr, 0},
    {0x0u, 0x50020000u, 13, 36, 54, 10, 0xffffffffu, 130, nullptr, 14},
    {0x0u, 0x50800880u, 68, 34, 84, 14, 0x3edu, 129, nullptr, 0},
    {0x2000u, 0x50800882u, 156, 34, 84, 14, 0x3f2u, 129, nullptr, 0},
    {0x0u, 0x50000007u, 5, 57, 242, 82, 0xffffffffu, 128, nullptr, 50},
    {0x0u, 0x50020000u, 13, 70, 54, 10, 0xffffffffu, 130, nullptr, 51},
    {0x0u, 0x50210142u, 68, 68, 84, 100, 0x445u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 13, 86, 54, 10, 0xffffffffu, 130, nullptr, 52},
    {0x0u, 0x50210142u, 68, 84, 84, 100, 0x442u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 13, 106, 54, 10, 0xffffffffu, 130, nullptr, 53},
    {0x0u, 0x50210102u, 68, 104, 84, 100, 0x443u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 13, 123, 54, 10, 0xffffffffu, 130, nullptr, 54},
    {0x0u, 0x50210102u, 68, 121, 84, 100, 0x444u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 13, 144, 226, 89, 0xffffffffu, 130, nullptr, 55},
    {0x0u, 0x50020000u, 7, 236, 185, 16, 0xffffffffu, 130, nullptr, 40},
    {0x0u, 0x50010000u, 196, 237, 50, 13, 0x416u, 128, nullptr, 41},
};
static const IntlDlgItemSpec kIntlDlgItems_108[] = {
    {0x0u, 0x50000007u, 5, 7, 242, 51, 0x3e9u, 128, nullptr, 26},
    {0x0u, 0x50020000u, 13, 20, 63, 10, 0x3efu, 130, nullptr, 11},
    {0x0u, 0x50800880u, 77, 18, 110, 14, 0x3edu, 129, nullptr, 0},
    {0x0u, 0x50020000u, 13, 37, 61, 10, 0x3f0u, 130, nullptr, 12},
    {0x0u, 0x50800880u, 77, 35, 110, 14, 0x3eeu, 129, nullptr, 0},
    {0x0u, 0x50000007u, 5, 65, 242, 70, 0x3eau, 128, nullptr, 57},
    {0x0u, 0x50020000u, 13, 77, 60, 10, 0xffffffffu, 130, nullptr, 5},
    {0x0u, 0x50210142u, 77, 75, 161, 100, 0x452u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 13, 95, 60, 10, 0xffffffffu, 130, nullptr, 6},
    {0x0u, 0x50210142u, 77, 92, 161, 100, 0x453u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 13, 111, 226, 20, 0x3f1u, 130, nullptr, 58},
    {0x0u, 0x50000007u, 5, 143, 242, 85, 0x3ebu, 128, nullptr, 59},
    {0x0u, 0x50020000u, 13, 154, 225, 8, 0xffffffffu, 130, nullptr, 60},
    {0x0u, 0x58812080u, 18, 165, 36, 12, 0x44eu, 129, nullptr, 0},
    {0x0u, 0x50020000u, 58, 167, 17, 8, 0xffffffffu, 130, nullptr, 61},
    {0x0u, 0x50812000u, 77, 165, 37, 12, 0x44fu, 129, nullptr, 0},
    {0x0u, 0x500000b7u, 113, 165, 10, 12, 0x450u, 0, L"msctls_updown32", 0},
    {0x0u, 0x50020000u, 13, 183, 60, 10, 0xffffffffu, 130, nullptr, 62},
    {0x0u, 0x50210003u, 77, 181, 161, 100, 0x455u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 13, 199, 56, 10, 0x44cu, 130, nullptr, 63},
    {0x0u, 0x50210103u, 77, 197, 161, 100, 0x44du, 133, nullptr, 0},
    {0x0u, 0x50020000u, 13, 215, 63, 10, 0x454u, 130, nullptr, 64},
    {0x0u, 0x50210003u, 77, 213, 161, 100, 0x451u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 7, 236, 185, 16, 0x417u, 130, nullptr, 40},
    {0x0u, 0x50010000u, 196, 237, 50, 13, 0x416u, 128, nullptr, 41},
};
static const IntlDlgItemSpec kIntlDlgItems_109[] = {
    {0x0u, 0x50020000u, 7, 15, 227, 20, 0x461u, 130, nullptr, 66},
    {0x0u, 0x50020000u, 7, 38, 228, 8, 0x462u, 130, nullptr, 67},
    {0x0u, 0x50210043u, 7, 48, 230, 40, 0x460u, 133, nullptr, 0},
    {0x0u, 0x50020000u, 7, 236, 185, 16, 0x417u, 130, nullptr, 40},
    {0x0u, 0x50010000u, 196, 237, 50, 13, 0x416u, 128, nullptr, 41},
};
static const IntlDlgItemSpec kIntlDlgItems_600[] = {
    {0x0u, 0x50020000u, 5, 8, 242, 24, 0x3feu, 130, nullptr, 69},
    {0x0u, 0x50020000u, 5, 38, 150, 10, 0x403u, 130, nullptr, 70},
    {0x0u, 0x50210103u, 5, 50, 237, 60, 0x3ffu, 133, nullptr, 0},
};
static const IntlDlgItemSpec kIntlDlgItems_700[] = {
    {0x0u, 0x50020000u, 8, 7, 244, 16, 0xffffffffu, 130, nullptr, 72},
    {0x0u, 0x50814001u, 8, 26, 244, 180, 0x83eu, 0, L"SysListView32", 0},
    {0x0u, 0x50020000u, 8, 206, 244, 8, 0x402u, 130, nullptr, 73},
    {0x0u, 0x50020000u, 8, 220, 244, 8, 0xffffffffu, 130, nullptr, 74},
    {0x0u, 0x50012403u, 18, 232, 232, 10, 0x41fu, 128, nullptr, 75},
    {0x0u, 0x50012403u, 18, 244, 232, 10, 0x41eu, 128, nullptr, 76},
    {0x0u, 0x50020000u, 18, 256, 234, 16, 0xbe0u, 130, nullptr, 77},
    {0x0u, 0x50010000u, 144, 276, 50, 14, 0x1u, 128, nullptr, 78},
    {0x0u, 0x50010000u, 202, 276, 50, 14, 0x2u, 128, nullptr, 79},
};
static const IntlDlgItemSpec kIntlDlgItems_800[] = {
    {0x0u, 0x50020000u, 5, 7, 239, 26, 0x41au, 130, nullptr, 81},
    {0x0u, 0x50020000u, 5, 37, 239, 10, 0x835u, 130, nullptr, 82},
    {0x0u, 0x50210103u, 5, 47, 239, 100, 0x41cu, 133, nullptr, 0},
    {0x0u, 0x50020000u, 5, 61, 225, 16, 0x402u, 130, nullptr, 73},
    {0x0u, 0x50012403u, 5, 73, 239, 10, 0x840u, 128, nullptr, 83},
    {0x0u, 0x50010000u, 140, 87, 50, 14, 0x1u, 128, nullptr, 78},
    {0x0u, 0x50010000u, 194, 87, 50, 14, 0x2u, 128, nullptr, 79},
};
static const IntlDlgSpec kIntlDialogs[] = {
    {101, 0x90c001c4u, 0, 0, 264, 236, 1, kIntlDlgItems_101, 25},
    {104, 0x90c001ccu, 0, 0, 264, 236, 16, kIntlDlgItems_104, 8},
    {105, 0x90c001ccu, 0, 0, 252, 254, 25, kIntlDlgItems_105, 33},
    {106, 0x90c001ccu, 0, 0, 252, 254, 42, kIntlDlgItems_106, 21},
    {107, 0x90c001ccu, 0, 0, 252, 254, 49, kIntlDlgItems_107, 19},
    {108, 0x90c001ccu, 0, 0, 252, 254, 56, kIntlDlgItems_108, 25},
    {109, 0x90c001ccu, 0, 0, 252, 254, 65, kIntlDlgItems_109, 5},
    {600, 0x90c001ccu, 0, 0, 264, 236, 68, kIntlDlgItems_600, 3},
    {700, 0x80c800c4u, 0, 0, 260, 298, 71, kIntlDlgItems_700, 9},
    {800, 0x80c800ccu, 0, 0, 250, 107, 80, kIntlDlgItems_800, 7},
};

static const wchar_t* LookupCatalogString(const MuiStringTable* table, UINT id, MuiLanguage lang) {
    for (const MuiStringTable* t = table; t->en != nullptr; ++t) {
        if (t->id != id) continue;
        switch (lang) {
            case MuiLanguage::IT_IT: return t->it;
            case MuiLanguage::FA_IR: return t->fa;
            case MuiLanguage::ES_ES: return t->es;
            case MuiLanguage::FR_FR: return t->fr;
            case MuiLanguage::PT_BR: return t->pt;
            default: return t->en;
        }
    }
    return nullptr;
}

static const wchar_t* GetMuiString(UINT id, MuiLanguage lang) {
    return LookupCatalogString(kMuiStrings, id, lang);
}

static const wchar_t* GetIntlMuiString(UINT id, MuiLanguage lang) {
    return LookupCatalogString(kIntlMuiStrings, id, lang);
}

static const wchar_t* GetIntlDlgString(UINT id, MuiLanguage lang) {
    return LookupCatalogString(kIntlDlgStrings, id, lang);
}

static MuiLanguage DetectMuiLanguage(const wchar_t* locale) {
    if (!locale || !locale[0]) return MuiLanguage::EN_US;
    auto startsWithNoCase = [](const wchar_t* text, const wchar_t* prefix) -> bool {
        while (*prefix) {
            wchar_t a = *text++;
            wchar_t b = *prefix++;
            if (a >= L'A' && a <= L'Z') a += (L'a' - L'A');
            if (b >= L'A' && b <= L'Z') b += (L'a' - L'A');
            if (a != b) return false;
        }
        return true;
    };
    if (startsWithNoCase(locale, L"it")) return MuiLanguage::IT_IT;
    if (startsWithNoCase(locale, L"fa")) return MuiLanguage::FA_IR;
    if (startsWithNoCase(locale, L"es")) return MuiLanguage::ES_ES;
    if (startsWithNoCase(locale, L"fr")) return MuiLanguage::FR_FR;
    if (startsWithNoCase(locale, L"pt")) return MuiLanguage::PT_BR;
    return MuiLanguage::EN_US;
}

static uint16_t GetEmbeddedLanguageId(MuiLanguage language) {
    switch (language) {
        case MuiLanguage::IT_IT: return 0x0410; // MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN)
        case MuiLanguage::FA_IR: return 0x0429; // MAKELANGID(LANG_PERSIAN, SUBLANG_PERSIAN_IRAN)
        case MuiLanguage::ES_ES: return 0x0C0A; // MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MODERN)
        case MuiLanguage::FR_FR: return 0x040C; // MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH)
        case MuiLanguage::PT_BR: return 0x0416; // MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN)
        case MuiLanguage::EN_US:
        default:                 return 0x0409; // MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)
    }
}

static bool BuildEmbeddedStringBlockBytes(const MuiStringTable* table, UINT blockId,
                                          MuiLanguage language, Bytes& output) {
    using namespace pepatch::detail;
    output.clear();
    bool hasString = false;
    for (UINT index = 0; index < 16; ++index) {
        const UINT id = (blockId - 1) * 16 + index;
        const wchar_t* text = LookupCatalogString(table, id, language);
        const size_t length = text ? wcslen(text) : 0;
        if (length > 0xFFFF) return false;
        if (text) hasString = true;
        Push16(output, static_cast<uint16_t>(length));
        for (size_t i = 0; i < length; ++i) {
            Push16(output, static_cast<uint16_t>(text[i]));
        }
    }
    return hasString;
}

static Bytes BuildIntlDialog(const IntlDlgSpec& spec, MuiLanguage language) {
    using namespace pepatch::detail;
    Bytes out;
    Push16(out, 1);              // dlgVer
    Push16(out, 0xFFFF);         // signature (DLGTEMPLATEEX)
    Push32(out, 0);              // helpID
    Push32(out, 0);              // exStyle
    Push32(out, spec.style);     // style
    Push16(out, static_cast<uint16_t>(spec.itemCount));
    Push16(out, static_cast<uint16_t>(spec.x));
    Push16(out, static_cast<uint16_t>(spec.y));
    Push16(out, static_cast<uint16_t>(spec.cx));
    Push16(out, static_cast<uint16_t>(spec.cy));
    Push16(out, 0);              // menu
    Push16(out, 0);              // windowClass
    const wchar_t* dlgTitle = spec.titleId ? GetIntlDlgString(spec.titleId, language) : nullptr;
    PushWStrZ(out, dlgTitle ? dlgTitle : L"");
    Push16(out, 9);              // pointsize = 9
    Push16(out, 0);              // weight = 0
    out.push_back(0);            // italic = 0
    out.push_back(0);            // charset = 0
    PushWStrZ(out, L"Segoe UI");

    for (size_t i = 0; i < spec.itemCount; ++i) {
        while (out.size() % 4) out.push_back(0);
        const IntlDlgItemSpec& item = spec.items[i];
        Push32(out, 0);          // helpID
        Push32(out, item.exStyle);
        Push32(out, item.style);
        Push16(out, static_cast<uint16_t>(item.x));
        Push16(out, static_cast<uint16_t>(item.y));
        Push16(out, static_cast<uint16_t>(item.cx));
        Push16(out, static_cast<uint16_t>(item.cy));
        Push32(out, item.id);
        if (item.clsName) {
            PushWStrZ(out, item.clsName);
        } else {
            Push16(out, 0xFFFF);
            Push16(out, item.clsOrd);
        }
        if (item.titleId) {
            const wchar_t* itemTitle = GetIntlDlgString(item.titleId, language);
            PushWStrZ(out, itemTitle ? itemTitle : L"");
        } else {
            Push16(out, 0);
        }
        Push16(out, 0);          // extraCount = 0
    }
    return out;
}

static Bytes BuildMinimalMessageTable() {
    using namespace pepatch::detail;
    Bytes out;
    Push32(out, 1);              // NumberOfBlocks = 1
    Push32(out, 0x90000001u);    // LowId
    Push32(out, 0x90000001u);    // HighId
    Push32(out, 16);             // OffsetToEntries
    Push16(out, 8);              // Length = 8 bytes
    Push16(out, 1);              // Flags = MESSAGE_RESOURCE_UNICODE
    Push16(out, L'\r');
    Push16(out, 0);
    return out;
}

static Bytes BuildUserLangMui(const Bytes& ulPe, MuiLanguage lang,
                              const wchar_t* langTag, uint16_t extraLcid = 0) {
    Bytes lnRc = pepatch::ExtractResource(ulPe, "MUI", 0, 1);
    Bytes verBlob = pepatch::ExtractResource(ulPe, nullptr, 16, 1);
    Bytes muiRc = pepatch::BuildMuiRcFromLn(lnRc, langTag);
    if (muiRc.empty() || verBlob.empty()) return {};

    pepatch::MuiResourceTypeGroup strGroup{6, {}};
    UINT maxId = 0;
    for (const MuiStringTable* r = kMuiStrings; r->en; ++r)
        if (r->id > maxId) maxId = r->id;
    const UINT maxBlock = maxId / 16 + 1;
    for (UINT b = 1; b <= maxBlock; ++b) {
        Bytes block;
        if (BuildEmbeddedStringBlockBytes(kMuiStrings, b, lang, block))
            strGroup.items.push_back({b, std::move(block)});
    }
    pepatch::MuiResourceTypeGroup msgGroup{11, {{1u, BuildMinimalMessageTable()}}};
    pepatch::MuiResourceTypeGroup verGroup{16, {{1u, std::move(verBlob)}}};

    std::vector<uint16_t> lcids = {0x0409, GetEmbeddedLanguageId(lang)};
    if (extraLcid) lcids.push_back(extraLcid);
    std::sort(lcids.begin(), lcids.end());
    lcids.erase(std::unique(lcids.begin(), lcids.end()), lcids.end());
    return pepatch::BuildMuiDll(muiRc, {strGroup, msgGroup, verGroup}, lcids);
}

static Bytes BuildIntlMui(const Bytes& intlPe, MuiLanguage lang,
                          const wchar_t* langTag, uint16_t extraLcid = 0) {
    Bytes lnRc = pepatch::ExtractResource(intlPe, "MUI", 0, 1);
    Bytes verBlob = pepatch::ExtractResource(intlPe, nullptr, 16, 1);
    Bytes muiRc = pepatch::BuildMuiRcFromLn(lnRc, langTag);
    if (muiRc.empty() || verBlob.empty()) return {};

    pepatch::MuiResourceTypeGroup dlgGroup{5, {}};
    for (const IntlDlgSpec& d : kIntlDialogs) {
        dlgGroup.items.push_back({d.dlgId, BuildIntlDialog(d, lang)});
    }

    pepatch::MuiResourceTypeGroup strGroup{6, {}};
    UINT maxId = 0;
    for (const MuiStringTable* r = kIntlMuiStrings; r->en; ++r)
        if (r->id > maxId) maxId = r->id;
    const UINT maxBlock = maxId / 16 + 1;
    for (UINT b = 1; b <= maxBlock; ++b) {
        Bytes block;
        if (BuildEmbeddedStringBlockBytes(kIntlMuiStrings, b, lang, block))
            strGroup.items.push_back({b, std::move(block)});
    }
    pepatch::MuiResourceTypeGroup msgGroup{11, {{1u, BuildMinimalMessageTable()}}};
    pepatch::MuiResourceTypeGroup verGroup{16, {{1u, std::move(verBlob)}}};

    std::vector<uint16_t> lcids = {0x0409, GetEmbeddedLanguageId(lang)};
    if (extraLcid) lcids.push_back(extraLcid);
    std::sort(lcids.begin(), lcids.end());
    lcids.erase(std::unique(lcids.begin(), lcids.end()), lcids.end());
    return pepatch::BuildMuiDll(muiRc, {dlgGroup, strGroup, msgGroup, verGroup}, lcids);
}

struct MuiLocaleTarget {
    MuiLanguage lang;
    const wchar_t* folder;
    uint16_t lcid;
};

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

struct OutFile { std::wstring rel; Bytes data; };   // rel = path below <storage>\bin
struct Win11Exports { std::vector<std::string> kernel32, winlangdb, bcp47langs; };

static bool HasName(const std::vector<std::string>& v, const std::string& s) {
    return std::find(v.begin(), v.end(), s) != v.end();
}

// Build the forwarder/stub DLL for every function `image` imports from `module`.
// A function the real Windows 11 DLL still exports is forwarded to it; otherwise it is forwarded to
// `fallbackModule` (the 1709 copy) or, when fallbackModule is null, replaced by a stub.
static Bytes MakeShim(const Bytes& image, const char* module, const std::string& shimName,
                      const std::vector<std::string>& win11Exports, const std::string& realModule,
                      const char* fallbackModule, std::vector<std::string>& notes) {
    std::vector<pepatch::ShimExport> ex;
    std::set<std::string> seen;
    for (const std::string& n : pepatch::ListNamedImports(image, module)) {
        if (!seen.insert(n).second) continue;
        pepatch::ShimExport e;
        e.name = n;
        if (HasName(win11Exports, n)) {
            e.forward = realModule + "." + n;
        } else if (fallbackModule) {
            e.forward = std::string(fallbackModule) + "." + n;
            notes.push_back(shimName + ": " + n + " -> " + fallbackModule);
        } else if (n == "NlsEventDataDescCreate") {
            e.code = pepatch::StubReturnArg1();
            notes.push_back(shimName + ": " + n + " -> stub (returns its first argument)");
        } else {
            e.code = pepatch::StubReturn0();
            notes.push_back(shimName + ": " + n + " -> stub (returns 0)");
        }
        ex.push_back(e);
    }
    return pepatch::BuildShimDll(shimName, ex);
}

// intl.cpl: Windows 11's kernel32 no longer exports two Nls ETW helpers it imports, so its kernel32 import is
// renamed to nlsk32.dll, a shim that forwards to the real kernel32 and stubs those two functions.
// Deterministic: the same inputs always give the same bytes (the redirect re-derives and compares them).
static bool BuildIntlFiles(const Bytes& intl, const std::vector<std::string>& kernel32Exports,
                           Bytes& intlOut, Bytes& nlskOut, std::vector<std::string>& notes, std::string& err) {
    Bytes p = intl;
    if (pepatch::PatchImportName(p, "kernel32.dll", "nlsk32.dll") <= 0) { err = "intl.cpl: no kernel32 import to rename"; return false; }
    intlOut = std::move(p);
    nlskOut = MakeShim(intl, "kernel32.dll", "nlsk32.dll", kernel32Exports, "kernel32", nullptr, notes);
    return true;
}

// `in` maps original file name -> verified bytes. Produces everything that goes into <storage>\bin.
static bool BuildOutputs(const std::map<std::wstring, Bytes>& in, const Win11Exports& w11,
                         std::vector<OutFile>& out, std::vector<std::string>& notes, std::string& err) {
    auto get = [&](const wchar_t* n) -> const Bytes* {
        auto it = in.find(n);
        return it == in.end() ? nullptr : &it->second;
    };
    const Bytes *intl = get(L"intl.cpl"), *ul = get(L"UserLanguagesCpl.dll"), *wl = get(L"WinLangdb.dll"),
                *bc = get(L"Bcp47Langs.dll"), *dui = get(L"dui70.dll");
    if (!intl || !ul || !wl || !bc || !dui) { err = "an input file is missing"; return false; }
    out.clear();

    // intl.cpl + its kernel32 shim.
    {
        Bytes patchedIntl, nlsk;
        if (!BuildIntlFiles(*intl, w11.kernel32, patchedIntl, nlsk, notes, err)) return false;
        out.push_back({L"intl.cpl", std::move(patchedIntl)});
        out.push_back({L"nlsk32.dll", std::move(nlsk)});
    }
    // UserLanguagesCpl.dll: private DirectUI copy + language-database DLLs.
    {
        Bytes p = *ul;
        if (pepatch::PatchImportName(p, "DUI70.dll", "DUI76.dll") <= 0 ||
            pepatch::PatchImportName(p, "WinLangdb.dll", "wlshim.dll") <= 0 ||
            pepatch::PatchImportName(p, "Bcp47Langs.dll", "bcshim.dll") <= 0) {
            err = "UserLanguagesCpl.dll: an import to rename was not found";
            return false;
        }
        out.push_back({L"UserLanguagesCpl.dll", p});
        out.push_back({L"wlshim.dll", MakeShim(*ul, "WinLangdb.dll", "wlshim.dll", w11.winlangdb, "WinLangdb", "WinLng17", notes)});
        out.push_back({L"bcshim.dll", MakeShim(*ul, "Bcp47Langs.dll", "bcshim.dll", w11.bcp47langs, "Bcp47Langs", "Bcp17Lng", notes)});
    }
    // The 1709 copies the shims fall back to.
    {
        Bytes p = *wl;
        if (pepatch::PatchImportName(p, "Bcp47Langs.dll", "Bcp17Lng.dll") <= 0) { err = "WinLangdb.dll: no Bcp47Langs import to rename"; return false; }
        out.push_back({L"WinLng17.dll", p});
        out.push_back({L"Bcp17Lng.dll", *bc});
        out.push_back({L"DUI76.dll", *dui});
    }
    // Generate .mui resource files directly from the embedded translations
    // for all 6 supported languages (English, Italian, Persian, Spanish, French, Portuguese).
    for (const MuiLocaleTarget& target : kMuiLocaleTargets) {
        Bytes m1 = BuildIntlMui(*intl, target.lang, target.folder, target.lcid);
        Bytes m2 = BuildUserLangMui(*ul, target.lang, target.folder, target.lcid);
        if (m1.empty() || m2.empty()) {
            err = "failed to build .mui from embedded translations";
            return false;
        }
        out.push_back({std::wstring(target.folder) + L"\\intl.cpl.mui", std::move(m1)});
        out.push_back({std::wstring(target.folder) + L"\\UserLanguagesCpl.dll.mui", std::move(m2)});
    }
    return true;
}
// ---- PURE-END ----
// =============================================================================
// PLATFORM PART: files, download, loading, virtual registration.
// =============================================================================
static const wchar_t* const kAppletClsid = L"{BF782CC9-5A52-4A17-806C-2A894FFEEAC5}";   // Microsoft.Language
static const wchar_t* kLayoutFolderClsid = L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}";
// COM classes implemented by UserLanguagesCpl.dll itself.
static const wchar_t* kDllComClsids[] = {L"{7F75058C-9A21-4F22-A7CA-110205479EF9}",
                                         L"{802E19CB-06A0-445D-8297-7DFEEA406DDD}"};

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

static std::wstring W(const std::string& s) { return std::wstring(s.begin(), s.end()); }   // ASCII only

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
    HINTERNET net = InternetOpenW(L"Windhawk Classic Language Applet Restorer", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
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

// ---- Windows 11's own export lists (read from the files, nothing is loaded) -----------------
static std::vector<std::string> SystemExports(const wchar_t* dll) {
    wchar_t sys[MAX_PATH] = {};
    GetSystemDirectoryW(sys, MAX_PATH);
    Bytes b;
    if (!ReadWholeFile(std::wstring(sys) + L"\\" + dll, b)) return {};
    return pepatch::ListNamedExports(b);
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


// ---- Embedded translation runtime & hooks (performance-info-tools-restorer pattern) ---------
enum class LanguageSettingMode {
    Auto,
    EN_US,
    IT_IT,
    FA_IR,
    ES_ES,
    FR_FR,
    PT_BR
};

static std::atomic<bool> g_forceTranslations{true};
static std::atomic<int> g_selectedLanguageMode{static_cast<int>(LanguageSettingMode::Auto)};
static std::atomic<HMODULE> g_hUserLanguagesCpl{nullptr};
static std::atomic<HMODULE> g_hIntlCpl{nullptr};
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
    ~ScopedResourceUpdateHandle() {
        if (handle) EndUpdateResourceW(handle, TRUE);
    }
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
    ULONG numLangs = 0;
    ULONG cchBuf = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLangs, nullptr, &cchBuf) && cchBuf > 0) {
        std::vector<wchar_t> buf(cchBuf, L'\0');
        if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLangs, buf.data(), &cchBuf) && buf[0]) {
            return DetectMuiLanguage(buf.data());
        }
    }
    const LANGID langId = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(langId)) {
        case LANG_ITALIAN:    return MuiLanguage::IT_IT;
        case LANG_PERSIAN:    return MuiLanguage::FA_IR;
        case LANG_SPANISH:    return MuiLanguage::ES_ES;
        case LANG_FRENCH:     return MuiLanguage::FR_FR;
        case LANG_PORTUGUESE: return MuiLanguage::PT_BR;
        default:              return MuiLanguage::EN_US;
    }
}

static MuiLanguage GetCurrentEmbeddedLanguage() {
    const auto mode = static_cast<LanguageSettingMode>(g_selectedLanguageMode.load());
    switch (mode) {
        case LanguageSettingMode::EN_US: return MuiLanguage::EN_US;
        case LanguageSettingMode::IT_IT: return MuiLanguage::IT_IT;
        case LanguageSettingMode::FA_IR: return MuiLanguage::FA_IR;
        case LanguageSettingMode::ES_ES: return MuiLanguage::ES_ES;
        case LanguageSettingMode::FR_FR: return MuiLanguage::FR_FR;
        case LanguageSettingMode::PT_BR: return MuiLanguage::PT_BR;
        case LanguageSettingMode::Auto:
        default:
            return DetectWindowsUiLanguage();
    }
}

static void LoadLanguageSetting() {
    PCWSTR raw = Wh_GetStringSetting(L"language");
    LanguageSettingMode mode = LanguageSettingMode::Auto;
    if (raw) {
        if (_wcsicmp(raw, L"en-US") == 0 || _wcsicmp(raw, L"en") == 0) {
            mode = LanguageSettingMode::EN_US;
        } else if (_wcsicmp(raw, L"it-IT") == 0 || _wcsicmp(raw, L"it") == 0) {
            mode = LanguageSettingMode::IT_IT;
        } else if (_wcsicmp(raw, L"fa-IR") == 0 || _wcsicmp(raw, L"fa") == 0) {
            mode = LanguageSettingMode::FA_IR;
        } else if (_wcsicmp(raw, L"es-ES") == 0 || _wcsicmp(raw, L"es") == 0) {
            mode = LanguageSettingMode::ES_ES;
        } else if (_wcsicmp(raw, L"fr-FR") == 0 || _wcsicmp(raw, L"fr") == 0) {
            mode = LanguageSettingMode::FR_FR;
        } else if (_wcsicmp(raw, L"pt-BR") == 0 || _wcsicmp(raw, L"pt-PT") == 0 || _wcsicmp(raw, L"pt") == 0) {
            mode = LanguageSettingMode::PT_BR;
        }
        Wh_FreeStringSetting(raw);
    }
    g_selectedLanguageMode.store(static_cast<int>(mode));
}

static const wchar_t* GetEmbeddedTranslation(UINT id) {
    if (!g_forceTranslations.load()) return nullptr;
    const MuiLanguage lang = GetCurrentEmbeddedLanguage();
    if (const wchar_t* text = GetMuiString(id, lang)) return text;
    if (lang != MuiLanguage::EN_US) return GetMuiString(id, MuiLanguage::EN_US);
    return nullptr;
}

static const wchar_t* GetEmbeddedIntlTranslation(UINT id) {
    if (!g_forceTranslations.load()) return nullptr;
    const MuiLanguage lang = GetCurrentEmbeddedLanguage();
    if (const wchar_t* text = GetIntlMuiString(id, lang)) return text;
    if (lang != MuiLanguage::EN_US) return GetIntlMuiString(id, MuiLanguage::EN_US);
    return nullptr;
}

static const wchar_t* GetLocalizedDisplayName() {
    if (const wchar_t* s = GetEmbeddedTranslation(1)) return s;
    if (const wchar_t* s = GetMuiString(1, MuiLanguage::EN_US)) return s;
    return L"Language";
}

static const wchar_t* GetLocalizedInfoTip() {
    if (const wchar_t* s = GetEmbeddedTranslation(2)) return s;
    if (const wchar_t* s = GetMuiString(2, MuiLanguage::EN_US)) return s;
    return L"Customize settings for display languages, input languages, and Regional formats";
}

static bool DisableMuiConfigInPrivateCopy(const std::wstring& path) {
    Bytes buf;
    if (!ReadWholeFile(path, buf) || buf.size() < sizeof(IMAGE_DOS_HEADER)) return false;

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(buf.data());
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return false;
    const size_t ntOff = static_cast<size_t>(dos->e_lfanew);
    if (ntOff + sizeof(IMAGE_NT_HEADERS64) > buf.size()) return false;

    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(buf.data() + ntOff);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        return false;
    }

    const IMAGE_DATA_DIRECTORY& resDir =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
    if (resDir.VirtualAddress == 0 || resDir.Size < sizeof(IMAGE_RESOURCE_DIRECTORY)) {
        return false;
    }

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
    if (resBase == static_cast<size_t>(-1) ||
        resBase + sizeof(IMAGE_RESOURCE_DIRECTORY) > buf.size()) {
        return false;
    }

    const auto* root = reinterpret_cast<const IMAGE_RESOURCE_DIRECTORY*>(buf.data() + resBase);
    const size_t entriesOff = resBase + sizeof(IMAGE_RESOURCE_DIRECTORY);
    const size_t namedCount = root->NumberOfNamedEntries;
    if (entriesOff + namedCount * sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) > buf.size()) {
        return false;
    }

    const auto* entries =
        reinterpret_cast<const IMAGE_RESOURCE_DIRECTORY_ENTRY*>(buf.data() + entriesOff);
    bool patched = false;
    for (size_t i = 0; i < namedCount; ++i) {
        if (!entries[i].NameIsString) continue;
        const size_t strOff = resBase + entries[i].NameOffset;
        if (strOff + sizeof(WORD) + 3 * sizeof(WCHAR) > buf.size()) continue;
        const WORD len = *reinterpret_cast<const WORD*>(buf.data() + strOff);
        auto* chars = reinterpret_cast<WCHAR*>(buf.data() + strOff + sizeof(WORD));
        if (len == 3 && chars[0] == L'M' && chars[1] == L'U' && chars[2] == L'I') {
            chars[0] = L'C';
            patched = true;
            break;
        }
    }
    if (!patched) return false;
    return WriteFileAtomic(path, buf);
}

static bool BuildEmbeddedStringBlock(UINT blockId, MuiLanguage language, std::vector<BYTE>& output) {
    output.clear();
    for (UINT index = 0; index < 16; ++index) {
        const UINT id = (blockId - 1) * 16 + index;
        const wchar_t* text = GetMuiString(id, language);
        const size_t length = text ? wcslen(text) : 0;
        if (length > 0xFFFF) return false;

        const WORD lenWord = static_cast<WORD>(length);
        const auto* lenBytes = reinterpret_cast<const BYTE*>(&lenWord);
        output.insert(output.end(), lenBytes, lenBytes + sizeof(WORD));
        if (length > 0) {
            const auto* strBytes = reinterpret_cast<const BYTE*>(text);
            output.insert(output.end(), strBytes, strBytes + length * sizeof(wchar_t));
        }
    }
    return true;
}

static void CleanupStaleLocalizedResourceModules(const std::wstring& dir,
                                                 const std::wstring& keepPath) {
    if (dir.empty()) return;
    const std::wstring pattern = dir + L"\\UserLanguagesCpl.resources*.dll";
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileW(pattern.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        std::wstring full = dir + L"\\" + fd.cFileName;
        if (!keepPath.empty() && _wcsicmp(full.c_str(), keepPath.c_str()) == 0) continue;
        DeleteFileW(full.c_str());
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);
}

static void ReleaseLocalizedResourceModuleLocked() {
    g_hLocalizedResources.store(nullptr);
    g_localizedResourceLanguage.store(-1);
    g_localizedResourcePath.clear();
}

static void ReleaseLocalizedResourceModule() {
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    ReleaseLocalizedResourceModuleLocked();
}

static bool LocalizedResourceModuleUpToDateLocked() {
    if (g_localizedResourcePath.empty()) return false;
    if (GetFileAttributesW(g_localizedResourcePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    const int wanted = static_cast<int>(GetCurrentEmbeddedLanguage());
    return g_localizedResourceLanguage.load() == wanted;
}

static bool BuildLocalizedResourceModule(const std::wstring& sourceDllPath,
                                         const std::wstring& dir) {
    if (sourceDllPath.empty() || dir.empty()) return false;

    const MuiLanguage selectedLanguage = GetCurrentEmbeddedLanguage();
    const uint32_t seq = g_localizedResourceSeq.fetch_add(1, std::memory_order_relaxed) + 1;
    wchar_t fileName[128] = {};
    swprintf_s(fileName, L"\\UserLanguagesCpl.resources-v1-%d-%lu-%u.dll",
               static_cast<int>(selectedLanguage), GetCurrentProcessId(), seq);
    const std::wstring targetPath = dir + fileName;

    CleanupStaleLocalizedResourceModules(dir, L"");

    if (!CopyFileW(sourceDllPath.c_str(), targetPath.c_str(), FALSE)) return false;
    SetFileAttributesW(targetPath.c_str(), FILE_ATTRIBUTE_NORMAL);

    if (!DisableMuiConfigInPrivateCopy(targetPath)) {
        Wh_Log(L"Localized resource module could not neutralize MUI config: %s",
               targetPath.c_str());
        DeleteFileW(targetPath.c_str());
        return false;
    }

    ScopedResourceUpdateHandle update(BeginUpdateResourceW(targetPath.c_str(), FALSE));
    if (!update) {
        DeleteFileW(targetPath.c_str());
        return false;
    }

    UINT maxId = 0;
    for (const MuiStringTable* row = kMuiStrings; row->en != nullptr; ++row) {
        if (row->id > maxId) maxId = row->id;
    }
    const UINT maxBlockId = (maxId / 16) + 1;

    const WORD targetLangIds[] = {
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        GetEmbeddedLanguageId(selectedLanguage),
    };

    for (UINT blockId = 1; blockId <= maxBlockId; ++blockId) {
        bool blockHasEmbeddedString = false;
        for (UINT index = 0; index < 16; ++index) {
            const UINT id = (blockId - 1) * 16 + index;
            if (GetMuiString(id, MuiLanguage::EN_US) != nullptr) {
                blockHasEmbeddedString = true;
                break;
            }
        }
        if (!blockHasEmbeddedString) continue;

        std::vector<BYTE> block;
        if (!BuildEmbeddedStringBlock(blockId, selectedLanguage, block)) {
            DeleteFileW(targetPath.c_str());
            return false;
        }
        for (size_t i = 0; i < ARRAYSIZE(targetLangIds); ++i) {
            if (i > 0 && targetLangIds[i] == targetLangIds[0]) continue;
            if (!UpdateResourceW(update.Get(), RT_STRING, MAKEINTRESOURCEW(blockId),
                                 targetLangIds[i],
                                 block.empty() ? nullptr : block.data(),
                                 static_cast<DWORD>(block.size()))) {
                DeleteFileW(targetPath.c_str());
                return false;
            }
        }
    }

    if (!update.Commit()) {
        DeleteFileW(targetPath.c_str());
        return false;
    }

    g_localizedResourcePath = targetPath;
    g_localizedResourceLanguage.store(static_cast<int>(selectedLanguage));
    CleanupStaleLocalizedResourceModules(dir, targetPath);
    return true;
}

static HMODULE EnsureLocalizedResourceModuleLoaded() {
    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
    const int wanted = static_cast<int>(GetCurrentEmbeddedLanguage());
    HMODULE existing = g_hLocalizedResources.load();
    if (existing && g_localizedResourceLanguage.load() == wanted) {
        return existing;
    }
    if (existing) {
        ReleaseLocalizedResourceModuleLocked();
    }
    if (!LocalizedResourceModuleUpToDateLocked()) {
        const std::wstring dir = StorageDir();
        if (dir.empty()) return nullptr;
        const std::wstring binDir = dir + L"\\bin";
        const std::wstring ulPath = binDir + L"\\UserLanguagesCpl.dll";
        if (GetFileAttributesW(ulPath.c_str()) == INVALID_FILE_ATTRIBUTES) return nullptr;
        if (!BuildLocalizedResourceModule(ulPath, binDir)) return nullptr;
    }

    HMODULE loaded = LoadLibraryExW(
        g_localizedResourcePath.c_str(), nullptr,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!loaded) {
        Wh_Log(L"Failed to load localized resource module: %s (error=%lu)",
               g_localizedResourcePath.c_str(), GetLastError());
        return nullptr;
    }

    g_hLocalizedResources.store(loaded);
    return loaded;
}

using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
using LoadStringA_t = int(WINAPI*)(HINSTANCE, UINT, LPSTR, int);
// DirectUI::XResourceProvider::Create(HINSTANCE, const wchar_t*, const wchar_t*, const wchar_t*, XResourceProvider**)
using XResourceProviderCreate_t = HRESULT(__fastcall*)(HINSTANCE, const wchar_t*,
                                                       const wchar_t*, const wchar_t*, void**);
using FindResourceExW_t = HRSRC(WINAPI*)(HMODULE, LPCWSTR, LPCWSTR, WORD);
using LoadResource_t = HGLOBAL(WINAPI*)(HMODULE, HRSRC);

static LoadStringW_t LoadStringWOriginalUser32 = nullptr;
static LoadStringW_t LoadStringWOriginalKernelBase = nullptr;
static LoadStringA_t LoadStringAOriginalUser32 = nullptr;
static LoadStringA_t LoadStringAOriginalKernelBase = nullptr;
static XResourceProviderCreate_t XResourceProviderCreateOriginalDui76 = nullptr;
static FindResourceExW_t FindResourceExWOriginal = nullptr;
static LoadResource_t LoadResourceOriginal = nullptr;

// Folder of the mod's generated files (lower case, trailing backslash); set in Wh_ModInit.
static std::wstring g_binDirLower;
static std::mutex g_resModuleCacheMutex;
static std::map<uintptr_t, bool> g_resModuleCache;   // module base -> "is the mod's own UserLanguagesCpl"

// Other handles to the mod's own UserLanguagesCpl.dll (for example a data-file load by the shell) can't be
// recognised by handle, so they are recognised by path, and only inside the mod's own bin folder. The stock
// Windows intl.cpl is never matched. The answer is cached per module base.
static bool IsModUserLanguagesModule(uintptr_t base) {
    if (g_binDirLower.empty()) return false;
    {
        std::lock_guard<std::mutex> l(g_resModuleCacheMutex);
        auto it = g_resModuleCache.find(base);
        if (it != g_resModuleCache.end()) return it->second;
    }
    bool match = false;
    wchar_t path[MAX_PATH] = {};
    if (GetModuleFileNameW(reinterpret_cast<HMODULE>(base), path, ARRAYSIZE(path))) {
        std::wstring p(path);
        for (auto& c : p) c = towlower(c);
        if (p.compare(0, g_binDirLower.size(), g_binDirLower) == 0) {
            const wchar_t* name = p.c_str() + g_binDirLower.size();
            match = wcscmp(name, L"userlanguagescpl.dll") == 0 ||
                    wcsncmp(name, L"userlanguagescpl.resources", 25) == 0;
        }
    }
    std::lock_guard<std::mutex> l(g_resModuleCacheMutex);
    g_resModuleCache[base] = match;
    return match;
}

static bool IsTargetResourceModule(HINSTANCE instance, bool* isIntlOut = nullptr) {
    if (isIntlOut) *isIntlOut = false;
    if (!instance) return false;

    const uintptr_t base = reinterpret_cast<uintptr_t>(instance) & ~static_cast<uintptr_t>(3);
    const auto same = [base](HMODULE m) -> bool {
        return m && (reinterpret_cast<uintptr_t>(m) & ~static_cast<uintptr_t>(3)) == base;
    };

    if (same(g_hUserLanguagesCpl.load()) || same(g_hLocalizedResources.load())) return true;
    if (same(g_hIntlCpl.load())) {   // only ever the mod's own copy (set by the intl.cpl redirect)
        if (isIntlOut) *isIntlOut = true;
        return true;
    }
    return IsModUserLanguagesModule(base);
}

static int HandleLoadStringW(LoadStringW_t orig, HINSTANCE instance, UINT id,
                             LPWSTR buffer, int bufferChars) {
    bool isIntl = false;
    if (IsTargetResourceModule(instance, &isIntl)) {
        if (!isIntl && bufferChars == 0 && buffer != nullptr && g_forceTranslations.load()) {
            if (HMODULE locMod = EnsureLocalizedResourceModuleLoaded()) {
                return orig(locMod, id, buffer, 0);
            }
        }
        if (buffer != nullptr && bufferChars > 0) {
            const wchar_t* translated = isIntl ? GetEmbeddedIntlTranslation(id)
                                               : GetEmbeddedTranslation(id);
            if (translated) {
                const size_t copyLen = std::min<size_t>(
                    wcslen(translated), static_cast<size_t>(bufferChars - 1));
                wmemcpy(buffer, translated, copyLen);
                buffer[copyLen] = L'\0';
                return static_cast<int>(copyLen);
            }
        }
    }
    return orig(instance, id, buffer, bufferChars);
}

static int HandleLoadStringA(LoadStringA_t orig, HINSTANCE instance, UINT id,
                             LPSTR buffer, int bufferChars) {
    bool isIntl = false;
    if (buffer != nullptr && bufferChars > 0 && IsTargetResourceModule(instance, &isIntl)) {
        const wchar_t* translated = isIntl ? GetEmbeddedIntlTranslation(id)
                                           : GetEmbeddedTranslation(id);
        if (translated) {
            int converted = WideCharToMultiByte(CP_ACP, 0, translated, -1,
                                                buffer, bufferChars, nullptr, nullptr);
            if (converted > 0) return converted - 1;
            buffer[bufferChars - 1] = '\0';
            return bufferChars - 1;
        }
    }
    return orig(instance, id, buffer, bufferChars);
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
    bool isIntl = false;
    if (g_forceTranslations.load() && IsTargetResourceModule(instance, &isIntl) && !isIntl) {
        if (HMODULE localizedModule = EnsureLocalizedResourceModuleLoaded()) {
            instance = localizedModule;
        }
    }
    return XResourceProviderCreateOriginalDui76(instance, r1, r2, r3, out);
}

static HRSRC WINAPI FindResourceExWHook(HMODULE hModule, LPCWSTR lpType,
                                        LPCWSTR lpName, WORD wLanguage) {
    if (lpType == RT_STRING && g_forceTranslations.load() && hModule != nullptr) {
        bool isIntl = false;
        if (IsTargetResourceModule(hModule, &isIntl) && !isIntl) {
            if (HMODULE locMod = EnsureLocalizedResourceModuleLoaded()) {
                const WORD langId = GetEmbeddedLanguageId(GetCurrentEmbeddedLanguage());
                if (HRSRC hRes = FindResourceExWOriginal(locMod, lpType, lpName, langId)) {
                    return hRes;
                }
                if (HRSRC hRes = FindResourceExWOriginal(locMod, lpType, lpName, wLanguage)) {
                    return hRes;
                }
            }
        }
    }
    return FindResourceExWOriginal(hModule, lpType, lpName, wLanguage);
}

static HGLOBAL WINAPI LoadResourceHook(HMODULE hModule, HRSRC hResInfo) {
    if (g_forceTranslations.load() && hModule != nullptr && hResInfo != nullptr) {
        bool isIntl = false;
        if (IsTargetResourceModule(hModule, &isIntl) && !isIntl) {
            if (HMODULE locMod = g_hLocalizedResources.load()) {
                MEMORY_BASIC_INFORMATION mbiRes{}, mbiLoc{};
                if (VirtualQuery(hResInfo, &mbiRes, sizeof(mbiRes)) &&
                    VirtualQuery(reinterpret_cast<LPCVOID>(
                                     reinterpret_cast<uintptr_t>(locMod) & ~static_cast<uintptr_t>(3)),
                                 &mbiLoc, sizeof(mbiLoc)) &&
                    mbiRes.AllocationBase == mbiLoc.AllocationBase) {
                    if (HGLOBAL h = LoadResourceOriginal(locMod, hResInfo)) {
                        return h;
                    }
                }
            }
        }
    }
    return LoadResourceOriginal(hModule, hResInfo);
}

static void InstallTranslationHook() {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) hUser32 = LoadLibraryW(L"user32.dll");
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");

    void* pLoadWUser32 = hUser32 ? reinterpret_cast<void*>(GetProcAddress(hUser32, "LoadStringW")) : nullptr;
    void* pLoadWKB = hKernelBase ? reinterpret_cast<void*>(GetProcAddress(hKernelBase, "LoadStringW")) : nullptr;
    if (pLoadWUser32) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringW_t>(pLoadWUser32),
                                       LoadStringWHookUser32, &LoadStringWOriginalUser32);
    }
    if (pLoadWKB && pLoadWKB != pLoadWUser32) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringW_t>(pLoadWKB),
                                       LoadStringWHookKernelBase, &LoadStringWOriginalKernelBase);
    }

    void* pLoadAUser32 = hUser32 ? reinterpret_cast<void*>(GetProcAddress(hUser32, "LoadStringA")) : nullptr;
    void* pLoadAKB = hKernelBase ? reinterpret_cast<void*>(GetProcAddress(hKernelBase, "LoadStringA")) : nullptr;
    if (pLoadAUser32) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringA_t>(pLoadAUser32),
                                       LoadStringAHookUser32, &LoadStringAOriginalUser32);
    }
    if (pLoadAKB && pLoadAKB != pLoadAUser32) {
        WindhawkUtils::SetFunctionHook(reinterpret_cast<LoadStringA_t>(pLoadAKB),
                                       LoadStringAHookKernelBase, &LoadStringAOriginalKernelBase);
    }

    if (hKernelBase) {
        if (auto* pFindEx = reinterpret_cast<FindResourceExW_t>(
                GetProcAddress(hKernelBase, "FindResourceExW"))) {
            WindhawkUtils::SetFunctionHook(pFindEx, FindResourceExWHook, &FindResourceExWOriginal);
        }
        if (auto* pLoadRes = reinterpret_cast<LoadResource_t>(
                GetProcAddress(hKernelBase, "LoadResource"))) {
            WindhawkUtils::SetFunctionHook(pLoadRes, LoadResourceHook, &LoadResourceOriginal);
        }
    }
}

// The applet's DirectUI copy (DUI76.dll) is only loaded by the setup thread, so it is hooked right after
// that load (and not in Wh_ModInit). The system dui70.dll is deliberately not hooked.
static void HookDui76(HMODULE hDui76) {
    static std::atomic<bool> hooked{false};
    if (!hDui76 || g_shuttingDown.load() || hooked.exchange(true)) return;
    if (auto* p = reinterpret_cast<XResourceProviderCreate_t>(GetProcAddress(
            hDui76, "?Create@XResourceProvider@DirectUI@@SAJPEAUHINSTANCE__@@PEBG11PEAPEAV12@@Z"))) {
        WindhawkUtils::SetFunctionHook(p, XResourceProviderCreateHookDui76,
                                       &XResourceProviderCreateOriginalDui76);
        Wh_ApplyHookOperations();
    } else {
        Wh_Log(L"XResourceProvider::Create not found in DUI76.dll");
    }
}

static void RefreshGeneratedMuiFiles(const std::wstring& dlDir, const std::wstring& binDir) {
    Bytes intl, ul;
    if (!ReadWholeFile(dlDir + L"\\intl.cpl", intl) ||
        !ReadWholeFile(dlDir + L"\\UserLanguagesCpl.dll", ul)) {
        return;
    }
    const auto mode = static_cast<LanguageSettingMode>(g_selectedLanguageMode.load());
    const bool overrideAll = g_forceTranslations.load() && mode != LanguageSettingMode::Auto;
    const MuiLanguage forcedLang = GetCurrentEmbeddedLanguage();

    std::vector<OutFile> muiFiles;
    for (const MuiLocaleTarget& target : kMuiLocaleTargets) {
        const MuiLanguage useLang = overrideAll ? forcedLang : target.lang;
        Bytes m1 = BuildIntlMui(intl, useLang, target.folder, target.lcid);
        Bytes m2 = BuildUserLangMui(ul, useLang, target.folder, target.lcid);
        if (!m1.empty() && !m2.empty()) {
            muiFiles.push_back({std::wstring(target.folder) + L"\\intl.cpl.mui", std::move(m1)});
            muiFiles.push_back({std::wstring(target.folder) + L"\\UserLanguagesCpl.dll.mui", std::move(m2)});
        }
    }
    WriteOutputs(binDir, muiFiles);
}

// Loads `path` only if it is byte-identical to `expected`. The file is held open with FILE_SHARE_READ only
// (so nobody can write to, rename or delete it) from before the comparison until the image is mapped; an image
// that is mapped can't be modified either. Reparse points are rejected.
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

static bool LoadPrivateModules(const std::wstring& binDir, const std::vector<OutFile>& outs) {
    // Dependencies first, by full path, so the loader finds them by name when the next one needs them.
    static const wchar_t* order[] = {L"Bcp17Lng.dll", L"WinLng17.dll", L"wlshim.dll",
                                     L"bcshim.dll",   L"DUI76.dll",    L"UserLanguagesCpl.dll"};
    for (const wchar_t* n : order) {
        const Bytes* expected = nullptr;
        for (const OutFile& f : outs) {
            if (_wcsicmp(f.rel.c_str(), n) == 0) { expected = &f.data; break; }
        }
        if (!expected) {
            Wh_Log(L"No expected contents known for %s", n);
            return false;
        }
        const std::wstring p = binDir + L"\\" + n;
        HMODULE h = LoadVerified(p, *expected, LOAD_WITH_ALTERED_SEARCH_PATH, LoadLibraryExW);
        if (!h) {
            Wh_Log(L"Could not load %s", p.c_str());
            return false;
        }
        if (_wcsicmp(n, L"DUI76.dll") == 0) {
            HookDui76(h);
        }
        if (_wcsicmp(n, L"UserLanguagesCpl.dll") == 0) {
            g_hUserLanguagesCpl.store(h);
            std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
            BuildLocalizedResourceModule(p, binDir);
        }
    }
    return true;
}
// ---- virtual registration ---------------------------------------------------------------------
// The applet's registry entries are answered by registry hooks inside explorer.exe and control.exe; the mod writes
// none of them. To the shell and COM these keys behave as if they existed:
//   CLSID\{BF782CC9-...}  (+ DefaultIcon, InProcServer32, Instance\InitPropertyBag, ShellFolder)
//   CLSID\{7F75058C-...}\InProcServer32 and CLSID\{802E19CB-...}\InProcServer32  (classes of UserLanguagesCpl.dll)
//   ...\Explorer\ControlPanel\NameSpace\{BF782CC9-...}, plus that GUID appended to the NameSpace enumeration,
//   and the three GUIDs as values of ...\Shell Extensions\Approved.
//
// A virtual key must still be a handle that every registry API accepts, hooked or not (RegEnumValueW,
// RegQueryValueExA, RegNotifyChangeKeyValue, ...). A made-up pointer is not: it makes explorer / Control Panel
// misbehave or crash. So each virtual key is a real, read-only handle to the root of a private, empty registry hive
// (RegLoadAppKeyW on a file in the mod's storage folder, one per process). That hive is never part of the user's
// registry, holds no data and is deleted when the mod is unloaded (leftovers are removed at the next start).
// If the hive can't be loaded, the virtual keys are simply unavailable (the applet isn't shown): nothing is ever
// written to the user's registry.
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

static bool ContainsKeywordI(LPCWSTR s) {   // "clsid", "controlpanel" or "shell extensions", no allocation
    if (!s) return false;
    for (; *s; ++s) {
        if (*s != L'c' && *s != L'C' && *s != L's' && *s != L'S') continue;
        if (_wcsnicmp(s, L"clsid", 5) == 0 || _wcsnicmp(s, L"controlpanel", 12) == 0 ||
            _wcsnicmp(s, L"shell extensions", 16) == 0)
            return true;
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
// nameOnly (control.exe): just enough for "Microsoft.Language" to be found by name. No server or instance keys, no
// DLL classes and no Approved entries, so nothing can be activated and the applet's DLL is never loaded there.
static std::shared_ptr<const Table> BuildTable(const std::wstring& binDir, bool nameOnly = false) {
    const std::wstring dll = binDir + L"\\UserLanguagesCpl.dll";
    const std::wstring displayName = g_forceTranslations.load() ? std::wstring(GetLocalizedDisplayName())
                                                                : (L"@" + dll + L",-1#immutable1");
    const std::wstring infoTip = g_forceTranslations.load() ? std::wstring(GetLocalizedInfoTip())
                                                            : (L"@" + dll + L",-2#immutable1");
    auto S = [](const wchar_t* n, DWORD type, const std::wstring& s) { return Val{n, type, s, 0}; };
    auto D = [](const wchar_t* n, DWORD d) { return Val{n, REG_DWORD, L"", d}; };

    auto t = std::make_shared<Table>();
    auto add = [&](const std::wstring& rel, std::vector<Val> vals, std::vector<std::wstring> subs) {
        t->keys.push_back(Key{rel, std::move(vals), std::move(subs)});
    };

    const std::wstring root = L"clsid\\" + Lower(kAppletClsid);
    add(root,
        {S(L"", REG_SZ, displayName),
         S(L"System.ControlPanel.Category", REG_SZ, L"6"),
         S(L"{305CA226-D286-468e-B848-2B2E8E697B74} 2", REG_EXPAND_SZ, L"6"),
         S(L"System.Software.TasksFileUrl", REG_SZ, L"Internal"),
         S(L"InfoTip", REG_EXPAND_SZ, infoTip),
         S(L"LocalizedString", REG_EXPAND_SZ, displayName),
         D(L"System.ControlPanel.EnableInSafeMode", 3),
         S(L"System.ApplicationName", REG_SZ, L"Microsoft.Language")},
        nameOnly ? std::vector<std::wstring>{}
                 : std::vector<std::wstring>{L"DefaultIcon", L"InProcServer32", L"Instance", L"ShellFolder"});
    if (nameOnly) {
        add(L"controlpanel\\namespace\\" + Lower(kAppletClsid), {S(L"", REG_SZ, L"Language Settings")}, {});
        return t;
    }
    add(root + L"\\defaulticon", {S(L"", REG_EXPAND_SZ, dll + L",-1")}, {});
    add(root + L"\\inprocserver32",
        {S(L"", REG_EXPAND_SZ, L"%SystemRoot%\\System32\\shdocvw.dll"), S(L"ThreadingModel", REG_SZ, L"Apartment")},
        {});
    add(root + L"\\instance", {S(L"CLSID", REG_SZ, kLayoutFolderClsid)}, {L"InitPropertyBag"});
    add(root + L"\\instance\\initpropertybag", {D(L"ResourceID", 100), S(L"ResourceDLL", REG_EXPAND_SZ, dll)}, {});
    // SFGAO_BROWSABLE is needed for Control Panel to open the item in place
    add(root + L"\\shellfolder", {D(L"Attributes", 0xa80001a0), S(L"WantsParseDisplayName", REG_SZ, L"")}, {});

    std::vector<Val> approved = {S(kAppletClsid, REG_SZ, L"")};
    for (const wchar_t* c : kDllComClsids) {
        const std::wstring r = L"clsid\\" + Lower(c);
        add(r, {}, {L"InProcServer32"});
        add(r + L"\\inprocserver32", {S(L"", REG_SZ, dll), S(L"ThreadingModel", REG_SZ, L"Apartment")}, {});
        approved.push_back(S(c, REG_SZ, L""));
    }
    add(L"controlpanel\\namespace\\" + Lower(kAppletClsid), {S(L"", REG_SZ, L"Language Settings")}, {});
    add(L"shell extensions\\approved", std::move(approved), {});
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
        return OpenKeyImpl(hKey, sub, opt, sam, phk, false);
    } catch (...) {
        Wh_Log(L"exception in RegOpenKeyExW hook");
        return RegOpenKeyExW_Orig(hKey, sub, opt, sam, phk);
    }
}

static LSTATUS WINAPI RegOpenKeyW_Hook(HKEY hKey, LPCWSTR sub, PHKEY phk) {
    try {
        return OpenKeyImpl(hKey, sub, 0, MAXIMUM_ALLOWED, phk, true);
    } catch (...) {
        Wh_Log(L"exception in RegOpenKeyW hook");
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

// ---- COM activation of the applet's classes ----
// The applet is answered from the class factory of the module that implements it: shdocvw.dll for the applet
// (which then reads the virtual "Instance" keys), UserLanguagesCpl.dll for its own two classes.
static const GUID kClsidApplet = {0xBF782CC9, 0x5A52, 0x4A17, {0x80, 0x6C, 0x2A, 0x89, 0x4F, 0xFE, 0xEA, 0xC5}};
static const GUID kClsidDll1 = {0x7F75058C, 0x9A21, 0x4F22, {0xA7, 0xCA, 0x11, 0x02, 0x05, 0x47, 0x9E, 0xF9}};
static const GUID kClsidDll2 = {0x802E19CB, 0x06A0, 0x445D, {0x82, 0x97, 0x7D, 0xFE, 0xEA, 0x40, 0x6D, 0xDD}};
static const GUID kIidClassFactory = {0x00000001, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

static bool IsOurClsid(REFCLSID c) {
    return IsEqualGUID(c, kClsidApplet) || IsEqualGUID(c, kClsidDll1) || IsEqualGUID(c, kClsidDll2);
}

static HRESULT GetOurFactory(REFCLSID clsid, REFIID riid, void** ppv) {
    HMODULE mod = nullptr;
    if (IsEqualGUID(clsid, kClsidApplet)) {
        mod = GetModuleHandleW(L"shdocvw.dll");
        if (!mod) mod = LoadLibraryExW(L"shdocvw.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    } else {
        mod = g_hUserLanguagesCpl.load();
    }
    if (!mod) return REGDB_E_CLASSNOTREG;
    using DllGetClassObject_t = HRESULT(STDAPICALLTYPE*)(REFCLSID, REFIID, LPVOID*);
    auto fn = reinterpret_cast<DllGetClassObject_t>(reinterpret_cast<void*>(GetProcAddress(mod, "DllGetClassObject")));
    if (!fn) return REGDB_E_CLASSNOTREG;
    return fn(clsid, riid, ppv);
}

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstanceCombase_Orig;
static CoCreateInstance_t CoCreateInstanceOle32_Orig;

static HRESULT HandleCoCreateInstance(REFCLSID rclsid, LPUNKNOWN outer, DWORD ctx, REFIID riid, LPVOID* ppv,
                                      CoCreateInstance_t original) {
    try {
        if (ppv && (ctx & CLSCTX_INPROC_SERVER) && IsOurClsid(rclsid) && GetTable()) {
            IClassFactory* factory = nullptr;
            HRESULT hr = GetOurFactory(rclsid, kIidClassFactory, reinterpret_cast<void**>(&factory));
            if (SUCCEEDED(hr) && factory) {
                *ppv = nullptr;
                hr = factory->CreateInstance(outer, riid, ppv);
                factory->Release();
                Wh_Log(L"[com] CoCreateInstance for the applet's class -> 0x%08lX", static_cast<unsigned long>(hr));
                return hr;
            }
            Wh_Log(L"[com] no class factory (0x%08lX), using the normal path", static_cast<unsigned long>(hr));
        }
    } catch (...) {
        Wh_Log(L"exception in CoCreateInstance hook");
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
    Wh_Log(L"Language applet registered virtually (applet keys are not written to the registry)");
}

static void UnpublishVirtualRegistration() {
    vreg::SetTable(nullptr);
    vreg::ForgetAllHandles();
    vreg::ReleaseScratchRoot();
}

// ---- setup thread ---------------------------------------------------------------------------
static void RunSetup() {
    const std::wstring dir = StorageDir();
    if (dir.empty()) { Wh_Log(L"No mod storage path; giving up"); return; }
    const std::wstring dlDir = dir + L"\\dl", binDir = dir + L"\\bin";

    // One setup at a time across all explorer.exe instances.
    HANDLE mtx = CreateMutexW(nullptr, FALSE, L"Windhawk.ClassicLanguageAppletRestorer.Setup");
    if (mtx) {
        HANDLE hs[] = {mtx, g_stopEvent};
        const DWORD w = WaitForMultipleObjects(g_stopEvent ? 2 : 1, hs, FALSE, 120000);
        if (w == WAIT_OBJECT_0 + 1) { CloseHandle(mtx); return; }
    }
    auto done = [&]() { if (mtx) { ReleaseMutex(mtx); CloseHandle(mtx); } };

    EnsureDirs(dlDir);
    EnsureDirs(binDir);

    std::map<std::wstring, Bytes> inputs;
    for (const FileSpec& f : kFiles) {
        Bytes data;
        if (g_shuttingDown.load() || !GetVerifiedFile(dlDir, f, data)) {
            Wh_Log(L"Setup stopped: %s is not available. Check your internet connection and restart explorer.exe.", f.name);
            done();
            return;
        }
        inputs[f.name] = std::move(data);
    }

    Win11Exports w11{SystemExports(L"kernel32.dll"), SystemExports(L"WinLangdb.dll"), SystemExports(L"Bcp47Langs.dll")};
    if (w11.kernel32.empty() || w11.winlangdb.empty() || w11.bcp47langs.empty()) {
        Wh_Log(L"Could not read the export lists of the Windows 11 system DLLs");
        done();
        return;
    }
    std::vector<OutFile> outs;
    std::vector<std::string> notes;
    std::string err;
    if (!BuildOutputs(inputs, w11, outs, notes, err)) {
        Wh_Log(L"Building the applet files failed: %s", W(err).c_str());
        done();
        return;
    }
    for (const std::string& n : notes) Wh_Log(L"  %s", W(n).c_str());
    if (!WriteOutputs(binDir, outs)) { done(); return; }
    done();
    if (static_cast<LanguageSettingMode>(g_selectedLanguageMode.load()) != LanguageSettingMode::Auto &&
        g_forceTranslations.load()) {
        RefreshGeneratedMuiFiles(dlDir, binDir);
    }
    if (g_shuttingDown.load()) return;

    if (!LoadPrivateModules(binDir, outs)) return;
    PublishVirtualRegistration(binDir);
}
// ---- intl.cpl redirect ----------------------------------------------------------------------
static std::atomic<bool> g_redirectIntl{false};
static std::atomic<bool> g_isExplorer{true};
static std::atomic<bool> g_isControl{false};
static std::wstring g_systemIntlPath;   // <System32>\intl.cpl
static std::wstring g_modIntlPath;      // <storage>\bin\intl.cpl
static std::wstring g_modBinDir;        // <storage>\bin
static std::wstring g_modDlDir;         // <storage>\dl

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

// What bin\intl.cpl and bin\nlsk32.dll must contain, derived from the pinned (size + SHA-256) original
// in the dl folder and the export list of this machine's kernel32. Works in every process, with no help
// from the setup thread.
static std::mutex g_intlPairMutex;
static bool g_intlPairReady = false;
static Bytes g_expectedIntl, g_expectedNlsk;

static bool GetExpectedIntlPair(const Bytes** intl, const Bytes** nlsk) {
    std::lock_guard<std::mutex> l(g_intlPairMutex);
    if (!g_intlPairReady) {
        const FileSpec* spec = nullptr;
        for (const FileSpec& f : kFiles) {
            if (wcscmp(f.name, L"intl.cpl") == 0) spec = &f;
        }
        if (!spec || g_modDlDir.empty()) return false;
        Bytes original;
        if (!ReadWholeFile(g_modDlDir + L"\\intl.cpl", original) || original.size() != spec->size ||
            Sha256Hex(original) != spec->sha256) {
            Wh_Log(L"dl\\intl.cpl is missing or doesn't match the pinned hash; intl.cpl is not redirected");
            return false;
        }
        const std::vector<std::string> k32 = SystemExports(L"kernel32.dll");
        std::vector<std::string> notes;
        std::string err;
        if (k32.empty() || !BuildIntlFiles(original, k32, g_expectedIntl, g_expectedNlsk, notes, err)) {
            Wh_Log(L"Could not derive the expected intl.cpl files");
            return false;
        }
        g_intlPairReady = true;
    }
    *intl = &g_expectedIntl;
    *nlsk = &g_expectedNlsk;
    return true;
}

static bool IsSystemIntlCpl(LPCWSTR p) {
    if (!p) return false;
    return _wcsicmp(p, L"intl.cpl") == 0 || _wcsicmp(p, g_systemIntlPath.c_str()) == 0;
}

// Loads the verified copy of the mod's intl.cpl (nlsk32.dll first, so that intl.cpl's renamed import
// resolves to the verified shim). Returns nullptr when anything is missing or doesn't match.
static HMODULE LoadModIntlCpl(DWORD flags) {
    const Bytes *intl = nullptr, *nlsk = nullptr;
    if (g_modIntlPath.empty() || !GetExpectedIntlPair(&intl, &nlsk)) return nullptr;
    if (!LoadVerified(g_modBinDir + L"\\nlsk32.dll", *nlsk, 0, LoadLibraryExW_Original)) return nullptr;
    const DWORD kSearchFlags = LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
                               LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32 |
                               LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
    DWORD f = flags;
    if (!(f & kSearchFlags)) f |= LOAD_WITH_ALTERED_SEARCH_PATH;
    return LoadVerified(g_modIntlPath, *intl, f, LoadLibraryExW_Original);
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    const DWORD kDataFile = LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE |
                            LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE;
    // Only real (code) loads are redirected; resource-only loads keep using the system file.
    if (g_redirectIntl.load() && !(flags & kDataFile) && IsSystemIntlCpl(name)) {
        if (HMODULE m = LoadModIntlCpl(flags)) {
            g_hIntlCpl.store(m);
            Wh_Log(L"intl.cpl redirected (%s)", name);
            return m;
        }
        Wh_Log(L"intl.cpl redirect not possible, using the system one");
    }
    return LoadLibraryExW_Original(name, file, flags);
}

static void InitRedirect() {
    wchar_t sys[MAX_PATH] = {};
    GetSystemDirectoryW(sys, MAX_PATH);
    g_systemIntlPath = std::wstring(sys) + L"\\intl.cpl";
    const std::wstring dir = StorageDir();
    if (!dir.empty()) {
        g_modBinDir = dir + L"\\bin";
        g_modDlDir = dir + L"\\dl";
        g_modIntlPath = g_modBinDir + L"\\intl.cpl";
        g_binDirLower = g_modBinDir + L"\\";
        for (auto& c : g_binDirLower) c = towlower(c);
    }

    if (HMODULE kb = GetModuleHandleW(L"kernelbase.dll")) {
        if (auto p = reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kb, "LoadLibraryExW"))) {
            WindhawkUtils::SetFunctionHook(p, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
        }
    }
}

// ---- unhide the applet (in-memory patch of ONE string in shell32.dll) ----
// Newer Windows builds hide this applet from Control Panel through a built-in list inside shell32.dll that is keyed
// by the applet's GUID string ("::{GUID}"). While the mod is enabled, that one string is blanked in memory; the
// original bytes are remembered and put back in Wh_ModUninit, so disabling the mod needs no explorer restart.
// Tested: patching shell32.dll alone is enough (a CompareStringOrdinal hook alone is not, and windows.storage.dll
// doesn't need to be touched), so only shell32.dll is patched.
static const wchar_t kHideGuidStr[] = L"::{BF782CC9-5A52-4A17-806C-2A894FFEEAC5}";
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
            for (BYTE* q = reinterpret_cast<BYTE*>((reinterpret_cast<uintptr_t>(rs) + 1) & ~uintptr_t(1)); q + nb <= re; q += 2) {
                const wchar_t* w = reinterpret_cast<const wchar_t*>(q);
                if (w[0] != L':') continue;
                if (ignoreCase ? _wcsnicmp(w, needle, nlen) == 0 : wcsncmp(w, needle, nlen) == 0) return q;
            }
        }
        p = re;
    }
    return nullptr;
}

static void BlankHideString(HMODULE m, const wchar_t* modName) {
    if (!m) { Wh_Log(L"[unhide] %s not loaded", modName); return; }
    BYTE* a = FindWideInModule(m, kHideGuidStr, false);
    if (!a) a = FindWideInModule(m, kHideGuidStr, true);
    if (!a) { Wh_Log(L"[unhide] %s: Language string not found (already blanked?)", modName); return; }
    const size_t nb = wcslen(kHideGuidStr) * sizeof(wchar_t);
    DWORD old = 0;
    if (!VirtualProtect(a, nb, PAGE_READWRITE, &old)) { Wh_Log(L"[unhide] %s: VirtualProtect failed (%lu)", modName, GetLastError()); return; }
    StringPatch sp;
    sp.addr = a;
    sp.orig.assign(a, a + nb);
    memset(a, 0, nb);
    DWORD tmp = 0;
    VirtualProtect(a, nb, old, &tmp);
    g_patches.push_back(std::move(sp));
    Wh_Log(L"[unhide] %s: Language string blanked at RVA 0x%llx", modName, (unsigned long long)(a - reinterpret_cast<BYTE*>(m)));
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

// =============================================================================
// Windhawk entry points
// =============================================================================
BOOL Wh_ModInit() {
    if (!IsRunningAsAmd64()) {
        Wh_Log(L"Not an x64 process; mod not started");
        return FALSE;
    }
    g_forceTranslations.store(Wh_GetIntSetting(L"forceTranslations") != 0);
    LoadLanguageSetting();
    g_redirectIntl.store(Wh_GetIntSetting(L"redirectIntlCpl") != 0);

    wchar_t exe[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    const wchar_t* exeName = wcsrchr(exe, L'\\');
    exeName = exeName ? exeName + 1 : exe;
    g_isExplorer.store(_wcsicmp(exeName, L"explorer.exe") == 0);
    const bool isControl = _wcsicmp(exeName, L"control.exe") == 0;
    g_isControl.store(isControl);

    // rundll32.exe only needs the intl.cpl redirect; control.exe also finds the applet by name.
    if (!g_isExplorer.load() && !isControl && !g_redirectIntl.load()) return FALSE;

    InitRedirect();

    if (!g_isExplorer.load()) {
        if (isControl) {
            // control.exe usually hands the item over to explorer.exe, so it only has to find it: registry hooks and
            // the name part of the table, published right away and only if the applet's files exist. No setup, no
            // COM hook, no DLL.
            const std::wstring dir = StorageDir();
            if (!dir.empty() &&
                GetFileAttributesW((dir + L"\\bin\\UserLanguagesCpl.dll").c_str()) != INVALID_FILE_ATTRIBUTES) {
                vreg::CleanupStaleHives();
                if (vreg::InstallHooks(false)) PublishVirtualRegistration(dir + L"\\bin", true);
            }
        }
        return TRUE;   // control.exe / rundll32.exe: nothing else (no download, no patching, no file cleanup)
    }
    vreg::CleanupStaleHives();

    g_shuttingDown.store(false);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    BlankHideString(GetModuleHandleW(L"shell32.dll"), L"shell32.dll");
    InstallTranslationHook();
    if (!vreg::InstallHooks()) return FALSE;
    try {
        g_setupThread.emplace([] { try { RunSetup(); } catch (...) { Wh_Log(L"Setup threw an exception"); } });
    } catch (...) {
        Wh_Log(L"Could not start the setup thread");
    }
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    try {
        g_forceTranslations.store(Wh_GetIntSetting(L"forceTranslations") != 0);
        LoadLanguageSetting();
        g_redirectIntl.store(Wh_GetIntSetting(L"redirectIntlCpl") != 0);
        if (!g_isExplorer.load()) {
            // Helper processes: reload, so Wh_ModInit decides again whether the mod is needed here.
            if (reload) *reload = TRUE;
            return TRUE;
        }
        const std::wstring dir = StorageDir();
        if (!dir.empty()) {
            const std::wstring dlDir = dir + L"\\dl", binDir = dir + L"\\bin";
            const std::wstring ulPath = binDir + L"\\UserLanguagesCpl.dll";
            if (GetFileAttributesW(ulPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                {
                    std::lock_guard<std::mutex> lock(g_localizedResourceMutex);
                    if (!LocalizedResourceModuleUpToDateLocked()) {
                        ReleaseLocalizedResourceModuleLocked();
                        BuildLocalizedResourceModule(ulPath, binDir);
                    }
                }
                RefreshGeneratedMuiFiles(dlDir, binDir);
                if (vreg::GetTable()) {
                    PublishVirtualRegistration(binDir);   // refresh the display name for the new language
                }
            }
        }
        if (reload) *reload = FALSE;
        return TRUE;
    } catch (...) {
        if (reload) *reload = TRUE;
        return TRUE;
    }
}

// Runs before Windhawk removes the hooks. The setup thread calls Wh_SetFunctionHook / Wh_ApplyHookOperations
// (HookDui76), which must not happen after this function returns, so it is stopped and joined here.
void Wh_ModBeforeUninit() {
    if (!g_isExplorer.load()) return;
    g_shuttingDown.store(true);
    if (g_stopEvent) SetEvent(g_stopEvent);
    CancelInFlightDownload();
    if (g_setupThread && g_setupThread->joinable()) g_setupThread->join();
    g_setupThread.reset();
}

void Wh_ModUninit() {
    if (!g_isExplorer.load()) {   // helper processes must not touch files
        if (g_isControl.load()) UnpublishVirtualRegistration();
        return;
    }
    RestoreHideStrings();
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }

    ReleaseLocalizedResourceModule();
    {
        std::lock_guard<std::mutex> l(g_resModuleCacheMutex);
        g_resModuleCache.clear();
    }
    UnpublishVirtualRegistration();   // the loaded DLLs stay mapped until explorer.exe exits: shell objects may still use them

    const std::wstring dir = StorageDir();
    if (!dir.empty()) {
        const std::wstring binDir = dir + L"\\bin", dlDir = dir + L"\\dl";
        CleanupStaleLocalizedResourceModules(binDir, L"");
        if (Wh_GetIntSetting(L"keepFilesOnDisable") == 0) {
            static const wchar_t* bins[] = {
                L"intl.cpl", L"nlsk32.dll", L"UserLanguagesCpl.dll", L"wlshim.dll",
                L"bcshim.dll", L"WinLng17.dll", L"Bcp17Lng.dll", L"DUI76.dll"
            };
            for (const wchar_t* b : bins) DeleteFileW((binDir + L"\\" + b).c_str());
            for (const MuiLocaleTarget& t : kMuiLocaleTargets) {
                const std::wstring locDir = binDir + L"\\" + t.folder;
                DeleteFileW((locDir + L"\\intl.cpl.mui").c_str());
                DeleteFileW((locDir + L"\\UserLanguagesCpl.dll.mui").c_str());
                RemoveDirectoryW(locDir.c_str());
            }
            for (const FileSpec& f : kFiles) DeleteFileW((dlDir + L"\\" + f.name).c_str());
            RemoveDirectoryW(binDir.c_str());
            RemoveDirectoryW(dlDir.c_str());
            Wh_Log(L"Files removed (any still in use are left and retried on a later unload)");
        }
    }
}
