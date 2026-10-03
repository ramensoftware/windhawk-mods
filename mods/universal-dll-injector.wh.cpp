// ==WindhawkMod==
// @id              universal-dll-injector
// @name            Universal DLL Injector
// @name:zh-CN      通用 DLL 注入器
// @description     Inject one or more custom DLLs into the target process on load
// @description:zh-CN 在目标进程启动时注入一个或多个自定义 DLL
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @include         mspaint.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Universal DLL Injector

Injects custom DLLs into the target process. Fill in the full paths of the
DLLs in the settings, and the mod calls `LoadLibraryW` for each of them in
turn when the target process starts.

## Usage

1. Open the mod in Windhawk, go to **Details** → **Advanced settings**, and put
   the executable name of your target in the **process inclusion list** there.
   It is set to `mspaint.exe` (Paint) as a placeholder, so nothing is injected
   until you change it. No source code changes are needed.
2. Fill in the full paths of the DLLs to inject under mod settings →
   **DLL path list**.
3. Once the mod is enabled, every listed DLL is loaded automatically each time
   the target process starts.

## Notes

- A DLL must match the target process architecture (64-bit process → 64-bit
  DLL).
- DLL paths support environment variables, for example
  `%USERPROFILE%\my.dll`.
- The injection result can be seen in Windhawk's **Log** panel.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DllPaths:
    - - Path: ""
        $name: DLL path
        $name:zh-CN: DLL 路径
        $description: The full path of the DLL to inject, for example C:\tools\my.dll
        $description:zh-CN: 要注入的 DLL 完整路径，例如 C:\tools\my.dll
  $name: DLL path list
  $name:zh-CN: DLL 路径列表
  $description: Injected in order; each entry holds the full path of one DLL.
  $description:zh-CN: 按顺序注入，每条记录填写一个 DLL 的完整路径
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <vector>

// ──────────────────────────────────────────────
// Reads the settings and returns the list of DLL paths
// ──────────────────────────────────────────────
static std::vector<std::wstring> LoadDllPaths() {
    std::vector<std::wstring> paths;

    for (int i = 0; ; i++) {
        PCWSTR raw = Wh_GetStringSetting(L"DllPaths[%d].Path", i);
        // An empty string marks the end of the list
        if (!raw || raw[0] == L'\0') {
            Wh_FreeStringSetting(raw);
            break;
        }

        // Expand environment variables (supports %USERPROFILE% and the like)
        wchar_t expanded[MAX_PATH * 2];
        if (ExpandEnvironmentStringsW(raw, expanded, ARRAYSIZE(expanded)) > 0) {
            paths.emplace_back(expanded);
        } else {
            paths.emplace_back(raw);
        }

        Wh_FreeStringSetting(raw);
    }

    return paths;
}

// ──────────────────────────────────────────────
// Injects every DLL
// ──────────────────────────────────────────────
static void InjectAll() {
    auto paths = LoadDllPaths();

    if (paths.empty()) {
        Wh_Log(L"[custom-dll-injector] No DLL path configured, skipping injection");
        return;
    }

    for (const auto& path : paths) {
        Wh_Log(L"[custom-dll-injector] Loading: %s", path.c_str());

        HMODULE hMod = LoadLibraryW(path.c_str());
        if (hMod) {
            Wh_Log(L"[custom-dll-injector] Succeeded: %s (handle=0x%p)", path.c_str(), (void*)hMod);
        } else {
            DWORD err = GetLastError();
            Wh_Log(L"[custom-dll-injector] Failed: %s (error=%lu)", path.c_str(), err);
        }
    }
}

// ──────────────────────────────────────────────
// Windhawk lifecycle callbacks
// ──────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"[custom-dll-injector] Wh_ModInit");
    InjectAll();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"[custom-dll-injector] Wh_ModAfterInit");
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[custom-dll-injector] Wh_ModBeforeUninit");
    // Note: a DLL already loaded through LoadLibraryW is not unloaded here.
    // To unload it, keep its HMODULE and call FreeLibrary at this point.
}

void Wh_ModUninit() {
    Wh_Log(L"[custom-dll-injector] Wh_ModUninit");
}

// Re-injects when the settings change (only newly added DLLs are loaded; the
// ones already loaded are not loaded again)
void Wh_ModSettingsChanged() {
    Wh_Log(L"[custom-dll-injector] Settings changed, injecting again");
    InjectAll();
}