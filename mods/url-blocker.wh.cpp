// ==WindhawkMod==
// @id              url-blocker
// @name            URL Blocker
// @description     Blocks selected applications from opening web URLs via ShellExecute. Useful for suppressing post-update pages, promo links, and similar browser popups
// @version         0.1
// @license         MIT
// @author          CrazyTim71
// @github          https://github.com/CrazyTim71
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## What it does

This mod prevents selected applications from opening web links in your default browser. It is mainly intended for cases where programs open unnecessary pages such as post-update "thank you" screens, release notes, promotional pages, or similar unwanted redirects.

The mod works by intercepting `ShellExecuteW` and `ShellExecuteA` calls and ignoring calls that try to open URLs (starting with `http`). Regular file paths are left alone, so opening local files will continue to work as usual.

## Configuration

This mod doesn't automatically target any processes. Add the programs for which you want to block URL calls under:

`Details` -> `Advanced` -> `Custom process inclusion list`

Only the processes in that list will have URL calls blocked.

***No guarantee that it will work on all programs!***

## Notes and limitations

- The mod only blocks URL launches that go through `ShellExecuteW` or `ShellExecuteA`.
- Programs that use another API or an embedded browser may still be able to open web pages.
- The mod is not intended for system-wide URL filtering.

## Credits

Inspired by the `F1 Blocker` mod from [d0gkiller87](https://github.com/d0gkiller87).

## Bug Report / Feature Request
Open an issue / discussion on https://github.com/ramensoftware/windhawk-mods and tag `@CrazyTim71`
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
  using ShellExecuteW_t = decltype( &ShellExecuteW );
  using ShellExecuteA_t = decltype( &ShellExecuteA );
  ShellExecuteW_t ShellExecuteW_Original;
  ShellExecuteA_t ShellExecuteA_Original;

  HINSTANCE WINAPI ShellExecuteW_Hook( HWND hWnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd ) {   
    // make sure to only block URLs and keep file links working
    if (lpFile && (_wcsnicmp(lpFile, L"http://", 7) == 0 || _wcsnicmp(lpFile, L"https://", 8) == 0)) {
        Wh_Log(L"Blocked URL: %s", lpFile);
        return ( HINSTANCE ) 33; // return > 32 if function succeeds
    }
    else {
        Wh_Log(L"Allowed URL: %s", lpFile);
        return Hooks::ShellExecuteW_Original(hWnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    }
  }

  HINSTANCE WINAPI ShellExecuteA_Hook( HWND hWnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd ) {
    // make sure to only block URLs and keep file links working
    if (lpFile && (_strnicmp(lpFile, "http://", 7) == 0 || _strnicmp(lpFile, "https://", 8) == 0)) {
        Wh_Log(L"Blocked URL: %S", lpFile);
        return ( HINSTANCE ) 33; // return > 32 if function succeeds
    }
    else {
        Wh_Log(L"Allowed URL: %S", lpFile);
        return Hooks::ShellExecuteA_Original(hWnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    }
  }
}

BOOL Wh_ModInit() {
    // Block ShellExecuteW
    HMODULE hShell32 = GetModuleHandle( L"shell32.dll" );
    void* pShellExecuteW  = ( void* ) GetProcAddress( hShell32, "ShellExecuteW" );
    if ( pShellExecuteW  == nullptr ) {
        Wh_Log( L"Failed to find shell32.ShellExecuteW: 0x%x", GetLastError() );
        return FALSE;
    }

    // Block ShellExecuteA
    void* pShellExecuteA  = ( void* ) GetProcAddress( hShell32, "ShellExecuteA" );
    if ( pShellExecuteA  == nullptr ) {
        Wh_Log( L"Failed to find shell32.ShellExecuteA: 0x%x", GetLastError() );
        return FALSE;
    }

    Wh_SetFunctionHook( pShellExecuteW , ( void* ) Hooks::ShellExecuteW_Hook, ( void** ) &Hooks::ShellExecuteW_Original );
    Wh_SetFunctionHook( pShellExecuteA , ( void* ) Hooks::ShellExecuteA_Hook, ( void** ) &Hooks::ShellExecuteA_Original );

  return TRUE;
}
