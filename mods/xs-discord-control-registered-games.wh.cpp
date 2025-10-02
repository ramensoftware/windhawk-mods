// ==WindhawkMod==
// @id xs-discord-control-registered-games
// @name Control Discord's Registered Games
// @description This mod allows you to control what games Discord can see and are collected for its Registered Games.
// @version 1.0
// @include Discord.exe
// @author Xetrill
// @github https://github.com/Xetrill/
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## Synopsis
This mod lets you effectively disable or control which games Discord can see and add to its **Registered Games** list.

## Background
Discord publishes games you run to your friends via the Registered Games feature. The built-in controls are awkward:
* Disabling **Share my activity** prevents Discord from announcing your activity, but Discord still scans and adds games to Registered Games.
* To hide a single game while keeping **Share my activity** enabled, you currently must:
  1. Disable **Share my activity**.
  2. Run the game and wait for it to appear in Registered Games (Discord scans every 5 seconds).
  3. Toggle detection off for the game in Registered Games.
  4. Re-enable **Share my activity**.

Discord identifies games by executable name but doesn't let you pre-add any.

## How Discord detects games
Discord takes a snapshot of running processes every 5 seconds, filters process names against its known-game list, and then adds matches to Registered Games.

This cannot be disabled.

## What this mod does
The mod hooks the process-snapshot function and either:
* reports no running processes, or
* returns only the process names explicitly allowed by you.

## Setting: Process Allow List
Contains executable names (including extension) that Discord is allowed to see.

* Default: *empty* (Discord sees no running processes).
* Example: add `UNDERTALE.exe` and save → Discord will be able to detect Undertale.

When the list is empty the mod has a slightly smaller footprint because no pre-filtering is needed.

## Personal note
I find **Registered Games** unnecessary: most launchers already provide richer friend/game integration.
I've never used Discord’s Registered Games other than to disable things on there.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- AllowList: [""]
  $name: Process Allow List
  $description: List of process names (e.g. "game.exe") which are allowed to be scanned by Discord.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tlhelp32.h>
#include <unordered_set>
#include <vector>
#include <string>

static HANDLE (WINAPI *orig_CreateToolhelp32Snapshot)(DWORD, DWORD) = nullptr;
static BOOL (WINAPI *orig_Process32FirstW)(HANDLE, LPPROCESSENTRY32W) = nullptr;
static BOOL (WINAPI *orig_Process32NextW)(HANDLE, LPPROCESSENTRY32W) = nullptr;
static BOOL (WINAPI *orig_CloseHandle)(HANDLE) = nullptr;

std::unordered_set<HANDLE> g_snapshots;
std::vector<std::wstring> g_allowList;

// Normalize to cleanup potentially bad user input
static inline std::wstring normalize(const std::wstring& s)
{
    const auto notSpace = [](wchar_t c) { return !std::iswspace(c); };
    const auto start = std::find_if(s.begin(), s.end(), notSpace);
    const auto end   = std::find_if(s.rbegin(), s.rend(), notSpace).base();

    if (start >= end) {
        return L"";
    }

    auto r = std::wstring(start, end);
    std::transform(r.begin(), r.end(), r.begin(), ::towlower);
    return r;
}

static bool isAllowed(const std::wstring& name)
{
    if (g_allowList.empty())
        return false;

    auto n = name;
    // No 'normalize' as we assume the OS gives proper output
    std::transform(n.begin(), n.end(), n.begin(), ::towlower);

    const auto r = std::binary_search(g_allowList.cbegin(), g_allowList.cend(), n);
    Wh_Log(L"isAllowed('%s'): %d", n.c_str(), r);
    return r;
}

static bool LoadSettings()
{
    std::vector<std::wstring> tmp;

    for (auto i = 0;; ++i) {
        const auto name = Wh_GetStringSetting(L"AllowList[%d]", i);
        if (!name || !*name)
            break;

        const auto n = normalize(name);
        tmp.push_back(n);

        Wh_FreeStringSetting(name);
    }

    std::sort(tmp.begin(), tmp.end());
    tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
    tmp.shrink_to_fit();

    g_allowList.swap(tmp);
    return g_allowList.empty();
}

HANDLE WINAPI hk_CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID)
{
    if ((dwFlags & TH32CS_SNAPPROCESS) == 0)
        return orig_CreateToolhelp32Snapshot(dwFlags, th32ProcessID);

    if (g_allowList.empty()) 
        return INVALID_HANDLE_VALUE;

    auto h = orig_CreateToolhelp32Snapshot(dwFlags, th32ProcessID);
    if (h != INVALID_HANDLE_VALUE && (dwFlags & TH32CS_SNAPPROCESS)) {
        g_snapshots.insert(h);
    }

    return h;
}

BOOL WINAPI hk_CloseHandle(HANDLE hObject)
{
    auto it = g_snapshots.find(hObject);
    if (it != g_snapshots.end()) {
        g_snapshots.erase(it);
    }

    return orig_CloseHandle(hObject);
}

BOOL WINAPI hk_Process32FirstW(HANDLE hSnapshot, LPPROCESSENTRY32W lppe)
{
    if (!orig_Process32FirstW)
        return FALSE;
    
    auto ok = orig_Process32FirstW(hSnapshot, lppe);

    if (g_snapshots.find(hSnapshot) != g_snapshots.cend()) {
        while (ok && !isAllowed(lppe->szExeFile)) {
            ok = orig_Process32NextW(hSnapshot, lppe);
        }
    }

    return ok;
}

BOOL WINAPI hk_Process32NextW(HANDLE hSnapshot, LPPROCESSENTRY32W lppe)
{
    if (!orig_Process32NextW)
        return FALSE;
    
    auto ok = orig_Process32NextW(hSnapshot, lppe);

    if (g_snapshots.find(hSnapshot) != g_snapshots.cend()) {
        while (ok && !isAllowed(lppe->szExeFile)) {
            ok = orig_Process32NextW(hSnapshot, lppe);
        }
    }

    return ok;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload)
{
    const auto wasEmpty = g_allowList.empty();
    const auto isEmpty = LoadSettings();
    if (wasEmpty != isEmpty) {
        *bReload = TRUE;
    }
    return TRUE;
}

BOOL Wh_ModInit(void)
{
    {
        const auto cmdline = std::wstring(GetCommandLineW());
        if (!cmdline.contains(L"--type=renderer"))
            return FALSE;
    }

    const auto useAllowList = !LoadSettings();

    auto hKernel = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel) {
        hKernel = LoadLibraryW(L"kernel32.dll");
        Wh_Log(L"hKernel loaded: %p (err=%u)", hKernel, GetLastError());
    }

    const auto pCloseHandle = (decltype(&CloseHandle))GetProcAddress(hKernel, "CloseHandle");
    if (!pCloseHandle) {
        Wh_Log(L"GetProcAddress failed for CloseHandle; aborting.");
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(pCloseHandle, hk_CloseHandle, &orig_CloseHandle)) {
        Wh_Log(L"SetFunctionHook(CloseHandle) failed (err=%u).", GetLastError());
        return FALSE;
    }

    const auto pCreateToolhelp32Snapshot = (decltype(&CreateToolhelp32Snapshot))GetProcAddress(hKernel, "CreateToolhelp32Snapshot");
    if (!pCreateToolhelp32Snapshot) {
        Wh_Log(L"GetProcAddress failed for CreateToolhelp32Snapshot; aborting.");
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(pCreateToolhelp32Snapshot, hk_CreateToolhelp32Snapshot, &orig_CreateToolhelp32Snapshot)) {
        Wh_Log(L"SetFunctionHook(CreateToolhelp32Snapshot) failed (err=%u).", GetLastError());
        return FALSE;
    }

    if (useAllowList) {
        const auto pProcess32FirstW = (decltype(&Process32FirstW))GetProcAddress(hKernel, "Process32FirstW");
        if (!pProcess32FirstW) {
            Wh_Log(L"GetProcAddress failed for Process32FirstW; aborting.");
            return FALSE;
        }
        if (!WindhawkUtils::SetFunctionHook(pProcess32FirstW, hk_Process32FirstW, &orig_Process32FirstW)) {
            Wh_Log(L"SetFunctionHook(Process32FirstW) failed (err=%u).", GetLastError());
            return FALSE;
        }

        const auto pProcess32NextW = (decltype(&Process32NextW))GetProcAddress(hKernel, "Process32NextW");
        if (!pProcess32NextW) {
            Wh_Log(L"GetProcAddress failed for Process32NextW; aborting.");
            return FALSE;
        }
        if (!WindhawkUtils::SetFunctionHook(pProcess32NextW, hk_Process32NextW, &orig_Process32NextW)) {
            Wh_Log(L"SetFunctionHook(Process32NextW) failed (err=%u).", GetLastError());
            return FALSE;
        }
    }

    return TRUE;
}
