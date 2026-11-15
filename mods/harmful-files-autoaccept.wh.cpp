// ==WindhawkMod==
// @id              harmful-files-autoaccept
// @name            Skip harmful files confirmation
// @description     Skips the harmful-files confirmation when dragging files in Explorer.
// @version         0.3.0
// @author          Arimodu
// @github          https://github.com/Arimodu
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Skip harmful files confirmation

![The harmful-files dialog crossed out with a red X](https://raw.githubusercontent.com/Arimodu/windhawk-mod-assets/main/harmful-files-preview.png)

Skips "These files might be harmful to your computer" when dragging files in
File Explorer. Windows can show this confirmation when it classifies the source
as untrusted, including some network shares.

The mod accepts drag-source confirmations for every source, before constructing
the dialog. It targets URLACTION_SHELL_SECURE_DRAGSOURCE with URLPOLICY_QUERY.
Other actions and denial policies keep their original behavior. Copy/paste
operations using other URL actions are not covered.

To trust a specific network location instead, configure that location in
Internet Options > Security > Local intranet > Sites.

Internet zone settings and Zone.Identifier streams are unchanged. Disable the
mod to restore the confirmation. Windows DLL files are not modified.

Version 0.3.0 was tested with Explorer drag-and-drop on Windows 11 x64.
ARM64 runtime behavior remains untested. Shell symbols are required.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <urlmon.h>
#include <windhawk_utils.h>

using ShowSecurityZoneDialog_t = HRESULT(WINAPI*)(void*, DWORD, DWORD, PCWSTR,
                                                 HWND, IUnknown*, int*);
ShowSecurityZoneDialog_t ShowSecurityZoneDialog_Original = nullptr;

HRESULT WINAPI ShowSecurityZoneDialog_Hook(void* self, DWORD action, DWORD policy,
                                           PCWSTR path, HWND owner, IUnknown* site,
                                           int* result) {
    Wh_Log(L"action=0x%X policy=%u", action, policy);
    if (action == URLACTION_SHELL_SECURE_DRAGSOURCE &&
        policy == URLPOLICY_QUERY && result) {
        *result = IDOK;
        return S_OK;
    }
    return ShowSecurityZoneDialog_Original(self, action, policy, path, owner,
                                           site, result);
}

BOOL Wh_ModInit() {
    HMODULE shell = GetModuleHandleW(L"shell32.dll");
    if (!shell) return FALSE;
    // shell32.dll
    const WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"public: virtual long __cdecl CSecurityZoneChecker::ShowSecurityZoneDialog(unsigned long,unsigned long,unsigned short const *,struct HWND__ *,struct IUnknown *,int *)"},
         &ShowSecurityZoneDialog_Original, ShowSecurityZoneDialog_Hook},
    };
    return WindhawkUtils::HookSymbols(shell, symbols, ARRAYSIZE(symbols));
}