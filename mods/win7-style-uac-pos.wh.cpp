// ==WindhawkMod==
// @id              win7-style-uac-pos
// @name            Windows 7 Style UAC Window Positioning
// @description     Make UAC windows show on top of parent window
// @version         0.1
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         consent.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Style UAC Window Positioning
* Make UAC windows positioned on top of the parent window, just like in Windows 7 and Vista, even in the secure desktop, instead of being centered on the screen as in Windows 8 and later.
* **Currently, this mod is only compatible with the classic UAC!** If you are using Windows 10 or later, restore the old one with [this NTMU pack](https://get-ntmu.github.io/#!/pack/classicuac) first. The new modern XAML-based UAC is much more difficult to mod and may require a completely different approach, so I have no plans to support it for now.
* It is highly recommended to use this mod together with [Windows 7 Style UAC Dim](https://windhawk.net/mods/win7-style-uac-dim) for the best experience.
## ⚠ Important usage note ⚠

In order to use this mod, you must allow Windhawk to inject into the **consent.exe**
system process. To do so, add it to the process inclusion list in the advanced
settings. If you do not do this, it will silently fail to inject.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

BOOL g_enteringCalculateWindowPosition = FALSE;

typedef long (*CredDialog__CalculateWindowPosition_t)(void* pThis, void* a1, void* a2, void* a3);
CredDialog__CalculateWindowPosition_t CredDialog__CalculateWindowPosition_original = NULL;
long CredDialog__CalculateWindowPosition_hook(void* pThis, void* a1, void* a2, void* a3) {
    Wh_Log(L"CredDialog__CalculateWindowPosition");
    g_enteringCalculateWindowPosition = TRUE;
    long ret = CredDialog__CalculateWindowPosition_original(pThis, a1, a2, a3);
    g_enteringCalculateWindowPosition = FALSE;
    return ret;
}

typedef BOOL (*CredDialog__IsOnSecureDesktop_t)(void* pThis);
CredDialog__IsOnSecureDesktop_t CredDialog__IsOnSecureDesktop_original = NULL;
BOOL CredDialog__IsOnSecureDesktop_hook(void* pThis) {
    Wh_Log(L"CredDialog__IsOnSecureDesktop");
    if (g_enteringCalculateWindowPosition) {
        return FALSE;
    }
    return CredDialog__IsOnSecureDesktop_original(pThis);
}

typedef HWND (*CCredUI__GetHwndParent_t)(void* pThis);
CCredUI__GetHwndParent_t CCredUI__GetHwndParent_original = NULL;
HWND CCredUI__GetHwndParent_hook(void* pThis) {
    Wh_Log(L"CCredUI__GetHwndParent");
    HWND fakeClientWindow = FindWindowW(L"$$$Secure UAP Background Fake Client Window Class", L"$$$Secure UAP Background Fake Client Window");
    if (fakeClientWindow) {
        Wh_Log(L"Found fake client window, using it as parent");
        return fakeClientWindow;
    }
    return CCredUI__GetHwndParent_original(pThis);
}

const WindhawkUtils::SYMBOL_HOOK authuiDllHooks[] = {
    {
        {
            L"private: long __cdecl CCredDialog::CalculateWindowPosition(struct tagSIZE,struct tagRECT,struct tagRECT *)",
        },
        &CredDialog__CalculateWindowPosition_original,
        CredDialog__CalculateWindowPosition_hook,
        false
    },
    {
        {
            L"private: bool __cdecl CCredDialog::_IsOnSecureDesktop(void)",
        },
        &CredDialog__IsOnSecureDesktop_original,
        CredDialog__IsOnSecureDesktop_hook,
        false
    },
    {
        {
            L"public: struct HWND__ * __cdecl CCredUI::_GetHwndParent(void)",
        },
        &CCredUI__GetHwndParent_original,
        CCredUI__GetHwndParent_hook,
        false
    },
};

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    wchar_t authuiDllPath[MAX_PATH];
    wcscpy_s(authuiDllPath, L"authui.dll"); // Fallback

    // Get value of HKEY_CLASSES_ROOT\CLSID\{745a5add-6a71-47b9-9bb9-31dd3a6913d4}\InProcServer32\(Default) to support various old UAC restoration methods
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"CLSID\\{745a5add-6a71-47b9-9bb9-31dd3a6913d4}\\InProcServer32", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t dllPath[MAX_PATH];
        DWORD size = sizeof(dllPath);
        if (RegQueryValueExW(hKey, NULL, NULL, NULL, (LPBYTE)dllPath, &size) == ERROR_SUCCESS) {
            ExpandEnvironmentStringsW(dllPath, authuiDllPath, MAX_PATH);
        }
        RegCloseKey(hKey);
    }
    Wh_Log(L"authui.dll path: %s", authuiDllPath);

    HMODULE authuiDll = LoadLibraryExW(authuiDllPath, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (authuiDll) {
        if (!WindhawkUtils::HookSymbols(authuiDll, authuiDllHooks, ARRAYSIZE(authuiDllHooks))) {
            Wh_Log(L"Failed to hook symbols in authui.dll. Make sure the classic UAC is restored");
            return FALSE;
        }
    } else {
        Wh_Log(L"Failed to load authui.dll");
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
