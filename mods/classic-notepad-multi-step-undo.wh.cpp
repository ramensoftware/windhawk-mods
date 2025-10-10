// ==WindhawkMod==
// @id              classic-notepad-multi-step-undo
// @name            Classic Notepad Multi-Step Undo/Redo
// @description     Adds multi-step undo/redo to classic Notepad instead of the single-step undo
// @version         1.1
// @author          David Trapp (CherryDT)
// @github          https://github.com/CherryDT
// @include         notepad.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lcomdlg32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- snapshot_interval: 2
  $name: Snapshot Interval (seconds)
  $description: Snapshot interval while typing - typing events are grouped into chunks based on this interval
- max_undo_entries: 300
  $name: Max. Undo Entries
  $description: Maximum number of undo history entries (0 for unlimited)
- max_memory_mib: 100
  $name: Max. Memory Usage (MiB)
  $description: Maximum memory usage by undo history
- min_undo_entries: 3
  $name: Min. Undo Entries (regardless of memory usage)
  $description: Minimum number of undo history entries to keep even if they would exceed the specified max. memory usage
- redo_menu_text: ""
  $name: "Redo Menu Item Text (default: automatic)"
  $description: Text of the "Redo" menu item, use "&" for access key prefix. The default is "R&edo" for English and otherwise a string taken from a shell menu which may or may not a have fitting access key. You can use this field to override it.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Classic Notepad Multi-Step Undo/Redo

This mod replaces Notepad's single-step undo (which usually restores some undesired state anyway and isn't all that useful) with a multi-step undo/redo system.

## Features:
- Groups single-character changes into undo steps if more than a configurable interval passes between them
- Records multi-character changes (like paste) immediately as separate undo steps
- Adds a "Redo" menu item with keyboard shortcut Ctrl+Y (alternatively, Ctrl+Shift+Z is also accepted)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <vector>
#include <string>
#include <windhawk_utils.h>

#define ID_FILE_NEW 1
#define ID_FILE_OPEN 2
#define ID_EDIT_UNDO 16
#define ID_EDIT_CUT 768
#define ID_EDIT_DELETE 771
#define ID_EDIT_REDO 96 // Custom

#define GROUP_TIMER 99

#define WM_UNDO 0x0304
#define MN_GETHMENU 0x01E1

// Settings
int g_snapshotIntervalSeconds;
int g_maxUndoEntries;
int g_maxMemoryMiB;
int g_minUndoEntries;
std::wstring g_redoMenuText;


// Structure to represent a snapshot of the edit control's state
struct UndoState {
  std::wstring text;    // The full text content at this state
  DWORD selStart;       // Start position of text selection
  DWORD selEnd;         // End position of text selection
};

// Global variables for managing undo/redo state
std::vector<UndoState> g_undoStack;  // Stack of previous text states for undo
std::vector<UndoState> g_redoStack;  // Stack of undone states for redo
HWND g_editHWnd = nullptr;           // Handle to the edit control
HWND g_mainHWnd = nullptr;           // Handle to the main Notepad window
bool g_pendingGroup = false;         // Flag indicating if a group of changes is pending
bool g_inUndoRedo = false;           // Flag to prevent recursive undo/redo operations
bool g_redoMenuAdded = false;        // Flag indicating if redo menu item has been added
size_t g_totalUndoSize = 0;          // Total memory size of undo stack in bytes (approx.)

void InitializeNotepadWindow(HWND hWnd);
void SubclassEditControl(HWND editHWnd);

// Clears all undo and redo history, resetting the state
void ClearUndoHistory() {
  g_undoStack.clear();
  g_redoStack.clear();
  g_totalUndoSize = 0;
  g_pendingGroup = false;
  if (g_editHWnd) KillTimer(g_editHWnd, GROUP_TIMER);
}

// Captures the current text and selection state of the edit control
UndoState GetCurrentState(HWND hWnd) {
  UndoState state;

  // Get text length
  int len = SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
  if (len > 0) {
    std::vector<WCHAR> buffer(len + 1);
    SendMessage(hWnd, WM_GETTEXT, len + 1, (LPARAM)buffer.data());
    state.text = buffer.data();
  }

  // Get selection
  SendMessage(hWnd, EM_GETSEL, (WPARAM)&state.selStart, (LPARAM)&state.selEnd);

  return state;
}

// Restores the edit control to a specific text and selection state
void SetCurrentState(HWND hWnd, const UndoState& state) {
  SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)state.text.c_str());
  SendMessage(hWnd, EM_SETSEL, state.selStart, state.selEnd);
  SendMessage(hWnd, EM_SCROLLCARET, 0, 0);
  SendMessage(hWnd, EM_SETMODIFY, TRUE, 0);
  SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), EN_UPDATE), (LPARAM)hWnd);
  SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), EN_CHANGE), (LPARAM)hWnd);
}

void UpdateUndoMenu();

// Saves the current state of the edit control to the undo stack
void SaveCurrentState() {
  if (!g_editHWnd || g_inUndoRedo) return;

  UndoState state = GetCurrentState(g_editHWnd);
  size_t newSize = state.text.size() * sizeof(wchar_t);
  g_totalUndoSize += newSize;
  g_undoStack.push_back(state);
  g_redoStack.clear();  // Clear redo on new action

  // Enforce limits: max undos or memory, whichever is lower, but no less than min undos
  while (g_maxUndoEntries > 0 && g_undoStack.size() > (size_t)g_maxUndoEntries) {
    size_t removedSize = g_undoStack.front().text.size() * sizeof(wchar_t);
    g_totalUndoSize -= removedSize;
    g_undoStack.erase(g_undoStack.begin());
    Wh_Log(L"Pruned oldest undo state due to count limit");
  }
  const size_t maxSize = (size_t)g_maxMemoryMiB * 1024LL * 1024;
  while (g_totalUndoSize > maxSize && g_undoStack.size() > (size_t)g_minUndoEntries) {
    size_t removedSize = g_undoStack.front().text.size() * sizeof(wchar_t);
    g_totalUndoSize -= removedSize;
    g_undoStack.erase(g_undoStack.begin());
    Wh_Log(L"Pruned oldest undo state due to size limit");
  }

  Wh_Log(L"State saved, undo stack size %d, total size %zu", g_undoStack.size(), g_totalUndoSize);
  UpdateUndoMenu();
}

void UpdateUndoMenu() {
  if (!g_mainHWnd) return;
  HMENU hMenu = GetMenu(g_mainHWnd);
  if (!hMenu) return;
  HMENU editMenu = GetSubMenu(hMenu, 1);
  if (!editMenu) return;
  BOOL enabled = !g_undoStack.empty();
  EnableMenuItem(hMenu, ID_EDIT_UNDO, MF_BYCOMMAND | (enabled ? MF_ENABLED : MF_GRAYED));
  enabled = !g_redoStack.empty();
  EnableMenuItem(hMenu, ID_EDIT_REDO, MF_BYCOMMAND | (enabled ? MF_ENABLED : MF_GRAYED));
  DrawMenuBar(g_mainHWnd);
}

// Hook procedure for context menu events to enable/disable undo in context menu
void CALLBACK CtxMenuHookWinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
  HMENU hMenu = (HMENU)SendMessage(hWnd, MN_GETHMENU, 0, 0);
  EnableMenuItem(hMenu, WM_UNDO, MF_BYCOMMAND | (!g_undoStack.empty() ? MF_ENABLED : MF_GRAYED));
}

// Handles text modification with selection checking - saves immediately if selection exists, otherwise groups with typing
void HandleTextModification(HWND hWnd) {
  DWORD selStart, selEnd;
  SendMessage(hWnd, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
  if (selStart != selEnd) {
    // Selection exists, save immediately (like cut/paste)
    SaveCurrentState();
  } else {
    // No selection, group with typing
    if (!g_pendingGroup) {
      SaveCurrentState();
      g_pendingGroup = true;
      SetTimer(hWnd, GROUP_TIMER, g_snapshotIntervalSeconds * 1000, nullptr);
    }
  }
}

// Subclass procedure for the edit control to intercept input and manage undo/redo
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
  switch (uMsg) {
  case WM_CHAR:
    if ((wParam >= 32) || wParam == VK_RETURN || wParam == VK_TAB) {
      // Printable character, enter, tab
      // Backspace handled in WM_KEYDOWN
      HandleTextModification(hWnd);
    }
    break;

  case WM_CUT:
  case WM_CLEAR:
  case WM_PASTE:
  case EM_REPLACESEL:
    if (g_pendingGroup) {
      KillTimer(hWnd, GROUP_TIMER);
      g_pendingGroup = false;
    }
    SaveCurrentState();
    break;

  case WM_KEYDOWN:
    if (wParam == VK_BACK || wParam == VK_DELETE) {
      HandleTextModification(hWnd);
    }
    if ((wParam == 'Y' && (GetKeyState(VK_CONTROL) & 0x8000)) ||
      (wParam == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))) {
      // Ctrl+Y or Ctrl+Shift+Z - Redo
      Wh_Log(L"Redo key pressed");
      PostMessage(g_mainHWnd, WM_COMMAND, MAKEWPARAM(ID_EDIT_REDO, 0), 0);
      return 0;  // Handled
    }
    break;

  case WM_TIMER:
    if (wParam == GROUP_TIMER) {
      KillTimer(hWnd, GROUP_TIMER);
      g_pendingGroup = false;
    }
    break;

  case WM_DESTROY:
    if (g_pendingGroup) {
      KillTimer(hWnd, GROUP_TIMER);
      g_pendingGroup = false;
    }
    break;

  case EM_CANUNDO:
    // Override to report based on our undo stack
    return !g_undoStack.empty();

  case EM_SETHANDLE:
    // Initial load or reset, clear history
    ClearUndoHistory();
    UpdateUndoMenu();
    break;

  case WM_UNDO:
    Wh_Log(L"Edit control undo received");
    // Forward to main window for our undo logic
    PostMessage(g_mainHWnd, WM_COMMAND, MAKEWPARAM(ID_EDIT_UNDO, 0), 0);
    return 0;

  case WM_CONTEXTMENU:
    {
      HWINEVENTHOOK hEventHook = SetWinEventHook(EVENT_SYSTEM_MENUPOPUPSTART, EVENT_SYSTEM_MENUPOPUPSTART, 0, CtxMenuHookWinEventProc, GetCurrentProcessId(), GetCurrentThreadId(), 0);
      LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);
      UnhookWinEvent(hEventHook);
      return ret;
    }
    break;
  }

  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Subclass procedure for the main Notepad window to handle commands and initialization
LRESULT CALLBACK MainWindowSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
  switch (uMsg) {
  case WM_PARENTNOTIFY:
    if (LOWORD(wParam) == WM_CREATE) {
      HWND child = (HWND)lParam;
      WCHAR className[100];
      if (GetClassNameW(child, className, 100) && wcscmp(className, L"Edit") == 0) {
        SubclassEditControl(child);
      }
    }
    break;

  case WM_COMMAND:
    if (LOWORD(wParam) == ID_EDIT_UNDO) {
      Wh_Log(L"Undo command received, stack size %d", g_undoStack.size());
      if (g_editHWnd && !g_undoStack.empty()) {
        UndoState current = GetCurrentState(g_editHWnd);
        g_redoStack.push_back(current);

        UndoState prev = g_undoStack.back();
        g_undoStack.pop_back();
        g_totalUndoSize -= prev.text.size() * sizeof(wchar_t);
        g_inUndoRedo = true;
        SetCurrentState(g_editHWnd, prev);
        g_inUndoRedo = false;
        Wh_Log(L"Undo performed, undo stack size %d, redo stack size %d", g_undoStack.size(), g_redoStack.size());
        UpdateUndoMenu();
      }
      return 0;
    } else if (LOWORD(wParam) == ID_EDIT_CUT) {
      SaveCurrentState();
    } else if (LOWORD(wParam) == ID_EDIT_DELETE) {
      SaveCurrentState();
    } else if (LOWORD(wParam) == ID_EDIT_REDO) {
      if (g_editHWnd && !g_redoStack.empty()) {
        // Save current state to undo
        UndoState current = GetCurrentState(g_editHWnd);
        g_undoStack.push_back(current);
        g_totalUndoSize += current.text.size() * sizeof(wchar_t);

        // Restore next state
        UndoState next = g_redoStack.back();
        g_redoStack.pop_back();
        g_inUndoRedo = true;
        SetCurrentState(g_editHWnd, next);
        g_inUndoRedo = false;
        Wh_Log(L"Redo performed, undo stack size %d, redo stack size %d", g_undoStack.size(), g_redoStack.size());
        UpdateUndoMenu();
      }
      return 0;
    }
    break;

  case WM_DESTROY:
    ClearUndoHistory();
    if (g_editHWnd) {
      WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_editHWnd, EditSubclassProc);
      g_editHWnd = nullptr;
    }
    g_mainHWnd = nullptr;
    break;
  }

  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Original CreateWindowExW function pointer for hooking
decltype(CreateWindowExW)* CreateWindowExW_Original;

// Hooked CreateWindowExW to detect and initialize Notepad windows
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
  HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

  if (hWnd && lpClassName && (ULONG_PTR)lpClassName >= 0x10000 && wcscmp(lpClassName, L"Notepad") == 0) {
    InitializeNotepadWindow(hWnd);
  }

  return hWnd;
}

// Initializes a Notepad window by subclassing it and adding the redo menu item
void InitializeNotepadWindow(HWND hWnd) {
  g_mainHWnd = hWnd;
  WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, MainWindowSubclassProc, 0);
  // Add redo menu item
  HMENU hMenu = GetMenu(hWnd);
  if (hMenu) {
    HMENU hEditMenu = GetSubMenu(hMenu, 1);
    if (hEditMenu && !g_redoMenuAdded) {
      std::wstring redoText = L"R&edo\tCtrl+Y"; // Avoid access key conflict between &Redo and &Replace
      if (!g_redoMenuText.empty()) {
        // Get undo menu text to extract hotkey
        std::wstring hotkey = L"Ctrl+Y";
        WCHAR undoText[256];
        if (GetMenuStringW(hEditMenu, ID_EDIT_UNDO, undoText, 256, MF_BYCOMMAND)) {
          std::wstring undoStr = undoText;
          size_t tabPos = undoStr.find(L'\t');
          if (tabPos != std::wstring::npos) {
            std::wstring afterTab = undoStr.substr(tabPos + 1);
            if (afterTab.size() >= 2 && afterTab.substr(afterTab.size() - 2) == L"+Z") {
              hotkey = afterTab.substr(0, afterTab.size() - 1) + L"Y";
            }
          }
        }

        redoText = g_redoMenuText + L"\t" + hotkey;
      } else {
        LANGID langid = GetUserDefaultUILanguage();
        if (PRIMARYLANGID(langid) != LANG_ENGLISH) {
          // Try to get a localized string from shell32.dll, disregarding access key conflicts
          HMODULE hShell32 = GetModuleHandle(L"shell32");
          if (hShell32) {
            WCHAR buffer[256] = {0};
            if (LoadStringW(hShell32, 4170, buffer, 256)) {
              redoText = buffer;
            }
          }
        }
      }
      InsertMenu(hEditMenu, 1, MF_BYPOSITION | MF_STRING, ID_EDIT_REDO, redoText.c_str());
      g_redoMenuAdded = true;
    }
  }
  Wh_Log(L"Main window subclassed");

  // Find and subclass edit control
  EnumChildWindows(hWnd, [](HWND child, LPARAM) -> BOOL {
    WCHAR className[100];
    if (GetClassNameW(child, className, 100) && wcscmp(className, L"Edit") == 0) {
      SubclassEditControl(child);
      return FALSE; // Stop enumeration
    }
    return TRUE;
  }, 0);
}

// Subclasses the edit control to enable input and context menu interception
void SubclassEditControl(HWND editHWnd) {
  g_editHWnd = editHWnd;
  WindhawkUtils::SetWindowSubclassFromAnyThread(editHWnd, EditSubclassProc, 0);
  Wh_Log(L"Edit control found and subclassed");
  UpdateUndoMenu();
}

// Module initialization function - sets up hooks and attaches to existing Notepad windows
BOOL Wh_ModInit() {
  Wh_Log(L"Mod initialized");
  // Check if this is the immersive Notepad (packaged app) vs classic Notepad
  WCHAR modulePath[MAX_PATH];
  if (GetModuleFileNameW(NULL, modulePath, MAX_PATH) > 0) {
    if (wcsstr(modulePath, L"WindowsApps") != NULL) {
      // Immersive Notepad (packaged app) - abort
      return FALSE;
    }
  }

  // Load settings
  g_snapshotIntervalSeconds = Wh_GetIntSetting(L"snapshot_interval");
  g_maxUndoEntries = Wh_GetIntSetting(L"max_undo_entries");
  g_maxMemoryMiB = Wh_GetIntSetting(L"max_memory_mib");
  g_minUndoEntries = Wh_GetIntSetting(L"min_undo_entries");
  g_redoMenuText = WindhawkUtils::StringSetting::make(L"redo_menu_text");

  WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExWHook, &CreateWindowExW_Original);
  Wh_Log(L"Hook set");

  // Attach to existing Notepad windows
  EnumWindows([](HWND hWnd, LPARAM) -> BOOL {
    WCHAR className[256];
    if (GetClassNameW(hWnd, className, 256) && wcscmp(className, L"Notepad") == 0) {
      // Check PID
      DWORD pid = 0;
      GetWindowThreadProcessId(hWnd, &pid);
      if (pid != GetCurrentProcessId()) return TRUE; // Not our process

      // Found Notepad window
      InitializeNotepadWindow(hWnd);
      return FALSE; // Stop enumeration
    }
    return TRUE;
  }, 0);

  return TRUE;
}

// Module cleanup function - removes hooks and subclasses and cleans up resources
void Wh_ModUninit() {
  // Cleanup data structures
  ClearUndoHistory();
  // Remove redo menu item
  if (g_redoMenuAdded && g_mainHWnd) {
    HMENU hMenu = GetMenu(g_mainHWnd);
    if (hMenu) {
      HMENU hEditMenu = GetSubMenu(hMenu, 1);
      if (hEditMenu) {
        DeleteMenu(hEditMenu, ID_EDIT_REDO, MF_BYCOMMAND);
      }
    }
  }
  // Cleanup subclasses
  if (g_mainHWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_mainHWnd, MainWindowSubclassProc);
    g_mainHWnd = nullptr;
  }
  if (g_editHWnd) {
    if (g_pendingGroup) {
      KillTimer(g_editHWnd, GROUP_TIMER);
      g_pendingGroup = false;
    }
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_editHWnd, EditSubclassProc);
    g_editHWnd = nullptr;
  }
}

// Called when mod settings are changed - we just want to reload in that case
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
  if (bReload) *bReload = TRUE;
  return TRUE;
}
