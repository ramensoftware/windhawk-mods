// ==WindhawkMod==
// @id              remove-shortcut-arrows
// @name            Remove Shortcut Arrow Overlays
// @description     Removes shortcut arrow overlays from desktop and File
// Explorer icons cleanly in memory without touching the Windows registry.
// @version         1.0.0
// @author          Community
// @github          https://github.com/ramensoftware/windhawk-mods
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Shortcut Arrow Overlays

Removes the shortcut arrow overlay from desktop and File Explorer icons cleanly
in memory.

### Why this is better than the Registry tweak:
* **No Black Squares**: The common registry tweak (`Shell Icons\29` pointing to
a blank `.ico`) frequently causes visual bugs such as black squares on icons
after icon cache updates or on high-DPI displays.
* **No Registry Tampering**: Does not alter system registry keys and does not
require copying files into `C:\Windows\System32`.
* **Zero Dependencies**: Generates a 100% transparent icon dynamically in
memory. No external `.ico` files are needed.
* **Instant Reversibility**: Automatically restores the original Windows
shortcut arrows when the mod is disabled or unloaded.
* **Optional Custom Icon**: If you want to use a custom badge or icon instead of
a completely transparent one, you can specify its path in the mod settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- custom_icon_path: ""
  $name: Custom Overlay Icon Path (Optional)
  $description: Leave this empty to use the built-in transparent icon
(recommended). If you want a custom overlay, specify the full path to a valid
.ico file (e.g. C:\icons\my_custom.ico).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <commoncontrols.h>
#include <objbase.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>

#include <string>
#include <vector>

#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif

#ifndef SHIL_LARGE
#define SHIL_LARGE 0
#endif
#ifndef SHIL_SMALL
#define SHIL_SMALL 1
#endif
#ifndef SHIL_EXTRALARGE
#define SHIL_EXTRALARGE 2
#endif
#ifndef SHIL_SYSSMALL
#define SHIL_SYSSMALL 3
#endif
#ifndef SHIL_JUMBO
#define SHIL_JUMBO 4
#endif

struct OverlayBackup {
  int shil;
  int originalIndex;
};

static std::vector<OverlayBackup> g_backupOverlays;
static std::wstring g_customIconPath;

// ---------------------------------------------------------------------------
// Transparent Icon Synthesis (In-Memory, Zero-Weight, 100% Clean)
// ---------------------------------------------------------------------------

static HICON CreateTransparentIcon(int cx, int cy) {
  if (cx <= 0 || cy <= 0)
    return nullptr;

  BITMAPV5HEADER bi = {};
  bi.bV5Size = sizeof(bi);
  bi.bV5Width = cx;
  bi.bV5Height = cy;
  bi.bV5Planes = 1;
  bi.bV5BitCount = 32;
  bi.bV5Compression = BI_BITFIELDS;
  bi.bV5RedMask = 0x00FF0000;
  bi.bV5GreenMask = 0x0000FF00;
  bi.bV5BlueMask = 0x000000FF;
  bi.bV5AlphaMask = 0xFF000000;

  HDC hdc = GetDC(nullptr);
  void *bits = nullptr;
  HBITMAP hColor = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
                                    &bits, nullptr, 0);
  ReleaseDC(nullptr, hdc);
  if (!hColor)
    return nullptr;

  if (bits) {
    memset(bits, 0, (size_t)cx * cy * 4);
  }

  // 1-bit monochrome AND mask: set all bits to 1 (transparent background for
  // Win32 icons)
  int rowBytes = ((cx + 15) / 16) * 2;
  std::vector<BYTE> maskBits((size_t)rowBytes * cy, 0xFF);
  HBITMAP hMask = CreateBitmap(cx, cy, 1, 1, maskBits.data());

  ICONINFO ii = {};
  ii.fIcon = TRUE;
  ii.hbmColor = hColor;
  ii.hbmMask = hMask;
  HICON hIcon = CreateIconIndirect(&ii);

  DeleteObject(hColor);
  if (hMask)
    DeleteObject(hMask);
  return hIcon;
}

static HICON GetReplacementIcon(int cx, int cy) {
  if (!g_customIconPath.empty()) {
    HICON hLoaded = (HICON)LoadImageW(nullptr, g_customIconPath.c_str(),
                                      IMAGE_ICON, cx, cy, LR_LOADFROMFILE);
    if (hLoaded) {
      return hLoaded;
    }
  }
  return CreateTransparentIcon(cx, cy);
}

// ---------------------------------------------------------------------------
// Overlay Management via Shell Image List
// ---------------------------------------------------------------------------

static void HideShortcutOverlay() {
  HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
  if (!hShell32)
    return;

  auto pSHGetImageList =
      (HRESULT(WINAPI *)(int, REFIID, void **))GetProcAddress(hShell32,
                                                              "SHGetImageList");
  auto pSHGetIconOverlayIndex = (int(WINAPI *)(LPCWSTR, int))GetProcAddress(
      hShell32, "SHGetIconOverlayIndexW");
  if (!pSHGetImageList || !pSHGetIconOverlayIndex)
    return;

  int linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
  if (linkOverlay <= 0)
    return;

  static const GUID iidImageList = {
      0x46eb5926,
      0x582e,
      0x4017,
      {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

  const int sizes[] = {SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL,
                       SHIL_JUMBO};
  g_backupOverlays.clear();

  for (int shil : sizes) {
    IImageList *piml = nullptr;
    if (FAILED(pSHGetImageList(shil, iidImageList, (void **)&piml)) || !piml) {
      continue;
    }

    // Back up original overlay index for clean restoration
    int origIndex = -1;
    if (SUCCEEDED(piml->GetOverlayImage(linkOverlay, &origIndex))) {
      g_backupOverlays.push_back({shil, origIndex});
    }

    int cx = 0, cy = 0;
    if (SUCCEEDED(piml->GetIconSize(&cx, &cy)) && cx > 0 && cy > 0) {
      if (HICON hIcon = GetReplacementIcon(cx, cy)) {
        int idx = -1;
        if (SUCCEEDED(piml->ReplaceIcon(-1, hIcon, &idx)) && idx >= 0) {
          piml->SetOverlayImage(idx, linkOverlay);
        }
        DestroyIcon(hIcon);
      }
    }
    piml->Release();
  }
}

static void RestoreShortcutOverlay() {
  HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
  if (!hShell32)
    return;

  auto pSHGetImageList =
      (HRESULT(WINAPI *)(int, REFIID, void **))GetProcAddress(hShell32,
                                                              "SHGetImageList");
  auto pSHGetIconOverlayIndex = (int(WINAPI *)(LPCWSTR, int))GetProcAddress(
      hShell32, "SHGetIconOverlayIndexW");
  if (!pSHGetImageList || !pSHGetIconOverlayIndex)
    return;

  int linkOverlay = pSHGetIconOverlayIndex(nullptr, IDO_SHGIOI_LINK);
  if (linkOverlay <= 0)
    return;

  static const GUID iidImageList = {
      0x46eb5926,
      0x582e,
      0x4017,
      {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

  for (const auto &backup : g_backupOverlays) {
    IImageList *piml = nullptr;
    if (SUCCEEDED(pSHGetImageList(backup.shil, iidImageList, (void **)&piml)) &&
        piml) {
      piml->SetOverlayImage(backup.originalIndex, linkOverlay);
      piml->Release();
    }
  }
  g_backupOverlays.clear();
}

// ---------------------------------------------------------------------------
// Desktop & Shell Redraw Helpers
// ---------------------------------------------------------------------------

static HWND GetDesktopListView() {
  HWND hProgman = FindWindow(L"Progman", L"Program Manager");
  HWND hDefView = FindWindowEx(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
  if (!hDefView) {
    HWND hWorkerW = nullptr;
    while ((hWorkerW = FindWindowEx(nullptr, hWorkerW, L"WorkerW", nullptr)) !=
           nullptr) {
      hDefView = FindWindowEx(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
      if (hDefView)
        break;
    }
  }
  if (hDefView) {
    return FindWindowEx(hDefView, nullptr, L"SysListView32", L"FolderView");
  }
  return nullptr;
}

static void RefreshDesktopAndExplorer() {
  SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
  if (HWND hDesktop = GetDesktopListView()) {
    int count = ListView_GetItemCount(hDesktop);
    if (count > 0) {
      ListView_RedrawItems(hDesktop, 0, count - 1);
    }
    InvalidateRect(hDesktop, nullptr, TRUE);
    UpdateWindow(hDesktop);
  }
}

// ---------------------------------------------------------------------------
// Settings & Lifecycle
// ---------------------------------------------------------------------------

static void LoadSettings() {
  PCWSTR customPath = Wh_GetStringSetting(L"custom_icon_path");
  g_customIconPath = customPath ? customPath : L"";
  Wh_FreeStringSetting(customPath);
}

BOOL Wh_ModInit() {
  Wh_Log(L"Initializing Remove Shortcut Arrows mod");
  LoadSettings();
  HideShortcutOverlay();
  return TRUE;
}

void Wh_ModAfterInit() { RefreshDesktopAndExplorer(); }

void Wh_ModUninit() {
  Wh_Log(L"Uninitializing Remove Shortcut Arrows mod");
  RestoreShortcutOverlay();
  RefreshDesktopAndExplorer();
}

BOOL Wh_ModSettingsChanged(BOOL *bReload) {
  *bReload = TRUE;
  return TRUE;
}
