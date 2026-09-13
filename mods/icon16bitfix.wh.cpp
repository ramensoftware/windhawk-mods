// ==WindhawkMod==
// @id              icon16bitfix
// @name            Icons of Win16 apps in Explorer and file dialogs
// @description     Adds support for icons of 16-bit (Win16) applications in File Explorer and file dialogs
// @version         1.0.10
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @license         LGPL-2.1-or-later
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds support for icons of 16-bit (Win16) executables (such as Civilization I or Castle of the Winds) in File Explorer and file dialogs.
The mod is adapted from [Icon16bitFix utility](https://github.com/otya128/Icon16bitFix).

![Foler view](https://i.imgur.com/g4epNjk.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <vector>

typedef WORD HANDLE16;

struct NE_NAMEINFO
{
    WORD     offset;
    WORD     length;
    WORD     flags;
    WORD     id;
    HANDLE16 handle;
    WORD     usage;
};

struct NE_TYPEINFO
{
    WORD  type_id;
    WORD  count;
    DWORD resloader;
};

#define NE_RSCTYPE_ICON         0x8003
#define NE_RSCTYPE_GROUP_ICON   0x800e

static constexpr LONGLONG kMaxNeFileSize = 16 * 1024 * 1024; // generous upper bound for a Win16 NE image
static constexpr UINT kNoIconId = 0xFFFFFFFF; // never a valid resource id, used as "not found"

static BYTE* LoadNeResource(BYTE* image, NE_NAMEINFO* pNInfo, WORD sizeShift, ULONG* uSize)
{
    *uSize = static_cast<DWORD>(pNInfo->length) << sizeShift;
    return image + (static_cast<DWORD>(pNInfo->offset) << sizeShift);
}

static bool IsValidGroupIconDir(const BYTE* p, ULONG size)
{
    if (!p || size < 6) return false;
    WORD count = *reinterpret_cast<const WORD*>(p + 4);
    return size >= 6u + static_cast<ULONG>(count) * 14u; // sizeof(GRPICONDIRENTRY) == 14
}

static bool ReadExact(HANDLE hFile, void* buf, DWORD size)
{
    DWORD read = 0;
    return ReadFile(hFile, buf, size, &read, nullptr) && read == size;
}

static HANDLE OpenForRead(LPCWSTR path)
{
    // Read-only access: don't require exclusivity with writers.
    return CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, 0, nullptr);
}

UINT NE_ExtractIcon(LPCWSTR lpszExeFileName,
    HICON* RetPtr,
    INT nIconIndex,
    UINT nIcons,
    UINT cxDesired,
    UINT cyDesired,
    UINT* pIconId,
    UINT flags)
{
    if (!lpszExeFileName)
    {
        return 0;
    }

    HANDLE hFile = OpenForRead(lpszExeFileName);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // Fall back to SearchPathW for bare file names (app dirs, PATH, etc.).
        WCHAR szExePath[MAX_PATH];
        DWORD n = SearchPathW(nullptr, lpszExeFileName, nullptr, MAX_PATH, szExePath, nullptr);
        if (n == 0 || n > MAX_PATH)
        {
            return 0; // no worse than the original PrivateExtractIconsW's 0
        }
        hFile = OpenForRead(szExePath);
        if (hFile == INVALID_HANDLE_VALUE) return 0;
    }

    LARGE_INTEGER fsize;
    IMAGE_DOS_HEADER mz{};
    IMAGE_OS2_HEADER ne{};
    LARGE_INTEGER off{};

    // Sniff the DOS/NE headers before allocating/reading the whole file -
    // this path runs for every icon-less PE file too.
    bool isNe = GetFileSizeEx(hFile, &fsize) && fsize.QuadPart > 0 &&
        ReadExact(hFile, &mz, sizeof(mz)) &&
        mz.e_magic == IMAGE_DOS_SIGNATURE && mz.e_lfanew >= 0 &&
        static_cast<ULONGLONG>(mz.e_lfanew) + sizeof(ne) <= static_cast<ULONGLONG>(fsize.QuadPart) &&
        (off.QuadPart = mz.e_lfanew, SetFilePointerEx(hFile, off, nullptr, FILE_BEGIN)) &&
        ReadExact(hFile, &ne, sizeof(ne)) &&
        ne.ne_magic == IMAGE_OS2_SIGNATURE;

    if (!isNe || fsize.QuadPart > kMaxNeFileSize)
    {
        CloseHandle(hFile);
        return 0;
    }

    DWORD fsizel = static_cast<DWORD>(fsize.QuadPart);
    std::vector<BYTE> fileBuf;
    off.QuadPart = 0;
    bool loaded = false;
    try
    {
        fileBuf.resize(fsizel);
        loaded = SetFilePointerEx(hFile, off, nullptr, FILE_BEGIN) && ReadExact(hFile, fileBuf.data(), fsizel);
    }
    catch (const std::bad_alloc&) {}
    CloseHandle(hFile);
    if (!loaded)
    {
        return 0;
    }

    BYTE* image = fileBuf.data();
    BYTE* imageEnd = image + fsizel;
    auto inRange = [&](const void* p, size_t size) {
        return (BYTE*)p >= image && (BYTE*)p <= imageEnd && size <= static_cast<size_t>(imageEnd - (BYTE*)p);
    };

    auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
    auto* neh = reinterpret_cast<const IMAGE_OS2_HEADER*>(image + dos->e_lfanew);
    if (neh->ne_rsrctab >= neh->ne_restab)
    {
        return 0;
    }

    BYTE* pData = image + dos->e_lfanew + neh->ne_rsrctab;
    if (!inRange(pData, sizeof(WORD)))
    {
        return 0;
    }
    WORD sizeShift = *reinterpret_cast<WORD*>(pData);
    if (sizeShift >= 16)
    {
        return 0; // invalid shift, would be UB below
    }

    UINT16 iconDirCount = 0, iconCount = 0;
    NE_NAMEINFO *pIconDir = nullptr, *pIconStorage = nullptr;
    auto pTInfo = reinterpret_cast<NE_TYPEINFO*>(pData + 2);

    while (inRange(pTInfo, sizeof(NE_TYPEINFO)) && pTInfo->type_id && !(pIconDir && pIconStorage))
    {
        auto* infos = reinterpret_cast<NE_NAMEINFO*>(pTInfo + 1);
        size_t infosSize = static_cast<size_t>(pTInfo->count) * sizeof(NE_NAMEINFO);
        if (!inRange(infos, infosSize))
        {
            break;
        }
        if (pTInfo->type_id == NE_RSCTYPE_GROUP_ICON)
        {
            iconDirCount = pTInfo->count;
            pIconDir = infos;
        }
        else if (pTInfo->type_id == NE_RSCTYPE_ICON)
        {
            iconCount = pTInfo->count;
            pIconStorage = infos;
        }
        pTInfo = reinterpret_cast<NE_TYPEINFO*>(reinterpret_cast<char*>(infos) + infosSize);
    }

    if (!pIconDir || !pIconStorage)
    {
        return 0;
    }
    if (nIcons == 0 || !RetPtr)
    {
        return iconDirCount;
    }

    if (nIcons > 0xFFFF)
    {
        nIcons = 0xFFFF; // iconDirCount is a UINT16; also bounds the allocation below
    }

    std::vector<UINT> localIconIds;
    if (!pIconId)
    {
        try { localIconIds.resize(nIcons); }
        catch (const std::bad_alloc&) { return 0; }
        pIconId = localIconIds.data();
    }
    pIconId[0] = kNoIconId; // only touch the caller's buffer once we know we'll populate it

    int resolvedIndex = nIconIndex;
    if (nIconIndex < 0)
    {
        resolvedIndex = -1;
        if (nIconIndex >= -0xFFFF) // else magnitude can't fit a resource id; avoid negating INT_MIN (UB)
        {
            WORD wantId = static_cast<WORD>((-nIconIndex) | 0x8000);
            for (UINT16 j = 0; j < iconDirCount; j++)
            {
                if (pIconDir[j].id == wantId) { resolvedIndex = j; break; }
            }
        }
    }
    if (resolvedIndex < 0 || static_cast<UINT>(resolvedIndex) >= iconDirCount)
    {
        return 0;
    }

    UINT16 baseIndex = static_cast<UINT16>(resolvedIndex);
    if (nIcons > static_cast<UINT>(iconDirCount - baseIndex))
    {
        nIcons = iconDirCount - baseIndex;
    }

    BYTE* pCIDir = nullptr;
    ULONG uSize = 0;
    UINT16 i, icon;
    for (i = 0; i < nIcons; i++)
    {
        pCIDir = LoadNeResource(image, pIconDir + i + baseIndex, sizeShift, &uSize);
        bool valid = inRange(pCIDir, uSize) && IsValidGroupIconDir(pCIDir, uSize);
        pIconId[i] = valid ? LookupIconIdFromDirectoryEx(pCIDir, TRUE, LOWORD(cxDesired), LOWORD(cyDesired), flags) : kNoIconId;

        if (HIWORD(cxDesired) && HIWORD(cyDesired) && i + 1 < nIcons)
        {
            valid = inRange(pCIDir, uSize) && IsValidGroupIconDir(pCIDir, uSize);
            pIconId[++i] = valid ? LookupIconIdFromDirectoryEx(pCIDir, TRUE, HIWORD(cxDesired), HIWORD(cyDesired), flags) : kNoIconId;
        }
    }

    // Create icons one slot at a time, stopping at the first failure, so we
    // never create (and thus leak) an icon past the count we report back.
    UINT created = 0;
    for (icon = 0; icon < nIcons; icon++)
    {
        pCIDir = nullptr;
        if (pIconId[icon] != kNoIconId)
        {
            for (i = 0; i < iconCount; i++)
            {
                if (pIconStorage[i].id == (static_cast<int>(pIconId[icon]) | 0x8000))
                {
                    ULONG sz = 0;
                    BYTE* cand = LoadNeResource(image, pIconStorage + i, sizeShift, &sz);
                    if (inRange(cand, sz)) { pCIDir = cand; uSize = sz; break; }
                }
            }
        }

        HICON hIcon = pCIDir ? CreateIconFromResourceEx(pCIDir, uSize, TRUE, 0x00030000,
            LOWORD(cxDesired), LOWORD(cyDesired), flags) : nullptr;
        if (!hIcon) break;
        RetPtr[created++] = hIcon;

        if (HIWORD(cxDesired) && HIWORD(cyDesired) && icon + 1 < nIcons)
        {
            hIcon = CreateIconFromResourceEx(pCIDir, uSize, TRUE, 0x00030000,
                HIWORD(cxDesired), HIWORD(cyDesired), flags);
            if (!hIcon) break;
            RetPtr[created++] = hIcon;
            icon++;
        }
    }
    for (UINT n = created; n < nIcons; n++)
    {
        RetPtr[n] = nullptr;
    }
    return created;
}

typedef UINT (WINAPI *PrivateExtractIconsW_t)(
    LPCWSTR szFileName, int nIconIndex, int cxIcon, int cyIcon,
    HICON* phicon, UINT* piconid, UINT nIcons, UINT flags);
PrivateExtractIconsW_t PrivateExtractIconsW_Original;

UINT WINAPI PrivateExtractIconsW_Hook(
    LPCWSTR szFileName, int nIconIndex, int cxIcon, int cyIcon,
    HICON* phicon, UINT* piconid, UINT nIcons, UINT flags)
{
    UINT a = PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon, cyIcon, phicon, piconid, nIcons, flags);
    if (a && a != (UINT)-1)
    {
        return a;
    }

    UINT b = NE_ExtractIcon(szFileName, phicon, nIconIndex, nIcons, cxIcon, cyIcon, piconid, flags);
    return (b && b != (UINT)-1) ? b : a;
}

BOOL Wh_ModInit(void)
{
    WindhawkUtils::SetFunctionHook(PrivateExtractIconsW, PrivateExtractIconsW_Hook, &PrivateExtractIconsW_Original);
    return TRUE;
}
