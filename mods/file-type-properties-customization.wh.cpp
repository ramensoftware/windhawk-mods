// ==WindhawkMod==
// @id              file-type-properties-customization
// @name            File Type Properties Customization
// @description     Customize file icons and type descriptions by extension while preserving system thumbnails
// @version         1.0.0
// @author          Momo
// @github          https://github.com/FreshP-0325
// @license         MIT
// @include         explorer.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lshell32 -lshlwapi -lole32 -loleaut32 -lpropsys -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Type Properties Customization

Customize file icons and type descriptions by extension. Icons and descriptions
can be enabled independently. File associations, default applications, and
system thumbnail providers are preserved.

Before:

![Before](https://raw.githubusercontent.com/FreshP-0325/windhawk-mod-assets/refs/heads/master/file-type-properties-customization/1.0.0/before.png)

After:

![After](https://raw.githubusercontent.com/FreshP-0325/windhawk-mod-assets/refs/heads/master/file-type-properties-customization/1.0.0/after.png)

## Windhawk settings

Fill in the first File type properties entry, then save settings. For example:

* Extensions: `.txt,.log`
* Icon: `%SystemRoot%\System32\shell32.dll,70`
* Description: `Text document`

Either property may be left empty to keep the preset or system value. A completely
empty entry ends the list; entries below it are ignored. Windhawk's Textual mode
can edit the same settings as YAML.

## Optional INI preset

Enable Load INI preset and select an absolute INI path. The preset is read first;
nonempty valid Windhawk settings override individual properties afterward.

```ini
[.txt,.log]
Icon=icons\text.ico
Description=Text document

[.log]
Description=Log file
```

Sections and keys are case-insensitive. Repeated sections merge per property in
file order. Empty or invalid values do not erase earlier usable values. Relative
resource paths use the INI directory; paths in settings must be absolute. Paths
support environment variables. Quote icon paths containing commas. Icon indexes
start at 0; negative indexes select resource IDs. Descriptions also accept
`@module,-resourceID`, or `@@` to start literal text with `@`.

Save INI files as UTF-8, or UTF-16 LE with BOM. Full-line comments start with
`;` or `#`. After editing the INI, re-enable the mod or save a changed mod setting.
The file is not watched automatically.

## Refresh and limitations

* Refresh Explorer when mappings change is off by default. Enabling it may redraw
  windows and reload thumbnails after changed mappings are saved, including the
  first or last rule. An empty configuration stays loaded without hooks while
  this option is on. Startup and shutdown do not request a refresh.
* Reopen affected windows or menus if they still show cached values.
* Prefer local icon files. First use of a slow or disconnected network resource
  can block the requesting window until Windows finishes trying to load it.
* By default, only explorer.exe is targeted. For an application's standard
  Open/Save dialogs, add its executable to Custom process inclusion list and
  restart it. Applications with their own rendering or caches may be unaffected.
* Description overrides cover supported Type columns, file properties, details,
  and native Shell New menus. If optional Shell symbols are unavailable, some
  views keep system descriptions; details are logged.
* x86, x64, and ARM64 are targeted. ARM64 has not been tested on a physical device.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableIcons: true
  $name: Customize icons
  $description: Enable icon overrides. Changing this option reloads the mod.
- enableDescriptions: true
  $name: Customize type descriptions
  $description: Enable description overrides in supported Shell views and menus. Changing this option reloads the mod.
- properties:
  - - extensions: ""
      $name: Extensions
      $description: Comma-separated file extensions, including the dot. For example, .txt,.log
    - icon: ""
      $name: Icon
      $description: Absolute icon path, optionally followed by an index. Leave empty to keep the preset or system icon.
    - description: ""
      $name: Description
      $description: Text or @module,-resourceID. Resource paths must be absolute. Leave empty to keep the preset or system description.
  $name: File type properties
  $description: These values override the INI per property. Fill in the first entry; a completely empty entry ends the list.
- useIni: false
  $name: Load INI preset
  $description: Read the INI first, then apply the list above as per-property overrides. No files are modified.
- iniPath: "%USERPROFILE%\\FileTypeProperties\\properties.ini"
  $name: INI preset path
  $description: Absolute path, used only when Load INI preset is enabled. Re-enable the mod after editing the file, or save a changed mod setting.
- refreshExplorer: false
  $name: Refresh Explorer when mappings change
  $description: Refresh cached icons and type names after saving changed mappings, including the first or last rule. May redraw windows and reload thumbnails. Keeps the mod loaded without hooks when no rules are configured. Usually leave off.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <propsys.h>
#include <propvarutil.h>
#include <atomic>
#include <array>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <wrl/client.h>
#include <climits>
#include <algorithm>
#include <utility>

namespace {
using Microsoft::WRL::ComPtr;
struct IconValidation { std::once_flag once; bool usable = false; };
struct IconLocation {
    std::wstring path;
    int index = 0;
    std::shared_ptr<IconValidation> validation = std::make_shared<IconValidation>();
    bool operator==(const IconLocation& other) const { return path == other.path && index == other.index; }
};
using IconMap = std::unordered_map<std::wstring, std::vector<IconLocation>>;
using TypeMap = std::unordered_map<std::wstring, std::wstring>;
struct Configuration {
    IconMap icons;
    TypeMap types;
    bool operator==(const Configuration&) const = default;
};
std::shared_ptr<const Configuration> g_configuration = std::make_shared<const Configuration>();
bool g_enableIcons = true;
bool g_enableDescriptions = true;
struct ActiveFeatures {
    bool icons = false;
    bool descriptions = false;
    bool operator==(const ActiveFeatures&) const = default;
};
ActiveFeatures g_activeFeatures;
bool g_refreshAfterInit = false;
bool g_emptyRefreshListener = false;
thread_local bool g_inHook = false;
struct HookGuard {
    HookGuard() { g_inHook = true; }
    ~HookGuard() { g_inHook = false; }
};

std::shared_ptr<const IconMap> GetIcons() {
    auto configuration = std::atomic_load(&g_configuration);
    return {configuration, &configuration->icons};
}
std::shared_ptr<const TypeMap> GetTypes() {
    auto configuration = std::atomic_load(&g_configuration);
    return {configuration, &configuration->types};
}

ActiveFeatures GetActiveFeatures() {
    auto configuration = std::atomic_load(&g_configuration);
    return {!configuration->icons.empty(), !configuration->types.empty()};
}

std::wstring Trim(std::wstring_view s) {
    const auto first = s.find_first_not_of(L" \t\r\n");
    if (first == s.npos) return {};
    return std::wstring(s.substr(first, s.find_last_not_of(L" \t\r\n") - first + 1));
}

std::wstring Lower(std::wstring s) {
    // Invariant case folding, independent of the process's C locale.
    if (!s.empty()) {
        std::wstring result(s.size(), L'\0');
        if (LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_LOWERCASE, s.data(),
                          static_cast<int>(s.size()), result.data(),
                          static_cast<int>(result.size()), nullptr, nullptr, 0)) {
            return result;
        }
    }
    return s;
}

std::optional<std::wstring> Expand(std::wstring s) {
    DWORD size = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (!size || size > 32768) return std::nullopt;
    std::wstring result(size, L'\0');
    DWORD written = ExpandEnvironmentStringsW(s.c_str(), result.data(), size);
    if (!written || written > size) return std::nullopt;
    result.resize(written - 1);
    return result;
}

bool ParseIndex(std::wstring_view text, int& result) {
    auto s = Trim(text);
    if (s.empty()) return false;
    size_t start = (s[0] == L'-' || s[0] == L'+') ? 1 : 0;
    if (start == s.size()) return false;
    unsigned long long n = 0;
    const auto limit = s[0] == L'-' ? 2147483648ULL : 2147483647ULL;
    for (size_t i = start; i < s.size(); i++) {
        if (s[i] < L'0' || s[i] > L'9') return false;
        n = n * 10 + (s[i] - L'0');
        if (n > limit) return false;
    }
    result = s[0] == L'-' ? static_cast<int>(-static_cast<long long>(n))
                           : static_cast<int>(n);
    return true;
}

std::optional<IconLocation> ParseLocation(std::wstring text,
                                        const std::filesystem::path& base) {
    text = Trim(text);
    IconLocation icon;
    std::wstring path;
    if (!text.empty() && text.front() == L'"') {
        const auto end = text.find(L'"', 1);
        if (end == text.npos) return std::nullopt;
        path = text.substr(1, end - 1);
        auto tail = Trim(std::wstring_view(text).substr(end + 1));
        if (!tail.empty() && (tail.front() != L',' ||
                             !ParseIndex(std::wstring_view(tail).substr(1), icon.index))) {
            return std::nullopt;
        }
    } else {
        const auto comma = text.rfind(L',');
        if (comma != text.npos) {
            if (!ParseIndex(std::wstring_view(text).substr(comma + 1), icon.index)) {
                return std::nullopt;
            }
            text.resize(comma);
        }
        path = Trim(text);
    }
    if (path.empty() || path.find(L'"') != path.npos || icon.index == INT_MIN) {
        return std::nullopt;
    }
    auto expanded = Expand(path);
    if (!expanded || expanded->empty()) return std::nullopt;
    std::filesystem::path p(*expanded);
    // Reject drive-relative and root-relative paths; they depend on host state.
    if (!p.is_absolute()) {
        if (base.empty() || p.has_root_name() || p.has_root_directory()) return std::nullopt;
        p = base / p;
    }
    icon.path = p.lexically_normal().wstring();
    return icon;
}

enum class IniReadResult { Line, End, Error };

class IniLineReader {
public:
    static constexpr DWORD kBufferSize = 16 * 1024;

    explicit IniLineReader(const std::wstring& path) {
        file_ = CreateFileW(path.c_str(), GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
        if (file_ == INVALID_HANDLE_VALUE) {
            error_ = GetLastError();
            return;
        }
        BYTE byte;
        while (prefixSize_ < 2 && ReadRawByte(byte)) prefix_[prefixSize_++] = byte;
        if (prefixSize_ == 2 && prefix_[0] == 0xff && prefix_[1] == 0xfe) {
            utf16_ = true;
            prefixSize_ = 0;
        } else {
            if (prefixSize_ == 2 && ReadRawByte(byte)) prefix_[prefixSize_++] = byte;
            if (prefixSize_ == 3 && prefix_[0] == 0xef && prefix_[1] == 0xbb && prefix_[2] == 0xbf)
                prefixSize_ = 0;
        }
    }

    ~IniLineReader() {
        if (file_ != INVALID_HANDLE_VALUE) CloseHandle(file_);
    }
    IniLineReader(const IniLineReader&) = delete;
    IniLineReader& operator=(const IniLineReader&) = delete;

    IniReadResult NextLine(std::wstring& line) {
        line.clear();
        bool hasInput = false;
        bool comment = false;
        WCHAR character;
        while (ReadCharacter(character)) {
            if (skipLf_) {
                skipLf_ = false;
                if (character == L'\n') continue;
            }
            hasInput = true;
            if (character == L'\r' || character == L'\n') {
                skipLf_ = character == L'\r';
                return IniReadResult::Line;
            }
            if (comment) continue;
            if (line.empty()) {
                if (character == L' ' || character == L'\t') continue;
                if (character == L';' || character == L'#') { comment = true; continue; }
            }
            line.push_back(character);
        }
        if (error_ != ERROR_SUCCESS) return IniReadResult::Error;
        return hasInput ? IniReadResult::Line : IniReadResult::End;
    }

    DWORD Error() const { return error_; }

private:
    bool ReadRawByte(BYTE& byte) {
        if (error_ != ERROR_SUCCESS) return false;
        if (position_ == available_) {
            if (end_) return false;
            DWORD count = 0;
            if (!ReadFile(file_, buffer_.data(), kBufferSize, &count, nullptr)) {
                error_ = GetLastError();
                if (error_ == ERROR_SUCCESS) error_ = ERROR_READ_FAULT;
                return false;
            }
            position_ = 0;
            available_ = count;
            if (!count) { end_ = true; return false; }
        }
        byte = buffer_[position_++];
        return true;
    }

    bool ReadByte(BYTE& byte) {
        if (prefixPosition_ < prefixSize_) { byte = prefix_[prefixPosition_++]; return true; }
        return ReadRawByte(byte);
    }

    bool InvalidEncoding() {
        if (error_ == ERROR_SUCCESS) error_ = ERROR_NO_UNICODE_TRANSLATION;
        return false;
    }

    bool ReadCharacter(WCHAR& character) {
        if (error_ != ERROR_SUCCESS) return false;
        if (pending_) { character = std::exchange(pending_, 0); return true; }
        BYTE first;
        if (!ReadByte(first)) return highSurrogate_ ? InvalidEncoding() : false;
        if (utf16_) {
            BYTE second;
            if (!ReadByte(second)) return InvalidEncoding();
            character = static_cast<WCHAR>(first | (static_cast<unsigned>(second) << 8));
            bool high = character >= 0xd800 && character <= 0xdbff;
            bool low = character >= 0xdc00 && character <= 0xdfff;
            if (highSurrogate_ != low) return InvalidEncoding();
            highSurrogate_ = high;
        } else if (first < 0x80) {
            character = first;
        } else {
            char bytes[4]{static_cast<char>(first)};
            int size = first >= 0xc2 && first <= 0xdf ? 2 :
                       first >= 0xe0 && first <= 0xef ? 3 :
                       first >= 0xf0 && first <= 0xf4 ? 4 : 0;
            if (!size) return InvalidEncoding();
            for (int i = 1; i < size; ++i) {
                BYTE next;
                if (!ReadByte(next)) return InvalidEncoding();
                bytes[i] = static_cast<char>(next);
            }
            WCHAR decoded[2];
            int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes, size, decoded, 2);
            if (!count) return InvalidEncoding();
            character = decoded[0];
            if (count == 2) pending_ = decoded[1];
        }
        if (!character) return InvalidEncoding();
        return true;
    }

    HANDLE file_ = INVALID_HANDLE_VALUE;
    std::array<BYTE, kBufferSize> buffer_{};
    DWORD position_ = 0;
    DWORD available_ = 0;
    DWORD error_ = ERROR_SUCCESS;
    BYTE prefix_[3]{};
    size_t prefixSize_ = 0;
    size_t prefixPosition_ = 0;
    WCHAR pending_ = 0;
    bool utf16_ = false;
    bool highSurrogate_ = false;
    bool end_ = false;
    bool skipLf_ = false;
};

bool ValidExtension(const std::wstring& ext) {
    return ext.size() > 1 && ext.front() == L'.' &&
           ext.find_first_of(L".\\/:*?\"<>|,; \t", 1) == ext.npos;
}

bool CanExtract(const IconLocation& icon) {
    HICON large = nullptr;
    HRESULT hr = SHDefExtractIconW(icon.path.c_str(), icon.index, 0, &large,
                                   nullptr, 32);
    bool ok = SUCCEEDED(hr) && large;
    if (large) DestroyIcon(large);
    return ok;
}

std::optional<std::wstring> ResolveDescription(std::wstring value,
                                              const std::filesystem::path& base) {
    value = Trim(value);
    if (value.empty()) return std::nullopt;
    if (value.starts_with(L"@@")) return value.substr(1);
    if (value.front() != L'@') return value;
    auto comma = value.rfind(L',');
    if (comma == value.npos) return std::nullopt;
    auto id = Trim(std::wstring_view(value).substr(comma + 1));
    if (id.size() < 2 || id.front() != L'-' ||
        id.find_first_not_of(L"0123456789", 1) != id.npos) return std::nullopt;
    unsigned long long resourceId = 0;
    for (size_t i = 1; i < id.size(); ++i) {
        resourceId = resourceId * 10 + (id[i] - L'0');
        if (resourceId > 2147483647ULL) return std::nullopt;
    }
    if (!resourceId) return std::nullopt;
    auto module = Trim(std::wstring_view(value).substr(1, comma - 1));
    if (module.size() >= 2 && module.front() == L'"' && module.back() == L'"') {
        module = module.substr(1, module.size() - 2);
    }
    auto expanded = Expand(module);
    if (!expanded || expanded->empty() || expanded->find(L'"') != expanded->npos) return std::nullopt;
    std::filesystem::path path(*expanded);
    if (!path.is_absolute()) {
        if (base.empty() || path.has_root_name() || path.has_root_directory()) return std::nullopt;
        path = base / path;
    }
    auto indirect = L"@" + path.lexically_normal().wstring() + L"," + id;
    std::vector<WCHAR> result(32768);
    if (FAILED(SHLoadIndirectString(indirect.c_str(), result.data(),
                                    static_cast<UINT>(result.size()), nullptr)) || !result[0]) {
        return std::nullopt;
    }
    return std::wstring(result.data());
}

std::vector<std::wstring> ParseExtensions(std::wstring_view text) {
    std::vector<std::wstring> extensions;
    size_t start = 0;
    do {
        auto end = text.find(L',', start);
        auto extension = Lower(Trim(text.substr(start,
            end == text.npos ? text.size() - start : end - start)));
        if (ValidExtension(extension)) {
            if (std::find(extensions.begin(), extensions.end(), extension) == extensions.end())
                extensions.push_back(std::move(extension));
        } else if (!extension.empty()) {
            Wh_Log(L"Invalid extension '%s'; skipped", extension.c_str());
        }
        if (end == text.npos) break;
        start = end + 1;
    } while (start <= text.size());
    return extensions;
}

void ApplyProperty(Configuration& configuration, const std::vector<std::wstring>& extensions,
                   std::wstring_view key, std::wstring value, const std::filesystem::path& base) {
    if (extensions.empty() || Trim(value).empty()) return;
    if (key == L"icon" && g_enableIcons) {
        auto icon = ParseLocation(std::move(value), base);
        if (!icon) { Wh_Log(L"Invalid icon location; skipped"); return; }
        for (const auto& extension : extensions) configuration.icons[extension].push_back(*icon);
    } else if (key == L"description" && g_enableDescriptions) {
        auto description = ResolveDescription(std::move(value), base);
        if (!description) { Wh_Log(L"Invalid description or unavailable resource; skipped"); return; }
        for (const auto& extension : extensions) configuration.types[extension] = *description;
    }
}

class IniParser {
public:
    explicit IniParser(std::filesystem::path base) : base_(std::move(base)) {}

    void ParseLine(std::wstring_view input) {
        auto line = Trim(input);
        if (line.empty() || line.front() == L';' || line.front() == L'#') return;
        if (line.front() == L'[') {
            extensions_.clear();
            if (line.back() != L']') return;
            auto name = Trim(std::wstring_view(line).substr(1, line.size() - 2));
            if (!name.empty() && name.front() == L'.') {
                extensions_ = ParseExtensions(name);
            }
            return;
        }
        auto equals = line.find(L'=');
        if (equals == line.npos || extensions_.empty()) return;
        auto key = Lower(Trim(std::wstring_view(line).substr(0, equals)));
        auto value = line.substr(equals + 1);
        if (key != L"icon" && key != L"description") {
            Wh_Log(L"Unknown INI property '%s'; skipped", key.c_str());
            return;
        }
        ApplyProperty(configuration_, extensions_, key, std::move(value), base_);
    }

    Configuration Finish() { return std::move(configuration_); }

private:
    Configuration configuration_;
    std::filesystem::path base_;
    std::vector<std::wstring> extensions_;
};

bool ReadIni(const std::wstring& path, Configuration& configuration) {
    IniLineReader reader(path);
    IniParser parser(std::filesystem::path(path).parent_path());
    std::wstring line;
    for (;;) {
        auto result = reader.NextLine(line);
        if (result == IniReadResult::Error) {
            Wh_Log(L"INI read or encoding error %lu; preset discarded", reader.Error());
            return false;
        }
        if (result == IniReadResult::End) break;
        parser.ParseLine(line);
    }
    // Only a completely read and decoded preset can reach the published snapshot.
    configuration = parser.Finish();
    return true;
}

bool LoadSettings() {
    Configuration configuration;
    if (Wh_GetIntSetting(L"useIni") && (g_enableIcons || g_enableDescriptions)) {
        auto setting = WindhawkUtils::StringSetting::make(L"iniPath");
        auto path = Expand(Trim(setting.get()));
        if (!path || !std::filesystem::path(*path).is_absolute() || !ReadIni(*path, configuration))
            Wh_Log(L"Cannot read INI preset; using Windhawk settings only (absolute path, UTF-8/UTF-16 LE)");
    }
    for (int i = 0; ; ++i) {
        auto extensionSetting = WindhawkUtils::StringSetting::make(L"properties[%d].extensions", i);
        auto iconSetting = WindhawkUtils::StringSetting::make(L"properties[%d].icon", i);
        auto descriptionSetting = WindhawkUtils::StringSetting::make(L"properties[%d].description", i);
        auto keys = Trim(extensionSetting.get());
        auto icon = Trim(iconSetting.get());
        auto description = Trim(descriptionSetting.get());
        if (keys.empty() && icon.empty() && description.empty()) break;
        auto extensions = ParseExtensions(keys);
        ApplyProperty(configuration, extensions, L"icon", std::move(icon), {});
        ApplyProperty(configuration, extensions, L"description", std::move(description), {});
    }
    auto previous = std::atomic_load(&g_configuration);
    bool changed = configuration != *previous;
    // Publish both maps together. Even unchanged saves invalidate icon validation
    // so a repaired or replaced resource can be retried without changing its path.
    std::atomic_store(&g_configuration, std::make_shared<const Configuration>(std::move(configuration)));
    Wh_Log(L"Loaded %zu icon and %zu description mappings",
           GetIcons()->size(), GetTypes()->size());
    return changed;
}


namespace Icons {
std::optional<IconLocation> LookupPath(PCWSTR path) {
    if (!path) return std::nullopt;
    auto icons = GetIcons();
    if (icons->empty()) return std::nullopt;
    auto it = icons->find(Lower(PathFindExtensionW(path)));
    if (it == icons->end()) return std::nullopt;
    for (auto candidate = it->second.rbegin(); candidate != it->second.rend(); ++candidate) {
        std::call_once(candidate->validation->once, [&] { candidate->validation->usable = CanExtract(*candidate); });
        if (candidate->validation->usable) return *candidate;
    }
    return std::nullopt;
}

std::optional<IconLocation> LookupItem(IShellFolder* folder, PCUITEMID_CHILD pidl) {
    if (!pidl || !pidl->mkid.cb || GetIcons()->empty()) {
        return std::nullopt;
    }
    STRRET name{};
    if (FAILED(folder->GetDisplayNameOf(pidl,
                static_cast<SHGDNF>(SHGDN_FORPARSING | SHGDN_INFOLDER), &name))) {
        return std::nullopt;
    }
    PWSTR path = nullptr;
    if (FAILED(StrRetToStrW(&name, pidl, &path))) return std::nullopt;
    auto icon = LookupPath(path);
    CoTaskMemFree(path);
    // Do not ask filesystem/cloud providers for attributes of unmapped items.
    if (!icon) return std::nullopt;
    SFGAOF attributes = SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_STREAM;
    if (FAILED(folder->GetAttributesOf(1, &pidl, &attributes)) ||
        !(attributes & SFGAO_FILESYSTEM) ||
        ((attributes & SFGAO_FOLDER) && !(attributes & SFGAO_STREAM))) return std::nullopt;
    return icon;
}

bool IsIconIID(REFIID iid) {
    return IsEqualIID(iid, IID_IExtractIconW) || IsEqualIID(iid, IID_IExtractIconA);
}

HRESULT CreateExtractor(const IconLocation& icon, REFIID iid, void** result) {
    ComPtr<IDefaultExtractIconInit> init;
    HRESULT hr = SHCreateDefaultExtractIcon(IID_PPV_ARGS(&init));
    if (FAILED(hr)) return hr;
    // System-owned COM object: no mod-owned vtable survives mod unloading.
    // A filename/index pair is already a stable cache key. Do not mark every
    // file as a per-instance icon: that can create redundant cache identities.
    if (FAILED(hr = init->SetFlags(0)) ||
        FAILED(hr = init->SetNormalIcon(icon.path.c_str(), icon.index)) ||
        FAILED(hr = init->SetOpenIcon(icon.path.c_str(), icon.index)) ||
        FAILED(hr = init->SetDefaultIcon(icon.path.c_str(), icon.index)) ||
        FAILED(hr = init->SetShortcutIcon(icon.path.c_str(), icon.index))) return hr;
    return init->QueryInterface(iid, result);
}

using GetUIObjectOf_t = HRESULT (STDMETHODCALLTYPE*)(
    IShellFolder*, HWND, UINT, PCUITEMID_CHILD_ARRAY, REFIID, UINT*, void**);
using GetIconOf_t = HRESULT (STDMETHODCALLTYPE*)(IShellIcon*, PCUITEMID_CHILD, UINT, int*);
using GetThumbnailHandler_t = HRESULT (STDMETHODCALLTYPE*)(
    IThumbnailHandlerFactory*, PCUITEMID_CHILD, IBindCtx*, REFIID, void**);
GetUIObjectOf_t g_getUIObjectOf = nullptr;
GetIconOf_t g_getIconOf = nullptr;
GetThumbnailHandler_t g_getThumbnailHandler = nullptr;
decltype(&SHCreateFileExtractIconW) g_createFileExtractIcon = nullptr;

HRESULT STDMETHODCALLTYPE GetUIObjectOfHook(IShellFolder* folder, HWND owner,
    UINT count, PCUITEMID_CHILD_ARRAY items, REFIID iid, UINT* reserved, void** result) {
    if (!g_inHook && result && count == 1 && items && IsIconIID(iid)) {
        HookGuard guard;
        if (auto icon = LookupItem(folder, items[0])) {
            if (SUCCEEDED(CreateExtractor(*icon, iid, result))) return S_OK;
        }
    }
    return g_getUIObjectOf(folder, owner, count, items, iid, reserved, result);
}

HRESULT STDMETHODCALLTYPE GetIconOfHook(IShellIcon* shellIcon, PCUITEMID_CHILD item,
                                       UINT flags, int* result) {
    if (!g_inHook && item && result && !GetIcons()->empty()) {
        HookGuard guard;
        ComPtr<IShellFolder> folder;
        // 'this' is the IShellIcon subobject. QI avoids version-dependent offsets.
        if (SUCCEEDED(shellIcon->QueryInterface(IID_PPV_ARGS(&folder)))) {
            if (auto icon = LookupItem(folder.Get(), item)) {
                int index = Shell_GetCachedImageIndexW(icon->path.c_str(), icon->index, 0);
                if (index >= 0) {
                    *result = index;
                    return S_OK;
                }
            }
        }
    }
    return g_getIconOf(shellIcon, item, flags, result);
}

HRESULT WINAPI CreateFileExtractIconHook(PCWSTR file, DWORD attributes,
                                        REFIID iid, void** result) {
    if (!g_inHook && result && !(attributes & FILE_ATTRIBUTE_DIRECTORY) && IsIconIID(iid)) {
        HookGuard guard;
        if (auto icon = LookupPath(file)) {
            if (SUCCEEDED(CreateExtractor(*icon, iid, result))) return S_OK;
        }
    }
    return g_createFileExtractIcon(file, attributes, iid, result);
}

HRESULT STDMETHODCALLTYPE GetThumbnailHandlerHook(IThumbnailHandlerFactory* factory,
    PCUITEMID_CHILD item, IBindCtx* bindCtx, REFIID iid, void** result) {
    // Despite its name, this factory also serves IExtractIcon requests on modern
    // Windows (including SHGetFileInfo and IShellItemImageFactory icon fallback).
    // Never substitute IThumbnailProvider, IExtractImage, or IUnknown requests.
    if (!g_inHook && result && IsIconIID(iid)) {
        HookGuard guard;
        ComPtr<IShellFolder> folder;
        if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&folder)))) {
            if (auto icon = LookupItem(folder.Get(), item)) {
                if (SUCCEEDED(CreateExtractor(*icon, iid, result))) return S_OK;
            }
        }
    }
    return g_getThumbnailHandler(factory, item, bindCtx, iid, result);
}

} // namespace Icons

namespace Descriptions {
std::optional<std::wstring> LookupExtension(PCWSTR ext) {
    if (!ext || ext[0] != L'.') return std::nullopt;
    auto types = GetTypes();
    auto it = types->find(Lower(ext));
    return it == types->end() ? std::nullopt : std::optional(it->second);
}

std::optional<std::wstring> LookupPath(PCWSTR path) {
    return path ? LookupExtension(PathFindExtensionW(path)) : std::nullopt;
}

std::optional<std::wstring> LookupItem(IShellFolder* folder, PCUITEMID_CHILD item) {
    if (!item || !item->mkid.cb || GetTypes()->empty()) return std::nullopt;
    STRRET name{};
    if (FAILED(folder->GetDisplayNameOf(item,
        static_cast<SHGDNF>(SHGDN_FORPARSING | SHGDN_INFOLDER), &name))) return std::nullopt;
    PWSTR path = nullptr;
    if (FAILED(StrRetToStrW(&name, item, &path))) return std::nullopt;
    auto value = LookupPath(path);
    CoTaskMemFree(path);
    if (!value) return std::nullopt;
    SFGAOF attributes = SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_STREAM;
    if (FAILED(folder->GetAttributesOf(1, &item, &attributes)) ||
        !(attributes & SFGAO_FILESYSTEM) ||
        ((attributes & SFGAO_FOLDER) && !(attributes & SFGAO_STREAM))) return std::nullopt;
    return value;
}

std::optional<std::wstring> LookupShellItem(IShellItem* item) {
    if (GetTypes()->empty()) return std::nullopt;
    PWSTR path = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) return std::nullopt;
    auto value = LookupPath(path);
    CoTaskMemFree(path);
    if (!value) return std::nullopt;
    SFGAOF attributes;
    if (FAILED(item->GetAttributes(SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_STREAM, &attributes)) ||
        !(attributes & SFGAO_FILESYSTEM) ||
        ((attributes & SFGAO_FOLDER) && !(attributes & SFGAO_STREAM))) return std::nullopt;
    return value;
}

bool IsTypeKey(REFPROPERTYKEY key) { return IsEqualPropertyKey(key, PKEY_ItemTypeText); }
HRESULT CopyString(const std::wstring& text, PWSTR* result) {
    *result = static_cast<PWSTR>(CoTaskMemAlloc((text.size() + 1) * sizeof(WCHAR)));
    if (!*result) return E_OUTOFMEMORY;
    std::memcpy(*result, text.c_str(), (text.size() + 1) * sizeof(WCHAR));
    return S_OK;
}

using DetailsEx_t = HRESULT (STDMETHODCALLTYPE*)(IShellFolder2*, PCUITEMID_CHILD, const SHCOLUMNID*, VARIANT*);
using DetailsOf_t = HRESULT (STDMETHODCALLTYPE*)(IShellFolder2*, PCUITEMID_CHILD, UINT, SHELLDETAILS*);
using ItemString_t = HRESULT (STDMETHODCALLTYPE*)(IShellItem2*, REFPROPERTYKEY, PWSTR*);
using ItemProperty_t = HRESULT (STDMETHODCALLTYPE*)(IShellItem2*, REFPROPERTYKEY, PROPVARIANT*);
using TypeFormat_t = HRESULT (STDMETHODCALLTYPE*)(IPropertyDescription*, REFPROPVARIANT,
                                                PROPDESC_FORMAT_FLAGS, PWSTR*);
using StoreValue_t = HRESULT (STDMETHODCALLTYPE*)(IPropertyStore*, REFPROPERTYKEY, PROPVARIANT*);
using CacheValue_t = HRESULT (STDMETHODCALLTYPE*)(IPropertyStoreCache*, REFPROPERTYKEY, PROPVARIANT*, PSC_STATE*);
DetailsEx_t g_detailsEx;
DetailsOf_t g_detailsOf;
ItemString_t g_itemString;
ItemProperty_t g_itemProperty;
TypeFormat_t g_typeFormat;
std::array<StoreValue_t, 2> g_storeValues{};
CacheValue_t g_cacheValue;
decltype(&AssocQueryStringW) g_assocW;
decltype(&AssocQueryStringA) g_assocA;
using NewMenuQuery_t = HRESULT (STDMETHODCALLTYPE*)(IContextMenu*, HMENU, UINT, UINT, UINT, UINT);
using NewMenuMessage_t = HRESULT (STDMETHODCALLTYPE*)(IContextMenu2*, UINT, WPARAM, LPARAM);
using NewMenuMessage2_t = HRESULT (STDMETHODCALLTYPE*)(IContextMenu3*, UINT, WPARAM, LPARAM, LRESULT*);
NewMenuQuery_t g_newMenuQuery;
NewMenuMessage_t g_newMenuMessage;
NewMenuMessage2_t g_newMenuMessage2;

struct SavedMenuText { std::wstring original, replacement; };
struct NewMenuContext {
    IContextMenu* owner; // Identity only. Never retained or dereferenced from this cache.
    UINT first, count;
    std::mutex mutex;
    std::unordered_map<UINT, SavedMenuText> labels;
    NewMenuContext(IContextMenu* object, UINT start, UINT length) : owner(object), first(start), count(length) {}
};
std::mutex g_newMenusMutex;
std::unordered_map<HMENU, std::shared_ptr<NewMenuContext>> g_newMenus;
thread_local bool g_inNewMenu = false;

std::optional<std::wstring> ReadMenuText(HMENU menu, UINT position) {
    MENUITEMINFOW info{}; info.cbSize = sizeof(info);
    info.fMask = MIIM_STRING;
    if (!GetMenuItemInfoW(menu, position, TRUE, &info) || info.cch > 32767) return std::nullopt;
    std::wstring value(info.cch + 1, L'\0');
    info.dwTypeData = value.data();
    info.cch = static_cast<UINT>(value.size());
    if (!GetMenuItemInfoW(menu, position, TRUE, &info)) return std::nullopt;
    value.resize(info.cch);
    return value;
}
std::wstring EscapeMenuText(std::wstring_view text) {
    std::wstring result;
    for (WCHAR c : text) { result += c; if (c == L'&') result += c; }
    return result;
}

HRESULT STDMETHODCALLTYPE NewMenuQueryHook(IContextMenu* self, HMENU menu, UINT position,
                                           UINT first, UINT last, UINT flags) {
    HRESULT hr = g_newMenuQuery(self, menu, position, first, last, flags);
    UINT count = HRESULT_CODE(hr);
    MENUITEMINFOW info{}; info.cbSize = sizeof(info);
    info.fMask = MIIM_SUBMENU | MIIM_ID;
    if (SUCCEEDED(hr) && count && GetMenuItemInfoW(menu, position, TRUE, &info) &&
        info.wID == first && info.hSubMenu) {
        auto context = std::make_shared<NewMenuContext>(self, first, count);
        std::lock_guard lock(g_newMenusMutex);
        // Keep only bounded identity/label metadata, with no COM references,
        // proxies, Release detours, or changes to native menu item data.
        // A QueryContextMenu call always replaces metadata for a reused HMENU.
        for (auto it = g_newMenus.begin(); it != g_newMenus.end();) {
            if (!IsMenu(it->first)) it = g_newMenus.erase(it); else ++it;
        }
        if (g_newMenus.size() < 256 || g_newMenus.contains(info.hSubMenu))
            g_newMenus[info.hSubMenu] = std::move(context);
    }
    return hr;
}

void ApplyNewMenuDescriptions(IContextMenu* self, HMENU menu) {
    if (g_inNewMenu) return;
    std::shared_ptr<NewMenuContext> context;
    {
        std::lock_guard lock(g_newMenusMutex);
        auto it = g_newMenus.find(menu);
        if (it == g_newMenus.end() || it->second->owner != self) return;
        context = it->second;
    }
    std::unique_lock lock(context->mutex, std::try_to_lock);
    if (!lock.owns_lock()) return;
    struct Scope { Scope() { g_inNewMenu = true; } ~Scope() { g_inNewMenu = false; } } scope;
    auto types = GetTypes();
    if (types->empty() && context->labels.empty()) return;
    int count = GetMenuItemCount(menu);
    if (count < 0 || count > 4096) return;
    for (int position = 0; position < count; ++position) {
        MENUITEMINFOW info{}; info.cbSize = sizeof(info);
        info.fMask = MIIM_ID | MIIM_FTYPE | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(menu, position, TRUE, &info) || info.hSubMenu ||
            (info.fType & (MFT_SEPARATOR | MFT_OWNERDRAW)) || info.wID < context->first ||
            info.wID - context->first >= context->count) continue;
        WCHAR verb[1024]{};
        // GetCommandString takes an offset, not the absolute menu command ID.
        // Never query separators/out-of-range IDs: native handlers can reject
        // them unsafely. File-creation verbs identify their exact extension.
        if (FAILED(self->GetCommandString(info.wID - context->first, GCS_VERBW, nullptr,
                         reinterpret_cast<LPSTR>(verb), ARRAYSIZE(verb)))) continue;
        verb[ARRAYSIZE(verb) - 1] = L'\0';
        auto ext = Lower(verb);
        if (!ValidExtension(ext)) continue; // Keep Folder/Shortcut special verbs intact.
        auto current = ReadMenuText(menu, position);
        if (!current) continue;
        auto saved = context->labels.find(info.wID);
        auto original = saved != context->labels.end() && *current == saved->second.replacement ?
            saved->second.original : *current;
        auto mapping = types->find(ext);
        // Menu labels have a much smaller practical limit than INI contents.
        // Skip oversized definitions here without affecting property getters.
        if (mapping != types->end() && mapping->second.size() > 16383) continue;
        auto replacement = mapping == types->end() ? original : EscapeMenuText(mapping->second);
        if (replacement == *current) continue;
        MENUITEMINFOW update{}; update.cbSize = sizeof(update);
        update.fMask = MIIM_STRING;
        update.dwTypeData = replacement.data();
        if (SetMenuItemInfoW(menu, position, TRUE, &update)) {
            if (mapping == types->end()) context->labels.erase(info.wID);
            else context->labels[info.wID] = {std::move(original), std::move(replacement)};
        }
    }
}
HRESULT STDMETHODCALLTYPE NewMenuMessageHook(IContextMenu2* self, UINT message, WPARAM wParam, LPARAM lParam) {
    HRESULT hr = g_newMenuMessage(self, message, wParam, lParam);
    if (SUCCEEDED(hr) && message == WM_INITMENUPOPUP)
        ApplyNewMenuDescriptions(self, reinterpret_cast<HMENU>(wParam));
    return hr;
}
HRESULT STDMETHODCALLTYPE NewMenuMessage2Hook(IContextMenu3* self, UINT message, WPARAM wParam,
                                             LPARAM lParam, LRESULT* result) {
    HRESULT hr = g_newMenuMessage2(self, message, wParam, lParam, result);
    if (SUCCEEDED(hr) && message == WM_INITMENUPOPUP)
        ApplyNewMenuDescriptions(self, reinterpret_cast<HMENU>(wParam));
    return hr;
}
decltype(&SHGetFileInfoW) g_infoW;
decltype(&SHGetFileInfoA) g_infoA;

HRESULT STDMETHODCALLTYPE DetailsExHook(IShellFolder2* folder, PCUITEMID_CHILD item,
                                       const SHCOLUMNID* key, VARIANT* result) {
    if (!g_inHook && key && result && IsTypeKey(*key)) {
        HookGuard guard;
        if (auto value = LookupItem(folder, item)) {
            BSTR text = SysAllocStringLen(value->data(), static_cast<UINT>(value->size()));
            if (!text) return E_OUTOFMEMORY;
            VariantInit(result); result->vt = VT_BSTR; result->bstrVal = text;
            return S_OK;
        }
    }
    return g_detailsEx(folder, item, key, result);
}

HRESULT STDMETHODCALLTYPE DetailsOfHook(IShellFolder2* folder, PCUITEMID_CHILD item,
                                       UINT column, SHELLDETAILS* result) {
    if (!g_inHook && item && result) {
        HookGuard guard;
        SHCOLUMNID key;
        // The legacy Type column maps to ItemType (the extension), but its
        // formatted display text is the friendly description. Only change the
        // formatted column; raw System.ItemType properties retain the extension.
        if (SUCCEEDED(folder->MapColumnToSCID(column, &key)) &&
            (IsTypeKey(key) || IsEqualPropertyKey(key, PKEY_ItemType))) {
            if (auto value = LookupItem(folder, item)) {
                PWSTR text;
                HRESULT hr = CopyString(*value, &text);
                if (FAILED(hr)) return hr;
                result->fmt = LVCFMT_LEFT; result->cxChar = 20;
                result->str.uType = STRRET_WSTR; result->str.pOleStr = text;
                return S_OK;
            }
        }
    }
    return g_detailsOf(folder, item, column, result);
}

HRESULT STDMETHODCALLTYPE ItemStringHook(IShellItem2* item, REFPROPERTYKEY key, PWSTR* result) {
    if (!g_inHook && result && IsTypeKey(key)) {
        HookGuard guard;
        if (auto value = LookupShellItem(item)) return CopyString(*value, result);
    }
    return g_itemString(item, key, result);
}
HRESULT STDMETHODCALLTYPE ItemPropertyHook(IShellItem2* item, REFPROPERTYKEY key, PROPVARIANT* result) {
    if (!g_inHook && result && IsTypeKey(key)) {
        HookGuard guard;
        if (auto value = LookupShellItem(item)) return InitPropVariantFromString(value->c_str(), result);
    }
    return g_itemProperty(item, key, result);
}

HRESULT STDMETHODCALLTYPE TypeFormatHook(IPropertyDescription* description, REFPROPVARIANT raw,
                                         PROPDESC_FORMAT_FLAGS flags, PWSTR* result) {
    // Reject unrelated values before loading a snapshot, folding case, or
    // looking up mappings. Raw ItemType itself must remain the extension.
    if (!g_inHook && result && (raw.vt == VT_LPWSTR || raw.vt == VT_BSTR)) {
        PCWSTR extension = raw.vt == VT_LPWSTR ? raw.pwszVal : raw.bstrVal;
        if (extension && extension[0] == L'.') {
            HookGuard guard;
            PROPERTYKEY key{};
            if (SUCCEEDED(description->GetPropertyKey(&key)) && IsEqualPropertyKey(key, PKEY_ItemType)) {
                if (auto value = LookupExtension(extension)) {
                    *result = nullptr;
                    PROPVARIANT text;
                    PropVariantInit(&text);
                    HRESULT hr = InitPropVariantFromString(value->c_str(), &text);
                    if (SUCCEEDED(hr))
                        hr = PSFormatForDisplayAlloc(PKEY_ItemTypeText, text, flags, result);
                    PropVariantClear(&text);
                    return hr;
                }
            }
        }
    }
    return g_typeFormat(description, raw, flags, result);
}

HRESULT OverrideStoredType(IPropertyStore* store, PROPVARIANT* result, HRESULT status) {
    // Derive the extension from this same store, never from the old display
    // text (which may be shared by many UWP-associated extensions).
    // The caller holds HookGuard, so the raw lookup cannot recursively override.
    if (FAILED(status) || (result->vt != VT_LPWSTR && result->vt != VT_BSTR)) return status;
    PROPVARIANT raw;
    PropVariantInit(&raw);
    HRESULT hr = store->GetValue(PKEY_ItemType, &raw);
    std::optional<std::wstring> value;
    if (SUCCEEDED(hr)) {
        if (raw.vt == VT_LPWSTR) value = LookupExtension(raw.pwszVal);
        else if (raw.vt == VT_BSTR) value = LookupExtension(raw.bstrVal);
    }
    PropVariantClear(&raw);
    if (!value) return status;
    PropVariantClear(result);
    hr = InitPropVariantFromString(value->c_str(), result);
    return FAILED(hr) ? hr : status;
}

template<size_t slot>
HRESULT STDMETHODCALLTYPE StoreValueHook(IPropertyStore* store, REFPROPERTYKEY key, PROPVARIANT* result) {
    if (!g_inHook && result && IsTypeKey(key) && !GetTypes()->empty()) {
        HookGuard guard;
        HRESULT hr = g_storeValues[slot](store, key, result);
        return OverrideStoredType(store, result, hr);
    }
    return g_storeValues[slot](store, key, result);
}

HRESULT STDMETHODCALLTYPE CacheValueHook(IPropertyStoreCache* store, REFPROPERTYKEY key,
                                         PROPVARIANT* result, PSC_STATE* state) {
    if (!g_inHook && result && state && IsTypeKey(key) && !GetTypes()->empty()) {
        HookGuard guard;
        HRESULT hr = g_cacheValue(store, key, result, state);
        // Preserve the cache's state and HRESULT. This modifies only the returned
        // copy; no SetValue/SetState calls or persistent cache mutations occur.
        return OverrideStoredType(store, result, hr);
    }
    return g_cacheValue(store, key, result, state);
}

std::wstring FromAnsi(PCSTR input) {
    if (!input) return {};
    int n = MultiByteToWideChar(CP_ACP, 0, input, -1, nullptr, 0);
    if (!n) return {};
    std::wstring text(n, L'\0');
    MultiByteToWideChar(CP_ACP, 0, input, -1, text.data(), n);
    text.resize(n - 1); return text;
}
std::string ToAnsi(const std::wstring& input) {
    int n = WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (!n) return {};
    std::string text(n, '\0');
    WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, text.data(), n, nullptr, nullptr);
    text.resize(n - 1); return text;
}
template<typename String, typename Char>
HRESULT AssocResult(ASSOCF flags, const String& text, Char* result, DWORD* size) {
    if (!size) return E_POINTER;
    DWORD capacity = *size;
    *size = static_cast<DWORD>(text.size() + 1);
    if (!result) return S_FALSE;
    if (capacity < *size) {
        if (flags & ASSOCF_NOTRUNCATE) {
            if (capacity) result[0] = 0;
            return E_POINTER;
        }
        if (capacity) {
            auto copied = static_cast<size_t>(capacity - 1);
            std::memcpy(result, text.data(), copied * sizeof(Char));
            result[copied] = 0;
        }
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    std::memcpy(result, text.c_str(), *size * sizeof(Char));
    return S_OK;
}
HRESULT WINAPI AssocWHook(ASSOCF flags, ASSOCSTR kind, PCWSTR assoc, PCWSTR extra,
                         PWSTR result, DWORD* size) {
    if (!g_inHook && kind == ASSOCSTR_FRIENDLYDOCNAME && !(flags & ASSOCF_OPEN_BYEXENAME)) {
        HookGuard guard;
        if (auto value = LookupExtension(assoc)) return AssocResult(flags, *value, result, size);
    }
    return g_assocW(flags, kind, assoc, extra, result, size);
}
HRESULT WINAPI AssocAHook(ASSOCF flags, ASSOCSTR kind, PCSTR assoc, PCSTR extra,
                         PSTR result, DWORD* size) {
    if (!g_inHook && kind == ASSOCSTR_FRIENDLYDOCNAME && !(flags & ASSOCF_OPEN_BYEXENAME)) {
        HookGuard guard;
        if (auto value = LookupExtension(FromAnsi(assoc).c_str())) return AssocResult(flags, ToAnsi(*value), result, size);
    }
    return g_assocA(flags, kind, assoc, extra, result, size);
}

std::optional<std::wstring> LookupInfo(PCWSTR input, DWORD attributes, UINT flags) {
    if (GetTypes()->empty()) return std::nullopt;
    PWSTR allocated = nullptr;
    PCWSTR path = input;
    if (flags & SHGFI_PIDL) {
        if (!input || FAILED(SHGetNameFromIDList(reinterpret_cast<PCIDLIST_ABSOLUTE>(input),
                                               SIGDN_FILESYSPATH, &allocated))) return std::nullopt;
        path = allocated;
    }
    auto value = LookupPath(path);
    if (value) {
        DWORD actual = (flags & SHGFI_USEFILEATTRIBUTES) ? attributes :
                       (path ? GetFileAttributesW(path) : INVALID_FILE_ATTRIBUTES);
        if (actual == INVALID_FILE_ATTRIBUTES || (actual & FILE_ATTRIBUTE_DIRECTORY)) value.reset();
    }
    CoTaskMemFree(allocated);
    return value;
}
DWORD_PTR WINAPI InfoWHook(PCWSTR path, DWORD attr, SHFILEINFOW* result, UINT size, UINT flags) {
    DWORD_PTR status = g_infoW(path, attr, result, size, flags);
    if (status && !g_inHook && result && size >= sizeof(*result) && (flags & SHGFI_TYPENAME)) {
        HookGuard guard;
        if (auto value = LookupInfo(path, attr, flags)) {
            lstrcpynW(result->szTypeName, value->c_str(), ARRAYSIZE(result->szTypeName));
        }
    }
    return status;
}
DWORD_PTR WINAPI InfoAHook(PCSTR path, DWORD attr, SHFILEINFOA* result, UINT size, UINT flags) {
    DWORD_PTR status = g_infoA(path, attr, result, size, flags);
    if (status && !g_inHook && result && size >= sizeof(*result) && (flags & SHGFI_TYPENAME)) {
        HookGuard guard;
        auto wide = (flags & SHGFI_PIDL) ? std::wstring() : FromAnsi(path);
        auto input = (flags & SHGFI_PIDL) ? reinterpret_cast<PCWSTR>(path) : wide.c_str();
        if (auto value = LookupInfo(input, attr, flags)) {
            auto ansi = ToAnsi(*value);
            lstrcpynA(result->szTypeName, ansi.c_str(), ARRAYSIZE(result->szTypeName));
        }
    }
    return status;
}

} // namespace Descriptions

struct ModuleReferences { std::vector<HMODULE> modules; } g_hookModules;

HMODULE LoadHookModule(PCWSTR name) {
    HMODULE module = LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module) g_hookModules.modules.push_back(module);
    return module;
}
void ReleaseHookModules() {
    // Windhawk removes detours before this callback. Never FreeLibrary from a
    // static destructor under the loader lock. On partial setup failure, keep
    // the bounded module references until process exit while hooks roll back.
    for (HMODULE module : g_hookModules.modules) FreeLibrary(module);
    g_hookModules.modules.clear();
}
bool IsTaskbarOwner() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD owner = 0;
    return taskbar && GetWindowThreadProcessId(taskbar, &owner) && owner == GetCurrentProcessId();
}
void NotifyShell() {
    if (IsTaskbarOwner()) SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}

// Windhawk unloads the DLL during a requested reload. Only the taskbar owner
// writes/consumes this one-shot request in Windhawk's mod storage. Session keys
// keep simultaneous logins separate; process creation time rejects PID reuse
// and stale requests after Explorer or Windows restarts.
struct RefreshRequest {
    DWORD processId;
    FILETIME creationTime;
};
std::optional<std::pair<std::wstring, RefreshRequest>> CurrentRefreshRequest() {
    if (!IsTaskbarOwner()) return std::nullopt;
    RefreshRequest request{};
    request.processId = GetCurrentProcessId();
    DWORD session = 0;
    FILETIME exit{}, kernel{}, user{};
    if (!ProcessIdToSessionId(request.processId, &session) ||
        !GetProcessTimes(GetCurrentProcess(), &request.creationTime, &exit, &kernel, &user)) return std::nullopt;
    return std::pair{L"pendingExplorerRefresh." + std::to_wstring(session), request};
}
void QueueRefreshAfterReload() {
    auto current = CurrentRefreshRequest();
    if (current && !Wh_SetBinaryValue(current->first.c_str(), &current->second, sizeof(RefreshRequest)))
        Wh_Log(L"Could not defer the requested Explorer refresh; reopen affected windows");
}
bool TakeRefreshAfterReload() {
    auto current = CurrentRefreshRequest();
    if (!current) return false;
    RefreshRequest pending{};
    if (Wh_GetBinaryValue(current->first.c_str(), &pending, sizeof(pending)) != sizeof(pending) ||
        pending.processId != current->second.processId ||
        CompareFileTime(&pending.creationTime, &current->second.creationTime) != 0) return false;
    // Consume before hook initialization: failed initialization must not leave
    // a request that a later unrelated load could accidentally replay.
    if (!Wh_DeleteValue(current->first.c_str())) {
        Wh_Log(L"Could not consume the pending Explorer refresh; skipping it");
        return false;
    }
    return true;
}

bool InstallHooks() {
    if (!g_activeFeatures.icons && !g_activeFeatures.descriptions) return false;
    HMODULE windowsStorage = LoadHookModule(L"windows.storage.dll");
    if (!windowsStorage) return false;
    using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;
    std::vector<COMBINED_SH> windowsStorageHooks;
    if (g_activeFeatures.icons) {
        // windows.storage.dll
        WindhawkUtils::SYMBOL_HOOK windowsStorageIconHooks[] = {
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CFSFolder::GetUIObjectOf(struct HWND__ *,unsigned int,struct _ITEMID_CHILD const __unaligned * const *,struct _GUID const &,unsigned int *,void * *)"
#else
                    L"public: virtual long __stdcall CFSFolder::GetUIObjectOf(struct HWND__ *,unsigned int,struct _ITEMID_CHILD const * const *,struct _GUID const &,unsigned int *,void * *)"
#endif
                },
                &Icons::g_getUIObjectOf, Icons::GetUIObjectOfHook
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CFSFolder::GetIconOf(struct _ITEMID_CHILD const __unaligned *,unsigned int,int *)"
#else
                    L"public: virtual long __stdcall CFSFolder::GetIconOf(struct _ITEMID_CHILD const *,unsigned int,int *)"
#endif
                },
                &Icons::g_getIconOf, Icons::GetIconOfHook
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CFSFolder::GetThumbnailHandler(struct _ITEMID_CHILD const __unaligned *,struct IBindCtx *,struct _GUID const &,void * *)"
#else
                    L"public: virtual long __stdcall CFSFolder::GetThumbnailHandler(struct _ITEMID_CHILD const *,struct IBindCtx *,struct _GUID const &,void * *)"
#endif
                },
                &Icons::g_getThumbnailHandler, Icons::GetThumbnailHandlerHook
            },
        };
        for (auto& hook : windowsStorageIconHooks) windowsStorageHooks.push_back(std::move(hook));
    }
    if (g_activeFeatures.descriptions) {
        // windows.storage.dll
        WindhawkUtils::SYMBOL_HOOK windowsStorageDescriptionHooks[] = {
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *)"
#else
                    L"public: virtual long __stdcall CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const *,struct _tagpropertykey const *,struct tagVARIANT *)"
#endif
                },
                &Descriptions::g_detailsEx, Descriptions::DetailsExHook
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CFSFolder::GetDetailsOf(struct _ITEMID_CHILD const __unaligned *,unsigned int,struct _SHELLDETAILS *)"
#else
                    L"public: virtual long __stdcall CFSFolder::GetDetailsOf(struct _ITEMID_CHILD const *,unsigned int,struct _SHELLDETAILS *)"
#endif
                },
                &Descriptions::g_detailsOf, Descriptions::DetailsOfHook
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CShellItem::GetString(struct _tagpropertykey const &,unsigned short * *)"
#else
                    L"public: virtual long __stdcall CShellItem::GetString(struct _tagpropertykey const &,unsigned short * *)"
#endif
                },
                &Descriptions::g_itemString, Descriptions::ItemStringHook
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CShellItem::GetProperty(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#else
                    L"public: virtual long __stdcall CShellItem::GetProperty(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#endif
                },
                &Descriptions::g_itemProperty, Descriptions::ItemPropertyHook
            },
        };
        for (auto& hook : windowsStorageDescriptionHooks) windowsStorageHooks.push_back(std::move(hook));
    }
    if (!WindhawkUtils::HookSymbols(windowsStorage, windowsStorageHooks.data(), windowsStorageHooks.size())) return false;
    if (g_activeFeatures.icons && !WindhawkUtils::SetFunctionHook(
            SHCreateFileExtractIconW, Icons::CreateFileExtractIconHook, &Icons::g_createFileExtractIcon)) return false;
    if (!g_activeFeatures.descriptions) return true;
    HMODULE propsys = LoadHookModule(L"propsys.dll");
    HMODULE shell32 = LoadHookModule(L"shell32.dll");
    Descriptions::g_typeFormat = nullptr;
    Descriptions::g_storeValues.fill(nullptr);
    Descriptions::g_cacheValue = nullptr;
    Descriptions::g_newMenuQuery = nullptr;
    Descriptions::g_newMenuMessage = nullptr;
    Descriptions::g_newMenuMessage2 = nullptr;
    if (propsys) {
        Descriptions::TypeFormat_t formatTarget = nullptr;
        std::array<Descriptions::StoreValue_t, 2> storeTargets{};
        Descriptions::CacheValue_t cacheTarget = nullptr;
        const WindhawkUtils::SYMBOL_HOOK propsysDllHooks[] = {
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CPropertyDescription::FormatForDisplay(struct tagPROPVARIANT const &,enum PROPDESC_FORMAT_FLAGS,unsigned short * *)"
#else
                    L"public: virtual long __stdcall CPropertyDescription::FormatForDisplay(struct tagPROPVARIANT const &,enum PROPDESC_FORMAT_FLAGS,unsigned short * *)"
#endif
                },
                &formatTarget, nullptr, true
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CPropertyProvider::GetValue(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#else
                    L"public: virtual long __stdcall CPropertyProvider::GetValue(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#endif
                },
                &storeTargets[0], nullptr, true
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CMemPropStore::GetValue(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#else
                    L"public: virtual long __stdcall CMemPropStore::GetValue(struct _tagpropertykey const &,struct tagPROPVARIANT *)"
#endif
                },
                &storeTargets[1], nullptr, true
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CMemPropStore::GetValueAndState(struct _tagpropertykey const &,struct tagPROPVARIANT *,enum PSC_STATE *)"
#else
                    L"public: virtual long __stdcall CMemPropStore::GetValueAndState(struct _tagpropertykey const &,struct tagPROPVARIANT *,enum PSC_STATE *)"
#endif
                },
                &cacheTarget, nullptr, true
            },
        };
        // Resolve optional targets before installing any of this group. A
        // failed symbol read must not leave a partially installed optional group.
        if (WindhawkUtils::HookSymbols(propsys, propsysDllHooks, ARRAYSIZE(propsysDllHooks))) {
            if (formatTarget) {
                if (!WindhawkUtils::SetFunctionHook(formatTarget, Descriptions::TypeFormatHook,
                                                   &Descriptions::g_typeFormat)) return false;
            } else Wh_Log(L"Optional type formatter unavailable; affected views retain system descriptions");
            const std::array<Descriptions::StoreValue_t, 2> detours{
                Descriptions::StoreValueHook<0>, Descriptions::StoreValueHook<1>};
            for (size_t i = 0; i < storeTargets.size(); ++i) {
                if (!storeTargets[i]) { Wh_Log(L"Optional property-store getter %zu unavailable", i); continue; }
                bool duplicate = false;
                for (size_t j = 0; j < i; ++j) if (storeTargets[i] == storeTargets[j]) duplicate = true;
                if (!duplicate && !WindhawkUtils::SetFunctionHook(storeTargets[i], detours[i],
                                                                &Descriptions::g_storeValues[i])) return false;
            }
            if (cacheTarget) {
                if (!WindhawkUtils::SetFunctionHook(cacheTarget, Descriptions::CacheValueHook,
                                                   &Descriptions::g_cacheValue)) return false;
            } else Wh_Log(L"Optional property-cache getter unavailable");
        } else Wh_Log(L"Optional property formatting/store symbols could not be read; using core description hooks");
    } else Wh_Log(L"Optional property formatting/store module unavailable");
    if (shell32) {
        Descriptions::NewMenuQuery_t queryTarget = nullptr;
        Descriptions::NewMenuMessage_t messageTarget = nullptr;
        Descriptions::NewMenuMessage2_t message2Target = nullptr;
        const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CNewMenu::QueryContextMenu(struct HMENU__ *,unsigned int,unsigned int,unsigned int,unsigned int)"
#else
                    L"public: virtual long __stdcall CNewMenu::QueryContextMenu(struct HMENU__ *,unsigned int,unsigned int,unsigned int,unsigned int)"
#endif
                },
                &queryTarget, nullptr, true
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CNewMenu::HandleMenuMsg(unsigned int,unsigned __int64,__int64)"
#else
                    L"public: virtual long __stdcall CNewMenu::HandleMenuMsg(unsigned int,unsigned int,long)"
#endif
                },
                &messageTarget, nullptr, true
            },
            {
                {
#ifdef _WIN64
                    L"public: virtual long __cdecl CNewMenu::HandleMenuMsg2(unsigned int,unsigned __int64,__int64,__int64 *)"
#else
                    L"public: virtual long __stdcall CNewMenu::HandleMenuMsg2(unsigned int,unsigned int,long,long *)"
#endif
                },
                &message2Target, nullptr, true
            },
        };
        // These hooks cooperate: enable the New-menu group only when all
        // three targets are available, keeping its label tracking consistent.
        if (WindhawkUtils::HookSymbols(shell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks)) &&
            queryTarget && messageTarget && message2Target) {
            if (!WindhawkUtils::SetFunctionHook(queryTarget, Descriptions::NewMenuQueryHook,
                                               &Descriptions::g_newMenuQuery) ||
                !WindhawkUtils::SetFunctionHook(messageTarget, Descriptions::NewMenuMessageHook,
                                               &Descriptions::g_newMenuMessage) ||
                !WindhawkUtils::SetFunctionHook(message2Target, Descriptions::NewMenuMessage2Hook,
                                               &Descriptions::g_newMenuMessage2)) return false;
        } else Wh_Log(L"Optional New-menu symbols unavailable; New-menu labels retain system descriptions");
    } else Wh_Log(L"Optional New-menu module unavailable");
    return WindhawkUtils::SetFunctionHook(AssocQueryStringW, Descriptions::AssocWHook, &Descriptions::g_assocW) &&
           WindhawkUtils::SetFunctionHook(AssocQueryStringA, Descriptions::AssocAHook, &Descriptions::g_assocA) &&
           WindhawkUtils::SetFunctionHook(SHGetFileInfoW, Descriptions::InfoWHook, &Descriptions::g_infoW) &&
           WindhawkUtils::SetFunctionHook(SHGetFileInfoA, Descriptions::InfoAHook, &Descriptions::g_infoA);
}
} // namespace

BOOL Wh_ModInit() {
    g_emptyRefreshListener = false;
    g_refreshAfterInit = TakeRefreshAfterReload();
    bool refresh = Wh_GetIntSetting(L"refreshExplorer") != 0;
    g_refreshAfterInit = g_refreshAfterInit && refresh;
    g_enableIcons = Wh_GetIntSetting(L"enableIcons") != 0;
    g_enableDescriptions = Wh_GetIntSetting(L"enableDescriptions") != 0;
    LoadSettings();
    g_activeFeatures = GetActiveFeatures();
    if (!g_activeFeatures.icons && !g_activeFeatures.descriptions) {
        // An opt-in, hook-free listener is needed to observe the first rule.
        // Otherwise Windhawk retries initialization without a settings callback.
        if (refresh) { g_emptyRefreshListener = true; return TRUE; }
        Wh_Log(L"No active mappings; waiting for settings changes");
        return FALSE;
    }
    bool ok = InstallHooks();
    if (!ok) Wh_Log(L"Required Shell symbols or hooks unavailable; mod will not load");
    return ok;
}
void Wh_ModAfterInit() {
    if (std::exchange(g_refreshAfterInit, false) && Wh_GetIntSetting(L"refreshExplorer")) NotifyShell();
}
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    bool icons = Wh_GetIntSetting(L"enableIcons") != 0;
    bool descriptions = Wh_GetIntSetting(L"enableDescriptions") != 0;
    bool switchesChanged = icons != g_enableIcons || descriptions != g_enableDescriptions;
    g_enableIcons = icons;
    g_enableDescriptions = descriptions;
    bool changed = LoadSettings();
    auto active = GetActiveFeatures();
    bool empty = !active.icons && !active.descriptions;
    bool refresh = Wh_GetIntSetting(L"refreshExplorer") != 0;
    if (switchesChanged || active != g_activeFeatures || (g_emptyRefreshListener && !refresh)) {
        if (changed && refresh) {
            if (empty) NotifyShell(); // The published empty snapshot already passes through.
            else QueueRefreshAfterReload(); // New hooks must be active before refreshing.
        }
        *reload = TRUE;
        return TRUE;
    }
    if (changed && refresh) NotifyShell();
    return TRUE;
}
void Wh_ModUninit() {
    g_refreshAfterInit = false;
    g_emptyRefreshListener = false;
    g_activeFeatures = {};
    std::atomic_store(&g_configuration, std::make_shared<const Configuration>());
    { std::lock_guard lock(Descriptions::g_newMenusMutex); Descriptions::g_newMenus.clear(); }
    ReleaseHookModules();
}
