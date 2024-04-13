// ==WindhawkMod==
// @id              icon-resource-redirect
// @name            Icon Resource Redirect
// @description     Define alternative resource files for loading icons (e.g. instead of imageres.dll) for simple theming without having to modify system files
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Icon Resource Redirect

Define alternative resource files for loading icons (e.g. instead of
imageres.dll) for simple theming without having to modify system files.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- redirectionResourcePaths:
  - - original: 'C:\Windows\System32\imageres.dll'
      $name: The original resource file
      $description: The original file from which icons are loaded
    - redirect: 'C:\my-themes\theme-1\imageres.dll'
      $name: The custom resource file
      $description: The custom resource file that will be used instead
  $name: Redirection resource paths
*/
// ==/WindhawkModSettings==

#include <shlobj.h>

#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::wstring, std::vector<std::wstring>>
    g_redirectionResourcePaths;

using PrivateExtractIconsW_t = decltype(&PrivateExtractIconsW);
PrivateExtractIconsW_t PrivateExtractIconsW_Original;
UINT WINAPI PrivateExtractIconsW_Hook(LPCWSTR szFileName,
                                      int nIconIndex,
                                      int cxIcon,
                                      int cyIcon,
                                      HICON* phicon,
                                      UINT* piconid,
                                      UINT nIcons,
                                      UINT flags) {
    Wh_Log(L"> %s", szFileName);

    std::wstring fileNameUpper{szFileName};
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &fileNameUpper[0],
                  static_cast<int>(fileNameUpper.length()), &fileNameUpper[0],
                  static_cast<int>(fileNameUpper.length()), nullptr, nullptr,
                  0);

    if (const auto it = g_redirectionResourcePaths.find(fileNameUpper);
        it != g_redirectionResourcePaths.end()) {
        const auto& redirects = it->second;
        for (const auto& redirect : redirects) {
            UINT result = PrivateExtractIconsW_Original(
                redirect.c_str(), nIconIndex, cxIcon, cyIcon, phicon, piconid,
                nIcons, flags);
            bool succeeded = result != (phicon ? 0xFFFFFFFF : 0);
            if (succeeded) {
                Wh_Log(L"Redirected %s -> %s", szFileName, redirect.c_str());
                return result;
            }
        }
    }

    return PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon, cyIcon,
                                         phicon, piconid, nIcons, flags);
}

void LoadSettings() {
    std::unordered_map<std::wstring, std::vector<std::wstring>> paths;

    for (int i = 0;; i++) {
        PCWSTR original =
            Wh_GetStringSetting(L"redirectionResourcePaths[%d].original", i);
        PCWSTR redirect =
            Wh_GetStringSetting(L"redirectionResourcePaths[%d].redirect", i);

        bool hasName = *original || *redirect;

        if (hasName) {
            std::wstring originalUpper{original};
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &originalUpper[0],
                static_cast<int>(originalUpper.length()), &originalUpper[0],
                static_cast<int>(originalUpper.length()), nullptr, nullptr, 0);

            paths[originalUpper].push_back(redirect);
        }

        Wh_FreeStringSetting(original);
        Wh_FreeStringSetting(redirect);

        if (!hasName) {
            break;
        }
    }

    g_redirectionResourcePaths = std::move(paths);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)PrivateExtractIconsW,
                       (void*)PrivateExtractIconsW_Hook,
                       (void**)&PrivateExtractIconsW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    // Invalidate icon cache.
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}
