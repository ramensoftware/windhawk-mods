// ==WindhawkMod==
// @id              dropbox-reduce-explorer-cpu-usage
// @name            Dropbox: Reduce Explorer CPU usage on sync
// @description     In Dropbox, disable SHChangeNotify calls to avoid Explorer slowdowns and high CPU usage from per-file overlay updates during large syncs.
// @version         1.1
// @author          David Trapp (CherryDT)
// @github          https://github.com/CherryDT
// @include         dropbox.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dropbox: Reduce Explorer CPU usage on sync

Dropbox can spam Explorer with `SHChangeNotify` while syncing many files, which may cause UI lag and high CPU usage by Explorer from constant overlay/icon updates.

This mod hooks `SHChangeNotify` **inside Dropbox.exe only** and turns it into a no-op.

**Trade-off:** Explorer won’t receive live overlay updates from Dropbox; press **F5** (Refresh) in a folder view to see updated sync icons.

## Notes
- Only affects Dropbox’s own calls to `SHChangeNotify`. Syncing and file I/O are unaffected, but updating the sync state indication in Explorer requires a manual refresh.
- Disable or uninstall the mod to restore normal behavior.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>

// ---------------------------------------------------------------------
// Hook: SHChangeNotify (shell32)
// ---------------------------------------------------------------------
using FnSHChangeNotify = void (WINAPI*)(LONG, UINT, LPCVOID, LPCVOID);
static FnSHChangeNotify pSHChangeNotify = nullptr;

static void WINAPI Hook_SHChangeNotify(
  LONG /*wEventId*/,
  UINT /*uFlags*/,
  LPCVOID /*dwItem1*/,
  LPCVOID /*dwItem2*/)
{
  // No-op
  return;
}

// ---------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------
BOOL Wh_ModInit() {
  WindhawkUtils::SetFunctionHook(
    SHChangeNotify,          // target function
    Hook_SHChangeNotify,     // our hook
    &pSHChangeNotify         // where to store the original (unused here)
  );

  return TRUE;
}

void Wh_ModUninit() {
  // no-op
}
