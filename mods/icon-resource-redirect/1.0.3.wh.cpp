// ==WindhawkMod==
// @id              icon-resource-redirect
// @name            Icon Resource Redirect
// @description     Define alternative resource files for loading icons (e.g. instead of imageres.dll) for simple theming without having to modify system files
// @version         1.0.3
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

#include <psapi.h>
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

bool DevicePathToDosPath(const WCHAR* device_path,
                         WCHAR* dos_path,
                         size_t dos_path_size) {
    WCHAR drive_strings[MAX_PATH];
    if (!GetLogicalDriveStrings(ARRAYSIZE(drive_strings), drive_strings)) {
        return false;
    }

    // Drive strings are stored as a set of null terminated strings, with an
    // extra null after the last string. Each drive string is of the form "C:\".
    // We convert it to the form "C:", which is the format expected by
    // QueryDosDevice().
    WCHAR drive_colon[3] = L" :";
    for (const WCHAR* next_drive_letter = drive_strings; *next_drive_letter;
         next_drive_letter += wcslen(next_drive_letter) + 1) {
        // Dos device of the form "C:".
        *drive_colon = *next_drive_letter;
        WCHAR device_name[MAX_PATH];
        if (!QueryDosDevice(drive_colon, device_name, ARRAYSIZE(device_name))) {
            continue;
        }

        size_t name_length = wcslen(device_name);
        if (_wcsnicmp(device_path, device_name, name_length) == 0) {
            size_t dos_path_size_required =
                2 + wcslen(device_path + name_length) + 1;
            if (dos_path_size < dos_path_size_required) {
                return false;
            }

            // Construct DOS path.
            wcscpy(dos_path, drive_colon);
            wcscat(dos_path, device_path + name_length);
            return true;
        }
    }

    return false;
}

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
        DWORD dwError = GetLastError();
        Wh_Log(L"LoadLibraryEx failed with error %u", dwError);
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
    Wh_Log(L"> szFileName: %s", szFileName);

    std::wstring fileNameUpper{szFileName};
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &fileNameUpper[0],
                  static_cast<int>(fileNameUpper.length()), &fileNameUpper[0],
                  static_cast<int>(fileNameUpper.length()), nullptr, nullptr,
                  0);

    bool triedRedirection = false;

    {
        std::shared_lock lock{g_redirectionResourcePathsMutex};

        if (const auto it = g_redirectionResourcePaths.find(fileNameUpper);
            it != g_redirectionResourcePaths.end()) {
            const auto& redirects = it->second;
            for (const auto& redirect : redirects) {
                if (!triedRedirection) {
                    Wh_Log(L"nIconIndex: %d", nIconIndex);
                    Wh_Log(L"cxIcon: %d", cxIcon);
                    Wh_Log(L"cyIcon: %d", cyIcon);
                    Wh_Log(L"phicon: %d", !!phicon);
                    Wh_Log(L"piconid: %d", !!piconid);
                    Wh_Log(L"nIcons: %u", nIcons);
                    Wh_Log(L"flags: 0x%08X", flags);
                    triedRedirection = true;
                }

                Wh_Log(L"Trying %s", redirect.c_str());

                UINT result = PrivateExtractIconsW_Original(
                    redirect.c_str(), nIconIndex, cxIcon, cyIcon, phicon,
                    piconid, nIcons, flags);
                if (result != 0xFFFFFFFF && result != 0) {
                    Wh_Log(L"Redirected successfully, result: %u", result);
                    return result;
                }

                Wh_Log(L"Redirection failed, result: %u", result);
            }
        }
    }

    if (triedRedirection) {
        Wh_Log(L"No redirection succeeded, falling back to original");
    }

    return PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon, cyIcon,
                                         phicon, piconid, nIcons, flags);
}

using LoadImageW_t = decltype(&LoadImageW);
LoadImageW_t LoadImageW_Original;
HANDLE WINAPI LoadImageW_Hook(HINSTANCE hInst,
                              LPCWSTR name,
                              UINT type,
                              int cx,
                              int cy,
                              UINT fuLoad) {
    auto original = [&]() {
        return LoadImageW_Original(hInst, name, type, cx, cy, fuLoad);
    };

    WCHAR szFileName[MAX_PATH];
    DWORD fileNameLen;
    if ((ULONG_PTR)hInst & 3) {
        WCHAR szNtFileName[MAX_PATH * 2];

        if (!GetMappedFileName(GetCurrentProcess(), (void*)hInst, szNtFileName,
                               ARRAYSIZE(szNtFileName))) {
            DWORD dwError = GetLastError();
            Wh_Log(L"> GetMappedFileName(%p) failed with error %u", hInst,
                   dwError);
            return original();
        }

        if (!DevicePathToDosPath(szNtFileName, szFileName,
                                 ARRAYSIZE(szFileName))) {
            Wh_Log(L"> DevicePathToDosPath failed");
            return original();
        }

        fileNameLen = wcslen(szFileName);
    } else {
        fileNameLen = GetModuleFileName((HINSTANCE)((ULONG_PTR)hInst & ~3),
                                        szFileName, ARRAYSIZE(szFileName));
        switch (fileNameLen) {
            case 0: {
                DWORD dwError = GetLastError();
                Wh_Log(L"> GetModuleFileName(%p) failed with error %u", hInst,
                       dwError);
                return original();
            }

            case ARRAYSIZE(szFileName):
                Wh_Log(L"> GetModuleFileName(%p) failed, name too long", hInst);
                return original();
        }
    }

    Wh_Log(L"> Module: %s", szFileName);

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &szFileName[0],
                  fileNameLen, &szFileName[0], fileNameLen, nullptr, nullptr,
                  0);

    bool triedRedirection = false;

    {
        std::shared_lock lock{g_redirectionResourcePathsMutex};

        if (const auto it = g_redirectionResourcePaths.find(szFileName);
            it != g_redirectionResourcePaths.end()) {
            const auto& redirects = it->second;
            for (const auto& redirect : redirects) {
                if (!triedRedirection) {
                    switch (type) {
                        case IMAGE_BITMAP:
                            Wh_Log(L"Resource type: Bitmap");
                            break;
                        case IMAGE_ICON:
                            Wh_Log(L"Resource type: Icon");
                            break;
                        case IMAGE_CURSOR:
                            Wh_Log(L"Resource type: Cursor");
                            break;
                        default:
                            Wh_Log(L"Resource type: %u", type);
                            break;
                    }

                    if (IS_INTRESOURCE(name)) {
                        Wh_Log(L"Resource number: %u", (DWORD)(ULONG_PTR)name);
                    } else {
                        Wh_Log(L"Resource name: %s", name);
                    }

                    Wh_Log(L"Width: %d", cx);
                    Wh_Log(L"Height: %d", cy);
                    Wh_Log(L"Flags: 0x%08X", fuLoad);

                    triedRedirection = true;
                }

                Wh_Log(L"Trying %s", redirect.c_str());

                HINSTANCE hinstRedirect = GetRedirectedModule(redirect);
                if (!hinstRedirect) {
                    Wh_Log(L"GetRedirectedModule failed");
                    continue;
                }

                HANDLE result = LoadImageW_Original(hinstRedirect, name, type,
                                                    cx, cy, fuLoad);
                if (result) {
                    Wh_Log(L"Redirected successfully");
                    return result;
                }

                DWORD dwError = GetLastError();
                Wh_Log(L"LoadLibraryEx failed with error %u", dwError);
            }
        }
    }

    if (triedRedirection) {
        Wh_Log(L"No redirection succeeded, falling back to original");
    }

    return original();
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

    Wh_SetFunctionHook((void*)LoadImageW, (void*)LoadImageW_Hook,
                       (void**)&LoadImageW_Original);

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
