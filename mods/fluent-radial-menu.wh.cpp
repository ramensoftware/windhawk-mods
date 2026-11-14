// ==WindhawkMod==
// @id              fluent-radial-menu
// @name            Fluent Radial Menu
// @description     Launch apps, files, folders, links, and shortcuts from a customizable radial menu on Windows 11.
// @version         0.8.4
// @author          Fenryr
// @github          https://github.com/soldier93HUN
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -ldwmapi -lgdi32 -lole32 -loleaut32 -lshell32 -ladvapi32 -lwindowscodecs -luuid -lruntimeobject -lmsimg32 -lcomctl32 -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fluent Radial Menu

Fluent Radial Menu adds a circular launcher to Windows 11. It comes with
three pages of apps, folders, and Windows shortcuts and a built-in editor.

![Fluent Radial Menu in action](https://i.imgur.com/fj0r5vV.gif)

## Getting started

1. Enable the mod in Windhawk and press **Ctrl+Alt+Space** to open the wheel
   at the pointer. To change the shortcut, set **Activation shortcut** in the
   mod's Windhawk settings and click **Save settings**.
2. Click a wedge to run its action. Scroll to switch pages or press **Esc**
   to dismiss the wheel. You can also use the arrow keys, **Enter**, and
   **Page Up** or **Page Down**.
3. Press **F2** while the wheel is open, or right-click it and choose
   **Edit radial menu**, to open the built-in editor.

## Make it yours

- **Actions** lets you choose an installed app, folder, file, website,
  Windows settings page, keyboard shortcut, or command. Select **Custom…**
  to browse for a local target. Choose the original app icon, a built-in
  symbol, or an icon file. Drag a wedge in the preview to move or swap it.
- **Pages** lets you add, rename, reorder, or duplicate up to eight pages
  with eight slots each.
- **Appearance** controls the theme, accent, size, transparency, blur, and
  labels. **Motion & activation** controls the profile shortcut, positioning,
  and animation. It can follow Windows' reduced motion setting.
- **Apply** saves your changes. **Test on desktop** opens the applied wheel
  with actions disabled.

## Share or back up a layout

**Export profile** saves the current editor layout and settings as JSON.
**Import profile** opens a profile in the editor. Review its actions and
select **Apply** when you are ready to use it. Shared profiles reference apps,
folders, and custom icon files by path, so those may need to be updated on
another PC. The built-in icons travel with the profile.

A shortcut set in Windhawk takes priority over the profile shortcut. Clear
the Windhawk field to use the profile shortcut again. Check the Windhawk mod
log if Windows cannot register a new shortcut.

On 64-bit Windows, the menu opens native system apps from either Windhawk
host. In the 32-bit file picker, enter **%WINDIR%\Sysnative** in the address
bar to browse the native System32 folder. The editor saves those selections
as System32 paths so the same profile works in the 64-bit host.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ActivationShortcut: ""
  $name: Activation shortcut
  $description: >-
    Enter a shortcut such as Ctrl+Alt+Space or Ctrl+Shift+M.
    Leave blank to use the profile shortcut (Ctrl+Alt+Space by default).
*/
// ==/WindhawkModSettings==



#define NOMINMAX
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <shellapi.h>
#include <shldisp.h>
#include <shlobj.h>
#include <shlguid.h>
#include <commctrl.h>
#include <commdlg.h>
#include <windowsx.h>
#include <aclapi.h>
#include <sddl.h>
#include <servprov.h>
#include <roapi.h>
#include <windows.data.json.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <cwctype>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <numbers>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
using Microsoft::WRL::ComPtr;
using Clock = std::chrono::steady_clock;
using Time = Clock::time_point;
using namespace std::chrono_literals;

constexpr int kSlots = 8;
constexpr int kMaxPages = 8;
constexpr float kTurn = 2 * std::numbers::pi_v<float>;
constexpr float kStep = kTurn / kSlots;
constexpr float kShadow = 20;
constexpr wchar_t kWindowClass[] = L"Windhawk.FluentRadialMenu";
constexpr wchar_t kPointerClass[] = L"Windhawk.FluentRadialMenu.Pointer";
constexpr int kHotkey = 1;
constexpr UINT kFrameTimer = 1;
constexpr UINT kPopupStopTimer = 2;
constexpr UINT kIconsReady = WM_APP + 2;
constexpr UINT kPointer = WM_APP + 3;
constexpr UINT kDismiss = WM_APP + 4;
constexpr UINT kWheel = WM_APP + 5;
constexpr UINT kSelect = WM_APP + 6;
constexpr UINT kNavigate = WM_APP + 7;
constexpr UINT kHoldRelease = WM_APP + 8;
constexpr UINT kRepaint = WM_APP + 9;
constexpr UINT kEditMenu = WM_APP + 10;
constexpr UINT kOpenEditor = WM_APP + 11;
constexpr UINT kAppsReady = WM_APP + 12;
constexpr UINT kEditorChoice = WM_APP + 13;

std::wstring Trim(std::wstring_view value) {
    const auto first = value.find_first_not_of(L" \t\r\n");
    if (first == value.npos) return {};
    return std::wstring(value.substr(first, value.find_last_not_of(L" \t\r\n") - first + 1));
}

bool UsesWow64FileRedirection() {
    static const bool redirected = [] {
        USHORT machine = IMAGE_FILE_MACHINE_UNKNOWN;
        return IsWow64Process2(GetCurrentProcess(), &machine, nullptr) &&
               (machine == IMAGE_FILE_MACHINE_I386 || machine == IMAGE_FILE_MACHINE_ARMNT);
    }();
    return redirected;
}

std::wstring Expand(std::wstring value) {
    if (UsesWow64FileRedirection() && GetEnvironmentVariableW(L"ProgramW6432", nullptr, 0)) {
        for (size_t start = value.find(L'%'); start != value.npos;) {
            const size_t end = value.find(L'%', start + 1);
            if (end == value.npos) break;
            constexpr std::wstring_view programFiles = L"%ProgramFiles%";
            constexpr std::wstring_view nativeProgramFiles = L"%ProgramW6432%";
            if (end - start + 1 == programFiles.size() &&
                _wcsnicmp(value.c_str() + start, programFiles.data(), programFiles.size()) == 0) {
                value.replace(start, programFiles.size(), nativeProgramFiles);
                start = value.find(L'%', start + nativeProgramFiles.size());
            } else {
                start = value.find(L'%', end + 1);
            }
        }
    }
    const DWORD size = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (!size || size > 32768) return value;
    std::wstring result(size, L'\0');
    if (ExpandEnvironmentStringsW(value.c_str(), result.data(), size) != size) return value;
    result.pop_back();
    return result;
}

const std::wstring& WindowsDirectory() {
    static const std::wstring directory = [] {
        wchar_t path[MAX_PATH];
        const UINT length = GetSystemWindowsDirectoryW(path, ARRAYSIZE(path));
        return length && length < ARRAYSIZE(path) ? std::wstring(path, length) : std::wstring{};
    }();
    return directory;
}

bool ReplaceSystemDirectory(std::wstring& path, std::wstring_view from, std::wstring_view to) {
    const auto& windows = WindowsDirectory();
    if (windows.empty()) return false;
    const std::wstring prefix = windows + L"\\" + std::wstring(from);
    const size_t offset = path.starts_with(L"\\\\?\\") ? 4 : 0;
    if (path.size() < offset + prefix.size()) return false;
    std::wstring leading = path.substr(offset, prefix.size());
    std::replace(leading.begin(), leading.end(), L'/', L'\\');
    if (_wcsicmp(leading.c_str(), prefix.c_str()) != 0) return false;
    const size_t boundary = offset + prefix.size();
    if (boundary < path.size() && path[boundary] != L'\\' && path[boundary] != L'/') return false;
    path.replace(boundary - from.size(), from.size(), to);
    if (to == L"Sysnative") std::replace(path.begin() + offset, path.end(), L'/', L'\\');
    return true;
}

std::wstring NativePath(std::wstring path) {
    if (UsesWow64FileRedirection()) ReplaceSystemDirectory(path, L"System32", L"Sysnative");
    return path;
}

bool IsExecutablePath(const std::wstring& path) {
    return path.size() >= 4 && _wcsicmp(path.c_str() + path.size() - 4, L".exe") == 0;
}

// A resolved bare name may already point into the redirected system directory.
// Keep the resolved file when Windows ships it only in that directory.
std::wstring ResolveBareExecutable(const std::wstring& name) {
    auto search = [&name](PCWSTR directory) -> std::wstring {
        wchar_t path[MAX_PATH];
        const DWORD length = SearchPathW(directory, name.c_str(), L".exe", ARRAYSIZE(path), path, nullptr);
        return length && length < ARRAYSIZE(path) ? std::wstring(path, length) : std::wstring{};
    };
    auto resolved = search(nullptr);
    if (!UsesWow64FileRedirection() || WindowsDirectory().empty()) return resolved;
    if (resolved.empty()) {
        const auto nativeDirectory = WindowsDirectory() + L"\\Sysnative";
        return search(nativeDirectory.c_str());
    }

    auto native = resolved;
    wchar_t wowPath[MAX_PATH];
    const UINT length = GetSystemWow64DirectoryW(wowPath, ARRAYSIZE(wowPath));
    const auto wowDirectory = length && length < ARRAYSIZE(wowPath)
        ? std::wstring_view(wowPath, length) : std::wstring_view{};
    const auto separator = wowDirectory.find_last_of(L'\\');
    if (ReplaceSystemDirectory(native, L"System32", L"Sysnative") ||
        (separator != wowDirectory.npos &&
         ReplaceSystemDirectory(native, wowDirectory.substr(separator + 1), L"Sysnative"))) {
        const DWORD attributes = GetFileAttributesW(native.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY))
            return native;
    }
    return resolved;
}

std::optional<unsigned long> ParseNumber(std::wstring_view value, int base) {
    if (value.empty() || value.size() > 8) return {};
    unsigned long number = 0;
    for (wchar_t character : value) {
        int digit = character >= L'0' && character <= L'9' ? character - L'0'
                    : character >= L'A' && character <= L'F' ? character - L'A' + 10
                    : character >= L'a' && character <= L'f' ? character - L'a' + 10 : -1;
        if (digit < 0 || digit >= base) return {};
        number = number * base + digit;
    }
    return number;
}

struct Chord {
    UINT modifiers = 0;
    UINT key = 0;
    bool operator==(const Chord&) const = default;
};

struct Modifier {
    UINT flag;
    WORD key;
    WORD alternate;
};
constexpr Modifier kModifiers[] = {
    {MOD_CONTROL, VK_CONTROL, 0}, {MOD_SHIFT, VK_SHIFT, 0},
    {MOD_ALT, VK_MENU, 0}, {MOD_WIN, VK_LWIN, VK_RWIN},
};

bool KeyDown(UINT key) {
    return key && (GetAsyncKeyState(key) & 0x8000);
}

bool ModifierDown(const Modifier& modifier) {
    return KeyDown(modifier.key) || KeyDown(modifier.alternate);
}

std::optional<Chord> ParseChord(std::wstring_view text) {
    Chord chord;
    while (!text.empty()) {
        size_t separator = text.find(L'+');
        std::wstring token = Trim(text.substr(0, separator));
        for (wchar_t& character : token) character = towupper(character);
        if (separator == text.npos) {
            if (token.size() == 1 && ((token[0] >= L'A' && token[0] <= L'Z') ||
                                      (token[0] >= L'0' && token[0] <= L'9'))) {
                chord.key = token[0];
            } else if (token.size() > 1 && token[0] == L'F') {
                const auto index = ParseNumber(std::wstring_view(token).substr(1), 10);
                if (index && *index >= 1 && *index <= 24) chord.key = VK_F1 + *index - 1;
            } else if (const auto key = ParseNumber(token, 10); key && *key > 0 && *key < 255) {
                chord.key = *key;
            } else {
                constexpr std::pair<std::wstring_view, UINT> names[] = {
                    {L"SPACE", VK_SPACE}, {L"TAB", VK_TAB}, {L"ENTER", VK_RETURN},
                    {L"ESC", VK_ESCAPE}, {L"ESCAPE", VK_ESCAPE}, {L"PERIOD", VK_OEM_PERIOD},
                    {L"LEFT", VK_LEFT}, {L"RIGHT", VK_RIGHT}, {L"UP", VK_UP},
                    {L"DOWN", VK_DOWN}, {L"HOME", VK_HOME}, {L"END", VK_END},
                    {L"PAGEUP", VK_PRIOR}, {L"PAGEDOWN", VK_NEXT}, {L"DELETE", VK_DELETE},
                    {L"BACKSPACE", VK_BACK}, {L"INSERT", VK_INSERT},
                    {L"PRINTSCREEN", VK_SNAPSHOT},
                };
                for (auto [name, key] : names) {
                    if (token == name) {
                        chord.key = key;
                        break;
                    }
                }
            }
            if (chord.key <= VK_XBUTTON2 || chord.key == VK_SHIFT ||
                chord.key == VK_CONTROL || chord.key == VK_MENU ||
                chord.key == VK_LWIN || chord.key == VK_RWIN ||
                (chord.key >= VK_LSHIFT && chord.key <= VK_RMENU)) return {};
            return chord;
        }
        if (token == L"CTRL" || token == L"CONTROL") chord.modifiers |= MOD_CONTROL;
        else if (token == L"ALT") chord.modifiers |= MOD_ALT;
        else if (token == L"SHIFT") chord.modifiers |= MOD_SHIFT;
        else if (token == L"WIN") chord.modifiers |= MOD_WIN;
        else return {};
        text.remove_prefix(separator + 1);
    }
    return {};
}

enum class Action { Empty, Open, Command, Shortcut };
enum class ActionCategory { Empty, App, Folder, Link, File, Settings, Keys, Command, Unspecified };
enum class Theme { System, Light, Dark };
enum class PageTransition { Instant, Crossfade, Rotate };

struct Slot {
    std::wstring name, target, arguments, icon;
    Action action = Action::Empty;
    ActionCategory category = ActionCategory::Unspecified;
    wchar_t glyph = 0;
    bool operator==(const Slot&) const = default;
    bool Active() const { return action != Action::Empty && !target.empty(); }
    const std::wstring& Label() const { return name.empty() ? target : name; }
};

struct ActionCategoryInfo {
    PCWSTR key, title, picker, targetLabel;
    Action action;
};

constexpr ActionCategoryInfo kActionCategories[] = {
    {L"empty", L"Empty", L"", L"", Action::Empty},
    {L"app", L"Open app", L"Application", L"App path or shortcut", Action::Open},
    {L"folder", L"Open folder", L"Folder", L"Folder path", Action::Open},
    {L"link", L"Open link", L"Link", L"Web address or URI", Action::Open},
    {L"file", L"Open file", L"File", L"File path", Action::Open},
    {L"settings", L"Windows settings", L"Settings page", L"Settings URI", Action::Open},
    {L"keys", L"Send keys", L"Keyboard shortcut", L"Key chord", Action::Shortcut},
    {L"command", L"Run command", L"Command", L"Executable", Action::Command},
};

const ActionCategoryInfo& CategoryInfo(ActionCategory category) {
    return kActionCategories[static_cast<size_t>(category)];
}

ActionCategory InferCategory(const Slot& slot) {
    if (slot.action == Action::Empty) return ActionCategory::Empty;
    if (slot.action == Action::Command) return ActionCategory::Command;
    if (slot.action == Action::Shortcut) return ActionCategory::Keys;
    const wchar_t* target = slot.target.c_str();
    if (_wcsnicmp(target, L"ms-settings:", 12) == 0) return ActionCategory::Settings;
    constexpr wchar_t appsFolder[] = L"shell:AppsFolder\\";
    if (_wcsnicmp(target, appsFolder, ARRAYSIZE(appsFolder) - 1) == 0) return ActionCategory::App;
    if (_wcsnicmp(target, L"shell:", 6) == 0 || slot.target == L"%USERPROFILE%")
        return ActionCategory::Folder;
    if (const auto colon = slot.target.find(L':'); colon != slot.target.npos && colon != 1)
        return ActionCategory::Link;
    const auto extension = slot.target.find_last_of(L'.');
    if (extension != slot.target.npos) {
        const wchar_t* suffix = target + extension;
        if (_wcsicmp(suffix, L".exe") == 0 || _wcsicmp(suffix, L".lnk") == 0 ||
            _wcsicmp(suffix, L".msc") == 0) return ActionCategory::App;
        if (_wcsicmp(suffix, L".url") == 0) return ActionCategory::Link;
    }
    return ActionCategory::File;
}

struct Settings {
    Theme theme = Theme::System;
    PageTransition transition = PageTransition::Crossfade;
    std::optional<DWORD> accent;
    std::optional<Chord> shortcut;
    int transparency, blur, diameter, centerDiameter, iconSize, border;
    int speed, iconScale, selectionMs, wheelSensitivity;
    bool dividers, outerLabels, centerLabel, pageLabel, animations, respectMotion;
    bool centerOnMonitor, closeAfterSelection, holdToSelect;
    std::wstring idleLabel, shortcutText, name, revision;
    std::vector<std::wstring> pageNames;
    std::vector<std::array<Slot, kSlots>> pages;
    bool operator==(const Settings&) const = default;
};

void ResolveGlyph(Slot& slot) {
    if (!slot.icon.starts_with(L"glyph:")) return;
    const auto value = ParseNumber(std::wstring_view(slot.icon).substr(6), 16);
    if (value && *value >= 0xE000 && *value <= 0xF8FF) slot.glyph = *value;
}

std::wstring ReadString(PCWSTR path) {
    std::unique_ptr<const wchar_t, decltype(&Wh_FreeStringSetting)> value(
        Wh_GetStringSetting(L"%s", path), Wh_FreeStringSetting);
    return value ? value.get() : L"";
}

Settings DefaultSettings() {
    Settings settings{};
    settings.transparency = settings.blur = 18;
    settings.diameter = 392;
    settings.centerDiameter = 180;
    settings.iconSize = 28;
    settings.border = 1;
    settings.dividers = settings.centerLabel = settings.pageLabel = true;
    settings.animations = settings.respectMotion = true;
    settings.speed = 100;
    settings.iconScale = settings.wheelSensitivity = 120;
    settings.selectionMs = 210;
    settings.shortcutText = L"Ctrl+Alt+Space";
    settings.shortcut = ParseChord(settings.shortcutText);
    settings.closeAfterSelection = true;
    settings.name = L"Default";
    settings.revision = L"00000000000000000000000000000000";
    const Slot slots[] = {
        {L"File Explorer", L"%WINDIR%\\explorer.exe", L"", L"glyph:E8B7", Action::Open},
        {L"Windows Settings", L"ms-settings:", L"", L"glyph:E713", Action::Open},
        {L"Task Manager", L"%WINDIR%\\System32\\Taskmgr.exe", L"", L"glyph:E9D9", Action::Open},
        {L"Calculator", L"%WINDIR%\\System32\\calc.exe", L"", L"glyph:E8EF", Action::Open},
        {L"Notepad", L"%WINDIR%\\System32\\notepad.exe", L"", L"glyph:E70B", Action::Open},
        {L"Snipping Tool", L"Win+Shift+S", L"", L"glyph:E8C8", Action::Shortcut},
        {L"Terminal", L"wt.exe", L"", L"glyph:E756", Action::Open},
        {L"Task View", L"Win+Tab", L"", L"glyph:E7C4", Action::Shortcut},
        {L"Home Folder", L"%USERPROFILE%", L"", L"glyph:E80F", Action::Open},
        {L"Documents", L"shell:Personal", L"", L"glyph:E8A5", Action::Open},
        {L"Downloads", L"shell:Downloads", L"", L"glyph:E896", Action::Open},
        {L"Pictures", L"shell:My Pictures", L"", L"glyph:EB9F", Action::Open},
        {L"Music", L"shell:My Music", L"", L"glyph:E8D6", Action::Open},
        {L"Videos", L"shell:My Video", L"", L"glyph:E714", Action::Open},
        {L"Desktop Folder", L"shell:Desktop", L"", L"glyph:E7F4", Action::Open},
        {L"Recycle Bin", L"shell:RecycleBinFolder", L"", L"glyph:E74D", Action::Open},
        {L"Web Browser", L"https://www.bing.com/", L"", L"glyph:E774", Action::Open},
        {L"Search", L"Win+S", L"", L"glyph:E721", Action::Shortcut},
        {L"Clipboard History", L"Win+V", L"", L"glyph:E77F", Action::Shortcut},
        {L"Emoji Panel", L"Win+Period", L"", L"glyph:E899", Action::Shortcut},
        {L"Quick Settings", L"Win+A", L"", L"glyph:E9E9", Action::Shortcut},
        {L"Display Settings", L"ms-settings:display", L"", L"glyph:E7F4", Action::Open},
        {L"Sound Settings", L"ms-settings:sound", L"", L"glyph:E767", Action::Open},
        {L"Show Desktop", L"Win+D", L"", L"glyph:E8A7", Action::Shortcut},
    };
    static_assert(ARRAYSIZE(slots) % kSlots == 0);
    for (size_t i = 0; i < ARRAYSIZE(slots); i += kSlots) {
        auto& page = settings.pages.emplace_back();
        std::copy_n(slots + i, kSlots, page.begin());
        for (auto& slot : page) ResolveGlyph(slot);
        settings.pageNames.push_back(L"Page " + std::to_wstring(settings.pages.size()));
    }
    return settings;
}

constexpr wchar_t kProfileFile[] = L"\\editor.json";
constexpr DWORD kMaxProfileBytes = 2 * 1024 * 1024;

using LocalSecurityDescriptor = std::unique_ptr<void, decltype(&LocalFree)>;
using FileHandle = std::unique_ptr<void, decltype(&CloseHandle)>;

std::wstring CurrentUserSid() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return {};
    DWORD length = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &length);
    std::vector<BYTE> user(length);
    const bool found = length && GetTokenInformation(token, TokenUser, user.data(), length, &length);
    CloseHandle(token);
    PWSTR sid = nullptr;
    if (!found || !ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(user.data())->User.Sid,
                                          &sid)) return {};
    std::unique_ptr<wchar_t, decltype(&LocalFree)> owner(sid, LocalFree);
    return sid;
}

bool HasPrivateProfilePermissions(HANDLE directory, PSECURITY_DESCRIPTOR expected) {
    PSECURITY_DESCRIPTOR actual = nullptr;
    PSID actualOwner = nullptr, expectedOwner = nullptr;
    PACL actualDacl = nullptr, expectedDacl = nullptr;
    PACL actualSacl = nullptr, expectedSacl = nullptr;
    const DWORD result = GetSecurityInfo(directory, SE_FILE_OBJECT,
        OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION,
        &actualOwner, nullptr, &actualDacl, &actualSacl, &actual);
    const LocalSecurityDescriptor actualDescriptor(actual, LocalFree);
    BOOL present = FALSE, defaulted = FALSE;
    SECURITY_DESCRIPTOR_CONTROL control;
    DWORD revision;
    if (result != ERROR_SUCCESS ||
        !GetSecurityDescriptorOwner(expected, &expectedOwner, &defaulted) ||
        !EqualSid(actualOwner, expectedOwner) ||
        !GetSecurityDescriptorControl(actual, &control, &revision) ||
        !(control & SE_DACL_PROTECTED) ||
        !GetSecurityDescriptorDacl(expected, &present, &expectedDacl, &defaulted) ||
        !GetSecurityDescriptorSacl(expected, &present, &expectedSacl, &defaulted)) return false;

    // Compare ACEs by their contents. Windows may reorder them or allocate
    // extra space in the returned ACL.
    const auto matches = [](PACL actualAcl, PACL expectedAcl) {
        if (!actualAcl || !expectedAcl || actualAcl->AceCount != expectedAcl->AceCount) return false;
        for (DWORD i = 0; i < expectedAcl->AceCount; ++i) {
            void* expectedAce = nullptr;
            if (!GetAce(expectedAcl, i, &expectedAce)) return false;
            const auto size = static_cast<ACE_HEADER*>(expectedAce)->AceSize;
            bool found = false;
            for (DWORD j = 0; j < actualAcl->AceCount; ++j) {
                void* actualAce = nullptr;
                if (GetAce(actualAcl, j, &actualAce) &&
                    static_cast<ACE_HEADER*>(actualAce)->AceSize == size &&
                    std::memcmp(actualAce, expectedAce, size) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return true;
    };
    return matches(actualDacl, expectedDacl) && matches(actualSacl, expectedSacl);
}

std::wstring ProfileRoot(FileHandle& directory) {
    wchar_t storage[MAX_PATH];
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage))) return {};
    const auto sid = CurrentUserSid();
    if (sid.empty()) return {};
    const auto path = std::wstring(storage) + L"\\" + sid;
    const auto sddl = L"O:" + sid + L"D:P(A;OICI;FA;;;" + sid +
                      L")(A;OICI;FA;;;SY)(A;OICI;FA;;;BA)S:(ML;OICI;NW;;;ME)";
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(), SDDL_REVISION_1,
                                                              &descriptor, nullptr)) return {};
    const LocalSecurityDescriptor owner(descriptor, LocalFree);
    SECURITY_ATTRIBUTES security{sizeof(security), descriptor, FALSE};
    if (!CreateDirectoryW(path.c_str(), &security) && GetLastError() != ERROR_ALREADY_EXISTS)
        return {};

    // Keep the directory open without delete sharing while its layout is in use.
    HANDLE handle = CreateFileW(path.c_str(), READ_CONTROL | FILE_READ_ATTRIBUTES,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (handle == INVALID_HANDLE_VALUE) return {};
    FileHandle candidate(handle, CloseHandle);
    BY_HANDLE_FILE_INFORMATION info{};
    if (!GetFileInformationByHandle(handle, &info) ||
        !(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
        !HasPrivateProfilePermissions(handle, descriptor)) {
        Wh_Log(L"Ignoring a radial menu profile directory without the expected private permissions.");
        return {};
    }
    directory = std::move(candidate);
    return path;
}

bool IsHex(std::wstring_view text) {
    return text.find_first_not_of(L"0123456789abcdefABCDEF") == text.npos;
}

std::wstring FromUtf8(const std::string& text) {
    const int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
                                         static_cast<int>(text.size()), nullptr, 0);
    if (!size) return {};
    std::wstring result(size, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
                        static_cast<int>(text.size()), result.data(), size);
    return result;
}

std::string ToUtf8(std::wstring_view text) {
    const int size = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                                         nullptr, 0, nullptr, nullptr);
    std::string result(size, '\0');
    if (size) WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                                  result.data(), size, nullptr, nullptr);
    return result;
}

void AppendQuoted(std::wstring& json, std::wstring_view value) {
    constexpr wchar_t digits[] = L"0123456789abcdef";
    json += L'"';
    for (wchar_t c : value) {
        if (c == L'"' || c == L'\\') json += L'\\';
        if (c < 0x20) {
            json += L"\\u00";
            json += digits[c >> 4];
            json += digits[c & 15];
        } else {
            json += c;
        }
    }
    json += L'"';
}

using Microsoft::WRL::Wrappers::HString;
using Microsoft::WRL::Wrappers::HStringReference;
namespace Json = ABI::Windows::Data::Json;
using JsonVector = ABI::Windows::Foundation::Collections::IVector<Json::IJsonValue*>;

struct JsonWriter {
    std::wstring& json;
    bool first = true;

    void Key(PCWSTR key) {
        if (!std::exchange(first, false)) json += L',';
        AppendQuoted(json, key);
        json += L':';
    }
    void String(PCWSTR key, const std::wstring& value, size_t = 0) {
        Key(key);
        AppendQuoted(json, value);
    }
    void Number(PCWSTR key, int value, int = 0, int = 0) {
        Key(key);
        json += std::to_wstring(value);
    }
    void Flag(PCWSTR key, bool value) {
        Key(key);
        json += value ? L"true" : L"false";
    }
};

struct JsonReader {
    Json::IJsonObject* object;
    std::wstring& error;

    void Invalid(PCWSTR field) {
        if (error.empty()) error = L"Invalid profile field: " + std::wstring(field);
    }
    void String(PCWSTR key, std::wstring& value, size_t maxLength) {
        HString string;
        if (FAILED(object->GetNamedString(HStringReference(key).Get(), string.GetAddressOf()))) {
            Invalid(key);
            return;
        }
        UINT32 length = 0;
        const wchar_t* text = string.GetRawBuffer(&length);
        if (length > maxLength || std::wstring_view(text, length).find(L'\0') != value.npos) {
            Invalid(key);
            return;
        }
        value.assign(text, length);
    }
    void Number(PCWSTR key, int& value, int low, int high) {
        double number;
        if (FAILED(object->GetNamedNumber(HStringReference(key).Get(), &number)) ||
            !std::isfinite(number) || number < low || number > high || std::floor(number) != number) {
            Invalid(key);
            return;
        }
        value = static_cast<int>(number);
    }
    void Flag(PCWSTR key, bool& value) {
        boolean enabled;
        if (FAILED(object->GetNamedBoolean(HStringReference(key).Get(), &enabled))) {
            Invalid(key);
            return;
        }
        value = enabled;
    }
    bool Array(PCWSTR key, ComPtr<Json::IJsonArray>& array, UINT32& count,
               UINT32 minCount, UINT32 maxCount) {
        ComPtr<JsonVector> vector;
        if (FAILED(object->GetNamedArray(HStringReference(key).Get(), &array)) ||
            FAILED(array.As(&vector)) || FAILED(vector->get_Size(&count)) ||
            count < minCount || count > maxCount) {
            Invalid(key);
            return false;
        }
        return true;
    }
};

struct ProfileOptions {
    int version = 1;
    std::wstring theme, accent, transition, position;
};

template<class IO, class S, class O>
void ProfileFields(IO& io, S& settings, O& options) {
    io.Number(L"version", options.version, 1, 1);
    io.String(L"revision", settings.revision, 32);
    io.String(L"name", settings.name, 100);
    io.String(L"theme", options.theme, 10);
    io.String(L"accent", options.accent, 7);
    io.Number(L"transparency", settings.transparency, 0, 55);
    io.Number(L"blur", settings.blur, 0, 36);
    io.Number(L"diameter", settings.diameter, 220, 720);
    io.Number(L"centerDiameter", settings.centerDiameter, 88, settings.diameter - 96);
    io.Number(L"iconSize", settings.iconSize, 12, 64);
    io.Number(L"border", settings.border, 0, 4);
    io.Flag(L"dividers", settings.dividers);
    io.Flag(L"outerLabels", settings.outerLabels);
    io.Flag(L"centerLabel", settings.centerLabel);
    io.Flag(L"pageLabel", settings.pageLabel);
    io.String(L"idleLabel", settings.idleLabel, 200);
    io.Flag(L"animations", settings.animations);
    io.Flag(L"respectMotion", settings.respectMotion);
    io.Number(L"speed", settings.speed, 25, 300);
    io.Number(L"iconScale", settings.iconScale, 100, 150);
    io.Number(L"selectionMs", settings.selectionMs, 80, 600);
    io.String(L"pageTransition", options.transition, 10);
    io.String(L"shortcut", settings.shortcutText, 100);
    io.String(L"position", options.position, 10);
    io.Number(L"wheelSensitivity", settings.wheelSensitivity, 40, 360);
    io.Flag(L"closeAfterSelection", settings.closeAfterSelection);
    io.Flag(L"holdToSelect", settings.holdToSelect);
}

template<class IO, class S, class T>
void SlotFields(IO& io, S& slot, T& type) {
    io.String(L"name", slot.name, 200);
    io.String(L"type", type, 10);
    io.String(L"target", slot.target, 2048);
    io.String(L"arguments", slot.arguments, 4096);
    io.String(L"icon", slot.icon, 1024);
}

constexpr std::array<PCWSTR, 4> kActionNames = {L"empty", L"open", L"command", L"shortcut"};

void AppendProfile(std::wstring& json, const Settings& settings) {
    wchar_t accent[8]{};
    if (settings.accent) swprintf_s(accent, L"#%06lX", *settings.accent);
    const ProfileOptions options{
        1,
        settings.theme == Theme::Dark ? L"dark" : settings.theme == Theme::Light ? L"light" : L"system",
        accent,
        settings.transition == PageTransition::Rotate ? L"rotate" :
        settings.transition == PageTransition::Instant ? L"none" : L"crossfade",
        settings.centerOnMonitor ? L"monitor" : L"pointer",
    };
    json += L'{';
    JsonWriter writer{json};
    ProfileFields(writer, settings, options);
    writer.Key(L"pages");
    json += L'[';
    for (size_t page = 0; page < settings.pages.size(); ++page) {
        if (page) json += L',';
        json += L'{';
        JsonWriter pageWriter{json};
        pageWriter.String(L"name", settings.pageNames[page]);
        pageWriter.Key(L"slots");
        json += L'[';
        for (int index = 0; index < kSlots; ++index) {
            if (index) json += L',';
            json += L'{';
            JsonWriter slotWriter{json};
            const Slot& slot = settings.pages[page][index];
            const std::wstring type = kActionNames[static_cast<size_t>(slot.action)];
            SlotFields(slotWriter, slot, type);
            if (slot.category != ActionCategory::Unspecified)
                slotWriter.String(L"category", CategoryInfo(slot.category).key);
            json += L'}';
        }
        json += L"]}";
    }
    json += L"]}";
}

std::optional<Settings> ParseProfile(const std::wstring& text, std::wstring& error) {
    // Windows.Data.Json is a public OS API. Use the declared WinRT interfaces
    // and query the activation factory and vector by their interface IDs.
    ComPtr<Json::IJsonValueStatics> factory;
    ComPtr<Json::IJsonValue> value;
    ComPtr<Json::IJsonObject> object;
    if (FAILED(RoGetActivationFactory(HStringReference(L"Windows.Data.Json.JsonValue").Get(),
                                      IID_PPV_ARGS(&factory)))) {
        error = L"Windows JSON support is unavailable.";
        return {};
    }
    if (FAILED(factory->Parse(HStringReference(text.c_str(), static_cast<UINT32>(text.size())).Get(),
                              &value)) ||
        FAILED(value->GetObject(&object))) {
        error = L"The active profile is not a JSON object.";
        return {};
    }

    Settings settings{};
    ProfileOptions options;
    JsonReader reader{object.Get(), error};
    ProfileFields(reader, settings, options);
    if (settings.revision.size() != 32 || !IsHex(settings.revision)) reader.Invalid(L"revision");
    if (Trim(settings.name).empty()) reader.Invalid(L"name");
    if (options.theme == L"light") settings.theme = Theme::Light;
    else if (options.theme == L"dark") settings.theme = Theme::Dark;
    else if (options.theme != L"system") reader.Invalid(L"theme");
    if (!options.accent.empty()) {
        if (options.accent.size() != 7 || options.accent[0] != L'#' ||
            !(settings.accent = ParseNumber(std::wstring_view(options.accent).substr(1), 16))) {
            reader.Invalid(L"accent");
        }
    }
    if (options.transition == L"none") settings.transition = PageTransition::Instant;
    else if (options.transition == L"rotate") settings.transition = PageTransition::Rotate;
    else if (options.transition != L"crossfade") reader.Invalid(L"pageTransition");
    if (options.position == L"monitor") settings.centerOnMonitor = true;
    else if (options.position != L"pointer") reader.Invalid(L"position");
    settings.shortcut = ParseChord(settings.shortcutText);
    if (!settings.shortcut) reader.Invalid(L"shortcut");
    if (!error.empty()) return {};

    ComPtr<Json::IJsonArray> pages;
    UINT32 count;
    if (!reader.Array(L"pages", pages, count, 1, kMaxPages)) return {};
    for (UINT32 page = 0; page < count; ++page) {
        ComPtr<Json::IJsonObject> pageObject;
        if (FAILED(pages->GetObjectAt(page, &pageObject))) {
            reader.Invalid(L"pages");
            return {};
        }
        JsonReader pageReader{pageObject.Get(), error};
        pageReader.String(L"name", settings.pageNames.emplace_back(), 100);
        if (Trim(settings.pageNames.back()).empty()) reader.Invalid(L"page name");
        ComPtr<Json::IJsonArray> slots;
        UINT32 slotCount;
        if (!pageReader.Array(L"slots", slots, slotCount, kSlots, kSlots)) return {};
        auto& pageSlots = settings.pages.emplace_back();
        for (UINT32 index = 0; index < slotCount; ++index) {
            ComPtr<Json::IJsonObject> slotObject;
            if (FAILED(slots->GetObjectAt(index, &slotObject))) {
                reader.Invalid(L"slots");
                return {};
            }
            Slot& slot = pageSlots[index];
            JsonReader slotReader{slotObject.Get(), error};
            std::wstring type;
            SlotFields(slotReader, slot, type);
            const auto action = std::find(kActionNames.begin(), kActionNames.end(), type);
            if (action == kActionNames.end()) reader.Invalid(L"action type");
            else slot.action = static_cast<Action>(action - kActionNames.begin());
            HString category;
            if (SUCCEEDED(slotObject->GetNamedString(HStringReference(L"category").Get(), category.GetAddressOf()))) {
                UINT32 length = 0;
                const auto* value = category.GetRawBuffer(&length);
                for (size_t i = 0; i < ARRAYSIZE(kActionCategories); ++i) {
                    if (std::wstring_view(value, length) == kActionCategories[i].key &&
                        slot.action == kActionCategories[i].action) {
                        slot.category = static_cast<ActionCategory>(i);
                        break;
                    }
                }
            }
            slot.target = Trim(slot.target);
            if (slot.action == Action::Shortcut && !ParseChord(slot.target)) reader.Invalid(L"slot shortcut");
            if (slot.action != Action::Empty && slot.target.empty()) reader.Invalid(L"slot target");
            ResolveGlyph(slot);
            if (slot.icon.starts_with(L"glyph:") && !slot.glyph) reader.Invalid(L"slot glyph");
            if (slot.icon.starts_with(L"asset:")) reader.Invalid(L"slot icon");
            if (!error.empty()) return {};
        }
    }
    return settings;
}

std::string ReadProfileBytes(const std::wstring& path, std::wstring& error, bool rejectReparse = false) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN |
                              (rejectReparse ? FILE_FLAG_OPEN_REPARSE_POINT : 0), nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        const auto code = GetLastError();
        error = code == ERROR_FILE_NOT_FOUND || code == ERROR_PATH_NOT_FOUND
                    ? L"No active profile." : L"Could not read the active profile.";
        return {};
    }
    if (rejectReparse) {
        BY_HANDLE_FILE_INFORMATION info{};
        if (!GetFileInformationByHandle(file, &info) ||
            (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) || info.nNumberOfLinks != 1) {
            CloseHandle(file);
            error = L"The active profile must be an ordinary file in Windhawk storage.";
            return {};
        }
    }
    LARGE_INTEGER size{};
    std::string contents;
    DWORD read = 0;
    if (GetFileSizeEx(file, &size) && size.QuadPart > 0 && size.QuadPart <= kMaxProfileBytes) {
        contents.resize(static_cast<size_t>(size.QuadPart));
        if (!ReadFile(file, contents.data(), static_cast<DWORD>(contents.size()), &read, nullptr) ||
            read != contents.size()) contents.clear();
    }
    CloseHandle(file);
    if (contents.empty()) error = L"The active profile is empty, incomplete, or too large.";
    return contents;
}

bool SaveProfileBytes(const std::wstring& path, const std::string& contents, bool replaceExisting = true) {
    GUID id;
    wchar_t unique[40];
    if (contents.empty() || contents.size() > kMaxProfileBytes || FAILED(CoCreateGuid(&id)) ||
        !StringFromGUID2(id, unique, ARRAYSIZE(unique))) return false;
    const auto temporary = path + L"." + unique + L".tmp";
    HANDLE file = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    bool saved = WriteFile(file, contents.data(), static_cast<DWORD>(contents.size()), &written, nullptr) &&
                 written == contents.size() && FlushFileBuffers(file);
    CloseHandle(file);
    if (saved) saved = MoveFileExW(temporary.c_str(), path.c_str(),
                                   MOVEFILE_WRITE_THROUGH | (replaceExisting ? MOVEFILE_REPLACE_EXISTING : 0));
    if (!saved) DeleteFileW(temporary.c_str());
    return saved;
}

std::wstring NewRevision() {
    GUID id;
    wchar_t text[40];
    if (FAILED(CoCreateGuid(&id)) || !StringFromGUID2(id, text, ARRAYSIZE(text))) return {};
    std::wstring revision;
    for (const wchar_t c : std::wstring_view(text)) if (std::iswxdigit(c)) revision += towlower(c);
    return revision;
}

std::optional<Settings> ParseProfileBytes(const std::string& bytes, std::wstring& error) {
    auto text = FromUtf8(bytes);
    if (text.starts_with(L'\xfeff')) text.erase(0, 1);
    if (text.empty()) {
        error = L"The active profile is not valid UTF-8.";
        return {};
    }
    return ParseProfile(text, error);
}

void MigrateEditorProfile(const std::wstring& path) {
    if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES ||
        GetLastError() != ERROR_FILE_NOT_FOUND) return;

    PWSTR folder = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &folder))) return;
    std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> owner(folder, CoTaskMemFree);
    const auto oldPath = std::wstring(folder) + L"\\FluentRadialMenu" + kProfileFile;
    std::wstring error;
    const auto bytes = ReadProfileBytes(oldPath, error);
    if (error == L"No active profile.") return;
    if (!error.empty() || !ParseProfileBytes(bytes, error)) {
        Wh_Log(L"Could not import the prerelease radial menu layout: %s", error.c_str());
        return;
    }
    if (!SaveProfileBytes(path, bytes, false) &&
        GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Could not copy the prerelease radial menu layout to Windhawk storage.");
    }
}

float EaseOut(float progress) {
    const float remainder = 1 - progress;
    return 1 - remainder * remainder * remainder;
}

struct Tween {
    float start = 0;
    float target = 0;
    Time began{};
    float duration = 0;

    float Elapsed(Time now) const {
        return std::chrono::duration<float, std::milli>(now - began).count();
    }

    float Value(Time now) const {
        if (!duration || start == target) return target;
        return std::lerp(start, target, EaseOut(std::clamp(Elapsed(now) / duration, 0.0f, 1.0f)));
    }

    bool Running(Time now) const {
        return start != target && Elapsed(now) < duration;
    }

    void Snap(float value) {
        start = target = value;
        duration = 0;
    }

    void To(float value, Time now, float milliseconds) {
        if (value == target && milliseconds > 0) return;
        start = Value(now);
        target = value;
        began = now;
        duration = milliseconds;
    }
};

struct CenterLabel {
    std::wstring shown, pending;
    Tween alpha, scale;
    bool replacing = false;

    void Reset(const std::wstring& text) {
        shown = pending = text;
        alpha.Snap(1);
        scale.Snap(1);
        replacing = false;
    }

    void Advance(Time now, float milliseconds) {
        if (!replacing || alpha.Running(now)) return;
        shown = pending;
        replacing = false;
        alpha.Snap(0);
        scale.Snap(0.96f);
        scale.To(1, now, milliseconds);
        alpha.To(1, now, milliseconds);
    }

    void Set(const std::wstring& text, Time now, float fadeOut, float fadeIn) {
        if (!fadeIn) {
            Reset(text);
            return;
        }
        if (text == pending) return;
        pending = text;
        replacing = text != shown;
        alpha.To(replacing ? 0 : 1, now, replacing && !shown.empty() ? fadeOut : fadeIn);
        if (shown.empty() && replacing) alpha.Snap(0);
        Advance(now, fadeIn);
    }
};

D2D1_POINT_2F Polar(float radius, float angle) {
    return D2D1::Point2F(radius * std::sin(angle), -radius * std::cos(angle));
}

struct Geometry {
    float radius = 0;
    float inner = 0;
    float pixelsPerDip = 1;
    POINT origin{};
    int extent = 0;
    float Mid() const { return (radius + inner) / 2; }
    float Offset() const { return extent / (2 * pixelsPerDip); }

    // The stable hit area follows the drawn circle's center on every DPI.
    // Opening and closing only animate the visual scale.
    int Hit(POINT point) const {
        const float x = (static_cast<float>(point.x) - origin.x - extent / 2.0f) / pixelsPerDip;
        const float y = (static_cast<float>(point.y) - origin.y - extent / 2.0f) / pixelsPerDip;
        const float distance = x * x + y * y;
        if (distance > radius * radius) return -2;
        if (distance < inner * inner) return -1;
        const float angle = std::atan2(x, -y);
        const int index = static_cast<int>(std::floor((angle + kStep / 2) / kStep));
        return (index + kSlots) % kSlots;
    }
};

float NearestAngle(float current, float target) {
    return current + std::remainder(target - current, kTurn);
}

struct Wheel {
    int remainder = 0;
    Time lastInput{}, lastStep{};

    int Push(int delta, int threshold, Time now) {
        if (now - lastInput > 240ms || (remainder > 0 && delta < 0) ||
            (remainder < 0 && delta > 0)) remainder = 0;
        lastInput = now;
        remainder = std::clamp(remainder + delta, -threshold, threshold);
        if (std::abs(remainder) < threshold) return 0;
        remainder = 0;
        if (now - lastStep < 105ms) return 0;
        lastStep = now;
        return delta < 0 ? 1 : -1;
    }
};

struct PageVisual {
    Tween alpha;
    Tween angle;
    std::array<Tween, kSlots> icons;
};

struct VisualState {
    int page = 0;
    int selected = -1;
    bool hasAngle = false;
    Tween visibility, scale, angle, highlight;
    CenterLabel label;
    std::array<PageVisual, kMaxPages> pages;

    bool Animating(Time now) const {
        if (visibility.Running(now) || scale.Running(now) || angle.Running(now) ||
            highlight.Running(now) || label.alpha.Running(now) ||
            label.scale.Running(now) || label.replacing) return true;
        for (const auto& pageVisual : pages) {
            if (pageVisual.alpha.Running(now) || pageVisual.angle.Running(now)) return true;
            for (const auto& icon : pageVisual.icons) {
                if (icon.Running(now)) return true;
            }
        }
        return false;
    }
};



struct IconPixels {
    UINT width = 0, height = 0;
    std::vector<BYTE> bgra;
};

struct ActionPreset {
    std::wstring name, target;
};

std::vector<ActionPreset> ListInstalledApps() {
    constexpr wchar_t prefix[] = L"shell:AppsFolder\\";
    ComPtr<IShellItem> folder;
    ComPtr<IEnumShellItems> items;
    if (FAILED(SHGetKnownFolderItem(FOLDERID_AppsFolder, KF_FLAG_DEFAULT, nullptr, IID_PPV_ARGS(&folder))) ||
        FAILED(folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&items)))) return {};

    std::vector<ActionPreset> apps;
    ComPtr<IShellItem> item;
    while (items->Next(1, item.ReleaseAndGetAddressOf(), nullptr) == S_OK) {
        using ShellString = std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)>;
        PWSTR value = nullptr;
        if (FAILED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &value))) continue;
        ShellString name(value, CoTaskMemFree);
        value = nullptr;
        if (FAILED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &value))) continue;
        ShellString id(value, CoTaskMemFree);
        if (!name || !*name || !id || !*id) continue;
        std::wstring target = std::wstring(prefix) + id.get();
        if (target.size() > 2048) continue;
        std::wstring label = name.get();
        label.resize(std::min<size_t>(label.size(), 200));
        apps.push_back({std::move(label), std::move(target)});
    }
    std::sort(apps.begin(), apps.end(), [](const ActionPreset& a, const ActionPreset& b) {
        const int byName = CompareStringOrdinal(a.name.c_str(), -1, b.name.c_str(), -1, TRUE);
        if (byName != CSTR_EQUAL) return byName == CSTR_LESS_THAN;
        return a.target < b.target;
    });
    return apps;
}

std::wstring AutomaticIconKey(const Slot& slot) {
    if (slot.Active() && (slot.action == Action::Open || slot.action == Action::Command)) {
        return L"shell:" + slot.target;
    }
    return {};
}

std::wstring IconKey(const Slot& slot) {
    if (!slot.Active() || slot.glyph) return {};
    if (!slot.icon.empty()) return L"custom:" + slot.icon;
    return AutomaticIconKey(slot);
}

std::shared_ptr<IconPixels> LoadIconPixels(IWICImagingFactory* wic, const std::wstring& key) {
    ComPtr<IWICBitmapSource> source;
    const bool custom = key.starts_with(L"custom:");
    std::wstring path = NativePath(Expand(key.substr(custom ? 7 : 6)));
    if (path.starts_with(L"\\\\") || path.starts_with(L"//")) return {};
    if (custom) {
        ComPtr<IWICBitmapDecoder> decoder;
        ComPtr<IWICBitmapFrameDecode> frame;
        if (SUCCEEDED(wic->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
                                                    WICDecodeMetadataCacheOnDemand, &decoder)) &&
            SUCCEEDED(decoder->GetFrame(0, &frame))) source = frame;
    }

    if (!source) {
        HICON icon = nullptr;
        if (custom) {
            int index = 0;
            auto comma = path.find_last_of(L',');
            if (comma != path.npos) {
                auto digits = std::wstring_view(path).substr(comma + 1);
                bool negative = digits.starts_with(L'-');
                if (negative) digits.remove_prefix(1);
                if (auto number = ParseNumber(digits, 10)) {
                    index = static_cast<int>(*number) * (negative ? -1 : 1);
                    path.resize(comma);
                }
            }
            ExtractIconExW(path.c_str(), index, &icon, nullptr, 1);
        } else {
            SHFILEINFOW info{};
            if (_wcsnicmp(path.c_str(), L"shell:", 6) == 0) {
                PIDLIST_ABSOLUTE item = nullptr;
                if (SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &item, 0, nullptr))) {
                    if (SHGetFileInfoW(reinterpret_cast<PCWSTR>(item), 0, &info, sizeof(info),
                                       SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON)) icon = info.hIcon;
                    CoTaskMemFree(item);
                }
            } else {
                if (path.find(L':') != path.npos && path.find(L':') != 1) return {};
                DWORD attributes = GetFileAttributesW(path.c_str());
                UINT flags = SHGFI_ICON | SHGFI_LARGEICON;
                if (attributes == INVALID_FILE_ATTRIBUTES) {
                    wchar_t resolved[MAX_PATH];
                    DWORD length = SearchPathW(nullptr, path.c_str(), nullptr, ARRAYSIZE(resolved),
                                               resolved, nullptr);
                    if (length && length < ARRAYSIZE(resolved)) {
                        path.assign(resolved, length);
                    } else {
                        flags |= SHGFI_USEFILEATTRIBUTES;
                        attributes = FILE_ATTRIBUTE_NORMAL;
                    }
                }
                if (SHGetFileInfoW(path.c_str(), attributes, &info, sizeof(info), flags)) {
                    icon = info.hIcon;
                }
            }
        }

        if (!icon) return {};
        ComPtr<IWICBitmap> bitmap;
        const HRESULT result = wic->CreateBitmapFromHICON(icon, &bitmap);
        DestroyIcon(icon);
        if (FAILED(result)) return {};
        source = bitmap;
    }

    UINT width, height;
    if (FAILED(source->GetSize(&width, &height)) || !width || !height) return {};
    constexpr UINT maxSize = 128;
    const UINT largest = std::max(width, height);
    if (largest > maxSize) {
        width = std::max(1u, static_cast<UINT>(maxSize * (double)width / largest));
        height = std::max(1u, static_cast<UINT>(maxSize * (double)height / largest));
        ComPtr<IWICBitmapScaler> scaler;
        if (FAILED(wic->CreateBitmapScaler(&scaler)) ||
            FAILED(scaler->Initialize(source.Get(), width, height, WICBitmapInterpolationModeFant))) {
            return {};
        }
        source = scaler;
    }

    ComPtr<IWICFormatConverter> converter;
    if (FAILED(wic->CreateFormatConverter(&converter)) ||
        FAILED(converter->Initialize(source.Get(), GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeCustom))) return {};
    auto pixels = std::make_shared<IconPixels>();
    pixels->width = width;
    pixels->height = height;
    pixels->bgra.resize(width * height * 4);
    if (FAILED(converter->CopyPixels(nullptr, width * 4,
                                     static_cast<UINT>(pixels->bgra.size()), pixels->bgra.data()))) {
        return {};
    }
    return pixels;
}

// Shell handlers and icon decoders can block on I/O. Keep them off the input
// and rendering thread. The worker owns its COM apartment and never draws.
class ShellWorker {
public:
    explicit ShellWorker(HWND window) : window_(window) {}

    static DWORD WINAPI Run(void* parameter) {
        std::unique_ptr<std::shared_ptr<ShellWorker>> owner(
            static_cast<std::shared_ptr<ShellWorker>*>(parameter));
        (*owner)->Work();
        return 0;
    }

    void RequestIcons(const Settings& settings, std::wstring automatic = {}) {
        std::lock_guard lock(mutex_);
        std::unordered_map<std::wstring, std::shared_ptr<IconPixels>> retained;
        std::deque<std::wstring> pending;
        auto request = [&](std::wstring key) {
            if (key.empty() || retained.contains(key)) return;
            if (auto found = pixels_.find(key); found != pixels_.end()) {
                retained.emplace(key, found->second);
            } else if (key != loadingIcon_ && std::find(pending.begin(), pending.end(), key) == pending.end()) {
                pending.push_back(std::move(key));
            }
        };
        request(std::move(automatic));
        for (const auto& page : settings.pages) {
            for (const Slot& slot : page) request(IconKey(slot));
        }
        pixels_ = std::move(retained);
        icons_ = std::move(pending);
        changed_.notify_one();
    }

    std::shared_ptr<IconPixels> FindIcon(const std::wstring& key) {
        std::lock_guard lock(mutex_);
        auto found = pixels_.find(key);
        return found != pixels_.end() ? found->second : nullptr;
    }

    void RequestApps() {
        std::lock_guard lock(mutex_);
        listApps_ = true;
        changed_.notify_one();
    }

    std::vector<ActionPreset> TakeApps() {
        std::lock_guard lock(mutex_);
        return std::move(apps_);
    }

    void Launch(const Slot& slot) {
        std::lock_guard lock(mutex_);
        if (actions_.size() >= 8) return;
        actions_.push_back(slot);
        changed_.notify_one();
    }

    void Stop() {
        std::lock_guard lock(mutex_);
        window_ = nullptr;
        changed_.notify_one();
    }

private:
    void Work() {
        if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return;
        ComPtr<IWICImagingFactory> wic;
        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(&wic));
        std::unique_lock lock(mutex_);
        for (;;) {
            changed_.wait(lock, [&] { return !window_ || !actions_.empty() || !icons_.empty() || listApps_; });
            if (!window_) break;
            if (!actions_.empty()) {
                Slot action = std::move(actions_.front());
                actions_.pop_front();
                lock.unlock();
                RunAction(action);
                lock.lock();
                continue;
            }
            if (std::exchange(listApps_, false)) {
                lock.unlock();
                auto apps = ListInstalledApps();
                lock.lock();
                if (window_) {
                    apps_ = std::move(apps);
                    PostMessageW(window_, kAppsReady, 0, 0);
                }
                continue;
            }
            std::wstring key = std::move(icons_.front());
            icons_.pop_front();
            loadingIcon_ = key;
            lock.unlock();
            auto result = wic ? LoadIconPixels(wic.Get(), key) : nullptr;
            lock.lock();
            loadingIcon_.clear();
            if (window_) {
                pixels_.emplace(std::move(key), std::move(result));
                PostMessageW(window_, kIconsReady, 0, 0);
            }
        }
        lock.unlock();
        wic.Reset();
        CoUninitialize();
    }

    static void RunAction(const Slot& slot) {
        auto target = Expand(slot.target);
        const auto arguments = Expand(slot.arguments);
        const bool command = slot.action == Action::Command;
        const bool bare = target.find_first_of(L"\\/:") == std::wstring::npos;
        if (bare && (command || (UsesWow64FileRedirection() &&
                                 (IsExecutablePath(target) || target.find(L'.') == target.npos)))) {
            auto resolved = ResolveBareExecutable(target);
            if (!resolved.empty() && (command || IsExecutablePath(resolved)))
                target = std::move(resolved);
        }
        if (!command && UsesWow64FileRedirection()) {
            if (bare && target.size() >= 4 &&
                _wcsicmp(target.c_str() + target.size() - 4, L".msc") == 0) {
                const auto console = WindowsDirectory() + L"\\System32\\" + target;
                if (GetFileAttributesW(NativePath(console).c_str()) != INVALID_FILE_ATTRIBUTES)
                    target = console;
            }
            // Bare-name resolution can return the alias used for this process.
            // Explorer and the elevation service need the real System32 path.
            if (ReplaceSystemDirectory(target, L"Sysnative", L"System32") ||
                ReplaceSystemDirectory(target, L"System32", L"System32")) {
                if (!OpenFromExplorer(target, arguments))
                    Wh_Log(L"Could not open the native System32 target through Explorer.");
                return;
            }
        }
        if (command) {
            target = NativePath(std::move(target));
            // Pass the executable separately from its arguments.
            std::wstring commandLine = L"\"" + target + L"\" " + arguments;
            STARTUPINFOW startup{};
            startup.cb = sizeof(startup);
            PROCESS_INFORMATION process{};
            if (CreateProcessW(target.c_str(), commandLine.data(), nullptr, nullptr, FALSE, 0,
                               nullptr, nullptr, &startup, &process)) {
                CloseHandle(process.hThread);
                CloseHandle(process.hProcess);
                return;
            }
        } else {
            SHELLEXECUTEINFOW execute{};
            execute.cbSize = sizeof(execute);
            execute.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
            execute.lpFile = target.c_str();
            execute.lpParameters = arguments.empty() ? nullptr : arguments.c_str();
            execute.nShow = SW_SHOWNORMAL;
            if (ShellExecuteExW(&execute)) return;
        }
        Wh_Log(L"Could not open radial menu target (error %lu)", GetLastError());
    }

    static bool OpenFromExplorer(const std::wstring& target, const std::wstring& arguments) {
        // Let the desktop shell resolve native System32 paths and file associations.
        ComPtr<IShellWindows> windows;
        if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                    IID_PPV_ARGS(&windows)))) return false;
        VARIANT location{}, root{};
        location.vt = VT_I4;
        location.lVal = CSIDL_DESKTOP;
        long window = 0;
        ComPtr<IDispatch> desktop;
        ComPtr<IServiceProvider> services;
        ComPtr<IShellBrowser> browser;
        ComPtr<IShellView> view;
        ComPtr<IDispatch> background, application;
        ComPtr<IShellFolderViewDual> folderView;
        ComPtr<IShellDispatch2> shell;
        if (windows->FindWindowSW(&location, &root, SWC_DESKTOP, &window, SWFO_NEEDDISPATCH,
                                  &desktop) != S_OK || !desktop ||
            FAILED(desktop.As(&services)) ||
            FAILED(services->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser))) ||
            FAILED(browser->QueryActiveShellView(&view)) ||
            FAILED(view->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&background))) ||
            FAILED(background.As(&folderView)) ||
            FAILED(folderView->get_Application(&application)) ||
            FAILED(application.As(&shell))) return false;

        std::unique_ptr<wchar_t, decltype(&SysFreeString)> file(SysAllocString(target.c_str()), SysFreeString);
        std::unique_ptr<wchar_t, decltype(&SysFreeString)> params(SysAllocString(arguments.c_str()), SysFreeString);
        if (!file || !params) return false;
        VARIANT parameters{}, show{};
        parameters.vt = VT_BSTR;
        parameters.bstrVal = params.get();
        show.vt = VT_I4;
        show.lVal = SW_SHOWNORMAL;
        return SUCCEEDED(shell->ShellExecute(file.get(), parameters, root, root, show));
    }

    HWND window_;
    bool listApps_ = false;
    std::vector<ActionPreset> apps_;
    std::mutex mutex_;
    std::condition_variable changed_;
    std::wstring loadingIcon_;
    std::deque<std::wstring> icons_;
    std::deque<Slot> actions_;
    std::unordered_map<std::wstring, std::shared_ptr<IconPixels>> pixels_;
};

struct Palette {
    D2D1_COLOR_F rim, panel, panelBottom, center, text, secondary, divider, accent, shadow;
    bool highContrast = false;
};

D2D1_COLOR_F WithAlpha(D2D1_COLOR_F color, float alpha) {
    color.a *= alpha;
    return color;
}

D2D1_COLOR_F FromColorRef(COLORREF value) {
    return D2D1::ColorF(GetRValue(value) / 255.0f, GetGValue(value) / 255.0f,
                        GetBValue(value) / 255.0f);
}

DWORD WindowsAccent() {
    DWORD argb = 0xFF0078D4;
    BOOL opaque;
    if (FAILED(DwmGetColorizationColor(&argb, &opaque))) return 0x0078D4;
    return argb & 0xFFFFFF;
}

Palette ReadPalette(const Settings& settings) {
    HIGHCONTRASTW contrast{sizeof(contrast), 0, nullptr};
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        const auto panel = FromColorRef(GetSysColor(COLOR_WINDOW));
        const auto text = FromColorRef(GetSysColor(COLOR_WINDOWTEXT));
        return {text, panel, panel, panel, text, text, WithAlpha(text, 0.6f),
                FromColorRef(GetSysColor(COLOR_HIGHLIGHT)), D2D1::ColorF(0, 0.0f), true};
    }

    DWORD light = 1;
    DWORD size = sizeof(light);
    if (settings.theme == Theme::System) {
        RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
    } else {
        light = settings.theme == Theme::Light;
    }
    const auto accent = D2D1::ColorF(settings.accent.value_or(WindowsAccent()));
    const float opacity = 1 - settings.transparency / 100.0f;
    if (light) {
        return {D2D1::ColorF(0xffffff, 0.88f), D2D1::ColorF(0xf9fafb, opacity),
                D2D1::ColorF(0xe6e8ed, opacity), D2D1::ColorF(0xffffff, 0.27f),
                D2D1::ColorF(0x191b20), D2D1::ColorF(0x424650),
                D2D1::ColorF(0x424650, 0.17f), accent, D2D1::ColorF(0x151e31, 0.21f)};
    }
    return {D2D1::ColorF(0xffffff, 0.23f), D2D1::ColorF(0x30333b, opacity),
            D2D1::ColorF(0x191b23, opacity), D2D1::ColorF(0x080a0f, 0.23f),
            D2D1::ColorF(0xf6f8fc), D2D1::ColorF(0xc8cdd8),
            D2D1::ColorF(0xffffff, 0.16f), accent, D2D1::ColorF(0x080b12, 0.42f)};
}

class Surface {
public:
    ~Surface() { Reset(); }

    bool Resize(int extent) { return Resize(extent, extent); }

    bool Resize(int width, int height) {
        if (width == width_ && height == height_ && dc_) return true;
        Reset();
        dc_ = CreateCompatibleDC(nullptr);
        if (!dc_) return false;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        bitmap_ = CreateDIBSection(dc_, &info, DIB_RGB_COLORS, &bits_, nullptr, 0);
        if (!bitmap_) {
            Reset();
            return false;
        }
        original_ = SelectObject(dc_, bitmap_);
        width_ = width;
        height_ = height;
        return true;
    }

    HDC Dc() const { return dc_; }
    BYTE* Bits() const { return static_cast<BYTE*>(bits_); }

    void Reset() {
        if (original_) SelectObject(dc_, original_);
        if (bitmap_) DeleteObject(bitmap_);
        if (dc_) DeleteDC(dc_);
        dc_ = nullptr;
        original_ = nullptr;
        bitmap_ = nullptr;
        bits_ = nullptr;
        width_ = height_ = 0;
    }

private:
    HDC dc_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ original_ = nullptr;
    void* bits_ = nullptr;
    int width_ = 0, height_ = 0;
};

void BlurPass(const BYTE* input, BYTE* output, int size, int radius, bool vertical) {
    const int stride = vertical ? size * 4 : 4;
    const int rowStride = vertical ? 4 : size * 4;
    for (int row = 0; row < size; ++row) {
        const BYTE* from = input + row * rowStride;
        BYTE* to = output + row * rowStride;
        for (int channel = 0; channel < 3; ++channel) {
            int sum = 0;
            for (int offset = -radius; offset <= radius; ++offset) {
                sum += from[std::clamp(offset, 0, size - 1) * stride + channel];
            }
            for (int i = 0; i < size; ++i) {
                to[i * stride + channel] = static_cast<BYTE>(sum / (radius * 2 + 1));
                sum -= from[std::max(0, i - radius) * stride + channel];
                sum += from[std::min(size - 1, i + radius + 1) * stride + channel];
            }
        }
    }
}

IconPixels CaptureBackdrop(const Geometry& geometry, int strength) {
    IconPixels result;
    if (!strength) return result;
    const int size = std::clamp(geometry.extent / 5, 36, 256);
    Surface capture;
    if (!capture.Resize(size)) return result;
    HDC screen = GetDC(nullptr);
    if (!screen) return result;
    SetStretchBltMode(capture.Dc(), HALFTONE);
    SetBrushOrgEx(capture.Dc(), 0, 0, nullptr);
    const BOOL copied = StretchBlt(capture.Dc(), 0, 0, size, size, screen,
                                   geometry.origin.x, geometry.origin.y,
                                   geometry.extent, geometry.extent, SRCCOPY | CAPTUREBLT);
    GdiFlush();
    ReleaseDC(nullptr, screen);
    if (!copied) return result;

    const size_t bytes = static_cast<size_t>(size) * size * 4;
    std::vector<BYTE> scratch(bytes);
    result.bgra.assign(bytes, 255);
    const int radius = std::clamp(static_cast<int>(std::round(
        strength * geometry.pixelsPerDip * size / geometry.extent * 0.66f)), 1, 24);
    BlurPass(capture.Bits(), scratch.data(), size, radius, false);
    BlurPass(scratch.data(), capture.Bits(), size, radius, true);
    BlurPass(capture.Bits(), scratch.data(), size, radius, false);
    BlurPass(scratch.data(), result.bgra.data(), size, radius, true);
    result.width = result.height = size;
    return result;
}



struct PreviewDrag {
    int source = -1, destination = -1;
    POINT pointer{};
};

class Renderer {
public:
    bool Init() {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory_.GetAddressOf())) ||
            FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                       reinterpret_cast<IUnknown**>(write_.GetAddressOf())))) {
            return false;
        }
        return Format(L"Segoe UI Variable Text", 18, DWRITE_FONT_WEIGHT_SEMI_BOLD, &title_) &&
               Format(L"Segoe UI Variable Text", 11, DWRITE_FONT_WEIGHT_NORMAL, &small_) &&
               Format(L"Segoe Fluent Icons", 32, DWRITE_FONT_WEIGHT_NORMAL, &glyph_);
    }

    void Configure(const Geometry& geometry, const Palette& palette, IconPixels backdrop) {
        geometry_ = geometry;
        palette_ = palette;
        backdrop_ = std::move(backdrop);
        ResetTarget();
        segment_.Reset();
        if (FAILED(factory_->CreatePathGeometry(&segment_))) return;
        ComPtr<ID2D1GeometrySink> sink;
        if (FAILED(segment_->Open(&sink))) {
            segment_.Reset();
            return;
        }
        const float start = -kStep / 2;
        const float end = kStep / 2;
        const float outer = geometry.radius - 0.8f;
        const float inner = geometry.inner + 0.6f;
        sink->BeginFigure(Polar(outer, start), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddArc(D2D1::ArcSegment(Polar(outer, end), D2D1::SizeF(outer, outer), 0,
                                      D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
        sink->AddLine(Polar(inner, end));
        sink->AddArc(D2D1::ArcSegment(Polar(inner, start), D2D1::SizeF(inner, inner), 0,
                                      D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        if (FAILED(sink->Close())) segment_.Reset();
    }

    void ReleaseSurface() {
        ResetTarget();
        surface_.Reset();
        backdrop_ = {};
        icons_.clear();
        layouts_.clear();
    }

    bool Draw(HWND window, const Settings& settings, const VisualState& state,
              ShellWorker& shell, Time now) {
        if (!DrawSurface(settings, state, shell, now)) return false;
        POINT from{};
        SIZE size{geometry_.extent, geometry_.extent};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, static_cast<BYTE>(std::lround(
            state.visibility.Value(now) * 255)), AC_SRC_ALPHA};
        return UpdateLayeredWindow(window, nullptr, &geometry_.origin, &size, surface_.Dc(),
                                   &from, 0, &blend, ULW_ALPHA);
    }

    bool DrawPreview(HDC dc, const Settings& settings, const VisualState& state,
                     ShellWorker& shell, Time now, const PreviewDrag* drag = nullptr) {
        if (!DrawSurface(settings, state, shell, now, drag)) return false;
        const int size = geometry_.extent;
        const auto alpha = static_cast<BYTE>(std::lround(state.visibility.Value(now) * 255));
        return AlphaBlend(dc, geometry_.origin.x, geometry_.origin.y, size, size,
                          surface_.Dc(), 0, 0, size, size, {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA});
    }

private:
    bool DrawSurface(const Settings& settings, const VisualState& state,
                     ShellWorker& shell, Time now, const PreviewDrag* drag = nullptr, bool retry = true) {
        if (!target_ && !CreateTarget()) return false;
        const float radius = geometry_.radius;
        const float inner = geometry_.inner;
        const float scale = state.scale.Value(now);
        const auto center = D2D1::Matrix3x2F::Translation(geometry_.Offset(), geometry_.Offset());
        const auto transform = D2D1::Matrix3x2F::Scale(scale, scale) * center;
        target_->BeginDraw();
        target_->Clear(D2D1::ColorF(0, 0.0f));
        target_->SetTransform(transform);
        if (shadow_) target_->FillEllipse(D2D1::Ellipse({0, 3}, radius + 16, radius + 16),
                                          shadow_.Get());
        if (backdropBrush_) {
            target_->FillEllipse(D2D1::Ellipse({0, 0}, radius, radius), backdropBrush_.Get());
        }
        target_->FillEllipse(D2D1::Ellipse({0, 0}, radius, radius), panel_.Get());
        FillCircle(inner, palette_.center);

        if (segment_ && state.highlight.Value(now) > 0.001f) {
            target_->SetTransform(D2D1::Matrix3x2F::Rotation(
                state.angle.Value(now) * 360 / kTurn) * transform);
            SetColor(WithAlpha(palette_.accent,
                               state.highlight.Value(now) * (palette_.highContrast ? 0.72f : 0.36f)));
            target_->FillGeometry(segment_.Get(), brush_.Get());
            target_->SetTransform(transform);
        }
        if (segment_ && drag && drag->destination >= 0 && drag->source != drag->destination) {
            target_->SetTransform(D2D1::Matrix3x2F::Rotation(drag->destination * 360.f / kSlots) * transform);
            SetColor(WithAlpha(palette_.accent, 0.55f));
            target_->FillGeometry(segment_.Get(), brush_.Get());
            target_->SetTransform(transform);
        }

        if (settings.dividers) {
            SetColor(palette_.divider);
            for (int slot = 0; slot < kSlots; ++slot) {
                const float angle = (slot + 0.5f) * kStep;
                target_->DrawLine(Polar(inner + 1, angle), Polar(radius - 1, angle),
                                  brush_.Get(), 0.75f);
            }
        }
        if (settings.border || palette_.highContrast) {
            SetColor(palette_.rim);
            const float border = static_cast<float>(std::max(settings.border, 1));
            const float edge = radius - border / 2;
            target_->DrawEllipse(D2D1::Ellipse({0, 0}, edge, edge), brush_.Get(), border);
            SetColor(palette_.divider);
            target_->DrawEllipse(D2D1::Ellipse({0, 0}, inner, inner), brush_.Get(), border);
        }

        // Draw outgoing pages first and the current page last.
        for (int page = 0; page < static_cast<int>(settings.pages.size()); ++page) {
            if (page != state.page) DrawPage(settings, state, shell, page, now);
        }
        DrawPage(settings, state, shell, state.page, now, drag ? drag->source : -1);
        DrawCenter(settings, state, now, transform);
        if (drag) {
            const auto position = D2D1::Point2F(
                (drag->pointer.x - geometry_.origin.x) / geometry_.pixelsPerDip - geometry_.Offset(),
                (drag->pointer.y - geometry_.origin.y) / geometry_.pixelsPerDip - geometry_.Offset());
            const float size = std::clamp(static_cast<float>(settings.iconSize), 24.f, 40.f);
            FillCircle(size * 0.7f, palette_.panelBottom, position);
            DrawIcon(settings.pages[state.page][drag->source], shell, position, size, 1);
        }
        const HRESULT result = target_->EndDraw();
        if (result == D2DERR_RECREATE_TARGET) {
            ResetTarget();
            return retry && DrawSurface(settings, state, shell, now, drag, false);
        }
        return SUCCEEDED(result);
    }

    bool Format(PCWSTR font, float size, DWRITE_FONT_WEIGHT weight, IDWriteTextFormat** format) {
        return SUCCEEDED(write_->CreateTextFormat(font, nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                                                  DWRITE_FONT_STRETCH_NORMAL, size, L"", format)) &&
               SUCCEEDED((*format)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)) &&
               SUCCEEDED((*format)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
    }

    void ResetTarget() {
        for (auto& [key, icon] : icons_) icon.bitmap.Reset();
        shadow_.Reset();
        panel_.Reset();
        backdropBrush_.Reset();
        brush_.Reset();
        target_.Reset();
    }

    bool CreateTarget() {
        const auto& g = geometry_;
        if (!surface_.Resize(g.extent)) return false;
        const float dpi = 96 * g.pixelsPerDip;
        const auto properties = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);
        const RECT area{0, 0, g.extent, g.extent};
        if (FAILED(factory_->CreateDCRenderTarget(&properties, &target_)) ||
            FAILED(target_->BindDC(surface_.Dc(), &area)) ||
            FAILED(target_->CreateSolidColorBrush(palette_.text, &brush_))) {
            ResetTarget();
            return false;
        }
        target_->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        const D2D1_GRADIENT_STOP panelStops[] = {{0, palette_.panel}, {1, palette_.panelBottom}};
        ComPtr<ID2D1GradientStopCollection> stops;
        if (FAILED(target_->CreateGradientStopCollection(panelStops, ARRAYSIZE(panelStops), &stops)) ||
            FAILED(target_->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties({0, -g.radius}, {0, g.radius}),
                stops.Get(), &panel_))) {
            ResetTarget();
            return false;
        }
        const D2D1_GRADIENT_STOP shadowStops[] = {
            {0, palette_.shadow}, {g.radius / (g.radius + 16), palette_.shadow},
            {(g.radius + 7) / (g.radius + 16), WithAlpha(palette_.shadow, 0.25f)},
            {1, WithAlpha(palette_.shadow, 0)},
        };
        stops.Reset();
        if (SUCCEEDED(target_->CreateGradientStopCollection(shadowStops, ARRAYSIZE(shadowStops),
                                                           &stops))) {
            target_->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties({0, 3}, {0, 0}, g.radius + 16, g.radius + 16),
                stops.Get(), &shadow_);
        }
        if (backdrop_.width && !palette_.highContrast) {
            if (auto bitmap = Upload(backdrop_)) {
                const float extent = 2 * g.Offset();
                const auto transform = D2D1::Matrix3x2F::Scale(
                    extent / backdrop_.width, extent / backdrop_.height) *
                    D2D1::Matrix3x2F::Translation(-g.Offset(), -g.Offset());
                target_->CreateBitmapBrush(bitmap.Get(), D2D1::BitmapBrushProperties(),
                                           D2D1::BrushProperties(0.65f, transform), &backdropBrush_);
            }
        }
        return true;
    }

    ComPtr<ID2D1Bitmap> Upload(const IconPixels& pixels) {
        ComPtr<ID2D1Bitmap> bitmap;
        auto properties = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
        target_->CreateBitmap(D2D1::SizeU(pixels.width, pixels.height), pixels.bgra.data(),
                              pixels.width * 4, properties, &bitmap);
        return bitmap;
    }

    void SetColor(D2D1_COLOR_F color) { brush_->SetColor(color); }

    void FillCircle(float radius, D2D1_COLOR_F color, D2D1_POINT_2F center = {0, 0}) {
        SetColor(color);
        target_->FillEllipse(D2D1::Ellipse(center, radius, radius), brush_.Get());
    }

    void Text(const std::wstring& text, IDWriteTextFormat* format, float x, float y,
              float width, float height, D2D1_COLOR_F color, bool wrap = false) {
        if (text.empty() || color.a < 0.001f) return;
        const std::wstring key = std::to_wstring(format == title_.Get()) + L" " +
            std::to_wstring(width) + L" " + std::to_wstring(height) + L" " + text;
        auto found = layouts_.find(key);
        if (found == layouts_.end()) {
            ComPtr<IDWriteTextLayout> layout;
            if (FAILED(write_->CreateTextLayout(text.c_str(), static_cast<UINT>(text.size()),
                                                format, width, height, &layout))) return;
            layout->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
            ComPtr<IDWriteInlineObject> ellipsis;
            if (SUCCEEDED(write_->CreateEllipsisTrimmingSign(format, &ellipsis))) {
                const DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
                layout->SetTrimming(&trimming, ellipsis.Get());
            }
            if (layouts_.size() >= 256) layouts_.clear();
            found = layouts_.emplace(key, std::move(layout)).first;
        }
        SetColor(color);
        target_->DrawTextLayout({x - width / 2, y - height / 2}, found->second.Get(),
                                brush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }

    void DrawPage(const Settings& settings, const VisualState& state, ShellWorker& shell,
                  int page, Time now, int dragged = -1) {
        const auto& visual = state.pages[page];
        const float opacity = visual.alpha.Value(now);
        if (opacity < 0.001f) return;
        const float offset = visual.angle.Value(now);
        const float thickness = geometry_.radius - geometry_.inner;
        const float width = std::min(95.0f, 2 * geometry_.Mid() * std::sin(kStep / 2) - 9);
        for (int i = 0; i < kSlots; ++i) {
            const Slot& slot = settings.pages[page][i];
            auto position = Polar(geometry_.Mid(), i * kStep + offset);
            const float iconOpacity = i == dragged ? opacity * 0.25f : opacity;
            if (!slot.Active()) {
                FillCircle(1.6f, WithAlpha(palette_.secondary, opacity * 0.52f), position);
                continue;
            }
            const float size = std::min(static_cast<float>(settings.iconSize),
                                        thickness * (settings.outerLabels ? 0.46f : 0.62f))
                               * visual.icons[i].Value(now);
            if (settings.outerLabels) position.y -= 7;
            DrawIcon(slot, shell, position, size, iconOpacity);
            if (settings.outerLabels) {
                Text(slot.Label(), small_.Get(), position.x, position.y + size / 2 + 8,
                     width, 14, WithAlpha(palette_.text, iconOpacity));
            }
        }
    }

    void DrawIcon(const Slot& slot, ShellWorker& shell, D2D1_POINT_2F p, float size, float opacity) {
        if (auto key = IconKey(slot); !key.empty()) {
            auto found = icons_.find(key);
            if (found == icons_.end()) {
                if (auto pixels = shell.FindIcon(key)) {
                    found = icons_.emplace(std::move(key), CachedIcon{std::move(pixels), {}}).first;
                }
            }
            if (found != icons_.end()) {
                auto& icon = found->second;
                if (!icon.bitmap) icon.bitmap = Upload(*icon.pixels);
                if (icon.bitmap) {
                    const auto image = icon.bitmap->GetSize();
                    const float factor = size / std::max(image.width, image.height) / 2;
                    target_->DrawBitmap(icon.bitmap.Get(),
                        D2D1::RectF(p.x - image.width * factor, p.y - image.height * factor,
                                    p.x + image.width * factor, p.y + image.height * factor),
                        opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    return;
                }
            }
        }
        const wchar_t glyph = slot.glyph ? slot.glyph : slot.action == Action::Shortcut ? L'\uE765'
            : slot.target.find(L"://") != std::wstring::npos ? L'\uE71B' : L'\uE7B8';
        D2D1_MATRIX_3X2_F old;
        target_->GetTransform(&old);
        target_->SetTransform(D2D1::Matrix3x2F::Scale(size / 32, size / 32) *
                              D2D1::Matrix3x2F::Translation(p.x, p.y) *
                              D2D1::Matrix3x2F(old._11, old._12, old._21, old._22, old._31, old._32));
        SetColor(WithAlpha(palette_.text, opacity));
        target_->DrawTextW(&glyph, 1, glyph_.Get(), D2D1::RectF(-24, -24, 24, 24), brush_.Get());
        target_->SetTransform(old);
    }

    void DrawCenter(const Settings& settings, const VisualState& state, Time now,
                    const D2D1_MATRIX_3X2_F& transform) {
        const float inner = geometry_.inner;
        const float titleY = settings.pageLabel ? -inner * 0.18f : 0;
        if (settings.centerLabel) {
            const float size = std::min(1.0f, inner / 64) * state.label.scale.Value(now);
            const auto matrix = D2D1::Matrix3x2F::Scale(size, size, {0, titleY});
            target_->SetTransform(matrix * D2D1::Matrix3x2F(
                transform._11, transform._12, transform._21, transform._22,
                transform._31, transform._32));
            Text(state.label.shown, title_.Get(), 0, titleY, std::max(80.0f, inner * 1.65f),
                 46, WithAlpha(palette_.text, state.label.alpha.Value(now)), true);
            target_->SetTransform(transform);
        }
        if (!settings.pageLabel) return;
        const int count = static_cast<int>(settings.pages.size());
        const float pageY = settings.centerLabel ? inner * 0.20f : -8;
        const float dotY = pageY + std::min(20.0f, inner * 0.28f);
        Text(L"Page " + std::to_wstring(state.page + 1) + L" of " + std::to_wstring(count),
             small_.Get(), 0, pageY, inner * 1.65f, 16, palette_.secondary);
        const float gap = std::min(13.0f, inner * 1.5f / count);
        for (int page = 0; page < count; ++page) {
            const bool active = page == state.page;
            FillCircle(active ? 3.0f : 2.1f,
                       active ? palette_.accent : WithAlpha(palette_.secondary, 0.53f),
                       {(page - (count - 1) / 2.0f) * gap, dotY});
        }
    }

    struct CachedIcon {
        std::shared_ptr<IconPixels> pixels;
        ComPtr<ID2D1Bitmap> bitmap;
    };

    Geometry geometry_;
    Palette palette_{};
    Surface surface_;
    IconPixels backdrop_;
    ComPtr<ID2D1Factory> factory_;
    ComPtr<IDWriteFactory> write_;
    ComPtr<ID2D1PathGeometry> segment_;
    ComPtr<IDWriteTextFormat> title_, small_, glyph_;
    ComPtr<ID2D1DCRenderTarget> target_;
    ComPtr<ID2D1SolidColorBrush> brush_;
    ComPtr<ID2D1LinearGradientBrush> panel_;
    ComPtr<ID2D1RadialGradientBrush> shadow_;
    ComPtr<ID2D1BitmapBrush> backdropBrush_;
    std::unordered_map<std::wstring, CachedIcon> icons_;
    std::unordered_map<std::wstring, ComPtr<IDWriteTextLayout>> layouts_;
};



constexpr ULONG_PTR kSyntheticKeys = 0x46524d55;

bool SendChord(const Chord& chord) {
    std::array<INPUT, 10> inputs{};
    size_t count = 0;
    auto append = [&](WORD key, bool up) {
        auto& input = inputs[count++];
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        input.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
        input.ki.dwExtraInfo = kSyntheticKeys;
        const UINT scan = MapVirtualKeyW(key, MAPVK_VK_TO_VSC_EX);
        if ((scan >> 8) == 0xe0) input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    };

    std::array<WORD, 4> pressed{};
    size_t pressedCount = 0;
    for (const auto& modifier : kModifiers) {
        if ((modifier.flag & chord.modifiers) && !ModifierDown(modifier)) {
            pressed[pressedCount++] = modifier.key;
            append(modifier.key, false);
        }
    }
    append(static_cast<WORD>(chord.key), false);
    append(static_cast<WORD>(chord.key), true);
    while (pressedCount) append(pressed[--pressedCount], true);

    const UINT sent = SendInput(static_cast<UINT>(count), inputs.data(), sizeof(INPUT));
    if (sent == count) return true;
    // Release synthetic modifiers if only a prefix was accepted.
    for (UINT index = 0; index < sent; ++index) {
        if (inputs[index].ki.dwFlags & KEYEVENTF_KEYUP) continue;
        auto release = inputs[index];
        release.ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(1, &release, sizeof(INPUT));
    }
    return false;
}


constexpr wchar_t kEditorClass[] = L"Windhawk.FluentRadialMenu.Editor";
constexpr wchar_t kEditorWheelClass[] = L"Windhawk.FluentRadialMenu.Editor.Wheel";

struct EditorPreset {
    ActionCategory category;
    PCWSTR name, target;
};

constexpr EditorPreset kEditorPresets[] = {
    {ActionCategory::App, L"File Explorer", L"%WINDIR%\\explorer.exe"},
    {ActionCategory::App, L"Task Manager", L"%WINDIR%\\System32\\Taskmgr.exe"},
    {ActionCategory::App, L"Notepad", L"%WINDIR%\\System32\\notepad.exe"},
    {ActionCategory::App, L"Calculator", L"%WINDIR%\\System32\\calc.exe"},
    {ActionCategory::Folder, L"Home folder", L"%USERPROFILE%"},
    {ActionCategory::Folder, L"Desktop", L"shell:Desktop"},
    {ActionCategory::Folder, L"Documents", L"shell:Personal"},
    {ActionCategory::Folder, L"Downloads", L"shell:Downloads"},
    {ActionCategory::Folder, L"Pictures", L"shell:My Pictures"},
    {ActionCategory::Folder, L"Music", L"shell:My Music"},
    {ActionCategory::Folder, L"Videos", L"shell:My Video"},
    {ActionCategory::Folder, L"This PC", L"shell:MyComputerFolder"},
    {ActionCategory::Folder, L"Recycle Bin", L"shell:RecycleBinFolder"},
    {ActionCategory::Link, L"Google", L"https://www.google.com/"},
    {ActionCategory::Link, L"YouTube", L"https://www.youtube.com/"},
    {ActionCategory::Link, L"GitHub", L"https://github.com/"},
    {ActionCategory::Link, L"Wikipedia", L"https://www.wikipedia.org/"},
    {ActionCategory::Link, L"Windhawk", L"https://windhawk.net/"},
    {ActionCategory::File, L"Windows wallpaper", L"%WINDIR%\\Web\\Wallpaper\\Windows\\img0.jpg"},
    {ActionCategory::Settings, L"Settings home", L"ms-settings:"},
    {ActionCategory::Settings, L"Display", L"ms-settings:display"},
    {ActionCategory::Settings, L"Sound", L"ms-settings:sound"},
    {ActionCategory::Settings, L"Bluetooth & devices", L"ms-settings:bluetooth"},
    {ActionCategory::Settings, L"Network & internet", L"ms-settings:network"},
    {ActionCategory::Settings, L"Personalization", L"ms-settings:personalization"},
    {ActionCategory::Settings, L"Windows Update", L"ms-settings:windowsupdate"},
    {ActionCategory::Keys, L"Task view", L"Win+Tab"},
    {ActionCategory::Keys, L"Show desktop", L"Win+D"},
    {ActionCategory::Keys, L"Search", L"Win+S"},
    {ActionCategory::Keys, L"Quick settings", L"Win+A"},
    {ActionCategory::Keys, L"Screen capture", L"Win+Shift+S"},
    {ActionCategory::Keys, L"Emoji panel", L"Win+Period"},
    {ActionCategory::Keys, L"Clipboard history", L"Win+V"},
    {ActionCategory::Command, L"Command Prompt", L"%WINDIR%\\System32\\cmd.exe"},
    {ActionCategory::Command, L"Windows PowerShell", L"%WINDIR%\\System32\\WindowsPowerShell\\v1.0\\powershell.exe"},
};

struct LibraryIcon {
    PCWSTR name;
    wchar_t glyph;

    std::wstring Code() const {
        wchar_t code[11]{};
        swprintf_s(code, L"glyph:%04X", static_cast<unsigned>(glyph));
        return code;
    }
};

constexpr LibraryIcon kLibraryIcons[] = {
    {L"App", L'\uE7B8'},
    {L"Folder", L'\uE8B7'},
    {L"Open folder", L'\uE838'},
    {L"Link", L'\uE71B'},
    {L"Globe", L'\uE774'},
    {L"Home", L'\uE80F'},
    {L"Document", L'\uE8A5'},
    {L"Download", L'\uE896'},
    {L"Picture", L'\uEB9F'},
    {L"Music", L'\uE8D6'},
    {L"Video", L'\uE714'},
    {L"Display", L'\uE7F4'},
    {L"Settings", L'\uE713'},
    {L"Search", L'\uE721'},
    {L"Keyboard", L'\uE765'},
    {L"Task view", L'\uE7C4'},
    {L"Calculator", L'\uE8EF'},
    {L"Camera", L'\uE722'},
    {L"Sound", L'\uE767'},
    {L"Bluetooth", L'\uE702'},
    {L"Wi-Fi", L'\uE701'},
    {L"Clipboard", L'\uE77F'},
    {L"Lock", L'\uE72E'},
    {L"Star", L'\uE734'},
    {L"Heart", L'\uEB51'},
    {L"Mail", L'\uE715'},
    {L"Calendar", L'\uE787'},
    {L"Clock", L'\uE823'},
    {L"Trash", L'\uE74D'},
    {L"Power", L'\uE7E8'},
    {L"Game controller", L'\uE7FC'},
    {L"Code", L'\uE943'},
    {L"Cloud", L'\uE753'},
    {L"System activity", L'\uE9D9'},
    {L"Quick note", L'\uE70B'},
    {L"Screen snip", L'\uE8C8'},
    {L"Terminal", L'\uE756'},
    {L"Emoji", L'\uE899'},
    {L"Equalizer", L'\uE9E9'},
    {L"New window", L'\uE8A7'},
    {L"Personalization", L'\uE771'},
    {L"Devices", L'\uE772'},
    {L"Brightness", L'\uE706'},
    {L"Location pin", L'\uE707'},
    {L"Airplane", L'\uE709'},
    {L"Tablet", L'\uE70A'},
    {L"Phone", L'\uE717'},
    {L"Shopping bag", L'\uE719'},
    {L"People", L'\uE716'},
    {L"Map", L'\uE826'},
    {L"Chat", L'\uE901'},
    {L"Microphone", L'\uE720'},
    {L"Headphones", L'\uE7F6'},
    {L"Mute", L'\uE74F'},
    {L"Play", L'\uE768'},
    {L"Pause", L'\uE769'},
    {L"Stop", L'\uE71A'},
    {L"Next track", L'\uE893'},
    {L"Previous track", L'\uE892'},
    {L"Save", L'\uE74E'},
    {L"Upload", L'\uE898'},
    {L"Print", L'\uE749'},
    {L"Bookmark", L'\uE8A4'},
    {L"History", L'\uE81C'},
    {L"Refresh", L'\uE72C'},
    {L"Sync", L'\uE895'},
    {L"Pin", L'\uE718'},
    {L"Shield", L'\uEA18'},
    {L"Battery", L'\uE850'},
    {L"Network", L'\uE968'},
    {L"USB", L'\uE88E'},
    {L"Hard drive", L'\uEDA2'},
    {L"Remote desktop", L'\uE8AF'},
    {L"Accessibility", L'\uE776'},
    {L"Lightbulb", L'\uEA80'},
    {L"Help", L'\uE897'},
    {L"Information", L'\uE946'},
    {L"Timer", L'\uE916'},
    {L"Alarm", L'\uE7ED'},
    {L"Checkmark", L'\uE73E'},
    {L"Share", L'\uE72D'},
    {L"Archive", L'\uF12B'},
};

struct AccentSwatch {
    PCWSTR name;
    DWORD rgb;
};

constexpr AccentSwatch kAccentSwatches[] = {
    {L"Sky blue", 0x60CDFF},
    {L"Blue", 0x3578E5},
    {L"Turquoise", 0x26BDB0},
    {L"Green", 0x4CAF50},
    {L"Amber", 0xF2BC36},
    {L"Orange", 0xF47A42},
    {L"Red", 0xE5505A},
    {L"Pink", 0xE66CAA},
    {L"Purple", 0x9B77EE},
    {L"Slate", 0x7B8BA0},
};

class MenuEditor {
public:
    using Save = std::function<bool(Settings, std::wstring&)>;
    using Test = std::function<void()>;

    ~MenuEditor() {
        Close();
        if (registered_) {
            UnregisterClassW(kEditorWheelClass, instance_);
            UnregisterClassW(kEditorClass, instance_);
        }
    }

    void Close() {
        if (window_) DestroyWindow(window_);
    }

    void IconsReady() {
        if (wheel_) InvalidateRect(wheel_, nullptr, FALSE);
        if (HWND picker = Get(IconLibrary)) {
            InvalidateRect(picker, nullptr, FALSE);
            COMBOBOXINFO info{sizeof(info)};
            if (GetComboBoxInfo(picker, &info) && IsWindowVisible(info.hwndList))
                InvalidateRect(info.hwndList, nullptr, FALSE);
        }
    }

    bool Show(HINSTANCE instance, HANDLE stop, const Settings& settings,
              const std::wstring& profileRoot, Save save, Test test) {
        if (window_) {
            ShowWindow(window_, SW_RESTORE);
            SetForegroundWindow(window_);
            return true;
        }
        if (!Register(instance)) return false;
        stop_ = stop;
        draft_ = settings;
        baseline_ = settings;
        page_ = slot_ = 0;
        profileRoot_ = profileRoot;
        saved_ = std::move(save);
        test_ = std::move(test);
        POINT point{};
        GetCursorPos(&point);
        const HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
        MONITORINFO screen{sizeof(screen)};
        if (!GetMonitorInfoW(monitor, &screen)) return false;
        const RECT work = screen.rcWork;
        const int width = std::min(1180, static_cast<int>(work.right - work.left));
        const int height = std::min(860, static_cast<int>(work.bottom - work.top));
        HWND editor = CreateWindowExW(WS_EX_APPWINDOW | WS_EX_CONTROLPARENT, kEditorClass,
                                      L"Fluent Radial Menu Settings", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                      (work.left + work.right - width) / 2, (work.top + work.bottom - height) / 2,
                                      width, height, nullptr, nullptr, instance, this);
        if (!editor) return false;
        ShowWindow(editor, SW_SHOW);
        SetForegroundWindow(editor);
        return true;
    }

    HWND Window() const { return window_; }

private:
    enum Id {
        Pages = 100, AddPage, DuplicatePage, UpPage, DownPage, RemovePage, Undo,
        Tab, Type, Name, Target, Arguments, RenamePage = Arguments + 4,
        ThemeId, AccentId, Diameter, CenterDiameter, Transparency, Blur, Border, IconSize, Dividers,
        OuterLabels, CenterLabel, PageLabel, Idle, Animations, RespectMotion, Speed, IconScale,
        SelectionMs, Transition, Shortcut, Position, WheelSensitivity, Hold, CloseAfter,
        Apply, Desktop, Replay, Background, NavAction = Background + 2, NavAppearance, NavMotion,
        Preset, IconLibrary = Preset + 2, SlotHeading,
        RenameEdit, ConfirmRename, CancelRename, ImportProfile, ExportProfile,
    };
    struct Field {
        HWND label;
        HWND control;
        int height;
    };

    bool Register(HINSTANCE instance) {
        if (registered_) return true;
        instance_ = instance;
        WNDCLASSW windowClass{};
        windowClass.hInstance = instance;
        windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        windowClass.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
        windowClass.lpszClassName = kEditorClass;
        windowClass.lpfnWndProc = WindowProc;
        if (!RegisterClassW(&windowClass)) return false;
        windowClass.lpszClassName = kEditorWheelClass;
        windowClass.lpfnWndProc = WheelProc;
        windowClass.hbrBackground = nullptr;
        registered_ = RegisterClassW(&windowClass) != 0;
        if (!registered_) UnregisterClassW(kEditorClass, instance);
        return registered_;
    }

    static MenuEditor* Owner(HWND window, UINT msg, LPARAM lp) {
        if (msg == WM_NCCREATE) {
            auto* editor = static_cast<MenuEditor*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(editor));
            return editor;
        }
        return reinterpret_cast<MenuEditor*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    }

    static LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wp, LPARAM lp) {
        auto* self = Owner(window, msg, lp);
        return self ? self->Message(window, msg, wp, lp) : DefWindowProcW(window, msg, wp, lp);
    }

    static LRESULT CALLBACK WheelProc(HWND window, UINT msg, WPARAM wp, LPARAM lp) {
        auto* self = Owner(window, msg, lp);
        if (self && msg == WM_PAINT) {
            self->PaintWheel(window);
            return 0;
        }
        if (self && msg == WM_SIZE) self->ConfigureWheel(window);
        if (self && msg == WM_GETDLGCODE) return DLGC_WANTARROWS | DLGC_WANTCHARS;
        if (self && msg == WM_LBUTTONDOWN) {
            SetFocus(window);
            const int hit = self->geometry_.Hit({GET_X_LPARAM(lp), GET_Y_LPARAM(lp)});
            if (hit >= 0) {
                self->SelectSlot(hit);
                if (self->slot_ == hit && self->draft_.pages[self->page_][hit].Active()) {
                    self->dragSlot_ = hit;
                    self->dragStart_ = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                    SetCapture(window);
                }
            }
            return 0;
        }
        if (self && msg == WM_MOUSEMOVE && self->dragSlot_ >= 0) {
            const POINT point{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            if (self->drag_ ||
                std::abs(point.x - self->dragStart_.x) >= GetSystemMetricsForDpi(SM_CXDRAG, self->dpi_) ||
                std::abs(point.y - self->dragStart_.y) >= GetSystemMetricsForDpi(SM_CYDRAG, self->dpi_)) {
                self->state_.visibility.Snap(1);
                self->state_.scale.Snap(1);
                self->drag_ = PreviewDrag{self->dragSlot_, self->geometry_.Hit(point), point};
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }
        if (self && msg == WM_LBUTTONUP && self->dragSlot_ >= 0) {
            const int from = std::exchange(self->dragSlot_, -1);
            const bool dragged = self->drag_.has_value();
            self->drag_.reset();
            ReleaseCapture();
            InvalidateRect(window, nullptr, FALSE);
            if (!dragged) return 0;
            POINT point{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            const int to = self->geometry_.Hit(point);
            if (to >= 0 && to != from) self->SwapSlots(self->page_, to);
            else if (to == -2) {
                HWND pages = self->Get(Pages);
                MapWindowPoints(window, pages, &point, 1);
                RECT area{};
                GetClientRect(pages, &area);
                if (PtInRect(&area, point)) {
                    const auto item = SendMessageW(pages, LB_ITEMFROMPOINT, 0, MAKELPARAM(point.x, point.y));
                    const int page = LOWORD(item);
                    if (!HIWORD(item) && page != self->page_ && page < static_cast<int>(self->draft_.pages.size())) {
                        const auto& slots = self->draft_.pages[page];
                        const auto empty = std::find_if(slots.begin(), slots.end(), [](const Slot& s) { return !s.Active(); });
                        self->SwapSlots(page, empty == slots.end() ? from : static_cast<int>(empty - slots.begin()));
                    }
                }
            }
            return 0;
        }
        if (self && msg == WM_CAPTURECHANGED) {
            self->dragSlot_ = -1;
            if (self->drag_) {
                self->drag_.reset();
                InvalidateRect(window, nullptr, FALSE);
            }
        }
        if (self && msg == WM_DROPFILES) {
            self->DropFile(window, reinterpret_cast<HDROP>(wp));
            return 0;
        }
        if (self && (msg == WM_KEYDOWN || msg == WM_MOUSEWHEEL)) {
            if (self->dragSlot_ >= 0) {
                if (msg == WM_KEYDOWN && wp == VK_ESCAPE) ReleaseCapture();
                return 0;
            }
            if (msg == WM_MOUSEWHEEL || wp == VK_PRIOR || wp == VK_NEXT) {
                const int direction = msg == WM_MOUSEWHEEL
                    ? (GET_WHEEL_DELTA_WPARAM(wp) < 0 ? 1 : -1) : (wp == VK_NEXT ? 1 : -1);
                self->SelectPage(direction);
            } else if (wp == VK_UP || wp == VK_LEFT || wp == VK_DOWN || wp == VK_RIGHT) {
                self->SelectSlot((self->slot_ + (wp == VK_UP || wp == VK_LEFT ? kSlots - 1 : 1)) % kSlots);
            }
            return 0;
        }
        if (msg == WM_ERASEBKGND) return TRUE;
        return DefWindowProcW(window, msg, wp, lp);
    }

    LRESULT Message(HWND window, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
            case WM_CREATE:
                window_ = window;
                dpi_ = GetDpiForWindow(window);
                { INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_BAR_CLASSES};
                  InitCommonControlsEx(&controls); }
                ThemeEditor();
                UpdateFont();
                Build();
                return InitializePreview() ? 0 : -1;
            case WM_DPICHANGED: {
                dpi_ = HIWORD(wp);
                UpdateFont();
                const auto& bounds = *reinterpret_cast<const RECT*>(lp);
                SetWindowPos(window, nullptr, bounds.left, bounds.top, bounds.right - bounds.left,
                             bounds.bottom - bounds.top, SWP_NOZORDER | SWP_NOACTIVATE);
                Layout();
                return 0;
            }
            case WM_GETMINMAXINFO: {
                auto* limits = reinterpret_cast<MINMAXINFO*>(lp);
                const UINT dpi = GetDpiForWindow(window);
                limits->ptMinTrackSize = {MulDiv(960, dpi, 96), MulDiv(680, dpi, 96)};
                return 0;
            }
            case WM_SIZE:
                if (wheel_) Layout();
                return 0;
            case WM_TIMER:
                if (wp == kModalTimer) {
                    if (WaitForSingleObject(stop_, 0) == WAIT_OBJECT_0) {
                        EndMenu();
                        HWND popup = GetLastActivePopup(window);
                        if (popup != window) PostMessageW(popup, WM_CLOSE, 0, 0);
                    }
                } else {
                    if (!state_.Animating(Clock::now())) KillTimer(window, 1);
                    InvalidateRect(wheel_, nullptr, FALSE);
                }
                return 0;
            case kIconsReady:
                IconsReady();
                return 0;
            case kAppsReady:
                if (shell_) {
                    installedApps_ = shell_->TakeApps();
                    appsLoading_ = false;
                    if (Get(Preset) && GetFocus() != Get(Preset) &&
                        Selected(Type) == static_cast<int>(ActionCategory::App) &&
                        !SendMessageW(Get(Preset), CB_GETDROPPEDSTATE, 0, 0)) FillPresets();
                }
                return 0;
            case kEditorChoice:
                if (pendingChoice_ == reinterpret_cast<HWND>(lp)) pendingChoice_ = nullptr;
                if (!building_ && Get(static_cast<Id>(wp)) == reinterpret_cast<HWND>(lp)) {
                    if (wp == AccentId && tab_ == 1) ChangeAccent();
                    else if (tab_ == 0) ChangeActionChoice(static_cast<Id>(wp));
                }
                return 0;
            case WM_COMMAND:
                if (!building_ && lp) Command(LOWORD(wp), HIWORD(wp));
                return 0;
            case WM_HSCROLL:
                if (!building_ && lp) {
                    ReadFields();
                    RefreshPreview(false, false, tab_ == 1);
                    for (const auto& f : fields_) {
                        if (!f.label || !ClassIs(f.control,TRACKBAR_CLASSW)) continue;
                        auto label = WindowText(f.label);
                        label = label.substr(0, label.find(L"  ·  "));
                        label += L"  ·  " + std::to_wstring(SendMessageW(f.control, TBM_GETPOS, 0, 0));
                        SetWindowTextW(f.label, label.c_str());
                    }
                    Status(L"Draft changed. Select Apply when ready.");
                }
                return 0;
            case WM_PAINT:
                PaintEditor();
                return 0;
            case WM_ERASEBKGND: return TRUE;
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC:
            case WM_CTLCOLORLISTBOX: {
                HDC dc = reinterpret_cast<HDC>(wp);
                const HWND child = reinterpret_cast<HWND>(lp);
                DWORD color = GetParent(child) == form_ ? cardColor_ : panelColor_;
                if (msg == WM_CTLCOLOREDIT) color = fieldColor_;
                SetTextColor(dc, Rgb(textColor_));
                SetBkColor(dc, Rgb(color));
                SetDCBrushColor(dc, Rgb(color));
                return reinterpret_cast<LRESULT>(GetStockObject(DC_BRUSH));
            }
            case WM_MEASUREITEM: {
                auto* item = reinterpret_cast<MEASUREITEMSTRUCT*>(lp);
                item->itemHeight = Px(item->CtlID == Pages ? 40 : 32);
                return TRUE;
            }
            case WM_DRAWITEM: {
                auto* item = reinterpret_cast<DRAWITEMSTRUCT*>(lp);
                if (item->itemID == UINT(-1)) return TRUE;
                Canvas c(*this, item->hDC, item->rcItem);
                const float w = (item->rcItem.right-item->rcItem.left)*96.f/dpi_;
                const float h = (item->rcItem.bottom-item->rcItem.top)*96.f/dpi_;
                const bool selected = item->itemState & ODS_SELECTED;
                c.Box({0,0,w,h}, panelColor_, 0);
                c.Box({2,2,w-2,h-2}, selected ? strokeColor_ : panelColor_, 6);
                if (selected && item->CtlID == Pages) c.Box({3,12,6,h-12},accentColor_,2);
                const UINT lengthMessage = item->CtlID == Pages ? LB_GETTEXTLEN : CB_GETLBTEXTLEN;
                const UINT textMessage = item->CtlID == Pages ? LB_GETTEXT : CB_GETLBTEXT;
                std::wstring label(SendMessageW(item->hwndItem,lengthMessage,item->itemID,0)+1,L'\0');
                label.resize(SendMessageW(item->hwndItem,textMessage,item->itemID,reinterpret_cast<LPARAM>(label.data())));
                const float left = item->CtlID == IconLibrary || item->CtlID == AccentId ? 42.f : 14.f;
                if (item->CtlID == IconLibrary) PaintIconChoice(c, item->itemID, h);
                if (item->CtlID == AccentId) PaintAccentChoice(c, item->itemID, h);
                c.Text(label,{left,0,w-8,h},textColor_,13,selected);
                if (item->itemState & ODS_FOCUS) c.Box({2,2,w-2,h-2},accentColor_,6,true);
                return TRUE;
            }
            case WM_DROPFILES:
                DropFile(window, reinterpret_cast<HDROP>(wp));
                return 0;
            case WM_SETTINGCHANGE:
            case WM_THEMECHANGED:
            case WM_SYSCOLORCHANGE:
            case WM_DWMCOLORIZATIONCOLORCHANGED:
                ThemeEditor();
                ConfigureWheel(wheel_);
                return 0;
            case WM_CLOSE:
                DestroyWindow(window);
                return 0;
            case WM_DESTROY:
                KillTimer(window, 1);
                KillTimer(window, kModalTimer);
                if (shell_) shell_->Stop();
                shell_.reset();
                renderer_.ReleaseSurface();
                previewFrame_.Reset();
                drag_.reset();
                dragSlot_ = -1;
                geometry_ = {};
                return 0;
            case WM_NCDESTROY:
                window_ = wheel_ = nullptr;
                pendingChoice_ = nullptr;
                if (font_) DeleteObject(std::exchange(font_, nullptr));
                if (headingFont_) DeleteObject(std::exchange(headingFont_, nullptr));
                fields_.clear();
                undo_.clear();
                installedApps_.clear();
                appsLoading_ = true;
                return DefWindowProcW(window, msg, wp, lp);
        }
        return DefWindowProcW(window, msg, wp, lp);
    }

    int Px(int value) const { return MulDiv(value, dpi_, 96); }

    struct Canvas {
        MenuEditor& editor;
        ComPtr<ID2D1DCRenderTarget> target;
        ComPtr<ID2D1SolidColorBrush> brush;
        Canvas(MenuEditor& owner, HDC dc, RECT bounds) : editor(owner) {
            if (!editor.uiFactory_) D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, editor.uiFactory_.GetAddressOf());
            if (!editor.uiWrite_) DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                                       reinterpret_cast<IUnknown**>(editor.uiWrite_.GetAddressOf()));
            if (!editor.uiFactory_ || !editor.uiWrite_) return;
            auto properties = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
            if (FAILED(editor.uiFactory_->CreateDCRenderTarget(&properties, &target)) ||
                FAILED(target->BindDC(dc, &bounds)) ||
                FAILED(target->CreateSolidColorBrush(D2D1::ColorF(0), &brush))) { target.Reset(); return; }
            target->BeginDraw();
            target->SetTransform(D2D1::Matrix3x2F::Scale(editor.dpi_ / 96.f, editor.dpi_ / 96.f));
            target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        }
        ~Canvas() { if (target) target->EndDraw(); }
        void Box(D2D1_RECT_F r, DWORD color, float radius = 7, bool outline = false) {
            if (!target) return;
            brush->SetColor(D2D1::ColorF(color));
            const auto rounded = D2D1::RoundedRect(r, radius, radius);
            if (outline) target->DrawRoundedRectangle(rounded, brush.Get(), 1);
            else target->FillRoundedRectangle(rounded, brush.Get());
        }
        void Image(const IconPixels& pixels, D2D1_RECT_F area) {
            if (!target) return;
            ComPtr<ID2D1Bitmap> bitmap;
            const auto properties = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
            if (FAILED(target->CreateBitmap({pixels.width, pixels.height}, pixels.bgra.data(),
                                           pixels.width * 4, properties, &bitmap))) return;
            const float scale = std::min((area.right - area.left) / pixels.width,
                                         (area.bottom - area.top) / pixels.height) / 2;
            const float x = (area.left + area.right) / 2, y = (area.top + area.bottom) / 2;
            target->DrawBitmap(bitmap.Get(), {x - pixels.width * scale, y - pixels.height * scale,
                                              x + pixels.width * scale, y + pixels.height * scale});
        }
        void Text(std::wstring_view text, D2D1_RECT_F r, DWORD color, float size = 14, bool bold = false,
                  bool center = false, PCWSTR family = L"Segoe UI Variable") {
            if (!target) return;
            ComPtr<IDWriteTextFormat> format;
            if (FAILED(editor.uiWrite_->CreateTextFormat(family, nullptr,
                bold ? DWRITE_FONT_WEIGHT_SEMI_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"", &format))) return;
            format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            format->SetTextAlignment(center ? DWRITE_TEXT_ALIGNMENT_CENTER : DWRITE_TEXT_ALIGNMENT_LEADING);
            format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            brush->SetColor(D2D1::ColorF(color));
            target->DrawTextW(text.data(), static_cast<UINT32>(text.size()), format.Get(), r, brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    };
    DWORD panelColor_=0x202020, cardColor_=0x2b2b2b, fieldColor_=0x333333, textColor_=0xf5f5f5;
    DWORD mutedColor_=0xb5b5b5, strokeColor_=0x464646, accentColor_=0x60cdff;
    ComPtr<ID2D1Factory> uiFactory_;
    ComPtr<IDWriteFactory> uiWrite_;
    HWND form_=nullptr;
    bool inForm_=false, lightUi_=false;
    int scroll_=0, contentHeight_=0, scrollGrab_=-1;

    static DWORD Rgb(DWORD c) { return RGB((c>>16)&255,(c>>8)&255,c&255); }
    static std::wstring WindowText(HWND window) {
        std::wstring value(GetWindowTextLengthW(window)+1,L'\0');
        value.resize(GetWindowTextW(window,value.data(),static_cast<int>(value.size())));
        return value;
    }
    static bool ClassIs(HWND window, PCWSTR name) {
        wchar_t type[32]{};
        GetClassNameW(window,type,ARRAYSIZE(type));
        return _wcsicmp(type,name)==0;
    }
    void ThemeEditor() {
        DWORD light=1, size=sizeof(light);
        RegGetValueW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme",RRF_RT_REG_DWORD,nullptr,&light,&size);
        lightUi_=light!=0;
        panelColor_=light?0xf3f3f3:0x202020; cardColor_=light?0xffffff:0x2b2b2b;
        fieldColor_=light?0xf9f9f9:0x333333; textColor_=light?0x1a1a1a:0xf5f5f5;
        mutedColor_=light?0x606060:0xb5b5b5; strokeColor_=light?0xd6d6d6:0x464646;
        accentColor_=light?0x0067c0:0x60cdff;
        HIGHCONTRASTW hc{sizeof(hc)};
        if (SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(hc),&hc,0) && (hc.dwFlags&HCF_HIGHCONTRASTON)) {
            auto convert=[](int index) { const COLORREF c=GetSysColor(index);
                return DWORD(GetRValue(c)<<16|GetGValue(c)<<8|GetBValue(c)); };
            panelColor_=cardColor_=fieldColor_=convert(COLOR_WINDOW);
            textColor_=mutedColor_=strokeColor_=convert(COLOR_WINDOWTEXT);
            accentColor_=convert(COLOR_HIGHLIGHT);
        }
        const BOOL dark=!light;
        const DWORD backdrop=DWMSBT_MAINWINDOW, corners=DWMWCP_ROUND;
        DwmSetWindowAttribute(window_,DWMWA_USE_IMMERSIVE_DARK_MODE,&dark,sizeof(dark));
        DwmSetWindowAttribute(window_,DWMWA_SYSTEMBACKDROP_TYPE,&backdrop,sizeof(backdrop));
        DwmSetWindowAttribute(window_,DWMWA_WINDOW_CORNER_PREFERENCE,&corners,sizeof(corners));
        RedrawWindow(window_,nullptr,nullptr,RDW_INVALIDATE|RDW_ALLCHILDREN);
    }
    static LRESULT CALLBACK ControlProc(HWND window, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR data) {
        auto& self=*reinterpret_cast<MenuEditor*>(data);
        if (msg==WM_NCDESTROY) { RemoveWindowSubclass(window,ControlProc,1); return DefSubclassProc(window,msg,wp,lp); }
        if (GetDlgCtrlID(window) == RenameEdit && (wp == VK_RETURN || wp == VK_ESCAPE)) {
            if (msg == WM_GETDLGCODE && lp) return DLGC_WANTMESSAGE;
            if (msg == WM_KEYDOWN) {
                self.Command(wp == VK_RETURN ? ConfirmRename : CancelRename, BN_CLICKED);
                return 0;
            }
        }
        const bool button=ClassIs(window,L"Button"), combo=ClassIs(window,L"ComboBox"), slider=ClassIs(window,TRACKBAR_CLASSW);
        if (msg==WM_SETFOCUS && !self.building_ && GetParent(window)==self.form_) {
            RECT field{}, area{};
            GetWindowRect(window,&field); MapWindowPoints(nullptr,self.form_,reinterpret_cast<POINT*>(&field),2);
            GetClientRect(self.form_,&area);
            if(field.top<self.Px(28)) self.scroll_+=MulDiv(field.top,96,self.dpi_)-28;
            else if(field.bottom>area.bottom) self.scroll_+=MulDiv(field.bottom-area.bottom,96,self.dpi_)+8;
            self.LayoutFields();
        }
        if (msg==WM_MOUSEMOVE) {
            TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,window,0};
            TrackMouseEvent(&track); InvalidateRect(window,nullptr,FALSE);
        }
        if (msg==WM_MOUSELEAVE || msg==WM_SETFOCUS || msg==WM_KILLFOCUS || msg==BM_SETCHECK || msg==CB_SETCURSEL || msg==WM_ENABLE) {
            InvalidateRect(window,nullptr,FALSE);
            if (ClassIs(window,L"Edit")) InvalidateRect(GetParent(window),nullptr,FALSE);
        }
        if (msg==WM_PAINT && (button||combo||slider)) {
            PAINTSTRUCT paint{}; HDC dc=BeginPaint(window,&paint);
            RECT rect{}; GetClientRect(window,&rect);
            {
                Canvas c(self,dc,rect);
                const float w=rect.right*96.f/self.dpi_,h=rect.bottom*96.f/self.dpi_;
                POINT point{}; GetCursorPos(&point); ScreenToClient(window,&point);
                const bool hover=PtInRect(&rect,point), focus=GetFocus()==window;
                const int id=GetDlgCtrlID(window);
                const bool toggle=button && (GetWindowLongPtrW(window,GWL_STYLE)&BS_TYPEMASK)==BS_AUTOCHECKBOX;
                const bool nav=id>=NavAction&&id<=NavMotion, selected=nav&&self.tab_==id-NavAction;
                const DWORD bg=toggle?self.cardColor_:nav?self.panelColor_:self.fieldColor_;
                c.Box({0,0,w,h},GetParent(window)==self.form_?self.cardColor_:self.panelColor_,0);
                if (slider) {
                    const int low=SendMessageW(window,TBM_GETRANGEMIN,0,0), high=SendMessageW(window,TBM_GETRANGEMAX,0,0);
                    const int value=SendMessageW(window,TBM_GETPOS,0,0);
                    const float x=10+(w-20)*(value-low)/std::max(1,high-low);
                    c.Box({10,h/2-2,w-10,h/2+2},self.strokeColor_,2);
                    c.Box({10,h/2-2,x,h/2+2},self.accentColor_,2);
                    c.Box({x-8,h/2-8,x+8,h/2+8},self.cardColor_,8);
                    c.Box({x-5,h/2-5,x+5,h/2+5},self.accentColor_,5);
                } else if (toggle) {
                    const bool checked=SendMessageW(window,BM_GETCHECK,0,0)==BST_CHECKED;
                    c.Text(WindowText(window),{12,0,w-64,h},self.textColor_,13);
                    c.Box({w-54,h/2-10,w-14,h/2+10},checked?self.accentColor_:self.strokeColor_,10);
                    const float x=checked?w-24:w-44;
                    c.Box({x-6,h/2-6,x+6,h/2+6},checked&&!self.lightUi_?0x202020:0xffffff,6);
                } else {
                    const bool enabled = IsWindowEnabled(window);
                    const bool primary = id == Apply && enabled;
                    c.Box({1,1,w-1,h-1},primary?self.accentColor_:(hover&&enabled)||selected?self.strokeColor_:bg);
                    if (!nav&&!primary) c.Box({1,1,w-1,h-1},self.strokeColor_,7,true);
                    if (selected) c.Box({3,10,6,h-10},self.accentColor_,2);
                    std::wstring title=WindowText(window);
                    float textLeft = nav || combo ? 14.f : 6.f;
                    if (combo) {
                        const auto index=SendMessageW(window,CB_GETCURSEL,0,0);
                        if(index!=CB_ERR) {
                            title.resize(SendMessageW(window,CB_GETLBTEXTLEN,index,0)+1);
                            title.resize(SendMessageW(window,CB_GETLBTEXT,index,reinterpret_cast<LPARAM>(title.data())));
                            if (id == IconLibrary) {
                                self.PaintIconChoice(c, static_cast<int>(index), h);
                                textLeft = 42;
                            } else if (id == AccentId) {
                                self.PaintAccentChoice(c, static_cast<int>(index), h);
                                textLeft = 42;
                            }
                        }
                        c.Text(L"\u2304",{w-28,0,w-8,h},self.mutedColor_,17,false,true);
                    }
                    const DWORD fg=!IsWindowEnabled(window)?self.mutedColor_:primary?(self.lightUi_?0xffffff:0x102030):self.textColor_;
                    c.Text(title,{textLeft,0,w-(combo?32.f:6.f),h},fg,13,selected||primary,!nav&&!combo);
                }
                if(focus) c.Box({2,2,w-2,h-2},self.accentColor_,6,true);
            }
            EndPaint(window,&paint); return 0;
        }
        if (msg==WM_ERASEBKGND&&(button||combo||slider)) return TRUE;
        return DefSubclassProc(window,msg,wp,lp);
    }
    static LRESULT CALLBACK FormProc(HWND window,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR,DWORD_PTR data) {
        auto& self=*reinterpret_cast<MenuEditor*>(data);
        if(msg==WM_NCDESTROY) { RemoveWindowSubclass(window,FormProc,1); return DefSubclassProc(window,msg,wp,lp); }
        if(msg==WM_LBUTTONDOWN||msg==WM_MOUSEMOVE||msg==WM_LBUTTONUP||msg==WM_CAPTURECHANGED) {
            RECT area{}; GetClientRect(window,&area);
            const int height=MulDiv(area.bottom,96,self.dpi_);
            const int track=std::max(1,height-4);
            const int thumb=std::min(track,std::max(24,track*height/std::max(height,self.contentHeight_)));
            const int maximum=std::max(0,self.contentHeight_-height);
            const int y=MulDiv(GET_Y_LPARAM(lp),96,self.dpi_)-2;
            const int top=maximum?self.scroll_*(track-thumb)/maximum:0;
            if(msg==WM_LBUTTONDOWN&&GET_X_LPARAM(lp)>=area.right-self.Px(14)&&maximum) {
                self.scrollGrab_=y>=top&&y<top+thumb?y-top:thumb/2;
                SetCapture(window);
            }
            if((msg==WM_MOUSEMOVE||msg==WM_LBUTTONDOWN)&&self.scrollGrab_>=0) {
                self.scroll_=std::clamp((y-self.scrollGrab_)*maximum/std::max(1,track-thumb),0,maximum);
                self.LayoutFields(); return 0;
            }
            if(msg==WM_LBUTTONUP&&self.scrollGrab_>=0) { self.scrollGrab_=-1; ReleaseCapture(); return 0; }
            if(msg==WM_CAPTURECHANGED) self.scrollGrab_=-1;
        }
        if(msg==WM_COMMAND||msg==WM_HSCROLL||msg==WM_CTLCOLORSTATIC||msg==WM_CTLCOLOREDIT||
           msg==WM_CTLCOLORLISTBOX||msg==WM_DRAWITEM||msg==WM_MEASUREITEM)
            return SendMessageW(self.window_,msg,wp,lp);
        if(msg==WM_MOUSEWHEEL||msg==WM_VSCROLL) {
            RECT area{}; GetClientRect(window,&area);
            SCROLLINFO info{sizeof(info),SIF_ALL}; GetScrollInfo(window,SB_VERT,&info);
            info.nPage=MulDiv(area.bottom,96,self.dpi_);
            int next=self.scroll_;
            if(msg==WM_MOUSEWHEEL) next-=GET_WHEEL_DELTA_WPARAM(wp)/WHEEL_DELTA*75;
            else switch(LOWORD(wp)) {
                case SB_LINEUP: next-=30; break; case SB_LINEDOWN: next+=30; break;
                case SB_PAGEUP: next-=info.nPage; break; case SB_PAGEDOWN: next+=info.nPage; break;
                case SB_THUMBTRACK: next=info.nTrackPos; break;
            }
            self.scroll_=std::clamp(next,0,std::max(0,self.contentHeight_-static_cast<int>(info.nPage)));
            self.LayoutFields(); return 0;
        }
        if(msg==WM_PAINT) {
            PAINTSTRUCT paint{}; HDC dc=BeginPaint(window,&paint);
            RECT rect{}; GetClientRect(window,&rect);
            {
                Canvas c(self,dc,rect); const float width=rect.right*96.f/self.dpi_-14;
                c.Box({0,0,width+14,rect.bottom*96.f/self.dpi_},self.panelColor_,0);
                float y=-static_cast<float>(self.scroll_);
                for(const auto& f:self.fields_) {
                    const float height=f.label?76.f:44.f;
                    c.Box({0,y,width-3,y+height},self.cardColor_);
                    if(f.label && ClassIs(f.control,L"Edit")) {
                        RECT r{}; GetWindowRect(f.control,&r); MapWindowPoints(nullptr,window,reinterpret_cast<POINT*>(&r),2);
                        const auto bounds=D2D1::RectF(r.left*96.f/self.dpi_-7,r.top*96.f/self.dpi_-5,
                                                    r.right*96.f/self.dpi_+7,r.bottom*96.f/self.dpi_+5);
                        c.Box(bounds,self.fieldColor_,5);
                        c.Box(bounds,GetFocus()==f.control?self.accentColor_:self.strokeColor_,5,true);
                    }
                    y+=height+7;
                }
                const float height=rect.bottom*96.f/self.dpi_,track=height-4;
                if(self.contentHeight_>height) {
                    const float thumb=std::max(24.f,track*height/self.contentHeight_);
                    const float top=2+self.scroll_*(track-thumb)/(self.contentHeight_-height);
                    c.Box({width+5,top,width+9,top+thumb},self.mutedColor_,2);
                }
            }
            EndPaint(window,&paint); return 0;
        }
        if(msg==WM_ERASEBKGND) return TRUE;
        return DefSubclassProc(window,msg,wp,lp);
    }
    void LayoutFields() {
        if(!form_) return;
        RECT area{}; GetClientRect(form_,&area);
        const int width=MulDiv(area.right,96,dpi_)-14,height=MulDiv(area.bottom,96,dpi_);
        contentHeight_=0;
        for(const auto& f:fields_) contentHeight_+=(f.label?76:44)+7;
        scroll_=std::clamp(scroll_,0,std::max(0,contentHeight_-height));
        int y=-scroll_;
        for(const auto& f:fields_) {
            if(f.label) {
                Place(f.label,14,y+7,width-30,22);
                const bool edit=ClassIs(f.control,L"Edit");
                Place(f.control,edit?21:13,y+(edit?40:34),width-(edit?46:30),IsCombo(f.control)?260:edit?22:33);
            } else Place(f.control,5,y+3,width-13,38);
            y+=(f.label?76:44)+7;
        }
        InvalidateRect(form_,nullptr,FALSE);
    }
    void PaintEditor() {
        PAINTSTRUCT paint{}; HDC dc=BeginPaint(window_,&paint);
        RECT r{}; GetClientRect(window_,&r);
        {
            Canvas c(*this,dc,r);
            const float w=r.right*96.f/dpi_,h=r.bottom*96.f/dpi_;
            c.Box({0,0,w,h},panelColor_,0);
            c.Text(L"Fluent Radial Menu",{24,15,w-30,57},textColor_,26,true);
            c.Text(L"Customize your launcher",{25,57,w-30,82},mutedColor_,13);
            c.Text(tab_==0?L"Actions":tab_==1?L"Appearance":L"Motion & activation",
                   {w-350,96,w-24,127},textColor_,20,true);
            c.Text(L"Preview",{212,96,w-375,127},textColor_,20,true);
        }
        EndPaint(window_,&paint);
    }


    HWND Control(PCWSTR type, PCWSTR value, DWORD style, int id = 0, DWORD ex = 0) {
        HWND control = CreateWindowExW(ex, type, value, WS_CHILD | WS_VISIBLE | style,
                                      0, 0, 0, 0, inForm_ ? form_ : window_,
                                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), instance_, nullptr);
        SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(font_), FALSE);
        SetWindowSubclass(control, ControlProc, 1, reinterpret_cast<DWORD_PTR>(this));
        return control;
    }

    HWND Button(PCWSTR label, Id id) { return Control(L"BUTTON", label, WS_TABSTOP | BS_PUSHBUTTON, id); }
    HWND Get(Id id) const {
        HWND control = GetDlgItem(window_, id);
        return control ? control : GetDlgItem(form_, id);
    }

    static BOOL CALLBACK SetChildFont(HWND child, LPARAM data) {
        auto& self=*reinterpret_cast<MenuEditor*>(data);
        SendMessageW(child,WM_SETFONT,reinterpret_cast<WPARAM>(child==self.heading_?self.headingFont_:self.font_),TRUE);
        return TRUE;
    }
    void UpdateFont() {
        const auto old = font_, oldHeading = headingFont_;
        font_ = CreateFontW(-Px(14), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        headingFont_ = CreateFontW(-Px(25), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        EnumChildWindows(window_,SetChildFont,reinterpret_cast<LPARAM>(this));
        if (old) DeleteObject(old);
        if (oldHeading) DeleteObject(oldHeading);
    }

    void Build() {
        building_ = true;
        tab_ = sample_ = scroll_ = 0;
        form_ = CreateWindowExW(WS_EX_CONTROLPARENT,L"STATIC",L"",WS_CHILD|WS_VISIBLE|SS_NOTIFY|WS_CLIPCHILDREN,
                                0,0,0,0,window_,nullptr,instance_,nullptr);
        SetWindowSubclass(form_,FormProc,1,reinterpret_cast<DWORD_PTR>(this));
        heading_ = Control(L"STATIC", L"Radial menu settings", SS_LEFT);
        SendMessageW(heading_, WM_SETFONT, reinterpret_cast<WPARAM>(headingFont_), FALSE);
        pageHeading_ = Control(L"STATIC", L"Pages", SS_LEFT);
        status_ = Control(L"STATIC", L"", SS_LEFT);
        Control(L"LISTBOX", L"", WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT |
                LBS_OWNERDRAWFIXED | LBS_HASSTRINGS, Pages);
        Button(L"Add page", AddPage);
        Button(L"Duplicate", DuplicatePage);
        Button(L"Move up", UpPage);
        Button(L"Move down", DownPage);
        Button(L"Remove", RemovePage);
        Button(L"Undo", Undo);
        Button(L"Rename", RenamePage);
        HWND rename = Control(L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, RenameEdit, WS_EX_CLIENTEDGE);
        SendMessageW(rename, EM_SETLIMITTEXT, 100, 0);
        Button(L"Save name", ConfirmRename);
        Button(L"Cancel", CancelRename);
        ShowRename(false);
        Combo(Tab, {L"Action", L"Appearance", L"Motion and activation"}, 0);
        ShowWindow(Get(Tab),SW_HIDE);
        ShowWindow(heading_,SW_HIDE);
        Button(L"Actions",NavAction);
        Button(L"Appearance",NavAppearance);
        Button(L"Motion & activation",NavMotion);
        Combo(Background, {L"Blue", L"Warm", L"Neutral"}, 0);
        Button(L"Replay opening", Replay);
        Button(L"Test on desktop", Desktop);
        Button(L"Apply", Apply);
        Button(L"Import profile", ImportProfile);
        Button(L"Export profile", ExportProfile);
        wheel_ = CreateWindowExW(0, kEditorWheelClass, L"Wheel preview", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                                 0, 0, 0, 0, window_, nullptr, instance_, this);
        DragAcceptFiles(window_, TRUE);
        DragAcceptFiles(wheel_, TRUE);
        UpdatePages();
        BuildFields();
        UpdateApply();
        Status(profileRoot_.empty() ? L"Windhawk profile storage is unavailable."
                                   : L"Select a wedge to edit it. Drag an icon to move or swap it.");
        building_ = false;
        Layout();
    }

    HWND Combo(Id id, std::initializer_list<PCWSTR> options, int selected) {
        HWND box = Control(L"COMBOBOX", L"", WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST |
                           CBS_OWNERDRAWFIXED | CBS_HASSTRINGS, id);
        SendMessageW(box,CB_SETITEMHEIGHT,-1,Px(30));
        if (id == Preset || id == IconLibrary || id == AccentId)
            SendMessageW(box, CB_SETMINVISIBLE, 8, 0);
        for (PCWSTR item : options) SendMessageW(box, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item));
        SendMessageW(box, CB_SETCURSEL, selected, 0);
        return box;
    }

    void Edit(PCWSTR label, Id id, const std::wstring& value, int limit) {
        HWND field = Control(L"EDIT", value.c_str(), WS_TABSTOP | ES_AUTOHSCROLL, id);
        SendMessageW(field, EM_SETLIMITTEXT, limit, 0);
        fields_.push_back({Control(L"STATIC", label, SS_LEFT), field, 25});
    }

    void Number(PCWSTR label, Id id, int value) {
        int low=0, high=100;
        switch(id) {
            case Diameter: low=220; high=720; break;
            case CenterDiameter: low=88; high=draft_.diameter-96; break;
            case Transparency: high=55; break; case Blur: high=36; break;
            case Border: high=4; break; case IconSize: low=12; high=64; break;
            case Speed: low=25; high=300; break; case IconScale: low=100; high=150; break;
            case SelectionMs: low=80; high=600; break;
            case WheelSensitivity: low=40; high=360; break;
            default: break;
        }
        HWND slider=Control(TRACKBAR_CLASSW,label,WS_TABSTOP|TBS_NOTICKS,id);
        SendMessageW(slider,TBM_SETRANGE,FALSE,MAKELPARAM(low,high));
        SendMessageW(slider,TBM_SETPOS,TRUE,value);
        const auto title=std::wstring(label)+L"  ·  "+std::to_wstring(value);
        fields_.push_back({Control(L"STATIC",title.c_str(),SS_LEFT),slider,33});
    }

    void Choice(PCWSTR label, Id id, std::initializer_list<PCWSTR> options, int selected) {
        fields_.push_back({Control(L"STATIC", label, SS_LEFT), Combo(id, options, selected), 25});
    }

    void Flag(PCWSTR label, Id id, bool value) {
        HWND field = Control(L"BUTTON", label, WS_TABSTOP | BS_AUTOCHECKBOX, id);
        SendMessageW(field, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
        fields_.push_back({nullptr, field, 22});
    }

    Slot& SelectedSlot() { return draft_.pages[page_][slot_]; }

    ActionCategory SlotCategory(const Slot& slot) const {
        return slot.category == ActionCategory::Unspecified ? InferCategory(slot) : slot.category;
    }

    static int LibraryIndex(wchar_t glyph) {
        for (size_t i = 0; i < ARRAYSIZE(kLibraryIcons); ++i) {
            if (kLibraryIcons[i].glyph == glyph) return static_cast<int>(i);
        }
        return -1;
    }

    static constexpr int kAutomaticIcon = 0;
    static constexpr int kBrowseIcon = 1;
    static constexpr int kFirstLibraryIcon = 2;
    static constexpr int kCustomIcon = kFirstLibraryIcon + ARRAYSIZE(kLibraryIcons);

    static int IconSelection(const Slot& slot) {
        if (slot.icon.empty()) return kAutomaticIcon;
        const int library = LibraryIndex(slot.glyph);
        return library >= 0 ? kFirstLibraryIcon + library : kCustomIcon;
    }

    void PaintIconChoice(Canvas& canvas, int index, float height) {
        const auto& slot = SelectedSlot();
        wchar_t glyph = L'\uE7B8';
        if (index == kAutomaticIcon || index == kCustomIcon) {
            const auto key = index == kAutomaticIcon ? AutomaticIconKey(slot) : IconKey(slot);
            if (shell_) {
                if (auto pixels = shell_->FindIcon(key)) {
                    canvas.Image(*pixels, {12, height / 2 - 10, 34, height / 2 + 10});
                    return;
                }
            }
            if (index == kCustomIcon) glyph = slot.glyph ? slot.glyph : L'\uEB9F';
            else if (slot.action == Action::Shortcut) glyph = L'\uE765';
            else if (SlotCategory(slot) == ActionCategory::Link) glyph = L'\uE71B';
        } else if (index == kBrowseIcon) {
            glyph = L'\uE838';
        } else if (index >= kFirstLibraryIcon && index < kCustomIcon) {
            glyph = kLibraryIcons[index - kFirstLibraryIcon].glyph;
        }
        canvas.Text(std::wstring_view(&glyph, 1), {12, 0, 34, height}, textColor_, 17,
                    false, true, L"Segoe Fluent Icons");
    }

    void SyncPreset() {
        int selection = 0;
        const auto& target = SelectedSlot().target;
        for (size_t i = 0; i < presets_.size(); ++i) {
            if (_wcsicmp(target.c_str(), presets_[i].target.c_str()) == 0) {
                selection = static_cast<int>(i) + 1;
                break;
            }
        }
        SendMessageW(Get(Preset), CB_SETCURSEL, selection, 0);
    }

    void FillPresets() {
        HWND box = Get(Preset);
        if (!box) return;
        const auto category = SlotCategory(SelectedSlot());
        presets_.clear();
        for (const auto& preset : kEditorPresets) {
            if (preset.category == category) presets_.push_back({preset.name, preset.target});
        }
        if (category == ActionCategory::App)
            presets_.insert(presets_.end(), installedApps_.begin(), installedApps_.end());
        SendMessageW(box, CB_RESETCONTENT, 0, 0);
        SendMessageW(box, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Custom…"));
        for (const auto& preset : presets_)
            SendMessageW(box, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(preset.name.c_str()));
        if (category == ActionCategory::App && appsLoading_)
            SendMessageW(box, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Loading installed apps…"));
        SyncPreset();
    }

    void UpdateSlotHeading() {
        const auto& slot = SelectedSlot();
        const auto title = L"Slot " + std::to_wstring(slot_ + 1) + L" • " +
                           (slot.Label().empty() ? std::wstring(L"Empty") : slot.Label());
        SetWindowTextW(Get(SlotHeading), title.c_str());
    }

    void BuildActionFields() {
        const Slot& slot = SelectedSlot();
        const auto category = SlotCategory(slot);
        const auto& info = CategoryInfo(category);
        fields_.push_back({nullptr, Control(L"STATIC", L"", SS_LEFTNOWORDWRAP | SS_ENDELLIPSIS, SlotHeading), 26});
        UpdateSlotHeading();
        Choice(L"Action type", Type, {}, static_cast<int>(category));
        for (const auto& entry : kActionCategories)
            SendMessageW(Get(Type), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(entry.title));
        SendMessageW(Get(Type), CB_SETCURSEL, static_cast<int>(category), 0);
        if (category != ActionCategory::Empty) {
            Choice(info.picker, Preset, {}, 0);
            FillPresets();
            const int iconSelection = IconSelection(slot);
            Choice(L"Choose an icon", IconLibrary, {L"Automatic", L"Browse…"}, kAutomaticIcon);
            HWND icons = Get(IconLibrary);
            for (const auto& icon : kLibraryIcons)
                SendMessageW(icons, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(icon.name));
            if (iconSelection == kCustomIcon)
                SendMessageW(icons, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Current custom icon"));
            SendMessageW(icons, CB_SETCURSEL, iconSelection, 0);
            Edit(L"Display name", Name, slot.name, 200);
            Edit(info.targetLabel, Target, slot.target, 2048);
            if (category == ActionCategory::Command || category == ActionCategory::App || !slot.arguments.empty())
                Edit(L"Arguments", Arguments, slot.arguments, 4096);
        }
    }

    static constexpr int kCustomAccent = 1 + ARRAYSIZE(kAccentSwatches);

    void PaintAccentChoice(Canvas& canvas, int index, float height) {
        DWORD color = WindowsAccent();
        if (index > 0 && index < kCustomAccent) color = kAccentSwatches[index - 1].rgb;
        if (index == kCustomAccent) color = draft_.accent.value_or(color);
        const auto bounds = D2D1::RectF(13, height / 2 - 8, 31, height / 2 + 8);
        canvas.Box(bounds, color, 4);
        canvas.Box(bounds, strokeColor_, 4, true);
    }

    void SyncAccentChoice() {
        int selected = draft_.accent ? kCustomAccent : 0;
        for (int i = 0; draft_.accent && i < kCustomAccent - 1; ++i) {
            if (*draft_.accent == kAccentSwatches[i].rgb) {
                selected = i + 1;
                break;
            }
        }
        SendMessageW(Get(AccentId), CB_SETCURSEL, selected, 0);
    }

    void ChangeAccent() {
        const int selected = Selected(AccentId);
        auto accent = draft_.accent;
        if (selected == 0) {
            accent.reset();
        } else if (selected > 0 && selected < kCustomAccent) {
            accent = kAccentSwatches[selected - 1].rgb;
        } else if (selected == kCustomAccent) {
            SyncAccentChoice();
            const DWORD initial = draft_.accent.value_or(WindowsAccent());
            CHOOSECOLORW picker{sizeof(picker)};
            picker.hwndOwner = window_;
            picker.rgbResult = Rgb(initial);
            picker.lpCustColors = customAccentColors_.data();
            picker.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (!SetTimer(window_, kModalTimer, 100, nullptr)) return;
            const BOOL accepted = ChooseColorW(&picker);
            KillTimer(window_, kModalTimer);
            if (!accepted) return;
            accent = (static_cast<DWORD>(GetRValue(picker.rgbResult)) << 16) |
                     (static_cast<DWORD>(GetGValue(picker.rgbResult)) << 8) |
                     GetBValue(picker.rgbResult);
        }
        draft_.accent = accent;
        SyncAccentChoice();
        Status(L"Draft changed. Select Apply when ready.");
        RefreshPreview(false, false, true);
    }

    void BuildFields() {
        const bool prior = std::exchange(building_, true);
        inForm_=true;
        for (const auto& field : fields_) {
            if (field.label) DestroyWindow(field.label);
            DestroyWindow(field.control);
        }
        fields_.clear();
        if (tab_ == 0) {
            BuildActionFields();
        } else if (tab_ == 1) {
            Choice(L"Theme", ThemeId, {L"System", L"Light", L"Dark"}, static_cast<int>(draft_.theme));
            Choice(L"Accent color", AccentId, {L"Windows accent"}, 0);
            HWND colors = Get(AccentId);
            for (const auto& swatch : kAccentSwatches)
                SendMessageW(colors, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(swatch.name));
            SendMessageW(colors, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Custom color…"));
            SyncAccentChoice();
            Number(L"Wheel diameter", Diameter, draft_.diameter);
            Number(L"Center diameter", CenterDiameter, draft_.centerDiameter);
            Number(L"Transparency (%)", Transparency, draft_.transparency);
            Number(L"Blur", Blur, draft_.blur);
            Number(L"Border thickness", Border, draft_.border);
            Number(L"Icon size", IconSize, draft_.iconSize);
            Flag(L"Show dividers", Dividers, draft_.dividers);
            Flag(L"Show outer labels", OuterLabels, draft_.outerLabels);
            Flag(L"Show selected name", CenterLabel, draft_.centerLabel);
            Flag(L"Show page counter and dots", PageLabel, draft_.pageLabel);
            Edit(L"Idle center text", Idle, draft_.idleLabel, 200);
        } else {
            Edit(L"Profile shortcut", Shortcut, draft_.shortcutText, 100);
            Choice(L"Placement", Position, {L"At pointer", L"Center of monitor"}, draft_.centerOnMonitor);
            Flag(L"Hold to select", Hold, draft_.holdToSelect);
            Flag(L"Close after selection", CloseAfter, draft_.closeAfterSelection);
            Number(L"Scroll sensitivity", WheelSensitivity, draft_.wheelSensitivity);
            Flag(L"Animate the menu", Animations, draft_.animations);
            Flag(L"Respect Windows reduced motion", RespectMotion, draft_.respectMotion);
            Number(L"Animation speed (%)", Speed, draft_.speed);
            Number(L"Hover scale (%)", IconScale, draft_.iconScale);
            Number(L"Selection time (ms)", SelectionMs, draft_.selectionMs);
            Choice(L"Page transition", Transition, {L"Instant", L"Crossfade", L"Rotate"}, static_cast<int>(draft_.transition));
        }
        inForm_=false;
        building_ = prior;
        if (wheel_) LayoutFields();
    }

    void Place(HWND element, int x, int y, int width, int height) {
        SetWindowPos(element, nullptr, Px(x), Px(y), Px(std::max(width, 1)), Px(std::max(height, 1)),
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Layout() {
        RECT r{};
        GetClientRect(window_,&r);
        const int w=MulDiv(r.right,96,dpi_), h=MulDiv(r.bottom,96,dpi_);
        const int right=w-350, previewWidth=right-236;
        Place(Get(NavAction),16,104,176,40);
        Place(Get(NavAppearance),16,150,176,40);
        Place(Get(NavMotion),16,196,176,40);
        Place(pageHeading_,24,259,160,24);
        Place(Get(Pages),16,292,176,h-529);
        Place(Get(AddPage),16,h-228,84,32);
        Place(Get(DuplicatePage),108,h-228,84,32);
        Place(Get(UpPage),16,h-189,84,32);
        Place(Get(DownPage),108,h-189,84,32);
        Place(Get(RemovePage),16,h-150,84,32);
        Place(Get(Undo),108,h-150,84,32);
        Place(Get(RenamePage),16,h-111,176,32);
        Place(Get(RenameEdit),16,h-150,176,32);
        Place(Get(ConfirmRename),16,h-111,84,32);
        Place(Get(CancelRename),108,h-111,84,32);
        Place(Get(Background),212,142,110,250);
        Place(Get(Replay),330,142,previewWidth-118,32);
        Place(wheel_,212,190,previewWidth,h-266);
        Place(form_,right,142,326,h-218);
        LayoutFields();
        Place(status_,24,h-51,w-342,40);
        Place(Get(Desktop),w-296,h-52,162,36);
        Place(Get(Apply),w-122,h-52,98,36);
        Place(Get(ExportProfile),w-148,28,124,36);
        Place(Get(ImportProfile),w-280,28,124,36);
        InvalidateRect(window_,nullptr,FALSE);
    }
    static bool IsCombo(HWND window) {
        wchar_t type[20];
        return GetClassNameW(window, type, ARRAYSIZE(type)) && wcscmp(type, L"ComboBox") == 0;
    }

    std::wstring Text(Id id) const {
        HWND window = Get(id);
        std::wstring text(GetWindowTextLengthW(window) + 1, L'\0');
        text.resize(GetWindowTextW(window, text.data(), static_cast<int>(text.size())));
        return text;
    }

    int Selected(Id id) const { return static_cast<int>(SendMessageW(Get(id), CB_GETCURSEL, 0, 0)); }
    bool Checked(Id id) const { return SendMessageW(Get(id), BM_GETCHECK, 0, 0) == BST_CHECKED; }

    void ReadNumber(Id id, int& value, int low, int high, PCWSTR label) {
        if(ClassIs(Get(id),TRACKBAR_CLASSW)) {
            SendMessageW(Get(id),TBM_SETRANGEMAX,FALSE,high);
            value=std::clamp(static_cast<int>(SendMessageW(Get(id),TBM_GETPOS,0,0)),low,high);
            SendMessageW(Get(id),TBM_SETPOS,TRUE,value);
            return;
        }
        const auto number = ParseNumber(Text(id), 10);
        if (number && *number >= static_cast<unsigned>(low) && *number <= static_cast<unsigned>(high))
            value = static_cast<int>(*number);
        else if (error_.empty())
            error_ = std::wstring(label) + L" must be between " + std::to_wstring(low) + L" and " + std::to_wstring(high) + L".";
    }

    void ReadFields() {
        error_.clear();
        if (tab_ == 0) {
            auto& slot = SelectedSlot();
            if (Get(Name)) slot.name = Trim(Text(Name));
            if (Get(Target)) slot.target = Trim(Text(Target));
            if (Get(Arguments)) slot.arguments = Text(Arguments);
        } else if (tab_ == 1) {
            draft_.theme = static_cast<Theme>(Selected(ThemeId));
            ReadNumber(Diameter, draft_.diameter, 220, 720, L"Wheel diameter");
            ReadNumber(CenterDiameter, draft_.centerDiameter, 88, draft_.diameter - 96, L"Center diameter");
            draft_.centerDiameter = std::min(draft_.centerDiameter, draft_.diameter - 96);
            ReadNumber(Transparency, draft_.transparency, 0, 55, L"Transparency");
            ReadNumber(Blur, draft_.blur, 0, 36, L"Blur");
            ReadNumber(Border, draft_.border, 0, 4, L"Border");
            ReadNumber(IconSize, draft_.iconSize, 12, 64, L"Icon size");
            draft_.dividers = Checked(Dividers);
            draft_.outerLabels = Checked(OuterLabels);
            draft_.centerLabel = Checked(CenterLabel);
            draft_.pageLabel = Checked(PageLabel);
            draft_.idleLabel = Text(Idle);
        } else {
            draft_.shortcutText = Trim(Text(Shortcut));
            draft_.shortcut = ParseChord(draft_.shortcutText);
            if (!draft_.shortcut) error_ = L"Enter a valid shortcut, such as Ctrl+Alt+Space.";
            draft_.centerOnMonitor = Selected(Position) == 1;
            draft_.holdToSelect = Checked(Hold);
            draft_.closeAfterSelection = Checked(CloseAfter);
            draft_.animations = Checked(Animations);
            draft_.respectMotion = Checked(RespectMotion);
            draft_.transition = static_cast<PageTransition>(Selected(Transition));
            ReadNumber(WheelSensitivity, draft_.wheelSensitivity, 40, 360, L"Scroll units");
            ReadNumber(Speed, draft_.speed, 25, 300, L"Animation speed");
            ReadNumber(IconScale, draft_.iconScale, 100, 150, L"Hover scale");
            ReadNumber(SelectionMs, draft_.selectionMs, 80, 600, L"Selection duration");
        }
    }

    void UpdatePages() {
        HWND list = Get(Pages);
        SendMessageW(list, LB_RESETCONTENT, 0, 0);
        for (const auto& name : draft_.pageNames) SendMessageW(list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(name.c_str()));
        SendMessageW(list, LB_SETCURSEL, page_, 0);
    }

    void UpdatePageName() {
        HWND list = Get(Pages);
        SendMessageW(list, LB_DELETESTRING, page_, 0);
        SendMessageW(list, LB_INSERTSTRING, page_,
                     reinterpret_cast<LPARAM>(draft_.pageNames[page_].c_str()));
        SendMessageW(list, LB_SETCURSEL, page_, 0);
    }

    void ShowRename(bool show) {
        renamingPage_ = show;
        for (Id id : {RenameEdit, ConfirmRename, CancelRename})
            ShowWindow(Get(id), show ? SW_SHOW : SW_HIDE);
        for (Id id : {RenamePage, RemovePage, Undo})
            ShowWindow(Get(id), show ? SW_HIDE : SW_SHOW);
        UpdateApply();
    }

    void BeginRename() {
        if (!renamingPage_) {
            SetWindowTextW(Get(RenameEdit), draft_.pageNames[page_].c_str());
            ShowRename(true);
        }
        SetFocus(Get(RenameEdit));
        SendMessageW(Get(RenameEdit), EM_SETSEL, 0, -1);
    }

    bool FinishRename() {
        if (!renamingPage_) return true;
        const auto name = Trim(Text(RenameEdit));
        if (name.empty()) {
            Status(L"Enter a page name.");
            SetFocus(Get(RenameEdit));
            return false;
        }
        if (name != draft_.pageNames[page_]) {
            Remember();
            draft_.pageNames[page_] = name;
            UpdatePageName();
            Status(L"Draft changed. Select Apply when ready.");
        }
        ShowRename(false);
        return true;
    }

    void SelectPage(int direction) {
        const int count = static_cast<int>(draft_.pages.size());
        ChangeSelection((page_ + count + direction) % count, slot_);
    }

    void SelectSlot(int slot) { ChangeSelection(page_, slot); }

    void ChangeSelection(int page, int slot) {
        ReadFields();
        if (!error_.empty() || (page != page_ && !FinishRename())) {
            if (!error_.empty()) Status(error_);
            SendMessageW(Get(Pages), LB_SETCURSEL, page_, 0);
            return;
        }
        if (page == page_ && slot == slot_) return;
        page_ = page;
        slot_ = slot;
        SendMessageW(Get(Pages), LB_SETCURSEL, page_, 0);
        if (tab_ == 0) BuildFields();
        RefreshPreview(true);
    }

    void Remember() {
        if (undo_.size() == 16) undo_.pop_front();
        undo_.push_back({draft_, page_, slot_});
    }

    void PageCommand(Id id) {
        if (id == Undo) {
            if (undo_.empty()) return;
            auto& change = undo_.back();
            // Ordinary action edits only undo their layout changes. Import
            // also changes the appearance and activation settings.
            if (change.wasImport) {
                draft_ = std::move(change.settings);
            } else {
                draft_.pages = std::move(change.settings.pages);
                draft_.pageNames = std::move(change.settings.pageNames);
            }
            page_ = change.page;
            slot_ = change.slot;
            undo_.pop_back();
        } else if (id == AddPage || id == DuplicatePage) {
            if (draft_.pages.size() == kMaxPages) { Status(L"The menu supports up to eight pages."); return; }
            Remember();
            auto slots = id == DuplicatePage ? draft_.pages[page_] : std::array<Slot, kSlots>{};
            auto name = id == DuplicatePage ? draft_.pageNames[page_].substr(0, 93) + L" (copy)"
                                           : L"Page " + std::to_wstring(draft_.pages.size() + 1);
            ++page_;
            draft_.pages.insert(draft_.pages.begin() + page_, std::move(slots));
            draft_.pageNames.insert(draft_.pageNames.begin() + page_, std::move(name));
        } else if (id == RemovePage) {
            if (draft_.pages.size() == 1) { Status(L"Keep at least one page."); return; }
            if (std::any_of(draft_.pages[page_].begin(), draft_.pages[page_].end(), [](const Slot& s) { return s.Active(); })) {
                if (!SetTimer(window_, kModalTimer, 100, nullptr)) return;
                const int answer = MessageBoxW(window_, L"Remove this page and its actions? You can undo this change.",
                                               L"Remove page", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONWARNING);
                KillTimer(window_, kModalTimer);
                if (answer != IDOK) return;
            }
            Remember();
            draft_.pages.erase(draft_.pages.begin() + page_);
            draft_.pageNames.erase(draft_.pageNames.begin() + page_);
            page_ = std::min(page_, static_cast<int>(draft_.pages.size()) - 1);
        } else {
            const int next = page_ + (id == UpPage ? -1 : 1);
            if (next < 0 || next >= static_cast<int>(draft_.pages.size())) return;
            Remember();
            std::swap(draft_.pages[page_], draft_.pages[next]);
            std::swap(draft_.pageNames[page_], draft_.pageNames[next]);
            page_ = next;
        }
        UpdatePages();
        BuildFields();
        RefreshPreview(true, false, id == Undo);
        Status(draft_ == baseline_ ? L"No unapplied changes." : L"Draft changed. Select Apply when ready.");
    }

    std::wstring PickFile(REFCLSID clsid, PCWSTR title, DWORD flags,
                          const COMDLG_FILTERSPEC* filters = nullptr, UINT count = 0) {
        ComPtr<IFileDialog> dialog;
        FILEOPENDIALOGOPTIONS options;
        if (FAILED(CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog))) ||
            FAILED(dialog->GetOptions(&options)) ||
            FAILED(dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_DONTADDTORECENT |
                                      FOS_NODEREFERENCELINKS | flags))) {
            Status(L"Could not open the file picker.");
            return {};
        }
        if (count) dialog->SetFileTypes(count, filters);
        if (clsid == CLSID_FileSaveDialog) {
            dialog->SetDefaultExtension(L"json");
            dialog->SetFileName(L"Fluent Radial Menu.json");
        }
        dialog->SetTitle(title);
        if (!SetTimer(window_, kModalTimer, 100, nullptr)) return {};
        const HRESULT result = dialog->Show(window_);
        KillTimer(window_, kModalTimer);
        if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED)) return {};
        ComPtr<IShellItem> item;
        PWSTR path = nullptr;
        if (FAILED(result) || FAILED(dialog->GetResult(&item)) ||
            FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
            Status(L"Could not get the selected path.");
            return {};
        }
        std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> selected(path, CoTaskMemFree);
        return selected.get();
    }

    static bool Browsable(ActionCategory category) {
        return category == ActionCategory::App || category == ActionCategory::Folder ||
               category == ActionCategory::File || category == ActionCategory::Command;
    }

    void BrowseFile(bool icon) {
        const auto category = SlotCategory(SelectedSlot());
        const bool folder = !icon && category == ActionCategory::Folder;
        static constexpr COMDLG_FILTERSPEC iconFilters[] = {
            {L"Images and icons", L"*.png;*.jpg;*.jpeg;*.bmp;*.ico;*.dll;*.exe"},
            {L"All files", L"*.*"},
        };
        static constexpr COMDLG_FILTERSPEC appFilters[] = {
            {L"Applications and shortcuts", L"*.exe;*.lnk;*.msc"},
            {L"All files", L"*.*"},
        };
        const bool executable = category == ActionCategory::App || category == ActionCategory::Command;
        auto path = PickFile(CLSID_FileOpenDialog,
            icon ? L"Choose an icon file" : folder ? L"Choose a folder" :
            executable ? L"Choose an application" : L"Choose a file",
            folder ? FOS_PICKFOLDERS : FOS_FILEMUSTEXIST,
            icon ? iconFilters : executable ? appFilters : nullptr,
            icon ? ARRAYSIZE(iconFilters) : executable ? ARRAYSIZE(appFilters) : 0);
        if (path.empty()) return;
        ReplaceSystemDirectory(path, L"Sysnative", L"System32");
        if (path.size() > (icon ? 1024u : 2048u)) {
            Status(L"The selected path is too long.");
            return;
        }
        if (icon) {
            if (SelectedSlot().icon == path) return;
            Remember();
            SetIcon(SelectedSlot(), path);
            RebuildActionFields(IconLibrary);
        } else {
            SetTarget(path.c_str(), slot_, category);
            SetFocus(Get(Preset));
        }
    }

    static void SetIcon(Slot& slot, std::wstring icon) {
        if (slot.icon == icon) return;
        slot.icon = std::move(icon);
        slot.glyph = 0;
        ResolveGlyph(slot);
    }

    void RebuildActionFields(Id focus) {
        BuildFields();
        SetFocus(Get(focus));
        Status(L"Draft changed. Select Apply when ready.");
        RefreshPreview(true);
    }

    void ChangeActionChoice(Id id) {
        ReadFields();
        auto& slot = SelectedSlot();
        const int selection = Selected(id);
        if (selection < 0) return;
        if (id == Type && static_cast<size_t>(selection) < ARRAYSIZE(kActionCategories)) {
            const auto category = static_cast<ActionCategory>(selection);
            if (category == SlotCategory(slot)) return;
            Remember();
            slot = {};
            slot.category = category;
            slot.action = CategoryInfo(category).action;
            RebuildActionFields(Type);
        } else if (id == Preset) {
            if (selection == 0) {
                if (Browsable(SlotCategory(slot))) {
                    SyncPreset();
                    BrowseFile(false);
                } else {
                    SetFocus(Get(Target));
                    SendMessageW(Get(Target), EM_SETSEL, 0, -1);
                }
            } else if (static_cast<size_t>(selection) <= presets_.size()) {
                const auto& preset = presets_[selection - 1];
                if (_wcsicmp(preset.target.c_str(), slot.target.c_str()) == 0) return;
                Remember();
                slot.category = SlotCategory(slot);
                slot.name = preset.name;
                slot.target = preset.target;
                slot.arguments.clear();
                SetIcon(slot, {});
                RebuildActionFields(Preset);
            } else {
                SyncPreset();
            }
        } else if (id == IconLibrary && selection != IconSelection(slot)) {
            if (selection == kBrowseIcon) {
                SendMessageW(Get(IconLibrary), CB_SETCURSEL, IconSelection(slot), 0);
                BrowseFile(true);
            } else if (selection < kCustomIcon) {
                const bool custom = IconSelection(slot) == kCustomIcon;
                Remember();
                SetIcon(slot, selection == kAutomaticIcon ? std::wstring{} :
                        kLibraryIcons[selection - kFirstLibraryIcon].Code());
                if (custom) RebuildActionFields(IconLibrary);
                else {
                    Status(L"Draft changed. Select Apply when ready.");
                    RefreshPreview(true);
                }
            }
        }
    }

    void SetTarget(PCWSTR path, int slot, ActionCategory category = ActionCategory::Unspecified) {
        if (draft_.pages[page_][slot].target == path) return;
        Remember();
        slot_ = slot;
        auto& action = draft_.pages[page_][slot_];
        const bool empty = !action.Active();
        const bool picked = category != ActionCategory::Unspecified;
        const bool command = action.action == Action::Command;
        action.target = path;
        if (category == ActionCategory::Unspecified) {
            const DWORD attributes = GetFileAttributesW(path);
            if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY))
                category = ActionCategory::Folder;
            else {
                action.action = command ? Action::Command : Action::Open;
                category = InferCategory(action);
            }
        }
        action.category = category;
        action.action = CategoryInfo(category).action;
        if (category == ActionCategory::App) SetIcon(action, {});
        if (picked || empty || action.name.empty()) {
            const auto separator = action.target.find_last_of(L"\\/");
            action.name = action.target.substr(separator == std::wstring::npos ? 0 : separator + 1, 200);
        }
        if (picked) action.arguments.clear();
        if (tab_ == 0) BuildFields();
        Status(L"Draft changed. Select Apply when ready.");
        RefreshPreview(true);
    }

    void DropFile(HWND source, HDROP drop) {
        const UINT length = DragQueryFileW(drop, 0, nullptr, 0);
        std::wstring path(length + 1, L'\0');
        POINT point{};
        DragQueryPoint(drop, &point);
        MapWindowPoints(source, wheel_, &point, 1);
        const int hit = geometry_.Hit(point);
        if (length && length <= 2048 && hit >= 0 &&
            DragQueryFileW(drop, 0, path.data(), length + 1)) {
            ReadFields();
            if (error_.empty()) SetTarget(path.c_str(), hit);
            else Status(error_);
        }
        DragFinish(drop);
    }

    void TransferProfile(bool exporting) {
        ReadFields();
        if (!error_.empty()) {
            Status(error_);
            return;
        }
        constexpr COMDLG_FILTERSPEC filters[] = {{L"Radial menu profile", L"*.json"}};
        if (exporting) {
            if (!FinishRename()) return;
            std::wstring json, error;
            AppendProfile(json, draft_);
            if (!ParseProfile(json, error)) {
                Status(error);
                return;
            }
            const auto path = PickFile(CLSID_FileSaveDialog, L"Export radial menu profile",
                                       FOS_OVERWRITEPROMPT, filters, ARRAYSIZE(filters));
            if (path.empty()) return;
            if (!profileRoot_.empty() &&
                _wcsicmp(path.c_str(), (profileRoot_ + kProfileFile).c_str()) == 0) {
                Status(L"Choose a file other than the active profile.");
                return;
            }
            Status(SaveProfileBytes(path, ToUtf8(json)) ? L"Profile exported."
                                                       : L"Could not export the profile.");
            return;
        }

        const auto path = PickFile(CLSID_FileOpenDialog, L"Import radial menu profile",
                                   FOS_FILEMUSTEXIST, filters, ARRAYSIZE(filters));
        if (path.empty()) return;
        std::wstring error;
        auto json = FromUtf8(ReadProfileBytes(path, error));
        if (!error.empty() || json.empty()) {
            Status(error.empty() ? L"The profile is not valid UTF-8." : error);
            return;
        }
        if (json.starts_with(L'\xfeff')) json.erase(0, 1);
        auto imported = ParseProfile(json, error);
        if (!imported) {
            Status(error);
            return;
        }
        if (!FinishRename()) return;
        imported->name = draft_.name;
        imported->revision = draft_.revision;
        if (*imported == draft_) {
            Status(L"This profile matches the current layout.");
            return;
        }
        Remember();
        undo_.back().wasImport = true;
        draft_ = std::move(*imported);
        page_ = slot_ = tab_ = scroll_ = 0;
        state_ = {};
        UpdatePages();
        BuildFields();
        RefreshPreview(true, false, true);
        RedrawWindow(window_, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);
        Status(draft_ == baseline_ ? L"Profile loaded. It matches the applied layout."
                                  : L"Profile loaded. Review the actions, then select Apply.");
    }

    void SwapSlots(int toPage, int toSlot) {
        if (toPage != page_ && !FinishRename()) return;
        Remember();
        std::swap(draft_.pages[page_][slot_], draft_.pages[toPage][toSlot]);
        page_ = toPage;
        slot_ = toSlot;
        UpdatePages();
        BuildFields();
        Status(L"Draft changed. Select Apply when ready.");
        RefreshPreview(true);
    }

    void Command(int id, int notification) {
        if (id == Type || id == Preset || id == IconLibrary || id == AccentId) {
            HWND box = Get(static_cast<Id>(id));
            if (notification == CBN_DROPDOWN && id == Preset && !appsLoading_ &&
                Selected(Type) == static_cast<int>(ActionCategory::App) &&
                SendMessageW(box, CB_GETCOUNT, 0, 0) != static_cast<LRESULT>(presets_.size() + 1)) {
                FillPresets();
            } else if (notification == CBN_SELENDOK ||
                       (notification == CBN_SELCHANGE && !SendMessageW(box, CB_GETDROPPEDSTATE, 0, 0))) {
                // Some choices recreate the combo box. Let it finish processing
                // the selection before changing the form. A commit can report
                // both SELENDOK and SELCHANGE.
                if (box && pendingChoice_ != box &&
                    PostMessageW(window_, kEditorChoice, id, reinterpret_cast<LPARAM>(box))) {
                    pendingChoice_ = box;
                }
            }
            return;
        }
        if(id>=NavAction && id<=NavMotion && notification==BN_CLICKED) {
            ReadFields();
            if(!error_.empty()) { Status(error_); return; }
            tab_=id-NavAction;
            scroll_=0;
            BuildFields();
            RedrawWindow(window_,nullptr,nullptr,RDW_INVALIDATE|RDW_ALLCHILDREN);
            return;
        }
        if (id == Pages && notification == LBN_SELCHANGE) {
            const auto index = SendMessageW(Get(Pages), LB_GETCURSEL, 0, 0);
            if (index != LB_ERR) ChangeSelection(static_cast<int>(index), slot_);
        } else if ((id == Pages && notification == LBN_DBLCLK) ||
                   (id == RenamePage && notification == BN_CLICKED)) {
            BeginRename();
        } else if (id == ConfirmRename && notification == BN_CLICKED) {
            if (FinishRename()) SetFocus(Get(Pages));
        } else if (id == CancelRename && notification == BN_CLICKED) {
            ShowRename(false);
            SetFocus(Get(Pages));
        } else if (id == Tab && notification == CBN_SELCHANGE) {
            ReadFields();
            if (!error_.empty()) {
                Status(error_);
                SendMessageW(Get(Tab), CB_SETCURSEL, tab_, 0);
                return;
            }
            tab_ = Selected(Tab);
            BuildFields();
        } else if (id == Background && notification == CBN_SELCHANGE) {
            sample_ = Selected(Background);
            ConfigureWheel(wheel_);
        } else if (id == Desktop && notification == BN_CLICKED) {
            test_();
        } else if (id == Replay && notification == BN_CLICKED) {
            RefreshPreview(false, true);
        } else if ((id == ImportProfile || id == ExportProfile) && notification == BN_CLICKED) {
            TransferProfile(id == ExportProfile);
        } else if (id == RenameEdit && notification == EN_CHANGE) {
            UpdateApply();
        } else if (id == Apply && notification == BN_CLICKED && !profileRoot_.empty()) {
            if (!FinishRename()) return;
            ReadFields();
            if (draft_ == baseline_) return;
            if (error_.empty() && tab_ == 0 && SelectedSlot().action != Action::Empty &&
                SelectedSlot().target.empty()) error_ = L"Choose a preset or enter a target for this action.";
            if (error_.empty() && saved_(draft_, error_)) baseline_ = draft_;
            Status(error_.empty() ? L"Applied to the desktop menu." : error_);
        } else if (id >= AddPage && id <= Undo && notification == BN_CLICKED) {
            if (!FinishRename()) return;
            ReadFields();
            if (!error_.empty()) Status(error_);
            else PageCommand(static_cast<Id>(id));
        } else if (id >= Type && id < Apply) {
            if (notification != EN_CHANGE && notification != CBN_SELCHANGE && notification != BN_CLICKED) return;
            ReadFields();
            if (id == Target || id == Name) UpdateSlotHeading();
            if (id == Target) SyncPreset();
            Status(error_.empty() ? L"Draft changed. Select Apply when ready." : error_);
            RefreshPreview(id == Target, false, tab_ == 1);
        }
    }

    void Status(std::wstring_view value) {
        SetWindowTextW(status_, std::wstring(value).c_str());
        UpdateApply();
    }

    void UpdateApply() {
        HWND apply = Get(Apply);
        if (!apply) return;
        const bool nameChanged = renamingPage_ && Trim(Text(RenameEdit)) != draft_.pageNames[page_];
        const bool enabled = !profileRoot_.empty() && (draft_ != baseline_ || nameChanged);
        if (static_cast<bool>(IsWindowEnabled(apply)) != enabled) {
            EnableWindow(apply, enabled);
            InvalidateRect(apply, nullptr, FALSE);
        }
    }

    bool InitializePreview() {
        if (!rendererReady_) rendererReady_ = renderer_.Init();
        if (!rendererReady_) return false;
        shell_ = std::make_shared<ShellWorker>(window_);
        auto owner = std::make_unique<std::shared_ptr<ShellWorker>>(shell_);
        HANDLE thread = CreateThread(nullptr, 0, ShellWorker::Run, owner.get(), 0, nullptr);
        if (!thread) return false;
        owner.release();
        CloseHandle(thread);
        shell_->RequestApps();
        RefreshPreview(true);
        return true;
    }

    static IconPixels SampleBackdrop(int sample, int blur) {
        constexpr int size = 72;
        constexpr std::array<std::array<BYTE, 6>, 3> samples{{
            {{92, 46, 20, 155, 128, 66}}, {{95, 42, 86, 107, 139, 200}}, {{66, 64, 62, 145, 142, 140}},
        }};
        const auto& colors = samples[std::clamp(sample, 0, 2)];
        IconPixels result{size, size, std::vector<BYTE>(size * size * 4)};
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                const float weight = std::clamp((x + y / 2 - 15) / 70.0f + ((x / 10 + y / 10) % 2) * .17f,
                                                0.0f, 1.0f);
                auto* pixel = &result.bgra[(y * size + x) * 4];
                for (int channel = 0; channel < 3; ++channel)
                    pixel[channel] = static_cast<BYTE>(std::lerp(colors[channel], colors[channel + 3], weight));
                pixel[3] = 255;
            }
        }
        if (blur) {
            auto interim = result.bgra;
            BlurPass(result.bgra.data(), interim.data(), size, std::max(1, blur / 3), false);
            BlurPass(interim.data(), result.bgra.data(), size, std::max(1, blur / 3), true);
        }
        return result;
    }

    void ConfigureWheel(HWND wheel) {
        if (!wheel || !rendererReady_) return;
        RECT area{};
        GetClientRect(wheel, &area);
        auto& g = geometry_;
        g.pixelsPerDip = dpi_ / 96.0f;
        const float extent = std::min(area.right, area.bottom) / g.pixelsPerDip;
        if (extent <= 2 * kShadow + 12) return;
        const float diameter = std::min(static_cast<float>(draft_.diameter), extent - 2 * kShadow);
        g.radius = diameter / 2;
        g.inner = g.radius * draft_.centerDiameter / draft_.diameter;
        g.extent = static_cast<int>(std::ceil((diameter + 2 * kShadow) * g.pixelsPerDip));
        g.origin = {(area.right - g.extent) / 2, (area.bottom - g.extent) / 2};
        const auto palette = ReadPalette(draft_);
        highContrast_ = palette.highContrast;
        renderer_.Configure(g, palette, highContrast_ ? IconPixels{} : SampleBackdrop(sample_, draft_.blur));
        InvalidateRect(wheel, nullptr, FALSE);
    }

    void RefreshPreview(bool icons, bool replay = false, bool appearanceChanged = false) {
        UpdateApply();
        if (!shell_) return;
        if (icons) shell_->RequestIcons(draft_, AutomaticIconKey(SelectedSlot()));
        const auto now = Clock::now();
        const Slot& selected = SelectedSlot();
        const std::wstring label = selected.Active() ? selected.Label() : L"Empty slot";
        BOOL animation = TRUE;
        if (draft_.respectMotion) SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animation, 0);
        const float factor = draft_.animations && animation ? 100.0f / draft_.speed : 0;
        if (state_.selected == slot_ && state_.page == page_ && !replay) {
            if (state_.label.shown != label) state_.label.Reset(label);
            auto& icon = state_.pages[page_].icons[slot_];
            const float scale = draft_.iconScale / 100.0f;
            if (icon.target != scale) {
                icon.To(scale, now, 180 * factor);
                if (icon.Running(now)) SetTimer(window_, 1, 15, nullptr);
            }
            if (appearanceChanged || !geometry_.extent) ConfigureWheel(wheel_);
            else InvalidateRect(wheel_, nullptr, FALSE);
            return;
        }
        const float from = state_.angle.Value(now);
        const int oldPage = state_.page;
        state_ = {};
        state_.page = page_;
        state_.selected = slot_;
        state_.visibility.Snap(1);
        state_.scale.Snap(1);
        state_.highlight.Snap(1);
        state_.angle.Snap(from);
        state_.angle.To(NearestAngle(from, slot_ * kStep), now, draft_.selectionMs * factor);
        for (auto& page : state_.pages) {
            for (auto& icon : page.icons) icon.Snap(1);
        }
        auto& page = state_.pages[page_];
        page.alpha.Snap(1);
        page.icons[slot_].To(draft_.iconScale / 100.0f, now, 180 * factor);
        if (oldPage != page_ && oldPage < static_cast<int>(draft_.pages.size()) &&
            factor && draft_.transition != PageTransition::Instant) {
            const float offset = draft_.transition == PageTransition::Rotate ? (oldPage < page_ ? 1 : -1) * kStep / 5 : 0;
            page.alpha.Snap(0);
            page.alpha.To(1, now, 195 * factor);
            page.angle.Snap(offset);
            page.angle.To(0, now, 195 * factor);
            auto& previous = state_.pages[oldPage];
            previous.alpha.Snap(1);
            previous.alpha.To(0, now, 195 * factor);
            previous.angle.To(-offset, now, 195 * factor);
        }
        state_.label.Reset(label);
        if (replay && factor) {
            state_.scale.Snap(.94f);
            state_.scale.To(1, now, 180 * factor);
            state_.visibility.Snap(0);
            state_.visibility.To(1, now, 180 * factor);
        }
        if (state_.Animating(now)) SetTimer(window_, 1, 15, nullptr);
        if (appearanceChanged || !geometry_.extent) ConfigureWheel(wheel_);
        else InvalidateRect(wheel_, nullptr, FALSE);
    }

    void PaintWheel(HWND window) {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(window, &paint);
        RECT area{};
        GetClientRect(window, &area);
        const bool buffered = area.right > 0 && area.bottom > 0 &&
                              previewFrame_.Resize(area.right, area.bottom);
        HDC frame = buffered ? previewFrame_.Dc() : dc;
        constexpr COLORREF backgrounds[] = {RGB(25, 46, 71), RGB(72, 42, 60), RGB(57, 61, 66)};
        SetDCBrushColor(frame, highContrast_ ? GetSysColor(COLOR_WINDOW) : backgrounds[std::clamp(sample_, 0, 2)]);
        FillRect(frame, &area, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
        if (shell_ && geometry_.extent > 0)
            renderer_.DrawPreview(frame, draft_, state_, *shell_, Clock::now(), drag_ ? &*drag_ : nullptr);
        if (buffered) {
            const auto& clip = paint.rcPaint;
            BitBlt(dc, clip.left, clip.top, clip.right - clip.left, clip.bottom - clip.top,
                   frame, clip.left, clip.top, SRCCOPY);
        }
        EndPaint(window, &paint);
    }

    static constexpr UINT_PTR kModalTimer = 2;
    HINSTANCE instance_ = nullptr;
    HANDLE stop_ = nullptr;
    HWND window_ = nullptr, wheel_ = nullptr, heading_ = nullptr, pageHeading_ = nullptr;
    HWND status_ = nullptr;
    HWND pendingChoice_ = nullptr;
    HFONT font_ = nullptr, headingFont_ = nullptr;
    UINT dpi_ = 96;
    bool registered_ = false, rendererReady_ = false, building_ = false, highContrast_ = false;
    int page_ = 0, slot_ = 0, tab_ = 0, sample_ = 0, dragSlot_ = -1;
    std::vector<Field> fields_;
    std::vector<ActionPreset> installedApps_, presets_;
    bool appsLoading_ = true;
    bool renamingPage_ = false;
    std::array<COLORREF, 16> customAccentColors_{};
    POINT dragStart_{};
    std::optional<PreviewDrag> drag_;
    Surface previewFrame_;
    Settings draft_{}, baseline_{};
    std::wstring error_, profileRoot_;
    struct Change {
        Settings settings;
        int page, slot;
        bool wasImport = false;
    };
    std::deque<Change> undo_;
    Geometry geometry_;
    VisualState state_;
    Renderer renderer_;
    std::shared_ptr<ShellWorker> shell_;
    Save saved_;
    Test test_;
};

class Menu {
public:
    Menu() = default;

    void Run(HANDLE stop, HANDLE reload) {
        stop_ = stop;
        const auto oldDpi = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        const HRESULT apartment = RoInitialize(RO_INIT_SINGLETHREADED);
        instance_ = GetModuleHandleW(nullptr);
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.hInstance = instance_;
        windowClass.lpszClassName = kWindowClass;
        windowClass.lpfnWndProc = WindowProc;
        windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        if (RegisterClassExW(&windowClass)) {
            window_ = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                                      WS_EX_TOPMOST | WS_EX_TRANSPARENT,
                                      kWindowClass, L"Fluent Radial Menu",
                                      WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, instance_, this);
            if (window_ && CreatePointerWindow() && renderer_.Init() && StartShell()) {
                profileRoot_ = ProfileRoot(profileDirectory_);
                if (!profileRoot_.empty()) {
                    MigrateEditorProfile(profileRoot_ + kProfileFile);
                    watcher_ = FindFirstChangeNotificationW(profileRoot_.c_str(), FALSE,
                        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE);
                } else {
                    Wh_Log(L"Windhawk profile storage is unavailable.");
                }
                ReloadWindhawkSettings();
                for (;;) {
                    HANDLE events[3] = {stop, reload};
                    DWORD count = 2;
                    const DWORD watchIndex = watcher_ != INVALID_HANDLE_VALUE ? count++ : MAXDWORD;
                    if (watchIndex != MAXDWORD) events[watchIndex] = watcher_;
                    const DWORD result = MsgWaitForMultipleObjectsEx(
                        count, events, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                    if (result == WAIT_OBJECT_0) break;
                    if (result == WAIT_OBJECT_0 + 1) {
                        ReloadWindhawkSettings();
                        continue;
                    }
                    if (watchIndex != MAXDWORD && result == WAIT_OBJECT_0 + watchIndex) {
                        if (!FindNextChangeNotification(watcher_)) {
                            FindCloseChangeNotification(std::exchange(watcher_, INVALID_HANDLE_VALUE));
                        }
                        ReloadProfile();
                        continue;
                    }
                    if (result != WAIT_OBJECT_0 + count) break;
                    MSG message;
                    bool quit = false;
                    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                        if (message.message == WM_QUIT) {
                            quit = true;
                            break;
                        }
                        if (HWND editor = editor_.Window();
                            editor && (message.hwnd == editor || IsChild(editor, message.hwnd))) {
                            if (IsDialogMessageW(editor, &message)) continue;
                        }
                        TranslateMessage(&message);
                        DispatchMessageW(&message);
                        if (WaitForSingleObject(stop, 0) == WAIT_OBJECT_0) {
                            quit = true;
                            break;
                        }
                    }
                    if (quit) break;
                }
            } else {
                Wh_Log(L"Could not initialize radial menu (error %lu)", GetLastError());
            }
            if (window_) {
                Hide();
                editor_.Close();
                if (hotkey_) UnregisterHotKey(window_, hotkeyId_);
                if (shell_) shell_->Stop();
                DestroyWindow(window_);
            }
            if (pointerWindow_) DestroyWindow(pointerWindow_);
            UnregisterClassW(kPointerClass, instance_);
            UnregisterClassW(kWindowClass, instance_);
        } else {
            Wh_Log(L"Could not register radial menu window (error %lu)", GetLastError());
        }
        if (watcher_ != INVALID_HANDLE_VALUE) FindCloseChangeNotification(watcher_);
        if (SUCCEEDED(apartment)) RoUninitialize();
        if (oldDpi) SetThreadDpiAwarenessContext(oldDpi);
    }

private:
    static LRESULT CALLBACK PointerWindowProc(HWND window, UINT message, WPARAM wParam,
                                               LPARAM lParam) {
        if (message == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
        if (message == WM_SETCURSOR) {
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            return TRUE;
        }
        return DefWindowProcW(window, message, wParam, lParam);
    }

    bool CreatePointerWindow() {
        WNDCLASSW windowClass{};
        windowClass.hInstance = instance_;
        windowClass.lpszClassName = kPointerClass;
        windowClass.lpfnWndProc = PointerWindowProc;
        windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        if (!RegisterClassW(&windowClass)) return false;
        pointerWindow_ = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                                         WS_EX_TOPMOST, kPointerClass, L"",
                                         WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, instance_, nullptr);
        // A nonzero alpha lets this window own the cursor. The rendered menu
        // sits above it and can keep its click-through shadow.
        return pointerWindow_ && SetLayeredWindowAttributes(pointerWindow_, 0, 1, LWA_ALPHA);
    }

    bool ShowPointerWindow() {
        const int inset = static_cast<int>(std::lround(
            geometry_.extent / 2.0f - geometry_.radius * geometry_.pixelsPerDip));
        const int diameter = geometry_.extent - 2 * inset;
        HRGN circle = CreateEllipticRgn(0, 0, diameter, diameter);
        if (!circle) return false;
        if (!SetWindowRgn(pointerWindow_, circle, FALSE)) {
            DeleteObject(circle);
            return false;
        }
        return SetWindowPos(pointerWindow_, HWND_TOPMOST, geometry_.origin.x + inset,
                            geometry_.origin.y + inset, diameter, diameter,
                            SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    bool StartShell() {
        shell_ = std::make_shared<ShellWorker>(window_);
        auto owner = std::make_unique<std::shared_ptr<ShellWorker>>(shell_);
        HANDLE thread = CreateThread(nullptr, 0, ShellWorker::Run, owner.get(), 0, nullptr);
        if (!thread) return false;
        owner.release();
        // The tool host exits on unload even if a shell extension is blocked.
        CloseHandle(thread);
        return true;
    }

    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
        auto* self = reinterpret_cast<Menu*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (message == WM_NCCREATE) {
            self = static_cast<Menu*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        if (self) return self->OnMessage(window, message, wParam, lParam);
        return DefWindowProcW(window, message, wParam, lParam);
    }

    LRESULT OnMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
        const Time now = Clock::now();
        switch (message) {
            case WM_HOTKEY:
                if (wParam == static_cast<WPARAM>(hotkeyId_) && hotkey_ &&
                    (LOWORD(lParam) & ~MOD_NOREPEAT) == hotkey_->modifiers &&
                    HIWORD(lParam) == hotkey_->key) {
                    if (inputActive_) {
                        if (desktopPreview_ || !settings_.holdToSelect) Close(now);
                    } else {
                        Open(now);
                    }
                }
                return 0;
            case WM_MOUSEACTIVATE:
                return MA_NOACTIVATE;
            case kPointer:
                pointerPosted_ = false;
                if (inputActive_) UpdatePointer(now);
                return 0;
            case kDismiss:
                if (inputActive_) Close(now);
                return 0;
            case kEditMenu:
                rightPressedInside_ = false;
                if (inputActive_ && !desktopPreview_) ShowEditMenu();
                return 0;
            case kOpenEditor:
                if (inputActive_ && !desktopPreview_) OpenEditor();
                return 0;
            case kWheel:
                if (inputActive_) {
                    const int direction = wheel_.Push(static_cast<short>(wParam),
                                                      settings_.wheelSensitivity, now);
                    if (direction) SwitchPage(direction, now);
                }
                return 0;
            case kSelect:
                if (inputActive_) {
                    if (wParam) {
                        keyboardSelection_ = false;
                        UpdateSelection(static_cast<int>(lParam), now);
                    }
                    Activate(now);
                }
                return 0;
            case kNavigate:
                if (inputActive_) Navigate(static_cast<UINT>(wParam), now);
                return 0;
            case kHoldRelease:
                if (inputActive_ && !desktopPreview_ && settings_.holdToSelect) {
                    if (!keyboardSelection_) UpdatePointer(now);
                    Activate(now, true);
                }
                return 0;
            case kIconsReady:
                if (visible_) RequestFrame();
                return 0;
            case kRepaint:
                Frame(now);
                return 0;
            case WM_TIMER:
                if (wParam == kFrameTimer) Frame(now);
                if (wParam == kPopupStopTimer && WaitForSingleObject(stop_, 0) == WAIT_OBJECT_0) EndMenu();
                return 0;
            case WM_DWMCOMPOSITIONCHANGED:
            case WM_DWMCOLORIZATIONCOLORCHANGED:
            case WM_SYSCOLORCHANGE:
            case WM_SETTINGCHANGE:
            case WM_THEMECHANGED:
                if (inputActive_) {
                    RefreshMotion();
                    const Palette palette = ReadPalette(settings_);
                    renderer_.Configure(geometry_, palette, backdrop_);
                    if (!animate_) Settle(now);
                    RequestFrame();
                }
                return 0;
            case WM_DISPLAYCHANGE:
            case WM_DPICHANGED:
                if (visible_) Hide();
                return 0;
            case WM_PAINT: {
                PAINTSTRUCT paint;
                BeginPaint(window, &paint);
                EndPaint(window, &paint);
                return 0;
            }
        }
        return DefWindowProcW(window, message, wParam, lParam);
    }

    void ReloadWindhawkSettings() {
        windhawkShortcut_ = Trim(ReadString(L"ActivationShortcut"));
        ReloadProfile(true);
    }

    void ReloadProfile(bool windhawkChanged = false) {
        if (!profileRoot_.empty()) {
            std::wstring error;
            auto bytes = ReadProfileBytes(profileRoot_ + kProfileFile, error, true);
            if (error.empty() && bytes != profileBytes_) {
                if (auto updated = ParseProfileBytes(bytes, error)) {
                    profileBytes_ = std::move(bytes);
                    ApplySettings(std::move(*updated));
                    return;
                }
            }
            if (!error.empty() && error != L"No active profile.") Wh_Log(L"%s", error.c_str());
        }
        if (settings_.pages.empty()) ApplySettings(DefaultSettings());
        else if (windhawkChanged) UpdateHotkey(settings_);
    }

    void ShowEditMenu() {
        POINT point;
        if (!GetCursorPos(&point)) return;
        const HMENU popup = CreatePopupMenu();
        if (!popup) return;
        AppendMenuW(popup, MF_STRING, 1, L"Edit radial menu");
        RECT ownerBounds{};
        GetWindowRect(pointerWindow_, &ownerBounds);
        Hide();
        if (!SetTimer(window_, kPopupStopTimer, 50, nullptr)) {
            DestroyMenu(popup);
            return;
        }
        SetWindowPos(pointerWindow_, HWND_TOPMOST, ownerBounds.left, ownerBounds.top, 0, 0,
                     SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow(pointerWindow_);
        const UINT choice = TrackPopupMenu(popup, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
                                          point.x, point.y, 0, pointerWindow_, nullptr);
        KillTimer(window_, kPopupStopTimer);
        PostMessageW(pointerWindow_, WM_NULL, 0, 0);
        ShowWindow(pointerWindow_, SW_HIDE);
        DestroyMenu(popup);
        if (choice == 1) OpenEditor();
    }

    void OpenEditor() {
        Hide();
        if (!editor_.Show(instance_, stop_, settings_, profileRoot_,
                          [this](Settings draft, std::wstring& status) {
                              return SaveEditor(std::move(draft), status);
                          },
                          [this] { Open(Clock::now(), true); })) {
            Wh_Log(L"Could not open the built-in radial menu editor");
        }
    }

    bool SaveEditor(Settings draft, std::wstring& status) {
        if (profileRoot_.empty()) {
            status = L"Windhawk profile storage is unavailable.";
            return false;
        }
        draft.revision = NewRevision();
        draft.name = L"Built-in editor";
        std::wstring json;
        AppendProfile(json, draft);
        if (!ParseProfile(json, status)) return false;
        if (!SaveProfileBytes(profileRoot_ + kProfileFile, ToUtf8(json))) {
            status = L"Could not save the radial menu profile.";
            return false;
        }
        ReloadProfile();
        if (settings_.revision != draft.revision) {
            status = L"The saved profile could not be applied.";
            return false;
        }
        if (!hotkeyError_.empty()) status = L"Saved. " + hotkeyError_;
        return true;
    }

    void ApplySettings(Settings updated) {
        const bool reopen = inputActive_ && !desktopPreview_ &&
                            !settings_.holdToSelect && !updated.holdToSelect;
        if (visible_) Hide();
        pendingKeys_.reset();
        KillTimer(window_, kFrameTimer);
        frameTimer_ = false;
        UpdateHotkey(updated);
        settings_ = std::move(updated);
        shell_->RequestIcons(settings_);
        if (reopen) Open(Clock::now());
    }

    std::wstring RegisterShortcut(const Chord& shortcut, const std::wstring& text) {
        if (shortcut != hotkey_) {
            const int nextId = hotkeyId_ == kHotkey ? kHotkey + 1 : kHotkey;
            if (!RegisterHotKey(window_, nextId, shortcut.modifiers | MOD_NOREPEAT, shortcut.key)) {
                const DWORD error = GetLastError();
                return L"Could not register " + text + L" (Windows error " +
                       std::to_wstring(error) + L").";
            }
            if (hotkey_) UnregisterHotKey(window_, hotkeyId_);
            hotkeyId_ = nextId;
            hotkey_ = shortcut;
        }
        registeredShortcut_ = text;
        return {};
    }

    void UpdateHotkey(const Settings& settings) {
        hotkeyError_.clear();
        if (!windhawkShortcut_.empty()) {
            const auto shortcut = ParseChord(windhawkShortcut_);
            hotkeyError_ = shortcut ? RegisterShortcut(*shortcut, windhawkShortcut_)
                                    : L"Invalid activation shortcut in Windhawk settings.";
            if (hotkeyError_.empty()) return;
        }
        if ((windhawkShortcut_.empty() || !hotkey_) && settings.shortcut) {
            const auto error = RegisterShortcut(*settings.shortcut, settings.shortcutText);
            if (!error.empty()) {
                if (!hotkeyError_.empty()) hotkeyError_ += L" ";
                hotkeyError_ += error;
            }
        }
        if (!hotkeyError_.empty()) {
            if (hotkey_) hotkeyError_ += L" Active shortcut: " + registeredShortcut_ + L".";
            Wh_Log(L"%s", hotkeyError_.c_str());
        }
    }

    void RefreshMotion() {
        BOOL enabled = TRUE;
        if (settings_.respectMotion) {
            SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0);
        }
        animate_ = settings_.animations && enabled;
    }

    float Duration(float milliseconds) const {
        return animate_ ? milliseconds * 100 / settings_.speed : 0;
    }

    bool Locate(POINT pointer) {
        HMONITOR monitor = MonitorFromPoint(pointer, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{sizeof(info), {}, {}, 0};
        if (!GetMonitorInfoW(monitor, &info)) return false;
        const RECT& work = info.rcWork;
        const POINT middle{work.left + (work.right - work.left) / 2,
                           work.top + (work.bottom - work.top) / 2};
        SetWindowPos(window_, nullptr, middle.x, middle.y, 1, 1, SWP_NOZORDER | SWP_NOACTIVATE);
        UINT dpi = GetDpiForWindow(window_);
        geometry_.pixelsPerDip = (dpi ? dpi : 96) / 96.0f;
        const float available = std::min(work.right - work.left, work.bottom - work.top) /
                                geometry_.pixelsPerDip - 2 * kShadow;
        const float diameter = std::min(static_cast<float>(settings_.diameter), available);
        if (diameter < 84) return false;
        geometry_.radius = diameter / 2;
        geometry_.inner = diameter * settings_.centerDiameter / settings_.diameter / 2;
        geometry_.extent = std::min(std::min(work.right - work.left, work.bottom - work.top),
            static_cast<LONG>(std::ceil((diameter + 2 * kShadow) * geometry_.pixelsPerDip)));
        const POINT anchor = settings_.centerOnMonitor ? middle : pointer;
        geometry_.origin = {std::clamp(anchor.x - geometry_.extent / 2, work.left,
                                       work.right - geometry_.extent),
                            std::clamp(anchor.y - geometry_.extent / 2, work.top,
                                       work.bottom - geometry_.extent)};
        return true;
    }

    bool Open(Time now, bool preview = false) {
        if (visible_) Hide();
        pendingKeys_.reset();
        POINT pointer;
        if (!GetCursorPos(&pointer) || !Locate(pointer)) return false;
        const bool hold = settings_.holdToSelect && !preview;
        if (hold && !ActivationHeld()) return false;
        desktopPreview_ = preview;
        RefreshMotion();
        const Palette palette = ReadPalette(settings_);
        backdrop_ = CaptureBackdrop(geometry_, palette.highContrast ? 0 : settings_.blur);
        renderer_.Configure(geometry_, palette, backdrop_);
        wheel_ = {};
        state_ = {};
        for (auto& page : state_.pages) {
            for (auto& icon : page.icons) icon.Snap(1);
        }
        state_.pages[0].alpha.Snap(1);
        state_.label.Reset(settings_.idleLabel);
        now = Clock::now();
        state_.visibility.Snap(0);
        state_.visibility.To(1, now, Duration(180));
        state_.scale.Snap(0.94f);
        state_.scale.To(1, now, Duration(180));
        inputActive_ = true;
        visible_ = true;
        keyboardSelection_ = false;
        UpdateSelection(geometry_.Hit(pointer), now);
        if ((hold && !ActivationHeld()) || !InstallHooks() ||
            !renderer_.Draw(window_, settings_, state_, *shell_, now)) {
            Hide();
            return false;
        }
        if (!ShowPointerWindow()) {
            Wh_Log(L"Could not show the radial menu input window (error %lu)", GetLastError());
            Hide();
            return false;
        }
        SetWindowPos(window_, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        if (GetCursorPos(&pointer) && geometry_.Hit(pointer) != -2) {
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
        }
        if (state_.Animating(now)) RequestFrame();
        return true;
    }

    void Close(Time now) {
        inputActive_ = false;
        RemoveHooks();
        ShowWindow(pointerWindow_, SW_HIDE);
        state_.visibility.To(0, now, Duration(140));
        state_.scale.To(0.965f, now, Duration(140));
        if (!animate_) Hide();
        else RequestFrame();
    }

    void Hide() {
        desktopPreview_ = false;
        inputActive_ = false;
        visible_ = false;
        RemoveHooks();
        if (pointerWindow_) ShowWindow(pointerWindow_, SW_HIDE);
        ShowWindow(window_, SW_HIDE);
        renderer_.ReleaseSurface();
        backdrop_ = {};
        if (!pendingKeys_) {
            KillTimer(window_, kFrameTimer);
            frameTimer_ = false;
        }
    }

    void UpdatePointer(Time now) {
        POINT point;
        if (!GetCursorPos(&point)) return;
        keyboardSelection_ = false;
        UpdateSelection(geometry_.Hit(point), now);
    }

    void UpdateSelection(int hit, Time now) {
        const int index = hit >= 0 && hit < kSlots && settings_.pages[state_.page][hit].Active()
                              ? hit : -1;
        if (index == state_.selected) return;
        auto& icons = state_.pages[state_.page].icons;
        if (state_.selected >= 0) icons[state_.selected].To(1, now, Duration(180));
        state_.selected = index;
        state_.highlight.To(index >= 0 ? 1 : 0, now, Duration(155));
        if (index >= 0) {
            icons[index].To(settings_.iconScale / 100.0f, now, Duration(180));
            const float target = index * kStep;
            if (!state_.hasAngle || state_.highlight.Value(now) <= 0.001f) {
                state_.angle.Snap(target);
                state_.hasAngle = true;
            } else {
                const float current = std::remainder(state_.angle.Value(now), kTurn);
                state_.angle.Snap(current);
                state_.angle.To(NearestAngle(current, target), now, Duration(settings_.selectionMs));
            }
        }
        state_.label.Set(index >= 0 ? settings_.pages[state_.page][index].Label() : settings_.idleLabel,
                         now, Duration(48), Duration(150));
        RequestFrame();
    }

    void SwitchPage(int direction, Time now) {
        const int count = static_cast<int>(settings_.pages.size());
        if (count < 2) return;
        const int oldPage = state_.page;
        const int next = (oldPage + direction + count) % count;
        const float duration = settings_.transition == PageTransition::Instant ? 0 : Duration(195);
        const float turn = settings_.transition == PageTransition::Rotate ? direction * kStep / 5 : 0;
        auto& from = state_.pages[oldPage];
        auto& to = state_.pages[next];
        if (state_.selected >= 0) from.icons[state_.selected].To(1, now, Duration(180));
        if (to.alpha.Value(now) <= 0.001f) to.angle.Snap(turn);
        from.alpha.To(0, now, duration);
        from.angle.To(-turn, now, duration);
        to.alpha.To(1, now, duration);
        to.angle.To(0, now, duration);
        state_.page = next;
        const int keyboardIndex = keyboardSelection_ ? state_.selected : -1;
        state_.selected = -1;
        if (keyboardSelection_) UpdateSelection(keyboardIndex, now);
        else UpdatePointer(now);
        if (state_.selected < 0) {
            state_.highlight.To(0, now, Duration(155));
            state_.label.Set(settings_.idleLabel, now, Duration(48), Duration(150));
        }
        RequestFrame();
    }

    void Navigate(UINT key, Time now) {
        if (key == VK_PRIOR || key == VK_NEXT) {
            SwitchPage(key == VK_NEXT ? 1 : -1, now);
            return;
        }
        keyboardSelection_ = true;
        const int direction = key == VK_LEFT || key == VK_UP ? -1 : 1;
        int index = state_.selected;
        if (index < 0) index = direction == 1 ? kSlots - 1 : 0;
        for (int i = 0; i < kSlots; ++i) {
            index = (index + direction + kSlots) % kSlots;
            if (settings_.pages[state_.page][index].Active()) {
                UpdateSelection(index, now);
                break;
            }
        }
    }

    void Activate(Time now, bool release = false) {
        if (state_.selected < 0) {
            if (release) Close(now);
            return;
        }
        const Slot& slot = settings_.pages[state_.page][state_.selected];
        const bool preview = desktopPreview_;
        if (settings_.holdToSelect || settings_.closeAfterSelection) Close(now);
        if (preview) return;
        if (slot.action == Action::Shortcut) {
            if (auto chord = ParseChord(slot.target)) {
                if (chord != hotkey_) {
                    pendingKeys_ = *chord;
                    keyDeadline_ = now + 1500ms;
                    DispatchKeys(now);
                    if (pendingKeys_) RequestFrame();
                } else {
                    Wh_Log(L"Radial menu action matches its activation shortcut");
                }
            } else {
                Wh_Log(L"Invalid radial menu shortcut action");
            }
        } else {
            shell_->Launch(slot);
        }
    }

    void DispatchKeys(Time now) {
        if (!pendingKeys_) return;
        if (now >= keyDeadline_) {
            pendingKeys_.reset();
            Wh_Log(L"Radial menu shortcut timed out waiting for held keys");
            return;
        }
        for (const auto& modifier : kModifiers) {
            if (!(pendingKeys_->modifiers & modifier.flag) && ModifierDown(modifier)) return;
        }
        if (KeyDown(pendingKeys_->key)) return;
        if (!SendChord(*pendingKeys_)) Wh_Log(L"Could not send radial menu shortcut");
        pendingKeys_.reset();
    }

    void Settle(Time now) {
        state_.visibility.To(inputActive_ ? 1 : 0, now, 0);
        state_.scale.Snap(1);
        state_.angle.Snap(state_.angle.target);
        state_.highlight.Snap(state_.highlight.target);
        state_.label.Reset(state_.label.pending);
        for (auto& page : state_.pages) {
            page.alpha.Snap(page.alpha.target);
            page.angle.Snap(page.angle.target);
            for (auto& icon : page.icons) icon.Snap(icon.target);
        }
    }

    void RequestFrame() {
        if (frameTimer_) return;
        frameTimer_ = SetTimer(window_, kFrameTimer, 15, nullptr) != 0;
        if (!frameTimer_) {
            animate_ = false;
            Settle(Clock::now());
            pendingKeys_.reset();
            PostMessageW(window_, kRepaint, 0, 0);
        }
    }

    void Frame(Time now) {
        DispatchKeys(now);
        if (visible_) {
            if (!inputActive_ && !state_.visibility.Running(now)) {
                Hide();
            } else {
                state_.label.Advance(now, Duration(150));
                if (!renderer_.Draw(window_, settings_, state_, *shell_, now)) Hide();
            }
        }
        if ((!visible_ || !state_.Animating(now)) && !pendingKeys_) {
            KillTimer(window_, kFrameTimer);
            frameTimer_ = false;
        }
    }

    bool InstallHooks() {
        current_ = this;
        HMODULE module = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<PCWSTR>(&MouseProc), &module);
        mouse_ = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, module, 0);
        keyboard_ = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, module, 0);
        if (mouse_ && keyboard_) return true;
        Wh_Log(L"Could not register radial menu input hooks (error %lu)", GetLastError());
        RemoveHooks();
        return false;
    }

    void RemoveHooks() {
        if (mouse_) UnhookWindowsHookEx(std::exchange(mouse_, nullptr));
        if (keyboard_) UnhookWindowsHookEx(std::exchange(keyboard_, nullptr));
        current_ = nullptr;
        pointerPosted_ = false;
        leftPressedInside_ = false;
        rightPressedInside_ = false;
    }

    bool ActivationHeld() const {
        if (!hotkey_ || !KeyDown(hotkey_->key)) return false;
        for (const auto& modifier : kModifiers) {
            if ((hotkey_->modifiers & modifier.flag) && !ModifierDown(modifier)) return false;
        }
        return true;
    }

    static LRESULT CALLBACK MouseProc(int code, WPARAM message, LPARAM payload) {
        if (code == HC_ACTION && current_ && current_->inputActive_) {
            Menu& menu = *current_;
            const auto& mouse = *reinterpret_cast<MSLLHOOKSTRUCT*>(payload);
            if (message == WM_MOUSEMOVE && !menu.pointerPosted_) {
                menu.pointerPosted_ = PostMessageW(menu.window_, kPointer, 0, 0);
            } else if (message == WM_MOUSEWHEEL) {
                if (PostMessageW(menu.window_, kWheel, HIWORD(mouse.mouseData), 0)) return 1;
            } else if (message == WM_LBUTTONUP && std::exchange(menu.leftPressedInside_, false)) {
                const int hit = menu.geometry_.Hit(mouse.pt);
                if (hit >= 0) PostMessageW(menu.window_, kSelect, 1, hit);
                return 1;
            } else if (message == WM_RBUTTONUP && std::exchange(menu.rightPressedInside_, false)) {
                PostMessageW(menu.window_, kEditMenu, 0, 0);
                return 1;
            } else if ((message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
                        message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN)) {
                if (menu.geometry_.Hit(mouse.pt) == -2) {
                    PostMessageW(menu.window_, kDismiss, 0, 0);
                } else if (message == WM_LBUTTONDOWN) {
                    menu.leftPressedInside_ = true;
                    return 1;
                } else if (message == WM_RBUTTONDOWN) {
                    menu.rightPressedInside_ = true;
                    return 1;
                }
            }
        }
        return CallNextHookEx(nullptr, code, message, payload);
    }

    static LRESULT CALLBACK KeyboardProc(int code, WPARAM message, LPARAM payload) {
        if (code == HC_ACTION && current_ && current_->inputActive_) {
            const auto& key = *reinterpret_cast<KBDLLHOOKSTRUCT*>(payload);
            if (key.dwExtraInfo != kSyntheticKeys && current_->OnKey(message, key.vkCode)) return 1;
        }
        return CallNextHookEx(nullptr, code, message, payload);
    }

    bool OnKey(WPARAM message, DWORD key) {
        const bool up = message == WM_KEYUP || message == WM_SYSKEYUP;
        if (up && !desktopPreview_ && settings_.holdToSelect && hotkey_) {
            UINT modifier = 0;
            if (key == VK_LCONTROL || key == VK_RCONTROL || key == VK_CONTROL) modifier = MOD_CONTROL;
            if (key == VK_LSHIFT || key == VK_RSHIFT || key == VK_SHIFT) modifier = MOD_SHIFT;
            if (key == VK_LMENU || key == VK_RMENU || key == VK_MENU) modifier = MOD_ALT;
            if (key == VK_LWIN || key == VK_RWIN) modifier = MOD_WIN;
            if (key == hotkey_->key || (modifier & hotkey_->modifiers)) {
                PostMessageW(window_, kHoldRelease, 0, 0);
                return false;
            }
        }
        if (hotkey_ && key == hotkey_->key) return false;
        switch (key) {
            case VK_F2:
                if (desktopPreview_) return false;
                if (up) PostMessageW(window_, kOpenEditor, 0, 0);
                return true;
            case VK_ESCAPE:
                if (!up) PostMessageW(window_, kDismiss, 0, 0);
                return true;
            case VK_RETURN:
                if (up) PostMessageW(window_, kSelect, 0, 0);
                return true;
            case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
            case VK_PRIOR: case VK_NEXT:
                if (!up) PostMessageW(window_, kNavigate, key, 0);
                return true;
        }
        return false;
    }

    inline static Menu* current_ = nullptr;
    HWND window_ = nullptr;
    HWND pointerWindow_ = nullptr;
    HINSTANCE instance_ = nullptr;
    HHOOK mouse_ = nullptr, keyboard_ = nullptr;
    bool visible_ = false, inputActive_ = false, frameTimer_ = false;
    bool pointerPosted_ = false, keyboardSelection_ = false, animate_ = false;
    bool leftPressedInside_ = false;
    bool rightPressedInside_ = false, desktopPreview_ = false;
    int hotkeyId_ = kHotkey;
    HANDLE watcher_ = INVALID_HANDLE_VALUE, stop_ = nullptr;
    FileHandle profileDirectory_{nullptr, CloseHandle};
    std::wstring profileRoot_, hotkeyError_, registeredShortcut_, windhawkShortcut_;
    std::string profileBytes_;
    std::optional<Chord> hotkey_, pendingKeys_;
    Time keyDeadline_{};
    Settings settings_{};
    Geometry geometry_;
    Wheel wheel_;
    VisualState state_;
    Renderer renderer_;
    MenuEditor editor_;
    IconPixels backdrop_;
    std::shared_ptr<ShellWorker> shell_;
};

HANDLE g_thread, g_stop, g_reload;

DWORD WINAPI MenuThread(void*) {
    Menu menu;
    menu.Run(g_stop, g_reload);
    return 0;
}

}  // namespace

void WhTool_ModUninit() {
    if (g_thread) {
        SetEvent(g_stop);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(std::exchange(g_thread, nullptr));
    }
    if (g_stop) CloseHandle(std::exchange(g_stop, nullptr));
    if (g_reload) CloseHandle(std::exchange(g_reload, nullptr));
}

BOOL WhTool_ModInit() {
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_reload = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stop && g_reload) {
        g_thread = CreateThread(nullptr, 0, MenuThread, nullptr, 0, nullptr);
    }
    if (g_thread) return TRUE;
    WhTool_ModUninit();
    return FALSE;
}

void WhTool_ModSettingsChanged() {
    if (g_reload) SetEvent(g_reload);
}


// clang-format off
////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
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
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
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

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
// clang-format on
