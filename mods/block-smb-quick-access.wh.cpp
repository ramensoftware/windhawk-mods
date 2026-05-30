// ==WindhawkMod==
// @id              block-smb-quick-access
// @name            Block SMB Quick Access
// @description     Prevents SMB/UNC network paths from being added to Quick Access in Windows Explorer
// @version         1.2
// @author          Townrain
// @github          https://github.com/Townrain
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshlwapi -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Block SMB Quick Access

Prevents SMB/UNC network paths (`\\server\share`) from being added to Quick Access
in Windows Explorer. Local paths continue to work normally.

## Features

- **Selective blocking**: Only blocks SMB/UNC network paths, local paths work normally
- **Comprehensive SHARD support**: Handles all SHARD types (PATHA, PATHW, PIDL, SHELLITEM, LINK, APPIDINFO, APPIDINFOIDLIST, APPIDINFOLINK)
- **UNC priority**: Uses `SLGP_UNCPRIORITY` to correctly detect mapped drive letters
- **Silent operation**: Runs in background without UI

## How it works

The mod hooks the `SHAddToRecentDocs` function in `shell32.dll`. When Explorer calls
this function to add a file to Quick Access, the hook checks if the path is a UNC
network path (starts with `\\`). If it is, the call is silently dropped. Otherwise,
the original function is called normally.

## Requirements

- Windows 10 or Windows 11 (64-bit)
- Windhawk 1.4 or newer

## Known limitations

- Does not block paths added via other mechanisms (e.g., registry manipulation)
- Some virtual folder PIDLs may not resolve to paths and will be allowed through

## Troubleshooting

If the mod doesn't seem to work:

1. Enable debug logging in Windhawk settings (Settings > Advanced > Debug logging)
2. Check Windhawk debug logs for `[NOSMB]` entries
3. Verify the mod is enabled for explorer.exe in Windhawk settings
4. Restart Explorer or log out/in after enabling the mod

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")

// Ensure SLGP_UNCPRIORITY is defined
#ifndef SLGP_UNCPRIORITY
#define SLGP_UNCPRIORITY 0x2
#endif

// =============================================================================
// Type definitions
// =============================================================================

typedef void (WINAPI *SHAddToRecentDocs_t)(UINT uFlags, LPCVOID pv);
static SHAddToRecentDocs_t pfnOriginal = nullptr;

// =============================================================================
// UNC/SMB path detection (optimized)
// =============================================================================

static bool IsUncPathW(LPCWSTR path) {
    if (!path || path[0] != L'\\' || path[1] != L'\\') return false;
    // Exclude extended-length prefix \\?\ and device namespace \\.\
    if (path[2] == L'?' || path[2] == L'.') return false;
    return true;
}

static bool IsUncPathA(LPCSTR path) {
    if (!path || path[0] != '\\' || path[1] != '\\') return false;
    if (path[2] == '?' || path[2] == '.') return false;
    return true;
}

// =============================================================================
// SHARD type handling
// =============================================================================

static bool ShouldBlock(UINT uFlags, LPCVOID pv) {
    if (!pv) return false;

    WCHAR path[MAX_PATH] = {0};

    switch (uFlags) {
        case SHARD_PATHA: {
            LPCSTR pathA = (LPCSTR)pv;
            Wh_Log(L"[NOSMB] SHARD_PATHA: %hs", pathA);
            return IsUncPathA(pathA);
        }

        case SHARD_PATHW: {
            LPCWSTR pathW = (LPCWSTR)pv;
            Wh_Log(L"[NOSMB] SHARD_PATHW: %s", pathW);
            return IsUncPathW(pathW);
        }

        case SHARD_PIDL: {
            if (SHGetPathFromIDListW((PCIDLIST_ABSOLUTE)pv, path)) {
                Wh_Log(L"[NOSMB] SHARD_PIDL: %s", path);
                return IsUncPathW(path);
            }
            Wh_Log(L"[NOSMB] SHARD_PIDL: (unresolvable)");
            return false;
        }

        case SHARD_SHELLITEM: {
            IShellItem* item = (IShellItem*)pv;
            LPWSTR displayName = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &displayName))) {
                bool blocked = IsUncPathW(displayName);
                Wh_Log(L"[NOSMB] SHARD_SHELLITEM: %s", displayName);
                CoTaskMemFree(displayName);
                return blocked;
            }
            Wh_Log(L"[NOSMB] SHARD_SHELLITEM: (no filesystem path)");
            return false;
        }

        case SHARD_LINK: {
            IShellLinkW* link = (IShellLinkW*)pv;
            // Use SLGP_UNCPRIORITY to get UNC path instead of mapped drive letter
            if (SUCCEEDED(link->GetPath(path, MAX_PATH, nullptr, SLGP_UNCPRIORITY))) {
                Wh_Log(L"[NOSMB] SHARD_LINK: %s", path);
                return IsUncPathW(path);
            }
            Wh_Log(L"[NOSMB] SHARD_LINK: (no path)");
            return false;
        }

        case SHARD_APPIDINFO: {
            // Use official SDK structure
            const SHARDAPPIDINFO* info = (const SHARDAPPIDINFO*)pv;
            if (info && info->psi) {
                LPWSTR displayName = nullptr;
                if (SUCCEEDED(info->psi->GetDisplayName(SIGDN_FILESYSPATH, &displayName))) {
                    bool blocked = IsUncPathW(displayName);
                    Wh_Log(L"[NOSMB] SHARD_APPIDINFO: %s", displayName);
                    CoTaskMemFree(displayName);
                    return blocked;
                }
            }
            Wh_Log(L"[NOSMB] SHARD_APPIDINFO: (no filesystem path)");
            return false;
        }

        case SHARD_APPIDINFOIDLIST: {
            // Use official SDK structure
            const SHARDAPPIDINFOIDLIST* info = (const SHARDAPPIDINFOIDLIST*)pv;
            if (info && info->pidl) {
                if (SHGetPathFromIDListW(info->pidl, path)) {
                    Wh_Log(L"[NOSMB] SHARD_APPIDINFOIDLIST: %s", path);
                    return IsUncPathW(path);
                }
            }
            Wh_Log(L"[NOSMB] SHARD_APPIDINFOIDLIST: (unresolvable)");
            return false;
        }

        case SHARD_APPIDINFOLINK: {
            // Use official SDK structure
            const SHARDAPPIDINFOLINK* info = (const SHARDAPPIDINFOLINK*)pv;
            if (info && info->psl) {
                // Use SLGP_UNCPRIORITY to get UNC path instead of mapped drive letter
                if (SUCCEEDED(info->psl->GetPath(path, MAX_PATH, nullptr, SLGP_UNCPRIORITY))) {
                    Wh_Log(L"[NOSMB] SHARD_APPIDINFOLINK: %s", path);
                    return IsUncPathW(path);
                }
            }
            Wh_Log(L"[NOSMB] SHARD_APPIDINFOLINK: (no path)");
            return false;
        }

        default:
            Wh_Log(L"[NOSMB] Unknown SHARD type: %u", uFlags);
            return false;
    }
}

// =============================================================================
// Hook function
// =============================================================================

void WINAPI HookedSHAddToRecentDocs(UINT uFlags, LPCVOID pv) {
    if (ShouldBlock(uFlags, pv)) {
        Wh_Log(L"[NOSMB] Blocked SMB/UNC path from Quick Access (flags=%u)", uFlags);
        return; // Silently drop the call
    }

    // Call the original function
    if (pfnOriginal) {
        pfnOriginal(uFlags, pv);
    }
}

// =============================================================================
// Windhawk callbacks
// =============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"[NOSMB] Initializing mod...");

    // Get shell32 module handle
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"[NOSMB] ERROR: Failed to get shell32.dll handle");
        return FALSE;
    }

    // Find SHAddToRecentDocs function
    void* pTarget = (void*)GetProcAddress(hShell32, "SHAddToRecentDocs");
    if (!pTarget) {
        Wh_Log(L"[NOSMB] ERROR: Failed to find SHAddToRecentDocs in shell32.dll");
        return FALSE;
    }

    Wh_Log(L"[NOSMB] Found SHAddToRecentDocs at %p", pTarget);

    // Register the hook
    if (!Wh_SetFunctionHook(pTarget, (void*)HookedSHAddToRecentDocs, (void**)&pfnOriginal)) {
        Wh_Log(L"[NOSMB] ERROR: Failed to hook SHAddToRecentDocs");
        return FALSE;
    }

    Wh_Log(L"[NOSMB] Hook installed successfully. Original function at %p", pfnOriginal);
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"[NOSMB] Mod initialized. SMB/UNC paths will be blocked from Quick Access.");
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[NOSMB] Mod uninitializing...");
}

void Wh_ModUninit() {
    Wh_Log(L"[NOSMB] Mod uninitialized.");
}
