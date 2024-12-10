// ==WindhawkMod==
// @id              all-resource-redirect-new
// @name            All Resource Redirect (new)
// @description     An alternative Mod define for loading resources (e.g. instead of imageres.dll) for simple theming without having to modify system files.
// @version         1.0 (new)
// @author          André_Goressi
// @include         *
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Icon Resource Redirect

Define alternative resource files for loading icons (e.g. instead of
imageres.dll) for simple theming without having to modify system files.

## Theme folder

A theme folder can be selected in the settings. It's a folder with alternative
resource files, and the `theme.ini` file that contains redirection rules. For
example, the `theme.ini` file may contain the following:

```
[redirections]
%SystemRoot%\explorer.exe=explorer.exe.dll
%SystemRoot%\System32\imageres.dll=imageres.dll
```

In this case, the folder must also contain the: `explorer.exe`, `imageres.dll`, 
files which will be used as the custom resource files.

## Supported resource types and loading methods

The mod started with icon redirection, but was extended with time, and now it
supports the following resource types and loading methods:

* Icons, Cursors, Bitmaps, Menus, Dialogs, Strings,
* GDI+ images (e.g. PNGs),
* DirectUI resources (usually UIFILE and XML), 
* AVI (Audio Video Interleave) resource files ".avi),

* Credits / Thanks to:

Michael (Ramen Software) aka Gigi D,Agostino ;) "The creator of the original Mod" :),
WildByDesign "for adding the theme.ini function" :).

 
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- themeFolder: ''
  $name: Theme folder
  $description: A folder with alternative resource files and theme.ini
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <functional>
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

bool RedirectModule(HINSTANCE hInstance,
                    std::function<void()> beforeFirstRedirectionFunction,
                    std::function<bool(HINSTANCE)> redirectFunction) {
    WCHAR szFileName[MAX_PATH];
    DWORD fileNameLen;
    if ((ULONG_PTR)hInstance & 3) {
        WCHAR szNtFileName[MAX_PATH * 2];

        if (!GetMappedFileName(GetCurrentProcess(), (void*)hInstance,
                               szNtFileName, ARRAYSIZE(szNtFileName))) {
            DWORD dwError = GetLastError();
            Wh_Log(L"> GetMappedFileName(%p) failed with error %u", hInstance,
                   dwError);
            return false;
        }

        if (!DevicePathToDosPath(szNtFileName, szFileName,
                                 ARRAYSIZE(szFileName))) {
            Wh_Log(L"> DevicePathToDosPath failed");
            return false;
        }

        fileNameLen = wcslen(szFileName);
    } else {
        fileNameLen =
            GetModuleFileName(hInstance, szFileName, ARRAYSIZE(szFileName));
        switch (fileNameLen) {
            case 0: {
                DWORD dwError = GetLastError();
                Wh_Log(L"> GetModuleFileName(%p) failed with error %u",
                       hInstance, dwError);
                return false;
            }

            case ARRAYSIZE(szFileName):
                Wh_Log(L"> GetModuleFileName(%p) failed, name too long",
                       hInstance);
                return false;
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
                    beforeFirstRedirectionFunction();
                    triedRedirection = true;
                }

                Wh_Log(L"Trying %s", redirect.c_str());

                HINSTANCE hInstanceRedirect = GetRedirectedModule(redirect);
                if (!hInstanceRedirect) {
                    Wh_Log(L"GetRedirectedModule failed");
                    continue;
                }

                if (redirectFunction(hInstanceRedirect)) {
                    return true;
                }
            }
        }
    }

    if (triedRedirection) {
        Wh_Log(L"No redirection succeeded, falling back to original");
    }

    return false;
}

using LdrAccessResource_t = NTSTATUS(WINAPI*)(HINSTANCE hInstance,
                                              PVOID param2,
                                              PVOID param3,
                                              PVOID param4);
LdrAccessResource_t LdrAccessResource_Original;
NTSTATUS WINAPI LdrAccessResource_Hook(HINSTANCE hInstance,
                                       PVOID param2,
                                       PVOID param3,
                                       PVOID param4) {
    Wh_Log(L">");

    NTSTATUS result;

    bool redirected = RedirectModule(
        hInstance, [&]() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = LdrAccessResource_Original(
                hInstanceRedirect, param2, param3, param4);
            if (result >= 0) {
                Wh_Log(L"Redirected successfully");
                return true;
            }

            Wh_Log(L"LdrAccessResource failed with error %08X",
                   result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return LdrAccessResource_Original(hInstance, param2, param3, param4);
}

using LdrpAccessResourceData_t = NTSTATUS(WINAPI*)(HINSTANCE hInstance,
                                                   PVOID param2,
                                                   PVOID param3,
                                                   PVOID param4);
LdrpAccessResourceData_t LdrpAccessResourceData_Original;
NTSTATUS WINAPI LdrpAccessResourceData_Hook(HINSTANCE hInstance,
                                            PVOID param2,
                                            PVOID param3,
                                            PVOID param4) {
    Wh_Log(L">");

    NTSTATUS result;

    bool redirected = RedirectModule(
        hInstance, [&]() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = LdrpAccessResourceData_Original(
                hInstanceRedirect, param2, param3, param4);
            if (result >= 0) {
                Wh_Log(L"Redirected successfully");
                return true;
            }

            Wh_Log(L"LdrpAccessResourceData failed with error %08X", 
                   result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return LdrpAccessResourceData_Original(hInstance, param2, param3, param4);
}

using LdrpAccessResourceDataNoMultipleLanguage_t = NTSTATUS(WINAPI*)(HINSTANCE hInstance, 
                                                                     PVOID param2, 
                                                                     PVOID param3, 
                                                                     PVOID param4);
LdrpAccessResourceDataNoMultipleLanguage_t LdrpAccessResourceDataNoMultipleLanguage_Original;
NTSTATUS WINAPI LdrpAccessResourceDataNoMultipleLanguage_Hook(HINSTANCE hInstance,
                                                              PVOID param2,
                                                              PVOID param3,
                                                              PVOID param4) {
    Wh_Log(L">");

    NTSTATUS result;

    bool redirected = RedirectModule(
        hInstance, [&]() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = LdrpAccessResourceDataNoMultipleLanguage_Original(
                hInstanceRedirect, param2, param3, param4);
            if (result >= 0) {
                Wh_Log(L"Redirected successfully");
                return true;
            }

            Wh_Log(
                L"LdrpAccessResourceDataNoMultipleLanguage failed with error "
                L"%08X",
                result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return LdrpAccessResourceDataNoMultipleLanguage_Original(hInstance, param2, param3, param4);
}

using LdrFindResource_U_t = NTSTATUS(WINAPI*)(HINSTANCE hInstance,
                                              PVOID param2,
                                              PVOID param3,
                                              PVOID param4);
LdrFindResource_U_t LdrFindResource_U_Original;
NTSTATUS WINAPI LdrFindResource_U_Hook(HINSTANCE hInstance,
                                       PVOID param2,
                                       PVOID param3,
                                       PVOID param4) {
    Wh_Log(L">");

    NTSTATUS result;

    bool redirected = RedirectModule(
        hInstance, [&]() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = LdrFindResource_U_Original(
                hInstanceRedirect, param2, param3, param4);
            if (result >= 0) {
                Wh_Log(L"Redirected successfully");
                return true;
            }

            Wh_Log(L"LdrFindResource_U failed with error %08X",
                   result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return LdrFindResource_U_Original(hInstance, param2, param3, param4);
}

using LdrFindResourceEx_U_t = NTSTATUS(WINAPI*)(PVOID param1,
                                                HINSTANCE hInstance,
                                                PVOID param3,
                                                PVOID param4,
                                                PVOID param5);
LdrFindResourceEx_U_t LdrFindResourceEx_U_Original;
NTSTATUS WINAPI LdrFindResourceEx_U_Hook(PVOID param1, 
                                         HINSTANCE hInstance,
                                         PVOID param3,
                                         PVOID param4,
                                         PVOID param5) {
    Wh_Log(L">");

    NTSTATUS result;

    bool redirected = RedirectModule(
        hInstance, [&]() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = LdrFindResourceEx_U_Original(
                param1, hInstanceRedirect, param3, param4, param5);
            if (result >= 0) {
                Wh_Log(L"Redirected successfully");
                return true;
            }

            Wh_Log(L"LdrFindResourceEx_U failed with error %08X",
                   result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return LdrFindResourceEx_U_Original(param1, hInstance, param3, param4, param5);
}

using LdrFindResourceDirectory_U_t = NTSTATUS(WINAPI*)(HINSTANCE hInstance,
                                                       PVOID param2,
                                                       PVOID param3,
                                                       PVOID param4);
LdrFindResourceDirectory_U_t LdrFindResourceDirectory_U_Original;
NTSTATUS WINAPI LdrFindResourceDirectory_U_Hook(HINSTANCE hInstance,
                                                PVOID param2,
                                                PVOID param3,
                                                PVOID param4) {
    Wh_Log(L">");

    NTSTATUS result;

    bool redirected = RedirectModule(
        hInstance, [&]() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = LdrFindResourceDirectory_U_Original(
                hInstanceRedirect, param2, param3, param4);
            if (result >= 0) {
                Wh_Log(L"Redirected successfully");
                return true;
            }

            Wh_Log(L"LdrFindResourceDirectory_U failed with error %08X",
                   result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return LdrFindResourceDirectory_U_Original(hInstance, param2, param3, param4);
}

using LdrpSearchResourceSection_U_t = NTSTATUS(WINAPI*)(HINSTANCE hInstance,
                                                        PVOID param2,
                                                        PVOID param3,
                                                        PVOID param4,
                                                        PVOID param5);
LdrpSearchResourceSection_U_t LdrpSearchResourceSection_U_Original;
NTSTATUS WINAPI LdrpSearchResourceSection_U_Hook(HINSTANCE hInstance,
                                                 PVOID param2,
                                                 PVOID param3,
                                                 PVOID param4,
                                                 PVOID param5) {
    Wh_Log(L">");

    NTSTATUS result;

    bool redirected = RedirectModule(
        hInstance, [&]() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = LdrpSearchResourceSection_U_Original(
                hInstanceRedirect, param2, param3, param4, param5);
            if (result >= 0) {
                Wh_Log(L"Redirected successfully");
                return true;
            }

            Wh_Log(L"LdrpSearchResourceSection_U failed with error %08X",
                   result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return LdrpSearchResourceSection_U_Original(hInstance, param2, param3, param4, param5);
}

void LoadSettings() {
    std::unordered_map<std::wstring, std::vector<std::wstring>> paths;

    auto addRedirectionPath = [&paths](PCWSTR original, PCWSTR redirect) {
        WCHAR originalExpanded[MAX_PATH];
        DWORD originalExpandedLen = ExpandEnvironmentStrings(
            original, originalExpanded, ARRAYSIZE(originalExpanded));
        if (!originalExpandedLen ||
            originalExpandedLen > ARRAYSIZE(originalExpanded)) {
            Wh_Log(L"Failed to expand path: %s", original);
            return;
        }

        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      originalExpanded, originalExpandedLen - 1,
                      originalExpanded, originalExpandedLen - 1, nullptr,
                      nullptr, 0);

        Wh_Log(L"Configuring %s->%s", originalExpanded, redirect);

        paths[originalExpanded].push_back(redirect);
    };

    PCWSTR themeFolder = Wh_GetStringSetting(L"themeFolder");

    if (*themeFolder) {
        WCHAR themeIniFile[MAX_PATH];
        if (PathCombine(themeIniFile, themeFolder, L"theme.ini")) {
            std::wstring data(32768, L'\0');
            DWORD result = GetPrivateProfileSection(
                L"redirections", data.data(), data.size(), themeIniFile);
            if (result != data.size() - 2) {
                for (auto* p = data.data(); *p;) {
                    auto* pNext = p + wcslen(p) + 1;
                    auto* pEq = wcschr(p, L'=');
                    if (pEq) {
                        *pEq = L'\0';

                        WCHAR redirectFile[MAX_PATH];
                        if (PathCombine(redirectFile, themeFolder, pEq + 1)) {
                            addRedirectionPath(p, redirectFile);
                        }
                    } else {
                        Wh_Log(L"Skipping %s", p);
                    }

                    p = pNext;
                }
            } else {
                Wh_Log(L"Failed to read theme file");
            }
        }
    }

    Wh_FreeStringSetting(themeFolder);

    for (int i = 0;; i++) {
        PCWSTR original =
            Wh_GetStringSetting(L"redirectionResourcePaths[%d].original", i);
        PCWSTR redirect =
            Wh_GetStringSetting(L"redirectionResourcePaths[%d].redirect", i);

        bool hasRedirection = *original || *redirect;

        if (hasRedirection) {
            addRedirectionPath(original, redirect);
        }

        Wh_FreeStringSetting(original);
        Wh_FreeStringSetting(redirect);

        if (!hasRedirection) {
            break;
        }
    }

    std::unique_lock lock{g_redirectionResourcePathsMutex};
    g_redirectionResourcePaths = std::move(paths);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE ntdllModule = GetModuleHandle(L"ntdll.dll");
    if (!ntdllModule) {
        Wh_Log(L"Couldn't load ntdll.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {L"LdrAccessResource"},
            (void**)&LdrAccessResource_Original,
            (void*)LdrAccessResource_Hook,
            true,
        },
        {
            {L"LdrpAccessResourceData"},
            (void**)&LdrpAccessResourceData_Original,
            (void*)LdrpAccessResourceData_Hook,
            true, 
        },
        {
            {L"LdrpAccessResourceDataNoMultipleLanguage"},
            (void**)&LdrpAccessResourceDataNoMultipleLanguage_Original,
            (void*)LdrpAccessResourceDataNoMultipleLanguage_Hook,
            true, 
        },
        {
            {L"LdrFindResource_U"},
            (void**)&LdrFindResource_U_Original,
            (void*)LdrFindResource_U_Hook,
            true, 
        },
        {
            {L"LdrFindResourceEx_U"},
            (void**)&LdrFindResourceEx_U_Original,
            (void*)LdrFindResourceEx_U_Hook,
            true, 
        },
        {
            {L"LdrFindResourceDirectory_U"},
            (void**)&LdrFindResourceDirectory_U_Original,
            (void*)LdrFindResourceDirectory_U_Hook,
            true, 
        },
        {
            {L"LdrpSearchResourceSection_U"},
            (void**)&LdrpSearchResourceSection_U_Original,
            (void*)LdrpSearchResourceSection_U_Hook,
            true,
        },
    };

    if (!HookSymbols(ntdllModule, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return FALSE;
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
