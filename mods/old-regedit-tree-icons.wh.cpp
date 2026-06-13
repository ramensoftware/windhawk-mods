// ==WindhawkMod==
// @id              old-regedit-tree-icons
// @name            Old Regedit Tree Icons
// @description     Makes the tree view in regedit use the icons from 2000/XP
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         regedit.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Old Regedit Tree Icons
In Windows XP and before, the Registry Editor's tree view had its own icons
instead of using system icons. This mod reverts that behavior and makes it use
its own icons again.

## Configuration
You will need a copy of regedit.exe from Windows XP or before for its icons. Once
you have this, provide the path to it in the mod options.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/old-regedit-tree-icons/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/old-regedit-tree-icons/after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- regedit_path: C:\path\to\regedit.exe
  $name: Path to regedit.exe
  $description: File path to regedit.exe from Windows XP or before to get icons from
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windhawk_utils.h>

HMODULE g_hinstRegeditXP = NULL;

#define IDI_COMPUTER                    201
#define IDI_REMOTE                      202
#define IDI_FOLDER                      203
#define IDI_FOLDEROPEN                  204

#define DECLARE_HOOK_FUNCTION(RETURN_TYPE, ATTRIBUTES, NAME, ...) \
    RETURN_TYPE (ATTRIBUTES *NAME ## _orig)(__VA_ARGS__);         \
    RETURN_TYPE ATTRIBUTES NAME ## _hook(__VA_ARGS__)

DECLARE_HOOK_FUNCTION(void, WINAPI, AddSystemImageIcon,
    HIMAGELIST    himl,
    SHSTOCKICONID siid
)
{
    UINT idIcon = (UINT)-1;
    switch (siid)
    {
        case SIID_SERVER:
            idIcon = IDI_COMPUTER;
            break;
        case ((SHSTOCKICONID)14):
            idIcon = IDI_REMOTE;
            break;
        case SIID_FOLDER:
            idIcon = IDI_FOLDER;
            break;
        case SIID_FOLDEROPEN:
            idIcon = IDI_FOLDEROPEN;
            break;
    }

    if (idIcon == (UINT)-1)
        return AddSystemImageIcon_orig(himl, siid);

    HICON hIcon = (HICON)LoadImageW(
        g_hinstRegeditXP, MAKEINTRESOURCEW(idIcon),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 
        LR_DEFAULTCOLOR);
    if (hIcon)
    {
        ImageList_AddIcon(himl, hIcon);
        DestroyIcon(hIcon);
    }
}

#ifdef _WIN64
#   define STDCALL_STR L"__cdecl"
#else
#   define STDCALL_STR L"__stdcall"
#endif

const WindhawkUtils::SYMBOL_HOOK regeditExeHooks[] = {
    {
        {
            // Windows 10 (C++ linkage):
            L"void " STDCALL_STR L" AddSystemImageIcon(struct _IMAGELIST *,enum SHSTOCKICONID)",
            // Windows 7 (C linkage):
#ifdef _WIN64
            L"AddSystemImageIcon"
#else
            L"_AddSystemImageIcon@8"
#endif
        },
        &AddSystemImageIcon_orig,
        AddSystemImageIcon_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    WCHAR szRegeditPath[MAX_PATH];
    LPCWSTR pszRegeditPath = Wh_GetStringSetting(L"regedit_path");
    ExpandEnvironmentStringsW(pszRegeditPath, szRegeditPath, ARRAYSIZE(szRegeditPath));
    Wh_FreeStringSetting(pszRegeditPath);

    g_hinstRegeditXP = LoadLibraryExW(szRegeditPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (!g_hinstRegeditXP)
    {
        MessageBoxW(
            NULL,

            L"Failed to load XP/2000 regedit.exe."
            L"\n\n"
            L"Did you provide a correct path in the mod options?",

            L"Windhawk: Old Regedit Tree Icons",
            MB_ICONERROR
        );
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        regeditExeHooks,
        ARRAYSIZE(regeditExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook AddSystemImageIcon");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit(void)
{
    if (g_hinstRegeditXP)
        FreeLibrary(g_hinstRegeditXP);
}