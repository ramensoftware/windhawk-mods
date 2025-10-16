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
/*...*/
// ==/WindhawkModReadme==

BOOL( *TranslateAcceleratorW_Original )( HWND hWnd, HACCEL hAccTable, LPMSG lpMsg );
BOOL TranslateAcceleratorW_Hook( HWND hWnd, HACCEL hAccTable, LPMSG lpMsg ) {
  return lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F1 ?
    TRUE : TranslateAcceleratorW_Original( hWnd, hAccTable, lpMsg );
}

BOOL Wh_ModInit() {
  HMODULE hUser32 = GetModuleHandle( L"user32.dll" );

  void* TranslateAcceleratorW = ( void* ) GetProcAddress( hUser32, "TranslateAcceleratorW" );
  Wh_SetFunctionHook( TranslateAcceleratorW, ( void* ) TranslateAcceleratorW_Hook, ( void** ) &TranslateAcceleratorW_Original );

  return TRUE;
}
