// ==WindhawkMod==
// @id              google-drive-icon-refresh-fix
// @name            Fix Google Drive Desktop Icon Refresh
// @description     Prevents Google Drive for Desktop from periodically refreshing Windows desktop icons and resetting the icon cache.
// @version         1.0.0
// @author          osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @include         GoogleDriveFS.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## Motivation: Stopping the Icon Flicker
Google Drive for Desktop (`GoogleDriveFS.exe`) has a recurring quirk on Windows 11 where it periodically triggers system-wide Explorer shell refreshes every 30–40 seconds, causing desktop icons to flicker and reload—even when Desktop or User folders are not synced and Stream mode is used.

Common workarounds such as blocking `drivefsext.dll` shell extensions or locking `HKCU` registry permissions either fail to stop the refresh loop or break Drive's context menu (such as the "Available offline" toggle). This mod stops the refresh directly at the source.

## How it Works
Instead of modifying the Windows Registry or disabling shell extensions, this mod intercepts the Win32 Shell notification API that forces Explorer to redraw the desktop:

1. **Targeted Injection:** Injects exclusively into `GoogleDriveFS.exe`, leaving `explorer.exe` and all other system processes untouched.
2. **API Hooking:** Resolves `SHChangeNotify` from `shell32.dll` and installs a lightweight hook via Windhawk.
3. **Configurable Filtering:** Inspects the `wEventId` bitmask on every call against a live atomic mask controlled by the mod's settings. Enabled flags (`SHCNE_ASSOCCHANGED` and/or `SHCNE_UPDATEIMAGE`) are stripped; if no other event flags remain, the call is dropped, while all other shell notifications pass through normally.

## Key Features
- **Stops Desktop Flickering:** Eliminates the periodic 30–40 second desktop icon refresh loop.
- **Live Toggleable Filters:** Enable or disable `SHCNE_ASSOCCHANGED` and `SHCNE_UPDATEIMAGE` filtering individually from the Settings tab without restarting Google Drive.
- **Non-Destructive:** Requires no registry hacks, ACL locks, or shell extension blocking.
- **Full Drive Functionality:** Preserves Google Drive context menus ("Available offline"), virtual drive access, and sync status overlay badges.
- **Zero Explorer Overhead:** Hooks only `GoogleDriveFS.exe`, keeping `explorer.exe` clean.

## Compatibility
- Works on Windows 10 and Windows 11 (x86, x64, and ARM64).
- Tested with Google Drive for Desktop in Stream mode (with or without offline folders).

## Limitations
- **Icon Association Updates:** While `SHCNE_ASSOCCHANGED` and `SHCNE_UPDATEIMAGE` calls from `GoogleDriveFS.exe` are suppressed, icon updates for Google file types (`.gdoc`, `.gsheet`, etc.) after a major Drive update may require restarting Explorer or temporarily toggling the settings off.
- **Scope:** Only intercepts notifications originating from `GoogleDriveFS.exe`. Icon refreshes caused by other cloud clients or third-party shell extensions are not affected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- blockAssocChanged: true
  $name: Block SHCNE_ASSOCCHANGED
  $description: Suppresses file association change notifications (0x08000000), the primary trigger for full desktop icon refreshes.
- blockUpdateImage: true
  $name: Block SHCNE_UPDATEIMAGE
  $description: Suppresses system image list update notifications (0x00008000) that force Explorer to redraw icons.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>

#include <atomic>

namespace {

using SHChangeNotify_t = decltype(&SHChangeNotify);
SHChangeNotify_t SHChangeNotify_Original = nullptr;

std::atomic<LONG> g_blockedEventsMask{SHCNE_ASSOCCHANGED | SHCNE_UPDATEIMAGE};

void LoadSettings() {
    LONG mask = 0;

    if (Wh_GetIntSetting(L"blockAssocChanged")) {
        mask |= SHCNE_ASSOCCHANGED;
    }

    if (Wh_GetIntSetting(L"blockUpdateImage")) {
        mask |= SHCNE_UPDATEIMAGE;
    }

    g_blockedEventsMask.store(mask, std::memory_order_relaxed);
}

void WINAPI SHChangeNotify_Hook(LONG wEventId,
                                UINT uFlags,
                                LPCVOID dwItem1,
                                LPCVOID dwItem2) {
    LONG blockedMask = g_blockedEventsMask.load(std::memory_order_relaxed);

    if (wEventId & blockedMask) {
        LONG filteredEventId = wEventId & ~blockedMask;
        Wh_Log(L"Filtered SHChangeNotify: original=0x%08lX, filtered=0x%08lX, uFlags=0x%08X",
               wEventId, filteredEventId, uFlags);

        if (filteredEventId == 0) {
            return;
        }

        wEventId = filteredEventId;
    }

    SHChangeNotify_Original(wEventId, uFlags, dwItem1, dwItem2);
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE hShell32 =
        LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) {
        Wh_Log(L"Failed to load shell32.dll (error %lu)", GetLastError());
        return FALSE;
    }

    void* pSHChangeNotify =
        reinterpret_cast<void*>(GetProcAddress(hShell32, "SHChangeNotify"));
    if (!pSHChangeNotify) {
        Wh_Log(L"Failed to resolve SHChangeNotify (error %lu)", GetLastError());
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            pSHChangeNotify, reinterpret_cast<void*>(SHChangeNotify_Hook),
            reinterpret_cast<void**>(&SHChangeNotify_Original))) {
        Wh_Log(L"Failed to hook SHChangeNotify");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}