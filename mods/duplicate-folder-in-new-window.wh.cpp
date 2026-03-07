// ==WindhawkMod==
// @id              duplicate-folder-in-new-window
// @name            Duplicate folder in new window
// @description     Adds "复制并打开" to File Explorer folder context menu and opens a duplicate Explorer window.
// @version         3.0
// @author          Neko_RTHsama
// @github          https://github.com/RaTaiHok
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds a custom command to File Explorer folder right-click menus:
- 复制并打开

When clicked, a new Explorer window opens at the same folder path as the
current Explorer folder window.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableContextMenuItem: true
  $name: Enable context menu item
  $description: Add "复制并打开" to Explorer folder context menus.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <exdisp.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <shlwapi.h>
#include <string>

constexpr UINT kCopyAndOpenCommandId = 0x3EF0;
constexpr const wchar_t* kMenuText = L"复制并打开";

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
using TrackPopupMenu_t = decltype(&TrackPopupMenu);
using SendMessageW_t = decltype(&SendMessageW);
using PostMessageW_t = decltype(&PostMessageW);
using DispatchMessageW_t = decltype(&DispatchMessageW);

TrackPopupMenuEx_t TrackPopupMenuEx_Original;
TrackPopupMenu_t TrackPopupMenu_Original;
SendMessageW_t SendMessageW_Original;
PostMessageW_t PostMessageW_Original;
DispatchMessageW_t DispatchMessageW_Original;

struct {
    bool enableContextMenuItem;
} g_settings;

DWORD g_lastExecuteTick = 0;
std::wstring g_pendingSelectedFolderPath;
DWORD g_pendingSelectedFolderTick = 0;

void LoadSettings() {
    g_settings.enableContextMenuItem = Wh_GetIntSetting(L"enableContextMenuItem");
}

bool IsExplorerFolderWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    wchar_t className[64] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return wcscmp(className, L"CabinetWClass") == 0 ||
           wcscmp(className, L"ExploreWClass") == 0;
}

bool IsOwnedByExplorerFolderWindow(HWND owner) {
    if (!owner) {
        return false;
    }

    HWND root = GetAncestor(owner, GA_ROOT);
    if (!root) {
        root = owner;
    }

    return IsExplorerFolderWindow(root);
}

bool DecodeLocationUrlToPath(BSTR locationUrl, std::wstring& outPath) {
    if (!locationUrl || !locationUrl[0]) {
        return false;
    }

    DWORD cch = 32768;
    std::wstring path(cch, L'\0');
    HRESULT hr = PathCreateFromUrlW(locationUrl, path.data(), &cch, 0);
    if (FAILED(hr) || cch == 0) {
        return false;
    }

    path.resize(cch);
    while (!path.empty() && path.back() == L'\0') {
        path.pop_back();
    }

    if (path.empty()) {
        return false;
    }

    outPath = path;
    return true;
}

bool GetSelectedFolderPathFromForegroundExplorer(std::wstring& outPath,
                                                 bool* outHasSelection) {
    if (outHasSelection) {
        *outHasSelection = false;
    }

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool canUseCom = SUCCEEDED(hrCo) || hrCo == RPC_E_CHANGED_MODE;
    bool uninit = SUCCEEDED(hrCo);

    if (!canUseCom) {
        return false;
    }

    CLSID clsidShellWindows{};
    HRESULT hr = CLSIDFromString(
        L"{9BA05972-F6A8-11CF-A442-00A0C90A8F39}", &clsidShellWindows);
    if (FAILED(hr)) {
        if (uninit) CoUninitialize();
        return false;
    }

    IShellWindows* shellWindows = nullptr;
    hr = CoCreateInstance(clsidShellWindows, nullptr, CLSCTX_ALL,
                          IID_PPV_ARGS(&shellWindows));
    if (FAILED(hr)) {
        if (uninit) CoUninitialize();
        return false;
    }

    HWND fg = GetForegroundWindow();
    HWND fgRoot = fg ? GetAncestor(fg, GA_ROOT) : nullptr;
    if (!fgRoot) {
        shellWindows->Release();
        if (uninit) CoUninitialize();
        return false;
    }

    long count = 0;
    hr = shellWindows->get_Count(&count);
    if (FAILED(hr) || count <= 0) {
        shellWindows->Release();
        if (uninit) CoUninitialize();
        return false;
    }

    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* dispatch = nullptr;
        hr = shellWindows->Item(index, &dispatch);
        VariantClear(&index);
        if (FAILED(hr) || !dispatch) {
            continue;
        }

        IWebBrowserApp* webBrowser = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&webBrowser));
        if (FAILED(hr) || !webBrowser) {
            dispatch->Release();
            continue;
        }

        SHANDLE_PTR browserHwndRaw = 0;
        hr = webBrowser->get_HWND(&browserHwndRaw);
        HWND browserHwnd = (HWND)(LONG_PTR)browserHwndRaw;
        if (FAILED(hr) || browserHwnd != fgRoot || !IsExplorerFolderWindow(browserHwnd)) {
            webBrowser->Release();
            dispatch->Release();
            continue;
        }

        IServiceProvider* sp = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&sp));
        dispatch->Release();
        webBrowser->Release();
        if (FAILED(hr) || !sp) {
            continue;
        }

        IShellBrowser* shellBrowser = nullptr;
        hr = sp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&shellBrowser));
        sp->Release();
        if (FAILED(hr) || !shellBrowser) {
            continue;
        }

        IShellView* shellView = nullptr;
        hr = shellBrowser->QueryActiveShellView(&shellView);
        shellBrowser->Release();
        if (FAILED(hr) || !shellView) {
            continue;
        }

        IFolderView2* folderView2 = nullptr;
        hr = shellView->QueryInterface(IID_PPV_ARGS(&folderView2));
        shellView->Release();
        if (FAILED(hr) || !folderView2) {
            continue;
        }

        IShellItemArray* selection = nullptr;
        hr = folderView2->GetSelection(FALSE, &selection);
        folderView2->Release();
        if (FAILED(hr) || !selection) {
            continue;
        }

        DWORD selCount = 0;
        hr = selection->GetCount(&selCount);
        if (FAILED(hr)) {
            selection->Release();
            continue;
        }

        if (outHasSelection) {
            *outHasSelection = (selCount > 0);
        }

        if (selCount != 1) {
            selection->Release();
            if (selCount == 0) {
                shellWindows->Release();
                if (uninit) CoUninitialize();
                return false;
            }
            shellWindows->Release();
            if (uninit) CoUninitialize();
            return false;
        }

        IShellItem* item = nullptr;
        hr = selection->GetItemAt(0, &item);
        selection->Release();
        if (FAILED(hr) || !item) {
            continue;
        }

        SFGAOF attrs = SFGAO_FOLDER;
        hr = item->GetAttributes(SFGAO_FOLDER, &attrs);
        if (FAILED(hr) || (attrs & SFGAO_FOLDER) == 0) {
            item->Release();
            shellWindows->Release();
            if (uninit) CoUninitialize();
            return false;
        }

        PWSTR folderPath = nullptr;
        hr = item->GetDisplayName(SIGDN_FILESYSPATH, &folderPath);
        item->Release();
        if (FAILED(hr) || !folderPath || !folderPath[0]) {
            if (folderPath) CoTaskMemFree(folderPath);
            shellWindows->Release();
            if (uninit) CoUninitialize();
            return false;
        }

        outPath.assign(folderPath);
        CoTaskMemFree(folderPath);
        shellWindows->Release();
        if (uninit) CoUninitialize();
        return true;
    }

    shellWindows->Release();
    if (uninit) CoUninitialize();
    return false;
}

bool GetBestExplorerFolderPath(std::wstring& outPath) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool canUseCom = SUCCEEDED(hrCo) || hrCo == RPC_E_CHANGED_MODE;
    bool uninit = SUCCEEDED(hrCo);

    if (!canUseCom) {
        return false;
    }

    CLSID clsidShellWindows{};
    HRESULT hr = CLSIDFromString(
        L"{9BA05972-F6A8-11CF-A442-00A0C90A8F39}", &clsidShellWindows);
    if (FAILED(hr)) {
        if (uninit) CoUninitialize();
        return false;
    }

    IShellWindows* shellWindows = nullptr;
    hr = CoCreateInstance(clsidShellWindows, nullptr, CLSCTX_ALL,
                          IID_PPV_ARGS(&shellWindows));
    if (FAILED(hr)) {
        if (uninit) CoUninitialize();
        return false;
    }

    HWND fg = GetForegroundWindow();
    HWND fgRoot = fg ? GetAncestor(fg, GA_ROOT) : nullptr;

    long count = 0;
    hr = shellWindows->get_Count(&count);
    if (FAILED(hr) || count <= 0) {
        shellWindows->Release();
        if (uninit) CoUninitialize();
        return false;
    }

    std::wstring fallbackPath;
    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* dispatch = nullptr;
        hr = shellWindows->Item(index, &dispatch);
        VariantClear(&index);
        if (FAILED(hr) || !dispatch) {
            continue;
        }

        IWebBrowserApp* webBrowser = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&webBrowser));
        dispatch->Release();
        if (FAILED(hr) || !webBrowser) {
            continue;
        }

        SHANDLE_PTR browserHwndRaw = 0;
        hr = webBrowser->get_HWND(&browserHwndRaw);
        HWND browserHwnd = (HWND)(LONG_PTR)browserHwndRaw;
        if (FAILED(hr) || !IsExplorerFolderWindow(browserHwnd)) {
            webBrowser->Release();
            continue;
        }

        BSTR locationUrl = nullptr;
        hr = webBrowser->get_LocationURL(&locationUrl);
        webBrowser->Release();
        if (FAILED(hr)) {
            if (locationUrl) SysFreeString(locationUrl);
            continue;
        }

        std::wstring path;
        bool decoded = DecodeLocationUrlToPath(locationUrl, path);
        SysFreeString(locationUrl);
        if (!decoded) {
            continue;
        }

        if (browserHwnd == fgRoot) {
            outPath = path;
            shellWindows->Release();
            if (uninit) CoUninitialize();
            return true;
        }

        if (fallbackPath.empty()) {
            fallbackPath = path;
        }
    }

    shellWindows->Release();

    if (!fallbackPath.empty()) {
        outPath = fallbackPath;
    }

    if (uninit) CoUninitialize();
    return !outPath.empty();
}

void ExecuteCopyAndOpen() {
    DWORD now = GetTickCount();
    if (now - g_lastExecuteTick < 150) {
        return;
    }
    g_lastExecuteTick = now;

    std::wstring folderPath;
    DWORD nowTick = GetTickCount();
    if (!g_pendingSelectedFolderPath.empty() &&
        nowTick - g_pendingSelectedFolderTick < 5000) {
        folderPath = g_pendingSelectedFolderPath;
    } else if (!GetSelectedFolderPathFromForegroundExplorer(folderPath, nullptr) &&
               !GetBestExplorerFolderPath(folderPath)) {
        Wh_Log(L"ExecuteCopyAndOpen: failed to resolve folder path");
        return;
    }

    HINSTANCE result = ShellExecuteW(nullptr, L"open", L"explorer.exe",
                                     folderPath.c_str(), nullptr, SW_SHOWNORMAL);
    if ((INT_PTR)result <= 32) {
        Wh_Log(L"ExecuteCopyAndOpen: ShellExecuteW failed=%Id path=%s",
               (INT_PTR)result, folderPath.c_str());
    }
}

void InsertMenuItemIfNeeded(HMENU menu) {
    if (!g_settings.enableContextMenuItem || !menu) {
        return;
    }

    if (GetMenuState(menu, kCopyAndOpenCommandId, MF_BYCOMMAND) != 0xFFFFFFFF) {
        return;
    }

    std::wstring targetPath;
    bool hasSelection = false;
    if (!GetSelectedFolderPathFromForegroundExplorer(targetPath, &hasSelection)) {
        if (!hasSelection) {
            if (!GetBestExplorerFolderPath(targetPath)) {
                return;
            }
        } else {
            return;
        }
    }

    g_pendingSelectedFolderPath = targetPath;
    g_pendingSelectedFolderTick = GetTickCount();

    int count = GetMenuItemCount(menu);
    if (count < 0) {
        return;
    }

    auto findMenuPosByText = [&](const wchar_t* text) -> int {
        for (int i = 0; i < count; i++) {
            wchar_t itemText[256] = {};
            if (GetMenuStringW(menu, (UINT)i, itemText, ARRAYSIZE(itemText),
                               MF_BYPOSITION) > 0 &&
                wcsstr(itemText, text)) {
                return i;
            }
        }
        return -1;
    };

    auto isSeparatorAt = [&](int pos) -> bool {
        if (pos < 0) {
            return false;
        }
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE;
        if (!GetMenuItemInfoW(menu, (UINT)pos, TRUE, &mii)) {
            return false;
        }
        return (mii.fType & MFT_SEPARATOR) != 0;
    };

    int propPos = findMenuPosByText(L"属性");
    if (propPos < 0) {
        propPos = findMenuPosByText(L"Properties");
    }

    UINT pos = (UINT)count;
    if (propPos >= 0) {
        pos = (UINT)propPos;
        if (propPos > 0 && !isSeparatorAt(propPos - 1)) {
            InsertMenuW(menu, pos, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
            pos++;
        }
    } else if (count > 0) {
        InsertMenuW(menu, pos, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
        pos++;
    }

    InsertMenuW(menu, pos, MF_BYPOSITION | MF_STRING | MF_ENABLED,
                kCopyAndOpenCommandId, kMenuText);
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU menu,
                                  UINT flags,
                                  int x,
                                  int y,
                                  HWND owner,
                                  LPTPMPARAMS params) {
    if (IsOwnedByExplorerFolderWindow(owner)) {
        InsertMenuItemIfNeeded(menu);
    }

    BOOL ret = TrackPopupMenuEx_Original(menu, flags, x, y, owner, params);
    if ((flags & TPM_RETURNCMD) && (UINT)ret == kCopyAndOpenCommandId) {
        ExecuteCopyAndOpen();
        return 0;
    }

    return ret;
}

BOOL WINAPI TrackPopupMenu_Hook(HMENU menu,
                                UINT flags,
                                int x,
                                int y,
                                int reserved,
                                HWND owner,
                                const RECT* rect) {
    if (IsOwnedByExplorerFolderWindow(owner)) {
        InsertMenuItemIfNeeded(menu);
    }

    BOOL ret = TrackPopupMenu_Original(menu, flags, x, y, reserved, owner, rect);
    if ((flags & TPM_RETURNCMD) && (UINT)ret == kCopyAndOpenCommandId) {
        ExecuteCopyAndOpen();
        return 0;
    }

    return ret;
}

LRESULT WINAPI SendMessageW_Hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND && LOWORD(wParam) == kCopyAndOpenCommandId) {
        ExecuteCopyAndOpen();
        return 0;
    }

    return SendMessageW_Original(hwnd, msg, wParam, lParam);
}

BOOL WINAPI PostMessageW_Hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND && LOWORD(wParam) == kCopyAndOpenCommandId) {
        ExecuteCopyAndOpen();
        return TRUE;
    }

    return PostMessageW_Original(hwnd, msg, wParam, lParam);
}

LRESULT WINAPI DispatchMessageW_Hook(const MSG* msg) {
    if (msg && msg->message == WM_COMMAND &&
        LOWORD(msg->wParam) == kCopyAndOpenCommandId) {
        ExecuteCopyAndOpen();
        return 0;
    }

    return DispatchMessageW_Original(msg);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing duplicate-folder-in-new-window (explorer context menu)");

    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        return FALSE;
    }

    auto trackPopupMenuEx =
        (TrackPopupMenuEx_t)GetProcAddress(user32, "TrackPopupMenuEx");
    auto trackPopupMenu = (TrackPopupMenu_t)GetProcAddress(user32, "TrackPopupMenu");
    auto sendMessageW = (SendMessageW_t)GetProcAddress(user32, "SendMessageW");
    auto postMessageW = (PostMessageW_t)GetProcAddress(user32, "PostMessageW");
    auto dispatchMessageW =
        (DispatchMessageW_t)GetProcAddress(user32, "DispatchMessageW");

    if (!trackPopupMenuEx || !trackPopupMenu || !sendMessageW ||
        !postMessageW || !dispatchMessageW) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)trackPopupMenuEx, (void*)TrackPopupMenuEx_Hook,
                       (void**)&TrackPopupMenuEx_Original);
    Wh_SetFunctionHook((void*)trackPopupMenu, (void*)TrackPopupMenu_Hook,
                       (void**)&TrackPopupMenu_Original);
    Wh_SetFunctionHook((void*)sendMessageW, (void*)SendMessageW_Hook,
                       (void**)&SendMessageW_Original);
    Wh_SetFunctionHook((void*)postMessageW, (void*)PostMessageW_Hook,
                       (void**)&PostMessageW_Original);
    Wh_SetFunctionHook((void*)dispatchMessageW, (void*)DispatchMessageW_Hook,
                       (void**)&DispatchMessageW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing duplicate-folder-in-new-window");
    g_pendingSelectedFolderPath.clear();
    g_pendingSelectedFolderTick = 0;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}
