// ==WindhawkMod==
// @id              dwm-eotf-gamma
// @name            DWM EOTF Gamma Curve
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
# DWM EOTF Gamma Curve

When Windows HDR is enabled, DWM converts SDR content to HDR scRGB using the
sRGB EOTF — an ~2.2 power curve with a linear toe segment near black. This mod
patches the DXBC shader bytecode embedded in `dwmcore.dll` at DWM startup,
replacing the sRGB curve with a simple power-law gamma. This gives you direct
control over the SDR-to-HDR tone mapping without permanently modifying any files
on disk.

Based on [dwm_eotf](https://github.com/ledoge/dwm_eotf) by ledoge (GPL-3.0).

## How it works

On DWM startup, Windhawk injects this mod before Direct3D is initialized. The
mod locates the known DXBC shader blobs in `dwmcore.dll`'s read-only memory
sections and patches four floating-point constants that define the sRGB transfer
function, replacing them with the equivalent pure power-law constants. The shader
checksums are recalculated so D3D accepts the patched bytecode.

When the mod is unloaded (Windhawk disabled, settings changed, or Windows
shutdown), all patched bytes are restored to their original values.

## Usage

1. Enable **Windows HDR** in display settings.
2. Install and enable this mod.
3. Select your preferred gamma value in the settings below.
4. **Log off and back in** (or otherwise restart DWM) for the change to take effect.
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
- The shader checksums in `dwmcore.dll` may change with Windows Updates. If the
  mod reports patching 0 shaders after an update, the known-checksum list may
  need updating.
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

#include <windows.h>
#include <cstring>
#include <cstdlib>
#include <vector>

// =============================================================================
// DXBC Checksum — AMD DXBCChecksum (modified MD5 used by Microsoft for DXBC)
// Source: https://github.com/GPUOpen-Archive/common-src-ShaderUtils
// Copyright 2008-2016 Advanced Micro Devices, Inc. All rights reserved.
// Free for all — MD5 algorithm derived from RSA Data Security, Inc. (1990).
// =============================================================================

typedef unsigned long DX_UINT4;

struct MD5_CTX_DX
{
    DX_UINT4 i[2];
    DX_UINT4 buf[4];
    unsigned char in[64];
};

static const unsigned char kMD5Padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

static void DX_MD5Transform(DX_UINT4 *buf, DX_UINT4 *in)
{
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

static void DX_MD5Init(MD5_CTX_DX *ctx)
{
    ctx->i[0] = ctx->i[1] = 0;
    ctx->buf[0] = 0x67452301u;
    ctx->buf[1] = 0xefcdab89u;
    ctx->buf[2] = 0x98badcfeu;
    ctx->buf[3] = 0x10325476u;
}

static void DX_MD5Update(MD5_CTX_DX *ctx, const unsigned char *data, unsigned int len)
{
    int mdi = (int)((ctx->i[0] >> 3) & 0x3F);
    if ((ctx->i[0] + ((DX_UINT4)len << 3)) < ctx->i[0])
        ctx->i[1]++;
    ctx->i[0] += (DX_UINT4)len << 3;
    ctx->i[1] += (DX_UINT4)len >> 29;
    while (len--)
    {
        ctx->in[mdi++] = *data++;
        if (mdi == 0x40)
        {
            DX_UINT4 tmp[16];
            for (unsigned i = 0, ii = 0; i < 16; i++, ii += 4)
                tmp[i] = ((DX_UINT4)ctx->in[ii + 3] << 24) | ((DX_UINT4)ctx->in[ii + 2] << 16) | ((DX_UINT4)ctx->in[ii + 1] << 8) | (DX_UINT4)ctx->in[ii];
            DX_MD5Transform(ctx->buf, tmp);
            mdi = 0;
        }
    }
}

// Computes the DXBC-variant MD5 checksum for a shader blob.
// pData must point to the start of the DXBC header; dwSize is the full shader size.
// The checksum is written into dwHash[4].
static void CalcDXBCChecksum(BYTE *pData, DWORD dwSize, DWORD dwHash[4])
{
    static const DWORD kHashOffset = 0x14; // skip magic(4) + checksum(16) = 20 bytes
    MD5_CTX_DX ctx;
    DX_MD5Init(&ctx);

    dwSize -= kHashOffset;
    pData += kHashOffset;

    DWORD numBits = dwSize * 8;
    DWORD fullChunksSize = dwSize & 0xFFFFFFC0u;

    DX_MD5Update(&ctx, pData, fullChunksSize);

    DWORD lastChunkSize = dwSize - fullChunksSize;
    DWORD paddingSize = 64 - lastChunkSize;
    const BYTE *lastChunk = pData + fullChunksSize;

    if (lastChunkSize >= 56)
    {
        DX_MD5Update(&ctx, lastChunk, lastChunkSize);
        DX_MD5Update(&ctx, kMD5Padding, paddingSize);
        DX_UINT4 blk[16] = {};
        blk[0] = numBits;
        blk[15] = (numBits >> 2) | 1;
        DX_MD5Transform(ctx.buf, blk);
    }
    else
    {
        DX_MD5Update(&ctx, (const unsigned char *)&numBits, 4);
        if (lastChunkSize)
            DX_MD5Update(&ctx, lastChunk, lastChunkSize);
        lastChunkSize += sizeof(DWORD);
        paddingSize -= sizeof(DWORD);
        memcpy(&ctx.in[lastChunkSize], kMD5Padding, paddingSize);
        ((DX_UINT4 *)ctx.in)[15] = (numBits >> 2) | 1;
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
struct DXBCHeader
{
    char magic[4];     // "DXBC"
    DWORD checksum[4]; // DXBC-MD5 checksum (bytes 4–19)
    DWORD reserved;    // always 1
    DWORD size;        // total blob size in bytes
};
#pragma pack(pop)

static_assert(sizeof(DXBCHeader) == 28, "DXBCHeader size mismatch");
static const size_t kDXBCHeaderSize = sizeof(DXBCHeader); // 28

// =============================================================================
// Patch state
// =============================================================================

// Records the original bytes of a patched memory range so we can restore them.
struct PatchRecord
{
    BYTE *addr;
    std::vector<BYTE> original;
};

static std::vector<PatchRecord> g_patches;
static float g_gamma = 2.2f;

// Known DXBC checksums for the sRGB-EOTF shaders in dwmcore.dll.
// These are the unmodified ("vanilla") checksums for the target shader blobs.
static const BYTE kKnownChecksums[4][16] = {
    {0x96, 0xe6, 0xd1, 0x58, 0x92, 0x55, 0xec, 0xcd, 0x1d, 0xd7, 0xd4, 0xdb, 0xec, 0x54, 0xd2, 0x85},
    {0x21, 0x26, 0xb0, 0x37, 0xc1, 0xa2, 0xfb, 0xdd, 0xe3, 0x55, 0xb6, 0xe6, 0xdd, 0x9c, 0xaf, 0x3c},
    {0x2c, 0x89, 0x26, 0xff, 0xe2, 0x29, 0xf0, 0x5d, 0x96, 0x7c, 0x72, 0x66, 0x8d, 0xc3, 0xad, 0xdb},
    {0xf6, 0x93, 0xbf, 0xbb, 0xaf, 0x24, 0xb3, 0xd9, 0x36, 0x63, 0x54, 0xbe, 0x88, 0x98, 0xa7, 0xf5},
};

// =============================================================================
// Memory helpers
// =============================================================================

// Write bytes to an arbitrary (potentially read-only) address in the current
// process by temporarily relaxing page protection.
static bool WriteMemorySafe(void *dst, const void *src, size_t size)
{
    DWORD oldProt = 0;
    if (!VirtualProtect(dst, size, PAGE_READWRITE, &oldProt))
        return false;
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProt, &oldProt); // restore; ignore error
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
// Index 0 (the exponent) is replaced with the user-chosen gamma, not kPatchConsts[0].

// Searches 'buf' (a copy of a shader blob) for the sRGB splat patterns and
// patches them in-place. Returns true if at least one substitution was made.
static bool PatchShaderBuf(BYTE *buf, DWORD size, float gamma)
{
    bool changed = false;
    for (int ci = 0; ci < 4; ci++)
    {
        float src = kSrgbConsts[ci];
        float dst = (ci == 0) ? gamma : kPatchConsts[ci];
        float pat[3] = {src, src, src};
        // Search from after the header; stop where a full 12-byte match would overflow.
        for (size_t j = kDXBCHeaderSize; j + sizeof(float) * 3 <= size; j++)
        {
            if (memcmp(pat, buf + j, sizeof(pat)) != 0)
                continue;
            float rep[3] = {dst, dst, dst};
            memcpy(buf + j, rep, sizeof(rep));
            changed = true;
        }
    }
    return changed;
}

// =============================================================================
// Region scanner
// =============================================================================

// Scans a read-only committed memory region for DXBC shader blobs, patches any
// that match the known checksums, and appends restoration records to g_patches.
// Returns the number of individual shaders patched.
//
// "Big shader" handling: dwmcore.dll embeds the target shaders as sub-blobs
// inside larger DXBC container blobs. The container's own checksum must be
// recalculated after its inner shaders are patched.
static int ScanRegionForShaders(BYTE *region, size_t regionSize, float gamma)
{
    int numPatched = 0;

    DXBCHeader *bigShader = nullptr; // enclosing container blob, if any
    bool bigPatched = false;         // did we patch any sub-shader inside it?

    for (size_t i = 0; i + kDXBCHeaderSize <= regionSize; i++)
    {
        // If we have advanced past the end of the current container blob,
        // finalize it: save its original checksum and write the new one.
        if (bigShader && region + i >= (BYTE *)bigShader + bigShader->size)
        {
            if (bigPatched)
            {
                // Save the original checksum of the big shader (bytes 4–19).
                PatchRecord bigRec;
                bigRec.addr = (BYTE *)bigShader + 4;
                bigRec.original.assign(bigRec.addr, bigRec.addr + 16);
                g_patches.push_back(std::move(bigRec));

                // Recompute and write back the container checksum.
                DWORD newCk[4];
                CalcDXBCChecksum((BYTE *)bigShader, bigShader->size, newCk);
                WriteMemorySafe((BYTE *)bigShader + 4, newCk, 16);

                Wh_Log(L"  Fixed container checksum at %p", (void *)bigShader);
            }
            bigShader = nullptr;
            bigPatched = false;
        }

        auto *hdr = (DXBCHeader *)(region + i);

        // Validate DXBC magic and reserved field.
        if (hdr->magic[0] != 'D' || hdr->magic[1] != 'X' ||
            hdr->magic[2] != 'B' || hdr->magic[3] != 'C')
            continue;
        if (hdr->reserved != 1)
            continue;

        // Basic size sanity: must fit within the remaining region.
        if (hdr->size < (DWORD)kDXBCHeaderSize || i + hdr->size > regionSize)
            continue;

        // Check if this blob's checksum matches one of our targets.
        int matchIdx = -1;
        for (int k = 0; k < 4; k++)
        {
            if (memcmp(kKnownChecksums[k], hdr->checksum, 16) == 0)
            {
                matchIdx = k;
                break;
            }
        }

        if (matchIdx < 0)
        {
            // Unknown blob — track as a potential container for inner sub-shaders.
            if (!bigShader || region + i >= (BYTE *)bigShader + bigShader->size)
                bigShader = hdr;
            continue;
        }

        // Found a target shader. Make a working copy, patch it, validate, write back.
        std::vector<BYTE> patched(hdr->size);
        memcpy(patched.data(), hdr, hdr->size);

        if (!PatchShaderBuf(patched.data(), hdr->size, gamma))
        {
            Wh_Log(L"  Shader #%d matched checksum but no constants found to patch", matchIdx);
            continue;
        }

        // Recalculate DXBC checksum for the patched copy.
        DWORD newCk[4];
        CalcDXBCChecksum(patched.data(), hdr->size, newCk);
        memcpy(patched.data() + 4, newCk, 16);

        // Save the original bytes for later restoration.
        PatchRecord rec;
        rec.addr = (BYTE *)hdr;
        rec.original.assign(rec.addr, rec.addr + hdr->size);

        // Write the patched shader back into the (read-only) memory region.
        if (!WriteMemorySafe((BYTE *)hdr, patched.data(), hdr->size))
        {
            Wh_Log(L"  VirtualProtect failed for shader #%d at %p", matchIdx, (void *)hdr);
            continue;
        }

        g_patches.push_back(std::move(rec));
        numPatched++;
        if (bigShader)
            bigPatched = true;

        Wh_Log(L"  Patched shader #%d at %p (size %lu)", matchIdx, (void *)hdr, hdr->size);
    }

    // Handle a container blob that reaches the very end of the region.
    if (bigShader && bigPatched)
    {
        PatchRecord bigRec;
        bigRec.addr = (BYTE *)bigShader + 4;
        bigRec.original.assign(bigRec.addr, bigRec.addr + 16);
        g_patches.push_back(std::move(bigRec));

        DWORD newCk[4];
        CalcDXBCChecksum((BYTE *)bigShader, bigShader->size, newCk);
        WriteMemorySafe((BYTE *)bigShader + 4, newCk, 16);

        Wh_Log(L"  Fixed trailing container checksum at %p", (void *)bigShader);
    }

    return numPatched;
}

// Restores all patched memory regions to their original bytes.
static void RestoreAllPatches()
{
    // Iterate in reverse so sub-shader records (added first) are restored after
    // their container checksum records (added last), giving a consistent final state.
    for (auto it = g_patches.rbegin(); it != g_patches.rend(); ++it)
        WriteMemorySafe(it->addr, it->original.data(), it->original.size());
    g_patches.clear();
}

// =============================================================================
// Windhawk callbacks
// =============================================================================

BOOL Wh_ModInit()
{
    // Read the user's gamma setting.
    PCWSTR gammaStr = Wh_GetStringSetting(L"GammaCurve");
    float gamma = wcstof(gammaStr, nullptr);
    Wh_FreeStringSetting(gammaStr);
    if (gamma < 1.0f || gamma > 10.0f)
    {
        Wh_Log(L"Invalid gamma value, falling back to 2.2");
        gamma = 2.2f;
    }
    g_gamma = gamma;

    Wh_Log(L"DWM EOTF: target gamma = %.1f", (double)gamma);

    // Locate dwmcore.dll (always loaded as a static import of dwm.exe).
    HMODULE hDwmcore = GetModuleHandleW(L"dwmcore.dll");
    if (!hDwmcore)
    {
        Wh_Log(L"dwmcore.dll not found in process — aborting");
        return FALSE;
    }

    // Determine the module's address range from its PE header.
    auto *dos = (IMAGE_DOS_HEADER *)hDwmcore;
    auto *nt = (IMAGE_NT_HEADERS *)((BYTE *)hDwmcore + dos->e_lfanew);
    BYTE *modBase = (BYTE *)hDwmcore;
    size_t modSize = nt->OptionalHeader.SizeOfImage;

    Wh_Log(L"dwmcore.dll at %p, image size %zu", (void *)modBase, modSize);

    // Walk the virtual memory regions within dwmcore.dll's image range.
    // The DXBC shader blobs live in PAGE_READONLY sections (typically .rdata).
    int totalPatched = 0;
    BYTE *addr = modBase;
    while (addr < modBase + modSize)
    {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
            break;

        size_t regionSize = mbi.RegionSize;
        // Clamp to the module boundary.
        if ((BYTE *)mbi.BaseAddress + regionSize > modBase + modSize)
            regionSize = (modBase + modSize) - (BYTE *)mbi.BaseAddress;

        if (mbi.State == MEM_COMMIT &&
            mbi.Protect == PAGE_READONLY &&
            regionSize > 4096)
        {
            Wh_Log(L"Scanning region at %p, size %zu", mbi.BaseAddress, regionSize);
            totalPatched += ScanRegionForShaders((BYTE *)mbi.BaseAddress, regionSize, gamma);
        }

        addr = (BYTE *)mbi.BaseAddress + mbi.RegionSize;
    }

    if (totalPatched == 0)
    {
        Wh_Log(L"No shaders patched. HDR may be off, or dwmcore.dll was updated (checksums changed).");
    }
    else
    {
        Wh_Log(L"Successfully patched %d shader(s) with gamma %.1f", totalPatched, (double)gamma);
    }

    return TRUE;
}

void Wh_ModBeforeUninit()
{
    Wh_Log(L"DWM EOTF: restoring original shader bytes");
    RestoreAllPatches();
}

// When the user changes the gamma setting, reload the mod so Wh_ModInit runs
// again with the new value. DWM must be restarted for the new shaders to compile.
BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    *bReload = TRUE;
    return TRUE;
}
