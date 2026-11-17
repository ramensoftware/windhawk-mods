// ==WindhawkMod==
// @id              explorer-no-help
// @name            Explorer: No Help
// @description     Removes the ribbon Help (?) button from File Explorer and blocks F1 in Explorer only
// @version         1.0
// @author          vt-idiot
// @github          https://github.com/vt-idiot
// @license         MIT
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer: No Help

For Windows 10 File Explorer (the ribbon one).

Before:
![Before](https://raw.githubusercontent.com/vt-idiot/windhawk-mods/refs/heads/reeeeeeeee/before.png)

After:
![After](https://raw.githubusercontent.com/vt-idiot/windhawk-mods/refs/heads/reeeeeeeee/after.png)

- **Hide Help button**: Explorer's ribbon ships pre-compiled inside
  `SystemResources\ExplorerFrame.dll.mun` (resource `UIFILE/EXPLORER_RIBBON`).
  When the ribbon framework loads it, this mod hands back a patched copy with
  the `tabs_help` block (the `?` button) removed, the same result as if
  Microsoft had compiled the ribbon without a help button. Nothing on disk is
  touched.
- **Block F1**: swallows bare F1 inside explorer.exe (folder windows, desktop,
  taskbar). Ctrl/Shift/Alt+F1 still go through, so Ctrl+F1 still collapses
  the ribbon. F1 in every other app is left alone.

Restart Explorer after enabling the mod or changing its settings.

If anything about the ribbon binary doesn't look exactly as expected, the
patch bails out and logs why, and Explorer gets the original ribbon.

Special thanks to d0gkiller87, whose F1 Blocker the F1 part copies, and to
DarkShadow44’s reverse-engineering notes on UIRibbon.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideHelpButton: true
  $name: Hide the ribbon Help (?) button
  $description: Takes effect after restarting Explorer.
- blockF1: true
  $name: Block F1 in Explorer
  $description: Bare F1 only. Ctrl/Shift/Alt+F1 still work (Ctrl+F1 collapses the ribbon).
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------------------
// Compiled ribbon (.bml / UIFILE) patcher. Pure function, no Windows APIs.
//
// Layout (reverse-engineered; see github.com/DarkShadow44/UIRibbon-Reversing):
//   header ("SCBin", total length) | strings | command resources |
//   top-level entries (command table, pointer to index) | root node |
//   u32 section length | data blocks (u16 length each, back to back, found by
//   absolute offset) | index (u16 size, u16, then 10-byte entries; ends at EOF)
//
// The help button is the root node's last child: an array of type 5
// ("tabs_help") holding one button node. We drop that child, turn the freed
// bytes into a well-formed unreferenced data block, delete the button's
// index entry, and shift the index 10 bytes later (padding the gap with
// another filler block), so the total size and every other offset stay put.
// ---------------------------------------------------------------------------

namespace RibbonPatch {

constexpr size_t kBad = static_cast<size_t>(-1);

struct Reader {
    const uint8_t* b;
    size_t n;
    bool ok = true;
    uint8_t U1(size_t o) {
        if (o >= n) { ok = false; return 0; }
        return b[o];
    }
    uint16_t U2(size_t o) {
        if (o + 2 > n || o + 2 < o) { ok = false; return 0; }
        return static_cast<uint16_t>(b[o] | (b[o + 1] << 8));
    }
    uint32_t U4(size_t o) {
        if (o + 4 > n || o + 4 < o) { ok = false; return 0; }
        return U2(o) | (static_cast<uint32_t>(U2(o + 2)) << 16);
    }
};

inline size_t TidLen(Reader& r, size_t o) {
    switch (r.U1(o)) {
        case 2: return 5;
        case 3: return 3;
        case 4: case 9: case 43: return 2;
    }
    return kBad;
}

inline size_t EntryEnd(Reader& r, size_t o, int depth) {
    if (depth > 64 || !r.ok || o >= r.n) return kBad;
    size_t end = kBad;
    switch (r.U1(o)) {
        case 0x01: {  // property
            uint8_t len = r.U1(o + 1);
            if (len == 1) {
                size_t t = TidLen(r, o + 3);
                if (t != kBad) end = o + 3 + t;
            } else if (len == 4) {
                end = o + 8;
            }
            break;
        }
        case 0x18: {  // array
            uint16_t count = r.U2(o + 3);
            size_t p = o + 5;
            for (uint16_t i = 0; i < count && p != kBad; i++) {
                p = EntryEnd(r, p, depth + 1);
            }
            end = p;
            break;
        }
        case 0x16: {  // node (carries its own size)
            uint16_t size = r.U2(o + 5);
            if (size >= 8) end = o + size;
            break;
        }
        case 0x3e:  // reference to a data block
            end = o + 5;
            break;
        case 0x3b:  // size-definition info
            switch (r.U1(o + 1)) {
                case 9: end = o + 3; break;
                case 3: end = o + 4; break;
                case 2: end = o + 6; break;
            }
            break;
    }
    if (!r.ok || end == kBad || end > r.n) return kBad;
    return end;
}

inline void Put16(std::vector<uint8_t>& v, uint16_t x) {
    v.push_back(x & 0xFF);
    v.push_back(x >> 8);
}
inline void Put32(std::vector<uint8_t>& v, uint32_t x) {
    Put16(v, x & 0xFFFF);
    Put16(v, x >> 16);
}
inline void Set16(std::vector<uint8_t>& v, size_t o, uint16_t x) {
    v[o] = x & 0xFF;
    v[o + 1] = x >> 8;
}
inline void Set32(std::vector<uint8_t>& v, size_t o, uint32_t x) {
    Set16(v, o, x & 0xFFFF);
    Set16(v, o + 2, x >> 16);
}

// A single well-formed tree entry exactly k bytes long.
inline bool Filler(std::vector<uint8_t>& v, size_t k) {
    if (k == 8) {  // long property
        const uint8_t p[] = {1, 4, 0x3f, 0, 0, 0, 0, 0};
        v.insert(v.end(), p, p + 8);
        return true;
    }
    if (k < 5) return false;
    size_t s = k - 5;  // array header is 5 bytes; fill with 6/5-byte props
    for (size_t t6 = 0; t6 * 6 <= s; t6++) {
        if ((s - t6 * 6) % 5) continue;
        size_t t5 = (s - t6 * 6) / 5;
        const uint8_t hdr[] = {0x18, 1, 0x3e};
        v.insert(v.end(), hdr, hdr + 3);
        Put16(v, static_cast<uint16_t>(t6 + t5));
        const uint8_t p6[] = {1, 1, 0x0b, 3, 0, 0};
        const uint8_t p5[] = {1, 1, 0x0b, 4, 0};
        for (size_t i = 0; i < t6; i++) v.insert(v.end(), p6, p6 + 6);
        for (size_t i = 0; i < t5; i++) v.insert(v.end(), p5, p5 + 5);
        return true;
    }
    return false;
}

// Returns true and fills `out` (same size as input) if the help button was
// removed. Otherwise returns false and sets `why`.
inline bool RemoveHelpButton(const uint8_t* b, size_t n,
                             std::vector<uint8_t>& out, const wchar_t** why,
                             uint16_t* helpIdOut) {
#define BAIL(msg) do { *why = msg; return false; } while (0)
    Reader r{b, n};
    if (n < 32 || memcmp(b + 9, "SCBin", 5) != 0) BAIL(L"not a ribbon binary");
    if (r.U4(14) != n) BAIL(L"header length != resource size");

    size_t o = 19 + static_cast<size_t>(r.U4(19));  // skip strings
    uint32_t resCount = r.U4(o);
    o += 4;
    for (uint32_t i = 0; i < resCount && r.ok; i++) {
        o += 4;
        uint8_t k = r.U1(o++);
        for (uint8_t j = 0; j < k && r.ok; j++) {
            uint8_t t = r.U1(o);
            o += 5 + ((t >= 3 && t <= 6) ? 2 : 0);
        }
    }
    if (!r.ok) BAIL(L"truncated resource table");

    size_t indexPtr = kBad;
    for (int guard = 0; r.ok && r.U1(o) != 0x16; guard++) {
        if (guard > 16) BAIL(L"too many top-level entries");
        uint8_t t = r.U1(o);
        if (t == 0x10) {
            uint32_t len = r.U4(o + 1);
            if (len < 5) BAIL(L"bad command table");
            o += len;
        } else if (t == 0x0d) {
            indexPtr = o + 3;
            o += 7;
        } else {
            BAIL(L"unknown top-level entry");
        }
    }
    if (!r.ok || indexPtr == kBad) BAIL(L"no root node / index pointer");

    const size_t root = o;
    const uint16_t rootSize = r.U2(root + 5);
    const uint8_t kids = r.U1(root + 7);
    const size_t rootEnd = root + rootSize;
    size_t p = root + 8, last = kBad;
    for (uint8_t i = 0; i < kids && p != kBad; i++) {
        last = p;
        p = EntryEnd(r, p, 0);
    }
    if (p == kBad || p != rootEnd || last == kBad) BAIL(L"root node doesn't parse");

    const size_t helpArr = last;
    if (r.U1(helpArr) != 0x18 || r.U1(helpArr + 2) != 5)
        BAIL(L"no help button (last root child isn't tabs_help)");
    if (r.U2(helpArr + 3) != 1) BAIL(L"tabs_help doesn't hold exactly one control");
    const size_t help = helpArr + 5;
    if (r.U1(help) != 0x16 || EntryEnd(r, help, 0) != rootEnd)
        BAIL(L"unexpected help control shape");

    // Command ID of the help control = its property of type 0.
    bool haveId = false;
    uint16_t hid = 0;
    size_t q = help + 8;
    for (uint8_t i = 0; i < r.U1(help + 7) && q != kBad; i++) {
        if (r.U1(q) == 1 && r.U1(q + 1) == 1 && r.U1(q + 2) == 0) {
            haveId = true;
            switch (r.U1(q + 3)) {
                case 2: hid = static_cast<uint16_t>(r.U4(q + 4)); break;
                case 3: hid = r.U2(q + 4); break;
                default: hid = r.U1(q + 4); break;
            }
        }
        q = EntryEnd(r, q, 0);
    }
    if (!haveId) BAIL(L"help control has no command id");

    const size_t idx = r.U4(indexPtr);
    const uint16_t idxSize = r.U2(idx);
    if (!r.ok || idx + idxSize != n || idxSize < 4 || (idxSize - 4) % 10)
        BAIL(L"index doesn't end at EOF");
    if (rootEnd + r.U4(rootEnd) != idx) BAIL(L"data section length mismatch");

    const size_t entries = (idxSize - 4) / 10;
    size_t hit = kBad, hits = 0;
    for (size_t i = 0; i < entries; i++) {
        size_t e = idx + 4 + i * 10;
        if (r.U2(e) == hid) {
            hits++;
            hit = e;
        }
    }
    if (hits != 1 || r.U2(hit + 6) != help) BAIL(L"help command index entry not as expected");
    if (!r.ok) BAIL(L"truncated");

    const size_t gap = rootEnd - helpArr;  // bytes the help block occupied
    out.clear();
    out.reserve(n);
    out.insert(out.end(), b, b + helpArr);
    out[root + 7] = kids - 1;
    Set16(out, root + 5, static_cast<uint16_t>(rootSize - gap));
    Put32(out, static_cast<uint32_t>((idx + 10) - helpArr));  // section length
    Put16(out, static_cast<uint16_t>(gap));                    // filler block
    if (!Filler(out, gap - 2)) BAIL(L"can't build filler block");
    out.insert(out.end(), b + rootEnd + 4, b + idx);          // real data blocks
    Put16(out, 10);                                            // filler block
    Filler(out, 8);
    Put16(out, static_cast<uint16_t>(idxSize - 10));
    Put16(out, r.U2(idx + 2));
    for (size_t i = 0; i < entries; i++) {
        size_t e = idx + 4 + i * 10;
        if (e != hit) out.insert(out.end(), b + e, b + e + 10);
    }
    Set32(out, indexPtr, static_cast<uint32_t>(idx + 10));
    if (out.size() != n) BAIL(L"size changed (bug)");
    *helpIdOut = hid;
    return true;
#undef BAIL
}

}  // namespace RibbonPatch

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

std::atomic<bool> g_hideHelpButton{true};
std::atomic<bool> g_blockF1{true};

void LoadSettings() {
    g_hideHelpButton = Wh_GetIntSetting(L"hideHelpButton") != 0;
    g_blockF1 = Wh_GetIntSetting(L"blockF1") != 0;
}

// ---------------------------------------------------------------------------
// F1 blocking (same approach as d0gkiller87's F1 Blocker, Explorer only)
// ---------------------------------------------------------------------------

using TranslateAcceleratorW_t = decltype(&TranslateAcceleratorW);
TranslateAcceleratorW_t TranslateAcceleratorW_Original;

int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg) {
    if (g_blockF1 && lpMsg && lpMsg->message == WM_KEYDOWN &&
        lpMsg->wParam == VK_F1 && !(GetKeyState(VK_CONTROL) & 0x8000) &&
        !(GetKeyState(VK_SHIFT) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000)) {
        return TRUE;  // "handled" -> keypress is eaten, help never fires
    }
    return TranslateAcceleratorW_Original(hWnd, hAccTable, lpMsg);
}

// ---------------------------------------------------------------------------
// Serve a patched copy of the ribbon binary from LoadResource
// ---------------------------------------------------------------------------

SRWLOCK g_cacheLock = SRWLOCK_INIT;
// original resource data -> patched copy (nullptr = checked, leave alone).
// Copies are never freed: live ribbons keep pointers into them.
std::unordered_map<const void*, void*>* g_cache;

using LoadResource_t = decltype(&LoadResource);
LoadResource_t LoadResource_Original;

HGLOBAL WINAPI LoadResource_Hook(HMODULE hModule, HRSRC hResInfo) {
    HGLOBAL res = LoadResource_Original(hModule, hResInfo);
    if (!res || !hResInfo || !g_hideHelpButton) return res;

    DWORD size = SizeofResource(hModule, hResInfo);
    const auto* data = static_cast<const uint8_t*>(res);
    if (size < 32 || memcmp(data + 9, "SCBin", 5) != 0) return res;

    AcquireSRWLockShared(&g_cacheLock);
    auto it = g_cache->find(res);
    bool known = it != g_cache->end();
    void* cached = known ? it->second : nullptr;
    ReleaseSRWLockShared(&g_cacheLock);
    if (known) return cached ? cached : res;

    std::vector<uint8_t> patched;
    const wchar_t* why = L"";
    uint16_t helpId = 0;
    void* copy = nullptr;
    if (RibbonPatch::RemoveHelpButton(data, size, patched, &why, &helpId)) {
        copy = HeapAlloc(GetProcessHeap(), 0, size);
        if (copy) memcpy(copy, patched.data(), size);
    }

    AcquireSRWLockExclusive(&g_cacheLock);
    auto ins = g_cache->emplace(res, copy);
    if (!ins.second) {  // another thread got there first
        if (copy) HeapFree(GetProcessHeap(), 0, copy);
        copy = ins.first->second;
    }
    ReleaseSRWLockExclusive(&g_cacheLock);

    if (ins.second) {
        if (copy) {
            Wh_Log(L"Ribbon binary (%u bytes): removed help button, command 0x%04X", size, helpId);
        } else {
            Wh_Log(L"Ribbon binary (%u bytes) left alone: %s", size, why);
        }
    }
    return copy ? copy : res;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();
    g_cache = new std::unordered_map<const void*, void*>();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) user32 = LoadLibraryW(L"user32.dll");
    void* tav = user32 ? (void*)GetProcAddress(user32, "TranslateAcceleratorW") : nullptr;
    if (tav) {
        Wh_SetFunctionHook(tav, (void*)TranslateAcceleratorW_Hook,
                           (void**)&TranslateAcceleratorW_Original);
    } else {
        Wh_Log(L"TranslateAcceleratorW not found, F1 blocking disabled");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    void* lr = kernelBase ? (void*)GetProcAddress(kernelBase, "LoadResource") : nullptr;
    if (lr) {
        Wh_SetFunctionHook(lr, (void*)LoadResource_Hook, (void**)&LoadResource_Original);
    } else {
        Wh_Log(L"kernelbase!LoadResource not found, help button stays");
    }

    return TRUE;
}

void Wh_ModUninit() {
    // Hooks are removed by Windhawk. Patched copies stay allocated on purpose:
    // ribbons that are already open still point into them.
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
