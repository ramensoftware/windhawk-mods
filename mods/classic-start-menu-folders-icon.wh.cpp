// ==WindhawkMod==
// @id              classic-start-menu-folders-icon
// @name            Classic Start Menu Folders Icon
// @description     Change the default icon of Start Menu folders like Windows XP and before
// @version         1.0
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Start Menu Folders Icon
Changes the default icon of Start Menu folders that was used in Windows XP and before.

![Open-Shell Menu comparison showing the icon that was used in Windows XP and before](https://i.imgur.com/rufM7Yh.png)

**NOTE:** This mod doesn't change the icon of Windows 10/11 Start Menu.
[Open-Shell](https://github.com/Open-Shell/Open-Shell-Menu) was used for the
screenshot.
*/
// ==/WindhawkModReadme==

#include <shlobj.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#define SIID_PROG 36

#ifdef _WIN64
#define THISCALL __cdecl
#define STHISCALL L"__cdecl"
#else
#define THISCALL __stdcall
#define STHISCALL L"__stdcall"
#endif

struct IDFOLDER {};
struct ICachedPrivateProfile {};
struct CFSFolder {};

UINT THISCALL (*CFSFolder__GetCSIDL)(void*);
HRESULT THISCALL (*CFSFolder__GetDesktopIniForItem)(void*,
                                                    ITEMIDLIST_RELATIVE*,
                                                    void*);
IDFOLDER* THISCALL (*CFSFolder__IsValidID)(void*, ITEMIDLIST_RELATIVE*, void*);

UINT THISCALL (*GetFolderString)(void*, LPCWSTR, LPCWSTR, LPWSTR*, int);
int THISCALL (*IsFileFolder)(IDFOLDER*);
int THISCALL (*Shell_GetStockImageIndex)(int);

BOOL CheckCSIDL(UINT csidl) {
    switch (csidl) {
        case CSIDL_STARTMENU:
        case CSIDL_COMMON_STARTMENU:
        case CSIDL_PROGRAMS:
        case CSIDL_COMMON_PROGRAMS:
            return true;
    }
    return false;
}

long THISCALL (*CFSFolder__CreateFileFolderDefExtIcon_orig)(CFSFolder*,
                                                            ITEMID_CHILD*,
                                                            IDFOLDER*,
                                                            IBindCtx*,
                                                            GUID*,
                                                            void**);
long THISCALL CFSFolder__CreateFileFolderDefExtIcon_hook(CFSFolder* pThis,
                                                         ITEMID_CHILD* pItemID,
                                                         IDFOLDER* pIDFOLDER,
                                                         IBindCtx* pBind,
                                                         GUID* guid,
                                                         void** ppv) {
    long result = CFSFolder__CreateFileFolderDefExtIcon_orig(
        pThis, pItemID, pIDFOLDER, pBind, guid, ppv);

    ICachedPrivateProfile* pICachedPrivateProfile;
    LPWSTR folderString[264];
    UINT csidl = CFSFolder__GetCSIDL(pThis);

    if (!CheckCSIDL(csidl)) {
        return result;
    }

    if (CFSFolder__GetDesktopIniForItem(pThis, pItemID,
                                        &pICachedPrivateProfile) >= 0) {
        if (GetFolderString(pICachedPrivateProfile, L".ShellClassInfo",
                            L"IconResource", folderString, MAX_PATH) ||
            GetFolderString(pICachedPrivateProfile, L".ShellClassInfo",
                            L"IconFile", folderString, MAX_PATH)) {
            return result;
        }
    }

    IDefaultExtractIconInit* pdxi = NULL;
    SHCreateDefaultExtractIcon(IID_PPV_ARGS(&pdxi));
    pdxi->SetNormalIcon(NULL, SIID_PROG);
    pdxi->SetOpenIcon(NULL, SIID_PROG);

    pdxi->QueryInterface((IID)*guid, ppv);

    if (pdxi) {
        pdxi->Release();
    }

    return result;
}

long THISCALL (*CFSFolder_GetIconOf_orig)(CFSFolder*,
                                          ITEMIDLIST_RELATIVE*,
                                          unsigned int,
                                          int*);
long THISCALL CFSFolder_GetIconOf_hook(CFSFolder* pThis,
                                       ITEMIDLIST_RELATIVE* pItemID,
                                       unsigned int a3,
                                       int* a4) {
    long result = CFSFolder_GetIconOf_orig(pThis, pItemID, a3, a4);

    ICachedPrivateProfile* pICachedPrivateProfile;
    LPWSTR folderString[264];
    IShellFolder* cfsFolder = (IShellFolder*)((CFSFolder*)pThis - 56);
    UINT csidl = CFSFolder__GetCSIDL((CFSFolder*)cfsFolder);
    IDFOLDER* idFolder =
        CFSFolder__IsValidID((CFSFolder*)cfsFolder, pItemID, NULL);

    if (!CheckCSIDL(csidl)) {
        return result;
    }

    if (!idFolder) {
        return result;
    }

    if (!IsFileFolder(idFolder)) {
        return result;
    }

    if (CFSFolder__GetDesktopIniForItem((void*)cfsFolder, pItemID,
                                        &pICachedPrivateProfile) <= 0) {
        if (pICachedPrivateProfile) {
            if (GetFolderString(pICachedPrivateProfile, L".ShellClassInfo",
                                L"IconResource", folderString, MAX_PATH) ||
                GetFolderString(pICachedPrivateProfile, L".ShellClassInfo",
                                L"IconFile", folderString, MAX_PATH)) {
                return result;
            }
            *a4 = Shell_GetStockImageIndex(SIID_PROG);
        }
    }

    return result;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    // windows.storage.dll
    const WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
        {{L"protected: long " STHISCALL
          " CFSFolder::_CreateFileFolderDefExtIcon(struct _ITEMID_CHILD const "
          "__unaligned *,struct IDFOLDER const __unaligned *,struct IBindCtx "
          "*,struct _GUID const &,void * *)"},
         &CFSFolder__CreateFileFolderDefExtIcon_orig,
         CFSFolder__CreateFileFolderDefExtIcon_hook,
         false},
        {{L"public: virtual long " STHISCALL
          " CFSFolder::GetIconOf(struct _ITEMID_CHILD const __unaligned "
          "*,unsigned int,int *)"},
         &CFSFolder_GetIconOf_orig,
         CFSFolder_GetIconOf_hook,
         false},
        {{L"protected: unsigned int " STHISCALL " CFSFolder::_GetCSIDL(void)"},
         &CFSFolder__GetCSIDL,
         nullptr,
         false},
        {{L"protected: long " STHISCALL
          " CFSFolder::_GetDesktopIniForItem(struct _ITEMIDLIST_RELATIVE const "
          "__unaligned *,struct ICachedPrivateProfile * *)"},
         &CFSFolder__GetDesktopIniForItem,
         nullptr,
         false},
        {{L"protected: struct IDFOLDER const __unaligned * " STHISCALL
          " CFSFolder::_IsValidID(struct _ITEMIDLIST_RELATIVE const "
          "__unaligned *)"},
         &CFSFolder__IsValidID,
         nullptr,
         false},
        {{L"GetFolderString"}, &GetFolderString, nullptr, false},
        {{L"int " STHISCALL
          " IsFileFolder(struct IDFOLDER const __unaligned *)"},
         &IsFileFolder,
         nullptr,
         false},
        {{L"int " STHISCALL " Shell_GetStockImageIndex(enum SHSTOCKICONID)"},
         &Shell_GetStockImageIndex,
         nullptr,
         false}};

    HMODULE hWindowsStorage = LoadLibraryW(L"windows.storage.dll");
    if (!hWindowsStorage) {
        Wh_Log(L"Failed to load windows.storage.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hWindowsStorage, windowsStorageHooks,
                                    ARRAYSIZE(windowsStorageHooks))) {
        Wh_Log(L"Failed to hook windows.storage.dll");
        return FALSE;
    }

    return TRUE;
}
