// ==WindhawkMod==
// @id              remove-quotes-from-ctrl-shift-c
// @name            CTRL+SHIFT+C quotes remover
// @description     Intercepts the copied file path (CTRL+SHIFT+C in Explorer) and removes surrounding quotes
// @version         1.0
// @author          draxas
// @github          https://github.com/draxas
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove quotes from CTRL+SHIFT+C file paths

When you press `CTRL+SHIFT+C` on a file in Windows Explorer, the file’s full path is usually copied to the clipboard with quotes, for example: `"C:\Path\To\File.txt"`. With this mod, those quotes are automatically removed, leaving you with a clean, unquoted path for easier use.

## How it works
- Whenever you copy a file path using `CTRL+SHIFT+C`, this mod intercepts the text before it’s placed into the clipboard.
- If the path is enclosed in quotes, they are removed.
- You end up with a path like `C:\Path\To\File.txt` instead of `"C:\Path\To\File.txt"`.

No additional configuration is needed—just use CTRL+SHIFT+C as you normally would.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
// No settings are defined for this mod.
// ==/WindhawkModSettings==

#include <windows.h>
#include <wchar.h>

typedef HANDLE (WINAPI *SetClipboardData_t)(UINT uFormat, HANDLE hMem);
SetClipboardData_t SetClipboardData_Original;

static void RemoveSurroundingQuotes(wchar_t* str) {
    size_t len = wcslen(str);
    if (len >= 2 && str[0] == L'\"' && str[len - 1] == L'\"') {
        wmemmove(str, str + 1, len - 2);
        str[len - 2] = L'\0';
    }
}

HANDLE WINAPI SetClipboardData_Hook(UINT uFormat, HANDLE hMem) {
    if (uFormat == CF_UNICODETEXT && hMem) {
        LPWSTR pData = (LPWSTR)GlobalLock(hMem);
        if (pData) {
            size_t len = wcslen(pData);
            wchar_t* buffer = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, (len + 1) * sizeof(wchar_t));
            if (buffer) {
                wcscpy_s(buffer, len + 1, pData);
                GlobalUnlock(hMem);

                RemoveSurroundingQuotes(buffer);

                size_t newLen = wcslen(buffer);
                HGLOBAL hNew = GlobalAlloc(GMEM_MOVEABLE, (newLen + 1) * sizeof(wchar_t));
                if (hNew) {
                    LPWSTR pNewData = (LPWSTR)GlobalLock(hNew);
                    if (pNewData) {
                        wcscpy_s(pNewData, newLen + 1, buffer);
                        GlobalUnlock(hNew);
                        HeapFree(GetProcessHeap(), 0, buffer);
                        return SetClipboardData_Original(uFormat, hNew);
                    }
                    GlobalFree(hNew);
                }
                HeapFree(GetProcessHeap(), 0, buffer);
            } else {
                GlobalUnlock(hMem);
            }
        }
    }

    return SetClipboardData_Original(uFormat, hMem);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init remove-quotes-from-ctrl-shift-c mod");

    Wh_SetFunctionHook((void*)SetClipboardData, (void*)SetClipboardData_Hook, (void**)&SetClipboardData_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit remove-quotes-from-ctrl-shift-c mod");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged remove-quotes-from-ctrl-shift-c mod");
}
