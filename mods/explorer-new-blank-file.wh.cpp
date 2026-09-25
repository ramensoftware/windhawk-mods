// ==WindhawkMod==
// @id              explorer-new-blank-file
// @name            Explorer Ctrl + Key = New Extensionless File
// @description     Use Ctrl+<Letter> (default Ctrl+G) in File Explorer to create a new file in the active folder or tab.
// @version         2.6.9.15
// @author          TheShadyRainbow4
// @github          https://github.com/TheShadyRainbow4
// @homepage        https://main.elitesoftwaretech.cc
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luuid -luser32 -luiautomationcore
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- mod1: ctrl
  $name: First Modifier
  $description: Primary modifier key
  $options:
  - ctrl: Ctrl
  - shift: Shift
  - alt: Alt
  - win: Windows Key
- mod2: none
  $name: Second Modifier (Optional)
  $description: Secondary modifier key
  $options:
  - none: None
  - ctrl: Ctrl
  - shift: Shift
  - alt: Alt
  - win: Windows Key
- hotkey_char: g
  $name: Hotkey Character
  $description: The letter or character for the hotkey
  $options:
  - a: A
  - b: B
  - c: C
  - d: D
  - e: E
  - f: F
  - g: G
  - h: H
  - i: I
  - j: J
  - k: K
  - l: L
  - m: M
  - n: N
  - o: O
  - p: P
  - q: Q
  - r: R
  - s: S
  - t: T
  - u: U
  - v: V
  - w: W
  - x: X
  - y: Y
  - z: Z
  - '0': '0'
  - '1': '1'
  - '2': '2'
  - '3': '3'
  - '4': '4'
  - '5': '5'
  - '6': '6'
  - '7': '7'
  - '8': '8'
  - '9': '9'
  - comma: ", (Comma)"
  - slash: "/ (Slash)"
  - semicolon: "; (Semicolon)"
  - quote: "' (Quote)"
  - lbracket: "[ (Left Bracket)"
  - rbracket: "] (Right Bracket)"
  - backslash: "\ (Backslash)"
  - minus: "- (Minus)"
  - equals: "= (Equals)"
  - backtick: "` (Backtick)"
  - multiply: "* (Multiply)"
- fileName: file
  $name: Default File Name
  $description: The default name for the new file
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*

Turns **Ctrl+<Letter>** (default **Ctrl+G**) in File Explorer into "create a new file here" instead of
opening a new Explorer window.

The mod creates an empty `file` file in the active Explorer folder and
immediately enters rename mode. If a file with that name already exists, the mod
uses the next available name such as `file (2)`. On the desktop, the file is
created directly in the desktop folder.

The hotkey letter can be customized in the mod settings (any letter A-Z).

**Version 1.2**

- Added customizable shortcut letter key setting (Ctrl + any letter A-Z).

**Version 1.1**

- Supports the active tab in tabbed Explorer windows.
- Uses Shell file operation fallback for protected folders, allowing Windows to
  show the standard elevation prompt when needed.
- Improves rename reliability by using the active Shell view first.
- Runs as a Windhawk tool mod, outside of explorer.exe.
- Resolves the active folder through Explorer's Shell view instead of scraping
  address bar text.

*/
// ==/WindhawkModReadme==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x06010000
#endif

#include <sdkddkver.h>

#include <windows.h>
#include <string>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <exdisp.h>
#include <oleauto.h>
#include <knownfolders.h>
#include <shlguid.h>
#include <uiautomation.h>

#ifndef WH_MOD
#define WH_MOD
#endif
#if __has_include(<windhawk_api.h>)
#include <windhawk_api.h>
#endif
#ifndef WH_MOD_ID
#define WH_MOD_ID L"explorer-ctrl-new-blank-file"
#endif

#ifndef GPFIDL_DEFAULT
#define GPFIDL_DEFAULT 0x0000
#endif

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "uiautomationcore.lib")

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#ifndef FOFX_SHOWELEVATIONPROMPT
#define FOFX_SHOWELEVATIONPROMPT 0x00040000
#endif

#ifndef FOFX_NOCOPYHOOKS
#define FOFX_NOCOPYHOOKS 0x00800000
#endif

template <typename T>
static void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

static IUIAutomation* g_uia = nullptr;
static volatile LONG g_actionRunning = 0;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static int g_mod1 = VK_CONTROL;
static int g_mod2 = 0;
static int g_hotkey = 'G';
static std::wstring g_fileName = L"file";

static bool EnsureUIA() {
    if (g_uia) {
        return true;
    }

    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation,
                                  nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&g_uia));

    if (FAILED(hr) || !g_uia) {
        Wh_Log(L"CoCreateInstance(CUIAutomation) failed. hr=0x%08X", hr);
        return false;
    }

    return true;
}

// ---------------- Basic helpers ----------------
static std::wstring JoinPath(const std::wstring& a, const std::wstring& b) {
    if (a.empty()) return b;
    if (a.back() == L'\\' || a.back() == L'/') return a + b;
    return a + L'\\' + b;
}

static std::wstring GetFileNamePart(const std::wstring& fullPath) {
    const wchar_t* p = PathFindFileNameW(fullPath.c_str());
    return p ? std::wstring(p) : L"";
}

static std::wstring GetParentDir(const std::wstring& fullPath) {
    wchar_t buf[MAX_PATH * 4] = {0};
    wcscpy_s(buf, fullPath.c_str());

    if (!PathRemoveFileSpecW(buf)) {
        return L"";
    }

    return buf;
}

static std::wstring MakeUniqueFilename(const std::wstring& dir) {
    AcquireSRWLockShared(&g_settingsLock);
    std::wstring baseName = g_fileName;
    ReleaseSRWLockShared(&g_settingsLock);
    std::wstring path = JoinPath(dir, baseName);

    if (!PathFileExistsW(path.c_str())) {
        return path;
    }

    for (int i = 2; i < 10000; ++i) {
        wchar_t buf[64] = {0};
        swprintf_s(buf, ARRAYSIZE(buf), L"%s (%d)", baseName.c_str(), i);

        path = JoinPath(dir, buf);
        if (!PathFileExistsW(path.c_str())) {
            return path;
        }
    }

    return JoinPath(dir, baseName + L"_new");
}

static std::wstring GetDesktopDir() {
    PWSTR p = nullptr;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &p))) {
        std::wstring ret = p;
        CoTaskMemFree(p);
        return ret;
    }

    wchar_t buf[MAX_PATH] = {0};

    if (SUCCEEDED(SHGetFolderPathW(nullptr,
                                   CSIDL_DESKTOPDIRECTORY,
                                   nullptr,
                                   SHGFP_TYPE_CURRENT,
                                   buf))) {
        return buf;
    }

    return L"";
}

static bool IsPermissionError(DWORD err) {
    return err == ERROR_ACCESS_DENIED ||
           err == ERROR_PRIVILEGE_NOT_HELD ||
           err == ERROR_ELEVATION_REQUIRED;
}

// ---------------- Window helpers ----------------
static bool IsClassName(HWND hwnd, const wchar_t* expected) {
    if (!hwnd || !expected) return false;

    wchar_t cls[128] = {0};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    return wcscmp(cls, expected) == 0;
}

static bool IsExplorerTopLevel(HWND hwnd) {
    return IsClassName(hwnd, L"CabinetWClass") ||
           IsClassName(hwnd, L"ExploreWClass");
}

static bool IsDescendantOrSelf(HWND parent, HWND child) {
    if (!parent || !child) return false;
    if (parent == child) return true;
    return IsChild(parent, child) != FALSE;
}

static HWND FindAncestorOrSelfByClass(HWND hwnd,
                                      const wchar_t* className,
                                      HWND stopAt) {
    for (HWND cur = hwnd; cur; cur = GetParent(cur)) {
        if (IsClassName(cur, className)) {
            return cur;
        }

        if (cur == stopAt) {
            break;
        }
    }

    return nullptr;
}

static HWND HitTestExplorerByCursor() {
    POINT pt = {};
    GetCursorPos(&pt);

    HWND h = WindowFromPoint(pt);
    if (!h) return nullptr;

    HWND root = GetAncestor(h, GA_ROOT);

    if (IsExplorerTopLevel(root)) {
        return root;
    }

    for (HWND cur = h; cur; cur = GetParent(cur)) {
        if (IsExplorerTopLevel(cur)) {
            return cur;
        }
    }

    return nullptr;
}

static HWND GetTargetExplorerHWND() {
    HWND fg = GetForegroundWindow();

    if (fg && (IsExplorerTopLevel(fg) || fg == GetShellWindow())) {
        return fg;
    }

    if (HWND h = HitTestExplorerByCursor()) {
        return h;
    }

    HWND shell = GetShellWindow();

    if (shell) {
        return shell;
    }

    return nullptr;
}

static HWND GetThreadFocusWindow(HWND topLevel) {
    if (!topLevel) return nullptr;

    DWORD tid = GetWindowThreadProcessId(topLevel, nullptr);

    GUITHREADINFO gi = {};
    gi.cbSize = sizeof(gi);

    if (!GetGUIThreadInfo(tid, &gi)) {
        return nullptr;
    }

    if (gi.hwndFocus) return gi.hwndFocus;
    if (gi.hwndActive) return gi.hwndActive;

    return nullptr;
}

struct DefViewSearchCtx {
    HWND found = nullptr;
};

static BOOL CALLBACK FindDefViewEnumProc(HWND child, LPARAM lParam) {
    DefViewSearchCtx* ctx = reinterpret_cast<DefViewSearchCtx*>(lParam);

    if (IsWindowVisible(child) && IsClassName(child, L"SHELLDLL_DefView")) {
        ctx->found = child;
        return FALSE;
    }

    return TRUE;
}

struct ShellTabSearchCtx {
    HWND bestTab = nullptr;
    LONG_PTR bestArea = 0;
};

static BOOL CALLBACK FindShellTabEnumProc(HWND hwnd, LPARAM lParam) {
    ShellTabSearchCtx* ctx = reinterpret_cast<ShellTabSearchCtx*>(lParam);

    if (!IsWindowVisible(hwnd)) return TRUE;
    if (!IsClassName(hwnd, L"ShellTabWindowClass")) return TRUE;

    DefViewSearchCtx defViewCtx;
    EnumChildWindows(hwnd,
                     FindDefViewEnumProc,
                     reinterpret_cast<LPARAM>(&defViewCtx));

    if (!defViewCtx.found) return TRUE;

    RECT rc = {};
    if (!GetWindowRect(defViewCtx.found, &rc)) return TRUE;

    LONG_PTR area =
        static_cast<LONG_PTR>(rc.right - rc.left) *
        static_cast<LONG_PTR>(rc.bottom - rc.top);

    if (area > ctx->bestArea) {
        ctx->bestArea = area;
        ctx->bestTab = hwnd;
    }

    return TRUE;
}

static HWND GetActiveShellTabHwnd(HWND explorerHwnd) {
    if (!explorerHwnd || explorerHwnd == GetShellWindow()) {
        return nullptr;
    }

    HWND focus = GetThreadFocusWindow(explorerHwnd);

    if (focus && IsDescendantOrSelf(explorerHwnd, focus)) {
        HWND tab = FindAncestorOrSelfByClass(focus,
                                             L"ShellTabWindowClass",
                                             explorerHwnd);
        if (tab) {
            return tab;
        }
    }

    // 如果焦点在标签栏、命令栏或地址栏，不一定处于 ShellTabWindowClass 子树。
    // 这时退回到可见的 SHELLDLL_DefView 所在 tab。
    ShellTabSearchCtx ctx;
    EnumChildWindows(explorerHwnd,
                     FindShellTabEnumProc,
                     reinterpret_cast<LPARAM>(&ctx));

    return ctx.bestTab;
}

static void ForceForeground(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    DWORD targetThread = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThread = GetCurrentThreadId();

    AttachThreadInput(currentThread, targetThread, TRUE);
    SetForegroundWindow(hwnd);
    BringWindowToTop(hwnd);
    AttachThreadInput(currentThread, targetThread, FALSE);
}

static void SendSimpleKey(WORD key) {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}




static int GetModifierVK(const std::wstring& modStr) {
    if (modStr == L"ctrl") return VK_CONTROL;
    if (modStr == L"shift") return VK_SHIFT;
    if (modStr == L"alt") return VK_MENU;
    if (modStr == L"win") return VK_LWIN;
    return 0;
}

static int GetHotkeyVK(const std::wstring& keyStr) {
    if (keyStr.empty()) return 'G';
    
    if (keyStr.length() == 1) {
        wchar_t c = towupper(keyStr[0]);
        if (c >= L'A' && c <= L'Z') return c;
        if (c >= L'0' && c <= L'9') return c;
        switch (c) {
            case L',': return VK_OEM_COMMA;
            case L'/': return VK_OEM_2;
            case L';': return VK_OEM_1;
            case L'\'': return VK_OEM_7;
            case L'[': return VK_OEM_4;
            case L']': return VK_OEM_6;
            case L'\\': return VK_OEM_5;
            case L'-': return VK_OEM_MINUS;
            case L'=': return VK_OEM_PLUS;
            case L'`': return VK_OEM_3;
            case L'*': return VK_MULTIPLY;
        }
    }
    
    if (keyStr == L"comma") return VK_OEM_COMMA;
    if (keyStr == L"slash") return VK_OEM_2;
    if (keyStr == L"semicolon") return VK_OEM_1;
    if (keyStr == L"quote") return VK_OEM_7;
    if (keyStr == L"lbracket") return VK_OEM_4;
    if (keyStr == L"rbracket") return VK_OEM_6;
    if (keyStr == L"backslash") return VK_OEM_5;
    if (keyStr == L"minus") return VK_OEM_MINUS;
    if (keyStr == L"equals") return VK_OEM_PLUS;
    if (keyStr == L"backtick") return VK_OEM_3;
    if (keyStr == L"multiply") return VK_MULTIPLY;
    
    return 'G';
}

static bool CheckModifiers() {
    bool mod1_pressed = (g_mod1 == 0) || ((GetAsyncKeyState(g_mod1) & 0x8000) != 0);
    bool mod2_pressed = (g_mod2 == 0) || ((GetAsyncKeyState(g_mod2) & 0x8000) != 0);
    
    // Ensure that if a modifier IS NOT selected, it MUST NOT be pressed
    if (g_mod1 != VK_CONTROL && g_mod2 != VK_CONTROL && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) return false;
    if (g_mod1 != VK_SHIFT && g_mod2 != VK_SHIFT && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) return false;
    if (g_mod1 != VK_MENU && g_mod2 != VK_MENU && (GetAsyncKeyState(VK_MENU) & 0x8000)) return false;
    if (g_mod1 != VK_LWIN && g_mod2 != VK_LWIN && ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000))) return false;
    
    return mod1_pressed && mod2_pressed;
}


static void LoadSettings() {
    PCWSTR pMod1 = Wh_GetStringSetting(L"mod1");
    int mod1 = GetModifierVK(pMod1 ? pMod1 : L"ctrl");
    if (pMod1) Wh_FreeStringSetting(pMod1);

    PCWSTR pMod2 = Wh_GetStringSetting(L"mod2");
    int mod2 = GetModifierVK(pMod2 ? pMod2 : L"none");
    if (pMod2) Wh_FreeStringSetting(pMod2);

    PCWSTR pHotkey = Wh_GetStringSetting(L"hotkey_char");
    int hotkey = GetHotkeyVK(pHotkey ? pHotkey : L"g");
    if (pHotkey) Wh_FreeStringSetting(pHotkey);

    PCWSTR pFile = Wh_GetStringSetting(L"fileName");
    std::wstring fileName = (pFile && wcslen(pFile) > 0) ? pFile : L"file";
    if (pFile) Wh_FreeStringSetting(pFile);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_mod1 = mod1;
    g_mod2 = mod2;
    g_hotkey = hotkey;
    g_fileName = fileName;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

static void WaitForModifierKeysReleased() {
    int targetKey;
    AcquireSRWLockShared(&g_settingsLock);
    targetKey = g_hotkey;
    ReleaseSRWLockShared(&g_settingsLock);
    
    for (int i = 0; i < 40; ++i) {
        bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool altDown = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool targetKeyDown = (GetAsyncKeyState(targetKey) & 0x8000) != 0;

        if (!ctrlDown && !altDown && !shiftDown && !targetKeyDown) {
            return;
        }

        Sleep(5);
    }
}

// ---------------- ShellView resolve for rename ----------------
static bool GetPathFromShellView(IShellView* view, std::wstring& outPath) {
    outPath.clear();

    if (!view) return false;

    IFolderView* folderView = nullptr;
    HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&folderView));

    if (FAILED(hr) || !folderView) {
        return false;
    }

    IPersistFolder2* persistFolder = nullptr;
    hr = folderView->GetFolder(IID_PPV_ARGS(&persistFolder));
    SafeRelease(folderView);

    if (FAILED(hr) || !persistFolder) {
        return false;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    hr = persistFolder->GetCurFolder(&pidl);
    SafeRelease(persistFolder);

    if (FAILED(hr) || !pidl) {
        return false;
    }

    wchar_t path[MAX_PATH * 4] = {0};
    bool ok = SHGetPathFromIDListEx(pidl,
                                    path,
                                    ARRAYSIZE(path),
                                    GPFIDL_DEFAULT) &&
              path[0] != L'\0';

    CoTaskMemFree(pidl);

    if (ok) {
        outPath = path;
        return true;
    }

    return false;
}

static bool ResolveActiveShellView(HWND explorerHwnd,
                                   std::wstring& outPath,
                                   IShellView** outView) {
    if (!outView) return false;
    outPath.clear();
    *outView = nullptr;

    if (!explorerHwnd || !IsWindow(explorerHwnd)) {
        return false;
    }

    HWND activeTab = GetActiveShellTabHwnd(explorerHwnd);

    IShellWindows* shellWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows,
                                  nullptr,
                                  CLSCTX_ALL,
                                  IID_PPV_ARGS(&shellWindows));

    if (FAILED(hr) || !shellWindows) {
        return false;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    for (long i = count - 1; i >= 0; --i) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;

        IDispatch* dispatch = nullptr;

        if (FAILED(shellWindows->Item(v, &dispatch)) || !dispatch) {
            VariantClear(&v);
            continue;
        }

        IServiceProvider* serviceProvider = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&serviceProvider));
        SafeRelease(dispatch);

        if (FAILED(hr) || !serviceProvider) {
            VariantClear(&v);
            continue;
        }

        IShellBrowser* browser = nullptr;
        hr = serviceProvider->QueryService(SID_STopLevelBrowser,
                                           IID_PPV_ARGS(&browser));
        SafeRelease(serviceProvider);

        if (FAILED(hr) || !browser) {
            VariantClear(&v);
            continue;
        }

        IShellView* view = nullptr;
        hr = browser->QueryActiveShellView(&view);
        SafeRelease(browser);

        if (FAILED(hr) || !view) {
            VariantClear(&v);
            continue;
        }

        HWND viewHwnd = nullptr;
        hr = view->GetWindow(&viewHwnd);

        if (FAILED(hr) || !viewHwnd) {
            SafeRelease(view);
            VariantClear(&v);
            continue;
        }

        HWND viewRoot = GetAncestor(viewHwnd, GA_ROOT);
        if (viewRoot != explorerHwnd) {
            SafeRelease(view);
            VariantClear(&v);
            continue;
        }

        if (activeTab) {
            HWND viewTab = FindAncestorOrSelfByClass(viewHwnd,
                                                     L"ShellTabWindowClass",
                                                     explorerHwnd);
            if (viewTab != activeTab) {
                SafeRelease(view);
                VariantClear(&v);
                continue;
            }
        }

        std::wstring viewPath;
        if (GetPathFromShellView(view, viewPath)) {
            outPath = viewPath;
            *outView = view;
            shellWindows->Release();
            VariantClear(&v);
            return true;
        }

        SafeRelease(view);
        VariantClear(&v);
    }

    shellWindows->Release();
    return false;
}

// ---------------- PIDL / rename ----------------
static bool BindParentAndChildPIDL(const std::wstring& fullPath,
                                   PIDLIST_ABSOLUTE* outFullPidl,
                                   PCUITEMID_CHILD* outChild) {
    if (!outFullPidl || !outChild) return false;

    *outFullPidl = nullptr;
    *outChild = nullptr;

    PIDLIST_ABSOLUTE pidl = nullptr;
    SFGAOF sf = 0;

    HRESULT hr = SHParseDisplayName(fullPath.c_str(),
                                    nullptr,
                                    &pidl,
                                    0,
                                    &sf);

    if (FAILED(hr) || !pidl) {
        return false;
    }

    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;

    hr = SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child);

    if (FAILED(hr) || !parent || !child) {
        if (parent) parent->Release();
        CoTaskMemFree(pidl);
        return false;
    }

    parent->Release();

    *outFullPidl = pidl;
    *outChild = child;
    return true;
}

static bool RenameViaShellView(IShellView* shellView,
                               const std::wstring& fullPath) {
    if (!shellView) {
        return false;
    }

    ULONGLONG t0 = GetTickCount64();

    PIDLIST_ABSOLUTE fullPidl = nullptr;
    PCUITEMID_CHILD child = nullptr;

    if (!BindParentAndChildPIDL(fullPath, &fullPidl, &child)) {
        Wh_Log(L"BindParentAndChildPIDL failed, cost=%llums",
               GetTickCount64() - t0);
        return false;
    }

    HRESULT hr = shellView->SelectItem(child,
                                       SVSI_SELECT |
                                       SVSI_ENSUREVISIBLE |
                                       SVSI_DESELECTOTHERS |
                                       SVSI_EDIT);

    CoTaskMemFree(fullPidl);

    if (SUCCEEDED(hr)) {
        Wh_Log(L"Rename via IShellView succeeded, cost=%llums",
               GetTickCount64() - t0);
        return true;
    }

    Wh_Log(L"IShellView::SelectItem failed. hr=0x%08X cost=%llums",
           hr,
           GetTickCount64() - t0);
    return false;
}

// ---------------- File creation ----------------
static bool CreateEmptyFileDirect(const std::wstring& fullPath,
                                  DWORD* pError = nullptr) {
    if (pError) {
        *pError = ERROR_SUCCESS;
    }

    HANDLE h = CreateFileW(fullPath.c_str(),
                           GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr,
                           CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL,
                           nullptr);

    if (h == INVALID_HANDLE_VALUE) {
        if (pError) {
            *pError = GetLastError();
        }
        return false;
    }

    CloseHandle(h);
    return true;
}

static bool CreateEmptyFileWithShellElevation(const std::wstring& fullPath,
                                              HWND owner) {
    std::wstring dir = GetParentDir(fullPath);
    std::wstring name = GetFileNamePart(fullPath);

    if (dir.empty() || name.empty()) return false;

    IShellItem* folderItem = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(dir.c_str(),
                                             nullptr,
                                             IID_PPV_ARGS(&folderItem));

    if (FAILED(hr) || !folderItem) {
        Wh_Log(L"SHCreateItemFromParsingName failed. hr=0x%08X dir=%ls",
               hr,
               dir.c_str());
        return false;
    }

    IFileOperation* fileOp = nullptr;
    hr = CoCreateInstance(CLSID_FileOperation,
                          nullptr,
                          CLSCTX_ALL,
                          IID_PPV_ARGS(&fileOp));

    if (FAILED(hr) || !fileOp) {
        SafeRelease(folderItem);
        Wh_Log(L"CoCreateInstance(CLSID_FileOperation) failed. hr=0x%08X",
               hr);
        return false;
    }

    if (owner && IsWindow(owner)) {
        fileOp->SetOwnerWindow(owner);
    }

    DWORD flags =
        FOF_NOCONFIRMMKDIR |
        FOFX_SHOWELEVATIONPROMPT |
        FOFX_NOCOPYHOOKS;

    fileOp->SetOperationFlags(flags);

    hr = fileOp->NewItem(folderItem,
                         FILE_ATTRIBUTE_NORMAL,
                         name.c_str(),
                         nullptr,
                         nullptr);

    if (SUCCEEDED(hr)) {
        hr = fileOp->PerformOperations();
    }

    BOOL aborted = FALSE;
    fileOp->GetAnyOperationsAborted(&aborted);

    SafeRelease(fileOp);
    SafeRelease(folderItem);

    if (FAILED(hr) || aborted) {
        Wh_Log(L"IFileOperation::NewItem failed. hr=0x%08X aborted=%d path=%ls",
               hr,
               aborted,
               fullPath.c_str());
        return false;
    }

    return PathFileExistsW(fullPath.c_str()) != FALSE;
}

// ---------------- UIA rename fallback ----------------
static IUIAutomationCondition* UIA_MakeExactNameCondition(const std::wstring& name) {
    if (!EnsureUIA()) return nullptr;

    VARIANT vName;
    VariantInit(&vName);
    vName.vt = VT_BSTR;
    vName.bstrVal = SysAllocString(name.c_str());

    IUIAutomationCondition* nameCond = nullptr;
    g_uia->CreatePropertyCondition(UIA_NamePropertyId,
                                   vName,
                                   &nameCond);

    VariantClear(&vName);
    return nameCond;
}

static bool UIA_SelectItemByNameFallback(HWND explorerHwnd,
                                         const std::wstring& fileName) {
    if (!explorerHwnd || !IsWindow(explorerHwnd) || fileName.empty()) {
        return false;
    }

    if (!EnsureUIA()) {
        return false;
    }

    IUIAutomationElement* root = nullptr;
    HRESULT hr = g_uia->ElementFromHandle(explorerHwnd, &root);

    if (FAILED(hr) || !root) {
        return false;
    }

    IUIAutomationCondition* nameCond = UIA_MakeExactNameCondition(fileName);
    if (!nameCond) {
        SafeRelease(root);
        return false;
    }

    IUIAutomationElement* item = nullptr;
    hr = root->FindFirst(TreeScope_Descendants, nameCond, &item);

    SafeRelease(nameCond);
    SafeRelease(root);

    if (FAILED(hr) || !item) {
        return false;
    }

    bool ok = false;

    IUnknown* unkPattern = nullptr;
    hr = item->GetCurrentPattern(UIA_SelectionItemPatternId, &unkPattern);

    if (SUCCEEDED(hr) && unkPattern) {
        IUIAutomationSelectionItemPattern* sel = nullptr;

        if (SUCCEEDED(unkPattern->QueryInterface(IID_PPV_ARGS(&sel))) && sel) {
            ok = SUCCEEDED(sel->Select());
            SafeRelease(sel);
        }

        SafeRelease(unkPattern);
    }

    if (!ok) {
        ok = SUCCEEDED(item->SetFocus());
    } else {
        item->SetFocus();
    }

    SafeRelease(item);
    return ok;
}

static void StartRename(HWND explorerHwnd,
                        IShellView* shellView,
                        const std::wstring& fullPath,
                        const std::wstring& folderPath,
                        bool elevated) {
    std::wstring fileName = GetFileNamePart(fullPath);

    if (fileName.empty()) return;

    SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW, fullPath.c_str(), nullptr);
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, folderPath.c_str(), nullptr);

    ULONGLONG t0 = GetTickCount64();

    if (shellView) {
        bool ok = RenameViaShellView(shellView, fullPath);

        if (ok) {
            return;
        }
    } else {
        Wh_Log(L"Active ShellView for rename not available, cost=%llums",
               GetTickCount64() - t0);
    }

    ULONGLONG fallbackStart = GetTickCount64();
    const int maxWaitMs = elevated ? 900 : 250;
    const int intervalMs = 10;
    const int maxTries = maxWaitMs / intervalMs;

    bool selected = false;

    for (int i = 0; i < maxTries; ++i) {
        selected = UIA_SelectItemByNameFallback(explorerHwnd, fileName);

        if (selected) {
            break;
        }

        Sleep(intervalMs);
    }

    if (!selected) {
        Wh_Log(L"Fallback UIA failed to select item: %ls, cost=%llums",
               fileName.c_str(),
               GetTickCount64() - fallbackStart);
        return;
    }

    WaitForModifierKeysReleased();
    ForceForeground(explorerHwnd);
    SendSimpleKey(VK_F2);

    Wh_Log(L"Rename via fallback UIA+F2, cost=%llums",
           GetTickCount64() - fallbackStart);
}

// ---------------- Main action ----------------
static void PerformNewFileAction() {
    if (InterlockedCompareExchange(&g_actionRunning, 1, 0) != 0) {
        return;
    }

    ULONGLONG totalStart = GetTickCount64();

    HWND target = GetTargetExplorerHWND();

    if (!target) {
        Wh_Log(L"No explorer window detected.");
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    std::wstring dir;

    IShellView* activeView = nullptr;

    if (target == GetShellWindow()) {
        dir = GetDesktopDir();
    } else if (!ResolveActiveShellView(target, dir, &activeView) || dir.empty()) {
        Wh_Log(L"Failed to resolve current active tab path.");
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) {
        dir.pop_back();
    }

    DWORD dirAttrs = GetFileAttributesW(dir.c_str());

    if (dirAttrs == INVALID_FILE_ATTRIBUTES ||
        !(dirAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"Unsupported target directory: %ls", dir.c_str());
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    std::wstring newFile = MakeUniqueFilename(dir);

    DWORD createError = ERROR_SUCCESS;
    bool elevated = false;

    ULONGLONG createStart = GetTickCount64();
    bool created = CreateEmptyFileDirect(newFile, &createError);

    if (!created) {
        Wh_Log(L"CreateFileW failed. error=%lu path=%ls",
               createError,
               newFile.c_str());

        if (!IsPermissionError(createError)) {
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }

        elevated = true;
        created = CreateEmptyFileWithShellElevation(newFile, target);

        if (!created) {
            Wh_Log(L"Elevated IFileOperation creation failed: %ls",
                   newFile.c_str());
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }
    }

    Wh_Log(L"File created: %ls, cost=%llums",
           newFile.c_str(),
           GetTickCount64() - createStart);

    StartRename(target, activeView, newFile, dir, elevated);
    SafeRelease(activeView);

    Wh_Log(L"Total action cost=%llums",
           GetTickCount64() - totalStart);

    InterlockedExchange(&g_actionRunning, 0);
}

// ---------------- Hook thread ----------------
static volatile HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HHOOK g_lowLevelHook = nullptr;
static volatile HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;

static volatile LONG g_ctrlNSequence = 0;
static volatile LONG g_actionPosted = 0;

static DWORD WINAPI WorkerThread(void* pParameter);
static DWORD WINAPI HookThread(void* pParameter);
static LRESULT CALLBACK LowLevelKeybdProc(int nCode,
                                          WPARAM wParam,
                                          LPARAM lParam);

static BOOL Worker_Init() {
    if (g_workerThread) {
        return TRUE;
    }

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) return FALSE;

    HANDLE hThread = CreateThread(nullptr,
                                  0,
                                  WorkerThread,
                                  readyEvent,
                                  CREATE_SUSPENDED,
                                  &g_workerThreadId);

    if (!hThread) {
        CloseHandle(readyEvent);
        return FALSE;
    }

    SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    ResumeThread(hThread);

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    g_workerThread = hThread;
    return TRUE;
}

static void Worker_Exit() {
    HANDLE hThread =
        (HANDLE)InterlockedExchangePointer((PVOID*)&g_workerThread, nullptr);

    if (!hThread) return;

    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_APP, 0, 0);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    g_workerThreadId = 0;
}

static BOOL KeybdHook_Init() {
    if (g_hookThread) {
        return TRUE;
    }

    if (!Worker_Init()) {
        Wh_Log(L"Worker thread initialization failed.");
        return FALSE;
    }

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        Worker_Exit();
        return FALSE;
    }

    HANDLE hThread = CreateThread(nullptr,
                                  0,
                                  HookThread,
                                  readyEvent,
                                  CREATE_SUSPENDED,
                                  &g_hookThreadId);

    if (!hThread) {
        CloseHandle(readyEvent);
        Worker_Exit();
        return FALSE;
    }

    SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    ResumeThread(hThread);

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    if (!g_lowLevelHook) {
        Wh_Log(L"SetWindowsHookEx failed.");
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        g_hookThreadId = 0;
        Worker_Exit();
        return FALSE;
    }

    g_hookThread = hThread;
    return TRUE;
}

static void KeybdHook_Exit() {
    HANDLE hThread =
        (HANDLE)InterlockedExchangePointer((PVOID*)&g_hookThread, nullptr);

    if (!hThread) return;

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_APP, 0, 0);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    g_hookThreadId = 0;
    g_lowLevelHook = nullptr;

    Worker_Exit();
}

static DWORD WINAPI WorkerThread(void* pParameter) {
    HANDLE readyEvent = (HANDLE)pParameter;
    MSG msg;

    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HRESULT hrCom = CoInitializeEx(nullptr,
                                   COINIT_APARTMENTTHREADED |
                                   COINIT_DISABLE_OLE1DDE);

    SetEvent(readyEvent);

    while (true) {
        BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);

        if (bRet <= 0) {
            break;
        }

        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }

            if (msg.message == WM_APP + 1) {
                InterlockedExchange(&g_actionPosted, 0);
                PerformNewFileAction();
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    SafeRelease(g_uia);

    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }

    return 0;
}

static DWORD WINAPI HookThread(void* pParameter) {
    HANDLE readyEvent = (HANDLE)pParameter;
    MSG msg;

    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_lowLevelHook = SetWindowsHookExW(WH_KEYBOARD_LL,
                                       LowLevelKeybdProc,
                                       HINST_THISCOMPONENT,
                                       0);

    SetEvent(readyEvent);

    if (!g_lowLevelHook) {
        return 0;
    }

    while (true) {
        BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);

        if (bRet <= 0) {
            break;
        }

        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }

        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(g_lowLevelHook);
    g_lowLevelHook = nullptr;

    return 0;
}

static bool IsExplorerForegroundForHotkey() {
    HWND fg = GetForegroundWindow();

    if (!fg) return false;

    return IsExplorerTopLevel(fg) || fg == GetShellWindow();
}

static LRESULT CALLBACK LowLevelKeybdProc(int nCode,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const KBDLLHOOKSTRUCT* info =
            reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if (!info) {
            return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
        }

        const bool injected = (info->flags & LLKHF_INJECTED) != 0;
        const bool isKeyDown =
            (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        const bool isKeyUp =
            (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (injected) {
            return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
        }

        AcquireSRWLockShared(&g_settingsLock);
        int targetKey = g_hotkey;
        ReleaseSRWLockShared(&g_settingsLock);

        if (isKeyDown && info->vkCode == targetKey) {
            AcquireSRWLockShared(&g_settingsLock);
            bool modifiers_match = CheckModifiers();
            ReleaseSRWLockShared(&g_settingsLock);

            if (modifiers_match && IsExplorerForegroundForHotkey()) {
                InterlockedExchange(&g_ctrlNSequence, 1);

                if (g_workerThreadId &&
                    InterlockedCompareExchange(&g_actionPosted, 1, 0) == 0) {
                    if (!PostThreadMessageW(g_workerThreadId,
                                            WM_APP + 1,
                                            0,
                                            0)) {
                        InterlockedExchange(&g_actionPosted, 0);
                    }
                }

                return 1;
            }
        }

        if (isKeyUp && info->vkCode == targetKey) {
            if (InterlockedExchange(&g_ctrlNSequence, 0) == 1) {
                return 1;
            }
        }
    }

    return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
}

// ---------------- Tool mod entry points ----------------
BOOL WhTool_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    return KeybdHook_Init();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    KeybdHook_Exit();
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod boilerplate

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; ++i) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; ++i) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileNameW(nullptr,
                               currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 256];
    swprintf_s(commandLine,
               L"\"%s\" -tool-mod \"%s\"",
               currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE,
        LPCWSTR,
        LPWSTR,
        LPSECURITY_ATTRIBUTES,
        LPSECURITY_ATTRIBUTES,
        WINBOOL,
        DWORD,
        LPVOID,
        LPCWSTR,
        LPSTARTUPINFOW,
        LPPROCESS_INFORMATION,
        PHANDLE);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION pi = {};
    if (!pCreateProcessInternalW(nullptr,
                                 currentProcessPath,
                                 commandLine,
                                 nullptr,
                                 nullptr,
                                 FALSE,
                                 NORMAL_PRIORITY_CLASS,
                                 nullptr,
                                 nullptr,
                                 &si,
                                 &pi,
                                 nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    Wh_Log(L"Settings changed");
    LoadSettings();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
