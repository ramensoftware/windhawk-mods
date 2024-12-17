// ==WindhawkMod==
// @id              taskbar-thumbnails
// @name            Disable Taskbar Thumbnails
// @description     Disable taskbar thumbnails on hover, or replace them with a list
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Disable Taskbar Thumbnails

Disable taskbar thumbnails on hover, or replace them with a list.

Only Windows 10 64-bit and Windows 11 are supported. For other Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.

![Demonstration](https://i.imgur.com/62DSgxs.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: disabled
  $name: Preview on hover
  $options:
  - disabled: Disabled
  - list: List
  - thumbnails: Thumbnails
- noTooltips: false
  $name: Disable tooltips on hover
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

enum class Mode {
    disabled,
    list,
    thumbnails,
};

struct {
    Mode mode;
    bool noTooltips;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

using CTaskListWnd__DisplayExtendedUI_t = HRESULT(WINAPI*)(void* pThis,
                                                           void* taskBtnGroup,
                                                           int param2,
                                                           DWORD flags,
                                                           int param4);
CTaskListWnd__DisplayExtendedUI_t CTaskListWnd__DisplayExtendedUI_Original;
HRESULT WINAPI CTaskListWnd__DisplayExtendedUI_Hook(void* pThis,
                                                    void* taskBtnGroup,
                                                    int param2,
                                                    DWORD flags,
                                                    int param4) {
    Wh_Log(L"> %x", flags);

    bool persistent = flags & 2;
    if (!persistent && g_settings.mode == Mode::disabled) {
        return S_OK;
    }

    HRESULT ret = CTaskListWnd__DisplayExtendedUI_Original(
        pThis, taskBtnGroup, param2, flags, param4);

    return ret;
}

using CTaskListThumbnailWnd__CanShowThumbnails_t = BOOL(WINAPI*)(void* pThis,
                                                                 void* param1,
                                                                 int param2,
                                                                 int param3);
CTaskListThumbnailWnd__CanShowThumbnails_t
    CTaskListThumbnailWnd__CanShowThumbnails_Original;
BOOL WINAPI CTaskListThumbnailWnd__CanShowThumbnails_Hook(void* pThis,
                                                          void* param1,
                                                          int param2,
                                                          int param3) {
    Wh_Log(L">");

    if (g_settings.mode == Mode::list) {
        return FALSE;
    }

    BOOL ret = CTaskListThumbnailWnd__CanShowThumbnails_Original(
        pThis, param1, param2, param3);

    return ret;
}

using CTaskListWnd__ShowToolTip_t = void(WINAPI*)(void* pThis, DWORD flags);
CTaskListWnd__ShowToolTip_t CTaskListWnd__ShowToolTip_Original;
void WINAPI CTaskListWnd__ShowToolTip_Hook(void* pThis, DWORD flags) {
    Wh_Log(L"> %x", flags);

    if (g_settings.noTooltips) {
        return;
    }

    CTaskListWnd__ShowToolTip_Original(pThis, flags);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else {
                return WinVersion::Win11;
            }
            break;
    }

    return WinVersion::Unsupported;
}

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode = Mode::disabled;
    if (wcscmp(mode, L"list") == 0) {
        g_settings.mode = Mode::list;
    } else if (wcscmp(mode, L"thumbnails") == 0) {
        g_settings.mode = Mode::thumbnails;
    }
    Wh_FreeStringSetting(mode);

    g_settings.noTooltips = Wh_GetIntSetting(L"noTooltips");

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_winVersion >= WinVersion::Win11 && g_settings.oldTaskbarOnWin11) {
        g_winVersion = WinVersion::Win10;
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(protected: long __cdecl CTaskListWnd::_DisplayExtendedUI(struct ITaskBtnGroup *,int,unsigned long,int))",
            },
            &CTaskListWnd__DisplayExtendedUI_Original,
            CTaskListWnd__DisplayExtendedUI_Hook,
        },
        {
            {
                LR"(private: int __cdecl CTaskListThumbnailWnd::_CanShowThumbnails(class CDPA<struct ITaskThumbnail,class CTContainer_PolicyUnOwned<struct ITaskThumbnail> > const *,int,int))",
            },
            &CTaskListThumbnailWnd__CanShowThumbnails_Original,
            CTaskListThumbnailWnd__CanShowThumbnails_Hook,
        },
        {
            {
                LR"(protected: void __cdecl CTaskListWnd::_ShowToolTip(enum ShowToolTipFlags))",
            },
            &CTaskListWnd__ShowToolTip_Original,
            CTaskListWnd__ShowToolTip_Hook,
        },
    };

    if (g_winVersion <= WinVersion::Win10) {
        if (!HookSymbols(GetModuleHandle(nullptr), symbolHooks,
                         ARRAYSIZE(symbolHooks))) {
            return FALSE;
        }
    } else {
        HMODULE taskbarModule = LoadLibrary(L"taskbar.dll");
        if (!taskbarModule) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return FALSE;
        }

        if (!HookSymbols(taskbarModule, symbolHooks, ARRAYSIZE(symbolHooks))) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
