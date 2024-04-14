// ==WindhawkMod==
// @id              icon-resource-redirect
// @name            Icon Resource Redirect
// @description     Define alternative resource files for loading icons (e.g. instead of imageres.dll) for simple theming without having to modify system files
// @version         1.0.1
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

#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

std::shared_mutex g_redirectionResourcePathsMutex;
std::unordered_map<std::wstring, std::vector<std::wstring>>
    g_redirectionResourcePaths;

std::shared_mutex g_redirectionResourceModulesMutex;
std::unordered_map<std::wstring, HMODULE> g_redirectionResourceModules;

HMODULE GetRedirectedModule(std::wstring_view fileName) {
    std::wstring fileNameUpper{fileName};
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &fileNameUpper[0],
                  static_cast<int>(fileNameUpper.length()), &fileNameUpper[0],
                  static_cast<int>(fileNameUpper.length()), nullptr, nullptr,
                  0);

    {
        std::shared_lock lock{g_redirectionResourceModulesMutex};
        const auto it = g_redirectionResourceModules.find(fileNameUpper);
        if (it != g_redirectionResourceModules.end()) {
            return it->second;
        }
    }

    HINSTANCE module = LoadLibraryEx(
        fileNameUpper.c_str(), nullptr,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (!module) {
        return nullptr;
    }

    {
        std::unique_lock lock{g_redirectionResourceModulesMutex};
        g_redirectionResourceModules.try_emplace(fileNameUpper, module);
    }

    return module;
}

void FreeAndClearRedirectedModules() {
    std::unordered_map<std::wstring, HMODULE> modules;

    {
        std::unique_lock lock{g_redirectionResourceModulesMutex};
        modules.swap(g_redirectionResourceModules);
    }

    for (const auto& it : modules) {
        FreeLibrary(it.second);
    }
}

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

    {
        std::shared_lock lock{g_redirectionResourcePathsMutex};

        if (const auto it = g_redirectionResourcePaths.find(fileNameUpper);
            it != g_redirectionResourcePaths.end()) {
            const auto& redirects = it->second;
            for (const auto& redirect : redirects) {
                UINT result = PrivateExtractIconsW_Original(
                    redirect.c_str(), nIconIndex, cxIcon, cyIcon, phicon,
                    piconid, nIcons, flags);
                if (result != 0xFFFFFFFF && result != 0) {
                    Wh_Log(L"Redirected %s -> %s", szFileName,
                           redirect.c_str());
                    return result;
                }
            }
        }
    }

    return PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon, cyIcon,
                                         phicon, piconid, nIcons, flags);
}

using LoadIconWithScaleDown_t = decltype(&LoadIconWithScaleDown);
LoadIconWithScaleDown_t LoadIconWithScaleDown_Original;
HRESULT WINAPI LoadIconWithScaleDown_Hook(HINSTANCE hinst,
                                          PCWSTR pszName,
                                          int cx,
                                          int cy,
                                          HICON* phico) {
    Wh_Log(L">");

    WCHAR szFileName[MAX_PATH];
    DWORD fileNameLen =
        GetModuleFileName(hinst, szFileName, ARRAYSIZE(szFileName));
    switch (fileNameLen) {
        case 0:
        case ARRAYSIZE(szFileName):
            Wh_Log(L"GetModuleFileName failed");
            return LoadIconWithScaleDown_Original(hinst, pszName, cx, cy,
                                                  phico);
    }

    Wh_Log(L"%s", szFileName);

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &szFileName[0],
                  fileNameLen, &szFileName[0], fileNameLen, nullptr, nullptr,
                  0);

    {
        std::shared_lock lock{g_redirectionResourcePathsMutex};

        if (const auto it = g_redirectionResourcePaths.find(szFileName);
            it != g_redirectionResourcePaths.end()) {
            const auto& redirects = it->second;
            for (const auto& redirect : redirects) {
                HINSTANCE hinstRedirect = GetRedirectedModule(redirect);
                if (!hinstRedirect) {
                    Wh_Log(L"GetRedirectedModule failed for %s",
                           redirect.c_str());
                    continue;
                }

                UINT result = LoadIconWithScaleDown_Original(
                    hinstRedirect, pszName, cx, cy, phico);
                if (result == S_OK) {
                    Wh_Log(L"Redirected %s -> %s", szFileName,
                           redirect.c_str());
                    return result;
                }
            }
        }
    }

    return LoadIconWithScaleDown_Original(hinst, pszName, cx, cy, phico);
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

    std::unique_lock lock{g_redirectionResourcePathsMutex};
    g_redirectionResourcePaths = std::move(paths);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)PrivateExtractIconsW,
                       (void*)PrivateExtractIconsW_Hook,
                       (void**)&PrivateExtractIconsW_Original);

    HMODULE user32Module = LoadLibrary(L"comctl32.dll");
    if (user32Module) {
        auto* pLoadIconWithScaleDown =
            (decltype(&LoadIconWithScaleDown))GetProcAddress(
                user32Module, "LoadIconWithScaleDown");
        if (pLoadIconWithScaleDown) {
            Wh_SetFunctionHook((void*)pLoadIconWithScaleDown,
                               (void*)LoadIconWithScaleDown_Hook,
                               (void**)&LoadIconWithScaleDown_Original);
        } else {
            Wh_Log(L"Failed to find LoadIconWithScaleDown");
        }
    } else {
        Wh_Log(L"Failed to load user32.dll");
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    FreeAndClearRedirectedModules();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    FreeAndClearRedirectedModules();

    // Invalidate icon cache.
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}
