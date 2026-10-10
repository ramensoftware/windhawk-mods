// ==WindhawkMod==
// @id           xandriw-explorer-select-interceptor
// @name         SameFolderOnly
// @description  When an app opens a folder or uses Show in folder, reuse an existing Explorer window for that folder and select the requested file instead of opening a duplicate window.
// @version      4.1.5
// @author       XandriW
// @github       https://github.com/xandri19wang
// @include      explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Purpose
- Source-level fix for "Show in folder", "Open downloads folder", and plain
  explorer.exe folder opens.
- Runs inside the new explorer.exe process before the duplicate window appears.

What it intercepts
- explorer.exe /select,"C:\Folder\File.ext"
- explorer.exe "C:\Folder"
- explorer.exe C:\Folder
- explorer.exe shell:Downloads
- explorer.exe /e,C:\Folder or /root,C:\Folder style commands when possible

Behavior
1. If a matching folder is already open:
   - Select the requested file if /select was used.
   - Activate that existing Explorer window if enabled.
   - Exit this helper explorer.exe before it creates a duplicate window.
2. If no matching folder is visible through ShellWindows:
   - By default, do NOT steal a random Explorer window.
   - Optional fallback can reuse another Explorer window if you explicitly enable it.
3. If no usable Explorer window is found:
   - Let Explorer continue normally.

Windows 11 tabs
- ShellWindows can enumerate inactive tabs, which share the window handle of
  their parent Explorer window. This mod checks each tab's Shell view visibility
  and only reuses an active tab, so a request is not swallowed by a hidden tab.
- If the target folder is only in a background tab, SameFolderOnly lets Explorer
  handle the request normally (it does not automatically switch to that tab).
- The optional ReuseAnyExplorerWindow fallback also skips inactive tabs.

Hold Shift (or your configured bypass key) to allow Explorer's normal behavior.
Requests not launched through a supported explorer.exe command line are unaffected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- InterceptSelect: true
  $name: Intercept explorer.exe /select,file

- InterceptFolderOpen: true
  $name: Intercept explorer.exe folder opens

- FallbackMode: SameFolderOnly
  $name: What to do if no same-folder Explorer window is detected
  $options:
  - SameFolderOnly: Do not reuse a random Explorer window
  - ReuseAnyExplorerWindow: Reuse any existing Explorer window

- SelectAfterNavigateDelayMs: 750
  $name: Delay after navigating before selecting file (ms)
  $description: Only used with ReuseAnyExplorerWindow fallback mode when navigating to a different folder before selecting a file.

- BypassModifier: Shift
  $name: Bypass key
  $options:
  - None: Never bypass
  - Shift: Hold Shift to bypass
  - Ctrl: Hold Ctrl to bypass
  - Alt: Hold Alt to bypass

- ActivateExistingWindow: true
  $name: Bring the existing Explorer window to front

- SuppressNewWindowIfSelectionFails: false
  $name: Still suppress new window if selecting the file fails
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <servprov.h>
#include <exdisp.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

// ---------------- Settings ----------------
static bool gInterceptSelect = true;
static bool gInterceptFolderOpen = true;
static int  gFallbackMode = 0; // 0=SameFolderOnly, 1=ReuseAnyExplorerWindow
static int  gSelectAfterNavigateDelayMs = 750;
static int  gBypassModifier = 1; // 0=None, 1=Shift, 2=Ctrl, 3=Alt
static bool gActivateExistingWindow = true;
static bool gSuppressNewWindowIfSelectionFails = false;

// ---------------- Dynamic COM / OleAut / Shell32 ----------------
static HMODULE gOle32    = nullptr;
static HMODULE gOleAut32 = nullptr;
static HMODULE gShell32  = nullptr;

typedef HRESULT (WINAPI *PFN_CoInitializeEx)(LPVOID, DWORD);
typedef void    (WINAPI *PFN_CoUninitialize)(void);
typedef HRESULT (WINAPI *PFN_CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
typedef BSTR    (WINAPI *PFN_SysAllocString)(const OLECHAR*);
typedef UINT    (WINAPI *PFN_SysStringLen)(BSTR);
typedef void    (WINAPI *PFN_SysFreeString)(BSTR);
typedef HRESULT (WINAPI *PFN_SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR*);
typedef void    (WINAPI *PFN_CoTaskMemFree)(LPVOID);

static PFN_CoInitializeEx       pCoInitializeEx       = nullptr;
static PFN_CoUninitialize       pCoUninitialize       = nullptr;
static PFN_CoCreateInstance     pCoCreateInstance     = nullptr;
static PFN_SysAllocString       pSysAllocString       = nullptr;
static PFN_SysStringLen         pSysStringLen         = nullptr;
static PFN_SysFreeString        pSysFreeString        = nullptr;
static PFN_SHGetKnownFolderPath pSHGetKnownFolderPath = nullptr;
static PFN_CoTaskMemFree        pCoTaskMemFree        = nullptr;

static void LoadComProcs() {
    if (!gOle32)    gOle32    = LoadLibraryW(L"ole32.dll");
    if (!gOleAut32) gOleAut32 = LoadLibraryW(L"oleaut32.dll");
    if (!gShell32)  gShell32  = LoadLibraryW(L"shell32.dll");

    if (gOle32) {
        pCoInitializeEx       = (PFN_CoInitializeEx)      GetProcAddress(gOle32,   "CoInitializeEx");
        pCoUninitialize       = (PFN_CoUninitialize)      GetProcAddress(gOle32,   "CoUninitialize");
        pCoCreateInstance     = (PFN_CoCreateInstance)    GetProcAddress(gOle32,   "CoCreateInstance");
        pCoTaskMemFree        = (PFN_CoTaskMemFree)       GetProcAddress(gOle32,   "CoTaskMemFree");
    }
    if (gOleAut32) {
        pSysAllocString       = (PFN_SysAllocString)      GetProcAddress(gOleAut32,"SysAllocString");
        pSysStringLen         = (PFN_SysStringLen)        GetProcAddress(gOleAut32,"SysStringLen");
        pSysFreeString        = (PFN_SysFreeString)       GetProcAddress(gOleAut32,"SysFreeString");
    }
    if (gShell32) {
        pSHGetKnownFolderPath = (PFN_SHGetKnownFolderPath)GetProcAddress(gShell32, "SHGetKnownFolderPath");
    }
}

// ---------------- Inline GUIDs, no uuid.lib needed ----------------
static const CLSID MY_CLSID_ShellWindows =
{ 0x9ba05972, 0xf6a8, 0x11cf, {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39} };

static const IID MY_IID_IShellWindows =
{ 0x85cb6900, 0x4d95, 0x11cf, {0x96, 0x0c, 0x00, 0x80, 0xc7, 0xf4, 0xee, 0x85} };

static const IID MY_IID_IWebBrowser2 =
{ 0xd30c1661, 0xcdaf, 0x11d0, {0x8a, 0x3e, 0x00, 0xc0, 0x4f, 0xc9, 0xe2, 0x6e} };

// Same shell interfaces and service GUID used by quick-explorer-switcher.
// Inline constants keep this build independent of an additional uuid library.
static const IID MY_IID_IServiceProvider =
{ 0x6d5140c1, 0x7436, 0x11ce, {0x80, 0x34, 0x00, 0xaa, 0x00, 0x60, 0x09, 0xfa} };
static const IID MY_IID_IShellBrowser =
{ 0x000214e2, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} };
static const GUID MY_SID_STopLevelBrowser =
{ 0x4c96be40, 0x915c, 0x11cf, {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37} };

static const IID MY_IID_NULL =
{ 0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };

static const GUID MY_FOLDERID_Downloads =
{ 0x374de290, 0x123f, 0x4565, {0x91, 0x64, 0x39, 0xc4, 0x92, 0x5e, 0x46, 0x7b} };

// ---------------- General helpers ----------------
static int ClampInt(int value, int lo, int hi) {
    return std::max(lo, std::min(hi, value));
}

static bool IsBypassHeld() {
    int vk = 0;
    switch (gBypassModifier) {
        case 1: vk = VK_SHIFT;   break;
        case 2: vk = VK_CONTROL; break;
        case 3: vk = VK_MENU;    break;
        default: return false;
    }
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) {
        return (wchar_t)towlower(c);
    });
    return s;
}

static void TrimInPlace(std::wstring& s) {
    while (!s.empty() && iswspace(s.front()))
        s.erase(s.begin());
    while (!s.empty() && iswspace(s.back()))
        s.pop_back();
}

static void UnquoteInPlace(std::wstring& s) {
    TrimInPlace(s);
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"')
        s = s.substr(1, s.size() - 2);
    TrimInPlace(s);
}

static bool IsDriveRoot(const std::wstring& p) {
    return p.size() == 3 && p[1] == L':' && (p[2] == L'\\' || p[2] == L'/');
}

static void NormalizePathInPlace(std::wstring& path) {
    std::replace(path.begin(), path.end(), L'/', L'\\');

    std::transform(path.begin(), path.end(), path.begin(), [](wchar_t c) {
        return (wchar_t)towlower(c);
    });

    if (!path.empty() && !IsDriveRoot(path)) {
        while (!path.empty() && (path.back() == L'\\' || path.back() == L'/')) {
            path.pop_back();
        }
    }
}

static std::wstring NormalizePathCopy(std::wstring path) {
    NormalizePathInPlace(path);
    return path;
}

static bool IsDirectoryPath(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static std::wstring ParentFolderOf(const std::wstring& fullPath) {
    std::wstring p = fullPath;
    std::replace(p.begin(), p.end(), L'/', L'\\');

    size_t pos = p.find_last_of(L'\\');
    if (pos == std::wstring::npos)
        return L"";

    if (pos == 2 && p.size() >= 3 && p[1] == L':')
        return p.substr(0, 3); // Drive root

    return p.substr(0, pos);
}

static std::wstring FileNameFromPath(const std::wstring& fullPath) {
    size_t pos = fullPath.find_last_of(L"\\/");
    if (pos == std::wstring::npos)
        return fullPath;
    return fullPath.substr(pos + 1);
}

static std::wstring UrlPercentDecode(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size());

    auto hex = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };

    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == L'%' && i + 2 < s.size()) {
            int a = hex(s[i + 1]);
            int b = hex(s[i + 2]);
            if (a >= 0 && b >= 0) {
                out.push_back((wchar_t)((a << 4) | b));
                i += 2;
                continue;
            }
        }
        out.push_back(s[i]);
    }

    return out;
}

static std::wstring BstrToWString(BSTR b) {
    if (!b)
        return L"";
    if (pSysStringLen) {
        UINT n = pSysStringLen(b);
        return std::wstring(b, b + n);
    }
    return std::wstring(b);
}

static std::wstring GetDownloadsPhysicalPath() {
    if (!pSHGetKnownFolderPath || !pCoTaskMemFree)
        return L"";

    PWSTR w = nullptr;
    if (SUCCEEDED(pSHGetKnownFolderPath(MY_FOLDERID_Downloads, 0, nullptr, &w)) && w) {
        std::wstring out(w);
        pCoTaskMemFree(w);
        NormalizePathInPlace(out);
        return out;
    }

    return L"";
}

static std::wstring ResolveSpecialFolder(std::wstring folder) {
    std::wstring low = ToLower(folder);
    UnquoteInPlace(low);

    if (low == L"shell:downloads" ||
        low == L"shell:downloadsfolder" ||
        low.find(L"374de290-123f-4565-9164-39c4925e467b") != std::wstring::npos) {
        std::wstring downloads = GetDownloadsPhysicalPath();
        if (!downloads.empty())
            return downloads;
    }

    return folder;
}

static std::wstring NormalizeFromLocationUrl(BSTR burl) {
    std::wstring u = BstrToWString(burl);
    if (u.empty())
        return L"";

    std::wstring low = ToLower(u);

    const std::wstring filePrefix = L"file:///";
    if (low.rfind(filePrefix, 0) == 0) {
        std::wstring path = UrlPercentDecode(u.substr(filePrefix.size()));
        NormalizePathInPlace(path);
        return path;
    }

    if (low.rfind(L"shell:downloads", 0) == 0 ||
        low.find(L"374de290-123f-4565-9164-39c4925e467b") != std::wstring::npos) {
        return GetDownloadsPhysicalPath();
    }

    return L"";
}

static void ActivateWindow(HWND hwnd) {
    if (!gActivateExistingWindow || !IsWindow(hwnd))
        return;

    if (IsIconic(hwnd))
        ShowWindow(hwnd, SW_RESTORE);

    // A maximized window must stay maximized.
    SetForegroundWindow(hwnd);
}

static BOOL CALLBACK EnumExplorerWindowProc(HWND hwnd, LPARAM lParam) {
    wchar_t className[128] = {0};
    GetClassNameW(hwnd, className, 128);

    if (_wcsicmp(className, L"CabinetWClass") == 0 ||
        _wcsicmp(className, L"ExploreWClass") == 0) {
        BOOL* found = (BOOL*)lParam;
        *found = TRUE;
        return FALSE;
    }

    return TRUE;
}

static bool HasAnyExplorerWindow() {
    BOOL found = FALSE;
    EnumWindows(EnumExplorerWindowProc, (LPARAM)&found);
    return found != FALSE;
}

// Wh_ModInit may also run in an existing Explorer process when the mod
// is enabled or updated. Count visible windows only: fresh helpers may
// already own hidden COM, IME or third-party mod windows.
static BOOL CALLBACK OwnsWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hwnd, &ownerPid);
    if (ownerPid == GetCurrentProcessId() && IsWindowVisible(hwnd)) {
        *reinterpret_cast<bool*>(lParam) = true;
        return FALSE;
    }
    return TRUE;
}

static bool OwnsTopLevelWindow() {
    bool ownsWindow = false;
    EnumWindows(OwnsWindowProc, reinterpret_cast<LPARAM>(&ownsWindow));
    return ownsWindow;
}


// ---------------- Command-line parser ----------------
struct OpenRequest {
    std::wstring folder;
    std::wstring fileToSelect; // empty for plain folder open
};

static bool SplitFirstCommandArg(const std::wstring& cmd, std::wstring& first, std::wstring& rest) {
    first.clear();
    rest.clear();

    size_t i = 0;
    while (i < cmd.size() && iswspace(cmd[i]))
        i++;

    if (i >= cmd.size())
        return false;

    if (cmd[i] == L'"') {
        size_t end = cmd.find(L'"', i + 1);
        if (end == std::wstring::npos)
            return false;

        first = cmd.substr(i + 1, end - i - 1);
        rest = cmd.substr(end + 1);
    } else {
        size_t end = i;
        while (end < cmd.size() && !iswspace(cmd[end]))
            end++;

        first = cmd.substr(i, end - i);
        rest = cmd.substr(end);
    }

    TrimInPlace(rest);
    return !first.empty();
}

static std::wstring ExtractSelectTarget(const std::wstring& args) {
    std::wstring low = ToLower(args);
    size_t pos = low.find(L"/select");
    if (pos == std::wstring::npos)
        pos = low.find(L"-select");
    if (pos == std::wstring::npos)
        return L"";

    size_t comma = args.find(L',', pos);
    if (comma == std::wstring::npos)
        return L"";

    size_t start = comma + 1;
    while (start < args.size() && iswspace(args[start]))
        start++;

    std::wstring target;

    if (start < args.size() && args[start] == L'"') {
        size_t end = args.find(L'"', start + 1);
        if (end == std::wstring::npos)
            return L"";
        target = args.substr(start + 1, end - start - 1);
    } else {
        target = args.substr(start);
        TrimInPlace(target);

        size_t sw = target.find(L" /");
        if (sw != std::wstring::npos)
            target = target.substr(0, sw);
        sw = target.find(L" -");
        if (sw != std::wstring::npos)
            target = target.substr(0, sw);
    }

    UnquoteInPlace(target);
    std::replace(target.begin(), target.end(), L'/', L'\\');
    return target;
}

static std::wstring ExtractFolderTargetFromArgs(std::wstring args) {
    TrimInPlace(args);
    if (args.empty())
        return L"";

    // Remove common explorer switches which may precede a comma path.
    std::wstring low = ToLower(args);

    // /e,C:\Folder  /root,C:\Folder  /n,C:\Folder
    size_t comma = args.find(L',');
    if (comma != std::wstring::npos) {
        std::wstring left = ToLower(args.substr(0, comma));
        TrimInPlace(left);
        if (left.find(L"/e") != std::wstring::npos ||
            left.find(L"-e") != std::wstring::npos ||
            left.find(L"/n") != std::wstring::npos ||
            left.find(L"-n") != std::wstring::npos ||
            left.find(L"/root") != std::wstring::npos ||
            left.find(L"-root") != std::wstring::npos) {
            std::wstring after = args.substr(comma + 1);
            TrimInPlace(after);
            UnquoteInPlace(after);
            if (!after.empty())
                return after;
        }
    }

    // If args start with a pure switch, ignore unless we handled comma form above.
    if (!args.empty() && (args[0] == L'/' || args[0] == L'-')) {
        return L"";
    }

    std::wstring first, rest;
    if (!SplitFirstCommandArg(args, first, rest))
        return L"";

    UnquoteInPlace(first);

    if (first.empty())
        return L"";

    std::wstring firstLow = ToLower(first);
    if (firstLow.rfind(L"shell:", 0) == 0)
        return first;

    if (IsDirectoryPath(first))
        return first;

    return L"";
}

static bool ParseExplorerCommandLine(OpenRequest& req) {
    req.folder.clear();
    req.fileToSelect.clear();

    std::wstring cmd = GetCommandLineW();
    std::wstring exe;
    std::wstring args;

    if (!SplitFirstCommandArg(cmd, exe, args))
        return false;

    if (gInterceptSelect) {
        std::wstring selectTarget = ExtractSelectTarget(args);
        if (!selectTarget.empty()) {
            std::wstring folder = ParentFolderOf(selectTarget);
            if (!folder.empty()) {
                req.fileToSelect = selectTarget;
                req.folder = folder;
                return true;
            }
        }
    }

    if (gInterceptFolderOpen) {
        std::wstring folder = ExtractFolderTargetFromArgs(args);
        if (!folder.empty()) {
            req.folder = folder;
            return true;
        }
    }

    return false;
}

// ---------------- IDispatch helpers ----------------
static void ClearVariantSafe(VARIANT& v) {
    if (v.vt == VT_BSTR && v.bstrVal && pSysFreeString) {
        pSysFreeString(v.bstrVal);
    } else if (v.vt == VT_DISPATCH && v.pdispVal) {
        v.pdispVal->Release();
    } else if (v.vt == VT_UNKNOWN && v.punkVal) {
        v.punkVal->Release();
    }
    ZeroMemory(&v, sizeof(v));
    v.vt = VT_EMPTY;
}

static bool GetDispId(IDispatch* disp, PCWSTR name, DISPID* id) {
    if (!disp || !name || !id)
        return false;

    LPOLESTR names[1];
    names[0] = const_cast<LPOLESTR>(name);
    return SUCCEEDED(disp->GetIDsOfNames(MY_IID_NULL, names, 1, LOCALE_USER_DEFAULT, id));
}

static bool InvokeDispatch(IDispatch* disp, PCWSTR name, WORD flags,
                           VARIANT* args, int argc, VARIANT* result) {
    if (!disp)
        return false;

    DISPID id;
    if (!GetDispId(disp, name, &id))
        return false;

    DISPPARAMS dp;
    ZeroMemory(&dp, sizeof(dp));
    dp.rgvarg = args;
    dp.cArgs = argc;

    HRESULT hr = disp->Invoke(id, MY_IID_NULL, LOCALE_USER_DEFAULT, flags, &dp, result, nullptr, nullptr);
    return SUCCEEDED(hr);
}

static IDispatch* GetDocumentDispatch(IWebBrowser2* browser) {
    if (!browser)
        return nullptr;

    IDispatch* doc = nullptr;
    if (SUCCEEDED(browser->get_Document(&doc)) && doc)
        return doc;

    return nullptr;
}

static bool SelectFileInBrowser(IWebBrowser2* browser, const std::wstring& fullFilePath) {
    if (!browser || fullFilePath.empty() || !pSysAllocString || !pSysFreeString)
        return false;

    std::wstring fileName = FileNameFromPath(fullFilePath);
    if (fileName.empty())
        return false;

    IDispatch* doc = GetDocumentDispatch(browser);
    if (!doc)
        return false;

    bool ok = false;

    VARIANT folderVar;
    ZeroMemory(&folderVar, sizeof(folderVar));
    folderVar.vt = VT_EMPTY;

    if (InvokeDispatch(doc, L"Folder", DISPATCH_PROPERTYGET, nullptr, 0, &folderVar) &&
        folderVar.vt == VT_DISPATCH && folderVar.pdispVal) {

        BSTR nameBstr = pSysAllocString(fileName.c_str());
        if (nameBstr) {
            VARIANT parseArg;
            ZeroMemory(&parseArg, sizeof(parseArg));
            parseArg.vt = VT_BSTR;
            parseArg.bstrVal = nameBstr;

            VARIANT itemVar;
            ZeroMemory(&itemVar, sizeof(itemVar));
            itemVar.vt = VT_EMPTY;

            if (InvokeDispatch(folderVar.pdispVal, L"ParseName", DISPATCH_METHOD, &parseArg, 1, &itemVar) &&
                itemVar.vt == VT_DISPATCH && itemVar.pdispVal) {

                // ShellFolderView.SelectItem(item, flags)
                // DISPPARAMS arguments are reversed.
                VARIANT args[2];
                ZeroMemory(args, sizeof(args));

                args[0].vt = VT_I4;
                args[0].lVal = 0x1 | 0x4 | 0x8 | 0x10; // select + deselect others + ensure visible + focused

                args[1].vt = VT_DISPATCH;
                args[1].pdispVal = itemVar.pdispVal;
                args[1].pdispVal->AddRef();

                VARIANT result;
                ZeroMemory(&result, sizeof(result));
                result.vt = VT_EMPTY;

                ok = InvokeDispatch(doc, L"SelectItem", DISPATCH_METHOD, args, 2, &result);

                ClearVariantSafe(result);
                ClearVariantSafe(args[1]);
            }

            ClearVariantSafe(itemVar);
            ClearVariantSafe(parseArg);
        }
    }

    ClearVariantSafe(folderVar);
    doc->Release();

    return ok;
}

// ---------------- Explorer COM actions ----------------
// IShellWindows can include background tabs; their view HWNDs are hidden.
// Do not consider such entries for window reuse.
static bool IsVisibleExplorerTab(IDispatch* disp) {
    if (!disp)
        return false;

    // Preserve normal behavior if a shell implementation does not expose
    // the service; when available, use its actual view visibility.
    bool active = true;
    IServiceProvider* provider = nullptr;
    if (SUCCEEDED(disp->QueryInterface(MY_IID_IServiceProvider,
                                      reinterpret_cast<void**>(&provider))) &&
        provider) {
        IShellBrowser* shellBrowser = nullptr;
        if (SUCCEEDED(provider->QueryService(MY_SID_STopLevelBrowser,
                                             MY_IID_IShellBrowser,
                                             reinterpret_cast<void**>(&shellBrowser))) &&
            shellBrowser) {
            IShellView* view = nullptr;
            if (SUCCEEDED(shellBrowser->QueryActiveShellView(&view)) && view) {
                HWND viewHwnd = nullptr;
                if (SUCCEEDED(view->GetWindow(&viewHwnd)) && viewHwnd)
                    active = IsWindowVisible(viewHwnd) != FALSE;
                view->Release();
            }
            shellBrowser->Release();
        }
        provider->Release();
    }
    return active;
}

struct ExplorerBrowser {
    IWebBrowser2* browser = nullptr;
    HWND hwnd = nullptr;
    std::wstring path;
};

static void ReleaseBrowsers(std::vector<ExplorerBrowser>& browsers) {
    for (auto& b : browsers) {
        if (b.browser)
            b.browser->Release();
    }
    browsers.clear();
}

static bool EnumerateExplorerBrowsers(std::vector<ExplorerBrowser>& browsers) {
    if (!pCoCreateInstance)
        return false;

    IShellWindows* psw = nullptr;
    if (FAILED(pCoCreateInstance(MY_CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                 MY_IID_IShellWindows, (void**)&psw)) || !psw) {
        return false;
    }

    VARIANT v;
    ZeroMemory(&v, sizeof(v));
    v.vt = VT_I4;

    long count = 0;
    if (SUCCEEDED(psw->get_Count(&count))) {
        for (long i = 0; i < count; ++i) {
            v.lVal = i;

            IDispatch* pDisp = nullptr;
            if (S_OK != psw->Item(v, &pDisp) || !pDisp)
                continue;

            // A background tab has the same top-level HWND as its active tab.
            // Its hidden shell view must not swallow a folder-open request.
            if (!IsVisibleExplorerTab(pDisp)) {
                pDisp->Release();
                continue;
            }

            IWebBrowser2* pWB = nullptr;
            if (SUCCEEDED(pDisp->QueryInterface(MY_IID_IWebBrowser2, (void**)&pWB)) && pWB) {
                LONG_PTR raw = 0;
                HWND hwnd = nullptr;
                if (SUCCEEDED(pWB->get_HWND(&raw)))
                    hwnd = (HWND)raw;

                BSTR url = nullptr;
                std::wstring norm;
                if (SUCCEEDED(pWB->get_LocationURL(&url))) {
                    norm = NormalizeFromLocationUrl(url);
                    if (pSysFreeString)
                        pSysFreeString(url);
                }

                if (!norm.empty()) {
                    browsers.push_back(ExplorerBrowser{ pWB, hwnd, norm });
                    pWB = nullptr;
                }

                if (pWB)
                    pWB->Release();
            }

            pDisp->Release();
        }
    }

    psw->Release();
    return true;
}

static bool NavigateBrowserToFolder(IWebBrowser2* browser, const std::wstring& folder) {
    if (!browser || !pSysAllocString || !pSysFreeString)
        return false;

    BSTR url = pSysAllocString(folder.c_str());
    if (!url)
        return false;

    VARIANT empty;
    ZeroMemory(&empty, sizeof(empty));
    empty.vt = VT_EMPTY;

    HRESULT hr = browser->Navigate(url, &empty, &empty, &empty, &empty);
    pSysFreeString(url);
    return SUCCEEDED(hr);
}

static bool TryReuseExistingExplorer(const OpenRequest& request) {
    if (request.folder.empty() || !pCoInitializeEx || !pCoCreateInstance || !pCoUninitialize)
        return false;

    std::wstring targetFolder = ResolveSpecialFolder(request.folder);
    UnquoteInPlace(targetFolder);

    if (targetFolder.empty())
        return false;

    std::wstring targetFolderNorm = NormalizePathCopy(targetFolder);

    HRESULT hr = pCoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool coInitialized = SUCCEEDED(hr);
    if (!coInitialized && hr != RPC_E_CHANGED_MODE)
        return false;

    bool handled = false;

    std::vector<ExplorerBrowser> browsers;
    EnumerateExplorerBrowsers(browsers);

    ExplorerBrowser* sameFolder = nullptr;
    ExplorerBrowser* anyFolder = nullptr;

    for (auto& b : browsers) {
        if (!b.browser)
            continue;

        if (!anyFolder)
            anyFolder = &b;

        if (b.path == targetFolderNorm) {
            sameFolder = &b;
            break;
        }
    }

    ExplorerBrowser* chosen = sameFolder;

    if (!chosen && gFallbackMode == 1)
        chosen = anyFolder;

    if (chosen && chosen->browser) {
        if (!sameFolder) {
            if (!NavigateBrowserToFolder(chosen->browser, targetFolder)) {
                ReleaseBrowsers(browsers);
                if (coInitialized)
                    pCoUninitialize();
                return false;
            }

            if (gSelectAfterNavigateDelayMs > 0)
                Sleep((DWORD)gSelectAfterNavigateDelayMs);
        }

        bool selectedOk = true;
        if (!request.fileToSelect.empty())
            selectedOk = SelectFileInBrowser(chosen->browser, request.fileToSelect);

        if (selectedOk || request.fileToSelect.empty() || gSuppressNewWindowIfSelectionFails) {
            ActivateWindow(chosen->hwnd);
            handled = true;
        }
    }

    ReleaseBrowsers(browsers);

    if (coInitialized)
        pCoUninitialize();

    return handled;
}

// ---------------- Settings ----------------
static void LoadSettings() {
    gInterceptSelect = Wh_GetIntSetting(L"InterceptSelect") != 0;
    gInterceptFolderOpen = Wh_GetIntSetting(L"InterceptFolderOpen") != 0;
    if (PCWSTR fb = Wh_GetStringSetting(L"FallbackMode")) {
        if (_wcsicmp(fb, L"ReuseAnyExplorerWindow") == 0)
            gFallbackMode = 1;
        else
            gFallbackMode = 0;
        Wh_FreeStringSetting(const_cast<PWSTR>(fb));
    } else {
        gFallbackMode = 0;
    }

    gSelectAfterNavigateDelayMs = ClampInt(Wh_GetIntSetting(L"SelectAfterNavigateDelayMs"), 0, 5000);

    gActivateExistingWindow = Wh_GetIntSetting(L"ActivateExistingWindow") != 0;
    gSuppressNewWindowIfSelectionFails = Wh_GetIntSetting(L"SuppressNewWindowIfSelectionFails") != 0;

    if (PCWSTR s = Wh_GetStringSetting(L"BypassModifier")) {
        if      (_wcsicmp(s, L"None")  == 0) gBypassModifier = 0;
        else if (_wcsicmp(s, L"Shift") == 0) gBypassModifier = 1;
        else if (_wcsicmp(s, L"Ctrl")  == 0) gBypassModifier = 2;
        else if (_wcsicmp(s, L"Alt")   == 0) gBypassModifier = 3;
        Wh_FreeStringSetting(const_cast<PWSTR>(s));
    }
}

// ---------------- Windhawk entry ----------------
BOOL Wh_ModInit() {
    LoadSettings();

    if (IsBypassHeld())
        return TRUE;

    OpenRequest request;
    if (!ParseExplorerCommandLine(request))
        return TRUE;

    // Only a new, windowless explorer.exe helper may intercept and exit.
    // Mod updates can invoke Wh_ModInit inside existing Explorer processes.
    if (OwnsTopLevelWindow())
        return TRUE;

    // First Explorer window after boot has nothing to reuse.
    // Avoid loading COM / ShellWindows here to reduce conflicts with visual Explorer mods.
    if (!HasAnyExplorerWindow())
        return TRUE;

    LoadComProcs();

    // The initial visible-window check protects long-lived Explorer hosts.
    // Do not re-check here: hidden windows may appear during COM operations.
    if (TryReuseExistingExplorer(request)) {
        ExitProcess(0);
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (gOle32)    { FreeLibrary(gOle32);    gOle32 = nullptr; }
    if (gOleAut32) { FreeLibrary(gOleAut32); gOleAut32 = nullptr; }
    if (gShell32)  { FreeLibrary(gShell32);  gShell32 = nullptr; }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
