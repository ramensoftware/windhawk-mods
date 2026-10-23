// ==WindhawkMod==
// @id              macos-enter-rename
// @name            Enter to Rename in File Explorer
// @description     Rename selected files and folders with Enter, like in macOS Finder.
// @version         0.4.0
// @author          emagiaco
// @github          https://github.com/emagiaco
// @include         explorer.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Enter to Rename in File Explorer

Use **Enter** to rename a selected file or folder in Windows File Explorer,
like in macOS Finder. Press **Enter** again to confirm the new name.
The standard **F2** shortcut is unchanged.

## Scope

- Works only in File Explorer windows, not on the desktop or in file dialogs,
    and only when the keyboard focus appears to be inside Explorer's shell file
    list.
- Does not intercept Enter while editing text, in the address/search controls,
  or with Ctrl, Alt, or Shift pressed.
- This mod intentionally does *not* remap Ctrl+Enter to open a file.

## Compatibility

Tested by the original user on Windows 11 for files, folders, rename confirmation,
search, and multi-selection. Other Windows versions, Explorer extensions and
alternative list controls have not yet been validated.

## Notes

The mod intercepts Enter and synthesizes an F2 key press. The context check
is based on Explorer window classes, which might differ across builds.
If the mod causes unintended behavior, disable it in Windhawk.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <cwchar>

using TranslateMessage_t = BOOL (WINAPI*)(const MSG*);
static TranslateMessage_t OriginalTranslateMessage = nullptr;
using DispatchMessageW_t = LRESULT (WINAPI*)(const MSG*);
static DispatchMessageW_t OriginalDispatchMessageW = nullptr;

// Keyboard messages are processed by the UI thread; preserve key-up handling
// even when focus moves to the rename edit box following F2.
static thread_local bool suppressedEnterDown = false;

static bool ClassIs(HWND hwnd, const wchar_t* expected) {
    wchar_t actual[128] = {};
    return hwnd && GetClassNameW(hwnd, actual, 128) &&
           _wcsicmp(actual, expected) == 0;
}

static bool InFileList() {
    HWND focus = GetFocus();
    if (!focus) return false;
    if (!ClassIs(GetAncestor(focus, GA_ROOT), L"CabinetWClass")) return false;
    bool shellView = false;
    bool fileList = false;
    for (HWND w = focus; w; w = GetParent(w)) {
        if (ClassIs(w, L"Edit") || ClassIs(w, L"RichEdit20W") ||
            ClassIs(w, L"RICHEDIT50W") || ClassIs(w, L"ComboBox") ||
            ClassIs(w, L"SysTreeView32")) return false;
        if (ClassIs(w, L"SysListView32") || ClassIs(w, L"DirectUIHWND") ||
            ClassIs(w, L"ItemsView")) fileList = true;
        if (ClassIs(w, L"SHELLDLL_DefView")) shellView = true;
    }
    return shellView && fileList;
}

static bool NoModifiers() {
    return !(GetKeyState(VK_CONTROL) & 0x8000) &&
           !(GetKeyState(VK_SHIFT) & 0x8000) &&
           !(GetKeyState(VK_MENU) & 0x8000);
}

static void PressF2() {
    INPUT input[2] = {};
    input[0].type = input[1].type = INPUT_KEYBOARD;
    input[0].ki.wVk = input[1].ki.wVk = VK_F2;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    if (SendInput(2, input, sizeof(INPUT)) != 2) {
        Wh_Log(L"F2 injection failed, GetLastError=%lu", GetLastError());
    }
}

static bool ShouldInterceptEnter(const MSG* msg) {
    return msg && msg->message == WM_KEYDOWN &&
           msg->wParam == VK_RETURN && NoModifiers() && InFileList();
}

static BOOL WINAPI TranslateMessage_Hook(const MSG* msg) {
    // Prevent a character event for an Enter press being used to rename.
    if (ShouldInterceptEnter(msg) ||
        (msg && msg->message == WM_KEYDOWN && msg->wParam == VK_RETURN &&
         suppressedEnterDown)) {
        return TRUE;
    }
    return OriginalTranslateMessage(msg);
}

static LRESULT WINAPI DispatchMessageW_Hook(const MSG* msg) {
    if (!msg || msg->wParam != VK_RETURN ||
        (msg->message != WM_KEYDOWN && msg->message != WM_KEYUP)) {
        return OriginalDispatchMessageW(msg);
    }

    if (msg->message == WM_KEYUP && suppressedEnterDown) {
        suppressedEnterDown = false;
        return 0;
    }

    if (msg->message == WM_KEYDOWN && (suppressedEnterDown ||
                                          ShouldInterceptEnter(msg))) {
        if (!suppressedEnterDown) {
            suppressedEnterDown = true;
            PressF2();
        }
        return 0;
    }

    return OriginalDispatchMessageW(msg);
}

BOOL Wh_ModInit() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return FALSE;
    auto dispatch = reinterpret_cast<void*>(
        GetProcAddress(user32, "DispatchMessageW"));
    auto translate = reinterpret_cast<void*>(
        GetProcAddress(user32, "TranslateMessage"));
    if (!dispatch || !translate) return FALSE;
    if (!Wh_SetFunctionHook(translate,
            reinterpret_cast<void*>(TranslateMessage_Hook),
            reinterpret_cast<void**>(&OriginalTranslateMessage))) return FALSE;
    return Wh_SetFunctionHook(dispatch,
        reinterpret_cast<void*>(DispatchMessageW_Hook),
        reinterpret_cast<void**>(&OriginalDispatchMessageW));
}
