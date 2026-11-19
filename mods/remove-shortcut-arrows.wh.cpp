// ==WindhawkMod==
// @id              remove-shortcut-arrows
// @name            Remove Shortcut Arrow Icons
// @description     Removes shortcut arrow icons from desktop and File Explorer cleanly in memory.
// @version         2.0.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         explorer.exe
// @compilerOptions -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Shortcut Arrow Icons

Removes shortcut arrow icons from desktop and File Explorer cleanly in memory without modifying the Windows Registry.

### Before and After

| Default Windows (With Arrows) | Clean Desktop (With Mod) |
| :---: | :---: |
| ![Before](https://raw.githubusercontent.com/Dev-StudioCode/Hide-Desktop-Shortcut-Icon/main/assets/before.png) | ![After](https://raw.githubusercontent.com/Dev-StudioCode/Hide-Desktop-Shortcut-Icon/main/assets/after.png) |

### Features
* **Zero Registry Modifications**: Operates purely in-memory, leaving your registry keys untouched.
* **No Black Squares or Artifacts**: Immune to the black box glitches caused by legacy Shell Icons registry hacks.
* **Instant Toggle**: Applies immediately on load and restores original shortcut arrows when unloaded.
* **Custom Badge Support**: Optional setting to supply your own custom icon badge instead of a transparent icon.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- custom_icon_path: ""
  $name: Custom Shortcut Icon Path (Optional)
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

static bool HideShortcutOverlay() {
  int linkOverlay = SHGetIconOverlayIndexW(nullptr, IDO_SHGIOI_LINK);
  if (linkOverlay <= 0)
    return false;

  const int sizes[] = {SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL,
                       SHIL_JUMBO};
  bool anyReplaced = false;

  for (int shil : sizes) {
    IImageList *piml = nullptr;
    if (FAILED(SHGetImageList(shil, g_iidImageList, (void **)&piml)) || !piml) {
      continue;
    }

    int imageIndex = -1, cx = 0, cy = 0;
    HICON original = nullptr;
    if (SUCCEEDED(piml->GetOverlayImage(linkOverlay, &imageIndex)) &&
        imageIndex >= 0 && SUCCEEDED(piml->GetIconSize(&cx, &cy))) {
      g_overlayImageIndex = imageIndex;

      // Check if this SHIL size was already backed up
      bool alreadyBackedUp = false;
      for (const auto &b : g_backupOverlays) {
        if (b.shil == shil) {
          alreadyBackedUp = true;
          break;
        }
      }

      if (!alreadyBackedUp) {
        if (SUCCEEDED(piml->GetIcon(imageIndex, ILD_TRANSPARENT, &original))) {
          if (HICON replacement = GetReplacementIcon(cx, cy)) {
            int replaced = -1;
            if (SUCCEEDED(piml->ReplaceIcon(imageIndex, replacement, &replaced))) {
              g_backupOverlays.push_back({shil, imageIndex, original});
              original = nullptr;
              anyReplaced = true;
            }
            DestroyIcon(replacement);
          }
          if (original) {
            DestroyIcon(original);
          }
        }
      } else {
        // Re-apply replacement in case image list was swapped by Explorer
        if (HICON replacement = GetReplacementIcon(cx, cy)) {
          int replaced = -1;
          if (SUCCEEDED(piml->ReplaceIcon(imageIndex, replacement, &replaced))) {
            anyReplaced = true;
          }
          DestroyIcon(replacement);
        }
      }
    }
    piml->Release();
  }

  return anyReplaced;
}

static void RestoreShortcutOverlay() {
  // Restore in reverse order so lists returned for multiple SHIL sizes restore
  // properly
  for (auto it = g_backupOverlays.rbegin(); it != g_backupOverlays.rend();
       ++it) {
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
// Desktop Redraw & Hook Helpers
// ---------------------------------------------------------------------------

static HWND GetDesktopListView() {
  HWND hShell = GetShellWindow();
  if (!hShell) {
    hShell = FindWindowW(L"Progman", L"Program Manager");
  }
  if (!hShell)
    return nullptr;

  HWND hDefView = FindWindowExW(hShell, nullptr, L"SHELLDLL_DefView", nullptr);
  if (!hDefView) {
    HWND hWorkerW = nullptr;
    while ((hWorkerW = FindWindowExW(nullptr, hWorkerW, L"WorkerW", nullptr)) !=
           nullptr) {
      hDefView = FindWindowExW(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
      if (hDefView)
        break;
    }
  }
  if (hDefView) {
    HWND hListView =
        FindWindowExW(hDefView, nullptr, L"SysListView32", L"FolderView");
    DWORD pid = 0;
    if (hListView && GetWindowThreadProcessId(hListView, &pid) &&
        pid == GetCurrentProcessId()) {
      return hListView;
    }
  }
  return nullptr;
}

static void RefreshDesktop() {
  if (HWND hListView = GetDesktopListView()) {
    InvalidateRect(hListView, nullptr, TRUE);
  }
}

static void RefreshDesktopAndExplorer() {
  if (g_overlayImageIndex >= 0) {
    SHChangeNotify(
        SHCNE_UPDATEIMAGE, SHCNF_DWORD | SHCNF_FLUSH, nullptr,
        reinterpret_cast<LPCVOID>(static_cast<ULONG_PTR>(g_overlayImageIndex)));
  }
  RefreshDesktop();
}

static bool EnsureShortcutOverlayHidden() {
  if (HideShortcutOverlay()) {
    RefreshDesktopAndExplorer();
    return true;
  }
  return false;
}

static bool IsDesktopFolderView(HWND hWnd) {
  WCHAR buf[32];
  if (!GetClassNameW(hWnd, buf, ARRAYSIZE(buf)) || wcscmp(buf, L"SysListView32"))
    return false;
  HWND hDefView = GetParent(hWnd);
  if (!hDefView || !GetClassNameW(hDefView, buf, ARRAYSIZE(buf)) ||
      wcscmp(buf, L"SHELLDLL_DefView"))
    return false;
  HWND hTop = GetParent(hDefView);
  if (!hTop || !GetClassNameW(hTop, buf, ARRAYSIZE(buf)) ||
      (wcscmp(buf, L"Progman") && wcscmp(buf, L"WorkerW")))
    return false;
  DWORD pid = 0;
  return GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId();
}

static LRESULT CALLBACK DesktopSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    DWORD_PTR dwRefData) {
  switch (uMsg) {
    case WM_SETTINGCHANGE:
    case WM_THEMECHANGED:
      EnsureShortcutOverlayHidden();
      break;
  }
  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;
static HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam) {
  HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                       dwStyle, X, Y, nWidth, nHeight,
                                       hWndParent, hMenu, hInstance, lpParam);
  if (hWnd && IsDesktopFolderView(hWnd)) {
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopSubclassProc, 0);
    EnsureShortcutOverlayHidden();
  }
  return hWnd;
}

// ---------------------------------------------------------------------------
// Settings & Lifecycle
// ---------------------------------------------------------------------------

static void LoadSettings() {
  g_customIconPath = WindhawkUtils::StringSetting::make(L"custom_icon_path");
}

BOOL Wh_ModInit() {
  Wh_Log(L"Initializing Remove Shortcut Arrow Icons mod");
  LoadSettings();

  WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                 &CreateWindowExW_Original);

  EnsureShortcutOverlayHidden();
  return TRUE;
}

void Wh_ModAfterInit() {
  if (HWND hDesktop = GetDesktopListView()) {
    WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, DesktopSubclassProc, 0);
  }
}

void Wh_ModUninit() {
  Wh_Log(L"Uninitializing Remove Shortcut Arrow Icons mod");

  if (HWND hDesktop = GetDesktopListView()) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, DesktopSubclassProc);
  }

  RestoreShortcutOverlay();
  RefreshDesktopAndExplorer();
  g_overlayImageIndex = -1;
}

BOOL Wh_ModSettingsChanged(BOOL *bReload) {
  *bReload = TRUE;
  return TRUE;
}
