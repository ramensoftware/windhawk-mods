// ==WindhawkMod==
// @id              two-sided-snapping
// @name            Two-sided snapping in Win11
// @description     Disables Windows corner snap (quadrants) while keeping snap to the sides.
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Disable Aero Snap Quadrants

This mod disables the corner snapping and enables the two-sided snapping, thus
restoring the behavior before Windows 10. This mod works only on Windows 11. 
Restart File Explorer after enabling.

This mod is ported from Explorer Patcher.

*/
// ==/WindhawkModReadme==

static HRESULT(WINAPI *SLGetWindowsInformationDWORDFunc)(PCWSTR pwszValueName, DWORD* pdwValue) = NULL;

HRESULT WINAPI windowsudkshellcommon_SLGetWindowsInformationDWORDHook(PCWSTR pwszValueName, DWORD* pdwValue)
{
    HRESULT hr = SLGetWindowsInformationDWORDFunc(pwszValueName, pdwValue);

    if (SUCCEEDED(hr) &&(!wcsncmp(pwszValueName, L"Shell-Windowing-LimitSnappedWindows", 35)))
        *pdwValue = 1;

    return hr;
}


BOOL Wh_ModInit() {
    
    Wh_SetFunctionHook(
        (void*)GetProcAddress(LoadLibrary(L"Slc.dll"), "SLGetWindowsInformationDWORD"),
        (void*)windowsudkshellcommon_SLGetWindowsInformationDWORDHook,
        (void**)&SLGetWindowsInformationDWORDFunc
    );

    return TRUE;
}

