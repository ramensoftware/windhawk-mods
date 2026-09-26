// ==WindhawkMod==
// @id              explorer-ctrlq-new-folder
// @name            Explorer Ctrl + Key = New Folder
// @description     Press a custom hotkey (default Ctrl+Q) in Explorer to create a new folder in current path.
// @version         1.4.8.9
// @author          TheShadyRainbow4
// @github          https://github.com/TheShadyRainbow4
// @homepage        https://main.elitesoftwaretech.cc
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luuid -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Explorer Custom Hotkey to New Folder

(Formally Ctrl Q New Folder)

This mod enhances Windows Explorer by mapping a configurable hotkey (default **Ctrl+Q**) to create a new folder instantly.

### Purpose & Fork Details
This is a fork of "Explorer Ctrl+N to New File". The original mod bound the action to **Ctrl+N**, which conflicted with the native "New Window" shortcut, and generated blank, extension-less files which were not useful for most users.

This version improves the workflow by:
1.  **Customizable Hotkey**: Remaps the hotkey (default Ctrl+Q) to eliminate conflicts with standard Windows shortcuts. Supports up to two modifiers and a key.
2.  **Creating Directories**: Generates standard file system folders instead of empty files.
3.  **Customizable Name**: Allows setting a custom default folder name.
4.  **Stability Fixes**: Implements `SHChangeNotify` and thread synchronization to prevent the "Race Condition" crash where Explorer would attempt to rename the folder before the UI had finished creating it.

### How it works
The mod utilizes Windhawk to inject a `WH_KEYBOARD_LL` (low-level keyboard hook) directly into the `explorer.exe` process. Upon triggering:
1.  It resolves the current directory path (supporting Windowed Explorer windows only not Desktop).
2.  It creates a unique directory name (e.g., "New folder (2)") based on the user-configured default name.
3.  It forces a Shell Update to register the change immediately.
4.  It programmatically selects the new folder and initiates the rename command.

*/
// ==/WindhawkModReadme==

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
- hotkey_char: q
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
- folderName: New folder
  $name: Default Folder Name
  $description: The default name for the new folder
*/
// ==/WindhawkModSettings==


#include <sdkddkver.h>

#include <windows.h>
#include <string>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <oleauto.h>
#include <KnownFolders.h>
#include <ShlGuid.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")


static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static int g_modifier1 = VK_CONTROL;
static int g_modifier2 = 0;
static int g_hotkey = 'Q';
static std::wstring g_folderName = L"New folder";

static int GetModifierVK(const std::wstring& modStr) {
    if (modStr == L"ctrl") return VK_CONTROL;
    if (modStr == L"shift") return VK_SHIFT;
    if (modStr == L"alt") return VK_MENU;
    if (modStr == L"win") return VK_LWIN;
    return 0;
}

static int GetHotkeyVK(const std::wstring& keyStr) {
    if (keyStr.empty()) return 'Q';
    
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
    
    return 'Q';
}

static void LoadSettings() {
    PCWSTR pMod1 = Wh_GetStringSetting(L"mod1");
    int mod1 = GetModifierVK(pMod1 ? pMod1 : L"ctrl");
    if (pMod1) Wh_FreeStringSetting(pMod1);

    PCWSTR pMod2 = Wh_GetStringSetting(L"mod2");
    int mod2 = GetModifierVK(pMod2 ? pMod2 : L"none");
    if (pMod2) Wh_FreeStringSetting(pMod2);

    PCWSTR pHotkey = Wh_GetStringSetting(L"hotkey_char");
    int hotkey = GetHotkeyVK(pHotkey ? pHotkey : L"q");
    if (pHotkey) Wh_FreeStringSetting(pHotkey);

    PCWSTR pFolder = Wh_GetStringSetting(L"folderName");
    std::wstring folderName = (pFolder && wcslen(pFolder) > 0) ? pFolder : L"New folder";
    if (pFolder) Wh_FreeStringSetting(pFolder);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_modifier1 = mod1;
    g_modifier2 = mod2;
    g_hotkey = hotkey;
    g_folderName = folderName;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

static bool CheckModifiers(int mod1, int mod2) {
    bool ctrlReq = (mod1 == VK_CONTROL || mod2 == VK_CONTROL);
    bool shiftReq = (mod1 == VK_SHIFT || mod2 == VK_SHIFT);
    bool altReq = (mod1 == VK_MENU || mod2 == VK_MENU);
    bool winReq = (mod1 == VK_LWIN || mod2 == VK_LWIN);

    bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool altDown = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool winDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 || (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;

    return (ctrlReq == ctrlDown) && (shiftReq == shiftDown) && (altReq == altDown) && (winReq == winDown);
}

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

// ----------------- Utilities -----------------
struct ComInit {
    ComInit() { CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); }
    ~ComInit() { CoUninitialize(); }
};

static std::wstring FromBSTR(BSTR b) {
    if (!b) return L"";
    return std::wstring(b, SysStringLen(b));
}

static std::wstring UrlToPath(const std::wstring& url) {
    if (url.empty()) return L"";
    if (url.rfind(L"about:", 0) == 0) return L"";
    wchar_t path[MAX_PATH * 4] = {0};
    DWORD cch = ARRAYSIZE(path);
    if (SUCCEEDED(PathCreateFromUrlW(url.c_str(), path, &cch, 0))) return path;
    const std::wstring pfx = L"file:///";
    if (url.rfind(pfx, 0) == 0) return url.substr(pfx.size());
    return url;
}

static bool IsExplorerTopLevel(HWND hwnd) {
    if (!hwnd) return false;
    wchar_t cls[64] = {0};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"CabinetWClass") == 0 || wcscmp(cls, L"ExploreWClass") == 0;
}

static bool IsCurrentProcessWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid == GetCurrentProcessId();
}

static HWND HitTestExplorerByCursor() {
    POINT pt;
    GetCursorPos(&pt);
    HWND h = WindowFromPoint(pt);
    if (!h) return nullptr;
    HWND root = GetAncestor(h, GA_ROOT);
    if (IsExplorerTopLevel(root) && IsCurrentProcessWindow(root)) return root;
    for (HWND cur = h; cur; cur = GetParent(cur)) {
        if (IsExplorerTopLevel(cur) && IsCurrentProcessWindow(cur)) return cur;
    }
    return nullptr;
}

static HWND GetTargetExplorerHWND() {
    if (HWND h = HitTestExplorerByCursor()) return h;
    HWND fg = GetForegroundWindow();
    if (IsExplorerTopLevel(fg) && IsCurrentProcessWindow(fg)) return fg;
    HWND shell = GetShellWindow();
    if (shell && IsCurrentProcessWindow(shell)) return shell;  // desktop fallback
    return nullptr;
}

static bool GetExplorerBrowserByHWND(HWND target, IWebBrowser2** outWB) {
    if (!outWB || !target) return false;
    *outWB = nullptr;

    IShellWindows* spWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&spWindows))))
        return false;

    long count = 0;
    spWindows->get_Count(&count);
    for (long i = 0; i < count; ++i) {
        VARIANT vI;
        VariantInit(&vI);
        V_VT(&vI) = VT_I4;
        V_I4(&vI) = i;

        IDispatch* disp = nullptr;
        if (SUCCEEDED(spWindows->Item(vI, &disp)) && disp) {
            IWebBrowser2* wb = nullptr;
            if (SUCCEEDED(disp->QueryInterface(IID_PPV_ARGS(&wb))) && wb) {
                SHANDLE_PTR spHwnd = 0;
                if (SUCCEEDED(wb->get_HWND(&spHwnd))) {
                    if ((HWND)(ULONG_PTR)spHwnd == target) {
                        *outWB = wb;  // keep ref
                        disp->Release();
                        spWindows->Release();
                        VariantClear(&vI);
                        return true;
                    }
                }
                wb->Release();
            }
            disp->Release();
        }
        VariantClear(&vI);
    }

    // desktop (Progman) fallback
    if (target == GetShellWindow()) {
        VARIANT vEmpty;
        VariantInit(&vEmpty);
        long hwndDummy = 0;
        IDispatch* disp = nullptr;
        if (SUCCEEDED(spWindows->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, &hwndDummy,
                                              SWFO_NEEDDISPATCH, &disp)) &&
            disp) {
            IWebBrowser2* wb = nullptr;
            if (SUCCEEDED(disp->QueryInterface(IID_PPV_ARGS(&wb))) && wb) {
                *outWB = wb;  // keep ref
                disp->Release();
                spWindows->Release();
                return true;
            }
            disp->Release();
        }
    }

    spWindows->Release();
    return false;
}

static bool GetExplorerPathFromHWND(HWND target, std::wstring& outPath) {
    outPath.clear();

    IWebBrowser2* wb = nullptr;
    if (!GetExplorerBrowserByHWND(target, &wb) || !wb) return false;

    BSTR bUrl = nullptr;
    if (SUCCEEDED(wb->get_LocationURL(&bUrl))) {
        outPath = UrlToPath(FromBSTR(bUrl));
        SysFreeString(bUrl);
    }
    wb->Release();
    return !outPath.empty();
}

static std::wstring GetDesktopDir() {
    PWSTR p = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &p))) {
        std::wstring ret = p;
        CoTaskMemFree(p);
        return ret;
    }
    wchar_t buf[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, SHGFP_TYPE_CURRENT, buf)))
        return buf;
    return L"";
}

static std::wstring MakeUniqueFolderName(const std::wstring& dir) {
    auto join = [](const std::wstring& a, const std::wstring& b) {
        if (a.empty()) return b;
        if (a.back() == L'\\' || a.back() == L'/') return a + b;
        return a + L'\\' + b;
    };
    
    std::wstring base;
    AcquireSRWLockShared(&g_settingsLock);
    base = g_folderName;
    ReleaseSRWLockShared(&g_settingsLock);

    std::wstring path = join(dir, base);
    if (!PathFileExistsW(path.c_str())) return path;
    
    for (int i = 2; i < 10000; ++i) {
        wchar_t buf[256];
        swprintf(buf, ARRAYSIZE(buf), L"%ls (%d)", base.c_str(), i);
        path = join(dir, buf);
        if (!PathFileExistsW(path.c_str())) return path;
    }
    return join(dir, base + L"_new");
}

static bool CreateNewDirectory(const std::wstring& fullPath) {
    if (CreateDirectoryW(fullPath.c_str(), nullptr)) {
        return true;
    }
    return false;
}

static bool BindParentAndChildPIDL(const std::wstring& fullPath, IShellFolder** ppFolder,
                                   PCUITEMID_CHILD* ppChild) {
    if (!ppFolder || !ppChild) return false;
    *ppFolder = nullptr;
    *ppChild = nullptr;

    PIDLIST_ABSOLUTE pidl = nullptr;
    SFGAOF sf = 0;
    if (FAILED(SHParseDisplayName(fullPath.c_str(), nullptr, &pidl, 0, &sf)) || !pidl) return false;

    IShellFolder* pFolder = nullptr;
    PCUITEMID_CHILD pChild = nullptr;
    HRESULT hr = SHBindToParent(pidl, IID_PPV_ARGS(&pFolder), &pChild);
    CoTaskMemFree(pidl);
    if (FAILED(hr)) return false;

    *ppFolder = pFolder;  // keep ref
    *ppChild = pChild;    // pointer owned by parent bind
    return true;
}

static void SelectAndRenameInWindow(HWND hwndExplorer, const std::wstring& fullPath) {
    IWebBrowser2* wb = nullptr;
    if (!GetExplorerBrowserByHWND(hwndExplorer, &wb) || !wb) return;

    wchar_t folderPath[MAX_PATH * 2] = {0};
    wcscpy_s(folderPath, fullPath.c_str());
    PathRemoveFileSpecW(folderPath);

    IServiceProvider* sp = nullptr;
    if (FAILED(wb->QueryInterface(IID_PPV_ARGS(&sp))) || !sp) {
        wb->Release();
        return;
    }

    IShellBrowser* psb = nullptr;
    HRESULT hr = sp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&psb));
    sp->Release();
    if (FAILED(hr) || !psb) {
        wb->Release();
        return;
    }

    IShellView* psv = nullptr;
    hr = psb->QueryActiveShellView(&psv);
    psb->Release();
    if (FAILED(hr) || !psv) {
        wb->Release();
        return;
    }

    IShellFolder* pParent = nullptr;
    PCUITEMID_CHILD pChild = nullptr;
    if (BindParentAndChildPIDL(fullPath, &pParent, &pChild)) {
        psv->SelectItem(pChild, SVSI_SELECT | SVSI_ENSUREVISIBLE | SVSI_DESELECTOTHERS | SVSI_EDIT);
        pParent->Release();
    }

    psv->Release();
    wb->Release();
}

// ----------------- New Folder Action -----------------
static void PerformNewFolderAction() {
    ComInit com;

    HWND target = GetTargetExplorerHWND();
    if (!target) {
        Wh_Log(L"[CtrlQ] No explorer window detected.");
        return;
    }

    const bool isShellWindow = target == GetShellWindow();

    std::wstring dir;
    if (!GetExplorerPathFromHWND(target, dir) || dir.empty()) {
        if (isShellWindow) dir = GetDesktopDir();
    }
    if (dir.empty()) {
        Wh_Log(L"[CtrlQ] Failed to resolve folder path.");
        return;
    }

    DWORD dirAttrs = GetFileAttributesW(dir.c_str());
    if (dirAttrs == INVALID_FILE_ATTRIBUTES || !(dirAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"[CtrlQ] Unsupported target path: %ls", dir.c_str());
        return;
    }
    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) dir.pop_back();

    std::wstring newFolder = MakeUniqueFolderName(dir);
    
    // Create the folder
    if (!CreateNewDirectory(newFolder)) {
        Wh_Log(L"[CtrlQ] Failed to create folder: %ls", newFolder.c_str());
        return;
    }

    // FIX: Notify the Shell that a new directory was made
    SHChangeNotify(SHCNE_MKDIR, SHCNF_PATH, newFolder.c_str(), nullptr);
    
    // FIX: Wait for the UI to catch up (prevents the crash)
    Sleep(150);

    SelectAndRenameInWindow(target, newFolder);
}

// ----------------- Hook thread -----------------
static volatile HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HHOOK g_lowLevelHook = nullptr;
static bool g_nKeyDown = false;

static DWORD WINAPI HookThread(void* pParameter);
static LRESULT CALLBACK LowLevelKeybdProc(int nCode, WPARAM wParam, LPARAM lParam);

static BOOL KeybdHook_Init() {
    if (g_hookThread) return TRUE;

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) return FALSE;

    HANDLE hThread = CreateThread(nullptr, 0, HookThread, readyEvent, CREATE_SUSPENDED, &g_hookThreadId);
    if (!hThread) {
        CloseHandle(readyEvent);
        return FALSE;
    }

    SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    ResumeThread(hThread);

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    if (!g_lowLevelHook) {
        Wh_Log(L"[CtrlQ] SetWindowsHookEx failed.");
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        g_hookThreadId = 0;
        return FALSE;
    }
    g_hookThread = hThread;
    return TRUE;
}

static void KeybdHook_Exit() {
    HANDLE hThread = (HANDLE)InterlockedExchangePointer((PVOID*)&g_hookThread, nullptr);
    if (!hThread) return;

    if (g_hookThreadId) PostThreadMessageW(g_hookThreadId, WM_APP, 0, 0);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    g_hookThreadId = 0;
    g_lowLevelHook = nullptr;
}

static DWORD WINAPI HookThread(void* pParameter) {
    HANDLE readyEvent = (HANDLE)pParameter;
    MSG msg;

    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create queue
    g_lowLevelHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeybdProc, HINST_THISCOMPONENT, 0);
    SetEvent(readyEvent);
    if (!g_lowLevelHook) return 0;

    while (true) {
        BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);
        if (bRet <= 0) break;

        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }
            if (msg.message == WM_APP + 1) {
                PerformNewFolderAction();
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

static LRESULT CALLBACK LowLevelKeybdProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const KBDLLHOOKSTRUCT* info = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        int hk, m1, m2;
        AcquireSRWLockShared(&g_settingsLock);
        hk = g_hotkey;
        m1 = g_modifier1;
        m2 = g_modifier2;
        ReleaseSRWLockShared(&g_settingsLock);

        if (info && isKeyUp) {
            g_nKeyDown = false;
        }

        if (isKeyDown && info && info->vkCode == hk && !g_nKeyDown) {
            if (CheckModifiers(m1, m2)) {
                HWND fg = GetForegroundWindow();
                DWORD fgPid = 0;
                if (fg) GetWindowThreadProcessId(fg, &fgPid);
                if (fg && (IsExplorerTopLevel(fg) || fg == GetShellWindow()) &&
                    fgPid == GetCurrentProcessId()) {
                    g_nKeyDown = true;
                    PostThreadMessageW(g_hookThreadId, WM_APP + 1, 0, 0);
                    return 1;  // swallow hotkey
                }
            }
        }
    }
    return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
}

// ----------------- Windhawk entry points -----------------
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    return KeybdHook_Init();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    KeybdHook_Exit();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
