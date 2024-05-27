// ==WindhawkMod==
// @id              icon-resource-redirect
// @name            Resource Redirect
// @description     Define alternative files for loading various resources (e.g. instead of icons in imageres.dll) for simple theming without having to modify system files
// @version         1.1.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Resource Redirect

Define alternative files for loading various resources (e.g. instead of icons in
imageres.dll) for simple theming without having to modify system files.

## Theme folder

A theme folder can be selected in the settings. It's a folder with alternative
resource files, and the `theme.ini` file that contains redirection rules. For
example, the `theme.ini` file may contain the following:

```
[redirections]
%SystemRoot%\explorer.exe=explorer.exe
%SystemRoot%\system32\imageres.dll=imageres.dll
```

In this case, the folder must also contain the `explorer.exe`, `imageres.dll`
files which will be used as the custom resource files.

## Supported resource types and loading methods

The mod supports the following resource types and loading methods:

* Icons extracted with the `PrivateExtractIconsW` function.
* Icons, cursors and bitmaps loaded with the `LoadImageW` function.
* Icons loaded with the `LoadIconW` function.
* Cursors loaded with the `LoadCursorW` function.
* Bitmaps loaded with the `LoadBitmapW` function.
* Menus loaded with the `LoadMenuW` function.
* Dialogs loaded with the `DialogBoxParamW`, `CreateDialogParamW` functions.
* Strings loaded with the `LoadStringW` function.
* GDI+ images (e.g. PNGs) loaded with the `SHCreateStreamOnModuleResourceW`
  function.
* DirectUI resources (usually `UIFILE` and `XML`) loaded with the
  `SetXMLFromResource` function.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- themeFolder: ''
  $name: Theme folder
  $description: A folder with alternative resource files and theme.ini
- redirectionResourcePaths:
  - - original: '%SystemRoot%\System32\imageres.dll'
      $name: The original resource file
      $description: The original file from which resources are loaded
    - redirect: 'C:\my-themes\theme-1\imageres.dll'
      $name: The custom resource file
      $description: The custom resource file that will be used instead
  $name: Redirection resource paths
*/
// ==/WindhawkModSettings==

#include <psapi.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <atomic>
#include <functional>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef LR_EXACTSIZEONLY
#define LR_EXACTSIZEONLY 0x10000
#endif

std::shared_mutex g_redirectionResourcePathsMutex;
thread_local bool g_redirectionResourcePathsMutexLocked;
std::unordered_map<std::wstring, std::vector<std::wstring>>
    g_redirectionResourcePaths;
std::unordered_map<std::string, std::vector<std::string>>
    g_redirectionResourcePathsA;

std::shared_mutex g_redirectionResourceModulesMutex;
std::unordered_map<std::wstring, HMODULE> g_redirectionResourceModules;

std::atomic<DWORD> g_operationCounter;

// chooseAW<char> returns OptionA.
// chooseAW<WCHAR> returns OptionW.
template <typename T, auto OptionA, auto OptionW>
auto chooseAW() {
    if constexpr (std::is_same_v<T, char>) {
        return OptionA;
    } else {
        static_assert(std::is_same_v<T, WCHAR>);
        return OptionW;
    }
}

auto StrToW(PCWSTR str) {
    struct {
        PCWSTR p;
    } result;
    result.p = str;
    return result;
}

// https://stackoverflow.com/a/69410299
auto StrToW(PCSTR str) {
    struct {
        std::wstring wstr;
        PCWSTR p;
    } result;

    int strLen = static_cast<int>(strlen(str));
    int sizeNeeded = MultiByteToWideChar(CP_ACP, 0, str, strLen, nullptr, 0);
    if (sizeNeeded <= 0) {
        throw std::runtime_error("MultiByteToWideChar() failed: " +
                                 std::to_string(sizeNeeded));
    }

    result.wstr.resize(sizeNeeded);
    MultiByteToWideChar(CP_ACP, 0, str, strLen, result.wstr.data(), sizeNeeded);
    result.p = result.wstr.c_str();
    return result;
}

// A helper function to skip locking if the thread already holds the lock, since
// it's UB. Nested locks may happen if one hooked function is implemented with
// the help of another hooked function. We assume here that the locks are freed
// in reverse order.
auto RedirectionResourcePathsMutexSharedLock() {
    class Result {
       public:
        Result() = default;

        Result(std::shared_mutex& mutex) : lock(std::shared_lock{mutex}) {
            g_redirectionResourcePathsMutexLocked = true;
        }

        ~Result() {
            if (lock) {
                g_redirectionResourcePathsMutexLocked = false;
            }
        }

       private:
        std::optional<std::shared_lock<std::shared_mutex>> lock;
    };

    if (g_redirectionResourcePathsMutexLocked) {
        return Result{};
    }

    return Result{g_redirectionResourcePathsMutex};
}

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

template <typename T>
bool RedirectFileName(DWORD c,
                      const T* fileName,
                      std::function<void()> beforeFirstRedirectionFunction,
                      std::function<bool(const T*)> redirectFunction) {
    if (!fileName) {
        Wh_Log(L"[%u] Error, nullptr file name, falling back to original", c);
        return false;
    }

    std::basic_string<T> fileNameUpper{fileName};
    (chooseAW<T, LCMapStringA, LCMapStringW>())(
        LOCALE_USER_DEFAULT, LCMAP_UPPERCASE, &fileNameUpper[0],
        static_cast<int>(fileNameUpper.length()), &fileNameUpper[0],
        static_cast<int>(fileNameUpper.length()));

    bool triedRedirection = false;

    {
        auto lock{RedirectionResourcePathsMutexSharedLock()};

        const auto& redirectionResourcePaths =
            *(chooseAW<T, &g_redirectionResourcePathsA,
                       &g_redirectionResourcePaths>());
        if (const auto it = redirectionResourcePaths.find(fileNameUpper);
            it != redirectionResourcePaths.end()) {
            const auto& redirects = it->second;
            for (const auto& redirect : redirects) {
                if (!triedRedirection) {
                    beforeFirstRedirectionFunction();
                    triedRedirection = true;
                }

                Wh_Log(L"[%u] Trying %s", c, StrToW(redirect.c_str()).p);

                if (redirectFunction(redirect.c_str())) {
                    return true;
                }
            }
        }
    }

    if (triedRedirection) {
        Wh_Log(L"[%u] No redirection succeeded, falling back to original", c);
    }

    return false;
}

bool RedirectModule(DWORD c,
                    HINSTANCE hInstance,
                    std::function<void()> beforeFirstRedirectionFunction,
                    std::function<bool(HINSTANCE)> redirectFunction) {
    WCHAR szFileName[MAX_PATH];
    DWORD fileNameLen;
    if ((ULONG_PTR)hInstance & 3) {
        WCHAR szNtFileName[MAX_PATH * 2];

        if (!GetMappedFileName(GetCurrentProcess(), (void*)hInstance,
                               szNtFileName, ARRAYSIZE(szNtFileName))) {
            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] GetMappedFileName(%p) failed with error %u", c,
                   hInstance, dwError);
            return false;
        }

        if (!DevicePathToDosPath(szNtFileName, szFileName,
                                 ARRAYSIZE(szFileName))) {
            Wh_Log(L"[%u] DevicePathToDosPath failed", c);
            return false;
        }

        fileNameLen = wcslen(szFileName);
    } else {
        fileNameLen =
            GetModuleFileName(hInstance, szFileName, ARRAYSIZE(szFileName));
        switch (fileNameLen) {
            case 0: {
                DWORD dwError = GetLastError();
                Wh_Log(L"[%u] GetModuleFileName(%p) failed with error %u", c,
                       hInstance, dwError);
                return false;
            }

            case ARRAYSIZE(szFileName):
                Wh_Log(L"[%u] GetModuleFileName(%p) failed, name too long", c,
                       hInstance);
                return false;
        }
    }

    Wh_Log(L"[%u] Module: %s", c, szFileName);

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &szFileName[0],
                  fileNameLen, &szFileName[0], fileNameLen, nullptr, nullptr,
                  0);

    bool triedRedirection = false;

    {
        auto lock{RedirectionResourcePathsMutexSharedLock()};

        if (const auto it = g_redirectionResourcePaths.find(szFileName);
            it != g_redirectionResourcePaths.end()) {
            const auto& redirects = it->second;
            for (const auto& redirect : redirects) {
                if (!triedRedirection) {
                    beforeFirstRedirectionFunction();
                    triedRedirection = true;
                }

                Wh_Log(L"[%u] Trying %s", c, redirect.c_str());

                HINSTANCE hInstanceRedirect = GetRedirectedModule(redirect);
                if (!hInstanceRedirect) {
                    Wh_Log(L"[%u] GetRedirectedModule failed", c);
                    continue;
                }

                if (redirectFunction(hInstanceRedirect)) {
                    return true;
                }
            }
        }
    }

    if (triedRedirection) {
        Wh_Log(L"[%u] No redirection succeeded, falling back to original", c);
    }

    return false;
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
    DWORD c = ++g_operationCounter;

    Wh_Log(L"[%u] > Icon index: %d, file name: %s", c, nIconIndex, szFileName);

    UINT result;

    bool redirected = RedirectFileName<WCHAR>(
        c, szFileName,
        [&]() {
            Wh_Log(L"[%u] cxIcon: %d, %d", c, LOWORD(cxIcon), HIWORD(cxIcon));
            Wh_Log(L"[%u] cyIcon: %d, %d", c, LOWORD(cyIcon), HIWORD(cyIcon));
            Wh_Log(L"[%u] phicon: %s", c, phicon ? L"out_ptr" : L"nullptr");
            Wh_Log(L"[%u] piconid: %s", c, piconid ? L"out_ptr" : L"nullptr");
            Wh_Log(L"[%u] nIcons: %u", c, nIcons);
            Wh_Log(L"[%u] flags: 0x%08X", c, flags);
        },
        [&](PCWSTR fileNameRedirect) {
            if (phicon) {
                std::fill_n(phicon, nIcons, nullptr);
            }

            result = PrivateExtractIconsW_Original(fileNameRedirect, nIconIndex,
                                                   cxIcon, cyIcon, phicon,
                                                   piconid, nIcons, flags);
            if (result != 0xFFFFFFFF && result != 0) {
                // In case multiple icons are requested and the custom resource
                // only overrides some of them, we'd ideally like to return a
                // combined result. Unfortunately, that's not trivial to
                // implement, so return the original icons in this case.
                //
                // An example where multiple icons are requested is the Change
                // Icon dialog in shortcut file properties. If a partial result
                // is returned, only the returned icons are displayed.
                bool multipleIcons = nIcons > (HIWORD(cxIcon) ? 2 : 1);
                bool partialResult = result < nIcons;
                UINT multipleIconsOriginalCount = 0;
                if (multipleIcons && partialResult) {
                    multipleIconsOriginalCount = PrivateExtractIconsW_Original(
                        szFileName, nIconIndex, cxIcon, cyIcon, nullptr,
                        nullptr, nIcons, flags);
                }

                if (result < multipleIconsOriginalCount) {
                    Wh_Log(
                        L"[%u] Got less icons than the original file has: %u "
                        L"vs. %u, replacing redirection with the original "
                        L"icons",
                        c, result, multipleIconsOriginalCount);

                    if (phicon) {
                        for (UINT i = 0; i < nIcons; i++) {
                            if (phicon[i]) {
                                DestroyIcon(phicon[i]);
                                phicon[i] = nullptr;
                            }
                        }
                    }

                    result = PrivateExtractIconsW_Original(
                        szFileName, nIconIndex, cxIcon, cyIcon, phicon, piconid,
                        nIcons, flags);
                }

                Wh_Log(L"[%u] Redirected successfully, result: %u", c, result);
                return true;
            }

            // If `LR_EXACTSIZEONLY` is used and the exact size is missing, the
            // function will return no icons. If the replacement file doesn't
            // have this icon, we want to fall back to the original file, but if
            // it has other sizes, we prefer to return an empty result,
            // hopefully the target app will try other sizes in this case.
            if (result == 0 && phicon && (flags & LR_EXACTSIZEONLY)) {
                HICON testIcon = nullptr;
                UINT testIconId;
                UINT testResult = PrivateExtractIconsW_Original(
                    fileNameRedirect, nIconIndex, LOWORD(cxIcon),
                    LOWORD(cyIcon), &testIcon, &testIconId, 1,
                    flags & ~LR_EXACTSIZEONLY);

                if (testIcon) {
                    DestroyIcon(testIcon);
                    testIcon = nullptr;
                }

                if (testResult > 0) {
                    Wh_Log(
                        L"[%u] Redirected successfully with an empty result: "
                        L"%u",
                        c, result);
                    return true;
                }
            }

            // If there is no exact match the API can return 0 but phicon[0] set
            // to a valid hicon. In that case destroy the icon and reset the
            // entry.
            // https://github.com/microsoft/terminal/blob/6d0342f0bb31bf245843411c6781d6d5399ff651/src/interactivity/win32/icon.cpp#L178
            if (phicon) {
                for (UINT i = 0; i < nIcons; i++) {
                    if (phicon[i]) {
                        DestroyIcon(phicon[i]);
                        phicon[i] = nullptr;
                    }
                }
            }

            Wh_Log(L"[%u] Redirection failed, result: %u", c, result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return PrivateExtractIconsW_Original(szFileName, nIconIndex, cxIcon, cyIcon,
                                         phicon, piconid, nIcons, flags);
}

template <auto* Original, typename T>
HANDLE LoadImageAW_Hook(HINSTANCE hInst,
                        const T* name,
                        UINT type,
                        int cx,
                        int cy,
                        UINT fuLoad) {
    DWORD c = ++g_operationCounter;

    PCWSTR typeClarification = L"";
    switch (type) {
        case IMAGE_BITMAP:
            typeClarification = L" (bitmap)";
            break;
        case IMAGE_ICON:
            typeClarification = L" (icon)";
            break;
        case IMAGE_CURSOR:
            typeClarification = L" (cursor)";
            break;
    }

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c, type: %u%s", c, chooseAW<T, L'A', L'W'>(),
               type, typeClarification);

    if (!hInst) {
        if (fuLoad & LR_LOADFROMFILE) {
            Wh_Log(L"%s, file name: %s", prefix, StrToW(name).p);
        } else {
            Wh_Log(L"%s, resource identifier: %zu", prefix, (ULONG_PTR)name);
        }
    } else if (IS_INTRESOURCE(name)) {
        Wh_Log(L"%s, resource number: %u", prefix, (DWORD)(ULONG_PTR)name);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(name).p);
    }

    HANDLE result;
    bool redirected;

    auto beforeFirstRedirectionFunction = [&]() {
        Wh_Log(L"[%u] Width: %d", c, cx);
        Wh_Log(L"[%u] Height: %d", c, cy);
        Wh_Log(L"[%u] Flags: 0x%08X", c, fuLoad);
    };

    if (!hInst && (fuLoad & LR_LOADFROMFILE)) {
        redirected = RedirectFileName<T>(
            c, name, std::move(beforeFirstRedirectionFunction),
            [&](const T* fileNameRedirect) {
                result =
                    (*Original)(hInst, fileNameRedirect, type, cx, cy, fuLoad);
                if (result) {
                    Wh_Log(L"[%u] Redirected successfully", c);
                    return true;
                }

                DWORD dwError = GetLastError();
                Wh_Log(L"[%u] LoadImage failed with error %u", c, dwError);
                return false;
            });
    } else {
        redirected = RedirectModule(
            c, hInst, std::move(beforeFirstRedirectionFunction),
            [&](HINSTANCE hInstanceRedirect) {
                result =
                    (*Original)(hInstanceRedirect, name, type, cx, cy, fuLoad);
                if (result) {
                    Wh_Log(L"[%u] Redirected successfully", c);
                    return true;
                }

                DWORD dwError = GetLastError();
                Wh_Log(L"[%u] LoadImage failed with error %u", c, dwError);
                return false;
            });
    }

    if (redirected) {
        return result;
    }

    return (*Original)(hInst, name, type, cx, cy, fuLoad);
}

using LoadImageA_t = decltype(&LoadImageA);
LoadImageA_t LoadImageA_Original;
HANDLE WINAPI LoadImageA_Hook(HINSTANCE hInst,
                              LPCSTR name,
                              UINT type,
                              int cx,
                              int cy,
                              UINT fuLoad) {
    return LoadImageAW_Hook<&LoadImageA_Original>(hInst, name, type, cx, cy,
                                                  fuLoad);
}

using LoadImageW_t = decltype(&LoadImageW);
LoadImageW_t LoadImageW_Original;
HANDLE WINAPI LoadImageW_Hook(HINSTANCE hInst,
                              LPCWSTR name,
                              UINT type,
                              int cx,
                              int cy,
                              UINT fuLoad) {
    return LoadImageAW_Hook<&LoadImageW_Original>(hInst, name, type, cx, cy,
                                                  fuLoad);
}

template <auto* Original, typename T>
HICON LoadIconAW_Hook(HINSTANCE hInstance, const T* lpIconName) {
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (!hInstance) {
        Wh_Log(L"%s, resource identifier: %zu", prefix, (ULONG_PTR)lpIconName);
    } else if (IS_INTRESOURCE(lpIconName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpIconName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpIconName).p);
    }

    HICON result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpIconName);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadIcon failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpIconName);
}

using LoadIconA_t = decltype(&LoadIconA);
LoadIconA_t LoadIconA_Original;
HICON WINAPI LoadIconA_Hook(HINSTANCE hInstance, LPCSTR lpIconName) {
    return LoadIconAW_Hook<&LoadIconA_Original>(hInstance, lpIconName);
}

using LoadIconW_t = decltype(&LoadIconW);
LoadIconW_t LoadIconW_Original;
HICON WINAPI LoadIconW_Hook(HINSTANCE hInstance, LPCWSTR lpIconName) {
    return LoadIconAW_Hook<&LoadIconW_Original>(hInstance, lpIconName);
}

template <auto* Original, typename T>
HCURSOR LoadCursorAW_Hook(HINSTANCE hInstance, const T* lpCursorName) {
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (!hInstance) {
        Wh_Log(L"%s, resource identifier: %zu", prefix,
               (ULONG_PTR)lpCursorName);
    } else if (IS_INTRESOURCE(lpCursorName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpCursorName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpCursorName).p);
    }

    HCURSOR result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpCursorName);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadCursor failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpCursorName);
}

using LoadCursorA_t = decltype(&LoadCursorA);
LoadCursorA_t LoadCursorA_Original;
HCURSOR WINAPI LoadCursorA_Hook(HINSTANCE hInstance, LPCSTR lpCursorName) {
    return LoadCursorAW_Hook<&LoadCursorA_Original>(hInstance, lpCursorName);
}

using LoadCursorW_t = decltype(&LoadCursorW);
LoadCursorW_t LoadCursorW_Original;
HCURSOR WINAPI LoadCursorW_Hook(HINSTANCE hInstance, LPCWSTR lpCursorName) {
    return LoadCursorAW_Hook<&LoadCursorW_Original>(hInstance, lpCursorName);
}

template <auto* Original, typename T>
HBITMAP LoadBitmapAW_Hook(HINSTANCE hInstance, const T* lpBitmapName) {
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (!hInstance) {
        Wh_Log(L"%s, resource identifier: %zu", prefix,
               (ULONG_PTR)lpBitmapName);
    } else if (IS_INTRESOURCE(lpBitmapName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpBitmapName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpBitmapName).p);
    }

    HBITMAP result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpBitmapName);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadBitmap failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpBitmapName);
}

using LoadBitmapA_t = decltype(&LoadBitmapA);
LoadBitmapA_t LoadBitmapA_Original;
HBITMAP WINAPI LoadBitmapA_Hook(HINSTANCE hInstance, LPCSTR lpBitmapName) {
    return LoadBitmapAW_Hook<&LoadBitmapA_Original>(hInstance, lpBitmapName);
}

using LoadBitmapW_t = decltype(&LoadBitmapW);
LoadBitmapW_t LoadBitmapW_Original;
HBITMAP WINAPI LoadBitmapW_Hook(HINSTANCE hInstance, LPCWSTR lpBitmapName) {
    return LoadBitmapAW_Hook<&LoadBitmapW_Original>(hInstance, lpBitmapName);
}

template <auto* Original, typename T>
HMENU LoadMenuAW_Hook(HINSTANCE hInstance, const T* lpMenuName) {
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (IS_INTRESOURCE(lpMenuName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpMenuName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpMenuName).p);
    }

    HMENU result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpMenuName);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadMenu failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpMenuName);
}

using LoadMenuA_t = decltype(&LoadMenuA);
LoadMenuA_t LoadMenuA_Original;
HMENU WINAPI LoadMenuA_Hook(HINSTANCE hInstance, LPCSTR lpMenuName) {
    return LoadMenuAW_Hook<&LoadMenuA_Original>(hInstance, lpMenuName);
}

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_Original;
HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    return LoadMenuAW_Hook<&LoadMenuW_Original>(hInstance, lpMenuName);
}

template <auto* Original, typename T>
INT_PTR DialogBoxParamAW_Hook(HINSTANCE hInstance,
                              const T* lpTemplateName,
                              HWND hWndParent,
                              DLGPROC lpDialogFunc,
                              LPARAM dwInitParam) {
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (IS_INTRESOURCE(lpTemplateName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpTemplateName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpTemplateName).p);
    }

    INT_PTR result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            // For other redirected functions, we check whether the function
            // succeeded. If it didn't, we try another redirection or fall back
            // to the original file.
            //
            // In this case, there's no reliable way to find out whether
            // DialogBoxParamW failed, since any value can be returned.
            // Therefore, only make sure that the dialog resource exists.
            if (!(chooseAW<T, FindResourceExA, FindResourceExW>())(
                    hInstanceRedirect, (T*)RT_DIALOG, lpTemplateName, 0)) {
                Wh_Log(L"[%u] Resource not found", c);
                return false;
            }

            Wh_Log(L"[%u] Redirected successfully", c);
            result = (*Original)(hInstanceRedirect, lpTemplateName, hWndParent,
                                 lpDialogFunc, dwInitParam);
            return true;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpTemplateName, hWndParent, lpDialogFunc,
                       dwInitParam);
}

using DialogBoxParamA_t = decltype(&DialogBoxParamA);
DialogBoxParamA_t DialogBoxParamA_Original;
INT_PTR WINAPI DialogBoxParamA_Hook(HINSTANCE hInstance,
                                    LPCSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    return DialogBoxParamAW_Hook<&DialogBoxParamA_Original>(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

using DialogBoxParamW_t = decltype(&DialogBoxParamW);
DialogBoxParamW_t DialogBoxParamW_Original;
INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE hInstance,
                                    LPCWSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    return DialogBoxParamAW_Hook<&DialogBoxParamW_Original>(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

template <auto* Original, typename T>
HWND CreateDialogParamAW_Hook(HINSTANCE hInstance,
                              const T* lpTemplateName,
                              HWND hWndParent,
                              DLGPROC lpDialogFunc,
                              LPARAM dwInitParam) {
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    if (IS_INTRESOURCE(lpTemplateName)) {
        Wh_Log(L"%s, resource number: %u", prefix,
               (DWORD)(ULONG_PTR)lpTemplateName);
    } else {
        Wh_Log(L"%s, resource name: %s", prefix, StrToW(lpTemplateName).p);
    }

    HWND result;

    bool redirected = RedirectModule(
        c, hInstance, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = (*Original)(hInstanceRedirect, lpTemplateName, hWndParent,
                                 lpDialogFunc, dwInitParam);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] CreateDialogParam failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, lpTemplateName, hWndParent, lpDialogFunc,
                       dwInitParam);
}

using CreateDialogParamA_t = decltype(&CreateDialogParamA);
CreateDialogParamA_t CreateDialogParamA_Original;
HWND WINAPI CreateDialogParamA_Hook(HINSTANCE hInstance,
                                    LPCSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    return CreateDialogParamAW_Hook<&CreateDialogParamA_Original>(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

using CreateDialogParamW_t = decltype(&CreateDialogParamW);
CreateDialogParamW_t CreateDialogParamW_Original;
HWND WINAPI CreateDialogParamW_Hook(HINSTANCE hInstance,
                                    LPCWSTR lpTemplateName,
                                    HWND hWndParent,
                                    DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    return CreateDialogParamAW_Hook<&CreateDialogParamW_Original>(
        hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

template <auto* Original, typename T>
int LoadStringAW_Hook(HINSTANCE hInstance,
                      UINT uID,
                      T* lpBuffer,
                      int cchBufferMax) {
    DWORD c = ++g_operationCounter;

    WCHAR prefix[64];
    swprintf_s(prefix, L"[%u] > %c", c, chooseAW<T, L'A', L'W'>());

    Wh_Log(L"%s, string number: %u", prefix, uID);

    int result;

    bool redirected = RedirectModule(
        c, hInstance, [&]() {},
        [&](HINSTANCE hInstanceRedirect) {
            result =
                (*Original)(hInstanceRedirect, uID, lpBuffer, cchBufferMax);
            if (result) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            DWORD dwError = GetLastError();
            Wh_Log(L"[%u] LoadString failed with error %u", c, dwError);
            return false;
        });
    if (redirected) {
        return result;
    }

    return (*Original)(hInstance, uID, lpBuffer, cchBufferMax);
}

using LoadStringA_t = decltype(&LoadStringA);
LoadStringA_t LoadStringA_Original;
int WINAPI LoadStringA_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPSTR lpBuffer,
                            int cchBufferMax) {
    return LoadStringAW_Hook<&LoadStringA_Original>(hInstance, uID, lpBuffer,
                                                    cchBufferMax);
}

using LoadStringW_t = decltype(&LoadStringW);
LoadStringW_t LoadStringW_Original;
int WINAPI LoadStringW_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPWSTR lpBuffer,
                            int cchBufferMax) {
    return LoadStringAW_Hook<&LoadStringW_Original>(hInstance, uID, lpBuffer,
                                                    cchBufferMax);
}

using SHCreateStreamOnModuleResourceW_t = HRESULT(WINAPI*)(HMODULE hModule,
                                                           LPCWSTR pwszName,
                                                           LPCWSTR pwszType,
                                                           IStream** ppStream);
SHCreateStreamOnModuleResourceW_t SHCreateStreamOnModuleResourceW_Original;
HRESULT WINAPI SHCreateStreamOnModuleResourceW_Hook(HMODULE hModule,
                                                    LPCWSTR pwszName,
                                                    LPCWSTR pwszType,
                                                    IStream** ppStream) {
    DWORD c = ++g_operationCounter;

    PCWSTR logTypeStr;
    WCHAR logTypeStrBuffer[16];
    if (IS_INTRESOURCE(pwszType)) {
        swprintf_s(logTypeStrBuffer, L"%u", (DWORD)(ULONG_PTR)pwszType);
        logTypeStr = logTypeStrBuffer;
    } else {
        logTypeStr = pwszType;
    }

    if (IS_INTRESOURCE(pwszName)) {
        Wh_Log(L"[%u] > Type: %s, resource number: %u", c, logTypeStr,
               (DWORD)(ULONG_PTR)pwszName);
    } else {
        Wh_Log(L"[%u] > Type: %s, resource name: %s", c, logTypeStr, pwszName);
    }

    HRESULT result;

    bool redirected = RedirectModule(
        c, hModule, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = SHCreateStreamOnModuleResourceW_Original(
                hInstanceRedirect, pwszName, pwszType, ppStream);
            if (SUCCEEDED(result)) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            Wh_Log(
                L"[%u] SHCreateStreamOnModuleResourceW failed with error %08X",
                c, result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return SHCreateStreamOnModuleResourceW_Original(hModule, pwszName, pwszType,
                                                    ppStream);
}

using SetXMLFromResource_t = HRESULT(__thiscall*)(void* pThis,
                                                  PCWSTR lpName,
                                                  PCWSTR lpType,
                                                  HMODULE hModule,
                                                  HINSTANCE param4,
                                                  HINSTANCE param5);
SetXMLFromResource_t SetXMLFromResource_Original;
HRESULT __thiscall SetXMLFromResource_Hook(void* pThis,
                                           PCWSTR lpName,
                                           PCWSTR lpType,
                                           HMODULE hModule,
                                           HINSTANCE param4,
                                           HINSTANCE param5) {
    DWORD c = ++g_operationCounter;

    PCWSTR logTypeStr;
    WCHAR logTypeStrBuffer[16];
    if (IS_INTRESOURCE(lpType)) {
        swprintf_s(logTypeStrBuffer, L"%u", (DWORD)(ULONG_PTR)lpType);
        logTypeStr = logTypeStrBuffer;
    } else {
        logTypeStr = lpType;
    }

    if (IS_INTRESOURCE(lpName)) {
        Wh_Log(L"[%u] > Type: %s, resource number: %u", c, logTypeStr,
               (DWORD)(ULONG_PTR)lpName);
    } else {
        Wh_Log(L"[%u] > Type: %s, resource name: %s", c, logTypeStr, lpName);
    }

    HRESULT result;

    bool redirected = RedirectModule(
        c, hModule, []() {},
        [&](HINSTANCE hInstanceRedirect) {
            result = SetXMLFromResource_Original(
                pThis, lpName, lpType, hInstanceRedirect, param4, param5);
            if (SUCCEEDED(result)) {
                Wh_Log(L"[%u] Redirected successfully", c);
                return true;
            }

            Wh_Log(L"[%u] SetXMLFromResource failed with error %08X", c,
                   result);
            return false;
        });
    if (redirected) {
        return result;
    }

    return SetXMLFromResource_Original(pThis, lpName, lpType, hModule, param4,
                                       param5);
}

void LoadSettings() {
    std::unordered_map<std::wstring, std::vector<std::wstring>> paths;
    std::unordered_map<std::string, std::vector<std::string>> pathsA;

    auto addRedirectionPath = [&paths, &pathsA](PCWSTR original,
                                                PCWSTR redirect) {
        WCHAR originalExpanded[MAX_PATH];
        DWORD originalExpandedLen = ExpandEnvironmentStrings(
            original, originalExpanded, ARRAYSIZE(originalExpanded));
        if (!originalExpandedLen ||
            originalExpandedLen > ARRAYSIZE(originalExpanded)) {
            Wh_Log(L"Failed to expand path: %s", original);
            return;
        }

        Wh_Log(L"Configuring %s->%s", originalExpanded, redirect);

        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      originalExpanded, originalExpandedLen - 1,
                      originalExpanded, originalExpandedLen - 1, nullptr,
                      nullptr, 0);

        paths[originalExpanded].push_back(redirect);

        char originalExpandedA[MAX_PATH];
        char redirectA[MAX_PATH];
        size_t charsConverted = 0;
        if (wcstombs_s(&charsConverted, originalExpandedA,
                       ARRAYSIZE(originalExpandedA), originalExpanded,
                       _TRUNCATE) == 0 &&
            wcstombs_s(&charsConverted, redirectA, ARRAYSIZE(redirectA),
                       redirect, _TRUNCATE) == 0) {
            pathsA[originalExpandedA].push_back(redirectA);
        } else {
            Wh_Log(L"Error configuring ANSI paths");
        }
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
    g_redirectionResourcePathsA = std::move(pathsA);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)PrivateExtractIconsW,
                       (void*)PrivateExtractIconsW_Hook,
                       (void**)&PrivateExtractIconsW_Original);

    Wh_SetFunctionHook((void*)LoadImageA, (void*)LoadImageA_Hook,
                       (void**)&LoadImageA_Original);

    Wh_SetFunctionHook((void*)LoadImageW, (void*)LoadImageW_Hook,
                       (void**)&LoadImageW_Original);

    Wh_SetFunctionHook((void*)LoadIconA, (void*)LoadIconA_Hook,
                       (void**)&LoadIconA_Original);

    Wh_SetFunctionHook((void*)LoadIconW, (void*)LoadIconW_Hook,
                       (void**)&LoadIconW_Original);

    Wh_SetFunctionHook((void*)LoadCursorA, (void*)LoadCursorA_Hook,
                       (void**)&LoadCursorA_Original);

    Wh_SetFunctionHook((void*)LoadCursorW, (void*)LoadCursorW_Hook,
                       (void**)&LoadCursorW_Original);

    Wh_SetFunctionHook((void*)LoadBitmapA, (void*)LoadBitmapA_Hook,
                       (void**)&LoadBitmapA_Original);

    Wh_SetFunctionHook((void*)LoadBitmapW, (void*)LoadBitmapW_Hook,
                       (void**)&LoadBitmapW_Original);

    Wh_SetFunctionHook((void*)LoadMenuA, (void*)LoadMenuA_Hook,
                       (void**)&LoadMenuA_Original);

    Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook,
                       (void**)&LoadMenuW_Original);

    Wh_SetFunctionHook((void*)DialogBoxParamA, (void*)DialogBoxParamA_Hook,
                       (void**)&DialogBoxParamA_Original);

    Wh_SetFunctionHook((void*)DialogBoxParamW, (void*)DialogBoxParamW_Hook,
                       (void**)&DialogBoxParamW_Original);

    Wh_SetFunctionHook((void*)CreateDialogParamA,
                       (void*)CreateDialogParamA_Hook,
                       (void**)&CreateDialogParamA_Original);

    Wh_SetFunctionHook((void*)CreateDialogParamW,
                       (void*)CreateDialogParamW_Hook,
                       (void**)&CreateDialogParamW_Original);

    Wh_SetFunctionHook((void*)LoadStringA, (void*)LoadStringA_Hook,
                       (void**)&LoadStringA_Original);

    Wh_SetFunctionHook((void*)LoadStringW, (void*)LoadStringW_Hook,
                       (void**)&LoadStringW_Original);

    HMODULE shcoreModule = LoadLibrary(L"shcore.dll");
    if (shcoreModule) {
        FARPROC pSHCreateStreamOnModuleResourceW =
            GetProcAddress(shcoreModule, (PCSTR)109);
        if (pSHCreateStreamOnModuleResourceW) {
            Wh_SetFunctionHook(
                (void*)pSHCreateStreamOnModuleResourceW,
                (void*)SHCreateStreamOnModuleResourceW_Hook,
                (void**)&SHCreateStreamOnModuleResourceW_Original);
        } else {
            Wh_Log(L"Couldn't find SHCreateStreamOnModuleResourceW (#109)");
        }
    } else {
        Wh_Log(L"Couldn't load shcore.dll");
    }

    HMODULE duiModule = LoadLibrary(L"dui70.dll");
    if (duiModule) {
        PCSTR procName =
            R"(?_SetXMLFromResource@DUIXmlParser@DirectUI@@IAEJPBG0PAUHINSTANCE__@@11@Z)";
        FARPROC pSetXMLFromResource = GetProcAddress(duiModule, procName);
        if (!pSetXMLFromResource) {
#ifdef _WIN64
            PCSTR procName_Win10_x64 =
                R"(?_SetXMLFromResource@DUIXmlParser@DirectUI@@IEAAJPEBG0PEAUHINSTANCE__@@11@Z)";
            pSetXMLFromResource = GetProcAddress(duiModule, procName_Win10_x64);
#endif
        }

        if (pSetXMLFromResource) {
            Wh_SetFunctionHook((void*)pSetXMLFromResource,
                               (void*)SetXMLFromResource_Hook,
                               (void**)&SetXMLFromResource_Original);
        } else {
            Wh_Log(L"Couldn't find SetXMLFromResource");
        }
    } else {
        Wh_Log(L"Couldn't load dui70.dll");
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
