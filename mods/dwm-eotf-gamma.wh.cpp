// clang-format off
// ==WindhawkMod==
// @id              dwm-eotf-gamma
// @name            Windows SDR to HDR Tonemapping Fix
// @description     Replaces DWM's sRGB EOTF with a pure power-law gamma for SDR-to-HDR tone mapping
// @version         1.0
// @author          millerpb
// @github          https://github.com/millerpb
// @license         GPL-3.0-or-later
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows SDR to HDR Tonemapping Fix

Viewing SDR content while using HDR in Windows causes washed out darker tones
like shadows. This corrects it so that Windows uses a standard gamma curve. It
should not affect native HDR content like games and movies.

Simple, accurate sRGB with a simple power-law gamma curve that you can control.

![Comparison Image](https://i.imgur.com/GwegdeU.png)

See examples and read more on the [win11hdr-srgb-to-gamma2.2-icm
GitHub](https://github.com/dylanraga/win11hdr-srgb-to-gamma2.2-icm), an
alternative icc based solution by dylanraga

## DWM EOTF Gamma Curve

When Windows HDR is enabled, DWM converts SDR content to HDR scRGB using the
sRGB EOTF (a ~2.2 power curve with a linear toe segment near black). This mod
patches the DXBC shader bytecode embedded in `dwmcore.dll` at DWM startup,
replacing the sRGB curve with a simple power-law gamma. This gives you direct
control over the SDR-to-HDR tone mapping without permanently modifying any files
on disk.

Based on [dwm_eotf](https://github.com/ledoge/dwm_eotf) by ledoge (GPL-3.0).

## How it works

When DWM starts, Windhawk injects this mod early, ideally before Direct3D
initializes, so the patched bytecode is what D3D compiles from. The mod walks
`dwmcore.dll`'s read-only PE sections looking for DXBC shader blobs that contain
all four sRGB EOTF float constants. When found, those constants are replaced
with the equivalent pure power-law values and the shader checksum is
recalculated so D3D accepts the modified bytecode.

When the mod is unloaded while DWM is running (e.g. Windhawk disabled or
settings changed), all patched bytes are restored in memory. Note that the
unload callback does not run at process exit, so no restore occurs at Windows
shutdown; this is harmless since the process is terminating anyway.

## Important: enable injection into dwm.exe

`dwm.exe` is a critical system process. Windhawk does not inject into it by
default — you must explicitly allow it in **Windhawk's advanced settings**:

1. Open Windhawk → **Advanced settings**
2. In the **Process inclusion list**, add `dwm.exe`
3. Click **Save**

Without this step the mod will be silently ignored.

![Windhawk advanced settings](https://i.imgur.com/LRhREtJ.png)

## Usage

1. Enable **Windows HDR** in display settings.
2. Install and enable this mod.
3. Select your preferred gamma value in the settings below.
4. **Log off and back in** (or otherwise restart DWM) for the change to take
effect.
5. To change the gamma value later, update the setting and restart DWM again.

## Gamma reference

| Value | Description |
|-------|-------------|
| 1.8   | Brighter highlights; old Apple/Mac standard |
| 2.0   | Moderately bright midpoint |
| 2.2   | Closest to the perceptual average of sRGB |
| 2.4   | sRGB peak exponent |
| 2.6   | Darker midtones, punchier contrast |

## Known limitations

- **Only applies when Windows HDR is enabled.** With HDR off, DWM does not use
  this code path.
- **Gamma changes require a DWM restart.** D3D compiles the shader bytecode once
  at startup. Patching the bytecode after that has no effect until DWM recreates
  its device (which happens on log off/on, or on display reconfiguration).
- **Chromium-based browsers and Electron apps** (Chrome, Edge, VS Code, Discord,
  etc.) switch between DWM's SDR compositing path and a direct scRGB path
  depending on tab/window content. Each
  transition passes one or two frames through DWM's patched SDR shader, which
  can produce a brief tone-mapping flicker. A per-app exclusion list is not
  feasible: the shader bytecode is patched once at DWM startup before Direct3D
  compiles it, the compiled shader objects are global across all windows, and
  patching or unpatching at runtime has no effect on shaders that are already
  compiled. The flicker is driven by Chrome's own mode-switching and cannot be
  suppressed from the DWM side.
- If the mod logs "No target shaders found" after a Windows Update, the shader
  structure in `dwmcore.dll` has changed (verified against 10.0.26100.8521).
  Please file an issue.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- GammaCurve: "2.2"
  $name: Gamma Curve
  $description: >
    Power-law gamma exponent used for SDR-to-HDR EOTF conversion.
    Log off and back in after changing for the new value to take effect.
  $options:
    - "1.8": "1.8 (bright, old Mac standard)"
    - "2.0": "2.0"
    - "2.2": "2.2 (sRGB average)"
    - "2.4": "2.4 (sRGB peak)"
    - "2.6": "2.6 (dark / high contrast)"
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windhawk_utils.h>
#include <windows.h>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <vector>

// =============================================================================
// DXBC Checksum — AMD DXBCChecksum (modified MD5 used by Microsoft for DXBC)
// Source: https://github.com/GPUOpen-Archive/common-src-ShaderUtils
// Copyright 2008-2016 Advanced Micro Devices, Inc. All rights reserved.
// Free for all — MD5 algorithm derived from RSA Data Security, Inc. (1990).
// =============================================================================

typedef uint32_t DX_UINT4;

struct MD5_CTX_DX {
    DX_UINT4 i[2];
    DX_UINT4 buf[4];
    unsigned char in[64];
};

static const unsigned char kMD5Padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define DX_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define DX_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define DX_H(x, y, z) ((x) ^ (y) ^ (z))
#define DX_I(x, y, z) ((y) ^ ((x) | (~z)))
#define DX_RL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define DX_FF(a, b, c, d, x, s, ac)                        \
    {                                                      \
        (a) += DX_F((b), (c), (d)) + (x) + (DX_UINT4)(ac); \
        (a) = DX_RL((a), (s));                             \
        (a) += (b);                                        \
    }
#define DX_GG(a, b, c, d, x, s, ac)                        \
    {                                                      \
        (a) += DX_G((b), (c), (d)) + (x) + (DX_UINT4)(ac); \
        (a) = DX_RL((a), (s));                             \
        (a) += (b);                                        \
    }
#define DX_HH(a, b, c, d, x, s, ac)                        \
    {                                                      \
        (a) += DX_H((b), (c), (d)) + (x) + (DX_UINT4)(ac); \
        (a) = DX_RL((a), (s));                             \
        (a) += (b);                                        \
    }
#define DX_II(a, b, c, d, x, s, ac)                        \
    {                                                      \
        (a) += DX_I((b), (c), (d)) + (x) + (DX_UINT4)(ac); \
        (a) = DX_RL((a), (s));                             \
        (a) += (b);                                        \
    }

static void DX_MD5Transform(DX_UINT4* buf, DX_UINT4* in) {
    DX_UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];
    DX_FF(a, b, c, d, in[0], 7, 3614090360u);
    DX_FF(d, a, b, c, in[1], 12, 3905402710u);
    DX_FF(c, d, a, b, in[2], 17, 606105819u);
    DX_FF(b, c, d, a, in[3], 22, 3250441966u);
    DX_FF(a, b, c, d, in[4], 7, 4118548399u);
    DX_FF(d, a, b, c, in[5], 12, 1200080426u);
    DX_FF(c, d, a, b, in[6], 17, 2821735955u);
    DX_FF(b, c, d, a, in[7], 22, 4249261313u);
    DX_FF(a, b, c, d, in[8], 7, 1770035416u);
    DX_FF(d, a, b, c, in[9], 12, 2336552879u);
    DX_FF(c, d, a, b, in[10], 17, 4294925233u);
    DX_FF(b, c, d, a, in[11], 22, 2304563134u);
    DX_FF(a, b, c, d, in[12], 7, 1804603682u);
    DX_FF(d, a, b, c, in[13], 12, 4254626195u);
    DX_FF(c, d, a, b, in[14], 17, 2792965006u);
    DX_FF(b, c, d, a, in[15], 22, 1236535329u);
    DX_GG(a, b, c, d, in[1], 5, 4129170786u);
    DX_GG(d, a, b, c, in[6], 9, 3225465664u);
    DX_GG(c, d, a, b, in[11], 14, 643717713u);
    DX_GG(b, c, d, a, in[0], 20, 3921069994u);
    DX_GG(a, b, c, d, in[5], 5, 3593408605u);
    DX_GG(d, a, b, c, in[10], 9, 38016083u);
    DX_GG(c, d, a, b, in[15], 14, 3634488961u);
    DX_GG(b, c, d, a, in[4], 20, 3889429448u);
    DX_GG(a, b, c, d, in[9], 5, 568446438u);
    DX_GG(d, a, b, c, in[14], 9, 3275163606u);
    DX_GG(c, d, a, b, in[3], 14, 4107603335u);
    DX_GG(b, c, d, a, in[8], 20, 1163531501u);
    DX_GG(a, b, c, d, in[13], 5, 2850285829u);
    DX_GG(d, a, b, c, in[2], 9, 4243563512u);
    DX_GG(c, d, a, b, in[7], 14, 1735328473u);
    DX_GG(b, c, d, a, in[12], 20, 2368359562u);
    DX_HH(a, b, c, d, in[5], 4, 4294588738u);
    DX_HH(d, a, b, c, in[8], 11, 2272392833u);
    DX_HH(c, d, a, b, in[11], 16, 1839030562u);
    DX_HH(b, c, d, a, in[14], 23, 4259657740u);
    DX_HH(a, b, c, d, in[1], 4, 2763975236u);
    DX_HH(d, a, b, c, in[4], 11, 1272893353u);
    DX_HH(c, d, a, b, in[7], 16, 4139469664u);
    DX_HH(b, c, d, a, in[10], 23, 3200236656u);
    DX_HH(a, b, c, d, in[13], 4, 681279174u);
    DX_HH(d, a, b, c, in[0], 11, 3936430074u);
    DX_HH(c, d, a, b, in[3], 16, 3572445317u);
    DX_HH(b, c, d, a, in[6], 23, 76029189u);
    DX_HH(a, b, c, d, in[9], 4, 3654602809u);
    DX_HH(d, a, b, c, in[12], 11, 3873151461u);
    DX_HH(c, d, a, b, in[15], 16, 530742520u);
    DX_HH(b, c, d, a, in[2], 23, 3299628645u);
    DX_II(a, b, c, d, in[0], 6, 4096336452u);
    DX_II(d, a, b, c, in[7], 10, 1126891415u);
    DX_II(c, d, a, b, in[14], 15, 2878612391u);
    DX_II(b, c, d, a, in[5], 21, 4237533241u);
    DX_II(a, b, c, d, in[12], 6, 1700485571u);
    DX_II(d, a, b, c, in[3], 10, 2399980690u);
    DX_II(c, d, a, b, in[10], 15, 4293915773u);
    DX_II(b, c, d, a, in[1], 21, 2240044497u);
    DX_II(a, b, c, d, in[8], 6, 1873313359u);
    DX_II(d, a, b, c, in[15], 10, 4264355552u);
    DX_II(c, d, a, b, in[6], 15, 2734768916u);
    DX_II(b, c, d, a, in[13], 21, 1309151649u);
    DX_II(a, b, c, d, in[4], 6, 4149444226u);
    DX_II(d, a, b, c, in[11], 10, 3174756917u);
    DX_II(c, d, a, b, in[2], 15, 718787259u);
    DX_II(b, c, d, a, in[9], 21, 3951481745u);
    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

static void DX_MD5Init(MD5_CTX_DX* ctx) {
    ctx->i[0] = ctx->i[1] = 0;
    ctx->buf[0] = 0x67452301u;
    ctx->buf[1] = 0xefcdab89u;
    ctx->buf[2] = 0x98badcfeu;
    ctx->buf[3] = 0x10325476u;
}

static void DX_MD5Update(MD5_CTX_DX* ctx,
                         const unsigned char* data,
                         unsigned int len) {
    int mdi = (int)((ctx->i[0] >> 3) & 0x3F);
    if ((ctx->i[0] + ((DX_UINT4)len << 3)) < ctx->i[0])
        ctx->i[1]++;
    ctx->i[0] += (DX_UINT4)len << 3;
    ctx->i[1] += (DX_UINT4)len >> 29;
    while (len--) {
        ctx->in[mdi++] = *data++;
        if (mdi == 0x40) {
            DX_UINT4 tmp[16];
            for (unsigned i = 0, ii = 0; i < 16; i++, ii += 4)
                tmp[i] = ((DX_UINT4)ctx->in[ii + 3] << 24) |
                         ((DX_UINT4)ctx->in[ii + 2] << 16) |
                         ((DX_UINT4)ctx->in[ii + 1] << 8) |
                         (DX_UINT4)ctx->in[ii];
            DX_MD5Transform(ctx->buf, tmp);
            mdi = 0;
        }
    }
}

// Computes the DXBC-variant MD5 checksum for a shader blob.
// pData must point to the start of the DXBC header; dwSize is the full shader
// size. The checksum is written into dwHash[4].
static void CalcDXBCChecksum(const BYTE* pData, DWORD dwSize, DWORD dwHash[4]) {
    static const DWORD kHashOffset =
        0x14;  // skip magic(4) + checksum(16) = 20 bytes
    MD5_CTX_DX ctx;
    DX_MD5Init(&ctx);

    dwSize -= kHashOffset;
    pData += kHashOffset;

    DWORD numBits = dwSize * 8;
    DWORD fullChunksSize = dwSize & 0xFFFFFFC0u;

    DX_MD5Update(&ctx, pData, fullChunksSize);

    DWORD lastChunkSize = dwSize - fullChunksSize;
    DWORD paddingSize = 64 - lastChunkSize;
    const BYTE* lastChunk = pData + fullChunksSize;

    if (lastChunkSize >= 56) {
        DX_MD5Update(&ctx, lastChunk, lastChunkSize);
        DX_MD5Update(&ctx, kMD5Padding, paddingSize);
        DX_UINT4 blk[16] = {};
        blk[0] = numBits;
        blk[15] = (numBits >> 2) | 1;
        DX_MD5Transform(ctx.buf, blk);
    } else {
        DX_MD5Update(&ctx, (const unsigned char*)&numBits, 4);
        if (lastChunkSize)
            DX_MD5Update(&ctx, lastChunk, lastChunkSize);
        lastChunkSize += sizeof(DWORD);
        paddingSize -= sizeof(DWORD);
        memcpy(&ctx.in[lastChunkSize], kMD5Padding, paddingSize);
        ((DX_UINT4*)ctx.in)[15] = (numBits >> 2) | 1;
        DX_UINT4 blk[16];
        memcpy(blk, ctx.in, 64);
        DX_MD5Transform(ctx.buf, blk);
    }

    memcpy(dwHash, ctx.buf, 4 * sizeof(DWORD));
}

// =============================================================================
// DXBC structure layout
// =============================================================================

#pragma pack(push, 1)
struct DXBCHeader {
    char magic[4];      // "DXBC"
    DWORD checksum[4];  // DXBC-MD5 checksum (bytes 4–19)
    DWORD reserved;     // always 1
    DWORD size;         // total blob size in bytes
};
#pragma pack(pop)

static_assert(sizeof(DXBCHeader) == 28, "DXBCHeader size mismatch");
static const size_t kDXBCHeaderSize = sizeof(DXBCHeader);  // 28

// =============================================================================
// Patch state
// =============================================================================

// Records the original bytes of a patched memory range so we can restore them.
struct PatchRecord {
    BYTE* addr;
    std::vector<BYTE> original;
};

static std::vector<PatchRecord> g_patches;

// Known DXBC checksums for the four sRGB-EOTF target shaders in dwmcore.dll.
// Verified against dwmcore.dll 10.0.26100.8521 (Windows 11 24H2).
//
// Used as the primary selector: dwmcore.dll contains dozens of other shaders
// that also contain sRGB constants, so constant-only detection alone
// over-patches. A content-based fallback (accept leaf blobs containing all four
// sRGB splats exactly once, require exactly four such blobs) was considered and
// deferred: it would survive Windows Updates that recompile these shaders but
// requires non-trivial cross-section state tracking. Left as a future
// improvement.
//
// If these change after a Windows Update, please file an issue. The
// per-constant replacement counts logged at INFO level help identify the new
// shader blobs.
static const BYTE kKnownChecksums[4][16] = {
    {0x96, 0xe6, 0xd1, 0x58, 0x92, 0x55, 0xec, 0xcd, 0x1d, 0xd7, 0xd4, 0xdb,
     0xec, 0x54, 0xd2, 0x85},
    {0x21, 0x26, 0xb0, 0x37, 0xc1, 0xa2, 0xfb, 0xdd, 0xe3, 0x55, 0xb6, 0xe6,
     0xdd, 0x9c, 0xaf, 0x3c},
    {0x2c, 0x89, 0x26, 0xff, 0xe2, 0x29, 0xf0, 0x5d, 0x96, 0x7c, 0x72, 0x66,
     0x8d, 0xc3, 0xad, 0xdb},
    {0xf6, 0x93, 0xbf, 0xbb, 0xaf, 0x24, 0xb3, 0xd9, 0x36, 0x63, 0x54, 0xbe,
     0x88, 0x98, 0xa7, 0xf5},
};

// =============================================================================
// Memory helpers
// =============================================================================

// Write bytes to an arbitrary (potentially read-only) address in the current
// process by temporarily relaxing page protection.
static bool WriteMemorySafe(void* dst, const void* src, size_t size) {
    DWORD oldProt = 0;
    if (!VirtualProtect(dst, size, PAGE_READWRITE, &oldProt))
        return false;
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProt, &oldProt);  // restore; ignore error
    return true;
}

// =============================================================================
// Shader patching logic
// =============================================================================

// The sRGB EOTF is encoded in shader bytecode as four float constants,
// each stored as three identical consecutive 32-bit values (a vec3 splat):
//
//   2.4f        — the exponent
//   0.04045f    — the linear-toe threshold
//   0.055000f   — the linear-toe offset
//   0.94786733f — the linear-toe scale (1.0 / 1.055)
//
// Replacing them with {gamma, 0, 0, 1} converts the piecewise sRGB curve into
// the simple power-law: out = in ^ gamma.

static const float kSrgbConsts[4] = {2.4f, 0.04045f, 0.055000f, 0.94786733f};
static const float kPatchConsts[4] = {0.0f, 0.0f, 0.0f, 1.0f};
// Index 0 (the exponent) is replaced with the user-chosen gamma, not
// kPatchConsts[0].

// Searches 'buf' (a copy of a shader blob) for the sRGB vec3-splat patterns and
// patches them in-place. Fills counts[4] with the number of replacements made
// for each constant. Returns the number of constants that had at least one
// match (0–4). Callers must require == 4 to ensure all-or-nothing patching, and
// should log the individual counts: some constants (e.g. 0.055) appear in both
// decode and encode paths, so a count > 1 indicates unexpected overlap that
// would corrupt unintended code.
static int PatchShaderBuf(BYTE* buf, DWORD size, float gamma, int counts[4]) {
    int found = 0;
    for (int ci = 0; ci < 4; ci++) {
        float src = kSrgbConsts[ci];
        float dst = (ci == 0) ? gamma : kPatchConsts[ci];
        float pat[3] = {src, src, src};
        counts[ci] = 0;
        // Search from after the header; stop where a full 12-byte match would
        // overflow. DXBC constants are DWORD-aligned, so step by 4 for
        // correctness and speed.
        for (size_t j = kDXBCHeaderSize; j + sizeof(float) * 3 <= size;
             j += 4) {
            if (memcmp(pat, buf + j, sizeof(pat)) != 0)
                continue;
            float rep[3] = {dst, dst, dst};
            memcpy(buf + j, rep, sizeof(rep));
            counts[ci]++;
        }
        if (counts[ci] > 0)
            found++;
    }
    return found;
}

// Returns true if the blob contains at least one valid nested DXBC blob.
// dwmcore.dll embeds the target leaf shaders inside larger container blobs.
// Container blobs have sub-blobs; leaf shaders do not. Distinguishing them
// prevents patching a container as a leaf, which would leave internal
// sub-blob checksums stale.
static bool HasDXBCSubBlob(const DXBCHeader* hdr) {
    const BYTE* base = (const BYTE*)hdr;
    // DXBC sub-blobs are at DWORD-aligned offsets within a container
    // (verified).
    for (size_t k = kDXBCHeaderSize; k + kDXBCHeaderSize <= hdr->size; k += 4) {
        const auto* inner = (const DXBCHeader*)(base + k);
        if (inner->magic[0] == 'D' && inner->magic[1] == 'X' &&
            inner->magic[2] == 'B' && inner->magic[3] == 'C' &&
            inner->reserved == 1 && inner->size >= (DWORD)kDXBCHeaderSize &&
            k + inner->size <= hdr->size)
            return true;
    }
    return false;
}

// Returns true if CalcDXBCChecksum reproduces the blob's own embedded checksum.
// Used as a pre-flight check before writing a new checksum into a container
// blob that is not on the known-checksum allowlist.
static bool ChecksumRoundTrips(const DXBCHeader* hdr) {
    DWORD verify[4];
    CalcDXBCChecksum((const BYTE*)hdr, hdr->size, verify);
    return memcmp(verify, hdr->checksum, 16) == 0;
}

// =============================================================================
// Region scanner
// =============================================================================

// Scans a read-only committed memory region for DXBC shader blobs, patches any
// that match the known checksums, and appends restoration records to g_patches.
// Accumulates matched-shader bits into matchedMask (bit i = kKnownChecksums[i]
// patched). Returns the number of individual shaders patched.
//
// "Big shader" handling: dwmcore.dll embeds the target shaders as sub-blobs
// inside larger DXBC container blobs. The container's own checksum must be
// recalculated after its inner shaders are patched.
static int ScanRegionForShaders(BYTE* region,
                                size_t regionSize,
                                float gamma,
                                unsigned& matchedMask) {
    int numPatched = 0;

    DXBCHeader* bigShader = nullptr;  // enclosing container blob, if any
    bool bigPatched = false;          // did we patch any sub-shader inside it?
    size_t containerPatchBase =
        0;  // g_patches index when the current container was accepted
    unsigned containerMatchedBits =
        0;  // matchedMask bits set for the current container's leaves

    // Finalizes a container whose inner shaders have been patched: saves its
    // original checksum, recomputes it, and writes it back. If the write fails,
    // rolls back all leaf patches belonging to this container and clears their
    // bits from matchedMask so Wh_ModInit's all-or-nothing check stays
    // accurate.
    auto FinalizeContainer = [&](DXBCHeader* container) -> bool {
        PatchRecord bigRec;
        bigRec.addr = (BYTE*)container + 4;
        bigRec.original.assign(bigRec.addr, bigRec.addr + 16);
        g_patches.push_back(std::move(bigRec));

        DWORD newCk[4];
        CalcDXBCChecksum((const BYTE*)container, container->size, newCk);
        if (!WriteMemorySafe((BYTE*)container + 4, newCk, 16)) {
            g_patches.pop_back();  // remove the checksum record we just pushed
            size_t toRollBack = g_patches.size() - containerPatchBase;
            Wh_Log(
                L"  VirtualProtect failed for container at %p — rolling back "
                L"%zu leaf patch(es)",
                (void*)container, toRollBack);
            while (g_patches.size() > containerPatchBase) {
                auto& rec = g_patches.back();
                WriteMemorySafe(rec.addr, rec.original.data(),
                                rec.original.size());
                g_patches.pop_back();
                numPatched--;
            }
            matchedMask &=
                ~containerMatchedBits;  // un-mark the rolled-back shaders
            return false;
        }
        Wh_Log(L"  Fixed container checksum at %p", (void*)container);
        return true;
    };

    // All DXBC blobs in dwmcore.dll's .rdata are DWORD-aligned (verified by
    // scanning the binary: 0 of 404 occurrences are unaligned). Step by 4 for
    // speed.
    for (size_t i = 0; i + kDXBCHeaderSize <= regionSize; i += 4) {
        // If we have advanced past the end of the current container blob,
        // finalize it if we patched any inner shaders.
        if (bigShader && region + i >= (BYTE*)bigShader + bigShader->size) {
            if (bigPatched)
                FinalizeContainer(bigShader);
            bigShader = nullptr;
            bigPatched = false;
            containerMatchedBits = 0;
        }

        auto* hdr = (DXBCHeader*)(region + i);

        // Validate DXBC magic and reserved field.
        if (hdr->magic[0] != 'D' || hdr->magic[1] != 'X' ||
            hdr->magic[2] != 'B' || hdr->magic[3] != 'C')
            continue;
        if (hdr->reserved != 1)
            continue;

        // Basic size sanity: must fit within the remaining region.
        if (hdr->size < (DWORD)kDXBCHeaderSize || i + hdr->size > regionSize)
            continue;

        // Container blobs embed sub-shaders as nested DXBC blobs; leaf shaders
        // do not. Track containers so we can fix their checksum after inner
        // shaders are patched.
        if (HasDXBCSubBlob(hdr)) {
            if (!bigShader) {
                // Pre-flight: verify our checksum routine reproduces this
                // blob's own hash before we commit to overwriting it. An
                // unlisted container whose hash doesn't round-trip is skipped
                // to avoid silent corruption.
                if (!ChecksumRoundTrips(hdr)) {
                    Wh_Log(
                        L"  Container at %p: checksum did not round-trip, "
                        L"skipping",
                        (void*)hdr);
                    i +=
                        hdr->size -
                        4;  // loop will add 4 more, landing just after the blob
                    continue;
                }
                bigShader = hdr;
                containerPatchBase = g_patches.size();
                containerMatchedBits = 0;
            } else {
                // A nested container inside the current one: multi-level
                // nesting is not supported. Skip past the inner blob so its
                // leaves are not patched without a proper container fixup.
                Wh_Log(
                    L"  Nested container at %p inside %p — skipping "
                    L"unsupported nesting depth",
                    (void*)hdr, (void*)bigShader);
                i += hdr->size -
                     4;  // loop will add 4 more, landing just after the blob
            }
            continue;
        }

        // Leaf blob — match against the known-checksum allowlist.
        // Many other DWM shaders also contain sRGB constants, so constant-only
        // detection would over-patch unrelated shaders; checksums select the
        // correct four. The constant search below acts as a sanity check.
        int matchIdx = -1;
        for (int k = 0; k < 4; k++) {
            if (memcmp(kKnownChecksums[k], hdr->checksum, 16) == 0) {
                matchIdx = k;
                break;
            }
        }
        if (matchIdx < 0)
            continue;

        // Verify our checksum implementation can reproduce this blob's exact
        // hash before rewriting it. A wrong hash here makes D3D reject the
        // shader, which in DWM means a compositor failure rather than a
        // cosmetic glitch.
        if (!ChecksumRoundTrips(hdr)) {
            Wh_Log(L"  Shader #%d at %p: checksum did not round-trip, skipping",
                   matchIdx, (void*)hdr);
            continue;
        }

        // Verify the leaf actually lies within the tracked container's bounds
        // before attributing the patch to that container. A positional
        // false-positive (a blob that looked like a container but preceded the
        // real one) would otherwise get its checksum overwritten for leaves it
        // doesn't contain.
        bool insideContainer =
            bigShader && (BYTE*)hdr >= (BYTE*)bigShader &&
            (BYTE*)hdr + hdr->size <= (BYTE*)bigShader + bigShader->size;

        // Checksum matched — copy the blob, patch the sRGB constants, verify
        // all four were found, then recompute the DXBC checksum.
        std::vector<BYTE> patched(hdr->size);
        memcpy(patched.data(), hdr, hdr->size);

        int counts[4] = {};
        int constsFound =
            PatchShaderBuf(patched.data(), hdr->size, gamma, counts);
        if (constsFound < 4) {
            Wh_Log(
                L"  Shader #%d: checksum matched but only %d/4 sRGB constants "
                L"found",
                matchIdx, constsFound);
            continue;
        }
        bool countsOk = true;
        for (int ci = 0; ci < 4; ci++) {
            Wh_Log(L"  Shader #%d: constant %d replaced %d time(s)", matchIdx,
                   ci, counts[ci]);
            if (counts[ci] != 1)
                countsOk = false;
        }
        if (!countsOk) {
            Wh_Log(
                L"  Shader #%d: expected each constant exactly once — skipping "
                L"to avoid corruption",
                matchIdx);
            continue;
        }

        DWORD newCk[4];
        CalcDXBCChecksum(patched.data(), hdr->size, newCk);
        memcpy(patched.data() + 4, newCk, 16);

        // Save the original bytes for later restoration.
        PatchRecord rec;
        rec.addr = (BYTE*)hdr;
        rec.original.assign(rec.addr, rec.addr + hdr->size);

        // Write the patched shader back into the (read-only) memory region.
        if (!WriteMemorySafe((BYTE*)hdr, patched.data(), hdr->size)) {
            Wh_Log(L"  VirtualProtect failed for shader #%d at %p", matchIdx,
                   (void*)hdr);
            continue;
        }

        g_patches.push_back(std::move(rec));
        numPatched++;
        matchedMask |= 1u << matchIdx;
        if (insideContainer) {
            containerMatchedBits |= 1u << matchIdx;
            bigPatched = true;
        }

        Wh_Log(L"  Patched shader #%d at %p (size %lu)", matchIdx, (void*)hdr,
               hdr->size);
    }

    // Handle a container blob that reaches the very end of the region.
    if (bigShader && bigPatched)
        FinalizeContainer(bigShader);

    return numPatched;
}

// Restores all patched memory regions to their original bytes.
static void RestoreAllPatches() {
    // Iterate in reverse so sub-shader records (added first) are restored after
    // their container checksum records (added last), giving a consistent final
    // state.
    for (auto it = g_patches.rbegin(); it != g_patches.rend(); ++it) {
        if (!WriteMemorySafe(it->addr, it->original.data(),
                             it->original.size()))
            Wh_Log(L"  VirtualProtect failed restoring %zu bytes at %p",
                   it->original.size(), (void*)it->addr);
    }
    g_patches.clear();
}

// =============================================================================
// Windhawk callbacks
// =============================================================================

BOOL Wh_ModInit() {
    // Resolve the gamma setting via lookup table to avoid locale-dependent
    // wcstof behaviour: a locale with ',' as the decimal separator would
    // silently parse "2.2" as 2.0, which passes a range check without any
    // visible error.
    static const struct {
        const wchar_t* str;
        float val;
    } kGammaOptions[] = {{L"1.8", 1.8f},
                         {L"2.0", 2.0f},
                         {L"2.2", 2.2f},
                         {L"2.4", 2.4f},
                         {L"2.6", 2.6f}};
    float gamma = 2.2f;
    {
        auto gammaSetting = WindhawkUtils::StringSetting::make(L"GammaCurve");
        for (const auto& opt : kGammaOptions) {
            if (wcscmp(gammaSetting, opt.str) == 0) {
                gamma = opt.val;
                break;
            }
        }
    }

    Wh_Log(L"Target gamma = %.1f", (double)gamma);

    // Locate dwmcore.dll. It is listed in dwm.exe's PE import directory and is
    // therefore mapped by the OS loader before any code runs in the process,
    // so GetModuleHandleW is guaranteed to succeed without needing a
    // LoadLibraryExW hook. (Verified: "dwmcore.dll" appears in dwm.exe's
    // raw import-table bytes at a fixed offset.)
    HMODULE hDwmcore = GetModuleHandleW(L"dwmcore.dll");
    if (!hDwmcore) {
        Wh_Log(L"dwmcore.dll not found — unexpected, please file an issue");
        return FALSE;
    }

    // Determine the module's address range from its PE header.
    auto* dos = (IMAGE_DOS_HEADER*)hDwmcore;
    auto* nt = (IMAGE_NT_HEADERS*)((BYTE*)hDwmcore + dos->e_lfanew);
    BYTE* modBase = (BYTE*)hDwmcore;
    size_t modSize = nt->OptionalHeader.SizeOfImage;

    Wh_Log(L"dwmcore.dll base %p, image size %zu", (void*)modBase, modSize);

    // Walk PE sections instead of VirtualQuery: deterministic regardless of
    // page-protection fragmentation from other mods or the loader.
    unsigned matchedMask = 0;
    int totalPatched = 0;
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (WORD s = 0; s < nt->FileHeader.NumberOfSections; s++, sec++) {
        // Scan readable, non-writable, non-executable sections only (e.g.
        // .rdata).
        if (!(sec->Characteristics & IMAGE_SCN_MEM_READ))
            continue;
        if (sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)
            continue;
        if (sec->Characteristics & IMAGE_SCN_MEM_WRITE)
            continue;
        if (sec->Misc.VirtualSize == 0)
            continue;

        Wh_Log(L"Scanning section at %p, size %lu",
               (void*)(modBase + sec->VirtualAddress), sec->Misc.VirtualSize);
        totalPatched +=
            ScanRegionForShaders(modBase + sec->VirtualAddress,
                                 sec->Misc.VirtualSize, gamma, matchedMask);
    }

    if (matchedMask == 0) {
        // No target shaders found. The DXBC blobs live in dwmcore.dll's .rdata
        // and are present regardless of HDR state, so matchedMask == 0 means
        // the known checksums no longer match — dwmcore.dll was recompiled by a
        // Windows Update.
        Wh_Log(
            L"No target shaders found — dwmcore.dll checksums changed after a "
            L"Windows Update; please file an issue");
        return FALSE;
    }
    if (matchedMask != 0xFu) {
        // Partial patch: some but not all of the four target shaders were
        // found. Leaving DWM with a mix of power-law and sRGB surfaces would
        // produce inconsistent rendering, so revert everything.
        Wh_Log(
            L"Partial patch (mask 0x%X of 0xF) — reverting to avoid "
            L"inconsistent SDR rendering",
            matchedMask);
        RestoreAllPatches();
        return FALSE;
    }
    Wh_Log(L"All 4 target shaders patched (%d blob(s) written) with gamma %.1f",
           totalPatched, (double)gamma);
    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Restoring original shader bytes");
    RestoreAllPatches();
}

// When the user changes the gamma setting, reload the mod so Wh_ModInit runs
// again with the new value. DWM must be restarted for the new shaders to
// compile.
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
