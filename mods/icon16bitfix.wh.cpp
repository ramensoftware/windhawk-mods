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

#define NE_RSCTYPE_ICON               0x8003
#define NE_RSCTYPE_GROUP_ICON         0x800e

static constexpr LONGLONG kMaxNeFileSize = 64LL * 1024 * 1024;

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
    if (!lpszExeFileName)
    {
        return 0;
    }

    UINT ret = 0;
    UINT cx1, cx2, cy1, cy2;
    BYTE* pData;
    HANDLE hFile;
    UINT16 iconDirCount = 0, iconCount = 0;
    hFile = CreateFileW(lpszExeFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        WCHAR szExePath[MAX_PATH];
        DWORD dwSearchReturn = SearchPathW(nullptr, lpszExeFileName, nullptr,
            sizeof(szExePath) / sizeof(szExePath[0]), szExePath, nullptr);
        if ((dwSearchReturn == 0) || (dwSearchReturn > sizeof(szExePath) / sizeof(szExePath[0])))
        {
            return 0;
        }

        hFile = CreateFileW(szExePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) return 0;
    }
    LARGE_INTEGER fsize;
    if (!GetFileSizeEx(hFile, &fsize) || fsize.QuadPart <= 0 || fsize.QuadPart > kMaxNeFileSize)
    {
        CloseHandle(hFile);
        return 0;
    }

    DWORD fileSize = static_cast<DWORD>(fsize.QuadPart);
    std::vector<BYTE> fileBuf;
    try
    {
        fileBuf.resize(fileSize);
    }
    catch (const std::bad_alloc&)
    {
        CloseHandle(hFile);
        return 0;
    }

    for (DWORD totalRead = 0; totalRead < fileSize; )
    {
        DWORD chunkRead = 0;
        if (!ReadFile(hFile, fileBuf.data() + totalRead, fileSize - totalRead, &chunkRead, nullptr) ||
            chunkRead == 0)
        {
            CloseHandle(hFile);
            return 0;
        }
        totalRead += chunkRead;
    }
    CloseHandle(hFile);

    BYTE* image = fileBuf.data();
    BYTE* imageEnd = image + fileSize;
    auto inRange = [&](const void* p, size_t size) {
        return (BYTE*)p >= image && (BYTE*)p <= imageEnd &&
               size <= static_cast<size_t>(imageEnd - (BYTE*)p);
    };

    cx1 = LOWORD(cxDesired);
    cx2 = HIWORD(cxDesired);
    cy1 = LOWORD(cyDesired);
    cy2 = HIWORD(cyDesired);

    if (nIcons > 0xFFFF)
    {
        nIcons = 0xFFFF;
    }

    std::vector<UINT> localIconIds;
    if (!pIconId)
    {
        try
        {
            localIconIds.resize(nIcons ? nIcons : 1);
        }
        catch (const std::bad_alloc&)
        {
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

    if (!inRange(mz_header, sizeof(*mz_header))) return ret;
    if (mz_header->e_magic != IMAGE_DOS_SIGNATURE) return ret;
    if (mz_header->e_lfanew < 0) return ret;
    if (!inRange(image + mz_header->e_lfanew, sizeof(*ne_header))) return ret;
    ne_header = reinterpret_cast<const IMAGE_OS2_HEADER*>(image + mz_header->e_lfanew);
    if (ne_header->ne_magic == IMAGE_NT_SIGNATURE) return ret;
    if (ne_header->ne_magic != IMAGE_OS2_SIGNATURE) return ret;

    pData = image + mz_header->e_lfanew + ne_header->ne_rsrctab;

    if (ne_header->ne_rsrctab < ne_header->ne_restab)
    {
        if (!inRange(pData, sizeof(WORD))) return ret;
        WORD sizeShift = *reinterpret_cast<WORD*>(pData);
        if (sizeShift >= 16)
        {
            return ret;
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
                    UINT step = (cx2 && cy2) ? 2u : 1u;
                    UINT groupsAvail = static_cast<UINT>(iconDirCount - baseIndex);
                    UINT groupsWanted = nIcons / step;
                    if (groupsWanted > groupsAvail)
                    {
                        groupsWanted = groupsAvail;
                    }
                    UINT total = groupsWanted * step; // <= nIcons, always in-bounds

                    for (UINT g = 0; g < groupsWanted; g++)
                    {
                        pCIDir = USER32_LoadResource(image, pIconDir + baseIndex + g, sizeShift, &uSize);
                        bool valid = inRange(pCIDir, uSize) && IsValidGroupIconDir(pCIDir, uSize);

                        pIconId[g * step] = valid
                            ? LookupIconIdFromDirectoryEx(pCIDir, TRUE, cx1, cy1, flags)
                            : 0;

                        if (step == 2)
                        {
                            pIconId[g * step + 1] = valid
                                ? LookupIconIdFromDirectoryEx(pCIDir, TRUE, cx2, cy2, flags)
                                : 0;
                        }
                    }

                    for (UINT n = 0; n < total; n++)
                    {
                        pCIDir = nullptr;
                        for (UINT16 i = 0; i < iconCount; i++)
                        {
                            if (pIconStorage[i].id == (static_cast<int>(pIconId[n]) | 0x8000))
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

                        RetPtr[n] = pCIDir
                            ? CreateIconFromResourceEx(pCIDir, uSize, TRUE, 0x00030000,
                                                       (n % step) ? cx2 : cx1,
                                                       (n % step) ? cy2 : cy1, flags)
                            : nullptr;
                    }
                    UINT created = 0;
                    while (created < total && RetPtr[created]) created++;
                    ret = created;
                }
            }
        }
    }

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
