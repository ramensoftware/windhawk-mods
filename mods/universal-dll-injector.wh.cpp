// ==WindhawkMod==
// @id              universal-dll-injector
// @name            Universal DLL Injector
// @name:zh-CN      通用 DLL 注入器
// @description     Loads one or more DLLs of your choice into the target process when it starts
// @description:zh-CN 目标进程启动时加载一个或多个你指定的 DLL
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Universal DLL Injector

Loads DLLs of your choice into a process you choose, when that process starts.

> **Note:** This mod loads prebuilt DLLs that are not part of the mod's source
> and that nobody has reviewed. That is what it is for, but it means the mod is
> only as trustworthy as the DLLs you point it at. Only use DLLs you built
> yourself or obtained from a source you trust.

## Usage

1. Open the mod in Windhawk, go to the **Advanced** tab, and put the executable
   name of your target in the **Custom process inclusion list**. The mod targets
   nothing until you do.
2. Fill in the full paths of the DLLs to load under mod settings →
   **DLL path list**.
3. Every listed DLL is loaded each time the target process starts.

Removing an entry from the list unloads that DLL; disabling the mod unloads all
of them. A DLL that started threads or installed hooks of its own cannot be
fully unloaded by `FreeLibrary`, so those effects can remain until the target
process restarts.

## Notes

- A DLL must match the target process architecture (64-bit process → 64-bit
  DLL).
- DLL paths support environment variables, for example `%APPDATA%\my.dll`.
- **Paths must not be writable by anyone but you.** If the target process runs
  elevated and the DLL sits in a folder that other users or programs can write
  to, that DLL is loaded with elevated privileges, which lets anything that can
  replace the file run code at that level. Keep such DLLs under a location only
  administrators can write to, such as `C:\Program Files\`.
- Because of that, injection into processes running above medium integrity is
  skipped unless **Allow elevated targets** is turned on. Turn it on only if you
  understand the risk above.
- The result for each DLL is written to Windhawk's **Log** panel.
*/
// ==WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DllPaths:
    - - Path: ""
        $name: DLL path
        $name:zh-CN: DLL 路径
        $description: The full path of the DLL to load, for example C:\tools\my.dll
        $description:zh-CN: 要加载的 DLL 完整路径，例如 C:\tools\my.dll
  $name: DLL path list
  $name:zh-CN: DLL 路径列表
  $description: Loaded in order; each entry holds the full path of one DLL.
  $description:zh-CN: 按顺序加载，每条记录填写一个 DLL 的完整路径
- allowElevated: false
  $name: Allow elevated targets
  $name:zh-CN: 允许提权目标
  $description: >-
    Load the DLLs even when the target process runs elevated. Only turn this on
    if the DLL paths are in a location only administrators can write to.
  $description:zh-CN: >-
    目标进程以管理员权限运行时也加载 DLL。仅当 DLL 所在位置只有管理员可写时才开启。
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <string>
#include <unordered_map>
#include <vector>

// ──────────────────────────────────────────────
// Settings
// ──────────────────────────────────────────────

std::vector<std::wstring> LoadDllPaths() {
    std::vector<std::wstring> paths;

    for (int i = 0;; i++) {
        PCWSTR raw = Wh_GetStringSetting(L"DllPaths[%d].Path", i);
        bool empty = !raw || !raw[0];
        if (!empty) {
            // Environment variables are expanded so entries can be written
            // without hardcoding a user name.
            WCHAR expanded[MAX_PATH * 2];
            if (ExpandEnvironmentStringsW(raw, expanded,
                                          ARRAYSIZE(expanded)) > 0) {
                paths.emplace_back(expanded);
            } else {
                paths.emplace_back(raw);
            }
        }
        Wh_FreeStringSetting(raw);
        if (empty) {
            break;
        }
    }

    return paths;
}

// ──────────────────────────────────────────────
// Integrity check
// ──────────────────────────────────────────────

// Whether this process runs above medium integrity, i.e. elevated. Loading a
// DLL from a user-writable path into such a process would let unelevated code
// run at high integrity, so it is skipped unless the user opts in.
bool IsAboveMediumIntegrity() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        // Be conservative: if the level can't be read, treat it as elevated.
        return true;
    }

    alignas(TOKEN_MANDATORY_LABEL) BYTE buffer[sizeof(TOKEN_MANDATORY_LABEL) +
                                               SECURITY_MAX_SID_SIZE];
    DWORD size = 0;
    bool aboveMedium = true;
    if (GetTokenInformation(token, TokenIntegrityLevel, buffer, sizeof(buffer),
                            &size)) {
        PSID sid = reinterpret_cast<TOKEN_MANDATORY_LABEL*>(buffer)->Label.Sid;
        DWORD rid = *GetSidSubAuthority(sid, *GetSidSubAuthorityCount(sid) - 1);
        aboveMedium = rid > SECURITY_MANDATORY_MEDIUM_RID;
    }

    CloseHandle(token);
    return aboveMedium;
}

// ──────────────────────────────────────────────
// Loading
// ──────────────────────────────────────────────

// The DLLs this mod loaded, so they can be unloaded when they leave the list
// and when the mod is disabled.
std::unordered_map<std::wstring, HMODULE> g_loaded;

void SyncDlls() {
    auto paths = LoadDllPaths();

    // Unload whatever is no longer listed.
    for (auto it = g_loaded.begin(); it != g_loaded.end();) {
        if (std::find(paths.begin(), paths.end(), it->first) == paths.end()) {
            Wh_Log(L"Unloading %s", it->first.c_str());
            FreeLibrary(it->second);
            it = g_loaded.erase(it);
        } else {
            ++it;
        }
    }

    // Load what is listed and not loaded yet.
    for (const auto& path : paths) {
        if (g_loaded.contains(path)) {
            continue;
        }

        HMODULE module = LoadLibraryW(path.c_str());
        if (module) {
            Wh_Log(L"Loaded %s", path.c_str());
            g_loaded.emplace(path, module);
        } else {
            Wh_Log(L"Failed to load %s (error %u)", path.c_str(),
                   GetLastError());
        }
    }
}

void UnloadAllDlls() {
    for (const auto& [path, module] : g_loaded) {
        Wh_Log(L"Unloading %s", path.c_str());
        FreeLibrary(module);
    }
    g_loaded.clear();
}

// ──────────────────────────────────────────────
// Windhawk lifecycle callbacks
// ──────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (IsAboveMediumIntegrity() &&
        !Wh_GetIntSetting(L"allowElevated")) {
        Wh_Log(L"Target runs elevated, skipping (see the mod settings)");
        return FALSE;
    }

    SyncDlls();

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    UnloadAllDlls();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    SyncDlls();
}
