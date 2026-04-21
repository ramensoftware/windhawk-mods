// ==WindhawkMod==
// @id            start-menu-custom-app-launcher
// @name          Start Menu Custom App Launcher
// @description   Customizable buttons with custom icons and dropdown menus for the Windows 11 Start menu.
// @version       1.0
// @author        SharkIT-sys
// @github        https://github.com/SharkIT-sys/
// @include       StartMenuExperienceHost.exe
// @include       explorer.exe
// @architecture  x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -luuid -luser32 -lwtsapi32 -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Custom App Launcher

Adds fully customizable buttons to the Windows 11 Start menu bottom bar.
Supports **custom image icons** (PNG, ICO) and **dropdown menus** for
grouping related apps (e.g. Browsers, Tools, Games).

Based on the original
[Windows 11 Start Menu Buttons](https://windhawk.net/mods/windows-11-start-menu-buttons)
mod by Salyts.

## Features

- **Custom icons**: Use Segoe Fluent Icons glyphs OR your own PNG/ICO image files
- **Dropdown menus**: Group apps into flyout menus (configured directly from settings)
- **Multiple action types**: Launch apps, open folders, URLs, system commands and more
- **Backward compatible**: Also supports the legacy `{"name","icon","action"}` syntax

## Icon Types

| Type | Example | Description |
|------|---------|-------------|
| Glyph | `\uE774` | Segoe Fluent Icons glyph code |
| Image | `C:\Icons\chrome.png` | Path to PNG, ICO, JPG or BMP file |
| Empty | *(leave blank)* | Uses a default icon |

Browse all available glyphs:
[Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/style/segoe-fluent-icons-font)

## Action Types

| Prefix | Example | Description |
|--------|---------|-------------|
| *(none)* | `C:\Program Files\app.exe` | Launch app by full path |
| `~` | `~chrome.exe` | Auto-search common folders |
| `http` | `https://google.com` | Open URL in default browser |
| `shell:` | `shell:Downloads` | Open known folder or run process |
| `ms-settings:` | `ms-settings:display` | Open Windows Settings page |
| `cmd:` | `cmd:ipconfig /all` | Run command via cmd.exe |

## Dropdown Menus

To create a dropdown, add a button and fill the **Submenu** section
using the **+** button in the settings UI. Leave the **Direct action** empty.

Example setup for a "Browsers" dropdown:

```
Button: Browsers (icon: \uE774)
  Submenu:
    [1] Chrome  | \uE774 | ~chrome.exe
    [2] Firefox | \uE7C3 | ~firefox.exe
    [3] Edge    | \uE774 | ~msedge.exe
```

## Custom Image Icons

1. Create a folder for your icons (e.g. `C:\Icons\`)
2. Save icons as PNG (recommended: 32x32 or 64x64 px, transparent background)
3. In the **Icon** field, enter the full path: `C:\Icons\chrome.png`

Image icons work in both main buttons and dropdown menu items.

## Common Segoe Fluent Icons

| Glyph | Description |
|-------|-------------|
| `\uE7E8` | Power |
| `\uE72E` | Lock |
| `\uE708` | Sleep |
| `\uE777` | Restart |
| `\uE774` | Globe / Browser |
| `\uE713` | Settings gear |
| `\uE756` | Command prompt |
| `\uE8B7` | Folder |
| `\uE896` | Download |
| `\uE715` | Mail |
| `\uE134` | Chat |
| `\uE943` | Rocket |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- alignment: right
  $name: Button alignment
  $description: Position of buttons in the Start menu bottom bar
  $options:
  - left: Left
  - center: Center
  - right: Right
- hide_account_button: 0
  $name: Hide user account button
  $description: Hides the user avatar button from the Start menu
- invert_buttons: 1
  $name: Invert button order
  $description: Reverses the order in which buttons appear
- close_start_menu: 1
  $name: Close Start menu after action
  $description: Automatically closes the Start menu after clicking a button
- button_spacing: 2
  $name: Button spacing (px)
  $description: Pixels of space between each button
- container_margin_left: -16
  $name: Container left margin
- container_margin_right: -16
  $name: Container right margin
- buttons:
  - - name: ""
      $name: Button name
      $description: Shown as a tooltip on hover
    - icon: ""
      $name: Icon
      $description: Segoe glyph (e.g. \uE774) or image path (e.g. C:\Icons\app.png)
    - action: ""
      $name: Direct action
      $description: App path, URL, folder or command. Leave empty if using a dropdown
    - submenu:
      - - name: ""
          $name: Option name
        - icon: ""
          $name: Option icon
          $description: Segoe glyph or image path
        - action: ""
          $name: Action
          $description: App path, URL or command to execute
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include <windows.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <powrprof.h>
#include <shellapi.h>
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
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

namespace wu   = winrt::Windows::UI;
namespace wux  = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;
namespace wuc  = winrt::Windows::UI::Core;

// -------------------- global state --------------------

static std::atomic<bool> g_initialized{false};
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_buttonsInjected{false};
static std::atomic<bool> g_monitoringActive{false};
static std::atomic<bool> g_forceRebuild{false};

static std::atomic<bool> g_closeStartMenu{true};

static std::mutex g_settingsMutex;

static const wchar_t* CONTAINER_TAG      = L"CustomLauncher_Container";
static const wchar_t* PROXY_WINDOW_CLASS = L"CustomLauncherProxy_Class";
static const wchar_t* PROXY_WINDOW_NAME  = L"CustomLauncherProxy_Window";
static const wchar_t* THREAD_CALL_MSG    = L"CustomLauncher_ThreadCall";
static constexpr ULONG_PTR kCopyDataMagic = 0x434C4150;

struct Settings {
    wux::HorizontalAlignment alignment = wux::HorizontalAlignment::Right;
    bool hideAccountButton = false;
    bool invertButtons     = true;
    bool closeStartMenu    = true;
    int  buttonSpacing     = 2;
    int  containerMarginLeft  = -16;
    int  containerMarginRight = -16;
};

struct ActionItem {
    std::wstring name;
    std::wstring icon;
    std::wstring action;
    std::vector<ActionItem> submenu;
};

static Settings g_settings;
static std::vector<ActionItem> g_buttons;

static winrt::weak_ref<wuxc::Panel>             g_parentPanel{nullptr};
static winrt::weak_ref<wux::FrameworkElement>    g_originalPowerButton{nullptr};
static winrt::weak_ref<wuxc::StackPanel>         g_buttonContainer{nullptr};

static HWND   g_proxyWindow   = NULL;
static HANDLE g_proxyThread   = NULL;
static DWORD  g_proxyThreadId = 0;

static winrt::event_token g_visibilityToken{};
static winrt::event_token g_activatedToken{};

// -------------------- string helpers --------------------

static std::wstring Trim(std::wstring s) {
    auto isSpace = [](wchar_t ch) { return iswspace(ch) != 0; };
    while (!s.empty() && isSpace(s.front())) s.erase(s.begin());
    while (!s.empty() && isSpace(s.back()))  s.pop_back();
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
                    if      (c >= L'0' && c <= L'9') value |= (c - L'0');
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

// -------------------- icon helpers --------------------

static bool IsImagePath(const std::wstring& icon) {
    if (icon.size() < 4) return false;
    if (icon.size() >= 2 && icon[1] == L':') return true;
    if (icon.size() >= 2 && icon[0] == L'\\' && icon[1] == L'\\') return true;
    std::wstring lower = ToLower(icon);
    if (lower.size() >= 4) {
        std::wstring ext = lower.substr(lower.size() - 4);
        if (ext == L".png" || ext == L".ico" || ext == L".jpg" || ext == L".bmp")
            return true;
    }
    if (lower.size() >= 5) {
        std::wstring ext = lower.substr(lower.size() - 5);
        if (ext == L".jpeg" || ext == L".webp")
            return true;
    }
    return false;
}

static std::wstring MakeFileUri(const std::wstring& path) {
    std::wstring uri = path;
    for (auto& ch : uri) {
        if (ch == L'\\') ch = L'/';
    }
    return L"file:///" + uri;
}

static wux::UIElement CreateButtonIconElement(const std::wstring& iconStr, double size) {
    if (!iconStr.empty() && IsImagePath(iconStr)) {
        try {
            wuxc::Image imgElement;
            imgElement.Width(size);
            imgElement.Height(size);
            imgElement.Stretch(wuxm::Stretch::Uniform);

            wuxmi::BitmapImage bmpImg;
            bmpImg.DecodePixelWidth((int)size * 2);
            bmpImg.DecodePixelHeight((int)size * 2);
            bmpImg.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(iconStr)));

            bmpImg.ImageFailed([iconStr](auto&&, auto&&) {
                Wh_Log(L"Failed to load icon image: %s", iconStr.c_str());
            });

            imgElement.Source(bmpImg);
            return imgElement;
        } catch (...) {
            Wh_Log(L"Exception creating image icon: %s", iconStr.c_str());
        }
    }

    wuxc::FontIcon fi;
    fi.Glyph(iconStr.empty() ? L"\uE712" : iconStr);
    fi.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    fi.FontSize(size);
    return fi;
}

static wuxc::IconElement CreateMenuIconElement(const std::wstring& iconStr, double size) {
    if (!iconStr.empty() && IsImagePath(iconStr)) {
        try {
            wuxc::BitmapIcon bi;
            bi.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(iconStr)));
            bi.ShowAsMonochrome(false);
            bi.Width(size);
            bi.Height(size);
            return bi;
        } catch (...) {
            Wh_Log(L"Exception creating menu image icon: %s", iconStr.c_str());
        }
    }

    wuxc::FontIcon fi;
    fi.Glyph(iconStr.empty() ? L"\uE712" : iconStr);
    fi.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    fi.FontSize(size);
    return fi;
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

    Wh_Log(L"Settings loaded: CloseStartMenu=%d", newSettings.closeStartMenu);
}

static std::wstring GetSettingStringIndex(const wchar_t* key, int index) {
    PCWSTR s = Wh_GetStringSetting(L"buttons[%d].%s", index, key);
    std::wstring result = s ? s : L"";
    SafeFreeString(s);
    return result;
}

// -------------------- submenu parsing (legacy syntax) --------------------

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
            Wh_Log(L"ParseActionGroup: missing '}' near '%s'", name.c_str());
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

// -------------------- submenu loading (nested array) --------------------

static std::vector<ActionItem> LoadSubmenuFromSettings(int buttonIndex) {
    std::vector<ActionItem> items;

    for (int j = 0; j < 64; j++) {
        PCWSTR nameStr = Wh_GetStringSetting(L"buttons[%d].submenu[%d].name",
                                              buttonIndex, j);
        std::wstring name = nameStr ? nameStr : L"";
        SafeFreeString(nameStr);
        if (name.empty()) break;

        PCWSTR iconStr   = Wh_GetStringSetting(L"buttons[%d].submenu[%d].icon",
                                                buttonIndex, j);
        PCWSTR actionStr = Wh_GetStringSetting(L"buttons[%d].submenu[%d].action",
                                                buttonIndex, j);

        ActionItem item;
        item.name   = Trim(name);
        item.icon   = Trim(DecodeEscapes(iconStr ? iconStr : L""));
        item.action = Trim(actionStr ? actionStr : L"");

        SafeFreeString(iconStr);
        SafeFreeString(actionStr);

        items.push_back(std::move(item));
    }

    return items;
}

// -------------------- build buttons --------------------

static void BuildButtons() {
    std::vector<ActionItem> newButtons;

    for (int i = 0; i < 128; i++) {
        std::wstring name = GetSettingStringIndex(L"name", i);
        if (name.empty()) break;

        std::wstring icon   = DecodeEscapes(GetSettingStringIndex(L"icon", i));
        std::wstring action = GetSettingStringIndex(L"action", i);

        ActionItem item;
        item.name   = Trim(name);
        item.icon   = Trim(icon);
        item.action = Trim(action);

        if (item.icon.empty() && !item.action.empty()) {
            std::wstring s = Trim(item.action);
            bool needsAutoIcon = false;

            if (StartsWithNoCase(s, L"~")) {
                needsAutoIcon = true;
            } else if (StartsWithNoCase(s, L"http://") || StartsWithNoCase(s, L"https://")) {
                needsAutoIcon = true;
            } else if (StartsWithNoCase(s, L"ms-settings:")) {
                needsAutoIcon = true;
            } else if (StartsWithNoCase(s, L"shell:") && !StartsWithNoCase(s, L"shell:shutdown") && !StartsWithNoCase(s, L"shell:rundll")) {
                needsAutoIcon = true;
            } else {
                s = StripOuterQuotes(s);
                std::wstring lower = ToLower(s);
                if (lower.ends_with(L".exe") || lower.ends_with(L".lnk")) {
                    needsAutoIcon = true;
                }
            }

            if (needsAutoIcon) {
                if (StartsWithNoCase(s, L"ms-settings:")) {
                    item.icon = L"\uE713";
                } else if (StartsWithNoCase(s, L"http://") || StartsWithNoCase(s, L"https://")) {
                    item.icon = L"\uE774";
                } else if (StartsWithNoCase(s, L"shell:")) {
                    item.icon = L"\uE8B7";
                } else {
                    item.icon = L"\uE7EE";
                }
            }
        }

        // Try nested settings array first (user-friendly way)
        item.submenu = LoadSubmenuFromSettings(i);

        // Fallback: parse legacy {...} syntax from action field
        if (item.submenu.empty() && !item.action.empty()) {
            auto parsed = ParseActionGroup(item.action);
            if (!parsed.empty()) {
                item.submenu = std::move(parsed);
            }
        }

        newButtons.push_back(std::move(item));
    }

    bool hasSettings = false;
    for (const auto& btn : newButtons) {
        if (btn.action == L"ms-settings:") {
            hasSettings = true;
            break;
        }
    }
    if (!hasSettings) {
        ActionItem settingsItem;
        settingsItem.name = L"Settings";
        settingsItem.icon = L"\uE713";
        settingsItem.action = L"ms-settings:";
        newButtons.insert(newButtons.begin(), settingsItem);
    }

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_buttons = std::move(newButtons);
}

// -------------------- start menu helpers --------------------

static bool ShouldCloseStartMenu() {
    return g_closeStartMenu.load(std::memory_order_relaxed);
}

static void SendEscapeKey() {
    INPUT inputs[2]{};
    inputs[0].type       = INPUT_KEYBOARD;
    inputs[0].ki.wVk     = VK_ESCAPE;
    inputs[1].type       = INPUT_KEYBOARD;
    inputs[1].ki.wVk     = VK_ESCAPE;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
}

static inline void CloseStartMenuAfterClick() {
    if (!ShouldCloseStartMenu()) return;
    try {
        auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (dq) {
            dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, []() {
                std::thread([]() { Sleep(100); SendEscapeKey(); }).detach();
            });
            return;
        }
    } catch (...) {}
    std::thread([]() { Sleep(100); SendEscapeKey(); }).detach();
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

static bool SearchRecursiveImpl(const std::wstring& root,
                                const std::wstring& fileName,
                                int depth, std::wstring& found) {
    if (depth < 0) return false;
    WIN32_FIND_DATAW fd{};
    std::wstring mask = root;
    if (!mask.empty() && mask.back() != L'\\') mask += L'\\';
    mask += L"*";

    HANDLE h = FindFirstFileW(mask.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) continue;
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
    auto addEnv = [&](const wchar_t* n) {
        wchar_t tmp[MAX_PATH * 2]{};
        if (GetEnvironmentVariableW(n, tmp, ARRAYSIZE(tmp)) > 0) roots.push_back(tmp);
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

static std::wstring GetExeIconPath(const std::wstring& action) {
    std::wstring s = Trim(action);
    if (s.empty()) return L"";

    if (StartsWithNoCase(s, L"http://") || StartsWithNoCase(s, L"https://") ||
        StartsWithNoCase(s, L"shell:") || StartsWithNoCase(s, L"ms-settings:") ||
        StartsWithNoCase(s, L"cmd:")) {
        return L"";
    }

    return L"";
}

static bool ExecuteHiddenProcess(const std::wstring& commandLine, bool useCmdExe) {
    std::wstring cmdLine = useCmdExe ? (L"cmd.exe /C " + commandLine) : commandLine;
    std::vector<wchar_t> mutableCmd(cmdLine.begin(), cmdLine.end());
    mutableCmd.push_back(L'\0');

    STARTUPINFOW si{};
    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

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

static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT message,
                                     WPARAM wParam, LPARAM lParam) {
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
        case WM_CLOSE:  DestroyWindow(hWnd); return 0;
        case WM_DESTROY: g_proxyWindow = NULL; PostQuitMessage(0); return 0;
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
        Wh_Log(L"Proxy: RegisterClassEx failed %lu", GetLastError());
        return 1;
    }
    g_proxyWindow = CreateWindowExW(
        0, PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME, 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    if (!g_proxyWindow) {
        Wh_Log(L"Proxy: CreateWindowEx failed %lu", GetLastError());
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
    g_proxyThread = CreateThread(nullptr, 0, ProxyWindowThread,
                                nullptr, 0, &g_proxyThreadId);
    if (!g_proxyThread)
        Wh_Log(L"Proxy: CreateThread failed %lu", GetLastError());
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
            menuItem.Icon(CreateMenuIconElement(item.icon, 16));
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
                           wux::FrameworkElement) {
    try {
        Settings currentSettings;
        std::vector<ActionItem> currentButtons;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            currentSettings = g_settings;
            currentButtons  = g_buttons;
        }

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
                bool isPowerButton = (fe.Name() == L"PowerButton");
                bool isUserTile = (fe.Name() == L"UserTileButton" ||
                                   fe.Name() == L"UserTile" ||
                                   fe.Name() == L"ProfileButton");
                if (isPowerButton) {
                    fe.Visibility(wux::Visibility::Visible);
                    continue;
                }
                if (!currentSettings.hideAccountButton && isUserTile) {
                    fe.Visibility(wux::Visibility::Visible);
                    continue;
                }
                fe.Visibility(wux::Visibility::Collapsed);
            }
        }

        wuxc::StackPanel container = g_buttonContainer.get();
        if (container) {
            try {
                auto parent = wuxm::VisualTreeHelper::GetParent(container);
                if (!parent || parent != parentPanel) {
                    container = nullptr;
                    g_buttonContainer = nullptr;
                }
            } catch (...) {
                container = nullptr;
                g_buttonContainer = nullptr;
            }
        }

        if (!container) {
            for (uint32_t i = 0; i < children.Size(); i++) {
                auto child = children.GetAt(i);
                if (auto panel = child.try_as<wuxc::StackPanel>()) {
                    auto tag = panel.Tag();
                    if (tag && winrt::unbox_value_or<winrt::hstring>(tag, L"") == CONTAINER_TAG) {
                        container = panel;
                        g_buttonContainer = container;
                        break;
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
        container.Margin({(double)currentSettings.containerMarginLeft, 0,
                          (double)currentSettings.containerMarginRight, 0});

        bool needRebuild = g_forceRebuild.exchange(false) ||
                           container.Children().Size() != currentButtons.size();
        if (!needRebuild) return;

        container.Children().Clear();

        for (size_t i = 0; i < currentButtons.size(); i++) {
            const auto& btnDef = currentButtons[i];
            int btnIdx = origIdx[i];

            wuxc::Button btn;
            btn.Width(40);
            btn.Height(40);
            btn.Margin({4, 0, 4, 0});
            btn.Padding({0, 0, 0, 0});
            btn.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
            btn.BorderThickness({0, 0, 0, 0});

            wux::UIElement iconEl = CreateButtonIconElement(btnDef.icon, 16);
            btn.Content(iconEl);
            wuxc::ToolTipService::SetToolTip(btn, winrt::box_value(btnDef.name));

            if (!btnDef.submenu.empty()) {
                auto submenu = btnDef.submenu;
                btn.Click([submenu, btn](auto&&, auto&&) {
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

// -------------------- visibility monitoring --------------------

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
            if (args.WindowActivationState() !=
                wuc::CoreWindowActivationState::Deactivated)
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
            if (g_visibilityToken)
                coreWindow.VisibilityChanged(std::exchange(g_visibilityToken, {}));
            if (g_activatedToken)
                coreWindow.Activated(std::exchange(g_activatedToken, {}));
        }
    } catch (...) {}

    try {
        auto panel       = g_parentPanel.get();
        auto container   = g_buttonContainer.get();
        auto originalBtn = g_originalPowerButton.get();

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

// -------------------- retry timer --------------------

static HWND g_retryHwnd = NULL;
static constexpr UINT_PTR kRetryTimerId = 0xC1A30B7D;

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

// -------------------- hooks --------------------

using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, PVOID, DWORD);
static CreateWindowInBand_t CreateWindowInBand_Original;

static HWND WINAPI CreateWindowInBand_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
    PVOID lpParam, DWORD dwBand)
{
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (hWnd && lpClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0)
        InitWithRetry(hWnd);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, PVOID, DWORD, DWORD);
static CreateWindowInBandEx_t CreateWindowInBandEx_Original;

static HWND WINAPI CreateWindowInBandEx_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
    PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags)
{
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance,
        lpParam, dwBand, dwTypeFlags);
    if (hWnd && lpClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0)
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
        if (!GetWindowThreadProcessId(w, &pid) ||
            pid != GetCurrentProcessId()) return TRUE;
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
    Wh_Log(L"Init - Custom App Launcher");
    LoadSettings();
    BuildButtons();

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
    Wh_Log(L"Uninit - Custom App Launcher");
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
