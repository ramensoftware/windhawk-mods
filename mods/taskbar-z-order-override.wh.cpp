// ==WindhawkMod==
// @id              taskbar-z-order-override
// @name            Taskbar Z-Order Override
// @description     Control whether the taskbar stays always on top, always at the bottom, or behaves like a normal window
// @version         0.5
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==
// ==WindhawkModSettings==
/*
- TaskbarZOrder: top
  $name: Taskbar Z-Order
  $description: Choose how the main taskbar should behave in the window Z-order
  $options:
    - top: Always on top
    - bottom: Always at bottom
    - interactive: Mimics normal window
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <string>

enum class TaskbarZOrder {
  Top,
  Bottom,
  Interactive,
};

namespace {

constexpr DWORD kDesktopBand = 1;

using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);

using SetWindowBand_t = BOOL(WINAPI*)(HWND, HWND, DWORD);

struct ModState {
  TaskbarZOrder mode = TaskbarZOrder::Interactive;

  HWND taskbarWnd = nullptr;
};

static ModState g_state;
static SetWindowPos_t SetWindowPos_Original = nullptr;
static SetWindowBand_t SetWindowBand_Original = nullptr;

static bool IsMainTaskbarWindow(HWND hWnd) {
  return hWnd && hWnd == g_state.taskbarWnd;
}

static bool SetTaskbarZOrder(HWND insertAfter) {
  if (!g_state.taskbarWnd || !SetWindowPos_Original) {
    return false;
  }

  return SetWindowPos_Original(g_state.taskbarWnd, insertAfter, 0, 0, 0, 0,
                               SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) !=
         FALSE;
}

// Forces the taskbar to the front of the normal (non-topmost)
// Z-order band.
static void BumpTaskbarToFrontOfNormalBand() {
  if (!g_state.taskbarWnd || !SetWindowPos_Original) {
    return;
  }

  SetWindowPos_Original(g_state.taskbarWnd, HWND_TOPMOST, 0, 0, 0, 0,
                        SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

  SetWindowPos_Original(g_state.taskbarWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                        SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}


static void ApplyConfiguredMode() {
  if (!g_state.taskbarWnd) {
    return;
  }

  switch (g_state.mode) {
    case TaskbarZOrder::Top:
      SetTaskbarZOrder(HWND_TOPMOST);
      break;

    case TaskbarZOrder::Bottom:
      SetTaskbarZOrder(HWND_BOTTOM);
      break;

    case TaskbarZOrder::Interactive:
      SetTaskbarZOrder(HWND_NOTOPMOST);
      break;
  }
}

// Before removing the subclass, temporarily force the taskbar into a stable
// top state so that it is not left between other top-level windows post-unload.
static void ForceTaskbarTopStateBeforeUnload() {
  const auto oldMode = g_state.mode;
  g_state.mode = TaskbarZOrder::Top;
  ApplyConfiguredMode();
  g_state.mode = oldMode;
}

static void HandlePinnedTaskbarPosChanging(WINDOWPOS* wp) {
  Wh_Log(L"HandlePinnedTaskbarPosChanging");
  if (!wp || (wp->flags & SWP_NOZORDER)) {
    return;
  }

  wp->hwndInsertAfter =
      (g_state.mode == TaskbarZOrder::Top) ? HWND_TOPMOST : HWND_BOTTOM;
}

// Explorer sometimes promotes the taskbar with HWND_TOPMOST.
// In interactive mode we downgrade that to HWND_TOP -
// top but not over all regular windows.
static BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X,
                                     int Y, int cx, int cy, UINT uFlags) {
  if (g_state.mode == TaskbarZOrder::Interactive &&
      IsMainTaskbarWindow(hWnd) && hWndInsertAfter == HWND_TOPMOST) {
    hWndInsertAfter = HWND_TOP;
  }

  return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// Explorer may try to move the taskbar to a different window band.
// In interactive mode, we block that change for the main taskbar and
// instead keep it at the front of the normal desktop band using the helper
// below.
//
// kDesktopBand = 1: undocumented value for normal desktop band.
static BOOL WINAPI SetWindowBand_Hook(HWND hWnd, HWND hwndInsertAfter,
                                      DWORD dwBand) {
  if (g_state.mode == TaskbarZOrder::Interactive &&
      IsMainTaskbarWindow(hWnd) && dwBand != kDesktopBand) {
    BumpTaskbarToFrontOfNormalBand();
    return TRUE;
  }

  return SetWindowBand_Original(hWnd, hwndInsertAfter, dwBand);
}

static LRESULT CALLBACK TaskbarSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                            LPARAM lParam,
                                            DWORD_PTR dwRefData) {
  switch (g_state.mode) {
    case TaskbarZOrder::Top:
    case TaskbarZOrder::Bottom:
      if (msg == WM_WINDOWPOSCHANGING) {
        HandlePinnedTaskbarPosChanging(reinterpret_cast<WINDOWPOS*>(lParam));
      }
      break;

    case TaskbarZOrder::Interactive:
      break;
  }

  return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void InstallTaskbarSubclass() {
  g_state.taskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
  if (!g_state.taskbarWnd) {
    Wh_Log(L"Shell_TrayWnd not found");
    return;
  }

  if (!WindhawkUtils::SetWindowSubclassFromAnyThread(g_state.taskbarWnd,
                                                     TaskbarSubclassProc, 0)) {
    Wh_Log(L"SetWindowSubclassFromAnyThread failed");
    g_state.taskbarWnd = nullptr;
    return;
  }
  ApplyConfiguredMode();
}

static void RemoveTaskbarSubclass() {
  if (g_state.taskbarWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        g_state.taskbarWnd,
        TaskbarSubclassProc);
  }

  g_state.taskbarWnd = nullptr;
}

static void ReadSettings() {
  PCWSTR raw = Wh_GetStringSetting(L"TaskbarZOrder");
  std::wstring value = raw ? raw : L"";
  Wh_FreeStringSetting(raw);

  if (value == L"top") {
    g_state.mode = TaskbarZOrder::Top;
  } else if (value == L"bottom") {
    g_state.mode = TaskbarZOrder::Bottom;
  } else {
    g_state.mode = TaskbarZOrder::Interactive;
  }
}

}  // namespace

BOOL Wh_ModInit() {
  ReadSettings();

  HMODULE user32 = GetModuleHandleW(L"user32.dll");
  if (!user32) {
    user32 = LoadLibraryW(L"user32.dll");
  }
  if (!user32) {
    Wh_Log(L"Failed to load user32.dll");
    return FALSE;
  }

  auto pSetWindowPos =
      reinterpret_cast<SetWindowPos_t>(GetProcAddress(user32, "SetWindowPos"));
  if (!pSetWindowPos) {
    Wh_Log(L"Failed to resolve SetWindowPos");
    return FALSE;
  }

  if (!Wh_SetFunctionHook(reinterpret_cast<void*>(pSetWindowPos),
                          reinterpret_cast<void*>(SetWindowPos_Hook),
                          reinterpret_cast<void**>(&SetWindowPos_Original))) {
    Wh_Log(L"Failed to set SetWindowPos function hook");
    return FALSE;
  }

  auto pSetWindowBand = reinterpret_cast<SetWindowBand_t>(
      GetProcAddress(user32, "SetWindowBand"));
  if (!pSetWindowBand) {
    Wh_Log(L"Failed to resolve SetWindowBand");
    return FALSE;
  }

  if (!Wh_SetFunctionHook(reinterpret_cast<void*>(pSetWindowBand),
                          reinterpret_cast<void*>(SetWindowBand_Hook),
                          reinterpret_cast<void**>(&SetWindowBand_Original))) {
    Wh_Log(L"Failed to set SetWindowBand function hook");
    return FALSE;
  }

  return TRUE;
}

void Wh_ModAfterInit() { InstallTaskbarSubclass(); }

void Wh_ModSettingsChanged() {
  ReadSettings();
  ApplyConfiguredMode();
}

void Wh_ModBeforeUninit() {
  ForceTaskbarTopStateBeforeUnload();
  RemoveTaskbarSubclass();
}
