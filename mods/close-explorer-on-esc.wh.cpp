// ==WindhawkMod==
// @id              close-explorer-on-esc
// @name            Close Explorer on Esc
// @description     Press Esc in File Explorer to close the window; ignores rename and input fields.
// @version         1.2
// @author          lieyanbang
// @github          https://github.com/lieyanbang
// @homepage        https://lieyanbang.com/
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Close Explorer on Esc

**ENGLISH**

Press **Esc** in File Explorer to close the current window.
The mod avoids accidental closure when:
- You are renaming a file/folder
- The foreground window is not the current Explorer

Optional:
- *Allow closing when IME candidate window exists* (for certain input methods that create temporary Edit focus)

**How it works**
A low-level keyboard hook detects `VK_ESCAPE` within explorer.exe.
If the focused control is safe (not text editing, not renaming), it sends `WM_CLOSE` to the current Explorer window.

---

**中文说明**

在资源管理器中按下 **Esc** 可关闭当前窗口。
模块会避免误关以下场景：
- 正在**重命名**
- 当前前台窗口不是该资源管理器实例

可选项：
- *当存在输入法候选窗口时仍允许关闭*（适用于部分输入法造成的临时 Edit 焦点）

**实现原理**
在 explorer.exe 内部安装低级键盘钩子，监听 Esc。
若判断当前焦点处于安全状态，则向窗口发送 `WM_CLOSE`。

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- allowWhenImeCandidate: false
  $name: Allow closing when IME candidate window is present
  $description: Ignore temporary Edit focus created by IME candidate popups.
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

// ---------- settings ----------
static bool g_allowImeCandidate = false;

// ---------- hook thread state ----------
static HHOOK  g_hook           = nullptr;
static HANDLE g_hookThread     = nullptr;
static DWORD  g_hookThreadId   = 0;
static HANDLE g_hookReadyEvent = nullptr; // signaled after SetWindowsHookEx

// ---------- helpers ----------
static bool IsClass(HWND h, LPCWSTR cls) {
    if (!h) return false;
    WCHAR buf[64] = {};
    if (!GetClassNameW(h, buf, ARRAYSIZE(buf))) return false;
    return lstrcmpiW(buf, cls) == 0;
}

static HWND GetTopLevel(HWND h) {
    return h ? GetAncestor(h, GA_ROOT) : nullptr;
}

static bool IsExplorerTop(HWND hTop) {
    return hTop && (IsClass(hTop, L"CabinetWClass") || IsClass(hTop, L"ExploreWClass"));
}

static bool IsDescendant(HWND parent, HWND child) {
    if (!parent || !child) return false;
    if (parent == child) return true;
    return IsChild(parent, child) != 0;
}

static bool IsInlineRename(HWND hFocus) {
    if (!hFocus) return false;
    if (!IsClass(hFocus, L"Edit") && !IsClass(hFocus, L"RichEdit20W")) return false;
    HWND p = hFocus;
    for (int i = 0; i < 8 && p; ++i) {
        p = GetParent(p);
        if (!p) break;
        if (IsClass(p, L"SHELLDLL_DefView") || IsClass(p, L"DirectUIHWND"))
            return true;
    }
    return false;
}

static bool IsTextInput(HWND hFocus) {
    if (!hFocus) return false;
    if (IsClass(hFocus, L"Edit") || IsClass(hFocus, L"RichEdit20W")) return true;
    if (IsClass(hFocus, L"ComboBox") || IsClass(hFocus, L"ComboBoxEx32")) return true;
    return false;
}

static bool IsImeCandidate(HWND hFocus, HWND explorerTop) {
    HWND top = GetTopLevel(hFocus);
    return top && top != explorerTop;
}

static bool SafeToClose(HWND explorerTop) {
    if (!explorerTop) return false;

    GUITHREADINFO gi = { sizeof(gi) };
    DWORD tid = GetWindowThreadProcessId(explorerTop, nullptr);
    if (!GetGUIThreadInfo(tid, &gi)) return false;

    HWND hFocus = gi.hwndFocus;
    if (!IsDescendant(explorerTop, hFocus)) return false;
    if (IsInlineRename(hFocus)) return false;

    if (IsTextInput(hFocus)) {
        if (!(g_allowImeCandidate && IsImeCandidate(hFocus, explorerTop)))
            return false;
    }
    return true;
}

// ---------- keyboard hook ----------
static LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const KBDLLHOOKSTRUCT* ks = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && ks->vkCode == VK_ESCAPE) {
            HWND fg = GetForegroundWindow();
            HWND top = GetTopLevel(fg);
            if (IsExplorerTop(top) && SafeToClose(top)) {
                PostMessageW(top, WM_CLOSE, 0, 0);
                return 1; // consume Esc
            }
        }
    }
    return CallNextHookEx(g_hook, code, wParam, lParam);
}

// ---------- hook thread ----------
static DWORD WINAPI HookThreadProc(LPVOID) {
    // Obtain the HMODULE of this DLL for SetWindowsHookEx.
    HMODULE hMod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&LowLevelKeyboardProc),
        &hMod);

    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
    if (g_hookReadyEvent) SetEvent(g_hookReadyEvent);

    if (!g_hook) {
        g_hookThreadId = 0;
        return 0;
    }

    // Message loop required by WH_KEYBOARD_LL delivery.
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }
    g_hookThreadId = 0;
    return 0;
}

// ---------- windhawk entry points ----------
static void LoadSettings() {
    g_allowImeCandidate = Wh_GetIntSetting(L"allowWhenImeCandidate") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_hookReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hookReadyEvent) return FALSE;

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        CloseHandle(g_hookReadyEvent);
        g_hookReadyEvent = nullptr;
        return FALSE;
    }

    // Wait for the hook attempt to complete (install success or fail).
    WaitForSingleObject(g_hookReadyEvent, 5000);
    CloseHandle(g_hookReadyEvent);
    g_hookReadyEvent = nullptr;

    if (!g_hook) {
        if (g_hookThread) {
            WaitForSingleObject(g_hookThread, INFINITE);
            CloseHandle(g_hookThread);
            g_hookThread = nullptr;
        }
        g_hookThreadId = 0;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 3000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
