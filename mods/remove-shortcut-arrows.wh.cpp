// ==WindhawkMod==
// @id              remove-shortcut-arrows
// @name            Remove Shortcut Arrow Overlays
// @description     Removes shortcut arrow overlays from desktop and File Explorer icons cleanly in memory.
// @version         1.0.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Shortcut Arrow Overlays

Removes the shortcut arrow overlay from desktop and File Explorer icons cleanly
in memory without modifying the Windows Registry.

### Why this is better than the Registry tweak:
* **In-Place Overlay Replacement**: Modifies Explorer's system image lists
in-place with zero image slot leaks.
* **No Black Squares**: The common registry tweak (`Shell Icons\29` pointing to
a blank `.ico`) frequently causes visual bugs such as black squares on icons
after icon cache updates or on high-DPI displays.
* **No Registry Tampering**: Does not alter system registry keys and does not
require copying files into `C:\Windows\System32`.
* **Zero Dependencies**: Generates a 100% transparent icon dynamically in
memory. No external `.ico` files are needed.
* **Instant Reversibility**: Automatically restores the original Windows
shortcut arrows when the mod is disabled or unloaded without an Explorer restart.
* **Optional Custom Icon**: Allows using a custom badge or icon instead of a
completely transparent one.

### Scope & Process Behavior
* System image lists are per-process, so this mod targets `explorer.exe` (Desktop
and File Explorer). Shell Open/Save dialogs hosted inside other application
processes retain their separate process image lists.

### Credits
* Credit to the **Hide Desktop Icon Text and Shortcut Arrows** mod
(`hide-desktop-icon-text`) by kivsak for the transparent GDI icon generation and
shell image list technique.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- custom_icon_path: ""
  $name: Custom Overlay Icon Path (Optional)
  $description: Leave empty for fully transparent arrows. To use a custom badge, specify the path to a .ico file with artwork in the bottom-left corner and the rest transparent.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <commoncontrols.h>
#include <shlobj.h>
#include <windows.h>

#include <vector>

// ---------------------------------------------------------------------------
// Types & State
// ---------------------------------------------------------------------------

struct OverlayBackup {
  int shil;
  int imageIndex;
  HICON originalIcon;
};

static const GUID g_iidImageList = {
    0x46eb5926,
    0x582e,
    0x4017,
    {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

static std::vector<OverlayBackup> g_backupOverlays;
static int g_overlayImageIndex = -1;
static WindhawkUtils::StringSetting g_customIconPath;

// ---------------------------------------------------------------------------
// Transparent Icon Synthesis (In-Memory, 32-bit ARGB)
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
  PCWSTR customPath = g_customIconPath.get();
  if (customPath && *customPath) {
    HICON hLoaded = (HICON)LoadImageW(nullptr, customPath, IMAGE_ICON, cx, cy,
                                      LR_LOADFROMFILE);
    if (hLoaded) {
      return hLoaded;
    }
  }
  return CreateTransparentIcon(cx, cy);
}

// ---------------------------------------------------------------------------
// Overlay Management via Shell Image List (In-Place Replacement)
// ---------------------------------------------------------------------------

static void HideShortcutOverlay() {
  int linkOverlay = SHGetIconOverlayIndexW(nullptr, IDO_SHGIOI_LINK);
  if (linkOverlay <= 0)
    return;

  const int sizes[] = {SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL,
                       SHIL_JUMBO};
  g_backupOverlays.clear();
  g_overlayImageIndex = -1;

  for (int shil : sizes) {
    IImageList *piml = nullptr;
    if (FAILED(SHGetImageList(shil, g_iidImageList, (void **)&piml)) || !piml) {
      continue;
    }

    int imageIndex = -1, cx = 0, cy = 0;
    HICON original = nullptr;
    if (SUCCEEDED(piml->GetOverlayImage(linkOverlay, &imageIndex)) &&
        imageIndex >= 0 && SUCCEEDED(piml->GetIconSize(&cx, &cy)) &&
        SUCCEEDED(piml->GetIcon(imageIndex, ILD_TRANSPARENT, &original))) {
      g_overlayImageIndex = imageIndex;
      if (HICON replacement = GetReplacementIcon(cx, cy)) {
        int replaced = -1;
        if (SUCCEEDED(piml->ReplaceIcon(imageIndex, replacement, &replaced))) {
          g_backupOverlays.push_back({shil, imageIndex, original});
          original = nullptr;
        }
        DestroyIcon(replacement);
      }
      if (original) {
        DestroyIcon(original);
      }
    }
    piml->Release();
  }
}

static void RestoreShortcutOverlay() {
  // Restore in reverse order so lists returned for multiple SHIL sizes restore
  // properly
  for (auto it = g_backupOverlays.rbegin(); it != g_backupOverlays.rend(); ++it) {
    IImageList *piml = nullptr;
    if (SUCCEEDED(SHGetImageList(it->shil, g_iidImageList, (void **)&piml)) &&
        piml) {
      int replaced = -1;
      piml->ReplaceIcon(it->imageIndex, it->originalIcon, &replaced);
      piml->Release();
    }
    DestroyIcon(it->originalIcon);
  }
  g_backupOverlays.clear();
}

// ---------------------------------------------------------------------------
// Desktop Redraw Helpers
// ---------------------------------------------------------------------------

static void RefreshDesktop() {
  HWND hShell = GetShellWindow();
  if (!hShell)
    return;

  DWORD pid = 0;
  GetWindowThreadProcessId(hShell, &pid);
  if (pid != GetCurrentProcessId())
    return;

  HWND hDefView = FindWindowEx(hShell, nullptr, L"SHELLDLL_DefView", nullptr);
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
    HWND hListView =
        FindWindowEx(hDefView, nullptr, L"SysListView32", L"FolderView");
    if (hListView) {
      InvalidateRect(hListView, nullptr, TRUE);
    }
  }
}

static void RefreshDesktopAndExplorer() {
  if (g_overlayImageIndex >= 0) {
    SHChangeNotify(
        SHCNE_UPDATEIMAGE, SHCNF_DWORD | SHCNF_FLUSH, nullptr,
        reinterpret_cast<LPCVOID>(
            static_cast<ULONG_PTR>(g_overlayImageIndex)));
  }
  RefreshDesktop();
}

// ---------------------------------------------------------------------------
// Settings & Lifecycle
// ---------------------------------------------------------------------------

static void LoadSettings() {
  g_customIconPath = WindhawkUtils::StringSetting::make(L"custom_icon_path");
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
