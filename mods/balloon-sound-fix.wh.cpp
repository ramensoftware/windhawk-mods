// ==WindhawkMod==
// @id              balloon-sound-fix
// @name            Balloon Sound Fix
// @description     Makes notifciation balloons play a sound again
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Balloon Sound Fix
Windows XP played a sound when showing balloon notifications. However, this
functionality has been broken since Windows Vista. This mod fixes that issue in
Windows 10.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

typedef unsigned __int64 QWORD;

struct CTrayItemIdentity
{
    GUID m_guidItem = GUID_NULL;
    HWND m_hWnd = nullptr;
    WCHAR m_szExeName[MAX_PATH];
    UINT m_uID = 0;
    UINT m_uVersion = 3;
    UINT m_uCallbackMsg = 0;
};

struct TNINFOITEM
{
    CTrayItemIdentity identity;
    WCHAR szInfoTitle[128];
    WCHAR szInfo[256];
    DWORD dwInfoFlags;
    BOOL bRealtime;
};

struct CTrayBalloonInfoTipManager
{
    HWND m_hwndTooltip;
    IUnknown *m_events;
    void *m_trayItemController;
    TNINFOITEM *m_info;
};

DWORD (*CTrayBalloonInfoTipManager__ShowBalloonTip_orig)(CTrayBalloonInfoTipManager *, HICON, DWORD);
DWORD CTrayBalloonInfoTipManager__ShowBalloonTip_hook(
    CTrayBalloonInfoTipManager *pThis,
    HICON hIcon,
    DWORD dwLastSoundTime
)
{
    if (pThis->m_info && !(pThis->m_info->dwInfoFlags & NIIF_NOSOUND)
        && GetTickCount() - dwLastSoundTime >= 500)
    {
        WCHAR szFileName[MAX_PATH] = { 0 };
        LONG cbSize = sizeof(szFileName);

        if (ERROR_SUCCESS == RegQueryValueW(
            HKEY_CURRENT_USER, L"AppEvents\\Schemes\\Apps\\.Default\\SystemNotification\\.Current", 
            szFileName, &cbSize)
            && szFileName[0])
        {
            PlaySoundW(szFileName, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
        }
    }
    return CTrayBalloonInfoTipManager__ShowBalloonTip_orig(pThis, hIcon, dwLastSoundTime);
}

const WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
    {
        {
            L"private: unsigned long __cdecl CTrayBalloonInfoTipManager::_ShowBalloonTip(struct HICON__ *,unsigned long)"
        },
        &CTrayBalloonInfoTipManager__ShowBalloonTip_orig,
        CTrayBalloonInfoTipManager__ShowBalloonTip_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        explorerExeHooks,
        ARRAYSIZE(explorerExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook CTrayBalloonInfoTipManager::_ShowBalloonTip");
        return FALSE;
    }

    return TRUE;
}