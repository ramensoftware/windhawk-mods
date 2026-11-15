// ==WindhawkMod==
// @id              ep-taskbar-button-width
// @name            ExplorerPatcher Taskbar Button Width
// @description     Customize the minimum width of taskbar buttons when using ExplorerPatcher's Windows 10 taskbar
// @version         1.0.0
// @author          Rod Boev
// @github          https://github.com/rodboev
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ExplorerPatcher Taskbar Button Width

This mod lets you change the width of taskbar items when using ExplorerPatcher's Windows 10 taskbar on Windows 11.

Before:

![Before](https://i.imgur.com/r82ISo3.png)

After (120%):

![After](https://i.imgur.com/qHaRdVt.png)

The item scales as a percentage from a pre-computed base value not exposed by EP.

(The registry hack at
`HKCU\Control Panel\Desktop\WindowMetrics\MinWidth` doesn't work with
ExplorerPatcher.)

**Requirements:** Windows 11 with ExplorerPatcher, using the Windows 10 taskbar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- buttonWidthPercent: 120
  $name: Button width (%)
  $description: "Recommended: 75-150%"
*/
// ==/WindhawkModSettings==

#include <psapi.h>
#include <atomic>

std::atomic<int> g_settingsPercent{120};

std::atomic<bool> g_hooked{false};

using ComputeSingleButtonWidth_t = int(WINAPI*)(void* pThis, int groupType, void* pTaskBtnGroup, int* pWidth);
ComputeSingleButtonWidth_t ComputeSingleButtonWidth_Original;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

int WINAPI ComputeSingleButtonWidth_Hook(void* pThis, int groupType, void* pTaskBtnGroup, int* pWidth) {
    int result = ComputeSingleButtonWidth_Original(pThis, groupType, pTaskBtnGroup, pWidth);

    int percent = g_settingsPercent.load();
    if (percent != 100) {
        result = (result * percent) / 100;
        if (pWidth) {
            *pWidth = (*pWidth * percent) / 100;
        }
    }

    return result;
}

void RefreshTaskbar() {
    // Primary taskbar: Shell_TrayWnd -> ReBarWindow32 -> MSTaskSwWClass
    HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        HWND hReBarWindow32 = FindWindowEx(taskbar, nullptr, L"ReBarWindow32", nullptr);
        if (hReBarWindow32) {
            HWND hMSTaskSwWClass = FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
            if (hMSTaskSwWClass) {
                SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
            }
        }
    }

    // Secondary taskbars: Shell_SecondaryTrayWnd -> WorkerW -> MSTaskSwWClass
    HWND secondaryTaskbar = nullptr;
    while ((secondaryTaskbar = FindWindowEx(nullptr, secondaryTaskbar, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        HWND hWorkerW = FindWindowEx(secondaryTaskbar, nullptr, L"WorkerW", nullptr);
        if (hWorkerW) {
            HWND hMSTaskSwWClass = FindWindowEx(hWorkerW, nullptr, L"MSTaskSwWClass", nullptr);
            if (hMSTaskSwWClass) {
                SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
            }
        }
    }
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileName(module, path, ARRAYSIZE(path))) {
        return false;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    if (!name) return false;
    name++;
    return _wcsnicmp(L"ep_taskbar.", name, 11) == 0;
}

bool HookExplorerPatcher(HMODULE module, bool calledFromInit) {
    bool expected = false;
    if (!g_hooked.compare_exchange_strong(expected, true)) {
        return true;
    }

    void* ptr = (void*)GetProcAddress(module,
        "?_ComputeSingleButtonWidth@CTaskListWnd@@IEAAHW4eTBGROUPTYPE@@PEAUITaskBtnGroup@@PEAH@Z");

    if (!ptr) {
        Wh_Log(L"ERROR: _ComputeSingleButtonWidth not found");
        g_hooked = false;
        return false;
    }

    if (!Wh_SetFunctionHook(ptr, (void*)ComputeSingleButtonWidth_Hook, (void**)&ComputeSingleButtonWidth_Original)) {
        Wh_Log(L"ERROR: Wh_SetFunctionHook failed");
        g_hooked = false;
        return false;
    }

    if (!calledFromInit) {
        Wh_ApplyHookOperations();
        RefreshTaskbar();
    }

    Wh_Log(L"Hooked at %p (%d%%)", ptr, g_settingsPercent.load());
    return true;
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_hooked && IsExplorerPatcherModule(module)) {
        Wh_Log(L"ExplorerPatcher loaded: %s", lpLibFileName);
        HookExplorerPatcher(module, false);
    }
    return module;
}

void LoadSettings() {
    int percent = Wh_GetIntSetting(L"buttonWidthPercent");
    if (percent <= 0) percent = 120;
    else if (percent < 50) percent = 50;
    else if (percent > 300) percent = 300;
    g_settingsPercent.store(percent);
}

BOOL Wh_ModInit() {
    Wh_Log(L"=== EP Taskbar Button Width v1.0.0 ===");
    LoadSettings();

    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
        size_t count = (cbNeeded < sizeof(hMods) ? cbNeeded : sizeof(hMods)) / sizeof(HMODULE);
        for (size_t i = 0; i < count; i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                HookExplorerPatcher(hMods[i], true);
                break;
            }
        }
    }

    HMODULE kb = GetModuleHandle(L"kernelbase.dll");
    if (kb) {
        auto pLoadLib = (LoadLibraryExW_t)GetProcAddress(kb, "LoadLibraryExW");
        if (pLoadLib) {
            Wh_SetFunctionHook((void*)pLoadLib, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_hooked) {
        RefreshTaskbar();
    }
}

void Wh_ModUninit() {
    g_settingsPercent.store(100);
    RefreshTaskbar();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RefreshTaskbar();
}
