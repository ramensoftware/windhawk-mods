// ==WindhawkMod==
// @id              explorer-ctrln-newfile
// @name            Explorer Ctrl+N to New File
// @name:zh-CN      用 ctrl+n 创建新文件
// @description     Swallow Ctrl+N in Explorer and create a new file in-place, including desktop.
// @version         1.0
// @author          lieyanbang
// @github          https://github.com/lieyanbang
// @homepage        https://lieyanbang.com/
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luuid -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

**English**

Intercepts Explorer’s **Ctrl+N** (open new window) and replaces it with creating an empty file in the current directory, then entering rename mode.

**How it works**

Uses Windhawk to inject a `WH_KEYBOARD_LL` low-level keyboard hook into explorer.exe to intercept Ctrl+N.
Since the desktop cannot navigate folders, files are created directly in the desktop path.

---

**中文说明**

拦截资源管理器的 **Ctrl+N**（新建窗口操作），改为在当前目录创建空文件并重命名。

**实现原理**

通过 Windhawk 在 explorer.exe 注入 `WH_KEYBOARD_LL` 低级键盘钩子拦截ctrl+n。
在桌面无法导航，因此直接在桌面路径生成文件。

*/
// ==/WindhawkModReadme==

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

static HWND HitTestExplorerByCursor() {
    POINT pt;
    GetCursorPos(&pt);
    HWND h = WindowFromPoint(pt);
    if (!h) return nullptr;
    HWND root = GetAncestor(h, GA_ROOT);
    if (IsExplorerTopLevel(root)) return root;
    for (HWND cur = h; cur; cur = GetParent(cur)) {
        if (IsExplorerTopLevel(cur)) return cur;
    }
    return nullptr;
}

static HWND GetTargetExplorerHWND() {
    if (HWND h = HitTestExplorerByCursor()) return h;
    HWND fg = GetForegroundWindow();
    if (IsExplorerTopLevel(fg)) return fg;
    if (HWND shell = GetShellWindow()) return shell;  // desktop fallback
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

static std::wstring MakeUniqueFilename(const std::wstring& dir) {
    auto join = [](const std::wstring& a, const std::wstring& b) {
        if (a.empty()) return b;
        if (a.back() == L'\\' || a.back() == L'/') return a + b;
        return a + L'\\' + b;
    };
    std::wstring base = L"file";
    std::wstring path = join(dir, base);
    if (!PathFileExistsW(path.c_str())) return path;
    for (int i = 2; i < 10000; ++i) {
        wchar_t buf[64];
        swprintf(buf, ARRAYSIZE(buf), L"file (%d)", i);
        path = join(dir, buf);
        if (!PathFileExistsW(path.c_str())) return path;
    }
    return join(dir, L"file_new");
}

static bool CreateEmptyFile(const std::wstring& fullPath) {
    HANDLE h = CreateFileW(fullPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    CloseHandle(h);
    return true;
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

// ----------------- New file action -----------------
static void PerformNewFileAction() {
    ComInit com;

    HWND target = GetTargetExplorerHWND();
    if (!target) {
        Wh_Log(L"[Ctrln] No explorer window detected.");
        return;
    }

    std::wstring dir;
    if (!GetExplorerPathFromHWND(target, dir) || dir.empty()) dir = GetDesktopDir();
    if (dir.empty()) {
        Wh_Log(L"[Ctrln] Failed to resolve folder path.");
        return;
    }
    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) dir.pop_back();

    std::wstring newFile = MakeUniqueFilename(dir);
    if (!CreateEmptyFile(newFile)) {
        Wh_Log(L"[Ctrln] Failed to create file: %ls", newFile.c_str());
        return;
    }

    SelectAndRenameInWindow(target, newFile);
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
        Wh_Log(L"[Ctrln] SetWindowsHookEx failed.");
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
                PerformNewFileAction();
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

        if (info && isKeyUp && info->vkCode == 'N') g_nKeyDown = false;
        if (info && isKeyUp && (info->vkCode == VK_LCONTROL || info->vkCode == VK_RCONTROL)) g_nKeyDown = false;

        if (isKeyDown && info && info->vkCode == 'N' && !g_nKeyDown) {
            const bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            if (ctrl && !shift) {
                HWND fg = GetForegroundWindow();
                if (IsExplorerTopLevel(fg) || fg == GetShellWindow()) {
                    g_nKeyDown = true;
                    PostThreadMessageW(g_hookThreadId, WM_APP + 1, 0, 0);
                    return 1;  // swallow Ctrl+N
                }
            }
        }
    }
    return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
}

// ----------------- Windhawk entry points -----------------
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    return KeybdHook_Init();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    KeybdHook_Exit();
}

void Wh_ModSettingsChanged() {
    // no settings
}
