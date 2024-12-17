// ==WindhawkMod==
// @id              no-hidden-cursor
// @name            No Hidden Cursor
// @description     Prevent programs (and games) from hiding your cursor
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# No Hidden Cursor
Some video games will hide your cursor in-game.
This mod will prevent games from hiding your cursor.

## How to enable the mod for a program
In Windhawk, go to the "Advanced" tab and scroll down to
"Custom process inclusion list". In that box, put the filename of the `.exe`.
The mod will immediately apply to those programs after you click "Save".

## Known Issues / Caveats
- This mod makes **no guarantees** of anti-cheat compatibility,
  and has not been tested with any games that use anti-cheat software.
*/
// ==/WindhawkModReadme==


using SetCursor_t = decltype(&SetCursor);
SetCursor_t SetCursor_Original;
HCURSOR SetCursor_Hook(HCURSOR hCursor) {
    if(hCursor == NULL) return NULL;

    return SetCursor_Original(hCursor);
}


BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    Wh_SetFunctionHook((void*)SetCursor, (void*)SetCursor_Hook, (void**)&SetCursor_Original);

    return TRUE;
}


void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
