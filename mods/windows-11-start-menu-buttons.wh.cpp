// ==WindhawkMod==
// @id            windows-11-start-menu-buttons
// @name          Windows 11 Start Menu Buttons
// @description         Customizable buttons for the Windows 11 Start menu.
// @description:ru-RU   Настраиваемые кнопки для меню «Пуск» Windows 11.
// @version       2.0
// @author        Salyts
// @license       MIT
// @github        https://github.com/Salyts
// @include       StartMenuExperienceHost.exe
// @include       explorer.exe
// @architecture  x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -luuid -luser32 -lwtsapi32 -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Start Menu Buttons

Replaces the default bottom row of the Windows 11 Start menu with fully customizable buttons.

Thank you so much, [@SharkIT-sys](https://github.com/SharkIT-sys), for helping to improve the mod!

![demo](https://i.imgur.com/T42w89W.gif)

![img](https://i.imgur.com/J5CC8XP.png)

![img](https://i.imgur.com/dm7SVQj.png)

❗**There may be issues with mods:** Windows 11 Start Menu Styler, Windows 11 Start Menu Power Buttons.

---

## ✨ Quick Start

1. Open Windhawk settings for this mod.
2. Add buttons using the **Buttons** list.
3. For each button, pick a **Preset** or set **Preset = Custom** and fill in Name, Icon, Action.
4. Save — the Start menu updates immediately.

---

## ❔ Button fields

| Field    | Description |
|----------|-------------|
| **Preset** | Ready-made button. Set to `Custom` to define your own. |
| **Name** | Tooltip shown on hover. Leave empty on presets to use their default name. |
| **Icon** | [Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font) glyph (e.g. `\uE7E8`) **or** full path to an image file (PNG, ICO, JPG, BMP). |
| **Action** | Used only when Preset = `Custom`. See action formats below. |
| **Submenu** | Optional list of child items shown in a flyout. If any submenu items are defined, the button opens the flyout instead of running a direct action. |

---

## ❔ Action formats (Custom preset only)

| Prefix | Example | Description |
|--------|---------|-------------|
| `" "` | `"C:\Program Files\app.exe"` | Opens a file or folder by absolute path. |
| `~` | `~Downloads` | Opens a folder or file by name. |
| `cmd:` | `cmd:control` | Runs a command through `cmd.exe`. |
| `shell:` | `shell:shutdown /r /f /t 0` | Runs through `powershell.exe`. |
| `web:` | `web:https://windhawk.net/` | Opens a URL in the default browser. |
| `ms-settings:` | `ms-settings:bluetooth` | Opens a Windows Settings page. |

---

## 🖼️ Image icons

Set the **Icon** field to a full path of an image file, e.g.:

`C:\Icons\chrome.png`

Supported formats: `.png`, `.ico`, `.jpg`, `.jpeg`, `.bmp`, `.webp`

Recommended size: 32×32 px, transparent background.

---

## #️⃣ Submenus

Fill in the **Submenu** entries for a button. Each submenu item has its own Name, Icon, and Action.
When at least one submenu item exists, the button opens a flyout menu instead of executing a direct action.

---
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- preset_language: en
  $name: Preset language
  $description: Language for the tooltip names of preset buttons.
  $options:
    - en: English
    - ru: Russian (Русский)

- alignment: right
  $name: Button alignment
  $description: Horizontal alignment of the custom button container within the Start menu bottom bar.
  $options:
    - left: Left
    - center: Center
    - right: Right

- account_button: left
  $name: Account button
  $description: Position or visibility of the user account avatar button.
  $options:
    - left: Left
    - center: Center
    - right: Right
    - hide: Hide

- invert_buttons: true
  $name: Invert button order
  $description: Reverses the order in which custom buttons appear. Useful when alignment is set to Right.

- invert_icons_submenus: false
  $name: Invert icons in submenus
  $description: Reverses the order in which icons are displayed in submenus.

- close_start_menu: true
  $name: Close Start menu after click
  $description: Automatically closes the Start menu after clicking any custom button (except buttons that open a submenu).

- button_spacing: 0
  $name: Spacing between buttons (px)
  $description: Horizontal margin between each button.

- container_margin_left: -16
  $name: Container left margin (px)
  $description: Left margin of the custom button container. Negative values let it overlap the default padding.

- container_margin_right: -16
  $name: Container right margin (px)
  $description: Right margin of the custom button container. Negative values let it overlap the default padding.

- buttons:
    - - Preset: menu_shutdown
        $name: Preset
        $description: "Choose a built-in preset button, or select Custom to define your own Name / Icon / Action."
        $options: 
          - custom: Custom
          - settings: Settings
          - explorer: Explorer
          - documents: Documents
          - downloads: Downloads
          - music: Music
          - pictures: Pictures
          - videos: Videos
          - network: Network
          - personal_folder: Personal Folder
          - shutdown: Shut down
          - restart: Restart
          - sign_out: Sign out
          - sleep: Sleep
          - hibernate: Hibernate
          - lock: Lock
          - menu_shutdown: Power Menu
      - Name: ""
        $name: Name
        $description: "Tooltip shown on hover. Leave empty on preset buttons to use the preset default."
      - Icon: ""
        $name: Icon
        $description: "Segoe Fluent glyph (e.g. \\uE7E8) or full image path (e.g. C:\\Icons\\app.png). Leave empty for preset default."
      - Action: ""
        $name: Action
        $description: "Only used when Preset = Custom. See mod description for supported formats."
      - submenu:
          - - name: ""
              $name: Item name
            - icon: ""
              $name: Item icon
            - action: ""
              $name: Item action
        $name: Submenu items
        $description: "If any items are added here, the button will open a flyout instead of running the direct action."
    - - Preset: settings
    - - Preset: explorer
  $name: Buttons
  $description: "Only used when Preset = Custom. See mod description for supported formats."
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


static const wchar_t* CONTAINER_TAG      = L"WH_SMB_Container";
static const wchar_t* PROXY_WINDOW_CLASS = L"WH_SMB_Proxy_Class";
static const wchar_t* PROXY_WINDOW_NAME  = L"WH_SMB_Proxy_Window";
static const wchar_t* THREAD_CALL_MSG    = L"WH_SMB_ThreadCall";
static constexpr ULONG_PTR kCopyDataMagic = 0x534D4231;
static const wchar_t* FALLBACK_ICON = L"\uE783";

static std::atomic<bool> g_initialized{false};
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_buttonsInjected{false};
static std::atomic<bool> g_monitoringActive{false};
static std::atomic<bool> g_forceRebuild{false};
static std::atomic<bool> g_closeStartMenu{true};
static std::atomic<bool> g_invertIconsSubmenus{false};

static std::mutex g_settingsMutex;

enum class AccountButtonMode { Left, Center, Right, Hide };

struct Settings {
    wux::HorizontalAlignment alignment         = wux::HorizontalAlignment::Right;
    AccountButtonMode        accountButton     = AccountButtonMode::Left;
    bool                     invertButtons     = true;
    bool                     invertIconsSubmenus = false;
    bool                     closeStartMenu    = true;
    int                      buttonSpacing     = 2;
    int                      containerMarginLeft  = -16;
    int                      containerMarginRight = -16;
    bool                     langRussian       = false;
};

struct ActionItem {
    std::wstring name;
    std::wstring icon;
    std::wstring action;
    std::vector<ActionItem> submenu;
};

static Settings              g_settings;
static std::vector<ActionItem> g_buttons;

static winrt::weak_ref<wuxc::Panel>           g_parentPanel{nullptr};
static winrt::weak_ref<wux::FrameworkElement>  g_originalPowerButton{nullptr};
static winrt::weak_ref<wuxc::StackPanel>       g_buttonContainer{nullptr};

static HWND   g_proxyWindow   = NULL;
static HANDLE g_proxyThread   = NULL;
static DWORD  g_proxyThreadId = 0;

static winrt::event_token g_visibilityToken{};
static winrt::event_token g_activatedToken{};

static std::wstring Trim(std::wstring s) {
    auto sp = [](wchar_t c) { return !!iswspace(c); };
    while (!s.empty() && sp(s.front())) s.erase(s.begin());
    while (!s.empty() && sp(s.back()))  s.pop_back();
    return s;
}

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

static bool StartsWithCI(const std::wstring& s, const std::wstring& p) {
    return s.size() >= p.size() && _wcsnicmp(s.c_str(), p.c_str(), p.size()) == 0;
}

static std::wstring StripOuterQuotes(const std::wstring& s) {
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"')
        return s.substr(1, s.size() - 2);
    return s;
}

static std::wstring DecodeEscapes(const std::wstring& in) {
    std::wstring out;
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == L'\\' && i + 1 < in.size()) {
            wchar_t n = in[i + 1];
            if (n == L'u' && i + 5 < in.size()) {
                unsigned v = 0; bool ok = true;
                for (size_t j = i + 2; j < i + 6; ++j) {
                    wchar_t c = in[j]; v <<= 4;
                    if      (c >= L'0' && c <= L'9') v |= (c - L'0');
                    else if (c >= L'a' && c <= L'f') v |= (10 + c - L'a');
                    else if (c >= L'A' && c <= L'F') v |= (10 + c - L'A');
                    else { ok = false; break; }
                }
                if (ok) { out.push_back((wchar_t)v); i += 5; continue; }
            }
            if (n == L'\\') { out.push_back(L'\\'); ++i; continue; }
            if (n == L'"')  { out.push_back(L'"');  ++i; continue; }
            if (n == L'n')  { out.push_back(L'\n'); ++i; continue; }
            if (n == L'r')  { out.push_back(L'\r'); ++i; continue; }
            if (n == L't')  { out.push_back(L'\t'); ++i; continue; }
        }
        out.push_back(in[i]);
    }
    return out;
}

static void SafeFreeString(PCWSTR s) { if (s) Wh_FreeStringSetting(s); }

static bool IsImagePath(const std::wstring& s) {
    if (s.size() < 4) return false;
    if (s.size() >= 2 && s[1] == L':') return true;
    if (s.size() >= 2 && s[0] == L'\\' && s[1] == L'\\') return true;
    std::wstring lo = ToLower(s);
    for (auto& ext : { std::wstring(L".png"), std::wstring(L".ico"),
                       std::wstring(L".jpg"), std::wstring(L".jpeg"),
                       std::wstring(L".bmp"), std::wstring(L".webp") }) {
        if (lo.size() >= ext.size() &&
            lo.compare(lo.size() - ext.size(), ext.size(), ext) == 0)
            return true;
    }
    return false;
}

static std::wstring MakeFileUri(const std::wstring& path) {
    std::wstring u = path;
    for (auto& c : u) if (c == L'\\') c = L'/';
    return L"file:///" + u;
}

static wux::UIElement MakeButtonIcon(const std::wstring& iconStr) {
    const std::wstring& glyph =
        (!iconStr.empty() && !IsImagePath(iconStr)) ? iconStr : std::wstring();

    if (!iconStr.empty() && IsImagePath(iconStr)) {
        if (GetFileAttributesW(iconStr.c_str()) != INVALID_FILE_ATTRIBUTES) {
            try {
                wuxc::Image img;
                img.Width(16); img.Height(16);
                img.Stretch(wuxm::Stretch::Uniform);
                wuxmi::BitmapImage bmp;
                bmp.DecodePixelWidth(32); bmp.DecodePixelHeight(32);
                bmp.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(iconStr)));
                bmp.ImageFailed([iconStr](auto&&, auto&&) {
                    Wh_Log(L"Icon load failed: %s", iconStr.c_str());
                });
                img.Source(bmp);
                return img;
            } catch (...) {
                Wh_Log(L"Exception creating image icon for: %s", iconStr.c_str());
            }
        } else {
            Wh_Log(L"Icon file not found: %s", iconStr.c_str());
        }
    }

    wuxc::FontIcon fi;
    fi.Glyph(!glyph.empty() ? glyph : FALLBACK_ICON);
    fi.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    fi.FontSize(16);
    return fi;
}

static wuxc::IconElement MakeMenuIcon(const std::wstring& iconStr) {
    if (!iconStr.empty() && IsImagePath(iconStr)) {
        if (GetFileAttributesW(iconStr.c_str()) != INVALID_FILE_ATTRIBUTES) {
            try {
                wuxc::BitmapIcon bi;
                bi.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(iconStr)));
                bi.ShowAsMonochrome(false);
                bi.Width(16); bi.Height(16);
                return bi;
            } catch (...) {
                Wh_Log(L"Exception creating menu image icon for: %s", iconStr.c_str());
            }
        } else {
            Wh_Log(L"Menu icon file not found: %s", iconStr.c_str());
        }
    }
    wuxc::FontIcon fi;
    fi.Glyph((!iconStr.empty() && !IsImagePath(iconStr)) ? iconStr : FALLBACK_ICON);
    fi.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    fi.FontSize(16);
    return fi;
}

struct PresetDef {
    const wchar_t* key;
    const wchar_t* nameEn;
    const wchar_t* nameRu;
    const wchar_t* icon;
    const wchar_t* action;
};

static const PresetDef kPresets[] = {
    { L"settings",        L"Settings",        L"Параметры",          L"\uE713", L"__preset:settings"        },
    { L"explorer",        L"Explorer",        L"Проводник",          L"\uEC50", L"__preset:explorer"        },
    { L"documents",       L"Documents",       L"Документы",          L"\uE8A5", L"__preset:documents"       },
    { L"downloads",       L"Downloads",       L"Загрузки",           L"\uE896", L"__preset:downloads"       },
    { L"music",           L"Music",           L"Музыка",             L"\uEC4F", L"__preset:music"           },
    { L"pictures",        L"Pictures",        L"Изображения",        L"\uE91B", L"__preset:pictures"        },
    { L"videos",          L"Videos",          L"Видео",              L"\uE714", L"__preset:videos"          },
    { L"network",         L"Network",         L"Сеть",               L"\uEC27", L"__preset:network"         },
    { L"personal_folder", L"Personal Folder", L"Личная папка",       L"\uEC25", L"__preset:personal_folder" },
    { L"shutdown",        L"Shut down",       L"Завершение работы",  L"\uE7E8", L"__preset:shutdown"        },
    { L"restart",         L"Restart",         L"Перезагрузка",       L"\uE777", L"__preset:restart"         },
    { L"sign_out",        L"Sign out",        L"Выйти",              L"\uF3B1", L"__preset:sign_out"        },
    { L"sleep",           L"Sleep",           L"Спящий режим",       L"\uE708", L"__preset:sleep"           },
    { L"hibernate",       L"Hibernate",       L"Гибернация",         L"\uE823", L"__preset:hibernate"       },
    { L"lock",            L"Lock",            L"Блокировка",         L"\uE72E", L"__preset:lock"            },
    { L"menu_shutdown",   L"Power",           L"Выключение",         L"\uE7E8", nullptr                     },
};
static constexpr int kPresetCount = (int)(sizeof(kPresets) / sizeof(kPresets[0]));

static const PresetDef* FindPreset(const std::wstring& key) {
    std::wstring k = ToLower(Trim(key));
    if (k.empty() || k == L"custom") return nullptr;
    for (int i = 0; i < kPresetCount; ++i)
        if (k == kPresets[i].key) return &kPresets[i];
    Wh_Log(L"Unknown preset key: '%s'", key.c_str());
    return nullptr;
}

static std::wstring PresetName(const PresetDef& pd, bool ru) {
    return ru ? pd.nameRu : pd.nameEn;
}

static bool EnableShutdownPrivilege() {
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    TOKEN_PRIVILEGES tkp{};
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    bool ok = LookupPrivilegeValueW(NULL, SE_SHUTDOWN_NAME,
                                    &tkp.Privileges[0].Luid) &&
              AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, NULL);
    CloseHandle(hToken);
    return ok;
}

static bool GetKnownFolderPath(const wchar_t* id, std::wstring& out) {
    KNOWNFOLDERID fid{};
    std::wstring n = ToLower(id);
    if      (n == L"downloads")                    fid = FOLDERID_Downloads;
    else if (n == L"documents" || n == L"personal") fid = FOLDERID_Documents;
    else if (n == L"music")                        fid = FOLDERID_Music;
    else if (n == L"pictures")                     fid = FOLDERID_Pictures;
    else if (n == L"videos")                       fid = FOLDERID_Videos;
    else if (n == L"desktop")                      fid = FOLDERID_Desktop;
    else if (n == L"profile" || n == L"home")      fid = FOLDERID_Profile;
    else return false;

    PWSTR raw = nullptr;
    bool ok = SUCCEEDED(SHGetKnownFolderPath(fid, 0, nullptr, &raw)) && raw;
    if (ok) out = raw;
    if (raw) CoTaskMemFree(raw);
    return ok;
}

static void OpenKnownFolder(const wchar_t* name) {
    std::wstring path;
    if (GetKnownFolderPath(name, path))
        ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static bool PerformPresetAction(const std::wstring& action) {
    if (!StartsWithCI(action, L"__preset:")) return false;
    std::wstring key = ToLower(action.substr(9));

    if (key == L"settings") {
        ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL);
    } else if (key == L"explorer") {
        ShellExecuteW(nullptr, L"open", L"explorer.exe", nullptr, nullptr, SW_SHOWNORMAL);
    } else if (key == L"documents") {
        OpenKnownFolder(L"documents");
    } else if (key == L"downloads") {
        OpenKnownFolder(L"downloads");
    } else if (key == L"music") {
        OpenKnownFolder(L"music");
    } else if (key == L"pictures") {
        OpenKnownFolder(L"pictures");
    } else if (key == L"videos") {
        OpenKnownFolder(L"videos");
    } else if (key == L"network") {
        ShellExecuteW(nullptr, L"open", L"shell:NetworkPlacesFolder",
                      nullptr, nullptr, SW_SHOWNORMAL);
    } else if (key == L"personal_folder") {
        OpenKnownFolder(L"profile");
    } else if (key == L"shutdown") {
        EnableShutdownPrivilege();
        ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
    } else if (key == L"restart") {
        EnableShutdownPrivilege();
        ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
    } else if (key == L"sign_out") {
        EnableShutdownPrivilege();
        ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);
    } else if (key == L"sleep") {
        SetSuspendState(FALSE, FALSE, FALSE);
    } else if (key == L"hibernate") {
        SetSuspendState(TRUE, FALSE, FALSE);
    } else if (key == L"lock") {
        LockWorkStation();
    } else {
        return false;
    }
    return true;
}

static bool SearchRecursive(const std::wstring& root, const std::wstring& name,
                            int depth, std::wstring& out) {
    if (depth < 0) return false;
    WIN32_FIND_DATAW fd{};
    std::wstring mask = root;
    if (!mask.empty() && mask.back() != L'\\') mask += L'\\';
    mask += L"*";
    HANDLE h = FindFirstFileW(mask.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;
    do {
        if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")) continue;
        std::wstring full = root;
        if (!full.empty() && full.back() != L'\\') full += L'\\';
        full += fd.cFileName;
        if (!_wcsicmp(fd.cFileName, name.c_str())) {
            out = full; FindClose(h); return true;
        }
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
            if (SearchRecursive(full, name, depth - 1, out)) {
                FindClose(h); return true;
            }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return false;
}

static bool SearchByName(const std::wstring& name, std::wstring& out) {
    wchar_t buf[MAX_PATH * 4]{};
    if (SearchPathW(nullptr, name.c_str(), nullptr, ARRAYSIZE(buf), buf, nullptr)) {
        out = buf; return true;
    }
    std::vector<std::wstring> roots;
    auto addEnv = [&](const wchar_t* v) {
        wchar_t tmp[MAX_PATH * 2]{};
        if (GetEnvironmentVariableW(v, tmp, ARRAYSIZE(tmp))) roots.push_back(tmp);
    };
    addEnv(L"ProgramFiles"); addEnv(L"ProgramFiles(x86)"); addEnv(L"ProgramW6432");
    addEnv(L"SystemRoot");   addEnv(L"LOCALAPPDATA");       addEnv(L"APPDATA");
    addEnv(L"USERPROFILE");
    for (auto& fold : { L"desktop", L"documents", L"downloads",
                        L"music",   L"pictures",  L"videos" }) {
        std::wstring p;
        if (GetKnownFolderPath(fold, p)) roots.push_back(p);
    }
    for (auto& r : roots)
        if (SearchRecursive(r, name, 5, out)) return true;
    return false;
}

static bool ExecuteProcess(const std::wstring& cmd, bool useCmdExe) {
    std::wstring line = useCmdExe ? (L"cmd.exe /C " + cmd) : cmd;
    std::vector<wchar_t> buf(line.begin(), line.end());
    buf.push_back(L'\0');
    STARTUPINFOW si{}; si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW; si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(nullptr, buf.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
        return false;
    CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    return true;
}

static void ExecuteActionText(const std::wstring& raw) {
    std::wstring a = Trim(raw);
    if (a.empty()) return;

    if (PerformPresetAction(a)) return;

    std::thread([a]() {
        if (StartsWithCI(a, L"cmd:"))          { ExecuteProcess(Trim(a.substr(4)), true);  return; }
        if (StartsWithCI(a, L"shell:")) { 
            ExecuteProcess(L"powershell.exe -NoProfile -ExecutionPolicy Bypass -Command " + Trim(a.substr(6)), false); 
            return; 
        }
        if (StartsWithCI(a, L"ms-settings:")) {
            ShellExecuteW(nullptr, L"open", a.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }
        if (StartsWithCI(a, L"web:")) {
            ShellExecuteW(nullptr, L"open", Trim(a.substr(4)).c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }
        if (a.front() == L'"') {
            ShellExecuteW(nullptr, L"open", StripOuterQuotes(a).c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }
        if (a.front() == L'~') {
            std::wstring tgt = Trim(a.substr(1));
            if (tgt.empty()) {
                Wh_Log(L"~search: empty target, ignoring");
                return;
            }
            std::wstring resolved;
            if (GetKnownFolderPath(tgt.c_str(), resolved) ||
                SearchByName(tgt, resolved)) {
                ShellExecuteW(nullptr, L"open", resolved.c_str(),
                              nullptr, nullptr, SW_SHOWNORMAL);
                return;
            }
            Wh_Log(L"~search: '%s' not found", tgt.c_str());
            return;
        }
        ExecuteProcess(a, false);
    }).detach();
}

static void SendEscapeKey() {
    INPUT in[2]{};
    in[0].type = in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = in[1].ki.wVk = VK_ESCAPE;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static void CloseStartMenuAfterClick() {
    if (!g_closeStartMenu.load(std::memory_order_relaxed)) return;
    try {
        auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (dq) {
            dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, []() {
                std::thread([] { Sleep(100); SendEscapeKey(); }).detach();
            });
            return;
        }
    } catch (...) {}
    std::thread([] { Sleep(100); SendEscapeKey(); }).detach();
}

static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT msg,
                                     WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        const COPYDATASTRUCT* cds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
        if (cds && cds->dwData == kCopyDataMagic &&
            cds->cbData == sizeof(int)) {
            int idx = *static_cast<const int*>(cds->lpData);
            std::lock_guard<std::mutex> lk(g_settingsMutex);
            if (idx >= 0 && idx < (int)g_buttons.size())
                ExecuteActionText(g_buttons[idx].action);
        }
        return 0;
    }
    switch (msg) {
        case WM_CLOSE:   DestroyWindow(hWnd); return 0;
        case WM_DESTROY: g_proxyWindow = NULL; PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static DWORD WINAPI ProxyWindowThread(LPVOID) {
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(wcex);
    wcex.lpfnWndProc   = ProxyWndProc;
    wcex.hInstance     = GetModuleHandleW(nullptr);
    wcex.lpszClassName = PROXY_WINDOW_CLASS;
    if (!RegisterClassExW(&wcex)) {
        Wh_Log(L"Proxy: RegisterClassEx failed %lu", GetLastError());
        return 1;
    }
    g_proxyWindow = CreateWindowExW(
        0, PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME, 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!g_proxyWindow) {
        Wh_Log(L"Proxy: CreateWindowEx failed %lu", GetLastError());
        UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandleW(nullptr));
        return 1;
    }
    MSG m{};
    while (GetMessageW(&m, nullptr, 0, 0)) {
        TranslateMessage(&m); DispatchMessageW(&m);
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

static bool SendActionToProxy(int idx) {
    HWND hw = FindWindowW(PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME);
    if (!hw) {
        std::lock_guard<std::mutex> lk(g_settingsMutex);
        if (idx >= 0 && idx < (int)g_buttons.size())
            ExecuteActionText(g_buttons[idx].action);
        return false;
    }
    COPYDATASTRUCT cds{ kCopyDataMagic, sizeof(int), &idx };
    return SendMessageW(hw, WM_COPYDATA, 0, (LPARAM)&cds) != 0;
}

static void LoadSettings() {
    Settings s;

    PCWSTR lang = Wh_GetStringSetting(L"preset_language");
    s.langRussian = lang && _wcsicmp(lang, L"ru") == 0;
    SafeFreeString(lang);

    PCWSTR align = Wh_GetStringSetting(L"alignment");
    if      (align && _wcsicmp(align, L"left")   == 0) s.alignment = wux::HorizontalAlignment::Left;
    else if (align && _wcsicmp(align, L"center") == 0) s.alignment = wux::HorizontalAlignment::Center;
    else                                                s.alignment = wux::HorizontalAlignment::Right;
    SafeFreeString(align);

    PCWSTR acct = Wh_GetStringSetting(L"account_button");
    if      (acct && _wcsicmp(acct, L"center") == 0) s.accountButton = AccountButtonMode::Center;
    else if (acct && _wcsicmp(acct, L"right")  == 0) s.accountButton = AccountButtonMode::Right;
    else if (acct && _wcsicmp(acct, L"hide")   == 0) s.accountButton = AccountButtonMode::Hide;
    else                                              s.accountButton = AccountButtonMode::Left;
    SafeFreeString(acct);

    s.invertButtons          = Wh_GetIntSetting(L"invert_buttons")          != 0;
    s.invertIconsSubmenus    = Wh_GetIntSetting(L"invert_icons_submenus")   != 0;
    s.closeStartMenu         = Wh_GetIntSetting(L"close_start_menu")        != 0;
    s.buttonSpacing          = Wh_GetIntSetting(L"button_spacing");
    s.containerMarginLeft    = Wh_GetIntSetting(L"container_margin_left");
    s.containerMarginRight   = Wh_GetIntSetting(L"container_margin_right");

    g_closeStartMenu.store(s.closeStartMenu, std::memory_order_relaxed);
    g_invertIconsSubmenus.store(s.invertIconsSubmenus, std::memory_order_relaxed);

    std::lock_guard<std::mutex> lk(g_settingsMutex);
    g_settings = s;
    Wh_Log(L"Settings loaded. lang=%s align=%d acct=%d invertIcons=%d",
           s.langRussian ? L"ru" : L"en",
           (int)s.alignment, (int)s.accountButton, (int)s.invertIconsSubmenus);
}

static std::vector<ActionItem> LoadSubmenu(int btnIdx) {
    std::vector<ActionItem> items;
    for (int j = 0; j < 64; ++j) {
        PCWSTR n = Wh_GetStringSetting(L"buttons[%d].submenu[%d].name",   btnIdx, j);
        std::wstring name = n ? n : L"";
        SafeFreeString(n);
        if (Trim(name).empty()) break;

        PCWSTR ic = Wh_GetStringSetting(L"buttons[%d].submenu[%d].icon",   btnIdx, j);
        PCWSTR ac = Wh_GetStringSetting(L"buttons[%d].submenu[%d].action", btnIdx, j);

        ActionItem item;
        item.name   = Trim(name);
        item.icon   = Trim(DecodeEscapes(ic ? ic : L""));
        item.action = Trim(ac ? ac : L"");
        SafeFreeString(ic); SafeFreeString(ac);
        items.push_back(std::move(item));
    }
    return items;
}

static void BuildButtons() {
    bool ruLang;
    {
        std::lock_guard<std::mutex> lk(g_settingsMutex);
        ruLang = g_settings.langRussian;
    }

    std::vector<ActionItem> newButtons;

    for (int i = 0; i < 128; ++i) {
        PCWSTR presetRaw = Wh_GetStringSetting(L"buttons[%d].Preset", i);
        std::wstring preset = presetRaw ? presetRaw : L"";
        SafeFreeString(presetRaw);

        PCWSTR nRaw = Wh_GetStringSetting(L"buttons[%d].Name",   i);
        PCWSTR iRaw = Wh_GetStringSetting(L"buttons[%d].Icon",   i);
        PCWSTR aRaw = Wh_GetStringSetting(L"buttons[%d].Action", i);

        std::wstring rawName   = nRaw ? nRaw : L"";
        std::wstring rawIcon   = DecodeEscapes(iRaw ? iRaw : L"");
        std::wstring rawAction = aRaw ? aRaw : L"";
        SafeFreeString(nRaw); SafeFreeString(iRaw); SafeFreeString(aRaw);

        bool isCustom = (ToLower(Trim(preset)) == L"custom" || Trim(preset).empty());

        if (isCustom) {
            bool allEmpty = Trim(rawName).empty() &&
                            Trim(rawIcon).empty() &&
                            Trim(rawAction).empty();
            if (Trim(preset).empty() && allEmpty) break;
            if (allEmpty) { continue; }
        }

        const PresetDef* pd = FindPreset(preset);

        ActionItem item;
        auto userSub = LoadSubmenu(i);

        if (pd) {
            item.name   = Trim(rawName).empty()
                            ? PresetName(*pd, ruLang)
                            : Trim(rawName);
            item.icon   = Trim(rawIcon).empty() ? pd->icon : Trim(rawIcon);

            if (!userSub.empty()) {
                item.submenu = std::move(userSub);
            } else if (pd->action) {
                item.action = pd->action;
            } else {
                auto& sub = item.submenu;
                struct SubDef { const wchar_t* key; };
                static const SubDef subs[] = {
                    { L"lock" }, { L"sleep" }, { L"shutdown" }, { L"restart" }
                };
                for (auto& sd : subs) {
                    const PresetDef* sp = FindPreset(sd.key);
                    if (!sp) continue;
                    ActionItem si;
                    si.name   = PresetName(*sp, ruLang);
                    si.icon   = sp->icon;
                    si.action = sp->action;
                    sub.push_back(std::move(si));
                }
            }
        } else {
            item.name   = Trim(rawName);
            item.icon   = Trim(rawIcon);
            item.action = Trim(rawAction);
            if (item.icon.empty()) item.icon = FALLBACK_ICON;
            if (!userSub.empty()) item.submenu = std::move(userSub);
        }

        newButtons.push_back(std::move(item));
    }

    std::lock_guard<std::mutex> lk(g_settingsMutex);
    g_buttons = std::move(newButtons);
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    return ToLower(path).find(L"explorer.exe") != std::wstring::npos;
}

static wux::FrameworkElement FindByName(wux::DependencyObject root,
                                       const wchar_t* name) {
    if (!root) return nullptr;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>())
            if (fe.Name() == name) return fe;
        auto found = FindByName(child, name);
        if (found) return found;
    }
    return nullptr;
}

static void AddMenuItems(wuxc::MenuFlyout flyout,
                         const std::vector<ActionItem>& items,
                         bool invertIconsSubmenus) {
    for (const auto& item : items) {
        wuxc::MenuFlyoutItem mi;
        mi.Text(item.name);
        std::wstring ico = item.icon.empty() ? std::wstring(FALLBACK_ICON) : item.icon;
        mi.Icon(MakeMenuIcon(ico));

        if (invertIconsSubmenus) {
            mi.FlowDirection(wux::FlowDirection::RightToLeft);
        }

        std::wstring act = item.action;
        mi.Click([act](auto&&, auto&&) {
            ExecuteActionText(act);
            CloseStartMenuAfterClick();
        });
        flyout.Items().Append(mi);
    }
}

static void InjectButtons(wuxc::Panel parentPanel,
                           wux::FrameworkElement) {
    try {
        Settings cur;
        std::vector<ActionItem> btns;
        {
            std::lock_guard<std::mutex> lk(g_settingsMutex);
            cur  = g_settings;
            btns = g_buttons;
        }

        std::vector<int> origIdx((int)btns.size());
        for (int i = 0; i < (int)origIdx.size(); ++i) origIdx[i] = i;
        if (cur.invertButtons) {
            std::reverse(btns.begin(), btns.end());
            std::reverse(origIdx.begin(), origIdx.end());
        }

        auto children = parentPanel.Children();
        for (uint32_t i = 0; i < children.Size(); ++i) {
            auto child = children.GetAt(i);
            auto fe = child.try_as<wux::FrameworkElement>();
            if (!fe) continue;

            auto tag = fe.Tag();
            if (tag && winrt::unbox_value_or<winrt::hstring>(tag, L"") == CONTAINER_TAG)
                continue;

            bool isUserTile =
                fe.Name() == L"UserTileButton" ||
                fe.Name() == L"UserTile"       ||
                fe.Name() == L"ProfileButton";
            bool isPowerButton = (fe.Name() == L"PowerButton");

            if (isPowerButton) {
                fe.Visibility(wux::Visibility::Collapsed);
                continue;
            }
            if (isUserTile) {
                switch (cur.accountButton) {
                    case AccountButtonMode::Hide:
                        fe.Visibility(wux::Visibility::Collapsed);
                        break;
                    case AccountButtonMode::Left:
                        fe.HorizontalAlignment(wux::HorizontalAlignment::Left);
                        fe.Visibility(wux::Visibility::Visible);
                        break;
                    case AccountButtonMode::Center:
                        fe.HorizontalAlignment(wux::HorizontalAlignment::Center);
                        fe.Visibility(wux::Visibility::Visible);
                        break;
                    case AccountButtonMode::Right:
                        fe.HorizontalAlignment(wux::HorizontalAlignment::Right);
                        fe.Visibility(wux::Visibility::Visible);
                        break;
                }
                continue;
            }
            fe.Visibility(wux::Visibility::Collapsed);
        }

        wuxc::StackPanel container = g_buttonContainer.get();
        if (container) {
            try {
                auto p = wuxm::VisualTreeHelper::GetParent(container);
                if (!p || p != parentPanel) {
                    container = nullptr; g_buttonContainer = nullptr;
                }
            } catch (...) { container = nullptr; g_buttonContainer = nullptr; }
        }
        if (!container) {
            for (uint32_t i = 0; i < children.Size(); ++i) {
                if (auto sp = children.GetAt(i).try_as<wuxc::StackPanel>()) {
                    auto tag = sp.Tag();
                    if (tag && winrt::unbox_value_or<winrt::hstring>(tag, L"") == CONTAINER_TAG) {
                        container = sp; g_buttonContainer = sp; break;
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
        container.HorizontalAlignment(cur.alignment);
        container.Margin({ (double)cur.containerMarginLeft, 0,
                           (double)cur.containerMarginRight, 0 });

        bool rebuild = g_forceRebuild.exchange(false) ||
                       container.Children().Size() != (uint32_t)btns.size();
        if (!rebuild) return;

        container.Children().Clear();

        for (size_t i = 0; i < btns.size(); ++i) {
            const auto& def = btns[i];
            int btnIdx = origIdx[i];

            wuxc::Button btn;
            btn.Width(40); btn.Height(40);
            double margin = (i + 1 < btns.size()) ? (double)cur.buttonSpacing : 0.0;
            btn.Margin({ 0, 0, margin, 0 });
            btn.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
            btn.BorderThickness({ 0, 0, 0, 0 });
            btn.CornerRadius({ 4, 4, 4, 4 });
            btn.Padding({ 0, 0, 0, 0 });

            std::wstring ico = def.icon.empty() ? std::wstring(FALLBACK_ICON) : def.icon;
            btn.Content(MakeButtonIcon(ico));
            wuxc::ToolTipService::SetToolTip(btn, winrt::box_value(def.name));

            if (!def.submenu.empty()) {
                auto sub = def.submenu;
                bool invertIcons = cur.invertIconsSubmenus;
                btn.Click([sub, btn, invertIcons](auto&&, auto&&) mutable {
                    wuxc::MenuFlyout flyout;
                    AddMenuItems(flyout, sub, invertIcons);
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

    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"InjectButtons: WinRT 0x%08X %s", (unsigned)ex.code(),
               ex.message().c_str());
    } catch (...) {
        Wh_Log(L"InjectButtons: unknown exception");
    }
}

static bool TryInjectButtons() {
    if (g_unloading) return false;
    try {
        auto window = wux::Window::Current();
        if (!window) return false;
        auto content = window.Content();
        if (!content) return false;

        auto powerBtn = FindByName(content, L"PowerButton");
        if (!powerBtn) return false;

        auto parentObj = wuxm::VisualTreeHelper::GetParent(powerBtn);
        if (!parentObj) return false;
        auto parentPanel = parentObj.try_as<wuxc::Panel>();
        if (!parentPanel) return false;

        g_parentPanel         = parentPanel;
        g_originalPowerButton = powerBtn;
        InjectButtons(parentPanel, powerBtn);
        g_buttonsInjected = true;
        return true;
    } catch (...) {
        Wh_Log(L"TryInjectButtons: exception");
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
        wuc::CoreWindow cw = wuc::CoreWindow::GetForCurrentThread();
        if (!cw) return false;

        g_visibilityToken = cw.VisibilityChanged([](auto&&, auto&& a) {
            if (a.Visible()) OnWindowVisible();
        });
        g_activatedToken = cw.Activated([](auto&&, auto&& a) {
            if (a.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated)
                OnWindowVisible();
        });

        g_monitoringActive = true;
        if (cw.Visible()) OnWindowVisible();
        return true;
    } catch (...) { return false; }
}

static void StopVisibilityMonitoring() {
    try {
        wuc::CoreWindow cw = wuc::CoreWindow::GetForCurrentThread();
        if (cw) {
            if (g_visibilityToken)
                cw.VisibilityChanged(std::exchange(g_visibilityToken, {}));
            if (g_activatedToken)
                cw.Activated(std::exchange(g_activatedToken, {}));
        }
    } catch (...) {}

    try {
        auto panel      = g_parentPanel.get();
        auto container  = g_buttonContainer.get();
        auto origBtn    = g_originalPowerButton.get();

        if (panel && container) {
            uint32_t idx;
            if (panel.Children().IndexOf(container, idx))
                panel.Children().RemoveAt(idx);
            auto ch = panel.Children();
            for (uint32_t i = 0; i < ch.Size(); ++i) {
                if (auto fe = ch.GetAt(i).try_as<wux::FrameworkElement>())
                    if (fe.Visibility() == wux::Visibility::Collapsed)
                        fe.Visibility(wux::Visibility::Visible);
            }
        }
        if (origBtn) origBtn.Visibility(wux::Visibility::Visible);
    } catch (...) {}

    g_parentPanel         = nullptr;
    g_originalPowerButton = nullptr;
    g_buttonContainer     = nullptr;
    g_monitoringActive    = false;
}

static HWND g_retryHwnd = NULL;
static constexpr UINT_PTR kRetryTimer = 0x574831AA;

static void CALLBACK RetryTimerProc(HWND hwnd, UINT, UINT_PTR id, DWORD) {
    KillTimer(hwnd, id);
    if (g_unloading) return;
    g_buttonsInjected = false;
    HWND h = g_retryHwnd ? g_retryHwnd : hwnd;
    if (!StartVisibilityMonitoring())
        SetTimer(h, kRetryTimer, 100, RetryTimerProc);
}

static void InitWithRetry(HWND hwnd) {
    if (!g_initialized.exchange(true)) {
        LoadSettings();
        BuildButtons();
    }
    if (hwnd) g_retryHwnd = hwnd;
    if (!StartVisibilityMonitoring())
        SetTimer(hwnd, kRetryTimer, 100, RetryTimerProc);
}

using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, PVOID, DWORD);
static CreateWindowInBand_t CreateWindowInBand_Original;

static HWND WINAPI CreateWindowInBand_Hook(
    DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style,
    int x, int y, int w, int h,
    HWND parent, HMENU menu, HINSTANCE inst, PVOID param, DWORD band)
{
    HWND hWnd = CreateWindowInBand_Original(
        exStyle, cls, name, style, x, y, w, h,
        parent, menu, inst, param, band);
    if (hWnd && cls && _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0)
        InitWithRetry(hWnd);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, PVOID, DWORD, DWORD);
static CreateWindowInBandEx_t CreateWindowInBandEx_Original;

static HWND WINAPI CreateWindowInBandEx_Hook(
    DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style,
    int x, int y, int w, int h,
    HWND parent, HMENU menu, HINSTANCE inst, PVOID param,
    DWORD band, DWORD typeFlags)
{
    HWND hWnd = CreateWindowInBandEx_Original(
        exStyle, cls, name, style, x, y, w, h,
        parent, menu, inst, param, band, typeFlags);
    if (hWnd && cls && _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0)
        InitWithRetry(hWnd);
    return hWnd;
}

using ThreadCallProc_t = void(WINAPI*)(PVOID);

static bool CallOnWindowThread(HWND hWnd, ThreadCallProc_t proc, PVOID param) {
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(param); return true; }

    struct CP { ThreadCallProc_t proc; PVOID param; };
    static const UINT msg = RegisterWindowMessageW(THREAD_CALL_MSG);

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lp;
                if (cwp->message == RegisterWindowMessageW(THREAD_CALL_MSG)) {
                    auto* p = (CP*)cwp->lParam;
                    p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        }, nullptr, tid);
    if (!hook) return false;

    CP p{ proc, param };
    SendMessageW(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindCoreWindow() {
    HWND result = nullptr;
    EnumWindows([](HWND w, LPARAM lp) -> BOOL {
        DWORD pid = 0;
        if (!GetWindowThreadProcessId(w, &pid) ||
            pid != GetCurrentProcessId()) return TRUE;
        WCHAR cls[128]{};
        if (!GetClassNameW(w, cls, ARRAYSIZE(cls))) return TRUE;
        if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            *(HWND*)lp = w; return FALSE;
        }
        return TRUE;
    }, (LPARAM)&result);
    return result;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit");
    LoadSettings();
    BuildButtons();

    if (IsExplorerProcess()) {
        StartProxyThread();
        return TRUE;
    }

    HMODULE hU32 = GetModuleHandleW(L"user32.dll");
    auto p = GetProcAddress(hU32, "CreateWindowInBand");
    if (p)
        Wh_SetFunctionHook((void*)p, (void*)CreateWindowInBand_Hook,
                           (void**)&CreateWindowInBand_Original);

    p = GetProcAddress(hU32, "CreateWindowInBandEx");
    if (p)
        Wh_SetFunctionHook((void*)p, (void*)CreateWindowInBandEx_Hook,
                           (void**)&CreateWindowInBandEx_Original);

    HWND cw = FindCoreWindow();
    if (cw) CallOnWindowThread(cw, [](PVOID hw) {
        InitWithRetry((HWND)hw);
    }, (PVOID)cw);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Wh_ModUninit");
    g_unloading = true;

    if (IsExplorerProcess()) {
        if (g_proxyWindow)
            PostMessageW(g_proxyWindow, WM_CLOSE, 0, 0);
        if (g_proxyThread) {
            WaitForSingleObject(g_proxyThread, 2000);
            CloseHandle(g_proxyThread);
            g_proxyThread = nullptr;
        }
        return;
    }

    HWND cw = FindCoreWindow();
    if (cw) CallOnWindowThread(cw, [](PVOID) {
        StopVisibilityMonitoring();
    }, nullptr);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Wh_ModSettingsChanged");
    LoadSettings();
    BuildButtons();
    g_forceRebuild    = true;
    g_buttonsInjected = false;

    HWND cw = FindCoreWindow();
    if (!cw) return;
    CallOnWindowThread(cw, [](PVOID) {
        try {
            if (auto w = wuc::CoreWindow::GetForCurrentThread())
                if (w.Visible()) OnWindowVisible();
        } catch (...) {}
    }, nullptr);
}
