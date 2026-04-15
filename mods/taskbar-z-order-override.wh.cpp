// ==WindhawkMod==
// @id              taskbar-z-order-override
// @name            Taskbar Z-Order Override
// @description     Control whether the taskbar stays always on top, always at the bottom, or behaves like a normal window
// @version         0.8
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Taskbar Z-Order Override

Control whether the Windows taskbar stays always on top, always at the bottom, or behaves like a normal window.

## Working modes

### Behaves like a normal window

Makes the taskbar behave like a regular window - like how it was possible pre-Vista. 

![Behaves like a normal window](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/taskbar-z-order-override/interactive.gif)

### Always on top

Keeps the taskbar above normal windows. 
Great for games or browser fullscreen. 

This is mainly useful for people who use **auto-hide** and want the taskbar to stay accessible consistently.

![Always on top](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/taskbar-z-order-override/always_on_top_low.gif)

### Always at bottom

Keeps the taskbar at the bottom of the Z-order.

If you are more of a window enjoyer than a taskbar enjoyer, this is for you. 

![Always at bottom](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/taskbar-z-order-override/always_bottom_low.gif)

## Notes

- Supports the main taskbar and secondary taskbars on multi-monitor setups.
- Intended for Explorer's standard taskbar windows.
- UWP fullscreen (entered with Shift + Win + Enter) expects Windows to temporarily promote the taskbar. Since Normal window mode & Always at bottom mode are designed to block those changes, the taskbar might not remain visible. If you don't actively use UWP fullscreen, this should not be a concern. 

## Credits

- Normal window mode original concept from **7+ Taskbar Tweaker** by m417z
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- TaskbarZOrder: top
  $name: Taskbar Z-Order
  $description: Choose how the main taskbar should behave in the window Z-order
  $options:
    - top: Always on top
    - bottom: Always at bottom
    - interactive: Behaves like a normal window
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <windhawk_utils.h>
#include <windows.h>
#include <vector>
#include <commctrl.h>
#include <string>

enum class TaskbarZOrder {
  Top,
  Bottom,
  Interactive,
};

namespace {

using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);

using SetWindowBand_t = BOOL(WINAPI*)(HWND, HWND, DWORD);

struct ModState {
  TaskbarZOrder mode = TaskbarZOrder::Interactive;

  HWND primaryTaskbarWnd = nullptr;
  std::vector<HWND> secondaryTaskbarWnds;

};

static ModState g_state;
static SetWindowPos_t SetWindowPos_Original = nullptr;
static SetWindowBand_t SetWindowBand_Original = nullptr;

static bool IsPrimaryTaskbarWindow(HWND hWnd) {
  return hWnd && hWnd == g_state.primaryTaskbarWnd;
}

static bool IsSecondaryTaskbarWindow(HWND hWnd) {
  for (HWND hwnd : g_state.secondaryTaskbarWnds) {
    if (hwnd == hWnd) {
      return true;
    }
  }
  return false;
}

static bool IsTrackedTaskbarWindow(HWND hWnd) {
  return IsPrimaryTaskbarWindow(hWnd) || IsSecondaryTaskbarWindow(hWnd);
}

static void SetTaskbarZOrder(HWND hwnd, HWND insertAfter) {
  if (!hwnd || !SetWindowPos_Original) {
    Wh_Log(L"SetTaskbarZOrder skipped: hwnd=%p SetWindowPos_Original=%p",
           hwnd, SetWindowPos_Original);
    return;
  }

  if (!SetWindowPos_Original(hwnd, insertAfter, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)) {
    Wh_Log(L"SetTaskbarZOrder failed: hwnd=%p insertAfter=%p error=%lu",
           hwnd, insertAfter, GetLastError());
  }
}

static void ApplyConfiguredMode() {
  auto applyOne = [](HWND hwnd, TaskbarZOrder mode) {
    switch (mode) {
      case TaskbarZOrder::Top:
        SetTaskbarZOrder(hwnd, HWND_TOPMOST);
        break;

      case TaskbarZOrder::Bottom:
        SetTaskbarZOrder(hwnd, HWND_BOTTOM);
        break;

      case TaskbarZOrder::Interactive:
        SetTaskbarZOrder(hwnd, HWND_NOTOPMOST);
        break;
    }
  };

  applyOne(g_state.primaryTaskbarWnd, g_state.mode);

  for (HWND hwnd : g_state.secondaryTaskbarWnds) {
    applyOne(hwnd, g_state.mode);
  }
}

static void ForceTaskbarTopStateBeforeUnload() {
  SetTaskbarZOrder(g_state.primaryTaskbarWnd, HWND_TOPMOST);

  for (HWND hwnd : g_state.secondaryTaskbarWnds) {
    SetTaskbarZOrder(hwnd, HWND_TOPMOST);
  }
}

static void HandlePinnedTaskbarPosChanging(WINDOWPOS* wp) {
  if (!wp || (wp->flags & SWP_NOZORDER)) {
    return;
  }

  wp->hwndInsertAfter =
      (g_state.mode == TaskbarZOrder::Top) ? HWND_TOPMOST : HWND_BOTTOM;
}

// Explorer sometimes promotes the taskbar with HWND_TOPMOST.
// In interactive mode we downgrade that to HWND_TOP. 
// Still top, but not above normal windows. 
static BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X,
                                     int Y, int cx, int cy, UINT uFlags) {
  if (g_state.mode == TaskbarZOrder::Interactive &&
      IsTrackedTaskbarWindow(hWnd) && 
      hWndInsertAfter == HWND_TOPMOST) {
    hWndInsertAfter = HWND_TOP;
  }

  return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

/*
Explorer may try to move the taskbar to a different window band.
For example, opening the start menu / full-screen UWP apps
moves taskbar to band 6. 

Each call bumps the taskbar to the top of the specified band even 
if the band number is identical. 

In top mode, we allow band promotion so that UWP full screen
behavior still works.

In bottom & interactive modes: we block that change so Windows
doesn't override our policy. 
*/
static BOOL WINAPI SetWindowBand_Hook(HWND hWnd,
                                      HWND hwndInsertAfter,
                                      DWORD dwBand) {
  if (IsTrackedTaskbarWindow(hWnd)) {
    //Wh_Log(L"Attempted Taskbar band change -> %lu", dwBand);

    if (g_state.mode == TaskbarZOrder::Top) {
      /* Allow Top mode to cooperate with UWP 
      fullscreen. Not so easy for other modes */
      return SetWindowBand_Original(hWnd, hwndInsertAfter, dwBand);
    }

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

  // handle destroyed taskbars
  if (msg == WM_NCDESTROY){
    if (hwnd == g_state.primaryTaskbarWnd) {
      g_state.primaryTaskbarWnd = nullptr;
    } else {
      g_state.secondaryTaskbarWnds.erase(
          std::remove(g_state.secondaryTaskbarWnds.begin(),
                      g_state.secondaryTaskbarWnds.end(),
                      hwnd),
          g_state.secondaryTaskbarWnds.end());
    }
  }

  return DefSubclassProc(hwnd, msg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

static bool IsCurrentProcessTaskbarClass(HWND hwnd, const wchar_t* wantedClass) {
  if (!hwnd) {
    return false;
  }

  DWORD pid = 0;
  if (!GetWindowThreadProcessId(hwnd, &pid) || pid != GetCurrentProcessId()) {
    return false;
  }

  wchar_t className[32] = {};
  if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
    return false;
  }

  return _wcsicmp(className, wantedClass) == 0;
}

static void TryAttachPrimaryTaskbarWindow(HWND hwnd) {
  if (!IsCurrentProcessTaskbarClass(hwnd, L"Shell_TrayWnd")) {
    return;
  }

  if (g_state.primaryTaskbarWnd == hwnd) {
    return;
  }

  if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
          hwnd, TaskbarSubclassProc, 0)) {
    Wh_Log(L"Failed to subclass primary taskbar hwnd=%p", hwnd);
    return;
  }

  g_state.primaryTaskbarWnd = hwnd;
  Wh_Log(L"Attached subclass to primary taskbar hwnd=%p", hwnd);
  ApplyConfiguredMode();
}

static void TryAttachSecondaryTaskbarWindow(HWND hwnd) {
  if (!IsCurrentProcessTaskbarClass(hwnd, L"Shell_SecondaryTrayWnd")) {
    return;
  }

  if (IsSecondaryTaskbarWindow(hwnd)) {
    return;
  }

  if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
          hwnd, TaskbarSubclassProc, 0)) {
    Wh_Log(L"Failed to subclass secondary taskbar hwnd=%p", hwnd);
    return;
  }

  g_state.secondaryTaskbarWnds.push_back(hwnd);
  Wh_Log(L"Attached subclass to secondary taskbar hwnd=%p", hwnd);
  ApplyConfiguredMode();
}

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                        LPCWSTR lpClassName,
                                        LPCWSTR lpWindowName,
                                        DWORD dwStyle,
                                        int X,
                                        int Y,
                                        int nWidth,
                                        int nHeight,
                                        HWND hWndParent,
                                        HMENU hMenu,
                                        HINSTANCE hInstance,
                                        LPVOID lpParam) {
  HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                       dwStyle, X, Y, nWidth, nHeight,
                                       hWndParent, hMenu, hInstance, lpParam);

  if (!hwnd || !lpClassName) {
    return hwnd;
  }

  const bool textualClassName =
      ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

  if (textualClassName) {
    if (_wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
      Wh_Log(L"Shell_TrayWnd created: hwnd=%p", hwnd);
      TryAttachPrimaryTaskbarWindow(hwnd);
    } else if (_wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
      Wh_Log(L"Shell_SecondaryTrayWnd created: hwnd=%p", hwnd);
      TryAttachSecondaryTaskbarWindow(hwnd);
    }
  }

  return hwnd;
}

static void FindAndSubclassExistingTaskbars() {
  EnumWindows(
      [](HWND hwnd, LPARAM) -> BOOL {
        TryAttachPrimaryTaskbarWindow(hwnd);
        TryAttachSecondaryTaskbarWindow(hwnd);
        return TRUE;
      },
      0);

  if (!g_state.primaryTaskbarWnd) {
    Wh_Log(L"No primary taskbar in this explorer.exe instance yet");
  }
}

static void RemoveTaskbarSubclasses() {
  if (g_state.primaryTaskbarWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        g_state.primaryTaskbarWnd,
        TaskbarSubclassProc);
  }

  for (HWND hwnd : g_state.secondaryTaskbarWnds) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        hwnd,
        TaskbarSubclassProc);
  }

  g_state.primaryTaskbarWnd = nullptr;
  g_state.secondaryTaskbarWnds.clear();
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

  if (!Wh_SetFunctionHook(reinterpret_cast<void*>(CreateWindowExW),
                          reinterpret_cast<void*>(CreateWindowExW_Hook),
                          reinterpret_cast<void**>(&CreateWindowExW_Original))) {
    Wh_Log(L"Failed to set CreateWindowExW function hook");
    return FALSE;
  }

  return TRUE;
}

void Wh_ModAfterInit() { FindAndSubclassExistingTaskbars(); }

void Wh_ModSettingsChanged() {
  ReadSettings();
  ApplyConfiguredMode();
}

void Wh_ModBeforeUninit() {
  ForceTaskbarTopStateBeforeUnload();
  RemoveTaskbarSubclasses();
}
