// ==WindhawkMod==
// @id              icon16bitfix
// @name            Icons of Win16 apps in Explorer and file dialogs
// @description     Adds support for icons of 16-bit (Win16) applications in File Explorer and file dialogs
// @version         1.1
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
#include <new>


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

typedef DWORD FARPROC16;

struct NE_TYPEINFO
{
    WORD        type_id;
    WORD        count;
    FARPROC16   resloader;
};

// Only the two resource types this mod actually walks are kept; the rest of
// the NE_RSCTYPE_* set inherited from the Wine port was unused.
#define NE_RSCTYPE_ICON               0x8003
#define NE_RSCTYPE_GROUP_ICON         0x800e

static BYTE* USER32_LoadResource(BYTE* peimage, NE_NAMEINFO* pNInfo, WORD sizeShift, ULONG* uSize)
{
    *uSize = static_cast<DWORD>(pNInfo->length) << sizeShift;
    return peimage + (static_cast<DWORD>(pNInfo->offset) << sizeShift);
}

static bool IsValidGroupIconDir(const BYTE* p, ULONG size)
{
    if (!p || size < 6) return false;
    WORD count = *reinterpret_cast<const WORD*>(p + 4);
    return size >= 6u + static_cast<ULONG>(count) * 14u; // sizeof(GRPICONDIRENTRY) == 14
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
    // PrivateExtractIconsW callers shouldn't pass NULL, but this now runs in
    // every process, so guard against it cheaply instead of trusting the
    // caller.
    if (!lpszExeFileName)
    {
        return 0;
    }

    UINT ret = 0;
    UINT cx1, cx2, cy1, cy2;
    BYTE* pData;
    HANDLE hFile;
    UINT16 iconDirCount = 0, iconCount = 0;
    BYTE* image;
    HANDLE fmapping;
    DWORD fsizeh, fsizel;

    // Try the path as given first. For an absolute path this resolves
    // directly to that file. For a bare/relative name CreateFileW itself
    // still consults the process's current directory (same as SearchPathW
    // would), so no special avoidance happens in that case - the fallback
    // below only helps SearchPathW's *additional* search locations (the
    // application/system directories, PATH, etc.) find a match that
    // CreateFileW's plain relative-path resolution wouldn't.
    hFile = CreateFileW(lpszExeFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        WCHAR szExePath[MAX_PATH];
        DWORD dwSearchReturn = SearchPathW(nullptr, lpszExeFileName, nullptr,
            sizeof(szExePath) / sizeof(szExePath[0]), szExePath, nullptr);
        if ((dwSearchReturn == 0) || (dwSearchReturn > sizeof(szExePath) / sizeof(szExePath[0])))
        {
            // The original PrivateExtractIconsW already returned 0 before
            // this fallback runs, so never report a "worse" result than that.
            return 0;
        }

        hFile = CreateFileW(szExePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) return 0;
    }

    fsizel = GetFileSize(hFile, &fsizeh);
    if (fsizel == INVALID_FILE_SIZE || fsizeh != 0)
    {
        // GetFileSize failed, or the file is 4 GB or larger - a valid NE
        // image can't be that big, and INVALID_FILE_SIZE (0xFFFFFFFF) would
        // make every bounds check below pass trivially.
        CloseHandle(hFile);
        return 0;
    }

    fmapping = CreateFileMappingW(hFile, nullptr, PAGE_READONLY | SEC_COMMIT, 0, 0, nullptr);
    CloseHandle(hFile);
    if (!fmapping)
    {
        return 0;
    }

    image = static_cast<BYTE*>(MapViewOfFile(fmapping, FILE_MAP_READ, 0, 0, 0));
    CloseHandle(fmapping);
    if (!image)
    {
        return 0;
    }

    BYTE* imageEnd = image + fsizel;
    auto inRange = [&](const void* p, size_t size) {
        return (BYTE*)p >= image && (BYTE*)p <= imageEnd &&
               size <= static_cast<size_t>(imageEnd - (BYTE*)p);
    };

    cx1 = LOWORD(cxDesired);
    cx2 = HIWORD(cxDesired);
    cy1 = LOWORD(cyDesired);
    cy2 = HIWORD(cyDesired);

    // iconDirCount (the real per-file limit) is a UINT16, so nIcons can
    // never usefully exceed that. Clamp it up front, before doing any
    // allocation based on it, so a caller-controlled UINT can't drive an
    // absurd allocation size.
    if (nIcons > 0xFFFF)
    {
        nIcons = 0xFFFF;
    }

    // If pIconId is nullptr, use a local scratch buffer instead of reusing
    // the caller's HICON array - on 64-bit HICON is 8 bytes while UINT is 4,
    // so writing UINTs into that array would corrupt not-yet-read entries.
    std::vector<UINT> localIconIds;
    if (!pIconId)
    {
        try
        {
            localIconIds.resize(nIcons ? nIcons : 1);
        }
        catch (const std::bad_alloc&)
        {
            // Belt-and-braces: even after the clamp above this shouldn't
            // throw, but if it ever does, don't leak the mapped view or let
            // a C++ exception cross back into the caller through user32.
            UnmapViewOfFile(image);
            return 0;
        }
        pIconId = localIconIds.data();
    }

    if (nIcons != 0)
    {
        pIconId[0] = 0xFFFFFFFF;
    }

    auto mz_header = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
    const IMAGE_OS2_HEADER* ne_header;

    if (!inRange(mz_header, sizeof(*mz_header))) goto end;
    if (mz_header->e_magic != IMAGE_DOS_SIGNATURE) goto end;
    if (mz_header->e_lfanew < 0) goto end;
    if (!inRange(image + mz_header->e_lfanew, sizeof(*ne_header))) goto end;
    ne_header = reinterpret_cast<const IMAGE_OS2_HEADER*>(image + mz_header->e_lfanew);
    if (ne_header->ne_magic == IMAGE_NT_SIGNATURE) goto end;
    if (ne_header->ne_magic != IMAGE_OS2_SIGNATURE) goto end;

    pData = image + mz_header->e_lfanew + ne_header->ne_rsrctab;

    if (ne_header->ne_rsrctab < ne_header->ne_restab)
    {
        if (!inRange(pData, sizeof(WORD))) goto end;
        WORD sizeShift = *reinterpret_cast<WORD*>(pData);
        if (sizeShift >= 16)
        {
            // A shift count this large is not a valid NE alignment shift
            // and would be undefined behavior when used below.
            goto end;
        }

        BYTE* pCIDir = nullptr;
        auto pTInfo = reinterpret_cast<NE_TYPEINFO*>(pData + 2);
        NE_NAMEINFO* pIconStorage = nullptr;
        NE_NAMEINFO* pIconDir = nullptr;
        ULONG uSize = 0;

        while (inRange(pTInfo, sizeof(NE_TYPEINFO)) && pTInfo->type_id && !(pIconStorage && pIconDir))
        {
            NE_NAMEINFO* infos = reinterpret_cast<NE_NAMEINFO*>(pTInfo + 1);
            size_t infosSize = static_cast<size_t>(pTInfo->count) * sizeof(NE_NAMEINFO);

            if (!inRange(infos, infosSize))
            {
                // Truncated/corrupt resource table - stop walking instead of
                // running off the end of the mapping.
                break;
            }

            if (pTInfo->type_id == NE_RSCTYPE_GROUP_ICON)
            {
                iconDirCount = pTInfo->count;
                pIconDir = infos;
            }
            if (pTInfo->type_id == NE_RSCTYPE_ICON)
            {
                iconCount = pTInfo->count;
                pIconStorage = infos;
            }
            pTInfo = reinterpret_cast<NE_TYPEINFO*>(reinterpret_cast<char*>(infos) + infosSize);
        }

        if (pIconStorage && pIconDir)
        {
            if (nIcons == 0 || !RetPtr)
            {
                ret = iconDirCount;
            }
            else
            {
                int resolvedIndex = nIconIndex;

                if (nIconIndex < 0)
                {
                    // Negative index is the documented "icon resource ID"
                    // convention (e.g. "file.exe,-3"). Resolve it against
                    // the RT_GROUP_ICON entry ids instead of indexing
                    // backwards from the table.
                    resolvedIndex = -1;
                    WORD wantId = static_cast<WORD>((-nIconIndex) | 0x8000);
                    for (UINT16 j = 0; j < iconDirCount; j++)
                    {
                        if (pIconDir[j].id == wantId)
                        {
                            resolvedIndex = j;
                            break;
                        }
                    }
                }

                if (resolvedIndex >= 0 && static_cast<UINT16>(resolvedIndex) < iconDirCount)
                {
                    UINT16 baseIndex = static_cast<UINT16>(resolvedIndex);
                    UINT16 i, icon;

                    if (nIcons > static_cast<UINT>(iconDirCount - baseIndex))
                    {
                        nIcons = iconDirCount - baseIndex;
                    }

                    for (i = 0; i < nIcons; i++)
                    {
                        pCIDir = USER32_LoadResource(image, pIconDir + i + baseIndex, sizeShift, &uSize);
                        pIconId[i] = (inRange(pCIDir, uSize) && IsValidGroupIconDir(pCIDir, uSize))
                            ? LookupIconIdFromDirectoryEx(pCIDir, TRUE, cx1, cy1, flags)
                            : 0;

                        if (cx2 && cy2 && i + 1 < nIcons)
                        {
                            pIconId[++i] = (inRange(pCIDir, uSize) && IsValidGroupIconDir(pCIDir, uSize))
                                ? LookupIconIdFromDirectoryEx(pCIDir, TRUE, cx2, cy2, flags)
                                : 0;
                        }
                    }

                    for (icon = 0; icon < nIcons; icon++)
                    {
                        pCIDir = nullptr;
                        for (i = 0; i < iconCount; i++)
                        {
                            if (pIconStorage[i].id == (static_cast<int>(pIconId[icon]) | 0x8000))
                            {
                                ULONG candidateSize = 0;
                                BYTE* candidate = USER32_LoadResource(image, pIconStorage + i, sizeShift, &candidateSize);
                                if (inRange(candidate, candidateSize))
                                {
                                    pCIDir = candidate;
                                    uSize = candidateSize;
                                    break;
                                }
                            }
                        }

                        if (pCIDir)
                        {
                            RetPtr[icon] = CreateIconFromResourceEx(pCIDir, uSize, TRUE, 0x00030000, cx1, cy1, flags);
                            if (cx2 && cy2 && icon + 1 < nIcons)
                            {
                                RetPtr[++icon] = CreateIconFromResourceEx(pCIDir, uSize, TRUE, 0x00030000, cx2, cy2, flags);
                            }
                        }
                        else
                        {
                            RetPtr[icon] = nullptr;
                        }
                    }
                    // icon is bounded by nIcons via the loop condition and
                    // the icon + 1 < nIcons guard above, so it can never
                    // exceed nIcons here - no extra clamp needed.
                    ret = icon;
                }
            }
        }
    }

end:
    UnmapViewOfFile(image);
    return ret;
}

typedef UINT (WINAPI *PrivateExtractIconsW_t)(
    LPCWSTR szFileName,
    int nIconIndex,
    int cxIcon,
    int cyIcon,
    HICON *phicon,
    UINT *piconid,
    UINT nIcons,
    UINT flags);
PrivateExtractIconsW_t PrivateExtractIconsW_Original;

UINT WINAPI PrivateExtractIconsW_Hook(
    LPCWSTR szFileName,
    int nIconIndex,
    int cxIcon,
    int cyIcon,
    HICON *phicon,
    UINT *piconid,
    UINT nIcons,
    UINT flags)
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
