// ==WindhawkMod==
// @id              f1-blocker
// @name            F1 Blocker
// @description     Prevent F1 from opening the help page in File Explorer and/or other selected applications
// @version         0.0.3
// @license         MIT
// @author          d0gkiller87
// @github          https://github.com/d0gkiller87
// @include         HelpPane.exe
// @include         explorer.exe
// @include         notepad.exe
// @include         7zFM.exe
// @include         WinRAR.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## Features
Prevents F1 from opening the help page in:
- File Explorer (`explorer.exe`)
- Notepad (`notepad.exe`)
- Control (`control.exe`)
- 7-Zip (`7zFM.exe`)
- WinRAR (`WinRAR.exe`)
- Other user defined programs (`Details` → `Advanced` → `Custom process inclusion list`). ***not guarantee to work on all programs!***

## Blocking Mechanism
- Hook `TranslateAcceleratorW` and block `F1` key presses
- Block `ShellExecuteW` in `HelpPane.exe`

## Bug Report / Feature Request
Open an issue / discussion on https://github.com/ramensoftware/windhawk-mods and tag `@d0gkiller87`
*/
// ==/WindhawkModReadme==

#include <algorithm>
#include <string>

namespace Utils {
  std::wstring GetCurrentProcessNameLower() {
    wchar_t path[MAX_PATH];
    DWORD pathLength = GetModuleFileNameW( nullptr, path, MAX_PATH );
    std::wstring pathString( path, pathLength );

    // Find the file name part
    size_t lastSlashIndex = pathString.find_last_of( L"\\/" );
    std::wstring filename = ( lastSlashIndex == std::wstring::npos ) ? pathString : pathString.substr( lastSlashIndex + 1 );

    // To lowercase
    std::transform(
      filename.begin(), filename.end(),
      filename.begin(),
      []( wchar_t chr ){ return std::towlower( chr ); }
    );

    return filename;
  }
}

namespace Hooks {
  using TranslateAcceleratorW_t = decltype( &TranslateAcceleratorW );
  TranslateAcceleratorW_t TranslateAcceleratorW_Original;

  BOOL WINAPI TranslateAcceleratorW_Hook( HWND hWnd, HACCEL hAccTable, LPMSG lpMsg ) {
    if (
      lpMsg->message == WM_KEYDOWN &&
      lpMsg->wParam == VK_F1 &&
      // Don't block when pressing with combination keys
      !( GetKeyState( VK_CONTROL ) & 0x8000 ) &&
      !( GetKeyState( VK_SHIFT ) & 0x8000 ) &&
      !( GetKeyState( VK_MENU ) & 0x8000 )
    ) {
      return TRUE;
    }
    return TranslateAcceleratorW_Original( hWnd, hAccTable, lpMsg );
  }

  using ShellExecuteW_t = decltype( &ShellExecuteW );
  ShellExecuteW_t ShellExecuteW_Original;

  BOOL WINAPI ShellExecuteW_Hook( HWND hWnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd ) {
    return 33; // return > 32 if function succeeds
  }
}

BOOL Wh_ModInit() {
  std::wstring processName = Utils::GetCurrentProcessNameLower();

  if ( processName == L"helppane.exe" ) {
    // Block ShellExecuteW for HelpPane.exe
    HMODULE hShell32 = GetModuleHandle( L"shell32.dll" );
    void* ShellExecuteW = ( void* ) GetProcAddress( hShell32, "ShellExecuteW" );
    if ( ShellExecuteW == nullptr ) {
      Wh_Log( L"Failed to find shell32.ShellExecuteW: 0x%x", GetLastError() );
      return FALSE;
    }
    Wh_SetFunctionHook( ShellExecuteW, ( void* ) Hooks::ShellExecuteW_Hook, ( void** ) &Hooks::ShellExecuteW_Original );
  } else {
    // Block F1 in TranslateAcceleratorW for other apps
    HMODULE hUser32 = GetModuleHandle( L"user32.dll" );
    void* TranslateAcceleratorW = ( void* ) GetProcAddress( hUser32, "TranslateAcceleratorW" );
    if ( TranslateAcceleratorW == nullptr ) {
      Wh_Log( L"Failed to find user32.TranslateAcceleratorW: 0x%x", GetLastError() );
      return FALSE;
    }
    Wh_SetFunctionHook( TranslateAcceleratorW, ( void* ) Hooks::TranslateAcceleratorW_Hook, ( void** ) &Hooks::TranslateAcceleratorW_Original );
  }

  return TRUE;
}
