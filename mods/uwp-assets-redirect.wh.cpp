// ==WindhawkMod==
// @id              uwp-assets-redirect
// @name            UWP Assets Redirect
// @description     Replace UWP app assets (such as icons) without worrying about updates or modifying system files permissions.
// @version         1.1
// @author          ferrys
// @github          https://github.com/atferrys
// @license         GPL-3.0
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         RuntimeBroker.exe
// @include         Taskmgr.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lgdiplus -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# UWP Assets Redirect
Replace UWP app assets (such as icons) without worrying about updates
or modifying system files permissions.

## Example: Before and After
![Before and after comparison of some applications](https://raw.githubusercontent.com/atferrys/uwp-assets-redirect/main/docs-assets/example-before-after.png)

# Icon themes
You can apply Icon themes directly from the **Settings** tab by selecting one from the list.
The theme will be automatically downloaded and applied. You can see the full list of themes
and their previews in the [theme repository](https://github.com/atferrys/uwp-assets-redirect/tree/main/themes).

_To contribute a new theme to the theme repository, follow the instructions
[here](https://github.com/atferrys/uwp-assets-redirect/blob/main/themes/README.md#contributing-new-themes)._

# Applying redirections
You can apply redirections to _Windows Apps_, _System Apps_ and _Custom_ paths directly from
the **Settings** tab.

For each redirection you will need to specify the **Bundle name** and a **Redirection folder or .ico file**:
You can find the application bundle by following [the guide below](#finding-the-application-bundle-and-assets), then you can specify a folder with the
custom assets files or a single `.ico` file.

If you provide a single `.ico` file, only the app icons shown throughout the generic parts of the system will be
replaced _(e.g. File Explorer, Start Menu, etc...)_, and it may not always be able to generate the correct assets.

**To fully replace assets**, it's recommended to use a Redirection folder with the correct assets.
You can find out more about creating custom assets [down below](#creating-custom-assets).

# Finding the Application bundle and assets
You can quickly identify both the application bundle and its Assets folder using Task Manager.

1. Open the application you want to redirect assets for. Then open Task Manager, right-click the
   application process, and select "**Open file location**":

    ![Task Manager right-click context menu on "Microsoft Store" with "Open file location" hovered](https://raw.githubusercontent.com/atferrys/uwp-assets-redirect/main/docs-assets/task-manager-open-location.png)

2. A File Explorer window will open with the application's folder highlighted:

    ![Application folder highlighted in File Explorer with the bundle identifier shown](https://raw.githubusercontent.com/atferrys/uwp-assets-redirect/main/docs-assets/application-folder.png)

3. The application bundle is the part of the folder name that comes before the first underscore,
   in this case "`Microsoft.WindowsStore`".

If Assets Redirect can't automatically locate the Assets folder, you can browse the application directory to manually find it.
In this example, although it was detected automatically, the assets were located in "`Assets\AppTiles`":

![The path to the assets folder](https://raw.githubusercontent.com/atferrys/uwp-assets-redirect/main/docs-assets/assets-folder.png)

You can specify them in the application bundle using this format: "`<application bundle>`|`<assets folder>`",
and in this case "`Microsoft.WindowsStore|Assets\AppTiles`".

# Creating custom assets
You can manually create replacement assets by copying the original Assets folder, removing any files you don't
want to replace, and editing the remaining ones making sure to preserve the original file resolutions.

A quicker and easier approach is to use something like [TileGen](https://tilegen.ferrys.it/assets-redirect), an open-source tool that generates the required assets
from a single image or an `.ico` file. _Using an .ico file is recommended, as it already contains multiple resolutions._

_Keep in mind that even if File Explorer doesn't show the non-redirected assets after you've applied them, they're
still present. This is only a visual issue caused by how File Explorer loads folder contents._

# Theme paths
Theme paths can be set in the settings. Each theme path can be a folder with custom assets files and a `theme.ini` file
that contains redirection rules, or the `.ini` theme file itself.

For example, the `theme.ini` file may contain the following redirection rules:

## WindowsApps and SystemApps redirections
For apps found in "`C:\Program Files\WindowsApps`" and in "`C:\Windows\SystemApps`", you can use respectively
the `[windows-apps]` and `[system-apps]` headers.

Each rule should be provided in this format: "`<application bundle>`=`<redirection folder/.ico>`".
The application bundle can be easily found by following [the guide above](#finding-the-application-bundle-and-assets).

### Example config
```
[windows-apps]
Microsoft.WindowsStore=.\Microsoft Store
Microsoft.WindowsCalculator=.\Calculator
Microsoft.WindowsTerminal=Terminal.ico
```

Most of the time, Assets Redirect can automatically locate the bundle's Assets folder. However, some applications use
the same application bundle as other apps, which can prevent Assets Redirect from identifying the correct folder.

In these cases, you can manually specify the Assets folder within the application bundle using the following format:
"`<application bundle>`|`<assets folder>`" (an example is shown below in `[system-apps]`).

### Example config
```
[system-apps]
MicrosoftWindows.Client.CBS|WindowsBackup\Assets=.\Windows Backup
Microsoft.PPIProjection=.\Wireless Display
```

## Custom redirections
For apps that aren't found in common folders like `WindowsApps` or `SystemApps`, like _Settings_, you can use
the `[custom]` header.

Each rule should be provided in this format: "`<assets folder>`=`<redirection folder>`".

### Example config
```
[custom]
%SystemRoot%\ImmersiveControlPanel\images=.\Settings
```

# Injecting into other processes
By default, Assets Redirect only targets the processes that most commonly use these assets, such as File Explorer and Start Menu.
As a result, UWP applications are not affected by asset redirection out of the box.

You can change this behavior using the "Custom process inclusion list" in the Advanced tab and doing one of the following:
- Include a specific application's executable name or path like "`WinStore.App.exe`" (you can find this in the Details tab of Task Manager).
- Include the entire applications directories: "`C:\Program Files\WindowsApps\*`" and "`C:\Windows\SystemApps\*`".
- Or, if you're willing to risk system stability, include all processes using "`*`".

Doing this applies your asset changes not only to the Windows shell, but also to the applications themselves, changing
their look as well (like the splash screen).

# Contributions
You can contribute to the mod development by opening a pull request [here](https://github.com/atferrys/uwp-assets-redirect/pulls).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- icon-theme: ""
  $name: Icon theme
  $description: >-
    The icon theme to use. Select one and it will be automatically
    downloaded and applied.

    You can contribute to the themes repository by following
    the instructions in the details tab.
  $options:
  - "": None
  - "ferrys/aero": Aero (by @ferrys)
  - "ferrys/luna": Luna (by @ferrys)
- theme-paths: [""]
  $name: Theme paths
  $description: >-
    Each path can be a folder with custom assets files and a theme.ini file,
    or the .ini theme file itself.

    Follow the guide on how to create a theme file in the details tab for more information.

- windows-apps:
  - - bundle: ""
      $name: Bundle name
      $description: >-
        The application bundle without any version appended.

        You can get easily get this via Task Manager, follow the guide in
        the details tab for more information.
    - redirect: ""
      $name: Redirection folder or .ico file
      $description: >-
        The folder with the custom assets files or a single .ico file.

        Note: If you provide a single .ico file, only app icons will be replaced.
        You can find more information in the details tab.
  $name: WindowsApps Redirections
  $description: >-
    Redirections for the apps found in "C:\Program Files\WindowsApps".

    This is used for Microsoft Store apps and some system apps.

- system-apps:
  - - bundle: ""
      $name: Bundle name
      $description: >-
        The application bundle without any version appended.

        You can get easily get this via Task Manager, follow the guide in
        the details tab for more information.
    - redirect: ""
      $name: Redirection folder or .ico file
      $description: >-
        The folder with the custom assets files or a single .ico file.

        Note: If you provide a single .ico file, only app icons will be replaced.
        You can find more information in the details tab.
  $name: SystemApps Redirections
  $description: >-
    Redirections for the apps found in "C:\Windows\SystemApps".

    This is used for some system apps like "Wireless Display".
- custom:
  - - assets-path: ""
      $name: Original path
      $description: >-
        The full application assets path.

        Can be a pattern where '*' matches any number of characters.
    - redirect: ""
      $name: Redirection folder
      $description: The folder with the custom assets files.
  $name: Custom Redirections
  $description: >-
    Redirections for system apps that aren't found in any of the previous folders.

    This can be used for apps like "Settings".

    You'll have to specify the assets folder in the path this time and not
    simply the app bundle.

- grant-permissions: true
  $name: Grant permissions
  $description: >-
    Some applications and features, such as the Start Menu search,
    require special permissions in order to access the Assets files.

    Enabling this feature will automatically grant the correct permissions
    to the redirected files while the mod is active.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <initguid.h>
#include <windows.h>
#include <winternl.h>
#include <shlobj.h>
#include <Aclapi.h>
#include <sddl.h>
#include <winrt/base.h>
#include <comutil.h>
#include <shldisp.h>
#include <gdiplus.h>
#include <format>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <vector>

std::unordered_map<std::wstring, std::wstring> g_redirections;
BOOL g_grant_permissions;

inline WCHAR ToLowerAscii(WCHAR c) {
    if (c >= L'A' && c <= L'Z') {
        return c + (L'a' - L'A');
    }
    return c;
}

template <typename T>
bool match(const T* pattern, size_t pattern_length, const T* str, size_t string_length, size_t& after_match_index) {
    size_t pattern_index = 0;
    size_t string_index = 0;

    // Flag used to only check first wildcard
    bool wildcard_matched = false;

    while (pattern_index < pattern_length && string_index < string_length) {
        if (!wildcard_matched && pattern[pattern_index] == '*') {
            ++pattern_index; // Skip the wildcard symbol
            wildcard_matched = true;

            // The rest of the pattern after the wildcard
            size_t rest_start = pattern_index;

            // Find the literal string after the wildcard (until the end)
            size_t literal_len = pattern_length - rest_start;

            // If the wildcard is the last thing present in the pattern
            if (literal_len == 0) {
                // Move forward until the first backslash or end of string
                while (string_index < string_length && str[string_index] != '\\') {
                    ++string_index;
                }

                after_match_index = string_index;
                return true;
            }

            // Try to match this literal in the string
            size_t match_pos = string_index;
            bool found = false;
            while (match_pos + literal_len <= string_length) {

                bool mismatch = false;
                for (size_t k = 0; k < literal_len; ++k) {
                    if (ToLowerAscii(str[match_pos + k]) != ToLowerAscii(pattern[rest_start + k])) {
                        mismatch = true;
                        break;
                    }
                }

                if (!mismatch) {
                    found = true;
                    break;
                }

                // Stop wildcard matching at '\'
                if (str[match_pos] == '\\') break;

                ++match_pos;
            }

            if (!found) return false;

            // Move past the matched literal
            string_index = match_pos + literal_len;
            pattern_index = rest_start + literal_len;

            // Make after_match_index point to the end of the matched pattern
            after_match_index = string_index;
        } else {
            if (ToLowerAscii(pattern[pattern_index]) != ToLowerAscii(str[string_index])) return false;
            ++pattern_index;
            ++string_index;
        }
    }

    // Ensure full pattern was matched
    return pattern_index == pattern_length;
}

using NtCreateFile_t = decltype(&NtCreateFile);
NtCreateFile_t NtCreateFile_Original;

NTSTATUS NTAPI NtCreateFile_Hook(
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer,
    ULONG EaLength
) {

    std::wstring originalPath;

    if(ObjectAttributes && ObjectAttributes->ObjectName) {
        UNICODE_STRING* ObjectName = ObjectAttributes->ObjectName;
        originalPath.assign(ObjectName->Buffer, ObjectName->Length / sizeof(WCHAR));
    }

    // Check if we should redirect
    if(!originalPath.empty()) {

        std::wstring redirectPath;

        const wchar_t* path = originalPath.c_str();
        size_t pathLength = originalPath.length();

        for(const auto& pair : g_redirections) {

            const wchar_t* pattern = pair.first.c_str();
            size_t patternLength = pair.first.length();

            // If there are no wildcards in pattern, simply
            // use the end of the pattern as the match_end_index
            size_t match_end_index = patternLength;

            if(match(pattern, patternLength, path, pathLength, match_end_index)) {
                redirectPath = pair.second + originalPath.substr(match_end_index);
                break;
            }

        }

        if(!redirectPath.empty()) {

            Wh_Log(L"[Redirect Attempt] %s -> %s", originalPath.c_str(), redirectPath.c_str());

            alignas(POBJECT_ATTRIBUTES) BYTE buffer[256];
            memcpy(buffer, ObjectAttributes, ObjectAttributes->Length);
            auto* RedirectedObjectAttributes = reinterpret_cast<POBJECT_ATTRIBUTES>(buffer);

            UNICODE_STRING ObjectName = {
                .Length = (USHORT) (redirectPath.length() * sizeof(WCHAR)),
                .MaximumLength = ObjectName.Length,
                .Buffer = (PWSTR) redirectPath.c_str()
            };

            RedirectedObjectAttributes->ObjectName = &ObjectName;

            NTSTATUS result = NtCreateFile_Original(
                FileHandle,
                DesiredAccess,
                RedirectedObjectAttributes,
                IoStatusBlock,
                AllocationSize,
                FileAttributes,
                ShareAccess,
                CreateDisposition,
                CreateOptions,
                EaBuffer,
                EaLength
            );

            if(NT_SUCCESS(result)) {
                Wh_Log(L"[Redirect Success] Redirected to: %s", redirectPath.c_str());
                return result;
            }

            Wh_Log(L"[Redirect Fail] Failed with code 0x%08X. Rolling back to original: %s", result, originalPath.c_str());

        }

    }

    return NtCreateFile_Original(
        FileHandle,
        DesiredAccess,
        ObjectAttributes,
        IoStatusBlock,
        AllocationSize,
        FileAttributes,
        ShareAccess,
        CreateDisposition,
        CreateOptions,
        EaBuffer,
        EaLength
    );
}

// Check if mod is hooked to the explorer process that owns the taskbar.
// Useful for running specific code once. Taken from:
// https://github.com/ramensoftware/windhawk-mods/blob/43fe260f5738d61ba09018c7de03863defbf40a0/mods/icon-resource-redirect.wh.cpp#L2120-L2122

bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    if (!GetWindowsDirectory(path, ARRAYSIZE(path))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    wcscat_s(path, MAX_PATH, L"\\explorer.exe");

    return GetModuleHandle(path) == GetModuleHandle(nullptr);
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

bool DoesCurrentProcessOwnTaskbar() {
    return IsExplorerProcess() && FindCurrentProcessTaskbarWnd();
}

HANDLE g_refresh_icons_prompt_thread;
std::atomic<HWND> g_refresh_icons_prompt;

constexpr WCHAR g_refresh_icons_prompt_title[] = L"UWP Assets Redirect - Windhawk";
constexpr WCHAR g_refresh_icons_prompt_header[] = L"Some icons might need refreshing";
constexpr WCHAR g_refresh_icons_prompt_body[] =
    L"To update certain parts of the system, such as the Start Menu, the icon cache must be cleared.\n"
    L"\n"
    L"Do you want to clear the icon cache now?";
constexpr WCHAR g_refresh_icons_prompt_footer[] =
    L"Icon cache files from File Explorer and Start Menu will be deleted, and File Explorer will be restarted.";

constexpr WCHAR g_clear_cache_command[] =
    LR"(cmd /c "title UWP Assets Redirect - Windhawk)"

    LR"(& echo Terminating Explorer...)"
    LR"(& taskkill /f /im explorer.exe)"
    LR"(& timeout /t 1 /nobreak >nul)"

    LR"(& del /f /q /a "%LocalAppData%\IconCache.db")"
    LR"(& del /f /s /q /a "%LocalAppData%\Microsoft\Windows\Explorer\iconcache_*.db")"
    LR"(& del /f /s /q /a "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db")"
    LR"(& rmdir /s /q "%LocalAppData%\Packages\Microsoft.Windows.StartMenuExperienceHost_cw5n1h2txyewy\TempState\")"

    LR"(& start explorer.exe)"
    LR"(& echo Starting Explorer...)"
    LR"(& timeout /t 3 /nobreak >nul")";

const std::wstring g_should_refresh_icons_key = L"should_refresh_icons";

void MarkShouldRefreshIcons() {
    if(DoesCurrentProcessOwnTaskbar()) {
        Wh_SetIntValue(g_should_refresh_icons_key.c_str(), TRUE);
    }
}

void RefreshIcons(bool check_should_refresh = false) {

    // Only run this once
    if (!DoesCurrentProcessOwnTaskbar()) {
        return;
    }

    if (g_refresh_icons_prompt_thread) {
        if (WaitForSingleObject(g_refresh_icons_prompt_thread, 0) != WAIT_OBJECT_0) {
            return;
        }
        CloseHandle(g_refresh_icons_prompt_thread);
    }

    if (check_should_refresh && !Wh_GetIntValue(g_should_refresh_icons_key.c_str(), FALSE)) {
        Wh_DeleteValue(g_should_refresh_icons_key.c_str());
        return;
    }

    if (check_should_refresh && g_redirections.empty()) {
        return;
    }

    g_refresh_icons_prompt_thread = CreateThread(
        nullptr,
        0,
        [](LPVOID lpParameter) WINAPI -> DWORD {

            static decltype(&TaskDialogIndirect) pTaskDialogIndirect = []() {
                HMODULE hComctl32 = LoadLibraryEx(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
                if (!hComctl32) {
                    Wh_Log(L"Failed to load comctl32.dll");
                    return (decltype(&TaskDialogIndirect)) nullptr;
                }
                return (decltype(&TaskDialogIndirect)) GetProcAddress(hComctl32, "TaskDialogIndirect");
            }();

            if(!pTaskDialogIndirect) {
                return 0;
            }

            TASKDIALOGCONFIG promptDialogConfig {
                .cbSize = sizeof(promptDialogConfig),
                .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_EXPAND_FOOTER_AREA,
                .dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
                .pszWindowTitle = g_refresh_icons_prompt_title,
                .pszMainIcon = TD_INFORMATION_ICON,
                .pszMainInstruction = g_refresh_icons_prompt_header,
                .pszContent = g_refresh_icons_prompt_body,
                .pszExpandedInformation = g_refresh_icons_prompt_footer,
                .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData) WINAPI -> HRESULT {
                    switch (msg) {
                        case TDN_CREATED:
                            g_refresh_icons_prompt = hwnd;
                            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                            break;
                        case TDN_DESTROYED:
                            g_refresh_icons_prompt = nullptr;
                            break;
                    }
                    return S_OK;
                },
            };

            int button;
            if (SUCCEEDED(pTaskDialogIndirect(&promptDialogConfig, &button, nullptr, nullptr)) && button == IDYES) {
                WCHAR commandLine[ARRAYSIZE(g_clear_cache_command)];
                memcpy(commandLine, g_clear_cache_command, sizeof(g_clear_cache_command));
                STARTUPINFO si = {
                    .cb = sizeof(si),
                };
                PROCESS_INFORMATION pi{};
                if (CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
                    WaitForSingleObject(pi.hProcess, INFINITE);
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
            }

            return 0;

        },
        nullptr,
        0,
        nullptr
    );

    // Let other processes some time to init/uninit.
    Sleep(500);

    // Invalidate icon cache.
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

    Wh_Log(L"Icon cache refreshed.");

}

// SID for "ALL APPLICATION PACKAGES"
constexpr LPCWSTR g_permission_sid = L"S-1-15-2-1";

// SID for "ALL RESTRICTED APPLICATION PACKAGES"
constexpr LPCWSTR g_permission_restricted_sid = L"S-1-15-2-2";

constexpr DWORD g_permission_access_mask = GENERIC_READ | GENERIC_EXECUTE;

void TogglePermissions(std::unordered_map<std::wstring, std::wstring>& redirections, bool toggle) {

    const auto apply_permission = [](const std::wstring path, PSID sid) -> BOOL {

        PACL old_permissions = nullptr;
        PSECURITY_DESCRIPTOR security_descriptor = nullptr;

        DWORD result = GetNamedSecurityInfoW(
            path.c_str(),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            nullptr,
            nullptr,
            &old_permissions,
            nullptr,
            &security_descriptor
        );

        if (result != ERROR_SUCCESS) {
            return false;
        }

        EXPLICIT_ACCESSW rule = {
            .grfAccessPermissions = g_permission_access_mask,
            .grfAccessMode = GRANT_ACCESS,
            .grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT,
            .Trustee = {
                .TrusteeForm = TRUSTEE_IS_SID,
                .TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP,
                .ptstrName = (LPWSTR) sid
            }
        };

        PACL new_permissions = nullptr;

        result = SetEntriesInAclW(
            1,
            &rule,
            old_permissions,
            &new_permissions
        );

        if (result != ERROR_SUCCESS) {
            if (security_descriptor) {
                LocalFree(security_descriptor);
            }
            return false;
        }

        result = SetNamedSecurityInfoW(
            (LPWSTR) path.c_str(),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            nullptr,
            nullptr,
            new_permissions,
            nullptr
        );

        if (security_descriptor) {
            LocalFree(security_descriptor);
        }

        if (new_permissions) {
            LocalFree(new_permissions);
        }

        return result == ERROR_SUCCESS;

    };

    const auto remove_permission = [](const std::wstring path, PSID sid) -> BOOL {

        PACL old_permissions = nullptr;
        PSECURITY_DESCRIPTOR security_descriptor = nullptr;

        DWORD result = GetNamedSecurityInfoW(
            path.c_str(),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            nullptr,
            nullptr,
            &old_permissions,
            nullptr,
            &security_descriptor
        );

        if (result != ERROR_SUCCESS || !old_permissions) {
            return false;
        }

        std::vector<EXPLICIT_ACCESSW> keep_rules;

        for (DWORD i = 0; i < old_permissions->AceCount; i++) {

            LPVOID ace = nullptr;

            if (!GetAce(old_permissions, i, &ace)) {
                continue;
            }

            ACE_HEADER* header = (ACE_HEADER*) ace;

            if (header->AceType != ACCESS_ALLOWED_ACE_TYPE) {
                continue;
            }

            ACCESS_ALLOWED_ACE* allowed = (ACCESS_ALLOWED_ACE*) ace;
            PSID ace_sid = &allowed->SidStart;

            // Removes the previously added permission
            if (EqualSid(ace_sid, sid)) {
                continue;
            }

            EXPLICIT_ACCESSW rule = {
                .grfAccessPermissions = allowed->Mask,
                .grfAccessMode = GRANT_ACCESS,
                .grfInheritance = header->AceFlags,
                .Trustee = {
                    .TrusteeForm = TRUSTEE_IS_SID,
                    .TrusteeType = TRUSTEE_IS_UNKNOWN,
                    .ptstrName = (LPWSTR) ace_sid
                }
            };

            keep_rules.push_back(rule);

        }

        PACL new_permissions = nullptr;

        if (!keep_rules.empty()) {
            result = SetEntriesInAclW(
                (ULONG )keep_rules.size(),
                keep_rules.data(),
                nullptr,
                &new_permissions
            );
            if (result != ERROR_SUCCESS) {
                if (security_descriptor) {
                    LocalFree(security_descriptor);
                }
                return false;
            }
        }

        result = SetNamedSecurityInfoW(
            (LPWSTR) path.c_str(),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            nullptr,
            nullptr,
            new_permissions,
            nullptr
        );

        if (security_descriptor) {
            LocalFree(security_descriptor);
        }

        if (new_permissions) {
            LocalFree(new_permissions);
        }

        return result == ERROR_SUCCESS;

    };

    PSID sid, restricted_sid;

    if (!ConvertStringSidToSid(g_permission_sid, &sid)) {
        sid = nullptr;
    }

    if (!ConvertStringSidToSid(g_permission_restricted_sid, &restricted_sid)) {
        restricted_sid = nullptr;
    }

    for(const auto& [_, path] : redirections) {
        if(toggle) {
            if(!apply_permission(path, sid) || !apply_permission(path, restricted_sid)) {
                Wh_Log(L"Failed to add special permissions to redirected assets path: %s", path.c_str());
            }
        } else {
            if(!remove_permission(path, sid) || !remove_permission(path, restricted_sid)) {
                Wh_Log(L"Failed to remove special permissions from redirected assets path: %s", path.c_str());
            }
        }
    }

    if(toggle) {
        Wh_Log(L"Added special permissions to redirected assets.");
    } else {
        Wh_Log(L"Removed special permissions from redirected assets.");
    }

    if(sid) {
       LocalFree(sid);
    }

    if(restricted_sid) {
        LocalFree(restricted_sid);
    }

}

const std::wstring g_redirections_cache_id_key = L"redirections_cache_id";
const std::wstring g_redirections_cache_size_key = L"redirections_cache_size";
const std::wstring g_redirections_cache_key = L"redirections_cache";

int g_redirections_cache_id = 0;

const std::wstring g_default_assets_folder = L"Assets";
const int g_max_read_tries = 20;

void write_redirections_cache(std::unordered_map<std::wstring, std::wstring>& redirections) {

    std::vector<uint8_t> buffer;

    auto write = [&](const void* data, size_t size) {
        size_t oldSize = buffer.size();
        buffer.resize(oldSize + size);
        memcpy(buffer.data() + oldSize, data, size);
    };

    // number of entries
    uint32_t count = (uint32_t) redirections.size();
    write(&count, sizeof(count));

    for (const auto& [key, value] : redirections) {
        uint32_t keyLen = (uint32_t) key.size();
        write(&keyLen, sizeof(keyLen));
        write(key.data(), keyLen * sizeof(wchar_t));

        uint32_t valueLen = (uint32_t) value.size();
        write(&valueLen, sizeof(valueLen));
        write(value.data(), valueLen * sizeof(wchar_t));
    }

    Wh_SetBinaryValue(g_redirections_cache_key.c_str(), buffer.data(), buffer.size());
    Wh_SetIntValue(g_redirections_cache_size_key.c_str(), buffer.size());

    Wh_SetIntValue(g_redirections_cache_id_key.c_str(), ++g_redirections_cache_id);

}

bool read_redirections_cache(std::unordered_map<std::wstring, std::wstring>& redirections) {

    int redirections_cache_id = Wh_GetIntValue(g_redirections_cache_id_key.c_str(), 0);

    if(redirections_cache_id <= g_redirections_cache_id) {
        return false;
    }

    g_redirections_cache_id = redirections_cache_id;

    std::vector<uint8_t> buffer;

    int buffer_size = Wh_GetIntValue(g_redirections_cache_size_key.c_str(), -1);

    if(buffer_size == -1) {
        return false;
    }

    buffer.resize(buffer_size);

    if(Wh_GetBinaryValue(g_redirections_cache_key.c_str(), buffer.data(), buffer.size()) == 0) {
        return false;
    }

    const uint8_t* ptr = buffer.data();

    auto read = [&](void* out, size_t size) {
        memcpy(out, ptr, size);
        ptr += size;
    };

    uint32_t count;
    read(&count, sizeof(count));

    for (uint32_t i = 0; i < count; i++) {
        uint32_t keyLen;
        read(&keyLen, sizeof(keyLen));

        std::wstring key(keyLen, L'\0');
        read(key.data(), keyLen * sizeof(wchar_t));

        uint32_t valueLen;
        read(&valueLen, sizeof(valueLen));

        std::wstring value(valueLen, L'\0');
        read(value.data(), valueLen * sizeof(wchar_t));

        redirections.emplace(std::move(key), std::move(value));
    }

    return true;

}

void ClearRedirectionsCache(bool check_main = true) {
    if(!check_main || DoesCurrentProcessOwnTaskbar()) {
        Wh_DeleteValue(g_redirections_cache_size_key.c_str());
        Wh_DeleteValue(g_redirections_cache_key.c_str());
        Wh_DeleteValue(g_redirections_cache_id_key.c_str());
    }
}

inline constexpr auto g_custom_redirections_blacklist = std::to_array<std::wstring_view>({
    LR"(?:\)",

    LR"(C:\Windows)",
    LR"(C:\Windows\System32)",
    LR"(C:\Windows\SysWOW64)",
    LR"(C:\Windows\WinSxS)",
    LR"(C:\Windows\servicing)",
    LR"(C:\Windows\Boot)",
    LR"(C:\Windows\CSC)",
    LR"(C:\Windows\SystemResources)",
    LR"(C:\Windows\System32\Config)",
    LR"(C:\Windows\ServiceProfiles)",

    LR"(C:\ProgramData\Microsoft\Crypto)",
    LR"(C:\ProgramData\Microsoft\Windows\SystemData)",

    LR"(?:\Program Files)",
    LR"(?:\Program Files (x86))",

    LR"(?:\Users)",
    LR"(?:\Users\*\AppData)",
    LR"(?:\Users\*\AppData\Local)",
    LR"(?:\Users\*\AppData\Roaming)",
    LR"(?:\Users\*\AppData\LocalLow)",

    LR"(C:\Program Files\WindowsApps)",
    LR"(C:\Windows\SystemApps)"
});

HANDLE g_downloading_theme_prompt_thread;
std::atomic<HWND> g_downloading_theme_prompt;

constexpr WCHAR g_downloading_theme_prompt_title[] = L"UWP Assets Redirect - Windhawk";
constexpr WCHAR g_downloading_theme_prompt_header[] = L"Downloading Icon theme...";
constexpr WCHAR g_downloading_theme_prompt_body[] = L"UWP Assets Redirect is downloading \"{}\", please wait...";

std::atomic<HWND> g_failed_to_download_theme_prompt;

constexpr WCHAR g_failed_to_download_theme_prompt_title[] = L"UWP Assets Redirect - Windhawk";
constexpr WCHAR g_failed_to_download_theme_prompt_header[] = L"Failed to download Icon theme";

constexpr auto g_normalize_path_base_path = L"C:\\Windows\\System32\\";

void LoadRedirections(std::unordered_map<std::wstring, std::wstring>& redirections) {

    auto normalize_path = [](std::wstring path, std::wstring base_path = g_normalize_path_base_path) -> std::wstring {

        // ### Expand environment strings like %ProgramFiles%

        DWORD path_size = ExpandEnvironmentStringsW(path.c_str(), nullptr, 0);

        if (path_size == 0) {
            return L"";
        }

        std::wstring expanded_path(path_size, L'\0');
        ExpandEnvironmentStringsW(path.c_str(), &expanded_path[0], path_size);

        // Remove trailing null character
        if (!expanded_path.empty() && expanded_path.back() == L'\0') {
            expanded_path.pop_back();
        }

        // ### Make path absolute if relative path like ".\Redirection" is given

        std::filesystem::path relative_path(expanded_path);

        if (relative_path.is_absolute()) {
            return std::filesystem::weakly_canonical(relative_path).wstring();
        }

        std::filesystem::path relative_prefixed_path = std::filesystem::path(base_path) / relative_path;
        std::wstring absolute_path = std::filesystem::weakly_canonical(relative_prefixed_path).wstring();

        // ### Remove trailing slashes

        while (!absolute_path.empty() && absolute_path.back() == L'\\') {

            size_t path_size = absolute_path.size();

            // Stop if it's a drive root like "C:\"
            if (path_size == 3 && absolute_path[path_size - 2] == L':' && absolute_path[path_size - 1] == L'\\') {
                break;
            }

            absolute_path.pop_back();

        }

        return absolute_path;

    };

    auto add_bundle_redirection = [&redirections, normalize_path](std::wstring bundles_root, std::wstring bundle, std::wstring redirect, std::filesystem::path theme_folder = g_normalize_path_base_path) {

        const auto deduct_bundle = [](std::wstring bundles_root, std::wstring bundle, std::wstring& bundle_id, std::wstring& assets_folder, std::filesystem::path& assets_folder_path) {

            const auto get_assets_folder = [](const std::wstring& appx_manifest) -> std::wstring {

                std::wifstream file(appx_manifest.c_str());

                if (!file.is_open() || file.fail()) {
                    Wh_Log(L"Failed to open manifest: %s", appx_manifest.c_str());
                    return L"";
                }

                std::wstringstream buffer;
                buffer << file.rdbuf();

                std::wstring xml = buffer.str();

                const std::wstring openTag = L"<Logo>";
                const std::wstring closeTag = L"</Logo>";

                size_t start = xml.find(openTag);

                if (start == std::wstring::npos) {
                    Wh_Log(L"Failed to find opening <Logo> tag: %s", appx_manifest.c_str());
                    return L"";
                }

                start += openTag.length();
                size_t end = xml.find(closeTag, start);

                if (end == std::wstring::npos) {
                    Wh_Log(L"Failed to find closing <Logo> tag: %s", appx_manifest.c_str());
                    return L"";
                }

                std::wstring logo_content = xml.substr(start, end - start);
                size_t backslash_position = logo_content.find_last_of('\\');

                // If assets are in root directory
                if (backslash_position == std::wstring::npos) {
                    Wh_Log(L"Failed to find external assets folder, assets are in the root directory: %s", appx_manifest.c_str());
                    return L"";
                }

                return logo_content.substr(0, backslash_position);

            };

            const auto find_bundle_folder = [](const std::wstring& bundles_root, const std::wstring& app_bundle) -> std::wstring {

                std::error_code error_code;

                for (const auto& entry : std::filesystem::directory_iterator(bundles_root, error_code)) {

                    if (!entry.is_directory()) {
                        continue;
                    }

                    std::wstring path = entry.path().wstring();

                    size_t last_backslash_index = path.find_last_of('\\');
                    size_t version_index = path.find_first_of('_', last_backslash_index);

                    if (last_backslash_index == std::string::npos || version_index == std::string::npos) {
                        continue;
                    }

                    if (path.substr(last_backslash_index + 1, version_index - last_backslash_index - 1) != app_bundle) {
                        continue;
                    }

                    if (path.find(L"neutral", version_index) != std::string::npos) {
                        continue;
                    }

                    if (!std::filesystem::exists(std::filesystem::path(path) / "AppxManifest.xml", error_code)) {
                        continue;
                    }

                    return path;

                }

                return L"";
            };

            const auto trim = [](std::wstring string) -> std::wstring {

                const auto should_trim = [](wchar_t character) {
                    return std::iswspace(character) || character == L'\\';
                };

                auto start = std::find_if_not(string.begin(), string.end(), should_trim);
                auto end = std::find_if_not(string.rbegin(), string.rend(), should_trim).base();

                if (start >= end) {
                    return L"";
                }

                return std::wstring(start, end);

            };

            size_t separator_index = bundle.find('|');

            if(bundle.find('\\') < (separator_index == std::wstring::npos ? bundle.size() : separator_index)) {
                Wh_Log(L"Invalid bundle name for \"%s\".", bundle.c_str());
                return;
            }

            if(separator_index != std::wstring::npos) {

                bundle_id = trim(bundle.substr(0, separator_index));
                assets_folder = trim(bundle.substr(separator_index + 1));

                if(assets_folder.find(L".\\") != std::wstring::npos ||
                   assets_folder.find(L"\\.") != std::wstring::npos ||
                   assets_folder == L"..") {
                    assets_folder = g_default_assets_folder;
                    Wh_Log(L"Invalid assets folder for \"%s\", falling back to default.", bundle_id.c_str());
                }

                std::wstring bundle_folder = find_bundle_folder(bundles_root, bundle_id);

                if(bundle_folder.empty()) {
                    assets_folder = L"";
                    Wh_Log(L"Failed to find bundle folder for \"%s\", skipping redirection.", bundle_id.c_str());
                } else {
                    assets_folder_path = std::filesystem::path(bundle_folder) / assets_folder;
                }

                return;
            }

            bundle_id = trim(bundle);
            std::wstring bundle_folder = find_bundle_folder(bundles_root, bundle_id);

            if(bundle_folder.empty()) {
                assets_folder = L"";
                Wh_Log(L"Failed to find bundle folder for \"%s\", skipping redirection.", bundle_id.c_str());
                return;
            }

            assets_folder = get_assets_folder(std::format(L"{}\\AppxManifest.xml", bundle_folder));

            if(assets_folder.empty()) {
                assets_folder = g_default_assets_folder;
                Wh_Log(L"Failed to get assets folder for \"%s\" automatically, falling back to default.", bundle_id.c_str());
                return;
            }

            assets_folder_path = std::filesystem::path(bundle_folder) / assets_folder;

            Wh_Log(L"Automatically found assets folder for \"%s\" in \"%s\".", bundle_id.c_str(), assets_folder.c_str());

        };

        const auto get_generated_assets_path = [](std::wstring bundle_id, std::wstring assets_folder, std::filesystem::path assets_folder_path, std::wstring ico_file) -> std::wstring {

            WCHAR storage_path_buffer[MAX_PATH];
            if (!Wh_GetModStoragePath(storage_path_buffer, ARRAYSIZE(storage_path_buffer))) {
                Wh_Log(L"Failed to setup ICO redirection: Unable to get mod's storage path.");
                return L"";
            }

            std::error_code error_code;
            auto generated_assets_path = std::filesystem::path{storage_path_buffer} / "generated-assets" / bundle_id / assets_folder;

            if(std::filesystem::is_directory(generated_assets_path, error_code)) {

                WIN32_FILE_ATTRIBUTE_DATA ico_info;
                if (!GetFileAttributesEx(ico_file.c_str(), GetFileExInfoStandard, &ico_info)) {
                    Wh_Log(L"Failed to setup ICO redirection: Unable to get ICO file attributes for \"%s\".", ico_file.c_str());
                    return L"";
                }

                FILETIME ico_last_modified_time = ico_info.ftLastWriteTime;

                WIN32_FILE_ATTRIBUTE_DATA generated_assets_info;
                if (!GetFileAttributesEx(generated_assets_path.c_str(), GetFileExInfoStandard, &generated_assets_info)) {
                    Wh_Log(L"Failed to setup ICO redirection: Unable to get generated assets folder attributes for \"%s\".", bundle_id.c_str());
                    return L"";
                }

                FILETIME generated_assets_creation_time = generated_assets_info.ftCreationTime;

                if (CompareFileTime(&ico_last_modified_time, &generated_assets_creation_time) == 0) {
                    Wh_Log(L"Assets for \"%s\" already generated and source icon didn't change.", bundle_id.c_str());
                    return generated_assets_path;
                }

                for (int suffix = 0; suffix < 100; suffix++) {

                    if (suffix > 0) {
                        generated_assets_path = std::filesystem::path{storage_path_buffer} / "generated-assets" / std::format(L"{}_{}", bundle_id, suffix + 1) / assets_folder;
                    }

                    if (!std::filesystem::is_directory(generated_assets_path, error_code)) {
                        // Generated assets folder doesn't exist, we can use it.
                        break;
                    }

                    // Generated assets folder exists, try to remove it.
                    Wh_Log(L"Generated assets folder already exists, trying to remove: %s", generated_assets_path.c_str());
                    std::filesystem::remove_all(generated_assets_path, error_code);

                    if (!std::filesystem::is_directory(generated_assets_path, error_code)) {
                        // Successfully removed Generated assets folder.
                        break;
                    }

                    Wh_Log(L"Failed to remove Generated assets folder, trying next usable name...");

                }

                if (std::filesystem::is_directory(generated_assets_path, error_code)) {
                    Wh_Log(L"Failed to setup ICO redirection: Unable to find a usable Generated assets folder name.");
                    return L"";
                }

                std::filesystem::create_directories(generated_assets_path, error_code);

                HANDLE generated_assets_handle = CreateFile(
                    generated_assets_path.c_str(),
                    FILE_WRITE_ATTRIBUTES,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    nullptr,
                    OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS,
                    nullptr
                );

                if (generated_assets_handle != INVALID_HANDLE_VALUE) {
                    SetFileTime(generated_assets_handle, &ico_last_modified_time, nullptr, nullptr);
                    CloseHandle(generated_assets_handle);
                }

            }

            Gdiplus::GdiplusStartupInput gdi_startup;
            ULONG_PTR gdi_token;

            if (GdiplusStartup(&gdi_token, &gdi_startup, nullptr) != Gdiplus::Ok) {
                Wh_Log(L"Failed to setup ICO redirection: Unable to initialize GDI+.");
                return L"";
            }

            const auto get_clsid_encoder = [](std::wstring mime_type, CLSID* clsid) -> int {

                UINT encoders_count = 0;
                UINT encoders_size = 0;

                Gdiplus::GetImageEncodersSize(&encoders_count, &encoders_size);

                if (encoders_size == 0) {
                    return -1;
                }

                std::vector<Gdiplus::ImageCodecInfo> encoders(encoders_size / sizeof(Gdiplus::ImageCodecInfo));
                Gdiplus::GetImageEncoders(encoders_count, encoders_size, encoders.data());

                for (UINT i = 0; i < encoders_count; ++i) {
                    if (wcscmp(encoders[i].MimeType, mime_type.c_str()) == 0) {
                        *clsid = encoders[i].Clsid;
                        return i;
                    }
                }

                return -1;

            };

            CLSID png_encoder_clsid;

            if (get_clsid_encoder(L"image/png", &png_encoder_clsid) == -1) {
                Wh_Log(L"Failed to setup ICO redirection: Unable to get PNG encoder.");
                return L"";
            }

            // Generate common tiles from an .ico file.
            // Same concept as TileGen: https://github.com/atferrys/TileGen
            // Partially taken and adapted from: https://stackoverflow.com/a/22885412

            const auto generate_tile = [ico_file, png_encoder_clsid](int width, int height, std::wstring output_file) {

                HICON icon = nullptr;

                UINT result = PrivateExtractIconsW(
                    ico_file.c_str(),
                    0,
                    width,
                    height,
                    &icon,
                    nullptr,
                    1,
                    0
                );

                if (result == 0 || icon == nullptr) {
                    Wh_Log(L"Failed to generate asset from ICO file: Unable to read icon.");
                    return;
                }

                ICONINFO icon_info = {};

                if (!GetIconInfo(icon, &icon_info)) {
                    DestroyIcon(icon);
                    Wh_Log(L"Failed to generate asset from ICO file: Unable to get icon info.");
                    return;
                }

                HDC hdc = GetDC(nullptr);

                const auto cleanup = [&hdc, &icon_info, &icon]() {

                    if (hdc) {
                        ReleaseDC(nullptr, hdc);
                    }

                    if (icon_info.hbmColor) {
                        DeleteObject(icon_info.hbmColor);
                    }

                    if (icon_info.hbmMask) {
                        DeleteObject(icon_info.hbmMask);
                    }

                    if (icon) {
                        DestroyIcon(icon);
                    }

                };

                BITMAP bitmap = {};

                if (!GetObject(icon_info.hbmColor, sizeof(BITMAP), &bitmap)) {
                    Wh_Log(L"Failed to generate asset from ICO file: Unable to get bitmap info.");
                    cleanup();
                    return;
                }

                BITMAPINFO bitmap_info = {
                    .bmiHeader = {
                        .biSize = sizeof(BITMAPINFOHEADER),
                        .biWidth = bitmap.bmWidth,
                        .biHeight = -bitmap.bmHeight,
                        .biPlanes = 1,
                        .biBitCount = 32,
                        .biCompression = BI_RGB
                    }
                };

                int pixel_count = bitmap.bmWidth * bitmap.bmHeight;
                std::vector<UINT32> pixels(pixel_count);

                if (!GetDIBits(
                    hdc,
                    icon_info.hbmColor,
                    0,
                    bitmap.bmHeight,
                    pixels.data(),
                    &bitmap_info,
                    DIB_RGB_COLORS
                )) {
                    Wh_Log(L"Failed to generate asset from ICO file: Unable to read bitmap pixels.");
                    cleanup();
                    return;
                }

                bool has_alpha = false;

                for (int pixel : pixels) {
                    if ((pixel & 0xFF000000) != 0) {
                        has_alpha = true;
                        break;
                    }
                }

                if (!has_alpha && icon_info.hbmMask) {

                    std::vector<UINT32> mask(pixel_count);

                    if (!GetDIBits(
                        hdc,
                        icon_info.hbmMask,
                        0,
                        bitmap.bmHeight,
                        mask.data(),
                        &bitmap_info,
                        DIB_RGB_COLORS
                    )) {
                        Wh_Log(L"Failed to generate asset from ICO file: Unable to read bitmap alpha mask.");
                        cleanup();
                        return;
                    }

                    for (int i = 0; i < pixel_count; i++) {
                        if (mask[i] == 0) {
                            pixels[i] |= 0xFF000000;
                        }
                    }

                }

                Gdiplus::Bitmap output_bitmap(
                    bitmap.bmWidth,
                    bitmap.bmHeight,
                    bitmap.bmWidth * 4,
                    PixelFormat32bppARGB,
                    (BYTE*) pixels.data()
                );

                if (output_bitmap.Save(output_file.c_str(), &png_encoder_clsid, nullptr) != Gdiplus::Ok) {
                    Wh_Log(L"Failed to generate asset from ICO file: Unable to save output bitmap: %s", output_file.c_str());
                }

                cleanup();

            };

            for (const auto& entry : std::filesystem::recursive_directory_iterator(assets_folder_path, error_code)) {

                if (!entry.is_regular_file()) {
                    continue;
                }

                std::filesystem::path path = entry.path();
                std::wstring filename = path.filename().wstring();

                if (!filename.ends_with(L".png")) {
                    continue;
                }

                if (filename.find(L"targetsize") == std::wstring::npos) {
                    continue;
                }

                if (filename.find(L"contrast") != std::wstring::npos) {
                    continue;
                }

                Gdiplus::Image tile_image = path.c_str();

                if (tile_image.GetLastStatus() != Gdiplus::Ok) {
                    Wh_Log(L"Failed to generate asset from ICO file: Unable to read Reference Tile: %s", path.c_str());
                    continue;
                }

                UINT tile_width = tile_image.GetWidth();
                UINT tile_height = tile_image.GetHeight();

                if (tile_width != tile_height) {
                    continue;
                }

                const auto generated_tile_path = generated_assets_path / path.lexically_relative(assets_folder_path);
                std::filesystem::create_directories(generated_tile_path.parent_path(), error_code);

                generate_tile(tile_width, tile_height, generated_tile_path);

            }

            Gdiplus::GdiplusShutdown(gdi_token);

            Wh_Log(L"Assets for \"%s\" generated from icon successfully.", bundle_id.c_str());

            return generated_assets_path;

        };

        std::wstring bundle_id;
        std::wstring assets_folder;
        std::filesystem::path assets_folder_path;

        deduct_bundle(bundles_root, std::wstring(bundle), bundle_id, assets_folder, assets_folder_path);

        if(bundle_id.empty() || assets_folder.empty()) {
            return;
        }

        std::filesystem::path normalized_redirect = normalize_path(redirect, theme_folder);
        std::error_code error_code;

        if(!assets_folder_path.empty() &&
            std::filesystem::exists(normalized_redirect, error_code) &&
            std::filesystem::is_regular_file(normalized_redirect, error_code) &&
            normalized_redirect.extension() == ".ico") {

            std::wstring generated_assets_path = get_generated_assets_path(
                bundle_id,
                assets_folder,
                assets_folder_path,
                normalized_redirect
            );

            if(generated_assets_path.empty()) {
                return;
            }

            normalized_redirect = generated_assets_path;

        }

        auto path = std::format(L"\\??\\{}\\{}_*\\{}", bundles_root, bundle_id, assets_folder);
        auto redirection = std::format(L"\\??\\{}", normalized_redirect.wstring());

        redirections[path] = redirection;

    };

    auto add_custom_redirection = [&redirections, normalize_path](std::wstring assets_path, std::wstring redirect, std::filesystem::path theme_folder = g_normalize_path_base_path) {

        const auto is_path_valid = [normalize_path](std::wstring path) -> BOOL {

            // Matches ? as a single character, and * as any number of characters
            const auto pattern_match = [](const wchar_t* pat, size_t plen, const wchar_t* str, size_t slen) -> BOOL {
                const wchar_t* p = pat;
                const wchar_t* s = str;
                const wchar_t* p_end = pat + plen;
                const wchar_t* s_end = str + slen;

                const wchar_t* star_pat = nullptr;
                const wchar_t* star_str = nullptr;

                while (s < s_end) {
                    if (p < p_end && (*p == L'?' || std::towlower(*p) == std::towlower(*s))) {
                        ++p;
                        ++s;
                    } else if (p < p_end && *p == L'*') {
                        star_pat = p++;
                        star_str = s;
                    } else if (star_pat) {
                        p = star_pat + 1;
                        s = ++star_str;
                    } else {
                        return false;
                    }
                }

                while (p < p_end && *p == L'*') {
                    ++p;
                }

                return p == p_end;
            };

            std::wstring normalized_path = normalize_path(path);

            for(const auto& blacklisted_path : g_custom_redirections_blacklist) {

                // Check if the path is in blacklist
                if(pattern_match(blacklisted_path.data(), blacklisted_path.size(), normalized_path.data(), normalized_path.size())) {
                    return false;
                }

                // Check if the path pattern matches one of the paths that are in blacklist.
                // Will prevent the matching of blacklisted folders with wildcards
                if(pattern_match(normalized_path.data(), normalized_path.size(), blacklisted_path.data(), blacklisted_path.size())) {
                    return false;
                }

            }

            return true;

        };

        if(!is_path_valid(assets_path)) {
            Wh_Log(L"Ignoring illegal custom redirection path: %s", assets_path.c_str());
            return;
        }

        auto path = std::format(L"\\??\\{}", normalize_path(assets_path));
        auto redirection = std::format(L"\\??\\{}", normalize_path(redirect, theme_folder));

        redirections[path] = redirection;

    };

    auto add_theme_redirections = [add_bundle_redirection, add_custom_redirection](std::filesystem::path theme_ini, std::filesystem::path theme_folder) {

        std::error_code error_code;

        if(!std::filesystem::exists(theme_ini, error_code)) {
            Wh_Log(L"Failed to read theme file, path doesn't exist: %s", theme_ini.c_str());
            return;
        }

        const auto read_section = [theme_ini, &error_code](std::wstring section_key, auto on_pair_read) {

            auto theme_ini_size = std::filesystem::file_size(theme_ini, error_code);
            std::wstring buffer(theme_ini_size + 2, L'\0');

            DWORD result = GetPrivateProfileSection(
                section_key.c_str(),
                buffer.data(), buffer.size(),
                theme_ini.c_str()
            );

            if (!result || result == buffer.size() - 2) {
                Wh_Log(L"Error reading section \"%s\" from theme file: %s - Error %u", section_key.c_str(), theme_ini.c_str(), GetLastError());
                return;
            }

            const wchar_t* ptr = buffer.data();

            while (*ptr) {

                std::wstring entry(ptr);
                size_t separator_index = entry.find(L'=');

                if (separator_index != std::wstring::npos) {

                    std::wstring key = entry.substr(0, separator_index);
                    std::wstring value = entry.substr(separator_index + 1);

                    on_pair_read(key, value);

                }

                ptr += entry.size() + 1;

            }

        };

        // Load WindowsApps redirections

        read_section(L"windows-apps", [add_bundle_redirection, theme_folder](std::wstring bundle, std::wstring redirect) {
            add_bundle_redirection(L"C:\\Program Files\\WindowsApps", bundle, redirect, theme_folder);
        });

        // Load SystemApps redirections

        read_section(L"system-apps", [add_bundle_redirection, theme_folder](std::wstring bundle, std::wstring redirect) {
            add_bundle_redirection(L"C:\\Windows\\SystemApps", bundle, redirect, theme_folder);
        });

        // Load Custom redirections

        read_section(L"custom", [add_custom_redirection, theme_folder](std::wstring assets_path, std::wstring redirect) {
            add_custom_redirection(assets_path, redirect, theme_folder);
        });

    };

    auto load_config_redirections = [add_bundle_redirection, add_custom_redirection]() {

        // Load WindowsApps redirections

        for(int i = 0;; i++) {

            PCWSTR bundle = Wh_GetStringSetting(L"windows-apps[%d].bundle", i);
            PCWSTR redirect = Wh_GetStringSetting(L"windows-apps[%d].redirect", i);

            bool hasRedirection = *bundle && *redirect;

            if(hasRedirection) {
                add_bundle_redirection(L"C:\\Program Files\\WindowsApps", bundle, redirect);
            }

            Wh_FreeStringSetting(bundle);
            Wh_FreeStringSetting(redirect);

            if(!hasRedirection) {
                break;
            }

        }

        // Load SystemApps redirections

        for(int i = 0;; i++) {

            PCWSTR bundle = Wh_GetStringSetting(L"system-apps[%d].bundle", i);
            PCWSTR redirect = Wh_GetStringSetting(L"system-apps[%d].redirect", i);

            bool hasRedirection = *bundle && *redirect;

            if(hasRedirection) {
                add_bundle_redirection(L"C:\\Windows\\SystemApps", bundle, redirect);
            }

            Wh_FreeStringSetting(bundle);
            Wh_FreeStringSetting(redirect);

            if(!hasRedirection) {
                break;
            }

        }

        // Load Custom redirections

        for(int i = 0;; i++) {

            PCWSTR assets_path = Wh_GetStringSetting(L"custom[%d].assets-path", i);
            PCWSTR redirect = Wh_GetStringSetting(L"custom[%d].redirect", i);

            bool hasRedirection = *assets_path && *redirect;

            if(hasRedirection) {
                add_custom_redirection(assets_path, redirect);
            }

            Wh_FreeStringSetting(assets_path);
            Wh_FreeStringSetting(redirect);

            if(!hasRedirection) {
                break;
            }

        }

    };

    auto load_themes_redirections = [add_theme_redirections, normalize_path]() {

        for(int i = 0;; i++) {

            PCWSTR theme_path = Wh_GetStringSetting(L"theme-paths[%d]", i);

            bool hasThemePath = *theme_path;

            if(hasThemePath) {

                std::filesystem::path normalized_theme_path = normalize_path(theme_path);

                std::filesystem::path theme_ini;
                std::filesystem::path theme_folder;

                if (std::filesystem::is_directory(normalized_theme_path)) {
                    theme_ini = normalized_theme_path / L"theme.ini";
                    theme_folder = normalized_theme_path;
                } else {
                    theme_ini = normalized_theme_path;
                    theme_folder = normalized_theme_path.parent_path();
                }

                add_theme_redirections(theme_ini, theme_folder);

            }

            Wh_FreeStringSetting(theme_path);

            if(!hasThemePath) {
                break;
            }

        }

    };

    auto load_icon_theme_redirections = [add_theme_redirections]() {

        // Download icon themes directly with mod options.
        // Mostly taken and adapted from:
        // https://github.com/ramensoftware/windhawk-mods/blob/9ee749979a3dfa668f4a16432d449f77ffb4fce9/mods/icon-resource-redirect.wh.cpp#L2150-L2453

        const auto get_icon_theme_path = [](std::wstring icon_theme) -> std::wstring {

            const auto replace = [](std::wstring string, const wchar_t old_character, const wchar_t new_character) -> std::wstring {
                std::wstring string_copy = string;
                std::replace(string_copy.begin(), string_copy.end(), old_character, new_character);
                return string_copy;
            };

            const auto create_lock = [](std::filesystem::path lock_file, DWORD timeout) {

                HANDLE hFile = CreateFile(
                    lock_file.c_str(),
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    nullptr,
                    OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                    nullptr
                );

                if (hFile == INVALID_HANDLE_VALUE) {
                    return INVALID_HANDLE_VALUE;
                }

                HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

                if (!hEvent) {
                    CloseHandle(hFile);
                    return INVALID_HANDLE_VALUE;
                }

                OVERLAPPED ov = {
                    .hEvent = hEvent
                };

                // Lock first byte only.
                BOOL locked = LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &ov);

                if (!locked) {

                    DWORD err = GetLastError();

                    if (err != ERROR_IO_PENDING) {
                        CloseHandle(hEvent);
                        CloseHandle(hFile);
                        return INVALID_HANDLE_VALUE;
                    }

                    DWORD waitResult = WaitForSingleObject(hEvent, timeout);

                    if (waitResult != WAIT_OBJECT_0) {
                        CancelIo(hFile);
                        CloseHandle(hEvent);
                        CloseHandle(hFile);
                        return INVALID_HANDLE_VALUE;
                    }

                    DWORD bytesTransferred;

                    if (!GetOverlappedResult(hFile, &ov, &bytesTransferred, FALSE)) {
                        CloseHandle(hEvent);
                        CloseHandle(hFile);
                        return INVALID_HANDLE_VALUE;
                    }

                }

                CloseHandle(hEvent);

                return hFile;

            };

            const auto remove_lock = [](HANDLE lock_handle, std::filesystem::path lock_file) {

                OVERLAPPED ov = {};
                BOOL unlocked = UnlockFileEx(lock_handle, 0, 1, 0, &ov);
                CloseHandle(lock_handle);

                if(unlocked) {
                    DeleteFile(lock_file.c_str());
                }

                return unlocked;

            };

            const auto download_theme = [](std::wstring icon_theme, std::filesystem::path& temp_zip, std::filesystem::path& temp_extract, std::filesystem::path& icon_theme_path) {

                const auto unzip_to_folder = [](std::filesystem::path zip_file, std::filesystem::path destination_path) {

                    winrt::com_ptr<IShellDispatch> shellDispatch;
                    HRESULT result = CoCreateInstance(
                        CLSID_Shell,
                        nullptr,
                        CLSCTX_INPROC_SERVER,
                        IID_PPV_ARGS(shellDispatch.put())
                    );

                    if (FAILED(result)) {
                        return result;
                    }

                    VARIANT zipFileVariant;
                    zipFileVariant.vt = VT_BSTR;
                    zipFileVariant.bstrVal = _bstr_t(zip_file.c_str());

                    winrt::com_ptr<Folder> zipFile;
                    result = shellDispatch->NameSpace(zipFileVariant, zipFile.put());

                    if (FAILED(result)) {
                        return result;
                    }

                    if (!zipFile) {
                        return E_FAIL;
                    }

                    VARIANT destinationVariant;
                    destinationVariant.vt = VT_BSTR;
                    destinationVariant.bstrVal = _bstr_t(destination_path.c_str());

                    winrt::com_ptr<Folder> destination;
                    result = shellDispatch->NameSpace(destinationVariant, destination.put());

                    if (FAILED(result)) {
                        return result;
                    }

                    if (!destination) {
                        return E_FAIL;
                    }


                    winrt::com_ptr<FolderItems> zipFiles;
                    result = zipFile->Items(zipFiles.put());

                    if (FAILED(result)) {
                        return result;
                    }

                    if (!zipFiles) {
                        return E_FAIL;
                    }

                    LONG zipFilesCount;
                    result = zipFiles->get_Count(&zipFilesCount);

                    if (FAILED(result)) {
                        return result;
                    }

                    // If the zip contains a single folder, select it to avoid an extra nesting.
                    if (zipFilesCount == 1) {

                        VARIANT index;
                        index.vt = VT_I4;
                        index.lVal = 0;

                        winrt::com_ptr<FolderItem> zipSubFolderItem;
                        result = zipFiles->Item(index, zipSubFolderItem.put());

                        if (FAILED(result)) {
                            return result;
                        }

                        if (!zipSubFolderItem) {
                            return E_FAIL;
                        }

                        VARIANT_BOOL isFolder;
                        result = zipSubFolderItem->get_IsFolder(&isFolder);

                        if (FAILED(result)) {
                            return result;
                        }

                        if (isFolder) {

                            winrt::com_ptr<IDispatch> zipSubFolderDispatch;
                            result = zipSubFolderItem->get_GetFolder(zipSubFolderDispatch.put());

                            if (FAILED(result)) {
                                return result;
                            }

                            if (!zipSubFolderDispatch) {
                                return E_FAIL;
                            }

                            winrt::com_ptr<Folder> zipSubFolder;
                            result = zipSubFolderDispatch->QueryInterface(IID_PPV_ARGS(zipSubFolder.put()));

                            if (FAILED(result)) {
                                return result;
                            }

                            if (!zipSubFolder) {
                                return E_FAIL;
                            }

                            result = zipSubFolder->Items(zipFiles.put());

                            if (FAILED(result)) {
                                return result;
                            }

                            if (!zipFiles) {
                                return E_FAIL;
                            }

                        }

                    }

                    winrt::com_ptr<IDispatch> zipFilesDispatch;
                    result = zipFiles->QueryInterface(IID_PPV_ARGS(zipFilesDispatch.put()));

                    if (FAILED(result)) {
                        return result;
                    }

                    if (!zipFilesDispatch) {
                        return E_FAIL;
                    }

                    VARIANT itemVariant;
                    itemVariant.vt = VT_DISPATCH;
                    itemVariant.pdispVal = zipFilesDispatch.get();

                    VARIANT options;
                    options.vt = VT_I4;
                    options.lVal = FOF_NO_UI;

                    return destination->CopyHere(itemVariant, options);

                };

                if (g_downloading_theme_prompt_thread) {
                    if (WaitForSingleObject(g_downloading_theme_prompt_thread, 0) != WAIT_OBJECT_0) {
                        return false;
                    }
                    CloseHandle(g_downloading_theme_prompt_thread);
                }

                static decltype(&TaskDialogIndirect) pTaskDialogIndirect = []() {
                    HMODULE hComctl32 = LoadLibraryEx(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
                    if (!hComctl32) {
                        Wh_Log(L"Failed to load comctl32.dll");
                            return (decltype(&TaskDialogIndirect)) nullptr;
                        }
                    return (decltype(&TaskDialogIndirect)) GetProcAddress(hComctl32, "TaskDialogIndirect");
                }();

                g_downloading_theme_prompt_thread = CreateThread(
                    nullptr,
                    0,
                    [](LPVOID lpParameter) WINAPI -> DWORD {

                        if(!pTaskDialogIndirect) {
                            return 0;
                        }

                        const std::wstring icon_theme = *static_cast<std::wstring*>(lpParameter);
                        const std::wstring message = std::format(g_downloading_theme_prompt_body, icon_theme.c_str());

                        const TASKDIALOG_BUTTON buttons[] = {
                            {IDCLOSE, L"Hide"}
                        };

                        TASKDIALOGCONFIG promptDialogConfig {
                            .cbSize = sizeof(promptDialogConfig),
                            .dwFlags = TDF_SHOW_MARQUEE_PROGRESS_BAR,
                            .dwCommonButtons = 0,

                            .pszWindowTitle = g_downloading_theme_prompt_title,
                            .pszMainIcon = TD_INFORMATION_ICON,
                            .pszMainInstruction = g_downloading_theme_prompt_header,
                            .pszContent = message.c_str(),

                            .cButtons = ARRAYSIZE(buttons),
                            .pButtons = buttons,

                            .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData) WINAPI -> HRESULT {
                                switch (msg) {
                                    case TDN_CREATED:
                                        g_downloading_theme_prompt = hwnd;
                                        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                                        SendMessage(hwnd, TDM_SET_PROGRESS_BAR_MARQUEE, TRUE, 0);
                                        break;

                                    case TDN_DESTROYED:
                                        g_downloading_theme_prompt = nullptr;
                                        break;
                                }
                                return S_OK;
                            },
                        };

                        pTaskDialogIndirect(&promptDialogConfig, nullptr, nullptr, nullptr);

                        delete static_cast<std::wstring*>(lpParameter);

                        return 0;

                    },
                    new std::wstring(icon_theme),
                    0,
                    nullptr
                );

                const auto show_error_message = [](std::wstring message) {

                    if(g_downloading_theme_prompt) {
                        SendMessage(g_downloading_theme_prompt, TDM_CLICK_BUTTON, TDCBF_CLOSE_BUTTON, 0);
                    }

                    if(!pTaskDialogIndirect) {
                        return;
                    }

                    TASKDIALOGCONFIG promptDialogConfig {
                        .cbSize = sizeof(promptDialogConfig),
                        .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
                        .dwCommonButtons = TDCBF_CLOSE_BUTTON,

                        .pszWindowTitle = g_failed_to_download_theme_prompt_title,
                        .pszMainIcon = TD_ERROR_ICON,
                        .pszMainInstruction = g_failed_to_download_theme_prompt_header,
                        .pszContent = message.c_str(),

                        .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData) WINAPI -> HRESULT {
                            switch (msg) {
                                case TDN_CREATED:
                                    g_failed_to_download_theme_prompt = hwnd;
                                    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                                    break;

                                case TDN_DESTROYED:
                                    g_failed_to_download_theme_prompt = nullptr;
                                    break;
                            }
                            return S_OK;
                        },
                    };

                    pTaskDialogIndirect(&promptDialogConfig, nullptr, nullptr, nullptr);

                };

                const std::wstring theme_url = std::format(
                    L"https://raw.githubusercontent.com/atferrys/uwp-assets-redirect/refs/heads/main/themes/{}.zip",
                    icon_theme
                );

                WH_GET_URL_CONTENT_OPTIONS options {
                    .optionsSize = sizeof(options),
                    .targetFilePath = temp_zip.c_str(),
                };

                const WH_URL_CONTENT* response = Wh_GetUrlContent(theme_url.c_str(), &options);

                if (!response) {
                    Wh_Log(L"Failed to download Icon theme: Wh_GetUrlContent returned NULL.");
                    show_error_message(L"An unknown error occurred while downloading the Icon theme.");
                    return false;
                }

                if (response->statusCode != 200) {
                    Wh_Log(L"Failed to download Icon theme: Request failed with code %d.", response->statusCode);
                    Wh_FreeUrlContent(response);
                    show_error_message(std::format(L"Icon theme download request failed with code {}.", response->statusCode));
                    return false;
                }

                Wh_FreeUrlContent(response);

                HRESULT result = CoInitialize(nullptr);

                if(FAILED(result)) {
                    Wh_Log(L"Failed to download Icon theme: CoInitialize returned 0x%08X", result);
                    show_error_message(std::format(L"COM initialization failed with code 0x{:08X}.", result));
                    return false;
                }

                std::error_code error_code;
                std::filesystem::remove_all(temp_extract, error_code);
                std::filesystem::create_directories(temp_extract, error_code);

                result = unzip_to_folder(temp_zip, temp_extract);

                if (FAILED(result)) {
                    Wh_Log(L"Failed to download Icon theme: UnzipToFolder returned 0x%08X", result);
                    show_error_message(std::format(L"Unzipping of downloaded theme file failed with code 0x{:08X}.", result));
                    return false;
                }

                std::filesystem::create_directories(icon_theme_path.parent_path());
                std::filesystem::rename(temp_extract, icon_theme_path, error_code);

                DeleteFile(temp_zip.c_str());
                CoUninitialize();

                if(g_downloading_theme_prompt) {
                    SendMessage(g_downloading_theme_prompt, TDM_CLICK_BUTTON, TDCBF_CLOSE_BUTTON, 0);
                }

                return std::filesystem::is_directory(icon_theme_path, error_code);

            };

            if(icon_theme.empty()) {
                return L"";
            }

            WCHAR storage_path_buffer[MAX_PATH];
            if (!Wh_GetModStoragePath(storage_path_buffer, ARRAYSIZE(storage_path_buffer))) {
                Wh_Log(L"Failed to setup Icon theme: Unable to get mod's storage path.");
                return L"";
            }

            const auto storage_path = std::filesystem::path{storage_path_buffer} / "icon-themes";
            std::filesystem::create_directories(storage_path);

            // Workaround for: https://github.com/ramensoftware/windhawk-mods/pull/4005#discussion_r3213756330
            // Taken and adapted from: https://github.com/m417z/my-windhawk-mods/commit/cdeeca27376e98cecea2e2f8b637902c86af9699
            const std::wstring icon_theme_path_key = std::format(L"icon_theme_path_{}", icon_theme);

            WCHAR stored_icon_theme_path[MAX_PATH];
            Wh_GetStringValue(
                icon_theme_path_key.c_str(),
                stored_icon_theme_path,
                ARRAYSIZE(stored_icon_theme_path)
            );

            std::error_code error_code;
            std::filesystem::path icon_theme_path;

            if(*stored_icon_theme_path) {
                icon_theme_path = storage_path / stored_icon_theme_path;
                if (std::filesystem::is_directory(icon_theme_path, error_code)) {
                    Wh_Log(L"Theme files for %s already downloaded.", icon_theme.c_str());
                    return icon_theme_path;
                }
            }

            // Find a usable icon theme folder name.
            const std::wstring icon_theme_folder = replace(icon_theme, L'/', L'\\');

            for (int suffix = 0; suffix < 100; suffix++) {

                if (suffix == 0) {
                    icon_theme_path = storage_path / icon_theme_folder;
                } else {
                    icon_theme_path = storage_path / std::format(L"{}_{}", icon_theme_folder, suffix + 1);
                }

                if (!std::filesystem::is_directory(icon_theme_path, error_code)) {
                    // Icon theme folder doesn't exist, we can use it.
                    break;
                }

                // Icon theme folder exists, try to remove it.
                Wh_Log(L"Icon theme folder already exists, trying to remove: %s", icon_theme_path.c_str());
                std::filesystem::remove_all(icon_theme_path, error_code);

                if (!std::filesystem::is_directory(icon_theme_path, error_code)) {
                    // Successfully removed icon theme folder.
                    break;
                }

                Wh_Log(L"Failed to remove Icon theme folder, trying next usable name...");

            }

            if (std::filesystem::is_directory(icon_theme_path, error_code)) {
                Wh_Log(L"Failed to download Icon theme: Unable to find a usable theme folder name.");
                return L"";
            }

            const auto lock_file = storage_path / std::format(L"{}_lock", replace(icon_theme, L'/', L'_'));
            const HANDLE lock_handle = create_lock(lock_file, 30000);

            if (!lock_handle) {
                Wh_Log(L"Failed to download Icon theme: Unable to create temp lock file.");
                return L"";
            }

            Wh_Log(L"Downloading Icon theme for %s...", icon_theme.c_str());

            auto temp_zip = storage_path / std::format(L"{}_temp.zip", replace(icon_theme, L'/', L'_'));
            auto temp_extract = storage_path / std::format(L"{}_temp_extract", replace(icon_theme, L'/', L'_'));

            if (download_theme(icon_theme, temp_zip, temp_extract, icon_theme_path)) {
                Wh_Log(L"Theme files for %s downloaded and extracted successfully.", icon_theme.c_str());
                // Store the Icon theme folder name.
                Wh_SetStringValue(icon_theme_path_key.c_str(), icon_theme_path.c_str());
            } else {
                Wh_Log(L"Failed to download and extract icon theme.");
                icon_theme_path.clear();
            }

            remove_lock(lock_handle, lock_file);

            return icon_theme_path;

        };

        PCWSTR icon_theme = Wh_GetStringSetting(L"icon-theme");

        if(*icon_theme) {

            std::wstring icon_theme_path = get_icon_theme_path(std::wstring(icon_theme));

            if(!icon_theme_path.empty()) {
                add_theme_redirections(
                    std::filesystem::path(icon_theme_path) / "theme.ini",
                    std::filesystem::path(icon_theme_path)
                );
            }

        }

        Wh_FreeStringSetting(icon_theme);

    };

    load_icon_theme_redirections();
    load_themes_redirections();
    load_config_redirections();

}

std::atomic<HWND> g_ownership_prompt;

constexpr WCHAR g_ownership_prompt_title[] = L"UWP Assets Redirect - Windhawk";
constexpr WCHAR g_ownership_prompt_header[] = L"Access required";
constexpr WCHAR g_ownership_prompt_body[] =
    L"To work properly, UWP Assets Redirect requires permission to access certain system-protected locations.\n"
    L"\n"
    L"To continue, click on \"Allow\" and respond to the UAC prompt.";

constexpr WCHAR g_ownership_command[] =
    LR"(powershell -NoProfile -WindowStyle minimized Start-Process -Wait -FilePath cmd -Verb RunAs -ArgumentList ')"

    LR"(/c title "UWP Assets Redirect - Windhawk")"
    LR"(& echo Updating permissions, please wait...)"
    LR"(& takeown /F \"C:\Program Files\WindowsApps\" /A)"
    LR"(& icacls \"C:\Program Files\WindowsApps\" /grant \"%COMPUTERNAME%\%USERNAME%:(R)\"')";

void CheckWindowsAppsOwner() {

    const auto is_owner_administrators = [](std::wstring folder_path) -> BOOL {

        PSECURITY_DESCRIPTOR security_descriptor = nullptr;
        PSID owner_sid = nullptr;

        DWORD result = GetNamedSecurityInfoW(
            folder_path.c_str(),
            SE_FILE_OBJECT,
            OWNER_SECURITY_INFORMATION,
            &owner_sid,
            nullptr,
            nullptr,
            nullptr,
            &security_descriptor
        );

        if (result != ERROR_SUCCESS) {
            if(security_descriptor) {
                LocalFree(security_descriptor);
            }
            return false;
        }

        SID_IDENTIFIER_AUTHORITY nt_authority_sid = SECURITY_NT_AUTHORITY;
        PSID administrators_sid = nullptr;

        WINBOOL allocation_result = AllocateAndInitializeSid(
            &nt_authority_sid,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0,
            0,
            0,
            0,
            0,
            0,
            &administrators_sid
        );

        if (!allocation_result) {
            LocalFree(security_descriptor);
            return false;
        }

        BOOL is_owner_adminstators = EqualSid(owner_sid, administrators_sid);

        FreeSid(administrators_sid);
        LocalFree(security_descriptor);

        return is_owner_adminstators;

    };

    if(is_owner_administrators(L"C:\\Program Files\\WindowsApps")) {
        return;
    }

    Wh_Log(L"WindowsApps folder not owned by Administrators group. This will prevent reads, asking permissions...");

    static decltype(&TaskDialogIndirect) pTaskDialogIndirect = []() {
        HMODULE hComctl32 = LoadLibraryEx(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!hComctl32) {
            Wh_Log(L"Failed to load comctl32.dll");
            return (decltype(&TaskDialogIndirect)) nullptr;
        }
        return (decltype(&TaskDialogIndirect)) GetProcAddress(hComctl32, "TaskDialogIndirect");
    }();

    if(!pTaskDialogIndirect) {
        return;
    }

    TASKDIALOG_BUTTON buttons[] = {
        {IDYES, L"Allow"},
        {IDCANCEL, L"Cancel"}
    };

    TASKDIALOGCONFIG promptDialogConfig {
        .cbSize = sizeof(promptDialogConfig),
        .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
        .dwCommonButtons = 0,

        .pszWindowTitle = g_ownership_prompt_title,
        .pszMainIcon = TD_WARNING_ICON,
        .pszMainInstruction = g_ownership_prompt_header,
        .pszContent = g_ownership_prompt_body,

        .cButtons = ARRAYSIZE(buttons),
        .pButtons = buttons,

        .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData) WINAPI -> HRESULT {
            switch (msg) {
                case TDN_CREATED:
                    g_ownership_prompt = hwnd;
                    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    SendMessage(hwnd, TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE, IDYES, TRUE);
                    break;

                case TDN_DESTROYED:
                    g_ownership_prompt = nullptr;
                    break;
            }
            return S_OK;
        },
    };

    int button;
    if (SUCCEEDED(pTaskDialogIndirect(&promptDialogConfig, &button, nullptr, nullptr)) && button == IDYES) {
        WCHAR commandLine[ARRAYSIZE(g_ownership_command)];
        memcpy(commandLine, g_ownership_command, sizeof(g_ownership_command));
        STARTUPINFO si = {
            .cb = sizeof(si),
        };
        PROCESS_INFORMATION pi{};
        if (CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }

}

void LoadSettings() {

    std::unordered_map<std::wstring, std::wstring> redirections;

    if(DoesCurrentProcessOwnTaskbar()) {

        CheckWindowsAppsOwner();

        ClearRedirectionsCache(false);
        LoadRedirections(redirections);
        write_redirections_cache(redirections);

        BOOL grant_permissions = Wh_GetIntSetting(L"grant-permissions");

        if (!grant_permissions && g_grant_permissions) {
            TogglePermissions(redirections, false);
        } else if (grant_permissions) {
            TogglePermissions(redirections, true);
        }

        g_grant_permissions = grant_permissions;

        Wh_Log(L"Loaded %i redirections and stored them to shared cache.", redirections.size());
        g_redirections = std::move(redirections);

    } else {

        int tries = 0;

        while(tries++ < g_max_read_tries && !read_redirections_cache(redirections)) {
            Wh_Log(L"Redirections haven't been cached yet, retrying... (%i/%i)", tries, g_max_read_tries);
            Sleep(250);
        }

        if(tries > g_max_read_tries) {
            Wh_Log(L"Redirections cache read tries exceeded.");
            return;
        }

        Wh_Log(L"Loaded %i redirections from shared cache.", redirections.size());
        g_redirections = std::move(redirections);

    }

}

BOOL Wh_ModInit() {

    LoadSettings();

    HMODULE ntdll = GetModuleHandle(L"ntdll.dll");

    Wh_SetFunctionHook(
        (void*)GetProcAddress(ntdll, "NtCreateFile"),
        (void*)NtCreateFile_Hook,
        (void**)&NtCreateFile_Original
    );

    RefreshIcons(true);

    Wh_Log(L"UWP Assets Redirect has been initialized.");

    return TRUE;

}

void Wh_ModSettingsChanged() {

    Wh_Log(L"Reloading configuration...");

    LoadSettings();
    RefreshIcons();

}

void Wh_ModUninit() {

    ClearRedirectionsCache();

    RefreshIcons();
    MarkShouldRefreshIcons();

    if(g_grant_permissions) {
        TogglePermissions(g_redirections, false);
    }

}