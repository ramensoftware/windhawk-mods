// ==WindhawkMod==
// @id              icon16bitfix
// @name            Icons of Win16 apps in Explorer
// @description     Adds support for icons of 16-bit (Win16) applications in File Explorer
// @version         1.0.3
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @license         LGPL-2.1-or-later
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds support for icons of 16-bit (Win16) executables in File Explorer (such as Civilization I or Castle of the Winds).
The mod is adapted from [Icon16bitFix utility](https://github.com/otya128/Icon16bitFix).

![Foler view](https://i.imgur.com/g4epNjk.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>


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

#define NE_RSCTYPE_CURSOR             0x8001
#define NE_RSCTYPE_BITMAP             0x8002
#define NE_RSCTYPE_ICON               0x8003
#define NE_RSCTYPE_MENU               0x8004
#define NE_RSCTYPE_DIALOG             0x8005
#define NE_RSCTYPE_STRING             0x8006
#define NE_RSCTYPE_FONTDIR            0x8007
#define NE_RSCTYPE_FONT               0x8008
#define NE_RSCTYPE_ACCELERATOR        0x8009
#define NE_RSCTYPE_RCDATA             0x800a
#define NE_RSCTYPE_GROUP_CURSOR       0x800c
#define NE_RSCTYPE_GROUP_ICON         0x800e
#define NE_RSCTYPE_SCALABLE_FONTPATH  0x80cc

static BYTE* USER32_LoadResource(BYTE* peimage, NE_NAMEINFO* pNInfo, WORD sizeShift, ULONG* uSize)
{
    // TRACE("%p %p 0x%08x\n", peimage, pNInfo, sizeShift); // Commented out

    *uSize = static_cast<DWORD>(pNInfo->length) << sizeShift;
    return peimage + (static_cast<DWORD>(pNInfo->offset) << sizeShift);
}

struct icoICONDIRENTRY
{
    BYTE        bWidth;
    BYTE        bHeight;
    BYTE        bColorCount;
    BYTE        bReserved;
    WORD        wPlanes;
    WORD        wBitCount;
    DWORD       dwBytesInRes;
    DWORD       dwImageOffset;
};

struct icoICONDIR
{
    WORD            idReserved;
    WORD            idType;
    WORD            idCount;
    icoICONDIRENTRY idEntries[1];
};

static BYTE* ICO_LoadIcon(BYTE* peimage, icoICONDIRENTRY* lpiIDE, ULONG* uSize)
{
    // TRACE("%p %p\n", peimage, lpiIDE); // Commented out

    *uSize = lpiIDE->dwBytesInRes;
    return peimage + lpiIDE->dwImageOffset;
}

char* get_search_path()
{
    return nullptr;
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
    UINT ret = 0;
    UINT cx1, cx2, cy1, cy2;
    BYTE* pData;
    HANDLE hFile;
    UINT16 iconDirCount = 0, iconCount = 0;
    BYTE* image;
    HANDLE fmapping;
    DWORD fsizeh, fsizel;
    WCHAR szExePath[MAX_PATH];
    DWORD dwSearchReturn;

    char* path = get_search_path();
    dwSearchReturn = SearchPathW(nullptr, lpszExeFileName, nullptr, sizeof(szExePath) / sizeof(szExePath[0]), szExePath, nullptr);
    HeapFree(GetProcessHeap(), 0, path);
    if ((dwSearchReturn == 0) || (dwSearchReturn > sizeof(szExePath) / sizeof(szExePath[0])))
    {
        return static_cast<UINT>(-1);
    }

    hFile = CreateFileW(szExePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return 0;
    fsizel = GetFileSize(hFile, &fsizeh);

    fmapping = CreateFileMappingW(hFile, nullptr, PAGE_READONLY | SEC_COMMIT, 0, 0, nullptr);
    CloseHandle(hFile);
    if (!fmapping)
    {
        return 0xFFFFFFFF;
    }

    image = static_cast<BYTE*>(MapViewOfFile(fmapping, FILE_MAP_READ, 0, 0, 0));
    CloseHandle(fmapping);
    if (!image)
    {
        return 0xFFFFFFFF;
    }

    cx1 = LOWORD(cxDesired);
    cx2 = HIWORD(cxDesired);
    cy1 = LOWORD(cyDesired);
    cy2 = HIWORD(cyDesired);

    if (pIconId)
    {
        *pIconId = 0xFFFFFFFF;
    }

    // If pIconId is nullptr, it's intended to store the result in RetPtr array
    if (!pIconId)
    {
        pIconId = reinterpret_cast<UINT*>(RetPtr);
    }

    auto mz_header = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
    const IMAGE_OS2_HEADER* ne_header;

    if (fsizel < sizeof(*mz_header)) goto end;
    if (mz_header->e_magic != IMAGE_DOS_SIGNATURE) goto end;
    ne_header = reinterpret_cast<const IMAGE_OS2_HEADER*>(image + mz_header->e_lfanew);
    if (mz_header->e_lfanew + sizeof(*ne_header) > fsizel) goto end;
    if (ne_header->ne_magic == IMAGE_NT_SIGNATURE) goto end;
    if (ne_header->ne_magic != IMAGE_OS2_SIGNATURE) goto end;

    pData = image + mz_header->e_lfanew + ne_header->ne_rsrctab;

    if (ne_header->ne_rsrctab < ne_header->ne_restab)
    {
        BYTE* pCIDir = nullptr;
        auto pTInfo = reinterpret_cast<NE_TYPEINFO*>(pData + 2);
        NE_NAMEINFO* pIconStorage = nullptr;
        NE_NAMEINFO* pIconDir = nullptr;
        ULONG uSize = 0;

        while (pTInfo->type_id && !(pIconStorage && pIconDir))
        {
            if (pTInfo->type_id == NE_RSCTYPE_GROUP_ICON)
            {
                iconDirCount = pTInfo->count;
                pIconDir = reinterpret_cast<NE_NAMEINFO*>(pTInfo + 1);
            }
            if (pTInfo->type_id == NE_RSCTYPE_ICON)
            {
                iconCount = pTInfo->count;
                pIconStorage = reinterpret_cast<NE_NAMEINFO*>(pTInfo + 1);
            }
            pTInfo = reinterpret_cast<NE_TYPEINFO*>(reinterpret_cast<char*>(pTInfo + 1) + pTInfo->count * sizeof(NE_NAMEINFO));
        }

        if (pIconStorage && pIconDir)
        {
            if (nIcons == 0)
            {
                ret = iconDirCount;
            }
            else if (nIconIndex < iconDirCount)
            {
                UINT16 i, icon;
                if (nIcons > iconDirCount - nIconIndex)
                {
                    nIcons = iconDirCount - nIconIndex;
                }

                for (i = 0; i < nIcons; i++)
                {
                    pCIDir = USER32_LoadResource(image, pIconDir + i + nIconIndex, *reinterpret_cast<WORD*>(pData), &uSize);
                    pIconId[i] = LookupIconIdFromDirectoryEx(pCIDir, TRUE, cx1, cy1, flags);
                    if (cx2 && cy2)
                    {
                        pIconId[++i] = LookupIconIdFromDirectoryEx(pCIDir, TRUE, cx2, cy2, flags);
                    }
                }

                for (icon = 0; icon < nIcons; icon++)
                {
                    pCIDir = nullptr;
                    for (i = 0; i < iconCount; i++)
                    {
                        if (pIconStorage[i].id == (static_cast<int>(pIconId[icon]) | 0x8000))
                        {
                            pCIDir = USER32_LoadResource(image, pIconStorage + i, *reinterpret_cast<WORD*>(pData), &uSize);
                        }
                    }

                    if (pCIDir)
                    {
                        RetPtr[icon] = CreateIconFromResourceEx(pCIDir, uSize, TRUE, 0x00030000, cx1, cy1, flags);
                        if (cx2 && cy2)
                        {
                            RetPtr[++icon] = CreateIconFromResourceEx(pCIDir, uSize, TRUE, 0x00030000, cx2, cy2, flags);
                        }
                    }
                    else
                    {
                        RetPtr[icon] = nullptr;
                    }
                }
                ret = icon;
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
    if (a)
        return a;
    return NE_ExtractIcon(szFileName, phicon, nIconIndex, nIcons, cxIcon, cyIcon, piconid, flags);
}

BOOL Wh_ModInit(void)
{   

    WindhawkUtils::Wh_SetFunctionHookT(PrivateExtractIconsW, PrivateExtractIconsW_Hook, &PrivateExtractIconsW_Original);
 
    return TRUE;
}
