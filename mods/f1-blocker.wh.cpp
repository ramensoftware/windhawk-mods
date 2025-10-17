// ==WindhawkMod==
// @id              f1-blocker
// @name            F1 Blocker
// @description     Prevent F1 from opening the help page in File Explorer and/or other selected applications
// @version         0.0.1
// @author          d0gkiller87
// @github          https://github.com/d0gkiller87
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*This mod prevents F1 from opening the help page in File Explorer by hooking `TranslateAcceleratorW`.
Can be applied to other programs (e.g., `notepad.exe`) by including them in `Details` -> `Advanced` -> `Custom process inclusion list`.*/
// ==/WindhawkModReadme==

using TranslateAcceleratorW_t = decltype( &TranslateAcceleratorW );
TranslateAcceleratorW_t TranslateAcceleratorW_Original;

BOOL WINAPI TranslateAcceleratorW_Hook( HWND hWnd, HACCEL hAccTable, LPMSG lpMsg ) {
  return lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F1 ?
    TRUE : TranslateAcceleratorW_Original( hWnd, hAccTable, lpMsg );
}

BOOL Wh_ModInit() {
  HMODULE hUser32 = GetModuleHandle( L"user32.dll" );

  void* TranslateAcceleratorW = ( void* ) GetProcAddress( hUser32, "TranslateAcceleratorW" );
  if ( TranslateAcceleratorW == nullptr ) {
    Wh_Log( L"Failed to find user32.TranslateAcceleratorW: 0x%x", GetLastError() );
    return FALSE;
  }

  Wh_SetFunctionHook( TranslateAcceleratorW, ( void* ) TranslateAcceleratorW_Hook, ( void** ) &TranslateAcceleratorW_Original );
  return TRUE;
}
