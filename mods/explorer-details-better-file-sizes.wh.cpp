// ==WindhawkMod==
// @id              explorer-details-better-file-sizes
// @name            Better file sizes in Explorer details
// @description     Explorer always shows file sizes in KBs in details, make it show MB/GB (or, optionally, KiB/MiB/GiB) when appropriate
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @compilerOptions -lpropsys
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
# Better file sizes in Explorer details

Explorer always shows file sizes in KBs in details, make it show MB/GB when
appropriate.

Also, optionally make use the International Electronic Commission terms (e.g.
KiB instead of KB). See also: [Why does Explorer use the term KB instead of
KiB?](https://devblogs.microsoft.com/oldnewthing/20090611-00/?p=17933).

![Screenshot](https://i.imgur.com/5d28APb.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- useIecTerms: false
  $name: Use IEC terms
  $description: >-
    Use the International Electronic Commission terms, e.g. KiB instead of KB
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <propsys.h>

struct {
    bool useIecTerms;
} g_settings;

HMODULE g_propsysModule;

using PSFormatForDisplayAlloc_t = decltype(&PSFormatForDisplayAlloc);
PSFormatForDisplayAlloc_t PSFormatForDisplayAlloc_Original;
HRESULT WINAPI PSFormatForDisplayAlloc_Hook(const PROPERTYKEY& key,
                                            const PROPVARIANT& propvar,
                                            PROPDESC_FORMAT_FLAGS pdff,
                                            PWSTR* ppszDisplay) {
    void* retAddress = __builtin_return_address(0);

    auto original = [=]() {
        return PSFormatForDisplayAlloc_Original(key, propvar, pdff,
                                                ppszDisplay);
    };

    HMODULE explorerFrame = GetModuleHandle(L"explorerframe.dll");
    if (!explorerFrame) {
        return original();
    }

    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)retAddress, &module) ||
        module != explorerFrame) {
        return original();
    }

    Wh_Log(L">");

    pdff &= ~PDFF_ALWAYSKB;
    return PSFormatForDisplayAlloc_Original(key, propvar, pdff, ppszDisplay);
}

using LoadStringW_t = decltype(&LoadStringW);
LoadStringW_t LoadStringW_Original;
int WINAPI LoadStringW_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPWSTR lpBuffer,
                            int cchBufferMax) {
    int ret = LoadStringW_Original(hInstance, uID, lpBuffer, cchBufferMax);
    if (!ret || hInstance != g_propsysModule || cchBufferMax == 0) {
        return ret;
    }

    PCWSTR newStr = nullptr;
    if (wcscmp(lpBuffer, L"%s KB") == 0) {
        newStr = L"%s KiB";
    } else if (wcscmp(lpBuffer, L"%s MB") == 0) {
        newStr = L"%s MiB";
    } else if (wcscmp(lpBuffer, L"%s GB") == 0) {
        newStr = L"%s GiB";
    } else if (wcscmp(lpBuffer, L"%s TB") == 0) {
        newStr = L"%s TiB";
    } else if (wcscmp(lpBuffer, L"%s PB") == 0) {
        newStr = L"%s PiB";
    } else if (wcscmp(lpBuffer, L"%s EB") == 0) {
        newStr = L"%s EiB";
    }

    if (!newStr) {
        return ret;
    }

    Wh_Log(L"> Overriding string %u: %s -> %s", uID, lpBuffer, newStr);
    wcsncpy_s(lpBuffer, cchBufferMax, newStr, cchBufferMax - 1);
    return wcslen(lpBuffer);
}

void LoadSettings() {
    g_settings.useIecTerms = Wh_GetIntSetting(L"useIecTerms");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WindhawkUtils::Wh_SetFunctionHookT(PSFormatForDisplayAlloc,
                                       PSFormatForDisplayAlloc_Hook,
                                       &PSFormatForDisplayAlloc_Original);

    if (g_settings.useIecTerms) {
        g_propsysModule = GetModuleHandle(L"propsys.dll");
        if (!g_propsysModule) {
            Wh_Log(L"Failed");
            return FALSE;
        }

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

        auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                         PCSTR targetName, void* hookFunction,
                                         void** originalFunction) {
            void* targetFunction =
                (void*)GetProcAddress(kernelBaseModule, targetName);
            if (!targetFunction) {
                targetFunction =
                    (void*)GetProcAddress(kernel32Module, targetName);
                if (!targetFunction) {
                    return FALSE;
                }
            }

            return Wh_SetFunctionHook(targetFunction, hookFunction,
                                      originalFunction);
        };

        setKernelFunctionHook("LoadStringW", (void*)LoadStringW_Hook,
                              (void**)&LoadStringW_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}
