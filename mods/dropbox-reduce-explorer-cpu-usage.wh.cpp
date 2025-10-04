// ==WindhawkMod==
// @id           dropbox-reduce-explorer-cpu-usage
// @name         Dropbox: Reduce Explorer CPU usage on sync
// @description  In Dropbox, disable SHChangeNotify calls to avoid Explorer slowdowns and high CPU usage from per-file overlay updates during large syncs.
// @version      1.0
// @author       David Trapp (CherryDT)
// @github       https://github.com/CherryDT
// @twitter      https://x.com/CherryDT
// @include      dropbox.exe
// @architecture x86-64
// @architecture arm64
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

#if __WIN64
  #define WINAPI_STR L"__cdecl"
#else
  #define WINAPI_STR L"__stdcall"
#endif

// ---------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------
BOOL Wh_ModInit() {
  // Ensure shell32 is loaded.
  HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
  if (!hShell32) {
    hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
      return TRUE; // soft-fail
  }

  auto real = reinterpret_cast<FnSHChangeNotify>(
    GetProcAddress(hShell32, "SHChangeNotify"));
  if (!real) {
    // Some toolchains/platforms might use stdcall-decorated name on 32-bit.
#ifndef _WIN64
    real = reinterpret_cast<FnSHChangeNotify>(
      GetProcAddress(hShell32, "_SHChangeNotify@16"));
#endif
  }

  if (real) {
    // Regular Windhawk function hook; no compile-time symbol needed.
    WindhawkUtils::SetFunctionHook(
      real,                    // target function
      Hook_SHChangeNotify,     // our hook
      &pSHChangeNotify         // where to store the original (unused here)
    );
  }

  return TRUE;
}

void Wh_ModUninit() {
  // no-op
}
