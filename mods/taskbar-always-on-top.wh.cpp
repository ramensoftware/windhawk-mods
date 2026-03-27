// ==WindhawkMod==
// @id              taskbar-z-order-override
// @name            Taskbar Z-Order Override
// @description     Forces the main taskbar to stay at the top or bottom of the Z-order
// @version         0.4
// @author          meteoni
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==
// ==WindhawkModSettings==
/*
- TaskbarZOrder: top
  $name: Taskbar Z-Order
  $description: Choose whether the taskbar is forced to the topmost/bottom band, or acts as a normal window 
  $options:
    - top: Always on top
    - bottom: Always at bottom
    - interactive: Mouse Interactive
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>

enum class TaskbarZOrder {
  Top,
  Bottom,
  Interactive,
};

namespace {

constexpr DWORD kInteractiveTimeoutMs = 1000;
constexpr DWORD kDesktopBand = 1;

using SetWindowPos_t =
    BOOL (WINAPI*)(HWND, HWND, int, int, int, int, UINT);

using SetWindowBand_t =
    BOOL (WINAPI*)(HWND, HWND, DWORD);

struct ModState {
  TaskbarZOrder mode = TaskbarZOrder::Interactive;

  HWND taskbarWnd = nullptr;
  WNDPROC originalTaskbarProc = nullptr;
  DWORD taskbarThreadId = 0;

  // In interactive mode we let the taskbar temporarily come forward during active
  // clicking or dragging events. Outside of that we mimic normal top-leven windows
  bool userInteractionActive = false;

  // Several helpers move the taskbar with SetWindowPos(); and generate WINDOWPOS
  // traffic too. We excludes out own ZOrder reorder attempts to prevent such fighting. 
  bool internalZOrderUpdate = false;
  
  // Timestamp of the most recent taskbar interaction. Interactive mode uses a
  // short timeout.
  DWORD lastInteractionTick = 0;
};

static ModState g_state;
static SetWindowPos_t SetWindowPos_Original = nullptr;
static SetWindowBand_t SetWindowBand_Original = nullptr;

// Automates internalZorderUpdate flagging
// Usage: IZG guard {};
// Flag reset once guard goes out of scope
struct InternalZOrderGuard {
  InternalZOrderGuard()  { g_state.internalZOrderUpdate = true; }
  ~InternalZOrderGuard() { g_state.internalZOrderUpdate = false; }
};

static bool IsMainTaskbarWindow(HWND hWnd) {
  return hWnd && hWnd == g_state.taskbarWnd;
}

static bool IsTaskbarThreadCall() {
  return GetCurrentThreadId() == g_state.taskbarThreadId;
}

static bool InsertTaskbarAfter(HWND insertAfter) {
  if (!g_state.taskbarWnd || !SetWindowPos_Original) {
    return false;
  }

  InternalZOrderGuard guard;
  return SetWindowPos_Original(
             g_state.taskbarWnd,
             insertAfter,
             0, 0, 0, 0,
             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) != FALSE;
}

// Forces the taskbar to the front of the normal (non-topmost)
// Z-order band.
static void BumpTaskbarToFrontOfNormalBand(HWND hWnd) {
  if (!hWnd || !SetWindowPos_Original) {
    return;
  }

  InternalZOrderGuard guard;

  SetWindowPos_Original(
      hWnd, HWND_TOPMOST, 0, 0, 0, 0,
      SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

  SetWindowPos_Original(
      hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
      SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

static void BeginInteraction() {
  g_state.userInteractionActive = true;
  g_state.lastInteractionTick = GetTickCount();
}

static void EndInteraction() {
  if (!g_state.userInteractionActive) {
    return;
  }

  g_state.userInteractionActive = false;
  InsertTaskbarAfter(HWND_NOTOPMOST);
}

static bool InteractionTimedOut() {
  return g_state.lastInteractionTick != 0 &&
         GetTickCount() - g_state.lastInteractionTick > kInteractiveTimeoutMs;
}

static void ApplyConfiguredMode() {
  if (!g_state.taskbarWnd) {
    return;
  }

  switch (g_state.mode) {
    case TaskbarZOrder::Top:
      InsertTaskbarAfter(HWND_TOPMOST);
      break;

    case TaskbarZOrder::Bottom:
      InsertTaskbarAfter(HWND_BOTTOM);
      break;

    case TaskbarZOrder::Interactive:
      InsertTaskbarAfter(HWND_TOP);
      break;
  }
}

static void NormalizeTaskbarBeforeUnload() {
  const auto oldMode = g_state.mode;
  g_state.mode = TaskbarZOrder::Top;
  ApplyConfiguredMode();
  g_state.mode = oldMode;
}

static void HandlePinnedTaskbarPosChanging(WINDOWPOS* wp) {
  if (!wp || (wp->flags & SWP_NOZORDER)) {
    return;
  }

  wp->hwndInsertAfter =
      (g_state.mode == TaskbarZOrder::Top) ? HWND_TOPMOST : HWND_BOTTOM;
}

static void HandleInteractiveTaskbarPosChanging(WINDOWPOS* wp) {
  if (!wp || g_state.internalZOrderUpdate || (wp->flags & SWP_NOZORDER)) {
    return;
  }

  const bool timedOut = InteractionTimedOut();

  if (g_state.userInteractionActive && !timedOut) {
    return;
  }

  wp->flags |= SWP_NOZORDER;

  if (timedOut) {
    EndInteraction();
  }
}

static void HandleInteractiveMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_ACTIVATE:
      if (LOWORD(wParam) == WA_CLICKACTIVE) {
        BeginInteraction();
      } else if (LOWORD(wParam) == WA_INACTIVE) {
        EndInteraction();
      }
      break;

    case WM_MOUSEACTIVATE:
    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
      BeginInteraction();
      break;

    case WM_CAPTURECHANGED:
      EndInteraction();
      break;

    case WM_WINDOWPOSCHANGING:
      HandleInteractiveTaskbarPosChanging(
          reinterpret_cast<WINDOWPOS*>(lParam));
      break;
  }
}

// Explorer sometimes promotes the taskbar with HWND_TOPMOST.
// In interactive mode we downgrade that to HWND_TOP -
// top but not over all regular windows.
static BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter,
                                     int X, int Y, int cx, int cy,
                                     UINT uFlags) {
  if (g_state.mode == TaskbarZOrder::Interactive &&
      IsTaskbarThreadCall() &&
      IsMainTaskbarWindow(hWnd) &&
      hWndInsertAfter == HWND_TOPMOST) {
    hWndInsertAfter = HWND_TOP;
  }

  return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

static BOOL WINAPI SetWindowBand_Hook(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand) {
  if (g_state.mode == TaskbarZOrder::Interactive &&
      IsTaskbarThreadCall() &&
      IsMainTaskbarWindow(hWnd) &&
      dwBand != kDesktopBand) {
    BumpTaskbarToFrontOfNormalBand(hWnd);
    return TRUE;
  }

  return SetWindowBand_Original(hWnd, hwndInsertAfter, dwBand);
}

static LRESULT CALLBACK TaskbarSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                            LPARAM lParam) {
  switch (g_state.mode) {
    case TaskbarZOrder::Top:
    case TaskbarZOrder::Bottom:
      if (msg == WM_WINDOWPOSCHANGING) {
        HandlePinnedTaskbarPosChanging(reinterpret_cast<WINDOWPOS*>(lParam));
      }
      break;

    case TaskbarZOrder::Interactive:
      HandleInteractiveMessage(msg, wParam, lParam);
      break;
  }

  return CallWindowProcW(g_state.originalTaskbarProc, hwnd, msg, wParam, lParam);
}

static void InstallTaskbarSubclass() {
  if (g_state.originalTaskbarProc) {
    return;
  }

  g_state.taskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
  if (!g_state.taskbarWnd) {
    Wh_Log(L"Shell_TrayWnd not found");
    return;
  }

  g_state.taskbarThreadId =
      GetWindowThreadProcessId(g_state.taskbarWnd, nullptr);

  SetLastError(0);
  auto prev = reinterpret_cast<WNDPROC>(
      SetWindowLongPtrW(g_state.taskbarWnd,
                        GWLP_WNDPROC,
                        reinterpret_cast<LONG_PTR>(TaskbarSubclassProc)));

  if (!prev && GetLastError() != 0) {
    Wh_Log(L"SetWindowLongPtrW failed: %lu", GetLastError());
    g_state.taskbarWnd = nullptr;
    g_state.taskbarThreadId = 0;
    return;
  }

  g_state.originalTaskbarProc = prev;
  ApplyConfiguredMode();
}

static void RemoveTaskbarSubclass() {
  if (g_state.taskbarWnd && g_state.originalTaskbarProc) {
    SetWindowLongPtrW(g_state.taskbarWnd,
                      GWLP_WNDPROC,
                      reinterpret_cast<LONG_PTR>(g_state.originalTaskbarProc));
  }

  g_state.originalTaskbarProc = nullptr;
  g_state.taskbarWnd = nullptr;
  g_state.taskbarThreadId = 0;
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

} 

BOOL Wh_ModInit() {
  ReadSettings();

  HMODULE user32 = GetModuleHandleW(L"user32.dll");
  if (!user32) {
    user32 = LoadLibraryW(L"user32.dll");
  }
  if (!user32) {
    return FALSE;
  }

  auto pSetWindowPos =
      reinterpret_cast<SetWindowPos_t>(GetProcAddress(user32, "SetWindowPos"));
  if (!pSetWindowPos) {
    return FALSE;
  }

  if (!Wh_SetFunctionHook(reinterpret_cast<void*>(pSetWindowPos),
                          reinterpret_cast<void*>(SetWindowPos_Hook),
                          reinterpret_cast<void**>(&SetWindowPos_Original))) {
    return FALSE;
  }

  auto pSetWindowBand =
      reinterpret_cast<SetWindowBand_t>(GetProcAddress(user32, "SetWindowBand"));
  if (!pSetWindowBand) {
    return FALSE;
  }

  if (!Wh_SetFunctionHook(reinterpret_cast<void*>(pSetWindowBand),
                          reinterpret_cast<void*>(SetWindowBand_Hook),
                          reinterpret_cast<void**>(&SetWindowBand_Original))) {
    return FALSE;
  }

  return TRUE;
}

void Wh_ModAfterInit() {
  InstallTaskbarSubclass();
}

void Wh_ModSettingsChanged() {
  ReadSettings();
  g_state.userInteractionActive = false;
  g_state.lastInteractionTick = 0;
  ApplyConfiguredMode();
}

void Wh_ModBeforeUninit() {
  NormalizeTaskbarBeforeUnload();
  RemoveTaskbarSubclass();
}
