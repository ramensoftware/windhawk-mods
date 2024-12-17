// ==WindhawkMod==
// @id              explorer-context-menu-classic
// @name            Classic context menu on Windows 11
// @description     Always show the classic context menu without having to select "Show More Options" or hold Shift
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic context menu on Windows 11

Always show the classic context menu without having to select "Show More
Options" or hold Shift.

You can hold the Ctrl key to temporarily disable the mod and open the new menu.

![Demonstration](https://i.imgur.com/GIdzI5V.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- overrideWithCtrl: true
  $name: Override with Ctrl
  $description: >-
    If enabled, you can hold the Ctrl key to temporarily disable the mod and
    open the new menu
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shlwapi.h>

struct {
    bool overrideWithCtrl;
} g_settings;

using IUnknown_QueryService_t = decltype(&IUnknown_QueryService);
IUnknown_QueryService_t IUnknown_QueryService_Original;
HRESULT WINAPI IUnknown_QueryService_Hook(IUnknown* punk,
                                          const GUID& guidService,
                                          const IID& riid,
                                          void** ppvOut) {
    // {B306C5B1-B4F2-473C-B6FF-701B246CE2D2}
    constexpr GUID guidServiceTarget = {
        0xb306c5b1,
        0xb4f2,
        0x473c,
        {0xb6, 0xff, 0x70, 0x1b, 0x24, 0x6c, 0xe2, 0xd2}};

    // {706461D1-AC5F-4730-BFE3-CAC6CAD5EF5E}
    constexpr GUID riidTarget = {
        0x706461d1,
        0xac5f,
        0x4730,
        {0xbf, 0xe3, 0xca, 0xc6, 0xca, 0xd5, 0xef, 0x5e}};

    if (IsEqualGUID(guidService, guidServiceTarget) &&
        IsEqualGUID(riid, riidTarget)) {
        Wh_Log(L">");

        if (g_settings.overrideWithCtrl && GetKeyState(VK_CONTROL) < 0) {
            // Temporarily off.
        } else {
            Wh_Log(L"Disallowing new menu");
            return E_FAIL;
        }
    }

    return IUnknown_QueryService_Original(punk, guidService, riid, ppvOut);
}

using CNscTree_ShouldShowMiniMenu_t = bool(WINAPI*)(void* pThis, void* param1);
CNscTree_ShouldShowMiniMenu_t CNscTree_ShouldShowMiniMenu_Original;
bool WINAPI CNscTree_ShouldShowMiniMenu_Hook(void* pThis, void* param1) {
    Wh_Log(L">");

    if (g_settings.overrideWithCtrl && GetKeyState(VK_CONTROL) < 0) {
        // Temporarily off.
    } else {
        Wh_Log(L"Disallowing new menu");
        return false;
    }

    return CNscTree_ShouldShowMiniMenu_Original(pThis, param1);
}

bool HookExplorerFrameSymbols() {
    HMODULE module = LoadLibrary(L"explorerframe.dll");
    if (!module) {
        Wh_Log(L"Couldn't load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {LR"(private: bool __cdecl CNscTree::ShouldShowMiniMenu(struct _TREEITEM *))"},
            &CNscTree_ShouldShowMiniMenu_Original,
            CNscTree_ShouldShowMiniMenu_Hook,
            true,
        },
    };

    return HookSymbols(module, explorerFrameDllHooks,
                       ARRAYSIZE(explorerFrameDllHooks));
}

void LoadSettings() {
    g_settings.overrideWithCtrl = Wh_GetIntSetting(L"overrideWithCtrl");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookExplorerFrameSymbols()) {
        Wh_Log(L"Error hooking explorer frame symbols");
        return FALSE;
    }

    HMODULE shcoreModule = LoadLibrary(L"shcore.dll");
    if (!shcoreModule) {
        Wh_Log(L"Error loading shcore.dll");
        return FALSE;
    }

    IUnknown_QueryService_t pIUnknown_QueryService =
        (IUnknown_QueryService_t)GetProcAddress(shcoreModule,
                                                "IUnknown_QueryService");
    if (!pIUnknown_QueryService) {
        Wh_Log(L"Error getting IUnknown_QueryService");
        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(pIUnknown_QueryService,
                                       IUnknown_QueryService_Hook,
                                       &IUnknown_QueryService_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
