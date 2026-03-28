        // ==WindhawkMod==
// @id            windows-11-start-menu-buttons
// @name          Windows 11 Start Menu Buttons
// @description   Customizable buttons for the Windows 11 Start menu.
// @version       1.1
// @author        Salyts
// @github        https://github.com/Salyts
// @include       StartMenuExperienceHost.exe
// @include       explorer.exe
// @architecture  x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -luuid -luser32 -lwtsapi32 -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Start Menu Buttons

This mod replaces the default bottom row of the Windows 11 Start menu with custom buttons.
Each button has three fields:

1. **Name** — tooltip text.
2. **Icon** — a [Segoe Fluent Icons / Segoe UI Symbol](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font) glyph, for example `\uE7E8`. 

3. **Action** — what the button does.

![img](https://i.imgur.com/FpVm6OR.png)

### Action formats

- `cmd:explorer.exe` — runs through `cmd.exe`.
- `shell:shutdown /r` — runs console window. 
- `"C:\Program Files"` — opens a folder.
- `"C:\Program Files\Google\Chrome\Application\chrome.exe"` — opens a file or app.
- `~Downloads` — searches for a folder by name in common system locations.
- `~chrome.exe` — searches for a file by name in common system locations.
- `https://www.youtube.com/` — opens a website.
- `{ "Restart", "\uE777", "shell:shutdown /r /f /t 0" }, { "Shut down", "\uE7E8", "shell:shutdown /s /f /t 0" }` — a submenu with multiple items.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- alignment: right
  $name: Alignment buttons
  $options:
    - left: Left
    - center: Center
    - right: Right

- hide_account_button: false
  $name: Hide Account Button

- invert_buttons: true
  $name: Invert Buttons

- close_start_menu: true
  $name: Close Start Menu after button click
  $description: Closes Start after clicking any custom button except the button that opens a submenu.

- button_spacing: 2
  $name: Spacing between buttons (px)
  $description: Margin between each button

- container_margin_left: -16
  $name: Container left margin (px)

- container_margin_right: -16
  $name: Container right margin (px)

- buttons:
    - - Name: Power
      - Icon: "\uE7E8"
      - Action: '{"Lock", "\uE72E", "shell:rundll32.exe user32.dll, LockWorkStation"}, {"Sleep", "\uE708", "shell:Rundll32.exe powrprof.dll,SetSuspendState Sleep 0"}, {"Restart", "\uE777", "shell:shutdown /r /f /t 0"}, {"Shut down", "\uE7E8", "shell:shutdown /s /f /t 0"}'
    - - Name: Settings
      - Icon: "\uE713"
      - Action: 'ms-settings:'
    - - Name: Explorer
      - Icon: "\uEC50"
      - Action: ~explorer.exe
    - - Name: Browser
      - Icon: "\uE774"
      - Action: https://www.google.com/
    - - Name: Pictures
      - Icon: "\uE91B"
      - Action: ~Pictures
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>

#include <windows.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <powrprof.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace wu = winrt::Windows::UI;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuc = winrt::Windows::UI::Core;

static std::atomic<bool> g_initialized{false};
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_buttonsInjected{false};
static std::atomic<bool> g_monitoringActive{false};
static std::atomic<bool> g_forceRebuild{false};

static std::atomic<bool> g_closeStartMenu{true};

static std::mutex g_settingsMutex;

static const wchar_t* CONTAINER_TAG = L"PowerButtons_Container";
static const wchar_t* PROXY_WINDOW_CLASS = L"PowerActionProxy_Class";
static const wchar_t* PROXY_WINDOW_NAME = L"PowerActionProxy_Window";
static const wchar_t* THREAD_CALL_MSG = L"PowerButtons_ThreadCall";
static constexpr ULONG_PTR kCopyDataMagic = 0x57483142;

struct Settings {
    wux::HorizontalAlignment alignment = wux::HorizontalAlignment::Right;
    bool hideAccountButton = false;
    bool invertButtons = true;
    bool closeStartMenu = true;
    int buttonSpacing = 2;
    int containerMarginLeft = -16;
    int containerMarginRight = -16;
};

struct ActionItem {
    std::wstring name;
    std::wstring icon;
    std::wstring action;
    std::vector<ActionItem> submenu;
};

static Settings g_settings;
static std::vector<ActionItem> g_buttons;

static winrt::weak_ref<wuxc::Panel> g_parentPanel{nullptr};
static winrt::weak_ref<wux::FrameworkElement> g_originalPowerButton{nullptr};
static winrt::weak_ref<wuxc::StackPanel> g_buttonContainer{nullptr};

static HWND g_proxyWindow = NULL;
static HANDLE g_proxyThread = NULL;
static DWORD g_proxyThreadId = 0;

static winrt::event_token g_visibilityToken{};
static winrt::event_token g_activatedToken{};

// -------------------- small helpers --------------------

static std::wstring Trim(std::wstring s) {
    auto isSpace = [](wchar_t ch) { return iswspace(ch) != 0; };
    while (!s.empty() && isSpace(s.front())) s.erase(s.begin());
    while (!s.empty() && isSpace(s.back())) s.pop_back();
    return s;
}

static std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

static bool StartsWithNoCase(const std::wstring& s, const std::wstring& prefix) {
    if (s.size() < prefix.size()) return false;
    return _wcsnicmp(s.c_str(), prefix.c_str(), prefix.size()) == 0;
}

static std::wstring StripOuterQuotes(const std::wstring& s) {
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"')
        return s.substr(1, s.size() - 2);
    return s;
}

static std::wstring DecodeEscapes(const std::wstring& input) {
    std::wstring out;
    out.reserve(input.size());

    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == L'\\' && i + 1 < input.size()) {
            wchar_t n = input[i + 1];
            if (n == L'u' && i + 5 < input.size()) {
                unsigned value = 0;
                bool ok = true;
                for (size_t j = i + 2; j < i + 6; j++) {
                    wchar_t c = input[j];
                    value <<= 4;
                    if (c >= L'0' && c <= L'9') value |= (c - L'0');
                    else if (c >= L'a' && c <= L'f') value |= (10 + c - L'a');
                    else if (c >= L'A' && c <= L'F') value |= (10 + c - L'A');
                    else { ok = false; break; }
                }
                if (ok) {
                    out.push_back((wchar_t)value);
                    i += 5;
                    continue;
                }
            }
            if (n == L'\\') { out.push_back(L'\\'); i++; continue; }
            if (n == L'"')  { out.push_back(L'"');  i++; continue; }
            if (n == L'n')  { out.push_back(L'\n'); i++; continue; }
            if (n == L'r')  { out.push_back(L'\r'); i++; continue; }
            if (n == L't')  { out.push_back(L'\t'); i++; continue; }
        }
        out.push_back(input[i]);
    }
    return out;
}

static bool LooksLikeUrl(const std::wstring& s) {
    return StartsWithNoCase(s, L"http://") ||
           StartsWithNoCase(s, L"https://") ||
           StartsWithNoCase(s, L"ftp://") ||
           StartsWithNoCase(s, L"mailto:");
}

static bool LooksLikeFolderPath(const std::wstring& s) {
    if (s.size() >= 2 && s[1] == L':') return true;
    if (s.rfind(L"\\\\", 0) == 0) return true;
    return false;
}

static void SafeFreeString(PCWSTR s) {
    if (s) Wh_FreeStringSetting(s);
}

// -------------------- settings --------------------

static void LoadSettings() {
    Settings newSettings;

    PCWSTR alignmentStr = Wh_GetStringSetting(L"alignment");
    if (_wcsicmp(alignmentStr, L"left") == 0)
        newSettings.alignment = wux::HorizontalAlignment::Left;
    else if (_wcsicmp(alignmentStr, L"center") == 0)
        newSettings.alignment = wux::HorizontalAlignment::Center;
    else
        newSettings.alignment = wux::HorizontalAlignment::Right;
    SafeFreeString(alignmentStr);

    newSettings.hideAccountButton = Wh_GetIntSetting(L"hide_account_button") != 0;
    newSettings.invertButtons     = Wh_GetIntSetting(L"invert_buttons") != 0;
    newSettings.closeStartMenu    = Wh_GetIntSetting(L"close_start_menu") != 0;
    newSettings.buttonSpacing     = Wh_GetIntSetting(L"button_spacing");
    newSettings.containerMarginLeft  = Wh_GetIntSetting(L"container_margin_left");
    newSettings.containerMarginRight = Wh_GetIntSetting(L"container_margin_right");

    g_closeStartMenu.store(newSettings.closeStartMenu, std::memory_order_relaxed);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = newSettings;
}

static std::wstring GetSettingStringIndex(const wchar_t* key, int index) {
    PCWSTR s = Wh_GetStringSetting(L"buttons[%d].%s", index, key);
    std::wstring result = s ? s : L"";
    SafeFreeString(s);
    return result;
}

static bool ParseQuotedToken(const std::wstring& s, size_t& pos, std::wstring& out) {
    while (pos < s.size() && iswspace(s[pos])) pos++;
    if (pos >= s.size() || s[pos] != L'\"') return false;
    pos++;

    std::wstring token;
    while (pos < s.size()) {
        wchar_t ch = s[pos++];
        if (ch == L'\"') {
            out = DecodeEscapes(token);
            return true;
        }
        if (ch == L'\\' && pos < s.size()) {
            token.push_back(L'\\');
            token.push_back(s[pos++]);
        } else {
            token.push_back(ch);
        }
    }
    return false;
}

static std::vector<ActionItem> ParseActionGroup(const std::wstring& actionText) {
    std::vector<ActionItem> items;
    std::wstring s = Trim(actionText);
    if (s.find(L'{') == std::wstring::npos) return items;

    size_t pos = 0;
    while (true) {
        pos = s.find(L'{', pos);
        if (pos == std::wstring::npos) break;
        pos++;

        std::wstring name, icon, action;
        if (!ParseQuotedToken(s, pos, name)) break;

        while (pos < s.size() && iswspace(s[pos])) pos++;
        if (pos >= s.size() || s[pos] != L',') break;
        pos++;

        if (!ParseQuotedToken(s, pos, icon)) break;

        while (pos < s.size() && iswspace(s[pos])) pos++;
        if (pos >= s.size() || s[pos] != L',') break;
        pos++;

        if (!ParseQuotedToken(s, pos, action)) break;

        while (pos < s.size() && iswspace(s[pos])) pos++;
        if (pos >= s.size() || s[pos] != L'}') {
            Wh_Log(L"ParseActionGroup: missing closing brace near item '%s'", name.c_str());
            break;
        }
        pos++;

        ActionItem item;
        item.name   = Trim(name);
        item.icon   = Trim(icon);
        item.action = Trim(action);
        items.push_back(std::move(item));
    }

    return items;
}

static void BuildButtons() {
    std::vector<ActionItem> newButtons;

    for (int i = 0; i < 128; i++) {
        std::wstring name = GetSettingStringIndex(L"name", i);
        if (name.empty()) break;

        std::wstring icon   = DecodeEscapes(GetSettingStringIndex(L"icon", i));
        std::wstring action = GetSettingStringIndex(L"action", i);

        ActionItem item;
        item.name    = Trim(name);
        item.icon    = Trim(icon);
        item.action  = Trim(action);
        item.submenu = ParseActionGroup(item.action);
        newButtons.push_back(std::move(item));
    }

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_buttons = std::move(newButtons);
}

static bool ShouldCloseStartMenu() {
    return g_closeStartMenu.load(std::memory_order_relaxed);
}

static void SendEscapeKey() {
    INPUT inputs[2]{};

    inputs[0].type        = INPUT_KEYBOARD;
    inputs[0].ki.wVk      = VK_ESCAPE;

    inputs[1].type        = INPUT_KEYBOARD;
    inputs[1].ki.wVk      = VK_ESCAPE;
    inputs[1].ki.dwFlags  = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

static inline void CloseStartMenuAfterClick() {
    if (!ShouldCloseStartMenu()) return;

    try {
        auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (dq) {
            dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, []() {
                SendEscapeKey();
            });
            return;
        }
    } catch (...) {}

    SendEscapeKey();
}

// -------------------- executing actions --------------------

static bool GetKnownFolderPathByName(const std::wstring& name, std::wstring& out) {
    PWSTR raw = nullptr;
    auto setPath = [&](REFKNOWNFOLDERID id) -> bool {
        if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &raw)) && raw) {
            out = raw;
            CoTaskMemFree(raw);
            raw = nullptr;
            return true;
        }
        if (raw) { CoTaskMemFree(raw); raw = nullptr; }
        return false;
    };

    std::wstring n = ToLower(Trim(name));
    if (n == L"downloads")                    return setPath(FOLDERID_Downloads);
    if (n == L"documents" || n == L"personal") return setPath(FOLDERID_Documents);
    if (n == L"music")                        return setPath(FOLDERID_Music);
    if (n == L"pictures")                     return setPath(FOLDERID_Pictures);
    if (n == L"videos")                       return setPath(FOLDERID_Videos);
    if (n == L"desktop")                      return setPath(FOLDERID_Desktop);
    if (n == L"profile" || n == L"home")      return setPath(FOLDERID_Profile);
    return false;
}

static bool PathExists(const std::wstring& path, DWORD* attrs = nullptr) {
    DWORD a = GetFileAttributesW(path.c_str());
    if (a == INVALID_FILE_ATTRIBUTES) return false;
    if (attrs) *attrs = a;
    return true;
}

static bool SearchRecursiveImpl(const std::wstring& root, const std::wstring& fileName, int depth, std::wstring& found) {
    if (depth < 0) return false;

    WIN32_FIND_DATAW fd{};
    std::wstring mask = root;
    if (!mask.empty() && mask.back() != L'\\') mask += L'\\';
    mask += L"*";

    HANDLE h = FindFirstFileW(mask.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;

        std::wstring full = root;
        if (!full.empty() && full.back() != L'\\') full += L'\\';
        full += fd.cFileName;

        if (_wcsicmp(fd.cFileName, fileName.c_str()) == 0) {
            found = full;
            FindClose(h);
            return true;
        }

        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            if (SearchRecursiveImpl(full, fileName, depth - 1, found)) {
                FindClose(h);
                return true;
            }
        }
    } while (FindNextFileW(h, &fd));

    FindClose(h);
    return false;
}

static bool SearchForPathByName(const std::wstring& fileName, std::wstring& out) {
    wchar_t buf[MAX_PATH * 4]{};
    if (SearchPathW(nullptr, fileName.c_str(), nullptr, ARRAYSIZE(buf), buf, nullptr) > 0) {
        out = buf;
        return true;
    }

    std::vector<std::wstring> roots;
    auto addEnv = [&](const wchar_t* name) {
        wchar_t tmp[MAX_PATH * 2]{};
        if (GetEnvironmentVariableW(name, tmp, ARRAYSIZE(tmp)) > 0) roots.push_back(tmp);
    };

    addEnv(L"ProgramFiles");
    addEnv(L"ProgramFiles(x86)");
    addEnv(L"ProgramW6432");
    addEnv(L"SystemRoot");
    addEnv(L"LOCALAPPDATA");
    addEnv(L"APPDATA");
    addEnv(L"USERPROFILE");

    std::wstring known;
    if (GetKnownFolderPathByName(L"desktop",   known)) roots.push_back(known);
    if (GetKnownFolderPathByName(L"documents", known)) roots.push_back(known);
    if (GetKnownFolderPathByName(L"downloads", known)) roots.push_back(known);
    if (GetKnownFolderPathByName(L"music",     known)) roots.push_back(known);
    if (GetKnownFolderPathByName(L"pictures",  known)) roots.push_back(known);
    if (GetKnownFolderPathByName(L"videos",    known)) roots.push_back(known);

    for (const auto& root : roots) {
        if (SearchRecursiveImpl(root, fileName, 5, out)) return true;
    }
    return false;
}

static bool ExecuteHiddenProcess(const std::wstring& commandLine, bool useCmdExe) {
    std::wstring cmdLine = useCmdExe ? (L"cmd.exe /C " + commandLine) : commandLine;
    std::vector<wchar_t> mutableCmd(cmdLine.begin(), cmdLine.end());
    mutableCmd.push_back(L'\0');

    STARTUPINFOW si{};
    si.cb           = sizeof(si);
    si.dwFlags      = STARTF_USESHOWWINDOW;
    si.wShowWindow  = SW_HIDE;

    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessW(
        nullptr, mutableCmd.data(), nullptr, nullptr,
        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);

    if (ok) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return true;
    }
    return false;
}

static bool ExecutePathOrUrl(const std::wstring& text) {
    std::wstring s = Trim(text);
    if (s.empty()) return false;

    if (LooksLikeUrl(s)) {
        ShellExecuteW(nullptr, L"open", s.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return true;
    }

    s = StripOuterQuotes(s);

    DWORD attrs = 0;
    if (PathExists(s, &attrs)) {
        ShellExecuteW(nullptr, L"open", s.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return true;
    }

    if (StartsWithNoCase(s, L"~")) {
        std::wstring target = Trim(s.substr(1));
        std::wstring resolved;
        if (GetKnownFolderPathByName(target, resolved)) {
            ShellExecuteW(nullptr, L"open", resolved.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            return true;
        }
        if (SearchForPathByName(target, resolved)) {
            ShellExecuteW(nullptr, L"open", resolved.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            return true;
        }
    }

    if (LooksLikeFolderPath(s)) {
        ShellExecuteW(nullptr, L"open", s.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return true;
    }

    return false;
}

static void ExecuteActionText(const std::wstring& rawAction) {
    std::wstring action = Trim(rawAction);
    if (action.empty()) return;

    if (StartsWithNoCase(action, L"cmd:")) {
        ExecuteHiddenProcess(Trim(action.substr(4)), true);
        return;
    }
    if (StartsWithNoCase(action, L"shell:")) {
        ExecuteHiddenProcess(Trim(action.substr(6)), false);
        return;
    }
    if (StartsWithNoCase(action, L"ms-settings:")) {
        ShellExecuteW(nullptr, L"open", action.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }

    if (ExecutePathOrUrl(action)) return;

    ExecuteHiddenProcess(action, false);
}

// -------------------- proxy window --------------------

static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_COPYDATA) {
        const COPYDATASTRUCT* cds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
        if (cds && cds->dwData == kCopyDataMagic && cds->cbData == sizeof(int)) {
            int idx = *static_cast<const int*>(cds->lpData);
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            if (idx >= 0 && idx < (int)g_buttons.size())
            ExecuteActionText(g_buttons[idx].action);
        }
        return 0;
    }  

    switch (message) {
        case WM_CLOSE:  
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            g_proxyWindow = NULL;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam); 
}

static DWORD WINAPI ProxyWindowThread(LPVOID) {
    WNDCLASSEXW wcex{};
    wcex.cbSize        = sizeof(wcex);
    wcex.lpfnWndProc   = ProxyWndProc;
    wcex.hInstance     = GetModuleHandleW(nullptr);
    wcex.lpszClassName = PROXY_WINDOW_CLASS;

    if (!RegisterClassExW(&wcex)) {
        Wh_Log(L"Proxy: RegisterClassEx failed, error %lu", GetLastError());
        return 1;
    }

    g_proxyWindow = CreateWindowExW(
        0, PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME, 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr,
        GetModuleHandleW(nullptr), nullptr);

    if (!g_proxyWindow) {
        Wh_Log(L"Proxy: CreateWindowEx failed, error %lu", GetLastError());
        UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandleW(nullptr));
        return 1;
    }

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandleW(nullptr));
    g_proxyWindow = NULL;
    return 0;
}

static void StartProxyThread() {
    if (g_proxyThread) return;
    g_proxyThread = CreateThread(nullptr, 0, ProxyWindowThread, nullptr, 0, &g_proxyThreadId);
    if (!g_proxyThread) Wh_Log(L"Proxy: CreateThread failed, error %lu", GetLastError());
}

static inline bool SendActionToProxy(int buttonIndex) {
    HWND hProxy = FindWindowW(PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME);
    if (!hProxy) {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        if (buttonIndex >= 0 && buttonIndex < (int)g_buttons.size())
            ExecuteActionText(g_buttons[buttonIndex].action);
        return false;
    }

    COPYDATASTRUCT cds{};
    cds.dwData = kCopyDataMagic;
    cds.cbData = sizeof(int);
    cds.lpData = &buttonIndex;
    return SendMessageW(hProxy, WM_COPYDATA, 0, (LPARAM)&cds) != 0; 
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    return ToLower(path).find(L"explorer.exe") != std::wstring::npos;
}

// -------------------- start menu injection --------------------

static wux::FrameworkElement FindOriginalPowerButton(wux::DependencyObject parent) {
    if (!parent) return nullptr;

    int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);
        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == L"PowerButton") return fe;
        }
        auto found = FindOriginalPowerButton(child);
        if (found) return found;
    }
    return nullptr;
}

static void AddMenuItemsFromActions(wuxc::MenuFlyout flyout,
                                    const std::vector<ActionItem>& items) {
    for (const auto& item : items) {
        wuxc::MenuFlyoutItem menuItem;
        menuItem.Text(item.name);
        if (!item.icon.empty()) {
            wuxc::FontIcon icon;
            icon.Glyph(item.icon);
            icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
            icon.FontSize(16);
            menuItem.Icon(icon);
        }
        std::wstring action = item.action;
        menuItem.Click([action](auto&&, auto&&) {
            ExecuteActionText(action);
            CloseStartMenuAfterClick();
        });
        flyout.Items().Append(menuItem);
    }
}

static void InjectButtons(wuxc::Panel parentPanel,
                          wux::FrameworkElement /*originalPowerButton*/) {
    try {
        Settings currentSettings;
        std::vector<ActionItem> currentButtons;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            currentSettings = g_settings;
            currentButtons  = g_buttons;
        }
 
        // Сохраняем оригинальные индексы ДО разворота
        std::vector<int> origIdx(currentButtons.size());
        for (int i = 0; i < (int)origIdx.size(); i++) origIdx[i] = i;
 
        if (currentSettings.invertButtons) {
            std::reverse(currentButtons.begin(), currentButtons.end());
            std::reverse(origIdx.begin(), origIdx.end());
        }
 
        auto children = parentPanel.Children();
        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i);
            if (auto fe = child.try_as<wux::FrameworkElement>()) {
                auto tag = fe.Tag();
                if (tag && winrt::unbox_value_or<winrt::hstring>(tag, L"") == CONTAINER_TAG)
                    continue;
                bool isUserTile = (fe.Name() == L"UserTileButton" ||
                                   fe.Name() == L"UserTile" ||
                                   fe.Name() == L"ProfileButton");
                if (!currentSettings.hideAccountButton && isUserTile) {
                    fe.Visibility(wux::Visibility::Visible); continue;
                }
                fe.Visibility(wux::Visibility::Collapsed);
            }
        }
 
        wuxc::StackPanel container = g_buttonContainer.get();
        if (container) {
            try {
                auto parent = wuxm::VisualTreeHelper::GetParent(container);
                if (!parent || parent != parentPanel) { container = nullptr; g_buttonContainer = nullptr; }
            } catch (...) { container = nullptr; g_buttonContainer = nullptr; }
        }
 
        if (!container) {
            for (uint32_t i = 0; i < children.Size(); i++) {
                auto child = children.GetAt(i);
                if (auto panel = child.try_as<wuxc::StackPanel>()) {
                    auto tag = panel.Tag();
                    if (tag && winrt::unbox_value_or<winrt::hstring>(tag, L"") == CONTAINER_TAG) {
                        container = panel; g_buttonContainer = container; break;
                    }
                }
            }
        }
 
        if (!container) {
            container = wuxc::StackPanel();
            container.Tag(winrt::box_value(CONTAINER_TAG));
            container.Orientation(wuxc::Orientation::Horizontal);
            container.VerticalAlignment(wux::VerticalAlignment::Center);
            parentPanel.Children().Append(container);
            g_buttonContainer = container;
        }
 
        container.Visibility(wux::Visibility::Visible);
        container.HorizontalAlignment(currentSettings.alignment);
        container.Margin({ (double)currentSettings.containerMarginLeft, 0,
                           (double)currentSettings.containerMarginRight, 0 });
 
        bool needRebuild = g_forceRebuild.exchange(false) ||
                           container.Children().Size() != currentButtons.size();
        if (!needRebuild) return;
 
        container.Children().Clear();
 
        for (size_t i = 0; i < currentButtons.size(); i++) {
            const auto& btnDef = currentButtons[i];
            int btnIdx = origIdx[i];
 
            wuxc::Button btn;
            btn.Width(40); btn.Height(40);
            btn.Margin({ 0, 0,
                i + 1 < currentButtons.size() ? (double)currentSettings.buttonSpacing : 0.0, 0 });
            btn.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
            btn.BorderThickness({ 0, 0, 0, 0 });
            btn.CornerRadius({ 4, 4, 4, 4 });
 
            wuxc::FontIcon icon;
            icon.Glyph(btnDef.icon);
            icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
            icon.FontSize(16);
            btn.Content(icon);
            wuxc::ToolTipService::SetToolTip(btn, winrt::box_value(btnDef.name));
 
            if (!btnDef.submenu.empty()) {
                auto submenu = btnDef.submenu;
                btn.Click([submenu, btn](auto&&, auto&&) mutable {
                    wuxc::MenuFlyout flyout;
                    AddMenuItemsFromActions(flyout, submenu);
                    flyout.ShowAt(btn);
                });
            } else {
                btn.Click([btnIdx](auto&&, auto&&) {
                    SendActionToProxy(btnIdx);
                    CloseStartMenuAfterClick();
                });
            }
 
            container.Children().Append(btn);
        }
 
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Inject: WinRT error 0x%08X", (unsigned)ex.code());
    } catch (...) {
        Wh_Log(L"Inject: Unknown error");
    }
}

static bool TryInjectButtons() {
    if (g_unloading) return false;

    try {
        auto window = wux::Window::Current();
        if (!window) return false;

        auto content = window.Content();
        if (!content) return false;

        auto powerButton = FindOriginalPowerButton(content);
        if (!powerButton) return false;

        auto parentObj = wuxm::VisualTreeHelper::GetParent(powerButton);
        if (!parentObj) return false;

        auto parentPanel = parentObj.try_as<wuxc::Panel>();
        if (!parentPanel) return false;

        g_parentPanel        = parentPanel;
        g_originalPowerButton = powerButton;

        InjectButtons(parentPanel, powerButton);
        g_buttonsInjected = true;
        return true;
    } catch (...) {
        Wh_Log(L"Inject: exception");
        return false;
    }
}

static void OnWindowVisible() {
    if (g_unloading) return;
    if (g_buttonsInjected && !g_forceRebuild) return;

    auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (dq) {
        dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, []() {
            if (!g_unloading) TryInjectButtons();
        });
    }
}

static bool StartVisibilityMonitoring() {
    if (g_monitoringActive) return true;

    try {
        wuc::CoreWindow coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) return false;

        g_visibilityToken = coreWindow.VisibilityChanged([](auto&&, auto&& args) {
            if (args.Visible()) OnWindowVisible();
        });

        g_activatedToken = coreWindow.Activated([](auto&&, auto&& args) {
            if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated)
                OnWindowVisible();
        });

        g_monitoringActive = true;
        if (coreWindow.Visible()) OnWindowVisible();
        return true;
    } catch (...) {
        return false;
    }
}

static void StopVisibilityMonitoring() {
    try {
        wuc::CoreWindow coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (coreWindow) {
            if (g_visibilityToken) {
                coreWindow.VisibilityChanged(std::exchange(g_visibilityToken, {}));
            }
            if (g_activatedToken) {
                coreWindow.Activated(std::exchange(g_activatedToken, {}));
            }
        }
    } catch (...) {}

    try {
        auto panel        = g_parentPanel.get();
        auto container    = g_buttonContainer.get();
        auto originalBtn  = g_originalPowerButton.get();

        if (panel && container) {
            uint32_t index;
            if (panel.Children().IndexOf(container, index))
                panel.Children().RemoveAt(index);

            auto ch = panel.Children();
            for (uint32_t i = 0; i < ch.Size(); i++) {
                auto child = ch.GetAt(i);
                if (auto fe = child.try_as<wux::FrameworkElement>()) {
                    if (fe.Visibility() == wux::Visibility::Collapsed)
                        fe.Visibility(wux::Visibility::Visible);
                }
            }
        }

        if (originalBtn)
            originalBtn.Visibility(wux::Visibility::Visible);
    } catch (...) {}

    g_parentPanel         = nullptr;
    g_originalPowerButton = nullptr;
    g_buttonContainer     = nullptr;
    g_monitoringActive    = false;
}

static HWND g_retryHwnd = NULL;

static constexpr UINT_PTR kRetryTimerId = 0xB77D1A3C;
static void CALLBACK RetryTimerProc(HWND hwnd, UINT, UINT_PTR idEvent, DWORD) {
    KillTimer(hwnd, idEvent);
    if (!g_unloading) {
        g_buttonsInjected = false;
        HWND h = g_retryHwnd ? g_retryHwnd : hwnd;
        if (!StartVisibilityMonitoring())
            SetTimer(h, kRetryTimerId, 100, RetryTimerProc);
    }
}

static void InitWithRetry(HWND hwnd) {
    if (!g_initialized.exchange(true)) {
        LoadSettings();
        BuildButtons();
    }
    if (hwnd) g_retryHwnd = hwnd;

    if (!StartVisibilityMonitoring())
        SetTimer(hwnd, kRetryTimerId, 100, RetryTimerProc); 
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD);
static CreateWindowInBand_t CreateWindowInBand_Original;

static HWND WINAPI CreateWindowInBand_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam, DWORD dwBand)
{
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);

    if (hWnd && lpClassName && _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) 
        InitWithRetry(hWnd);

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD, DWORD);
static CreateWindowInBandEx_t CreateWindowInBandEx_Original;

static HWND WINAPI CreateWindowInBandEx_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags)
{
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);

    if (hWnd && lpClassName && _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0)
        InitWithRetry(hWnd);

    return hWnd;
}

using ThreadCallProc_t = void(WINAPI*)(PVOID);

static bool CallOnWindowThread(HWND hWnd, ThreadCallProc_t proc, PVOID param) {
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) return false;

    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    struct CallParam { ThreadCallProc_t proc; PVOID param; };
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == RegisterWindowMessageW(THREAD_CALL_MSG)) {
                    auto* p = (CallParam*)cwp->lParam;
                    p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);

    if (!hook) return false;

    static const UINT msg = RegisterWindowMessageW(THREAD_CALL_MSG);
    CallParam p{proc, param};
    SendMessageW(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindCoreWindow() {
    struct EnumParam { HWND* result; };
    HWND hWnd = nullptr;
    EnumParam param{&hWnd};

    EnumWindows([](HWND w, LPARAM lParam) -> BOOL {
        auto& p = *(EnumParam*)lParam;
        DWORD pid = 0;
        if (!GetWindowThreadProcessId(w, &pid) || pid != GetCurrentProcessId()) return TRUE;

        WCHAR cls[128]{};
        if (GetClassNameW(w, cls, ARRAYSIZE(cls)) == 0) return TRUE;
        if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            *p.result = w;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&param);

    return hWnd;
}

static void WINAPI InitOnWindowThread(PVOID param) {
    InitWithRetry((HWND)param);
}

// -------------------- entry points --------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    if (IsExplorerProcess()) {
        StartProxyThread();
        return TRUE;
    }

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    void* pCreateWindowInBand   = (void*)GetProcAddress(hUser32, "CreateWindowInBand");
    void* pCreateWindowInBandEx = (void*)GetProcAddress(hUser32, "CreateWindowInBandEx");

    if (pCreateWindowInBand)
        Wh_SetFunctionHook(pCreateWindowInBand, (void*)CreateWindowInBand_Hook,
                           (void**)&CreateWindowInBand_Original);
    if (pCreateWindowInBandEx)
        Wh_SetFunctionHook(pCreateWindowInBandEx, (void*)CreateWindowInBandEx_Hook,
                           (void**)&CreateWindowInBandEx_Original);

    HWND hWnd = FindCoreWindow();
    if (hWnd)
        CallOnWindowThread(hWnd, InitOnWindowThread, (PVOID)hWnd);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_unloading = true;

    if (IsExplorerProcess()) {
        if (g_proxyWindow)
            PostMessageW(g_proxyWindow, WM_CLOSE, 0, 0);
        if (g_proxyThread) {
            WaitForSingleObject(g_proxyThread, 1000);
            CloseHandle(g_proxyThread);
            g_proxyThread = nullptr;
        }
        return;
    }

    HWND hWnd = FindCoreWindow();
    if (hWnd) {
        CallOnWindowThread(hWnd, [](PVOID) {
            StopVisibilityMonitoring();
        }, nullptr);
    }
}

void Wh_ModSettingsChanged() { 
    LoadSettings();
    BuildButtons();
    g_forceRebuild    = true;
    g_buttonsInjected = false;

    HWND hWnd = FindCoreWindow();
    if (hWnd) {
        CallOnWindowThread(hWnd, [](PVOID) {
            try {
                if (auto coreWindow = wuc::CoreWindow::GetForCurrentThread()) {
                    if (coreWindow.Visible())
                        OnWindowVisible();
                }
            } catch (...) {}
        }, nullptr);
    }
}
