// ==WindhawkMod==
// @id              clean-desktop-shortcuts
// @name            Clean Desktop Shortcuts
// @description     Hides desktop icon labels and shortcut arrows
// @version         2.2.0
// @author          ybreakless
// @github          https://github.com/ybreakless
// @include         explorer.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lcomctl32 -luxtheme
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Clean Desktop Shortcuts

Makes the Windows desktop clean:

- **Hides the text under every desktop icon.** Files, folders, shortcuts and
  system icons such as the Recycle Bin. Only the picture is left.
- **Removes the little arrow badge** from the bottom left corner of shortcuts.
- **Optionally removes the hover / selection rectangle** that Windows draws
  behind an icon you point at or select.

Nothing is written to the registry. Nothing is changed for other programs.
Everything is put back the moment you turn the mod off. Explorer does not
need to be restarted, not when the mod loads and not when you change a
setting.

## What still works

Single click, double click, right click menu, F2 rename, drag and drop, box
selection and the arrow keys all behave normally. The icons are still there,
they simply have no text under them.

## What stops working

- **Type-ahead selection.** Pressing `d` to jump to `Documents` needs the
  label text, and the label text is what this mod takes away. Turn on
  "Show the label while the icon is selected" to get a usable middle ground.
- **Tooltips and screen readers** on the desktop see an empty name while the
  mod is on.
- If you also turn on "Remove the hover and selection box" *and* leave label
  visibility on "never", there is no visual feedback at all for which icon is
  selected. The combination is allowed, but it is not recommended.

## Options worth knowing about

- **Show the label again on hover or when selected.** The desktop stays clean
  and you can still read a name when you need one.
- **Tighter grid.** Closes the empty space where the text used to be, so the
  icons sit closer together.
- **Keep the label for these items.** A list of names you want to keep
  readable, for example `Recycle Bin`.
- **Apply the arrow removal to folder windows too.** Off by default, so
  ordinary Explorer windows are left alone.

## If something looks wrong

Turn the mod off. The labels and arrows come back within a second. If the
desktop still looks odd, press F5 on the desktop. See the rollback notes in
the repository for the manual repair steps.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideLabels: true
  $name: Hide desktop icon labels
  $description: Removes the text under every icon on the desktop.
- labelVisibility: never
  $name: Show the label anyway when...
  $description: Brings a label back for the icon you point at or select.
  $options:
  - never: Never show a label
  - onHover: While the mouse is over the icon
  - onSelect: While the icon is selected
  - onHoverOrSelect: While hovering or selected
- keepLabelsFor:
  - ""
  $name: Keep the label for these items
  $description: One name per line, for example "Recycle Bin".
- removeShortcutArrow: true
  $name: Remove the shortcut arrow overlay
  $description: Removes the arrow badge from shortcut icons.
- applyInFolderWindows: false
  $name: Also remove the arrow inside Explorer folder windows
  $description: Off by default, so folder windows keep their normal look.
- overlayFallbackHook: false
  $name: Use the drawing fallback for the arrow
  $description: Turn on only if the arrow badge is still visible.
- removeHoverSelectionBox: false
  $name: Remove the hover and selection box
  $description: Removes the rectangle drawn behind a hovered or selected icon.
- compactSpacing: false
  $name: Tighter grid
  $description: Closes the empty space the label used.
- spacingX: 0
  $name: Manual horizontal spacing (pixels)
  $description: 0 means work it out from the icon size.
- spacingY: 0
  $name: Manual vertical spacing (pixels)
  $description: 0 means work it out from the icon size.
- debugLog: false
  $name: Write a diagnostic log
  $description: Also writes %TEMP%\windhawk-clean-desktop.log. Noisy.
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windhawk_utils.h>

#include <commctrl.h>
#include <stdarg.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <uxtheme.h>
#include <windowsx.h>

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Constants that some MinGW-w64 header sets are missing or spell differently.
// ---------------------------------------------------------------------------

#ifndef LVIF_DI_SETITEM
#define LVIF_DI_SETITEM 0x1000
#endif
#ifndef LVP_LISTITEM
#define LVP_LISTITEM 1
#endif
#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif
#ifndef WM_DPICHANGED_AFTERPARENT
#define WM_DPICHANGED_AFTERPARENT 0x02E3
#endif

// DefView's "Refresh" command id. Sending it is the same as pressing F5 on the
// desktop. TODO-VERIFY: confirm on your build by sending it by hand from a test
// harness, or by watching WM_COMMAND in Spy++ while you press F5 on the
// desktop.
#define DEFVIEW_CMD_REFRESH 0x7103

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

enum LabelVis {
  kVisNever = 0,
  kVisOnHover,
  kVisOnSelect,
  kVisOnHoverOrSelect,
};

std::atomic<bool> g_hideLabels{false};
std::atomic<bool> g_removeArrow{false};
std::atomic<bool> g_applyInFolderWindows{false};
std::atomic<bool> g_overlayFallbackHook{false};
std::atomic<bool> g_removeHoverSelectionBox{false};
std::atomic<bool> g_compactSpacing{false};
std::atomic<bool> g_debugLog{false};
std::atomic<int> g_labelVisibility{kVisNever};
std::atomic<int> g_spacingX{0};
std::atomic<int> g_spacingY{0};

// Names whose label survives. Guarded by g_keepMutex; g_haveKeepList lets the
// paint path skip the lock entirely in the common case (empty list).
std::mutex g_keepMutex;
std::vector<std::wstring> g_keepLabels;
std::atomic<bool> g_haveKeepList{false};

// The overlay slot the shell uses for the shortcut arrow. -1 means "unknown",
// in which case we clear every overlay rather than guess.
int g_linkOverlayIndex = -1;

// Whether the optional hooks went in. A feature whose hook failed stays off
// even if the user turns its setting on, rather than silently doing nothing.
bool g_imageListHookInstalled = true;
bool g_themeHookInstalled = true;

// Set before we tear down, so nothing re-attaches while we are unloading.
std::atomic<bool> g_uninitializing{false};

UINT g_reapplyMsg = 0;

// Writable empty string. LVN_GETDISPINFO handlers are allowed to point pszText
// at their own buffer, so we hand out this one instead of writing into the
// shell's buffer (writing there would corrupt the item's cached name).
WCHAR g_emptyLabel[1] = L"";

// Appends a line to %TEMP%\windhawk-clean-desktop.log, but only while debugLog
// is on.
//
// The Windhawk log window is cleared when the machine restarts, which makes it
// useless for working out what the mod did during startup - exactly the thing
// we need to see. A file survives the reboot.
static void FileLog(PCWSTR format, ...) {
  if (!g_debugLog) {
    return;
  }

  WCHAR directory[MAX_PATH];
  DWORD length = GetTempPathW(MAX_PATH, directory);
  if (!length || length >= MAX_PATH) {
    return;
  }

  WCHAR path[MAX_PATH];
  _snwprintf(path, MAX_PATH, L"%swindhawk-clean-desktop.log", directory);
  path[MAX_PATH - 1] = L'\0';

  WCHAR message[512];
  va_list args;
  va_start(args, format);
  _vsnwprintf(message, ARRAYSIZE(message) - 1, format, args);
  va_end(args);
  message[ARRAYSIZE(message) - 1] = L'\0';

  SYSTEMTIME now;
  GetLocalTime(&now);

  WCHAR line[640];
  _snwprintf(line, ARRAYSIZE(line), L"%02d:%02d:%02d.%03d pid %lu  %s\r\n",
             now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
             GetCurrentProcessId(), message);
  line[ARRAYSIZE(line) - 1] = L'\0';

  char utf8[1280];
  int bytes =
      WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8, sizeof(utf8), nullptr,
                          nullptr);
  if (bytes <= 1) {
    return;
  }

  HANDLE file =
      CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                  nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return;
  }

  DWORD written = 0;
  WriteFile(file, utf8, (DWORD)(bytes - 1), &written, nullptr);
  CloseHandle(file);
}

// Set once the label handler has actually run, so the log shows whether the
// desktop ever asked us for a name.
std::atomic<bool> g_loggedFirstDispInfo{false};

#define DBG(...)                                                               \
  do {                                                                         \
    if (g_debugLog) {                                                          \
      Wh_Log(__VA_ARGS__);                                                     \
    }                                                                          \
  } while (0)

// ---------------------------------------------------------------------------
// Per-view state
// ---------------------------------------------------------------------------

struct ViewInfo {
  HWND hDefView = nullptr;
  HWND hListView = nullptr;
  bool isDesktop = false;

  // Number of subclasses still installed. The ViewInfo is only freed once
  // this hits zero AND we are sweeping or shutting down - never from inside
  // the subclass proc itself.
  //
  // RemoveWindowSubclassFromAnyThread returns void, so we cannot tell from it
  // whether a subclass was really there. These two flags are exchanged
  // instead, which guarantees exactly one decrement per installed subclass no
  // matter whether the window died on its own or we detached it.
  std::atomic<int> liveSubclasses{0};
  std::atomic<bool> defViewSubclassed{false};
  std::atomic<bool> listViewSubclassed{false};
  std::atomic<bool> dead{false};

  // Touched only on the window's own thread.
  int hotItem = -1;
  int editItem = -1;
  bool spacingApplied = false;
  bool reapplyPosted = false;
  bool mouseTracking = false;
};

std::mutex g_viewsMutex;
std::vector<ViewInfo *> g_views;

// A dead ViewInfo is reclaimed by the sweep in AttachView. That sweep can run
// on the shell thread at the same moment the settings thread is walking a copy
// of the list, so reclamation is held back while any copy is alive. Both are
// guarded by g_viewsMutex.
int g_viewsIterationDepth = 0;
std::vector<ViewInfo *> g_viewsPendingFree;

// Painting depth for the current thread. The overlay and theme hooks only act
// while one of our list views is painting, which is what keeps the rest of
// Explorer untouched.
thread_local int t_desktopPaintDepth = 0;
thread_local int t_folderPaintDepth = 0;

// Guards against re-entering the display-info handler if a state query ever
// causes another callback.
thread_local bool t_inDispInfo = false;

// ---------------------------------------------------------------------------
// Originals
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

using ImageList_DrawIndirect_t = BOOL(WINAPI *)(IMAGELISTDRAWPARAMS *);
ImageList_DrawIndirect_t ImageList_DrawIndirect_Original;

using DrawThemeBackground_t = HRESULT(WINAPI *)(HTHEME, HDC, int, int, LPCRECT,
                                                LPCRECT);
DrawThemeBackground_t DrawThemeBackground_Original;

using DrawThemeBackgroundEx_t = HRESULT(WINAPI *)(HTHEME, HDC, int, int,
                                                  LPCRECT, const DTBGOPTS *);
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Original;

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

// Case-insensitive substring search. Hand-rolled so the mod does not have to
// link shlwapi just for StrStrIW.
static bool ContainsNoCase(PCWSTR haystack, PCWSTR needle) {
  size_t needleLength = wcslen(needle);
  if (needleLength == 0) {
    return true;
  }
  for (PCWSTR at = haystack; *at; at++) {
    if (_wcsnicmp(at, needle, needleLength) == 0) {
      return true;
    }
  }
  return false;
}

static bool ClassNameIs(HWND hWnd, PCWSTR name) {
  WCHAR buf[64];
  if (!hWnd || !GetClassNameW(hWnd, buf, ARRAYSIZE(buf))) {
    return false;
  }
  return _wcsicmp(buf, name) == 0;
}

// The desktop's DefView hangs off Progman most of the time, and off a WorkerW
// after some wallpaper events. Both are treated as "the desktop".
static bool IsDesktopDefView(HWND hDefView) {
  HWND root = GetAncestor(hDefView, GA_ROOT);
  return ClassNameIs(root, L"Progman") || ClassNameIs(root, L"WorkerW");
}

// Explorer normally loads comctl32 version 6 from WinSxS, and may have the
// version 5 copy from System32 loaded as well. GetModuleHandle would give us
// whichever came first, so pick by path instead.
static HMODULE GetComctl32V6() {
  HMODULE best = nullptr;
  HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
  if (snap != INVALID_HANDLE_VALUE) {
    MODULEENTRY32W me{};
    me.dwSize = sizeof(me);
    if (Module32FirstW(snap, &me)) {
      do {
        if (_wcsicmp(me.szModule, L"comctl32.dll") != 0) {
          continue;
        }
        if (!best) {
          best = me.hModule;
        }
        if (ContainsNoCase(me.szExePath, L"Common-Controls")) {
          best = me.hModule;
          break;
        }
      } while (Module32NextW(snap, &me));
    }
    CloseHandle(snap);
  }
  return best ? best : GetModuleHandleW(L"comctl32.dll");
}

static bool ShouldKeepLabel(PCWSTR text) {
  if (!g_haveKeepList || !text || !*text) {
    return false;
  }

  std::lock_guard<std::mutex> guard(g_keepMutex);
  for (const std::wstring &name : g_keepLabels) {
    if (_wcsicmp(text, name.c_str()) == 0) {
      return true;
    }
    // Also match when the user left the extension off.
    const WCHAR *dot = wcsrchr(text, L'.');
    if (dot && dot != text) {
      size_t stem = (size_t)(dot - text);
      if (name.size() == stem && _wcsnicmp(text, name.c_str(), stem) == 0) {
        return true;
      }
    }
  }
  return false;
}

static void RedrawItem(ViewInfo *info, int index) {
  if (index >= 0 && info->hListView) {
    SendMessageW(info->hListView, LVM_REDRAWITEMS, (WPARAM)index,
                 (LPARAM)index);
  }
}

// Puts every item's text back into "ask me for it" mode.
//
// This is the whole reason the mod needed a manual off/on after a reboot. When
// DefView answers a text request it can set LVIF_DI_SETITEM, which tells the
// list view to keep the string instead of ever asking again. If the desktop was
// populated before we attached - which is exactly what happens at boot - those
// items hold real strings, our callback never fires for them, and the labels
// stay visible no matter how often we repaint.
//
// Setting the text back to LPSTR_TEXTCALLBACK restores the state DefView itself
// set up originally, so the next paint asks us again.
static void ForceTextCallback(ViewInfo *info) {
  if (!info->isDesktop || !info->hListView) {
    return;
  }

  int count = (int)SendMessageW(info->hListView, LVM_GETITEMCOUNT, 0, 0);
  for (int i = 0; i < count; i++) {
    LVITEMW item{};
    item.iSubItem = 0;
    item.pszText = LPSTR_TEXTCALLBACKW;
    SendMessageW(info->hListView, LVM_SETITEMTEXTW, (WPARAM)i, (LPARAM)&item);
  }

  DBG(L"reset %d items back to callback text", count);
}

static void RedrawAll(ViewInfo *info) {
  if (!info->hListView) {
    return;
  }
  int count = (int)SendMessageW(info->hListView, LVM_GETITEMCOUNT, 0, 0);
  if (count > 0) {
    SendMessageW(info->hListView, LVM_REDRAWITEMS, 0, (LPARAM)(count - 1));
  }
  InvalidateRect(info->hListView, nullptr, TRUE);
}

// True when the label for this item should stay readable because of the
// hover / selection setting.
static bool LabelVisibleByState(ViewInfo *info, int index) {
  int vis = g_labelVisibility;
  if (vis == kVisNever || index < 0) {
    return false;
  }

  if (vis == kVisOnHover || vis == kVisOnHoverOrSelect) {
    if (index == info->hotItem) {
      return true;
    }
  }

  if (vis == kVisOnSelect || vis == kVisOnHoverOrSelect) {
    // LVIS_SELECTED is not in the shell's callback mask, so this read can
    // not come back round into the display-info handler. The guard is
    // belt and braces in case a future build changes that.
    if (!t_inDispInfo) {
      UINT state = (UINT)SendMessageW(info->hListView, LVM_GETITEMSTATE,
                                      (WPARAM)index, LVIS_SELECTED);
      if (state & LVIS_SELECTED) {
        return true;
      }
    }
  }

  return false;
}

// ---------------------------------------------------------------------------
// The core of the mod: post-process what the shell put into the item
// ---------------------------------------------------------------------------

// Called after DefView has filled the item in. Two jobs: blank the text, and
// drop the shortcut overlay bits.
static void PostProcessDispInfo(ViewInfo *info, NMLVDISPINFOW *pdi) {
  LVITEMW &item = pdi->item;

  t_inDispInfo = true;

  // --- overlay ---------------------------------------------------------
  // Shell views call LVM_SETCALLBACKMASK with LVIS_OVERLAYMASK, which is why
  // the overlay index arrives here as callback state instead of being stored
  // in the list view. Clearing it here means the arrow is never drawn, and
  // nothing global is touched.
  //
  // TODO-VERIFY: that the desktop really delivers the overlay this way on
  // your build. Turn on debugLog and watch for the "clearing overlay" lines
  // below. If they never appear while shortcuts are on the desktop, the
  // assumption is wrong on your build - turn on overlayFallbackHook, which
  // strips the overlay at draw time instead.
  if (g_removeArrow && (info->isDesktop || g_applyInFolderWindows) &&
      (item.mask & LVIF_STATE) && (item.stateMask & LVIS_OVERLAYMASK)) {
    UINT overlay = item.state & LVIS_OVERLAYMASK;
    if (overlay) {
      bool isLink = g_linkOverlayIndex < 0 ||
                    overlay == (UINT)INDEXTOOVERLAYMASK(g_linkOverlayIndex);
      if (isLink) {
        DBG(L"clearing overlay 0x%X on item %d", overlay, item.iItem);
        item.state &= ~(UINT)LVIS_OVERLAYMASK;
      }
    }
  }

  // --- label -----------------------------------------------------------
  // TODO-VERIFY: that the desktop stores its item text as a callback
  // (LPSTR_TEXTCALLBACK) rather than as real strings. If it does not, this
  // notification never carries LVIF_TEXT, and the labels will simply stay
  // visible. Turn on debugLog and look for the "dispinfo" line below.
  // One line the first time the desktop asks us for a name. If this never shows
  // up in the log after a boot, the mod is attached but the list view is not
  // using the callback, and the label hiding cannot work by this route.
  if ((item.mask & LVIF_TEXT) && !g_loggedFirstDispInfo.exchange(true)) {
    FileLog(L"first text request from the desktop, item %d", item.iItem);
  }

  if (g_debugLog && (item.mask & LVIF_TEXT)) {
    DBG(L"dispinfo item %d mask 0x%X text '%s'", item.iItem, item.mask,
        item.pszText ? item.pszText : L"(null)");
  }
  if (info->isDesktop && g_hideLabels && (item.mask & LVIF_TEXT) &&
      item.pszText && item.pszText[0]) {
    bool keep = (item.iItem == info->editItem) ||
                ShouldKeepLabel(item.pszText) ||
                LabelVisibleByState(info, item.iItem);
    if (!keep) {
      item.pszText = g_emptyLabel;
      // Stop the list view caching our blank string as the real name.
      item.mask &= ~(UINT)LVIF_DI_SETITEM;
    }
  }

  t_inDispInfo = false;
}

// ---------------------------------------------------------------------------
// Icon spacing
// ---------------------------------------------------------------------------

// Non-zero while we are the ones setting the spacing. Deliberately a global
// rather than thread_local: the SendMessage is often issued from the settings
// thread but handled on the shell thread, and both sides need to see it.
std::atomic<int> g_applyingSpacing{0};

static void ApplySpacing(ViewInfo *info) {
  if (!info->isDesktop || !info->hListView) {
    return;
  }

  bool compact = g_compactSpacing;
  int wantX = g_spacingX;
  int wantY = g_spacingY;

  if (!compact && wantX <= 0 && wantY <= 0) {
    if (info->spacingApplied) {
      // -1 in both halves restores the system default spacing.
      g_applyingSpacing++;
      SendMessageW(info->hListView, LVM_SETICONSPACING, 0, MAKELPARAM(-1, -1));
      g_applyingSpacing--;
      SendMessageW(info->hListView, LVM_ARRANGE, LVA_DEFAULT, 0);
      info->spacingApplied = false;
      DBG(L"spacing restored to default");
    }
    return;
  }

  int iconW = 32, iconH = 32;
  HIMAGELIST himl = (HIMAGELIST)SendMessageW(info->hListView, LVM_GETIMAGELIST,
                                             LVSIL_NORMAL, 0);
  if (himl) {
    int w = 0, h = 0;
    if (ImageList_GetIconSize(himl, &w, &h) && w > 0 && h > 0) {
      iconW = w;
      iconH = h;
    }
  }

  int cx = wantX > 0 ? wantX : iconW + iconW / 3;
  int cy = wantY > 0 ? wantY : iconH + iconH / 3;

  // Never let the cell get smaller than the icon, and never let a typo turn
  // the desktop into one giant column. Written out rather than using min/max,
  // which may be macros or may be missing depending on NOMINMAX.
  if (cx > 512) {
    cx = 512;
  }
  if (cx < iconW + 4) {
    cx = iconW + 4;
  }
  if (cy > 512) {
    cy = 512;
  }
  if (cy < iconH + 4) {
    cy = iconH + 4;
  }

  g_applyingSpacing++;
  SendMessageW(info->hListView, LVM_SETICONSPACING, 0, MAKELPARAM(cx, cy));
  g_applyingSpacing--;
  SendMessageW(info->hListView, LVM_ARRANGE, LVA_DEFAULT, 0);
  info->spacingApplied = true;
  DBG(L"spacing set to %dx%d (icon %dx%d)", cx, cy, iconW, iconH);
}

// The shell resets spacing on refresh, icon size change and DPI change, so we
// re-apply through a posted message rather than fighting it inline.
static void ScheduleSpacingReapply(ViewInfo *info) {
  if (!g_reapplyMsg || info->reapplyPosted || !info->hListView) {
    return;
  }
  info->reapplyPosted = true;
  PostMessageW(info->hListView, g_reapplyMsg, 0, 0);
}

// ---------------------------------------------------------------------------
// Subclass lifetime
// ---------------------------------------------------------------------------

// Drops one installed subclass. Safe to call from either the window's own
// thread (WM_NCDESTROY) or from a detach: the exchange makes sure only the
// caller that actually flipped the flag does the decrement.
static void ReleaseSubclass(ViewInfo *info, std::atomic<bool> *flag) {
  if (flag->exchange(false)) {
    info->liveSubclasses--;
  }
}

// ---------------------------------------------------------------------------
// SHELLDLL_DefView subclass - this is where LVN_* notifications land
// ---------------------------------------------------------------------------

LRESULT CALLBACK DefViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData) {
  ViewInfo *info = (ViewInfo *)dwRefData;

  if (uMsg == WM_NCDESTROY) {
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (info) {
      info->dead = true;
      ReleaseSubclass(info, &info->defViewSubclassed);
    }
    return result;
  }

  if (!info || info->dead || uMsg != WM_NOTIFY) {
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
  }

  NMHDR *hdr = (NMHDR *)lParam;
  if (!hdr || hdr->hwndFrom != info->hListView) {
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
  }

  switch (hdr->code) {
  case LVN_GETDISPINFOW: {
    // Let the shell fill the item in first, then edit the result. This
    // is what makes "keep the label for these names" possible at all -
    // we need to see the real name before deciding.
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    PostProcessDispInfo(info, (NMLVDISPINFOW *)hdr);
    return result;
  }

  case LVN_ENDLABELEDITW:
  case LVN_ENDLABELEDITA: {
    int edited = info->editItem;
    info->editItem = -1;
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    RedrawItem(info, edited);
    return result;
  }

  case LVN_ITEMCHANGED: {
    NMLISTVIEW *nmlv = (NMLISTVIEW *)hdr;
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    int vis = g_labelVisibility;
    if (g_hideLabels && (vis == kVisOnSelect || vis == kVisOnHoverOrSelect) &&
        (nmlv->uChanged & LVIF_STATE) &&
        ((nmlv->uOldState ^ nmlv->uNewState) & LVIS_SELECTED)) {
      RedrawItem(info, nmlv->iItem);
    }
    return result;
  }

  case LVN_INSERTITEM: {
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    // A refresh (F5) rebuilds the items and drops our spacing.
    if (info->spacingApplied || g_compactSpacing || g_spacingX > 0 ||
        g_spacingY > 0) {
      ScheduleSpacingReapply(info);
    }
    return result;
  }
  }

  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// SysListView32 subclass - paint bracketing, hover tracking, spacing
// ---------------------------------------------------------------------------

static void UpdateHotItem(ViewInfo *info, LPARAM lParam) {
  LVHITTESTINFO hit{};
  hit.pt.x = GET_X_LPARAM(lParam);
  hit.pt.y = GET_Y_LPARAM(lParam);

  int index = (int)SendMessageW(info->hListView, LVM_HITTEST, 0, (LPARAM)&hit);
  if (!(hit.flags &
        (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON))) {
    index = -1;
  }

  if (index != info->hotItem) {
    int previous = info->hotItem;
    info->hotItem = index;
    RedrawItem(info, previous);
    RedrawItem(info, index);
  }
}

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam, DWORD_PTR dwRefData) {
  ViewInfo *info = (ViewInfo *)dwRefData;

  if (uMsg == WM_NCDESTROY) {
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (info) {
      info->dead = true;
      ReleaseSubclass(info, &info->listViewSubclassed);
    }
    return result;
  }

  if (!info || info->dead) {
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
  }

  if (g_reapplyMsg && uMsg == g_reapplyMsg) {
    info->reapplyPosted = false;
    ApplySpacing(info);
    return 0;
  }

  switch (uMsg) {
  case WM_PAINT:
  case WM_PRINTCLIENT:
  case WM_ERASEBKGND: {
    // Everything the list view draws for this window happens inside
    // here, including the double-buffered pass. The depth counter is
    // what tells the imagelist and theme hooks that the pixels they
    // are about to produce belong to us.
    int *depth = info->isDesktop ? &t_desktopPaintDepth : &t_folderPaintDepth;
    (*depth)++;
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    (*depth)--;
    return result;
  }

  case WM_MOUSEMOVE: {
    int vis = g_labelVisibility;
    if (g_hideLabels && (vis == kVisOnHover || vis == kVisOnHoverOrSelect)) {
      if (!info->mouseTracking) {
        TRACKMOUSEEVENT tme{};
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hWnd;
        if (TrackMouseEvent(&tme)) {
          info->mouseTracking = true;
        }
      }
      UpdateHotItem(info, lParam);
    }
    break;
  }

  case WM_MOUSELEAVE: {
    info->mouseTracking = false;
    if (info->hotItem >= 0) {
      int previous = info->hotItem;
      info->hotItem = -1;
      RedrawItem(info, previous);
    }
    break;
  }

  case LVM_EDITLABELW:
  case LVM_EDITLABELA:
    // Caught here rather than at LVN_BEGINLABELEDIT because the list
    // view pre-fills the edit box from the item text *before* it sends
    // that notification. If we were still blanking the text at that
    // point, F2 would open an empty box.
    info->editItem = (int)(INT_PTR)wParam;
    break;

  case LVM_SETIMAGELIST:
    ScheduleSpacingReapply(info);
    break;

  case LVM_SETICONSPACING:
    if (g_applyingSpacing == 0) {
      // The shell just overwrote our spacing (icon size change).
      ScheduleSpacingReapply(info);
    }
    break;

  case WM_SETTINGCHANGE:
  case WM_DISPLAYCHANGE:
  case WM_THEMECHANGED:
  case WM_DPICHANGED:
  case WM_DPICHANGED_AFTERPARENT:
    ScheduleSpacingReapply(info);
    break;
  }

  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Attach / detach
// ---------------------------------------------------------------------------

static void AttachView(HWND hDefView, HWND hListView) {
  if (g_uninitializing || !hDefView || !hListView) {
    return;
  }

  bool isDesktop = IsDesktopDefView(hDefView);
  if (!isDesktop && !g_applyInFolderWindows) {
    return;
  }

  ViewInfo *info = nullptr;
  {
    std::lock_guard<std::mutex> guard(g_viewsMutex);
    if (g_uninitializing) {
      return;
    }

    // Reclaim anything that has finished dying. A view is only safe to free
    // once its subclasses are gone (so no proc can still be running) and
    // nobody is holding a copy of the list.
    for (size_t i = g_views.size(); i-- > 0;) {
      ViewInfo *candidate = g_views[i];
      if (candidate->dead && candidate->liveSubclasses == 0) {
        g_views.erase(g_views.begin() + i);
        if (g_viewsIterationDepth == 0) {
          delete candidate;
        } else {
          g_viewsPendingFree.push_back(candidate);
        }
      } else if (!candidate->dead && candidate->hDefView == hDefView) {
        return; // already attached
      }
    }

    info = new ViewInfo();
    info->hDefView = hDefView;
    info->hListView = hListView;
    info->isDesktop = isDesktop;
    g_views.push_back(info);
  }

  // Deliberately outside the lock. These calls send a message to the window's
  // own thread; if that thread were blocked waiting for g_viewsMutex we would
  // deadlock.
  if (WindhawkUtils::SetWindowSubclassFromAnyThread(
          hDefView, DefViewSubclassProc, (DWORD_PTR)info)) {
    info->defViewSubclassed = true;
    info->liveSubclasses++;
  }
  if (WindhawkUtils::SetWindowSubclassFromAnyThread(
          hListView, ListViewSubclassProc, (DWORD_PTR)info)) {
    info->listViewSubclassed = true;
    info->liveSubclasses++;
  }

  if (info->liveSubclasses == 0) {
    info->dead = true;
    DBG(L"failed to subclass view %p", (void *)hDefView);
    return;
  }

  DBG(L"attached to %s view: defview %p, listview %p",
      isDesktop ? L"desktop" : L"folder", (void *)hDefView, (void *)hListView);

  FileLog(L"attached to %s view, defview %p", isDesktop ? L"desktop" : L"folder",
          (void *)hDefView);

  ApplySpacing(info);
  // Must come before the repaint: items that already exist are holding cached
  // strings, and a repaint alone would never ask us about them.
  ForceTextCallback(info);
  RedrawAll(info);

  if (info->isDesktop) {
    // This is the step that was missing, and the reason a manual off/on was
    // needed after every boot.
    //
    // Turning the mod off posts this same refresh, and because it is a post
    // rather than a send it lands a moment later - by which time the user has
    // already turned the mod back on. So the thing that actually fixed the
    // desktop was a refresh arriving *after* the hooks were live. Nothing did
    // that on a normal startup.
    //
    // Doing it here makes DefView rebuild every item while we are attached,
    // which is the same end state, without the user touching anything.
    PostMessageW(hDefView, WM_COMMAND, DEFVIEW_CMD_REFRESH, 0);
  }
}

static void DetachView(ViewInfo *info, bool restore) {
  info->dead = true;

  if (restore && info->isDesktop && info->spacingApplied &&
      IsWindow(info->hListView)) {
    g_applyingSpacing++;
    SendMessageW(info->hListView, LVM_SETICONSPACING, 0, MAKELPARAM(-1, -1));
    g_applyingSpacing--;
    SendMessageW(info->hListView, LVM_ARRANGE, LVA_DEFAULT, 0);
    info->spacingApplied = false;
  }

  // RemoveWindowSubclassFromAnyThread is synchronous on the owning thread, so
  // once it returns the proc is guaranteed not to be running. Calling it for a
  // window that already went away is a no-op.
  WindhawkUtils::RemoveWindowSubclassFromAnyThread(info->hDefView,
                                                   DefViewSubclassProc);
  ReleaseSubclass(info, &info->defViewSubclassed);

  WindhawkUtils::RemoveWindowSubclassFromAnyThread(info->hListView,
                                                   ListViewSubclassProc);
  ReleaseSubclass(info, &info->listViewSubclassed);

  if (restore && IsWindow(info->hListView)) {
    // Done after the subclasses are gone, so the re-query reaches an unmodified
    // DefView and the real names come straight back.
    ForceTextCallback(info);
    int count = (int)SendMessageW(info->hListView, LVM_GETITEMCOUNT, 0, 0);
    if (count > 0) {
      SendMessageW(info->hListView, LVM_REDRAWITEMS, 0, (LPARAM)(count - 1));
    }
    InvalidateRect(info->hListView, nullptr, TRUE);
    UpdateWindow(info->hListView);
  }
  if (restore && IsWindow(info->hDefView)) {
    // Force the shell to hand the overlay state back to a clean list view.
    PostMessageW(info->hDefView, WM_COMMAND, DEFVIEW_CMD_REFRESH, 0);
  }
}

static BOOL CALLBACK FindDefViewsProc(HWND hWnd, LPARAM lParam) {
  auto *found = (std::vector<HWND> *)lParam;
  if (ClassNameIs(hWnd, L"SHELLDLL_DefView")) {
    found->push_back(hWnd);
  }
  return TRUE;
}

static BOOL CALLBACK ScanTopLevelProc(HWND hWnd, LPARAM lParam) {
  DWORD pid = 0;
  GetWindowThreadProcessId(hWnd, &pid);
  if (pid != GetCurrentProcessId()) {
    return TRUE;
  }
  EnumChildWindows(hWnd, FindDefViewsProc, lParam);
  return TRUE;
}

// Finds every shell view in this process, on both the Progman and the WorkerW
// path, and attaches to the ones we care about.
static void ScanAndAttach() {
  std::vector<HWND> defViews;
  EnumWindows(ScanTopLevelProc, (LPARAM)&defViews);

  for (HWND hDefView : defViews) {
    HWND hListView =
        FindWindowExW(hDefView, nullptr, L"SysListView32", nullptr);
    if (hListView) {
      AttachView(hDefView, hListView);
    }
  }
}

// Takes a copy of the live views. Reclamation is held back until the matching
// ReleaseViewsSnapshot, so the pointers in the copy stay valid.
static std::vector<ViewInfo *> AcquireViewsSnapshot() {
  std::lock_guard<std::mutex> guard(g_viewsMutex);
  g_viewsIterationDepth++;
  return g_views;
}

static void ReleaseViewsSnapshot() {
  std::vector<ViewInfo *> toFree;
  {
    std::lock_guard<std::mutex> guard(g_viewsMutex);
    if (--g_viewsIterationDepth == 0) {
      toFree.swap(g_viewsPendingFree);
    }
  }
  for (ViewInfo *info : toFree) {
    delete info;
  }
}

// Pushes the current settings into every attached view, and drops the folder
// views if the user just turned folder support off.
static void ApplySettingsToViews() {
  std::vector<ViewInfo *> snapshot = AcquireViewsSnapshot();

  for (ViewInfo *info : snapshot) {
    if (info->dead) {
      continue;
    }
    if (!info->isDesktop && !g_applyInFolderWindows) {
      DetachView(info, true);
      continue;
    }
    ApplySpacing(info);
    ForceTextCallback(info);
    RedrawAll(info);
  }

  ReleaseViewsSnapshot();
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                 int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu,
                                 HINSTANCE hInstance, PVOID lpParam) {
  HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                       dwStyle, X, Y, nWidth, nHeight,
                                       hWndParent, hMenu, hInstance, lpParam);
  if (!hWnd || !hWndParent || g_uninitializing) {
    return hWnd;
  }

  // This runs for every window Explorer creates, so bail out cheaply.
  if (lpClassName && !IS_INTRESOURCE(lpClassName)) {
    if (_wcsicmp(lpClassName, L"SysListView32") != 0) {
      return hWnd;
    }
  } else if (!ClassNameIs(hWnd, L"SysListView32")) {
    return hWnd;
  }

  if (ClassNameIs(hWndParent, L"SHELLDLL_DefView")) {
    FileLog(L"window hook saw a shell list view, parent %p", (void *)hWndParent);
    AttachView(hWndParent, hWnd);
  }
  return hWnd;
}

// Fallback for the arrow, off by default. Only fires while one of our views is
// painting, so folder windows and the rest of Explorer are unaffected.
BOOL WINAPI ImageList_DrawIndirect_Hook(IMAGELISTDRAWPARAMS *pimldp) {
  if (pimldp && g_removeArrow && g_overlayFallbackHook &&
      (t_desktopPaintDepth > 0 ||
       (t_folderPaintDepth > 0 && g_applyInFolderWindows))) {
    UINT overlay = pimldp->fStyle & ILD_OVERLAYMASK;
    if (overlay) {
      bool isLink = g_linkOverlayIndex < 0 ||
                    overlay == (UINT)INDEXTOOVERLAYMASK(g_linkOverlayIndex);
      if (isLink) {
        pimldp->fStyle &= ~(UINT)ILD_OVERLAYMASK;
      }
    }
  }
  return ImageList_DrawIndirect_Original(pimldp);
}

static std::atomic<int> g_themeLogBudget{0};

// True when this call is the list view drawing the rounded hover / selection
// rectangle behind an item.
static bool ShouldSkipThemePart(int iPartId, int iStateId) {
  if (!g_removeHoverSelectionBox || t_desktopPaintDepth <= 0) {
    return false;
  }
  if (g_themeLogBudget > 0) {
    g_themeLogBudget--;
    Wh_Log(L"desktop DrawThemeBackground part=%d state=%d", iPartId, iStateId);
  }
  // TODO-VERIFY: LVP_LISTITEM is part 1 of the list view theme class, and the
  // hot / selected states are 1..6. Turn on debugLog and hover an icon to see
  // the part and state your build really sends before trusting this.
  return iPartId == LVP_LISTITEM && iStateId >= 1 && iStateId <= 6;
}

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME hTheme, HDC hdc, int iPartId,
                                        int iStateId, LPCRECT pRect,
                                        LPCRECT pClipRect) {
  if (ShouldSkipThemePart(iPartId, iStateId)) {
    return S_OK;
  }
  return DrawThemeBackground_Original(hTheme, hdc, iPartId, iStateId, pRect,
                                      pClipRect);
}

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME hTheme, HDC hdc, int iPartId,
                                          int iStateId, LPCRECT pRect,
                                          const DTBGOPTS *pOptions) {
  if (ShouldSkipThemePart(iPartId, iStateId)) {
    return S_OK;
  }
  return DrawThemeBackgroundEx_Original(hTheme, hdc, iPartId, iStateId, pRect,
                                        pOptions);
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

static void LoadSettings() {
  g_hideLabels = Wh_GetIntSetting(L"hideLabels") != 0;
  g_removeArrow = Wh_GetIntSetting(L"removeShortcutArrow") != 0;
  g_applyInFolderWindows = Wh_GetIntSetting(L"applyInFolderWindows") != 0;
  // A feature whose hook never went in stays off no matter what the setting
  // says, so the log explains the difference instead of it failing silently.
  g_overlayFallbackHook =
      g_imageListHookInstalled && Wh_GetIntSetting(L"overlayFallbackHook") != 0;
  g_removeHoverSelectionBox =
      g_themeHookInstalled && Wh_GetIntSetting(L"removeHoverSelectionBox") != 0;
  g_compactSpacing = Wh_GetIntSetting(L"compactSpacing") != 0;
  g_debugLog = Wh_GetIntSetting(L"debugLog") != 0;
  g_spacingX = Wh_GetIntSetting(L"spacingX");
  g_spacingY = Wh_GetIntSetting(L"spacingY");

  int visibility = kVisNever;
  PCWSTR value = Wh_GetStringSetting(L"labelVisibility");
  if (value) {
    if (wcscmp(value, L"onHover") == 0) {
      visibility = kVisOnHover;
    } else if (wcscmp(value, L"onSelect") == 0) {
      visibility = kVisOnSelect;
    } else if (wcscmp(value, L"onHoverOrSelect") == 0) {
      visibility = kVisOnHoverOrSelect;
    }
    Wh_FreeStringSetting(value);
  }
  g_labelVisibility = visibility;

  std::vector<std::wstring> keep;
  for (int i = 0; i < 256; i++) {
    PCWSTR name = Wh_GetStringSetting(L"keepLabelsFor[%d]", i);
    bool done = !name || !*name;
    if (!done) {
      keep.emplace_back(name);
    }
    if (name) {
      Wh_FreeStringSetting(name);
    }
    if (done) {
      break;
    }
  }
  {
    std::lock_guard<std::mutex> guard(g_keepMutex);
    g_haveKeepList = !keep.empty();
    g_keepLabels = std::move(keep);
  }

  g_themeLogBudget = g_debugLog ? 40 : 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
  LoadSettings();
  FileLog(L"--- mod init, hideLabels=%d removeArrow=%d",
          (int)g_hideLabels.load(), (int)g_removeArrow.load());

  g_reapplyMsg = RegisterWindowMessageW(L"WH_CleanDesktopShortcuts_Reapply");
  if (!g_reapplyMsg) {
    Wh_Log(L"RegisterWindowMessage failed, the tighter grid will not survive a "
           L"refresh");
  }

  // Ask the shell which overlay slot the shortcut arrow lives in, so we only
  // strip that one and leave the "shared", "offline" and similar badges alone.
  if (HMODULE hShell32 = GetModuleHandleW(L"shell32.dll")) {
    auto pSHGetIconOverlayIndexW = (int(WINAPI *)(LPCWSTR, int))GetProcAddress(
        hShell32, "SHGetIconOverlayIndexW");
    if (pSHGetIconOverlayIndexW) {
      int index = pSHGetIconOverlayIndexW(nullptr, IDO_SHGIOI_LINK);
      if (index > 0 && index <= 15) {
        g_linkOverlayIndex = index;
      }
    }
  }
  Wh_Log(L"shortcut overlay slot: %d%s", g_linkOverlayIndex,
         g_linkOverlayIndex < 0 ? L" (unknown - will clear every overlay)"
                                : L"");

  // The important hook: it is how we find the desktop again after a wallpaper
  // change re-creates WorkerW. If it fails the mod still loads and still works
  // on the desktop that exists right now, it just will not follow a
  // re-created one.
  bool createWindowHooked = false;
  if (HMODULE hUser32 = GetModuleHandleW(L"user32.dll")) {
    if (FARPROC target = GetProcAddress(hUser32, "CreateWindowExW")) {
      createWindowHooked =
          Wh_SetFunctionHook((void *)target, (void *)CreateWindowExW_Hook,
                             (void **)&CreateWindowExW_Original);
    }
  }
  if (!createWindowHooked) {
    Wh_Log(L"could not hook CreateWindowExW - the mod will not re-attach "
           L"after a wallpaper change or a desktop rebuild");
  }

  // Optional hooks. A failure here disables one feature and nothing else.
  //
  // Note: Windhawk does not fill the *_Original pointers until the hooks are
  // actually applied, which happens after this function returns. So success
  // is tracked with these locals - reading the Original pointer here would
  // always see null and would wrongly disable the feature.
  bool imageListHooked = false;
  if (HMODULE hComctl32 = GetComctl32V6()) {
    if (FARPROC target = GetProcAddress(hComctl32, "ImageList_DrawIndirect")) {
      imageListHooked = Wh_SetFunctionHook(
          (void *)target, (void *)ImageList_DrawIndirect_Hook,
          (void **)&ImageList_DrawIndirect_Original);
    }
  }
  if (!imageListHooked) {
    Wh_Log(L"could not hook ImageList_DrawIndirect, the arrow fallback is off");
    g_overlayFallbackHook = false;
    g_imageListHookInstalled = false;
  }

  bool themeHooked = false;
  if (HMODULE hUxTheme = GetModuleHandleW(L"uxtheme.dll")) {
    if (FARPROC target = GetProcAddress(hUxTheme, "DrawThemeBackground")) {
      themeHooked |=
          Wh_SetFunctionHook((void *)target, (void *)DrawThemeBackground_Hook,
                             (void **)&DrawThemeBackground_Original) != FALSE;
    }
    if (FARPROC target = GetProcAddress(hUxTheme, "DrawThemeBackgroundEx")) {
      themeHooked |=
          Wh_SetFunctionHook((void *)target, (void *)DrawThemeBackgroundEx_Hook,
                             (void **)&DrawThemeBackgroundEx_Original) != FALSE;
    }
  }
  if (!themeHooked) {
    Wh_Log(L"could not hook uxtheme, the hover box option is off");
    g_removeHoverSelectionBox = false;
    g_themeHookInstalled = false;
  }

  return TRUE;
}

// ---------------------------------------------------------------------------
// Startup retry
// ---------------------------------------------------------------------------

HANDLE g_retryThread = nullptr;
HANDLE g_retryStopEvent = nullptr;

static bool HaveDesktopView() {
  std::lock_guard<std::mutex> guard(g_viewsMutex);
  for (ViewInfo *info : g_views) {
    if (info->isDesktop && !info->dead) {
      return true;
    }
  }
  return false;
}

// At boot the mod can load into Explorer before the shell has built the
// desktop, so the scan in Wh_ModAfterInit finds nothing. The window creation
// hook is what normally catches it a moment later; this thread is the safety
// net for the case where that does not happen, for example when Explorer was
// already running before Windhawk injected.
//
// It gives up as soon as it has a desktop, and in any case after a minute, so
// it costs nothing once the desktop is up.
// It runs for as long as the mod is loaded rather than giving up after a while.
// When a desktop is attached this costs one cheap list check every few seconds;
// the expensive window scan only happens while we have no desktop at all. That
// also covers the desktop being destroyed and rebuilt later, for instance by a
// wallpaper change that the window creation hook did not catch.
DWORD WINAPI RetryThreadProc(LPVOID) {
  for (;;) {
    if (WaitForSingleObject(g_retryStopEvent, 3000) == WAIT_OBJECT_0) {
      return 0;
    }
    if (g_uninitializing) {
      return 0;
    }
    if (HaveDesktopView()) {
      continue;
    }
    FileLog(L"watchdog: no desktop attached, scanning");
    ScanAndAttach();
  }
}

void Wh_ModAfterInit() {
  ScanAndAttach();
  FileLog(L"after init, desktop attached: %s",
          HaveDesktopView() ? L"yes" : L"no");

  // Always started, not just when the first scan came up empty, because the
  // desktop can also go away and come back later.
  g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  if (g_retryStopEvent) {
    g_retryThread =
        CreateThread(nullptr, 0, RetryThreadProc, nullptr, 0, nullptr);
    if (!g_retryThread) {
      CloseHandle(g_retryStopEvent);
      g_retryStopEvent = nullptr;
    }
  }
}

BOOL Wh_ModSettingsChanged(BOOL *bReload) {
  *bReload = FALSE; // everything below is live
  LoadSettings();
  ScanAndAttach();        // picks up folder views if they were just enabled
  ApplySettingsToViews(); // and pushes the rest into the views we already have
  return TRUE;
}

void Wh_ModBeforeUninit() { g_uninitializing = true; }

void Wh_ModUninit() {
  g_uninitializing = true;

  // Stop the retry thread and wait for it to actually be gone before anything
  // else. It touches the view list and the mod's code, so it must not still be
  // running when this DLL is unloaded.
  if (g_retryThread) {
    if (g_retryStopEvent) {
      SetEvent(g_retryStopEvent);
    }
    WaitForSingleObject(g_retryThread, 10000);
    CloseHandle(g_retryThread);
    g_retryThread = nullptr;
  }
  if (g_retryStopEvent) {
    CloseHandle(g_retryStopEvent);
    g_retryStopEvent = nullptr;
  }

  // Turn every feature off before we redraw, so the restoring redraw below
  // produces a normal-looking desktop even if a paint is already in flight.
  g_hideLabels = false;
  g_removeArrow = false;
  g_removeHoverSelectionBox = false;
  g_overlayFallbackHook = false;
  g_compactSpacing = false;
  g_spacingX = 0;
  g_spacingY = 0;

  std::vector<ViewInfo *> snapshot;
  std::vector<ViewInfo *> pending;
  {
    std::lock_guard<std::mutex> guard(g_viewsMutex);
    snapshot.swap(g_views);
    pending.swap(g_viewsPendingFree);
  }

  // DetachView removes the subclasses synchronously on each window's own
  // thread, so once this loop is done no mod code can still be running in a
  // window procedure and the memory is safe to release.
  for (ViewInfo *info : snapshot) {
    DetachView(info, true);
  }
  for (ViewInfo *info : snapshot) {
    delete info;
  }
  for (ViewInfo *info : pending) {
    delete info;
  }
}
