// ==WindhawkMod==
// @id              explorer-item-pins
// @name            Explorer Item Pins
// @description     Pin files and folders within their parent folder in File Explorer
// @description:ru-RU Закрепляет файлы и папки внутри открытой папки Проводника
// @description:es-ES Ancla archivos y carpetas dentro de su carpeta principal en el Explorador de archivos
// @description:de-DE Heftet Dateien und Ordner im Datei-Explorer in ihrem übergeordneten Ordner an
// @description:pt-BR Fixa arquivos e pastas na pasta que os contém no Explorador de Arquivos
// @description:zh-CN 在文件资源管理器中将文件和文件夹固定在各自的上级文件夹内
// @version         1.0.0
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lruntimeobject -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -lgdi32 -lcomctl32
// ==/WindhawkMod==
// Build: 23

// ==WindhawkModReadme==
/*
# Explorer Item Pins

**English** · [Русский](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Explorer_Item_Pins/README.ru.md)

Explorer Item Pins keeps selected files and folders at the top of their parent folder in Windows File Explorer. Each folder has its own pins.

## Features

| ![Dark theme with large icons and the Unpin command](https://raw.githubusercontent.com/CTravkin/DSN_Windhawk_Mods/main/Explorer_Item_Pins/Assets/Screenshots/Dark_Large_Icons.png) | ![Light theme with small icons, the Pinned group, and the Pin command](https://raw.githubusercontent.com/CTravkin/DSN_Windhawk_Mods/main/Explorer_Item_Pins/Assets/Screenshots/Light_Small_Icons.png) |
| :---: | :---: |

- **Two ways to pin.** Use the context menu or the pin button between **Share** and **Delete** on the command bar.
- **Sort and group.** Pins stay at the top in either sort direction. With grouping on, they appear in the first group, **Pinned**. Explorer's sorting still applies within the group.
- **Pin indicator.** A pushpin appears at the upper-right corner of the file or folder icon. It follows the icon size and works with thumbnails, lists, details, tiles and content view.
- **Interface languages.** English, Russian, Spanish, German, Brazilian Portuguese, and Simplified Chinese.

## Platforms

The mod requires [Windhawk](https://windhawk.net/) and Windows 11 x64 with the tabbed File Explorer. Tested on Windows 11 26H2 (build `26300.9974`) with Windhawk 1.7.3. Compatibility with other Explorer builds depends on available symbols and compatible internal interfaces.

## Data exchange

Pins are saved per Windows account in `%LOCALAPPDATA%\Explorer Item Pins\pins.json`. Replacing a file under the same name keeps its pin. A rename within the same folder can keep the pin when the mod can identify the original file by its file ID. Copying or moving an item to another folder does not carry its pin with it.

## Operations

### Installation

Install Explorer Item Pins from the Windhawk catalog. To build a local copy from source, follow these steps.

1. Open Windhawk and choose **Create a New Mod**.
2. Replace the contents of `mod.wh.cpp` with the complete [mod source](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Explorer_Item_Pins/Mod/explorer-item-pins.wh.cpp).
3. Use **Compile Mod** or `Ctrl+B` with the Windhawk sidebar open. Exit the editor and check that **Explorer Item Pins** is enabled on the Windhawk home page.

To update an existing local installation, edit that mod in Windhawk, replace its source and compile it again.

[Windhawk editing mode guide](https://github.com/ramensoftware/windhawk/wiki/Editing-mode-guidelines)

### Usage

1. Select a file or folder and choose **Pin in item pane** in its context menu, or click the pin on the command bar.
2. To remove the pin, select the item and choose **Unpin from item pane** or click the crossed-out pin on the command bar. The item returns to its usual position in the folder.

In Windhawk, open the mod's **Settings** to control these options independently. All three are enabled by default.

| Setting | Controls |
| --- | --- |
| Context menu command | Pin and unpin in the context menu |
| Command bar button | Pin and unpin on the File Explorer command bar |
| Pinned item indicator | The pushpin on pinned file and folder icons |

### Verification

Run the tests from this mod directory on Windows with PowerShell and the compiler bundled with Windhawk 1.7.3.

```powershell
pwsh -NoProfile -File .\Tests\Test-PinStore.ps1
pwsh -NoProfile -File .\Tests\Test-ExplorerCommands.ps1
```

The tests cover storage, concurrent access, pin operations, sorting, grouping, and indicator rendering in a native Explorer view hosted in the test process.

## Links

- [Explorer Item Pins on GitHub](https://github.com/CTravkin/DSN_Windhawk_Mods/tree/main/Explorer_Item_Pins)
- [DSN Windhawk Mods](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/README.md)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- contextMenu: true
  $name: Context menu command
  $name:ru-RU: Команда контекстного меню
  $name:es-ES: Comando del menú contextual
  $name:de-DE: Befehl im Kontextmenü
  $name:pt-BR: Comando do menu de contexto
  $name:zh-CN: 上下文菜单命令
- commandBar: true
  $name: Command bar button
  $name:ru-RU: Кнопка на панели команд
  $name:es-ES: Botón de la barra de comandos
  $name:de-DE: Schaltfläche in der Befehlsleiste
  $name:pt-BR: Botão na barra de comandos
  $name:zh-CN: 命令栏按钮
- pinIndicator: true
  $name: Pinned item indicator
  $name:ru-RU: Индикатор закрепленного объекта
  $name:es-ES: Indicador de elemento anclado
  $name:de-DE: Symbol für angeheftete Elemente
  $name:pt-BR: Indicador de item fixado
  $name:zh-CN: 固定项标记
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <roapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <wincodec.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
#include <wrl/client.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

#ifndef EXPLORER_ITEM_PINS_TEST
#include <inspectable.h>
#include <ocidl.h>
#include <shldisp.h>
#include <shdispid.h>
#include <xamlom.h>
#undef GetCurrentTime
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#endif

namespace explorer_item_pins {

namespace fs = std::filesystem;
using winrt::Windows::Data::Json::IJsonValue;
using winrt::Windows::Data::Json::IJsonObjectStatics;
using winrt::Windows::Data::Json::IJsonValueStatics;
using winrt::Windows::Data::Json::JsonArray;
using winrt::Windows::Data::Json::JsonObject;
using winrt::Windows::Data::Json::JsonValue;

struct Item {
    std::wstring name;
    std::wstring identity;
    bool operator==(const Item&) const = default;
};

namespace {

constexpr unsigned formatVersion = 1;
constexpr size_t maxStoreBytes = 16 * 1024 * 1024;

int CompareNames(std::wstring_view left, std::wstring_view right) {
    return CompareStringOrdinal(left.data(), static_cast<int>(left.size()),
                                right.data(), static_cast<int>(right.size()), TRUE);
}

struct FolderLess {
    bool operator()(const std::wstring& left, const std::wstring& right) const {
        return CompareNames(left, right) == CSTR_LESS_THAN;
    }
};

using Folders = std::map<std::wstring, std::vector<Item>, FolderLess>;
using FileHandle = std::unique_ptr<void, decltype(&CloseHandle)>;

class JsonApartment {
public:
    JsonApartment() : result_(RoInitialize(RO_INIT_MULTITHREADED)) {
        if (result_ != RPC_E_CHANGED_MODE) winrt::check_hresult(result_);
    }
    ~JsonApartment() {
        if (SUCCEEDED(result_)) RoUninitialize();
    }
    JsonApartment(const JsonApartment&) = delete;
    JsonApartment& operator=(const JsonApartment&) = delete;

private:
    HRESULT result_;
};

template <typename Class, typename Interface = winrt::Windows::Foundation::IActivationFactory>
auto JsonFactory() {
    // Keep activation factories within the lifetime of the calling apartment.
    auto factory = winrt::try_get_activation_factory<Class, Interface>();
    if (!factory) throw std::runtime_error("Cannot activate Windows JSON");
    return factory;
}

[[noreturn]] void FileError(DWORD code, const char* operation) {
    throw std::system_error(static_cast<int>(code), std::system_category(), operation);
}

std::wstring FolderKey(fs::path path) {
    if (!path.is_absolute() || path.native().find(L'\0') != std::wstring::npos) {
        throw std::invalid_argument("An absolute Windows folder path is required");
    }
    path = path.lexically_normal().make_preferred();
    if (path.has_relative_path() && !path.has_filename()) path = path.parent_path();
    return path.native();
}

void CheckItem(const Item& item) {
    if (item.name.empty() || item.name == L"." || item.name == L".." ||
        item.name.find_first_of(L"\\/:*?\"<>|") != std::wstring::npos ||
        item.name.find(L'\0') != std::wstring::npos ||
        item.identity.find(L'\0') != std::wstring::npos) {
        throw std::invalid_argument("A pin must have a valid relative item name");
    }
}

FileHandle OpenPinFile(const fs::path& path) {
    const HANDLE raw = CreateFileW(path.c_str(), GENERIC_READ,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (raw == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return FileHandle(nullptr, CloseHandle);
        FileError(error, "Open pin store");
    }
    return FileHandle(raw, CloseHandle);
}

std::string ReadFileBytes(HANDLE file) {
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size)) FileError(GetLastError(), "Size pin store");
    if (size.QuadPart <= 0 || size.QuadPart > static_cast<LONGLONG>(maxStoreBytes)) {
        throw std::runtime_error("Invalid pin store size");
    }
    std::string bytes(static_cast<size_t>(size.QuadPart), '\0');
    DWORD count = 0;
    if (!ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &count, nullptr)) {
        FileError(GetLastError(), "Read pin store");
    }
    if (count != bytes.size()) throw std::runtime_error("Incomplete pin store read");
    return bytes;
}

Folders ParseFolders(HANDLE file) {
    const auto bytes = ReadFileBytes(file);
    try {
        const JsonApartment apartment;
        const auto root = JsonFactory<JsonObject, IJsonObjectStatics>().Parse(winrt::to_hstring(bytes));
        if (root.GetNamedNumber(L"version") != formatVersion) {
            throw std::runtime_error("Unsupported pin store version");
        }
        Folders folders;
        for (const auto& value : root.GetNamedArray(L"folders")) {
            const auto folder = value.GetObject();
            const auto key = FolderKey(std::wstring(folder.GetNamedString(L"path")));
            const auto [position, inserted] = folders.try_emplace(key);
            if (!inserted) throw std::runtime_error("Duplicate folder in pin store");
            auto& pins = position->second;
            for (const auto& pinValue : folder.GetNamedArray(L"pins")) {
                const auto pin = pinValue.GetObject();
                Item item{std::wstring(pin.GetNamedString(L"name")),
                          std::wstring(pin.GetNamedString(L"identity"))};
                CheckItem(item);
                if (std::ranges::find(pins, item.name, &Item::name) != pins.end()) {
                    throw std::runtime_error("Duplicate item name in pin store");
                }
                pins.push_back(std::move(item));
            }
        }
        return folders;
    } catch (const winrt::hresult_error&) {
        throw std::runtime_error("Cannot parse pin store JSON");
    }
}

Folders ReadFolders(const fs::path& path) {
    const auto file = OpenPinFile(path);
    return file ? ParseFolders(file.get()) : Folders{};
}

std::string WriteJson(const Folders& folders) {
    try {
        const JsonApartment apartment;
        const auto objectFactory = JsonFactory<JsonObject>();
        const auto arrayFactory = JsonFactory<JsonArray>();
        const auto valueFactory = JsonFactory<JsonValue, IJsonValueStatics>();
        auto folderArray = arrayFactory.ActivateInstance<JsonArray>();
        for (const auto& [path, items] : folders) {
            auto pins = arrayFactory.ActivateInstance<JsonArray>();
            for (const auto& item : items) {
                auto pin = objectFactory.ActivateInstance<JsonObject>();
                pin.SetNamedValue(L"name", valueFactory.CreateStringValue(item.name));
                pin.SetNamedValue(L"identity", valueFactory.CreateStringValue(item.identity));
                pins.Append(pin.as<IJsonValue>());
            }
            auto folder = objectFactory.ActivateInstance<JsonObject>();
            folder.SetNamedValue(L"path", valueFactory.CreateStringValue(path));
            folder.SetNamedValue(L"pins", pins.as<IJsonValue>());
            folderArray.Append(folder.as<IJsonValue>());
        }
        auto root = objectFactory.ActivateInstance<JsonObject>();
        root.SetNamedValue(L"version", valueFactory.CreateNumberValue(formatVersion));
        root.SetNamedValue(L"folders", folderArray.as<IJsonValue>());
        return winrt::to_string(root.Stringify());
    } catch (const winrt::hresult_error&) {
        throw std::runtime_error("Cannot serialize pin store JSON");
    }
}

FileHandle LockForWriting(const fs::path& path) {
    fs::path lockPath = path;
    lockPath += L".lock";
    const auto deadline = GetTickCount64() + 5000;
    for (;;) {
        const HANDLE raw = CreateFileW(lockPath.c_str(), GENERIC_READ | GENERIC_WRITE | DELETE, 0,
                                       nullptr, OPEN_ALWAYS,
                                       FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
        if (raw != INVALID_HANDLE_VALUE) return FileHandle(raw, CloseHandle);

        // A delete-on-close lock may be reported as access denied until its owner exits.
        const DWORD error = GetLastError();
        if ((error != ERROR_SHARING_VIOLATION && error != ERROR_ACCESS_DENIED &&
             error != ERROR_DELETE_PENDING) || GetTickCount64() >= deadline) {
            FileError(error, "Lock pin store");
        }
        Sleep(15);
    }
}

void ReplacePinFile(const fs::path& path, std::string_view bytes) {
    if (bytes.size() > maxStoreBytes) throw std::runtime_error("Pin store size limit exceeded");
    GUID token{};
    wchar_t suffix[39]{};
    if (FAILED(CoCreateGuid(&token)) || !StringFromGUID2(token, suffix, ARRAYSIZE(suffix))) {
        throw std::runtime_error("Cannot allocate a temporary pin store name");
    }
    fs::path temporary = path;
    temporary += std::wstring(suffix) + L".tmp";
    const HANDLE raw = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                                   FILE_ATTRIBUTE_NORMAL, nullptr);
    if (raw == INVALID_HANDLE_VALUE) FileError(GetLastError(), "Create temporary pin store");
    FileHandle file(raw, CloseHandle);
    try {
        DWORD written = 0;
        if (!WriteFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr)) {
            FileError(GetLastError(), "Write temporary pin store");
        }
        if (written != bytes.size()) throw std::runtime_error("Incomplete pin store write");
        if (!FlushFileBuffers(file.get())) FileError(GetLastError(), "Flush temporary pin store");
        file.reset();
        for (DWORD delay = 5;; delay *= 2) {
            if (MoveFileExW(temporary.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) break;
            const auto error = GetLastError();
            if (delay > 80 || (error != ERROR_ACCESS_DENIED && error != ERROR_SHARING_VIOLATION &&
                              error != ERROR_LOCK_VIOLATION)) FileError(error, "Replace pin store");
            // Another process may still hold a read handle to the old snapshot.
            Sleep(delay);
        }
    } catch (...) {
        file.reset();
        DeleteFileW(temporary.c_str());
        throw;
    }
}

}  // namespace

Item ReadItem(const fs::path& path) {
    Item item{path.filename().native(), {}};
    const HANDLE raw = CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   nullptr, OPEN_EXISTING,
                                   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (raw == INVALID_HANDLE_VALUE) return item;
    const FileHandle file(raw, CloseHandle);
    FILE_ID_INFO info{};
    if (!GetFileInformationByHandleEx(file.get(), FileIdInfo, &info, sizeof(info)) ||
        std::ranges::all_of(info.FileId.Identifier, [](BYTE byte) { return byte == 0; })) {
        return item;
    }

    constexpr wchar_t digits[] = L"0123456789abcdef";
    const auto appendByte = [&](BYTE byte) {
        item.identity.push_back(digits[byte >> 4]);
        item.identity.push_back(digits[byte & 0xf]);
    };
    item.identity.reserve(16 + 1 + 32);
    for (int shift = 56; shift >= 0; shift -= 8) {
        appendByte(static_cast<BYTE>(info.VolumeSerialNumber >> shift));
    }
    item.identity.push_back(L':');
    for (const auto byte : info.FileId.Identifier) appendByte(byte);
    return item;
}

class PinStore {
public:
    explicit PinStore(fs::path path) : path_(std::move(path)) {}

    static fs::path DefaultPath() {
        PWSTR path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DONT_VERIFY, nullptr, &path);
        const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> folder(path, CoTaskMemFree);
        if (FAILED(result) || !folder) throw std::runtime_error("Cannot locate Local AppData");
        return fs::path(folder.get()) / L"Explorer Item Pins" / L"pins.json";
    }

    std::vector<Item> ReadFolder(const fs::path& folder) const {
        const auto key = FolderKey(folder);
        const auto file = OpenPinFile(path_);
        if (!file) return {};
        const auto lookup = [&](const Folders& folders) {
            const auto found = folders.find(key);
            return found != folders.end() ? found->second : std::vector<Item>{};
        };
        BY_HANDLE_FILE_INFORMATION info{};
        if (!GetFileInformationByHandle(file.get(), &info)) return lookup(ParseFolders(file.get()));
        const FileVersion version{info.dwVolumeSerialNumber, info.nFileIndexHigh, info.nFileIndexLow,
                                  info.ftLastWriteTime.dwHighDateTime, info.ftLastWriteTime.dwLowDateTime,
                                  info.nFileSizeHigh, info.nFileSizeLow};
        {
            const std::scoped_lock lock(cache_->mutex);
            if (cache_->version == version) return lookup(cache_->folders);
        }
        auto folders = ParseFolders(file.get());
        const std::scoped_lock lock(cache_->mutex);
        cache_->folders = std::move(folders);
        cache_->version = version;
        return lookup(cache_->folders);
    }

    bool SetPinned(const fs::path& folder, const Item& item, bool pinned) const {
        const auto key = FolderKey(folder);
        CheckItem(item);
        fs::create_directories(path_.parent_path());
        const auto lock = LockForWriting(path_);
        auto data = ReadFolders(path_);
        auto& entries = data[key];
        const auto match = std::ranges::find(entries, item.name, &Item::name);
        if (pinned) {
            if (match == entries.end()) {
                entries.push_back(item);
            } else {
                if (*match == item) return false;
                *match = item;
            }
        } else {
            if (match == entries.end()) return false;
            entries.erase(match);
            if (entries.empty()) data.erase(key);
        }
        ReplacePinFile(path_, WriteJson(data));
        return true;
    }

private:
    struct FileVersion {
        DWORD volume, indexHigh, indexLow, writeHigh, writeLow, sizeHigh, sizeLow;
        bool operator==(const FileVersion&) const = default;
    };

    struct ReadCache {
        std::mutex mutex;
        std::optional<FileVersion> version;
        Folders folders;
    };

    fs::path path_;
    std::shared_ptr<ReadCache> cache_ = std::make_shared<ReadCache>();
};

// Resolves one visible item per stored pin. Reserve names for all pins before
// following identities so a replacement at an existing name keeps its pin.
std::vector<std::optional<size_t>> ResolvePins(std::span<const Item> pins, std::span<const Item> items) {
    std::vector<std::optional<size_t>> resolved(pins.size());
    std::vector<bool> used(items.size());
    const auto resolveUnique = [&](auto matches) {
        for (size_t pin = 0; pin < pins.size(); ++pin) {
            if (resolved[pin]) continue;
            std::optional<size_t> match;
            for (size_t item = 0; item < items.size(); ++item) {
                if (!matches(pins[pin], items[item])) continue;
                if (match) {
                    match.reset();
                    break;
                }
                match = item;
            }
            if (match && !used[*match]) {
                used[*match] = true;
                resolved[pin] = match;
            }
        }
    };
    resolveUnique([](const Item& pin, const Item& item) { return pin.name == item.name; });
    resolveUnique([](const Item& pin, const Item& item) {
        return CompareNames(pin.name, item.name) == CSTR_EQUAL;
    });
    resolveUnique([](const Item& pin, const Item& item) {
        return !pin.identity.empty() && pin.identity == item.identity;
    });
    return resolved;
}

struct ExplorerSelection {
    fs::path folder;
    fs::path item;
};

enum class PinAction { Pin, Unpin };

namespace {

std::optional<fs::path> FileSystemPath(PCIDLIST_ABSOLUTE id) {
    if (!id) return std::nullopt;
    PWSTR rawName = nullptr;
    const HRESULT nameResult = SHGetNameFromIDList(id, SIGDN_FILESYSPATH, &rawName);
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> name(rawName, CoTaskMemFree);
    if (FAILED(nameResult) || !name) return std::nullopt;

    fs::path path(name.get());
    if (!path.is_absolute()) return std::nullopt;
    return path;
}

std::optional<fs::path> FileSystemFolderPath(IPersistFolder2& folder) {
    PIDLIST_ABSOLUTE rawFolder = nullptr;
    const HRESULT result = folder.GetCurFolder(&rawFolder);
    const std::unique_ptr<std::remove_pointer_t<PIDLIST_ABSOLUTE>, decltype(&CoTaskMemFree)>
        folderId(rawFolder, CoTaskMemFree);
    return SUCCEEDED(result) ? FileSystemPath(folderId.get()) : std::nullopt;
}

}  // namespace

std::optional<ExplorerSelection> CaptureSingleSelection(IFolderView2& view) {
    using Microsoft::WRL::ComPtr;
    ComPtr<IShellItemArray> selected;
    if (view.GetSelection(FALSE, &selected) != S_OK || !selected) return std::nullopt;
    DWORD count = 0;
    if (FAILED(selected->GetCount(&count)) || count != 1) return std::nullopt;

    ComPtr<IShellItem> item;
    if (FAILED(selected->GetItemAt(0, &item)) || !item) return std::nullopt;
    PWSTR rawItem = nullptr;
    const HRESULT itemResult = item->GetDisplayName(SIGDN_FILESYSPATH, &rawItem);
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> itemName(rawItem, CoTaskMemFree);
    if (FAILED(itemResult) || !itemName) return std::nullopt;

    ComPtr<IPersistFolder2> folder;
    if (FAILED(view.GetFolder(IID_PPV_ARGS(&folder))) || !folder) return std::nullopt;
    auto folderPath = FileSystemFolderPath(*folder.Get());
    fs::path itemPath(itemName.get());
    if (!folderPath || !itemPath.is_absolute() || !itemPath.has_filename() ||
        CompareNames(FolderKey(itemPath.parent_path()), FolderKey(*folderPath)) != CSTR_EQUAL) {
        return std::nullopt;
    }
    return ExplorerSelection{std::move(*folderPath), std::move(itemPath)};
}

namespace {

struct SelectionPin {
    Item item;
    std::optional<Item> stored;
};

// Call off the Explorer UI thread. This reads the store and may scan the folder.
SelectionPin InspectSelection(const PinStore& store, const ExplorerSelection& selection, bool needItemIdentity) {
    const auto pins = store.ReadFolder(selection.folder);
    const auto name = selection.item.filename().native();
    const auto exact = std::ranges::find(pins, name, &Item::name);
    if (exact != pins.end()) return {{name, {}}, *exact};

    const bool hasIdentityPin = std::ranges::any_of(pins, [](const Item& pin) { return !pin.identity.empty(); });
    auto item = needItemIdentity || hasIdentityPin ? ReadItem(selection.item) : Item{name, {}};
    const bool possibleRename = !item.identity.empty() &&
        std::ranges::find(pins, item.identity, &Item::identity) != pins.end();
    const bool possibleCaseMatch = std::ranges::any_of(pins, [&](const Item& pin) {
        return CompareNames(pin.name, item.name) == CSTR_EQUAL;
    });
    if (!possibleRename && !possibleCaseMatch) return {std::move(item), std::nullopt};

    // Only an alias candidate needs a directory scan. A surviving old name
    // or an ambiguous file identity must not bind the old record to this item.
    std::vector<Item> entries;
    std::optional<size_t> selectedIndex;
    for (const auto& entry : fs::directory_iterator(selection.folder)) {
        const auto entryName = entry.path().filename().native();
        if (entryName == item.name) {
            selectedIndex = entries.size();
            entries.push_back(item);
        } else if (possibleRename) {
            entries.push_back(ReadItem(entry.path()));
        } else {
            entries.push_back({entryName, {}});
        }
    }
    if (selectedIndex) {
        const auto matches = ResolvePins(pins, entries);
        const auto match = std::ranges::find(matches, selectedIndex);
        if (match != matches.end()) {
            return {std::move(item), pins[static_cast<size_t>(match - matches.begin())]};
        }
    }
    return {std::move(item), std::nullopt};
}

}  // namespace

PinAction GetPinAction(const PinStore& store, const ExplorerSelection& selection) {
    return InspectSelection(store, selection, false).stored ? PinAction::Unpin : PinAction::Pin;
}

bool ApplyPinAction(const PinStore& store, const ExplorerSelection& selection, PinAction action) {
    const auto current = InspectSelection(store, selection, action == PinAction::Pin);
    if (action == PinAction::Unpin) {
        return current.stored && store.SetPinned(selection.folder, *current.stored, false);
    }
    return !current.stored && store.SetPinned(selection.folder, current.item, true);
}

using PinNames = std::set<std::wstring, std::less<>>;

PinNames ReadPinNames(const PinStore& store, const fs::path& folder) {
    const auto pins = store.ReadFolder(folder);
    PinNames names;
    for (const auto& pin : pins) {
        WIN32_FIND_DATAW found{};
        HANDLE search = FindFirstFileW((folder / pin.name).c_str(), &found);
        if (search == INVALID_HANDLE_VALUE) continue;
        FindClose(search);
        if (pin.name == found.cFileName) names.insert(pin.name);
    }
    if (names.size() == pins.size()) return names;

    // Only a missing exact name needs enumeration and possible identity lookup.
    const bool lookUpIdentities = std::ranges::any_of(pins, [&](const Item& pin) {
        return !pin.identity.empty() && !names.contains(pin.name);
    });
    std::vector<Item> entries;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (lookUpIdentities) {
            entries.push_back(ReadItem(entry.path()));
        } else {
            entries.push_back({entry.path().filename().native(), {}});
        }
    }
    names.clear();
    for (const auto& index : ResolvePins(pins, entries)) {
        if (index) names.insert(std::move(entries[*index].name));
    }
    return names;
}

namespace {

using CompareViewItems = int (*)(const void*, const void*, void*);

// Layout of the synchronous merge-sort request in the supported shell binary.
struct ViewSortBatch {
    int count;
    void* items;
    int itemSize;
    CompareViewItems compare;
    void* compareContext;
};

static_assert(offsetof(ViewSortBatch, compare) == 0x18);
static_assert(offsetof(ViewSortBatch, compareContext) == 0x20);

using SortViewBatch = BOOL (*)(ViewSortBatch*);

struct NativeOrdering {
    SortViewBatch sort;
    CompareViewItems compare;
};

// Keep offline initialization for these verified images, including builds whose
// symbols are temporarily unavailable. Other images use Windhawk's resolver.
constexpr GUID storagePdb{0x6ebed4d4, 0x5734, 0x9fc4, {0x9e, 0x3e, 0x8d, 0xe5, 0x0b, 0x33, 0x0f, 0x0f}};
constexpr GUID shellPdb{0x617696ae, 0x77ca, 0x0846, {0x74, 0x3e, 0x0c, 0x34, 0x54, 0xe1, 0x7c, 0x05}};
constexpr GUID storagePdb9974{0x63a99e08, 0xa6bc, 0x71de, {0x16, 0x8a, 0xd4, 0xf5, 0x4b, 0x00, 0x12, 0x59}};
constexpr GUID shellPdb9974{0x64708141, 0x4aa5, 0xc991, {0xb8, 0xa9, 0x4f, 0x15, 0xc5, 0xba, 0x60, 0x02}};

unsigned char* CheckedImage(HMODULE module, const GUID& expectedPdb, DWORD lastRva) {
    if (!module) return nullptr;
    auto* image = reinterpret_cast<unsigned char*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
    const auto* pe = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image + dos->e_lfanew);
    if (pe->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) return nullptr;

    const auto& debug = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
    const auto size = pe->OptionalHeader.SizeOfImage;
    if (!debug.VirtualAddress || debug.VirtualAddress >= size || debug.Size > size - debug.VirtualAddress ||
        lastRva >= size) return nullptr;

    const auto* entries = reinterpret_cast<const IMAGE_DEBUG_DIRECTORY*>(image + debug.VirtualAddress);
    for (DWORD i = 0; i < debug.Size / sizeof(*entries); ++i) {
        const auto& entry = entries[i];
        constexpr DWORD codeViewHeaderSize = 24;
        if (entry.Type != IMAGE_DEBUG_TYPE_CODEVIEW || !entry.AddressOfRawData ||
            entry.AddressOfRawData >= size || entry.SizeOfData < codeViewHeaderSize ||
            entry.SizeOfData > size - entry.AddressOfRawData) continue;
        const auto* data = image + entry.AddressOfRawData;
        DWORD age = 0;
        std::memcpy(&age, data + 20, sizeof(age));
        if (age == 1 && std::memcmp(data, "RSDS", 4) == 0 &&
            std::memcmp(data + 4, &expectedPdb, sizeof(expectedPdb)) == 0) return image;
    }
    return nullptr;
}

[[maybe_unused]] std::optional<NativeOrdering> FindNativeOrdering(HMODULE storage) {
    constexpr DWORD sortRva = 0x1537e8;
    constexpr DWORD compareRva = 0x155190;
    auto image = CheckedImage(storage, storagePdb, compareRva);
    if (!image) image = CheckedImage(storage, storagePdb9974, compareRva);
    if (!image) return std::nullopt;
    return NativeOrdering{reinterpret_cast<SortViewBatch>(image + sortRva),
                          reinterpret_cast<CompareViewItems>(image + compareRva)};
}

std::wstring_view LeafName(std::wstring_view path) {
    const auto separator = path.find_last_of(L"\\/");
    return separator == std::wstring_view::npos ? path : path.substr(separator + 1);
}

// The native comparer gets the child PIDL through these shell interfaces.
struct ViewCompareContext {
    IShellFolder* folder;
    IUnknown* itemResolver;
};

bool IsPinnedViewItem(const ViewCompareContext& view, const void* item, const PinNames& names) {
    using Microsoft::WRL::ComPtr;
    constexpr GUID iidPidlAccessor{0xc412bf5b, 0x91ea, 0x4904, {0xb3, 0x4a, 0x85, 0x55, 0x04, 0xfb, 0xbf, 0x0b}};
    using Bind = HRESULT (*)(IUnknown*, const void*, IShellFolder*, IBindCtx*, REFIID, void**);
    using GetId = PCUITEMID_CHILD (*)(IUnknown*);
    const auto bind = reinterpret_cast<Bind>((*reinterpret_cast<void***>(view.itemResolver))[8]);
    ComPtr<IUnknown> accessor;
    if (FAILED(bind(view.itemResolver, item, view.folder, nullptr, iidPidlAccessor,
                    reinterpret_cast<void**>(accessor.GetAddressOf()))) || !accessor) return false;
    const auto getId = reinterpret_cast<GetId>((*reinterpret_cast<void***>(accessor.Get()))[3]);
    const auto id = getId(accessor.Get());
    if (!id) return false;

    STRRET display{};
    if (FAILED(view.folder->GetDisplayNameOf(id, SHGDN_INFOLDER | SHGDN_FORPARSING, &display))) return false;
    PWSTR rawName = nullptr;
    const HRESULT nameResult = StrRetToStrW(&display, id, &rawName);
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> name(rawName, CoTaskMemFree);
    if (FAILED(nameResult) || !name) return false;

    return names.contains(LeafName(name.get()));
}

struct PinnedComparison {
    CompareViewItems original;
    ViewCompareContext* view;
    const PinNames& names;

    static int Compare(const void* left, const void* right, void* context) {
        const auto& state = *static_cast<const PinnedComparison*>(context);
        const bool pinnedLeft = IsPinnedViewItem(*state.view, left, state.names);
        const bool pinnedRight = IsPinnedViewItem(*state.view, right, state.names);
        if (pinnedLeft != pinnedRight) return pinnedLeft ? -1 : 1;
        return state.original(left, right, state.view);
    }
};

[[maybe_unused]] BOOL SortWithPins(const PinStore& store, SortViewBatch sort, CompareViewItems compare,
                                  ViewSortBatch* batch) {
    if (!batch || batch->count < 2 || batch->compare != compare || !batch->compareContext) return sort(batch);

    auto* view = static_cast<ViewCompareContext*>(batch->compareContext);
    if (!view->folder || !view->itemResolver) return sort(batch);
    PinNames names;
    try {
        Microsoft::WRL::ComPtr<IPersistFolder2> folder;
        if (SUCCEEDED(view->folder->QueryInterface(IID_PPV_ARGS(&folder))) && folder) {
            if (const auto path = FileSystemFolderPath(*folder.Get())) names = ReadPinNames(store, *path);
        }
    } catch (...) {
        // A malformed store or an inaccessible folder must leave sorting available.
    }
    if (names.empty()) return sort(batch);

    PinnedComparison pinned{compare, view, names};
    // The native request lives on the caller's stack. Its scratch pointer is
    // stored immediately after these fields and must stay in the same request.
    batch->compare = PinnedComparison::Compare;
    batch->compareContext = &pinned;
    const BOOL result = sort(batch);
    batch->compareContext = view;
    batch->compare = compare;
    return result;
}

constexpr DWORD pinnedGroupId = 0x56494e50;

struct GroupList : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT index, IUnknown** group) = 0;
};

using GroupBatch = HRESULT (*)(const unsigned char*, void*);
using AssignGroup = HRESULT (*)(IUnknown*, IUnknown*, void*, IShellFolder2*, DWORD);
using GroupInfo = HRESULT (*)(IUnknown*, DWORD, IUnknown**);
using ApplyGroups = HRESULT (*)(const unsigned char*, GroupList*, UINT*);
using SetGrouping = HRESULT (*)(IFolderView2*, REFPROPERTYKEY, BOOL);
using RegisterGroup = HRESULT (*)(IUnknown*, DWORD);
using MakeGroup = HRESULT (*)(DWORD, UINT64, const PROPERTYKEY*, DWORD, IUnknown*, PCWSTR, IUnknown**);

struct NativeGrouping {
    GroupBatch batch;
    AssignGroup assign;
    GroupInfo rangeInfo;
    GroupInfo textInfo;
    ApplyGroups apply;
    SetGrouping setGrouping;
    RegisterGroup remember;
    MakeGroup make;
    void* rangeCategory;
    void* textCategory;
};

[[maybe_unused]] SetGrouping FindKnownSetGrouping(HMODULE shell) {
    constexpr DWORD oldRva = 0x2e8ae0;
    constexpr DWORD updatedRva = 0x2e8e50;
    if (const auto image = CheckedImage(shell, shellPdb, oldRva)) {
        return reinterpret_cast<SetGrouping>(image + oldRva);
    }
    if (const auto image = CheckedImage(shell, shellPdb9974, updatedRva)) {
        return reinterpret_cast<SetGrouping>(image + updatedRva);
    }
    return nullptr;
}

[[maybe_unused]] std::optional<NativeGrouping> FindNativeGrouping(HMODULE storage, SetGrouping setGrouping) {
    const auto updated = CheckedImage(storage, storagePdb9974, 0x5bcf00);
    const auto image = updated ? updated : CheckedImage(storage, storagePdb, 0x5bcb30);
    if (!image || !setGrouping) return std::nullopt;
    return NativeGrouping{
        reinterpret_cast<GroupBatch>(image + 0x153e50), reinterpret_cast<AssignGroup>(image + 0x1641f0),
        reinterpret_cast<GroupInfo>(image + (updated ? 0x22fcf0 : 0x22fc90)), reinterpret_cast<GroupInfo>(image + 0x16f0c0),
        reinterpret_cast<ApplyGroups>(image + 0x16eab8), setGrouping,
        reinterpret_cast<RegisterGroup>(image + 0x1642dc), reinterpret_cast<MakeGroup>(image + (updated ? 0x22fdac : 0x22fd4c)),
        image + (updated ? 0x248790 : 0x248730),
        image + (updated ? 0x5bcf00 : 0x5bcb30),
    };
}

struct PinText {
    PCWSTR pin;
    PCWSTR unpin;
    PCWSTR group;
};

PinText LocalizedPinText(LANGID language) {
    switch (PRIMARYLANGID(language)) {
        case LANG_RUSSIAN:
            return {L"Закрепить в области объектов", L"Открепить в области объектов", L"Закрепленные"};
        case LANG_SPANISH:
            return {L"Anclar en el panel de elementos", L"Desanclar del panel de elementos", L"Anclados"};
        case LANG_GERMAN:
            return {L"Im Elementbereich anheften", L"Vom Elementbereich lösen", L"Angeheftet"};
        case LANG_PORTUGUESE:
            if (SUBLANGID(language) == SUBLANG_PORTUGUESE_BRAZILIAN) {
                return {L"Fixar no painel de itens", L"Desafixar do painel de itens", L"Fixados"};
            }
            break;
        case LANG_CHINESE:
            if (SUBLANGID(language) == SUBLANG_CHINESE_SIMPLIFIED) {
                return {L"固定到项目窗格", L"从项目窗格取消固定", L"已固定"};
            }
            break;
    }
    return {L"Pin in item pane", L"Unpin from item pane", L"Pinned"};
}

// A grouping worker runs synchronously inside its batch. Each view gets its
// own snapshot, including when Explorer groups the same folder in two tabs.
class GroupingPins {
public:
    explicit GroupingPins(const PinStore& store) : store_(store), previous_(std::exchange(current_, this)) {}
    ~GroupingPins() { current_ = previous_; }
    GroupingPins(const GroupingPins&) = delete;
    GroupingPins& operator=(const GroupingPins&) = delete;

    static const PinNames* ForFolder(IShellFolder2* folder) {
        if (!current_ || !folder) return nullptr;
        if (current_->folder_.Get() != folder) current_->ReadFolder(folder);
        return &current_->names_;
    }

private:
    void ReadFolder(IShellFolder2* folder) {
        folder_ = folder;
        names_.clear();
        try {
            Microsoft::WRL::ComPtr<IPersistFolder2> source;
            if (SUCCEEDED(folder->QueryInterface(IID_PPV_ARGS(&source))) && source) {
                if (const auto path = FileSystemFolderPath(*source.Get())) names_ = ReadPinNames(store_, *path);
            }
        } catch (...) {
            // Keep the shell's groups if the folder or store cannot be read.
        }
    }

    inline static thread_local GroupingPins* current_ = nullptr;
    const PinStore& store_;
    GroupingPins* previous_;
    Microsoft::WRL::ComPtr<IShellFolder2> folder_;
    PinNames names_;
};

[[maybe_unused]] HRESULT GroupWithPins(const PinStore& store, GroupBatch original,
                                       const unsigned char* batch, void* result) {
    const GroupingPins scope(store);
    return original(batch, result);
}

bool IsPinnedGroupItem(IUnknown* row, const PinNames& names) {
    using ReadValue = HRESULT (*)(IUnknown*, REFPROPERTYKEY, PROPVARIANT*);
    const auto read = reinterpret_cast<ReadValue>((*reinterpret_cast<void***>(row))[3]);
    PROPVARIANT path{};
    const HRESULT result = read(row, PKEY_ParsingPath, &path);
    const bool pinned = SUCCEEDED(result) && path.vt == VT_LPWSTR && path.pwszVal &&
                        names.contains(LeafName(path.pwszVal));
    PropVariantClear(&path);
    return pinned;
}

[[maybe_unused]] HRESULT AssignWithPins(const NativeGrouping& api, IUnknown* category, IUnknown* row,
                                        void* item, IShellFolder2* context, DWORD flags) {
    const auto* names = GroupingPins::ForFolder(context);
    if (!names || names->empty() || !row) return api.assign(category, row, item, context, flags);
    const auto categoryMethod = (*reinterpret_cast<void***>(category))[6];
    if ((categoryMethod != api.rangeCategory && categoryMethod != api.textCategory) ||
        !IsPinnedGroupItem(row, *names)) return api.assign(category, row, item, context, flags);

    using SetCategory = HRESULT (*)(IUnknown*, void*, const PROPVARIANT*, DWORD);
    const auto set = reinterpret_cast<SetCategory>((*reinterpret_cast<void***>(row))[5]);
    PROPVARIANT group{};
    group.vt = VT_UI4;
    group.ulVal = pinnedGroupId;
    const auto result = set(row, item, &group, flags);
    return SUCCEEDED(result) ? api.remember(category, pinnedGroupId) : result;
}

[[maybe_unused]] HRESULT GroupInfoWithPins(MakeGroup make, GroupInfo original, IUnknown* category,
                                           DWORD group, IUnknown** output) {
    if (group != pinnedGroupId) return original(category, group, output);
    Microsoft::WRL::ComPtr<IObjectWithPropertyKey> source;
    PROPERTYKEY property{};
    if (FAILED(category->QueryInterface(IID_PPV_ARGS(&source))) || !source || FAILED(source->GetPropertyKey(&property))) {
        return original(category, group, output);
    }
    return make(group, 0, &property, 0, nullptr, LocalizedPinText(GetUserDefaultUILanguage()).group, output);
}

class ReorderedGroupList final : public GroupList {
public:
    ReorderedGroupList(GroupList& original, UINT pinned, UINT count)
        : original_(original), pinned_(pinned), count_(count) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** output) override {
        *output = nullptr;
        if (iid != IID_IUnknown) return E_NOINTERFACE;
        *output = static_cast<GroupList*>(this);
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE GetCount(UINT* count) override { return original_.GetCount(count); }
    HRESULT STDMETHODCALLTYPE GetAt(UINT index, IUnknown** group) override {
        if (index >= count_) return original_.GetAt(index, group);
        // Explorer starts at either end of the array, depending on its group order.
        if (!first_) first_ = index;
        const UINT target = *first_;
        if (target != 0 && target != count_ - 1) return original_.GetAt(index, group);
        if (index == target) return original_.GetAt(pinned_, group);
        const UINT withoutPinned = index - (index > target ? 1 : 0);
        return original_.GetAt(withoutPinned + (withoutPinned >= pinned_ ? 1 : 0), group);
    }

private:
    GroupList& original_;
    UINT pinned_;
    UINT count_;
    std::optional<UINT> first_;
};

std::optional<DWORD> NativeGroupId(IUnknown& group) {
    using GetId = HRESULT (*)(IUnknown*, DWORD*);
    const auto read = reinterpret_cast<GetId>((*reinterpret_cast<void***>(&group))[3]);
    DWORD id = 0;
    return SUCCEEDED(read(&group, &id)) ? std::optional(id) : std::nullopt;
}

[[maybe_unused]] HRESULT ApplyWithPins(const NativeGrouping& api, const unsigned char* view, GroupList* groups,
                                       UINT* added) {
    UINT count = 0;
    if (!groups || FAILED(groups->GetCount(&count))) return api.apply(view, groups, added);
    for (UINT i = 0; i < count; ++i) {
        Microsoft::WRL::ComPtr<IUnknown> group;
        if (FAILED(groups->GetAt(i, group.GetAddressOf())) || !group || NativeGroupId(*group.Get()) != pinnedGroupId) continue;
        ReorderedGroupList reordered(*groups, i, count);
        return api.apply(view, &reordered, added);
    }
    return api.apply(view, groups, added);
}

[[maybe_unused]] HRESULT SetGroupingWithPins(const PinStore& store, SetGrouping target, SetGrouping original, IFolderView2* view,
                                             REFPROPERTYKEY key, BOOL ascending) {
    // CDefView can have multiple interface pointers. Check the public IFolderView2
    // slot before using other methods through the pointer from the native hook.
    constexpr size_t setGroupBySlot = 17;
    if (!view || (*reinterpret_cast<SetGrouping* const*>(view))[setGroupBySlot] != target) {
        return original(view, key, ascending);
    }
    PROPERTYKEY previous{};
    BOOL previousAscending = FALSE;
    const bool flips = view->GetGroupBy(&previous, &previousAscending) == S_OK &&
                       IsEqualPropertyKey(key, previous) && (ascending != FALSE) != (previousAscending != FALSE);
    Microsoft::WRL::ComPtr<IShellView> refresh;
    if (flips) {
        try {
            Microsoft::WRL::ComPtr<IPersistFolder2> folder;
            if (SUCCEEDED(view->GetFolder(IID_PPV_ARGS(&folder))) && folder) {
                if (const auto path = FileSystemFolderPath(*folder.Get()); path && !store.ReadFolder(*path).empty()) {
                    view->QueryInterface(IID_PPV_ARGS(&refresh));
                }
            }
        } catch (...) {
            // The direction change remains available if the store is unreadable.
        }
    }
    const auto result = original(view, key, ascending);
    // A direction-only change normally reverses cached groups. Rebuild the
    // pinned folder's presentation with the same grouping key and new order.
    if (SUCCEEDED(result) && refresh) refresh->Refresh();
    return result;
}

using DrawItemImage = HRESULT (*)(const unsigned char*, HDC, const RECT*);
using PaintFolderImage = void (*)(void*, HDC, const RECT*, const RECT*, RECT*, RECT*);

constexpr GUID framePdb{0x468742f3, 0xd41c, 0x3441, {0xa5, 0x02, 0x2f, 0xbf, 0xfe, 0xf9, 0xa6, 0x56}};
constexpr GUID framePdb9974{0xf8886b4f, 0x52b8, 0x73ab, {0x4a, 0x57, 0x43, 0xf3, 0x89, 0x20, 0x9a, 0xce}};

[[maybe_unused]] DrawItemImage FindKnownImageRenderer(HMODULE storage) {
    constexpr DWORD draw = 0x16d730;
    auto image = CheckedImage(storage, storagePdb, draw);
    if (!image) image = CheckedImage(storage, storagePdb9974, draw);
    return image ? reinterpret_cast<DrawItemImage>(image + draw) : nullptr;
}

[[maybe_unused]] PaintFolderImage FindKnownFolderImagePaint(HMODULE frame) {
    constexpr DWORD paint = 0xbde30;
    auto image = CheckedImage(frame, framePdb, paint);
    if (!image) image = CheckedImage(frame, framePdb9974, paint);
    return image ? reinterpret_cast<PaintFolderImage>(image + paint) : nullptr;
}

class FolderImagePaintScope {
public:
    FolderImagePaintScope() : previous_(std::exchange(active_, true)) {}
    ~FolderImagePaintScope() { active_ = previous_; }
    FolderImagePaintScope(const FolderImagePaintScope&) = delete;
    FolderImagePaintScope& operator=(const FolderImagePaintScope&) = delete;

    static bool Active() { return active_; }

private:
    inline static thread_local bool active_ = false;
    bool previous_;
};

template <typename Job>
bool QueueModuleWork(Job job) {
    struct Work {
        HMODULE module = nullptr;
        Job run;
    };
    auto work = std::make_unique<Work>(nullptr, std::move(job));
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<PCWSTR>(QueueModuleWork<Job>),
                            &work->module)) return false;
    auto* context = work.release();
    if (TrySubmitThreadpoolCallback([](PTP_CALLBACK_INSTANCE callback, void* raw) noexcept {
            std::unique_ptr<Work> work(static_cast<Work*>(raw));
            FreeLibraryWhenCallbackReturns(callback, work->module);
            const auto apartment = RoInitialize(RO_INIT_MULTITHREADED);
            try { work->run(); } catch (...) {}
            work.reset();
            if (SUCCEEDED(apartment)) RoUninitialize();
        }, context, nullptr)) return true;
    const auto module = context->module;
    delete context;
    FreeLibrary(module);
    return false;
}

bool WindowHasClass(HWND window, PCWSTR expected) {
    wchar_t name[32]{};
    return GetClassNameW(window, name, ARRAYSIZE(name)) && std::wstring_view(name) == expected;
}

bool IsExplorerFolderWindow(HWND window) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    return process == GetCurrentProcessId() && IsWindowVisible(window) && WindowHasClass(window, L"CabinetWClass");
}

void RedrawExplorerItemIcons() {
    EnumWindows([](HWND window, LPARAM) -> BOOL {
        if (!IsExplorerFolderWindow(window)) return TRUE;
        EnumChildWindows(window, [](HWND child, LPARAM) -> BOOL {
            if (IsWindowVisible(child) && WindowHasClass(child, L"SHELLDLL_DefView")) {
                RedrawWindow(child, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);
            }
            return TRUE;
        }, 0);
        return TRUE;
    }, 0);
}

class IndicatorPins {
public:
    std::optional<bool> Cached(const fs::path& item) {
        const auto folder = FolderKey(item.parent_path());
        const std::scoped_lock lock(mutex_);
        const auto found = folders_.find(folder);
        if (found == folders_.end() || !found->second.ready || found->second.expires <= GetTickCount64()) return std::nullopt;
        return found->second.names.contains(item.filename().native());
    }

    uint64_t Revision() {
        const std::scoped_lock lock(mutex_);
        return revision_;
    }

    std::optional<bool> Publish(const fs::path& parent, PinNames names, uint64_t revision) {
        const auto folder = FolderKey(parent);
        const std::scoped_lock lock(mutex_);
        if (revision != revision_) return std::nullopt;
        const auto entry = FindEntry(folder);
        if (!entry) return std::nullopt;
        return SetNames(*entry, std::move(names));
    }

    bool Contains(const PinStore& store, const fs::path& item) {
        const auto parent = item.parent_path();
        if (!parent.is_absolute() || !item.has_filename()) return false;
        const auto folder = FolderKey(parent);
        const auto now = GetTickCount64();
        uint64_t request = 0;
        bool pinned = false;
        {
            const std::scoped_lock lock(mutex_);
            const auto entry = FindEntry(folder);
            if (!entry) return false;
            pinned = entry->names.contains(item.filename().native());
            if (!entry->request && entry->expires <= now) {
                if (pending_ < 4) {
                    request = entry->request = ++sequence_;
                    ++pending_;
                } else {
                    waitingForCapacity_ = true;
                }
            }
        }
        if (request) {
            bool submitted = false;
            try {
                submitted = QueueModuleWork([this, store, parent, folder, request] {
                    std::optional<PinNames> names;
                    try { names = ReadPinNames(store, parent); } catch (...) {}
                    if (Complete(folder, request, std::move(names))) RedrawExplorerItemIcons();
                });
            } catch (...) {}
            if (!submitted && Complete(folder, request, std::nullopt)) RedrawExplorerItemIcons();
        }
        return pinned;
    }

    void Invalidate(const fs::path& parent) {
        const std::scoped_lock lock(mutex_);
        ++revision_;
        const auto found = folders_.find(FolderKey(parent));
        if (found != folders_.end()) {
            found->second.expires = 0;
            found->second.request = 0;
        }
    }

private:
    struct Entry {
        PinNames names;
        ULONGLONG expires = 0;
        uint64_t request = 0;
        bool ready = false;
    };

    Entry* FindEntry(const std::wstring& folder) {
        if (const auto found = folders_.find(folder); found != folders_.end()) return &found->second;
        if (folders_.size() >= 64) {
            const auto evict = std::ranges::find_if(folders_, [](const auto& entry) { return !entry.second.request; });
            if (evict == folders_.end()) return nullptr;
            folders_.erase(evict);
        }
        return &folders_.try_emplace(folder).first->second;
    }

    bool SetNames(Entry& entry, PinNames names) {
        const bool changed = entry.names != names;
        entry.names = std::move(names);
        entry.expires = GetTickCount64() + 1000;
        entry.request = 0;
        entry.ready = true;
        return changed;
    }

    bool Complete(const std::wstring& folder, uint64_t request, std::optional<PinNames> names) {
        const std::scoped_lock lock(mutex_);
        --pending_;
        const bool retryWaiting = std::exchange(waitingForCapacity_, false);
        const auto found = folders_.find(folder);
        if (found == folders_.end() || found->second.request != request) return retryWaiting;
        auto& entry = found->second;
        if (names) return SetNames(entry, std::move(*names)) || retryWaiting;
        entry.request = 0;
        entry.expires = GetTickCount64() + 1000;
        entry.ready = false;
        return retryWaiting;
    }

    std::mutex mutex_;
    std::map<std::wstring, Entry, FolderLess> folders_;
    uint64_t sequence_ = 0;
    uint64_t revision_ = 0;
    size_t pending_ = 0;
    bool waitingForCapacity_ = false;
};

std::optional<fs::path> ItemImagePath(const unsigned char* image) {
    auto* source = *reinterpret_cast<IUnknown* const*>(image + 0x28);
    if (!source) return std::nullopt;
    using GetValues = HRESULT (*)(IUnknown*, DWORD, const PROPERTYKEY*, PROPVARIANT*, ULONG*);
    const auto read = reinterpret_cast<GetValues>((*reinterpret_cast<void***>(source))[3]);
    ULONG found = 0;
    PROPVARIANT path{};
    const auto clear = [](PROPVARIANT* value) { PropVariantClear(value); };
    const std::unique_ptr<PROPVARIANT, decltype(clear)> cleanup(&path, clear);
    if (FAILED(read(source, 0, &PKEY_ParsingPath, &path, &found)) || found != 1 ||
        path.vt != VT_LPWSTR || !path.pwszVal) return std::nullopt;
    return fs::path(path.pwszVal);
}

HWND ExplorerFrameForPaint(HDC dc) {
    auto frame = GetAncestor(WindowFromDC(dc), GA_ROOT);
    if (!WindowHasClass(frame, L"CabinetWClass")) frame = GetActiveWindow();
    if (!WindowHasClass(frame, L"CabinetWClass")) {
        frame = nullptr;
        EnumThreadWindows(GetCurrentThreadId(), [](HWND candidate, LPARAM output) -> BOOL {
            if (!IsWindowVisible(candidate) || !WindowHasClass(candidate, L"CabinetWClass")) return TRUE;
            *reinterpret_cast<HWND*>(output) = candidate;
            return FALSE;
        }, reinterpret_cast<LPARAM>(&frame));
    }
    return frame && GetWindowThreadProcessId(frame, nullptr) == GetCurrentThreadId() ? frame : nullptr;
}

struct IndicatorRetry {
    ULONGLONG time = 0;
    unsigned redraws = 0;

    void AfterFailure(HDC dc, ULONGLONG now) {
        constexpr DWORD delay = 60;
        const bool requestRedraw = redraws < 2;
        time = now + (requestRedraw ? delay : 1000);
        if (!requestRedraw) return;
        ++redraws;
        if (const auto frame = ExplorerFrameForPaint(dc)) {
            const auto thread = GetCurrentThreadId();
            QueueModuleWork([frame, thread] {
                Sleep(delay);
                if (GetWindowThreadProcessId(frame, nullptr) == thread) {
                    RedrawWindow(frame, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_INTERNALPAINT);
                }
            });
        }
    }
};

RECT VisibleBounds(int width, int height, auto visible) {
    RECT bounds{width, height, 0, 0};
    for (LONG y = 0; y < height; ++y) {
        for (LONG x = 0; x < width; ++x) {
            if (!visible(static_cast<size_t>(y) * width + x)) continue;
            bounds.left = std::min(bounds.left, x);
            bounds.top = std::min(bounds.top, y);
            bounds.right = std::max(bounds.right, x + 1);
            bounds.bottom = std::max(bounds.bottom, y + 1);
        }
    }
    return IsRectEmpty(&bounds) ? RECT{} : bounds;
}

using IconHandle = std::unique_ptr<std::remove_pointer_t<HICON>, decltype(&DestroyIcon)>;

struct NavigationPin {
    std::vector<DWORD> pixels;
    int width = 0;
    int height = 0;
};

NavigationPin LoadNavigationPin(HDC dc) {
    // The shell loads this icon group for pinned navigation items.
    constexpr int navigationPinResource = 5100;
    constexpr int originalSize = 256;
    constexpr DWORD resourceFlags = LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    const std::unique_ptr<std::remove_pointer_t<HMODULE>, decltype(&FreeLibrary)> module(
        LoadLibraryExW(L"imageres.dll", nullptr, resourceFlags), FreeLibrary);
    if (!module) return {};
    const IconHandle icon(static_cast<HICON>(LoadImageW(module.get(), MAKEINTRESOURCEW(navigationPinResource),
                                                      IMAGE_ICON, originalSize, originalSize, LR_DEFAULTCOLOR)), DestroyIcon);
    ICONINFO info{};
    if (!icon || !GetIconInfo(icon.get(), &info)) return {};
    const std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(&DeleteObject)>
        color(info.hbmColor, DeleteObject), mask(info.hbmMask, DeleteObject);
    BITMAP size{};
    if (!color || !GetObjectW(color.get(), sizeof(size), &size) || size.bmWidth <= 0 || size.bmHeight <= 0) return {};
    const int width = size.bmWidth, height = size.bmHeight;

    BITMAPINFO bitmap{};
    bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap.bmiHeader.biWidth = width;
    bitmap.bmiHeader.biHeight = -height;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    std::vector<DWORD> pixels(static_cast<size_t>(width) * height);
    if (GetDIBits(dc, color.get(), 0, height, pixels.data(), &bitmap, DIB_RGB_COLORS) != height) return {};
    auto ink = VisibleBounds(width, height, [&](size_t i) { return (pixels[i] >> 24) != 0; });
    if (IsRectEmpty(&ink)) {
        std::vector<DWORD> transparency(pixels.size());
        if (!mask || GetDIBits(dc, mask.get(), 0, height, transparency.data(), &bitmap, DIB_RGB_COLORS) != height) return {};
        for (size_t i = 0; i < pixels.size(); ++i) pixels[i] = transparency[i] & 0x00ffffff ? 0 : pixels[i] | 0xff000000;
        ink = VisibleBounds(width, height, [&](size_t i) { return (pixels[i] >> 24) != 0; });
    }
    if (IsRectEmpty(&ink)) return {};

    // Fit the navigation asset's visible pixels to the shortcut overlay footprint.
    NavigationPin pin{{}, ink.right - ink.left, ink.bottom - ink.top};
    pin.pixels.reserve(static_cast<size_t>(pin.width) * pin.height);
    for (int y = ink.top; y < ink.bottom; ++y) {
        const auto row = pixels.begin() + static_cast<size_t>(y) * width;
        for (int x = ink.left; x < ink.right; ++x) pin.pixels.push_back(row[x] >> 24 ? row[x] : 0);
    }
    return pin;
}

bool ScaleNavigationPin(NavigationPin& pin, int width, int height, void* buffer) {
    if (pin.width == width && pin.height == height) {
        std::ranges::copy(pin.pixels, static_cast<DWORD*>(buffer));
        return true;
    }
    Microsoft::WRL::ComPtr<IWICImagingFactory> imaging;
    Microsoft::WRL::ComPtr<IWICBitmap> source;
    Microsoft::WRL::ComPtr<IWICFormatConverter> premultiplied, straight;
    Microsoft::WRL::ComPtr<IWICBitmapScaler> scaler;
    constexpr UINT pixelBytes = sizeof(DWORD);
    const auto interpolation = width < pin.width ? WICBitmapInterpolationModeFant : WICBitmapInterpolationModeCubic;
    const auto convert = [&](IWICBitmapSource* input, REFWICPixelFormatGUID format, auto& output) {
        return SUCCEEDED(imaging->CreateFormatConverter(&output)) &&
               SUCCEEDED(output->Initialize(input, format, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom));
    };
    // Resample premultiplied colors to avoid fringes, then restore the straight alpha expected by HICON.
    return SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imaging))) &&
           SUCCEEDED(imaging->CreateBitmapFromMemory(pin.width, pin.height, GUID_WICPixelFormat32bppBGRA,
                     pin.width * pixelBytes, static_cast<UINT>(pin.pixels.size()) * pixelBytes,
                     reinterpret_cast<BYTE*>(pin.pixels.data()), &source)) &&
           convert(source.Get(), GUID_WICPixelFormat32bppPBGRA, premultiplied) &&
           SUCCEEDED(imaging->CreateBitmapScaler(&scaler)) &&
           SUCCEEDED(scaler->Initialize(premultiplied.Get(), width, height, interpolation)) &&
           convert(scaler.Get(), GUID_WICPixelFormat32bppBGRA, straight) &&
           SUCCEEDED(straight->CopyPixels(nullptr, width * pixelBytes, width * height * pixelBytes, static_cast<BYTE*>(buffer)));
}

IconHandle CreateNavigationPinIcon(NavigationPin& pin, HDC dc, int width, int height) {
    BITMAPINFO bitmap{};
    bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap.bmiHeader.biWidth = width;
    bitmap.bmiHeader.biHeight = -height;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    void* buffer = nullptr;
    const std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(&DeleteObject)>
        color(CreateDIBSection(dc, &bitmap, DIB_RGB_COLORS, &buffer, nullptr, 0), DeleteObject);
    const std::vector<WORD> blankMask(static_cast<size_t>((width + 15) / 16) * height);
    const std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(&DeleteObject)>
        mask(CreateBitmap(width, height, 1, 1, blankMask.data()), DeleteObject);
    if (!color || !mask || !ScaleNavigationPin(pin, width, height, buffer)) return {nullptr, DestroyIcon};
    ICONINFO pinInfo{TRUE, 0, 0, mask.get(), color.get()};
    return {CreateIconIndirect(&pinInfo), DestroyIcon};
}

int ShortcutOverlaySlot() {
    static std::atomic_int slot;
    if (const int cached = slot.load(std::memory_order_relaxed)) return cached;
    SHSTOCKICONINFO link{};
    link.cbSize = sizeof(link);
    Microsoft::WRL::ComPtr<IImageList> list;
    if (FAILED(SHGetStockIconInfo(SIID_LINK, SHGSI_SYSICONINDEX, &link)) ||
        FAILED(SHGetImageList(SHIL_SMALL, IID_PPV_ARGS(&list)))) return 0;
    for (int i = 1; i <= 15; ++i) {
        int index = -1;
        if (SUCCEEDED(list->GetOverlayImage(i, &index)) && index == link.iSysImageIndex) {
            slot.store(i, std::memory_order_relaxed);
            return i;
        }
    }
    return 0;
}

RECT MeasureShortcutOverlay(IUnknown* cache, IUnknown* item, HDC dc, int width, int height) {
    const int slot = ShortcutOverlaySlot();
    if (!slot) return {};

    BITMAPINFO bitmap{};
    bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap.bmiHeader.biWidth = width;
    bitmap.bmiHeader.biHeight = -height;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    void* buffer = nullptr;
    const std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(&DeleteObject)>
        surface(CreateDIBSection(dc, &bitmap, DIB_RGB_COLORS, &buffer, nullptr, 0), DeleteObject);
    const std::unique_ptr<std::remove_pointer_t<HDC>, decltype(&DeleteDC)>
        canvas(CreateCompatibleDC(dc), DeleteDC);
    if (!surface || !canvas || !SelectObject(canvas.get(), surface.get())) return {};

    // Render a stock icon with and without the shell's link overlay. The native
    // compositor supplies the overlay footprint for this size, including rounding.
    using DrawCachedIcon = HRESULT (*)(IUnknown*, IMAGELISTDRAWPARAMS*);
    const auto draw = reinterpret_cast<DrawCachedIcon>((*reinterpret_cast<void***>(cache))[16]);
    IMAGELISTDRAWPARAMS params{};
    params.cbSize = sizeof(params);
    params.i = SIID_DOCNOASSOC;
    params.hdcDst = canvas.get();
    params.cx = width;
    params.cy = height;
    params.rgbBk = CLR_NONE;
    constexpr UINT nativeDrawStyle = 0x80000 | ILD_ASYNC | ILD_TRANSPARENT;
    const UINT overlayStyle = nativeDrawStyle | INDEXTOOVERLAYMASK(slot);
    const auto count = static_cast<size_t>(width) * height;
    auto* pixels = static_cast<DWORD*>(buffer);
    constexpr DWORD background = 0xff808080;
    const auto render = [&](UINT style) {
        std::fill_n(pixels, count, background);
        params.fStyle = style;
        const auto result = draw(cache, &params);
        GdiFlush();
        return result;
    };
    const auto overlayResult = render(overlayStyle);
    constexpr HRESULT missingOverlay = 0x10;
    if (SUCCEEDED(overlayResult) && (overlayResult & missingOverlay)) {
        // Use Explorer's own cache-miss path when this folder has no ready link overlay.
        using FillCachedOverlay = HRESULT (*)(IUnknown*, IUnknown*, HRESULT, DWORD, IMAGELISTDRAWPARAMS*, int*);
        const auto fill = reinterpret_cast<FillCachedOverlay>((*reinterpret_cast<void***>(cache))[17]);
        constexpr DWORD loadOverlayOnly = 2;
        int state = 0;
        if (item) fill(cache, item, overlayResult, loadOverlayOnly, &params, &state);
        GdiFlush();
    }
    if (FAILED(render(nativeDrawStyle))) return {};
    const std::vector<DWORD> plain(pixels, pixels + count);
    if (FAILED(render(overlayStyle))) return {};
    const auto bounds = VisibleBounds(width, height, [&](size_t i) {
        return ((pixels[i] ^ plain[i]) & 0x00ffffff) != 0;
    });
    // A placeholder icon can change while the overlay is loaded asynchronously.
    if (bounds.right > width / 2 || bounds.top < height / 2) return {};
    return bounds;
}

RECT ShortcutOverlayBounds(const unsigned char* image, HDC dc, const RECT& icon) {
    // The verified renderer layout holds the shell icon cache at this offset.
    auto* cache = *reinterpret_cast<IUnknown* const*>(image + 0x20);
    const int width = icon.right - icon.left, height = icon.bottom - icon.top;
    if (!cache || width <= 0 || height <= 0 || width > 1024 || height > 1024) return {};
    struct Overlay {
        IUnknown* cache = nullptr;
        int width = 0;
        int height = 0;
        RECT bounds{};
        IndicatorRetry retry{};
    };
    static thread_local std::array<Overlay, 32> sizes;
    static thread_local size_t nextSize = 0;
    auto size = std::ranges::find_if(sizes, [&](const auto& entry) {
        return entry.cache == cache && entry.width == width && entry.height == height;
    });
    if (size == sizes.end()) {
        size = sizes.begin() + nextSize++ % sizes.size();
        *size = {cache, width, height};
    }
    const auto now = GetTickCount64();
    if (!IsRectEmpty(&size->bounds) || now < size->retry.time) return size->bounds;
    auto* item = *reinterpret_cast<IUnknown* const*>(image + 0x28);
    size->bounds = MeasureShortcutOverlay(cache, item, dc, width, height);
    if (IsRectEmpty(&size->bounds)) size->retry.AfterFailure(dc, now);
    return size->bounds;
}

struct NavigationPinCache {
    struct Image {
        int width = 0;
        int height = 0;
        IconHandle icon{nullptr, DestroyIcon};
        IndicatorRetry retry{};
    };
    std::mutex mutex;
    NavigationPin pin;
    IndicatorRetry retry{};
    std::array<Image, 8> sizes{};
    size_t nextSize = 0;
};

bool DrawPinnedIndicator(HDC dc, const RECT& icon, const RECT& shortcut, NavigationPinCache& asset, auto loadPin) {
    if (!dc || IsRectEmpty(&shortcut)) return false;
    const std::scoped_lock lock(asset.mutex);
    auto& pin = asset.pin;
    if (pin.pixels.empty()) {
        const auto now = GetTickCount64();
        if (now < asset.retry.time) return false;
        pin = loadPin(dc);
        if (pin.pixels.empty()) {
            asset.retry.AfterFailure(dc, now);
            return false;
        }
    }
    const int extent = std::min(shortcut.right - shortcut.left, shortcut.bottom - shortcut.top);
    const int nativeExtent = std::max(pin.width, pin.height);
    const int pinWidth = MulDiv(pin.width, extent, nativeExtent);
    const int pinHeight = MulDiv(pin.height, extent, nativeExtent);
    if (pinWidth <= 0 || pinHeight <= 0) return false;

    // Resample once per output size, then draw the cached icon at native size.
    auto scaled = std::ranges::find_if(asset.sizes, [&](const auto& entry) {
        return entry.width == pinWidth && entry.height == pinHeight;
    });
    if (scaled == asset.sizes.end()) {
        scaled = asset.sizes.begin() + asset.nextSize++ % asset.sizes.size();
        *scaled = {pinWidth, pinHeight};
    }
    if (!scaled->icon) {
        const auto now = GetTickCount64();
        if (now < scaled->retry.time) return false;
        scaled->icon = CreateNavigationPinIcon(pin, dc, pinWidth, pinHeight);
        if (!scaled->icon) {
            scaled->retry.AfterFailure(dc, now);
            return false;
        }
    }
    // Reflect the shortcut overlay's edge offsets onto the pin's visible bounds.
    const int x = icon.right - shortcut.left - pinWidth;
    const int y = icon.bottom - shortcut.bottom;
    const auto draw = [&] { return DrawIconEx(dc, x, y, scaled->icon.get(), pinWidth, pinHeight, 0, nullptr, DI_NORMAL) != FALSE; };
    const auto shortcutRight = icon.left + shortcut.right;
    const auto shortcutTop = icon.top + shortcut.top;
    if (x >= shortcutRight || y + pinHeight <= shortcutTop) return draw();

    // Keep the shortcut corner intact in the smallest views.
    const auto saved = SaveDC(dc);
    if (!saved) return false;
    ExcludeClipRect(dc, icon.left + shortcut.left, shortcutTop, shortcutRight, icon.top + shortcut.bottom);
    const bool painted = draw();
    RestoreDC(dc, saved);
    return painted;
}

bool DrawPinnedIndicator(HDC dc, const RECT& icon, const RECT& shortcut) {
    static NavigationPinCache cache;
    return DrawPinnedIndicator(dc, icon, shortcut, cache, LoadNavigationPin);
}

[[maybe_unused]] HRESULT DrawWithIndicator(const PinStore& store, IndicatorPins& pins, DrawItemImage original,
                                          const unsigned char* image, HDC dc, const RECT* rect, bool folderView) {
    const auto result = original(image, dc, rect);
    if (FAILED(result) || !folderView || !rect) return result;

    try {
        const auto path = ItemImagePath(image);
        if (path && pins.Contains(store, *path)) {
            DrawPinnedIndicator(dc, *rect, ShortcutOverlayBounds(image, dc, *rect));
        }
    } catch (...) {
        // Keep the standard icon if a property or pin snapshot is unavailable.
    }
    return result;
}

#ifndef EXPLORER_ITEM_PINS_TEST
IndicatorPins indicatorPins;
std::optional<PinStore> nativePinStore;

namespace ui = winrt::Microsoft::UI;
namespace xaml = ui::Xaml;
namespace controls = xaml::Controls;
using CommandVector = winrt::Windows::Foundation::Collections::IObservableVector<controls::ICommandBarElement>;
using Microsoft::WRL::ComPtr;

constexpr CLSID commandsTap{0x0489a391, 0x4585, 0x4d13, {0xa9, 0xef, 0x61, 0x7f, 0x10, 0x78, 0x9a, 0x2a}};
constexpr wchar_t buttonName[] = L"ExplorerItemPinsAction";

// ShellTabWindowClass returns its borrowed IShellBrowser through this shell
// message. Keep the pointer on the tab's owning thread.
ComPtr<IShellView> ActiveShellView(HWND window) {
    constexpr UINT getShellBrowser = WM_USER + 7;
    for (HWND tab = nullptr; (tab = FindWindowExW(window, tab, L"ShellTabWindowClass", nullptr));) {
        if (!IsWindowVisible(tab) || GetWindowThreadProcessId(tab, nullptr) != GetCurrentThreadId()) continue;
        const auto native = reinterpret_cast<IShellBrowser*>(SendMessageW(tab, getShellBrowser, 0, 0));
        if (!native) continue;
        const ComPtr<IShellBrowser> browser(native);
        ComPtr<IShellView> view;
        HWND viewWindow = nullptr;
        if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view && SUCCEEDED(view->GetWindow(&viewWindow)) &&
            IsWindowVisible(viewWindow) && IsChild(tab, viewWindow)) return view;
    }
    return {};
}

std::optional<ExplorerSelection> ViewSelection(const ComPtr<IShellView>& shell) {
    ComPtr<IFolderView2> folder;
    if (!shell || FAILED(shell.As(&folder))) return std::nullopt;
    return CaptureSingleSelection(*folder.Get());
}

void RefreshCommandFolder(HWND window, const fs::path& target) {
    const auto shell = ActiveShellView(window);
    ComPtr<IFolderView2> view;
    ComPtr<IPersistFolder2> folder;
    if (!shell || FAILED(shell.As(&view)) || FAILED(view->GetFolder(IID_PPV_ARGS(&folder)))) return;
    const auto path = FileSystemFolderPath(*folder.Get());
    if (path && CompareNames(FolderKey(*path), FolderKey(target)) == CSTR_EQUAL) shell->Refresh();
}

HWND CommandWindow(const controls::CommandBar& bar) {
    if (!bar.XamlRoot()) return nullptr;
    const auto island = bar.XamlRoot().ContentIslandEnvironment();
    if (!island) return nullptr;
    const auto id = reinterpret_cast<HWND>(island.AppWindowId().Value);
    const auto window = GetAncestor(id, GA_ROOTOWNER);
    DWORD owner = 0;
    GetWindowThreadProcessId(window, &owner);
    wchar_t name[32]{};
    if (owner != GetCurrentProcessId() || !GetClassNameW(window, name, ARRAYSIZE(name)) ||
        std::wstring_view(name) != L"CabinetWClass") return nullptr;
    return window;
}

xaml::Controls::IconElement CommandIcon(PinAction action, xaml::ElementTheme theme) {
    wchar_t windows[MAX_PATH]{};
    if (!GetWindowsDirectoryW(windows, ARRAYSIZE(windows))) return nullptr;
    std::wstring path = windows;
    path += LR"(\SystemApps\MicrosoftWindows.Client.FileExp_cw5n1h2txyewy\FileExplorerExtensions\Assets\images\)";
    HIGHCONTRASTW contrast{};
    contrast.cbSize = sizeof(contrast);
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        path += GetRValue(GetSysColor(COLOR_WINDOW)) < 128 ? L"contrast-black\\" : L"contrast-white\\";
    } else {
        path += theme == xaml::ElementTheme::Dark ? L"contrast-standard\\theme-dark\\" : L"contrast-standard\\theme-light\\";
    }
    path += action == PinAction::Pin ? L"PinToStartScreen.svg" : L"windows.unpinfromstartscreen.svg";
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) return nullptr;
    std::ranges::replace(path, L'\\', L'/');
    xaml::Media::Imaging::SvgImageSource svg;
    svg.UriSource(winrt::Windows::Foundation::Uri(L"file:///" + path));
    controls::ImageIcon icon;
    icon.Source(svg);
    icon.Width(16);
    icon.Height(16);
    return icon;
}

struct CommandPlacement {
    CommandVector items;
    uint32_t index;
    controls::AppBarButton neighbor;
};

void InheritCommandStyle(const controls::AppBarButton& command, const controls::AppBarButton& native) {
    const std::pair<xaml::DependencyProperty, PCWSTR> properties[]{
        {xaml::FrameworkElement::StyleProperty(), L"Style"},
        {controls::Control::TemplateProperty(), L"Template"},
        {controls::Control::CornerRadiusProperty(), L"CornerRadius"},
        {xaml::FrameworkElement::WidthProperty(), L"Width"},
        {xaml::FrameworkElement::HeightProperty(), L"Height"},
        {xaml::FrameworkElement::MinWidthProperty(), L"MinWidth"},
        {xaml::FrameworkElement::MinHeightProperty(), L"MinHeight"},
        {xaml::FrameworkElement::HorizontalAlignmentProperty(), L"HorizontalAlignment"},
        {controls::Control::HorizontalContentAlignmentProperty(), L"HorizontalContentAlignment"},
        {controls::Control::VerticalContentAlignmentProperty(), L"VerticalContentAlignment"},
        {controls::Control::PaddingProperty(), L"Padding"},
        {xaml::FrameworkElement::MarginProperty(), L"Margin"},
    };
    for (const auto& [property, name] : properties) {
        xaml::Data::Binding binding;
        binding.Source(native);
        binding.Path(xaml::PropertyPath(name));
        binding.Mode(xaml::Data::BindingMode::OneWay);
        command.SetBinding(property, binding);
    }
}

std::optional<CommandPlacement> FindCommandPlacement(const controls::CommandBar& bar, bool context) {
    const auto primary = bar.PrimaryCommands();
    if (!context) {
        // This command bar's editing buttons are followed by a separator and
        // the named Sort and Group command, independent of the display language.
        if (primary.Size() < 10 || !primary.GetAt(8).try_as<controls::AppBarSeparator>()) return std::nullopt;
        const auto sort = primary.GetAt(9).try_as<controls::AppBarButton>();
        const auto share = primary.GetAt(6).try_as<controls::AppBarButton>();
        const auto remove = primary.GetAt(7).try_as<controls::AppBarButton>();
        if (!sort || sort.Name() != L"SortAndGroupButton" || !share || !remove) return std::nullopt;
        return CommandPlacement{primary, 7, remove};
    }

    for (const auto* verb : {L"PinToStartScreen", L"pintohomefile", L"pintohome", L"Windows.CompressTo"}) {
        for (const auto& items : {primary, bar.SecondaryCommands()}) {
            for (uint32_t i = 0; i < items.Size(); ++i) {
                const auto item = items.GetAt(i).try_as<controls::AppBarButton>();
                if (item && item.Name() == verb) {
                    const auto index = i + (std::wstring_view(verb) == L"Windows.CompressTo" ? 0 : 1);
                    return CommandPlacement{items, index, item};
                }
            }
        }
    }
    return std::nullopt;
}

bool SameSelection(const ExplorerSelection& left, const ExplorerSelection& right) {
    return left.folder == right.folder && left.item == right.item;
}

std::optional<PinAction> CachedPinAction(const ExplorerSelection& target) {
    const auto cached = indicatorPins.Cached(target.item);
    if (!cached) return std::nullopt;
    return *cached ? PinAction::Unpin : PinAction::Pin;
}

double NativeIconOpacity(const controls::CommandBar& bar, bool enabled) {
    for (const auto& commands : {bar.PrimaryCommands(), bar.SecondaryCommands()}) {
        for (const auto& entry : commands) {
            const auto button = entry.try_as<controls::AppBarButton>();
            if (!button || button.Name() == buttonName || button.IsEnabled() != enabled) continue;
            if (const auto image = button.Icon().try_as<controls::ImageIcon>()) {
                const auto opacity = image.Opacity();
                if (enabled || opacity < 1) return opacity;
            }
        }
    }
    return 1;
}

struct AttachedPinCommand;

void CloseContextMenu(const controls::CommandBar& bar) noexcept try {
    if (!bar) return;
    const auto root = bar.XamlRoot();
    if (!root) return;
    using xaml::Media::VisualTreeHelper;
    const auto popups = VisualTreeHelper::GetOpenPopupsForXamlRoot(root);
    for (auto node = xaml::DependencyObject(bar); node; node = VisualTreeHelper::GetParent(node)) {
        for (const auto& popup : popups) {
            if (popup.Child() == node) {
                popup.IsOpen(false);
                return;
            }
        }
    }
} catch (...) {}

struct ViewSelectionEvents : winrt::implements<ViewSelectionEvents, DShellFolderViewEvents> {
    std::weak_ptr<AttachedPinCommand> command;

    explicit ViewSelectionEvents(std::weak_ptr<AttachedPinCommand> binding) : command(std::move(binding)) {}

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* count) noexcept override {
        *count = 0;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) noexcept override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*) noexcept override {
        return DISP_E_UNKNOWNNAME;
    }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID member, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*,
                                     EXCEPINFO*, UINT*) noexcept override;
};

struct CommandUi {
    PinStore store;
    const bool context;
    const bool toolbar;
    FileHandle stop;
    std::atomic_bool active{true};
    std::atomic_bool observing{false};
    std::mutex mutex;
    std::vector<std::shared_ptr<AttachedPinCommand>> buttons;

    CommandUi(PinStore pins, bool menu, bool bar)
        : store(std::move(pins)), context(menu), toolbar(bar),
          stop(CreateEventW(nullptr, TRUE, FALSE, nullptr), CloseHandle) {}
};

struct AttachedPinCommand : std::enable_shared_from_this<AttachedPinCommand> {
    std::weak_ptr<CommandUi> owner;
    winrt::weak_ref<controls::CommandBar> bar;
    winrt::weak_ref<controls::AppBarButton> button;
    winrt::weak_ref<controls::AppBarButton> neighbor;
    ui::Dispatching::DispatcherQueue dispatcher{nullptr};
    ui::Dispatching::DispatcherQueueTimer timer{nullptr};
    winrt::event_token click{};
    winrt::event_token tick{};
    winrt::event_token selectionChanged{};
    winrt::event_token themeChanged{};
    HWND window = nullptr;
    HWND observedView = nullptr;
    winrt::agile_ref<IConnectionPoint> selectionEvents;
    DWORD selectionCookie = 0;
    bool context = false;
    std::optional<ExplorerSelection> selected;
    PinAction action = PinAction::Pin;
    std::optional<xaml::ElementTheme> appliedTheme;
    controls::IconElement pinIcon{nullptr};
    controls::IconElement unpinIcon{nullptr};
    ULONGLONG lastRead = 0;
    uint64_t readGeneration = 0;
    bool resolving = false;
    bool writing = false;

    void StopObservingSelection() {
        if (const auto cookie = std::exchange(selectionCookie, 0)) {
            if (const auto point = selectionEvents.get()) point->Unadvise(cookie);
        }
        selectionEvents = {};
        observedView = nullptr;
    }

    void ObserveSelection(const ComPtr<IShellView>& view) noexcept try {
        HWND current = nullptr;
        if (view) view->GetWindow(&current);
        if (context || observedView == current) return;
        StopObservingSelection();
        observedView = current;
        if (!current) return;

        ComPtr<IDispatch> automation;
        ComPtr<IConnectionPointContainer> events;
        winrt::com_ptr<IConnectionPoint> point;
        if (FAILED(view->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&automation))) || FAILED(automation.As(&events)) ||
            FAILED(events->FindConnectionPoint(DIID_DShellFolderViewEvents, point.put()))) return;
        const auto sink = winrt::make_self<ViewSelectionEvents>(weak_from_this());
        auto agile = winrt::make_agile(point);
        if (SUCCEEDED(point->Advise(sink.get(), &selectionCookie))) selectionEvents = std::move(agile);
    } catch (...) {}

    void Present(bool ready) {
        const auto state = owner.lock();
        const auto control = button.get();
        const auto commandBar = bar.get();
        if (!state || !state->active || !control || !commandBar) return;
        const auto theme = commandBar.ActualTheme();
        const auto visibleAction = ready ? action : PinAction::Pin;
        const auto text = LocalizedPinText(GetUserDefaultUILanguage());
        const auto title = visibleAction == PinAction::Pin ? text.pin : text.unpin;
        if (appliedTheme != theme) {
            pinIcon = CommandIcon(PinAction::Pin, theme);
            unpinIcon = CommandIcon(PinAction::Unpin, theme);
            appliedTheme = theme;
        }
        const auto& icon = visibleAction == PinAction::Pin ? pinIcon : unpinIcon;
        if (!icon) { control.IsEnabled(false); return; }
        if (control.Label() != title) {
            control.Label(title);
            xaml::Automation::AutomationProperties::SetName(control, title);
            if (!context) controls::ToolTipService::SetToolTip(control, winrt::box_value(winrt::hstring(title)));
        }
        const auto opacity = NativeIconOpacity(commandBar, ready);
        if (icon.Opacity() != opacity) icon.Opacity(opacity);
        if (control.Icon() != icon) control.Icon(icon);
        if (control.IsEnabled() != ready) control.IsEnabled(ready);
    }

    void FinishReading(uint64_t generation, const ExplorerSelection& target, std::optional<PinAction> result) noexcept try {
        if (generation != readGeneration) return;
        resolving = false;
        const auto state = owner.lock();
        if (!state || !state->active || writing) return;
        const auto current = ViewSelection(ActiveShellView(window));
        if (!current || !SameSelection(*current, target)) {
            if (!context) Update(true);
            return;
        }
        if (result) action = *result;
        lastRead = result ? GetTickCount64() : 0;
        Present(result.has_value());
    } catch (...) {}

    void Update(bool force = false) noexcept try {
        const auto state = owner.lock();
        const auto control = button.get();
        if (!state || !state->active || !control) return;
        const auto view = ActiveShellView(window);
        ObserveSelection(view);
        auto next = ViewSelection(view);
        const bool same = next && selected && SameSelection(*selected, *next);
        if (!same) {
            selected = next;
            lastRead = 0;
            if (next) {
                if (const auto cached = CachedPinAction(*next)) {
                    action = *cached;
                    lastRead = GetTickCount64();
                }
            }
        }
        Present(next.has_value() && lastRead != 0);
        if (!next || resolving || writing || (!force && lastRead && GetTickCount64() - lastRead < 1000)) return;
        resolving = QueueModuleWork([state, weak = weak_from_this(), queue = dispatcher,
                                      generation = ++readGeneration, target = std::move(*next)] {
            if (!state->active) return;
            std::optional<PinAction> result;
            try {
                result = CachedPinAction(target);
                if (!result) {
                    const auto revision = indicatorPins.Revision();
                    auto names = ReadPinNames(state->store, target.folder);
                    result = names.contains(target.item.filename().native()) ? PinAction::Unpin : PinAction::Pin;
                    const auto published = indicatorPins.Publish(target.folder, std::move(names), revision);
                    if (!published) result = GetPinAction(state->store, target);
                    else if (*published) RedrawExplorerItemIcons();
                }
            } catch (...) {}
            queue.TryEnqueue([weak, generation, target, result] {
                if (const auto command = weak.lock()) command->FinishReading(generation, target, result);
            });
        });
    } catch (...) {
        try { if (const auto control = button.get()) control.IsEnabled(false); } catch (...) {}
    }

    void Invoke() noexcept try {
        const auto state = owner.lock();
        if (!state || !state->active || writing || !lastRead) return;
        const auto current = ViewSelection(ActiveShellView(window));
        if (!current || !selected || !SameSelection(*current, *selected)) {
            if (!context) Update(true);
            return;
        }
        if (context) CloseContextMenu(bar.get());
        writing = QueueModuleWork([state, weak = weak_from_this(), queue = dispatcher, target = *current,
                                    window = window, action = action] {
            if (!state->active) return;
            bool changed = false;
            try { changed = ApplyPinAction(state->store, target, action); } catch (...) {}
            if (changed) {
                indicatorPins.Invalidate(target.folder);
                SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, target.folder.c_str(), nullptr);
            }
            queue.TryEnqueue([state, weak, window, folder = target.folder, changed]() noexcept {
                if (!state->active) return;
                try { if (changed) RefreshCommandFolder(window, folder); } catch (...) {}
                if (const auto command = weak.lock()) {
                    command->writing = false;
                    command->lastRead = 0;
                    command->Update(true);
                }
            });
        });
        if (writing) {
            ++readGeneration;
            resolving = false;
        }
    } catch (...) {}

    void Remove() noexcept try {
        StopObservingSelection();
        if (const auto anchor = neighbor.get()) anchor.IsEnabledChanged(selectionChanged);
        if (!context) {
            if (const auto commandBar = bar.get()) commandBar.ActualThemeChanged(themeChanged);
        }
        if (timer) {
            timer.Stop();
            timer.Tick(tick);
            timer = nullptr;
        }
        const auto control = button.get();
        const auto commandBar = bar.get();
        if (!control || !commandBar) return;
        control.Click(click);
        for (const auto& items : {commandBar.PrimaryCommands(), commandBar.SecondaryCommands()}) {
            uint32_t index = 0;
            if (items.IndexOf(control, index)) items.RemoveAt(index);
        }
    } catch (...) {}
};

HRESULT STDMETHODCALLTYPE ViewSelectionEvents::Invoke(DISPID member, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*,
                                                      EXCEPINFO*, UINT*) noexcept try {
    if (member == DISPID_SELECTIONCHANGED) {
        if (const auto binding = command.lock()) {
            binding->dispatcher.TryEnqueue([weak = command] {
                if (const auto item = weak.lock()) item->Update();
            });
        }
    }
    return S_OK;
} catch (...) { return winrt::to_hresult(); }

void InsertPinCommand(const std::shared_ptr<CommandUi>& state, const controls::CommandBar& bar, bool context) noexcept try {
    if (!state->active || (context ? !state->context : !state->toolbar)) return;
    for (const auto& commands : {bar.PrimaryCommands(), bar.SecondaryCommands()}) {
        for (uint32_t i = 0; i < commands.Size(); ++i) {
            const auto element = commands.GetAt(i).try_as<controls::AppBarButton>();
            if (element && element.Name() == buttonName) {
                std::shared_ptr<AttachedPinCommand> binding;
                {
                    const std::scoped_lock lock(state->mutex);
                    const auto found = std::ranges::find_if(state->buttons, [&](const auto& item) {
                        return item->dispatcher.HasThreadAccess() && item->button.get() == element;
                    });
                    if (found != state->buttons.end()) binding = *found;
                }
                if (binding) {
                    binding->Update();
                    return;
                }
                // A previous settings load can still be removing its button.
                commands.RemoveAt(i);
                break;
            }
        }
    }
    const auto placement = FindCommandPlacement(bar, context);
    const auto window = CommandWindow(bar);
    if (!placement || !window) return;

    controls::AppBarButton control;
    control.Name(buttonName);
    xaml::Automation::AutomationProperties::SetAutomationId(control, buttonName);
    const auto& native = placement->neighbor;
    InheritCommandStyle(control, native);
    control.IsCompact(!context);
    if (!context) control.LabelPosition(controls::CommandBarLabelPosition::Collapsed);
    auto command = std::make_shared<AttachedPinCommand>();
    command->owner = state;
    command->bar = winrt::make_weak(bar);
    command->button = winrt::make_weak(control);
    command->dispatcher = bar.DispatcherQueue();
    command->window = window;
    command->context = context;
    control.IsEnabled(false);
    command->Update(true);
    if (!control.Icon() || (context && !command->selected)) return;
    const std::weak_ptr<AttachedPinCommand> weak = command;
    command->click = control.Click([weak](auto const&, auto const&) { if (const auto item = weak.lock()) item->Invoke(); });
    placement->items.InsertAt(placement->index, control);

    if (!context) {
        command->neighbor = winrt::make_weak(native);
        command->selectionChanged = native.IsEnabledChanged([weak](auto const&, auto const&) {
            if (const auto item = weak.lock()) {
                item->dispatcher.TryEnqueue([weak] {
                    if (const auto command = weak.lock()) command->Update();
                });
            }
        });
        command->themeChanged = bar.ActualThemeChanged([weak](auto const&, auto const&) {
            if (const auto item = weak.lock()) item->Update();
        });
        command->timer = command->dispatcher.CreateTimer();
        command->timer.Interval(std::chrono::seconds(1));
        command->timer.IsRepeating(true);
        command->tick = command->timer.Tick([weak](auto const& timer, auto const&) {
            const auto item = weak.lock();
            if (!item || !item->button.get()) { timer.Stop(); return; }
            if (GetAncestor(GetForegroundWindow(), GA_ROOTOWNER) == item->window &&
                (item->selected || !item->selectionCookie || !IsWindowVisible(item->observedView))) item->Update();
        });
        command->timer.Start();
    }
    {
        const std::scoped_lock lock(state->mutex);
        std::erase_if(state->buttons, [](const auto& item) {
            return !IsWindow(item->window) || (item->dispatcher.HasThreadAccess() && !item->button.get());
        });
        if (state->active) {
            state->buttons.push_back(command);
            return;
        }
    }
    command->Remove();
} catch (...) {}

std::mutex commandUiMutex;
std::shared_ptr<CommandUi> commandUi;

struct CommandTreeWatcher : winrt::implements<CommandTreeWatcher, IVisualTreeServiceCallback2> {
    std::shared_ptr<CommandUi> state;
    winrt::com_ptr<IXamlDiagnostics> diagnostics;

    CommandTreeWatcher(std::shared_ptr<CommandUi> commands, winrt::com_ptr<IXamlDiagnostics> site)
        : state(std::move(commands)), diagnostics(std::move(site)) {}

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation, VisualElement element,
                                                 VisualMutationType mutation) noexcept override try {
        if (mutation != Add || !state->active || !element.Type) return S_OK;
        const std::wstring_view type(element.Type);
        const bool popup = type == L"Microsoft.UI.Xaml.Controls.Primitives.CommandBarFlyoutCommandBar";
        if (popup ? !state->context : (!state->toolbar || type != L"Microsoft.UI.Xaml.Controls.CommandBar")) return S_OK;
        winrt::com_ptr<::IInspectable> object;
        if (FAILED(diagnostics->GetIInspectableFromHandle(element.Handle, object.put())) || !object) return S_OK;
        const auto bar = object.try_as<controls::CommandBar>();
        if (!bar || (!popup && bar.Name() != L"FileExplorerCommandBar")) return S_OK;
        bar.DispatcherQueue().TryEnqueue([state = state, weak = winrt::make_weak(bar), popup] {
            if (const auto current = weak.get()) InsertPinCommand(state, current, popup);
        });
        return S_OK;
    } catch (...) { return S_OK; }
};

template<typename Arguments, void (*Run)(Arguments&)>
bool StartCommandWorker(Arguments arguments) {
    struct Worker {
        HMODULE module;
        Arguments arguments;
        static DWORD WINAPI Entry(void* raw) noexcept {
            std::unique_ptr<Worker> work(static_cast<Worker*>(raw));
            const auto module = work->module;
            const HRESULT apartment = RoInitialize(RO_INIT_MULTITHREADED);
            if (SUCCEEDED(apartment)) {
                try { Run(work->arguments); } catch (...) {}
                work.reset();
                RoUninitialize();
            } else {
                work.reset();
            }
            FreeLibraryAndExitThread(module, 0);
        }
    };
    auto work = std::make_unique<Worker>(nullptr, std::move(arguments));
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            reinterpret_cast<PCWSTR>(Worker::Entry), &work->module)) return false;
    const auto context = work.release();
    const auto thread = CreateThread(nullptr, 0, Worker::Entry, context, 0, nullptr);
    if (!thread) {
        FreeLibrary(context->module);
        delete context;
        return false;
    }
    CloseHandle(thread);
    return true;
}

struct TreeConnection {
    std::shared_ptr<CommandUi> state;
    winrt::com_ptr<IVisualTreeService> service;
    winrt::com_ptr<CommandTreeWatcher> watcher;
};

void WatchCommandTree(TreeConnection& connection) {
    if (!connection.state->active || FAILED(connection.service->AdviseVisualTreeChange(connection.watcher.get()))) {
        connection.state->observing = false;
        return;
    }
    WaitForSingleObject(connection.state->stop.get(), INFINITE);
    connection.service->UnadviseVisualTreeChange(connection.watcher.get());
    std::vector<std::shared_ptr<AttachedPinCommand>> buttons;
    {
        const std::scoped_lock lock(connection.state->mutex);
        buttons.swap(connection.state->buttons);
    }
    for (const auto& button : buttons) {
        try { button->dispatcher.TryEnqueue([button] { button->Remove(); }); } catch (...) {}
    }
}

struct CommandTapSite : winrt::implements<CommandTapSite, IObjectWithSite> {
    winrt::com_ptr<IUnknown> site;

    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* value) noexcept override try {
        site.copy_from(value);
        if (!site) return S_OK;
        auto diagnostics = site.as<IXamlDiagnostics>();
        std::shared_ptr<CommandUi> state;
        {
            const std::scoped_lock lock(commandUiMutex);
            state = commandUi;
        }
        if (!state || !state->active || !state->stop) return E_ABORT;
        if (state->observing.exchange(true)) return S_OK;
        TreeConnection connection{state, site.as<IVisualTreeService>(), winrt::make_self<CommandTreeWatcher>(state, diagnostics)};
        if (StartCommandWorker<TreeConnection, WatchCommandTree>(std::move(connection))) return S_OK;
        state->observing = false;
        return E_FAIL;
    } catch (...) { return winrt::to_hresult(); }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid, void** value) noexcept override { return site.as(iid, value); }
};

struct CommandTapFactory : winrt::implements<CommandTapFactory, IClassFactory> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID iid, void** value) noexcept override try {
        *value = nullptr;
        if (outer) return CLASS_E_NOAGGREGATION;
        // WinUI may release its TAP or queued delegates after Windhawk
        // unloads the mod. Their code must remain mapped until Explorer exits.
        HMODULE module = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                                reinterpret_cast<PCWSTR>(&commandUi), &module)) return HRESULT_FROM_WIN32(GetLastError());
        return winrt::make<CommandTapSite>().as(iid, value);
    } catch (...) { return winrt::to_hresult(); }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

// The show event wakes ConnectCommandTree while it waits for the first window.
void CALLBACK ExplorerWindowShown(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {}

bool HasExplorerWindow() {
    bool found = false;
    EnumWindows([](HWND window, LPARAM output) -> BOOL {
        if (!IsExplorerFolderWindow(window)) return TRUE;
        *reinterpret_cast<bool*>(output) = true;
        return FALSE;
    }, reinterpret_cast<LPARAM>(&found));
    return found;
}

void ConnectCommandTree(std::shared_ptr<CommandUi>& state) {
    HMODULE self = nullptr;
    wchar_t path[MAX_PATH]{};
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<PCWSTR>(ConnectCommandTree), &self) ||
        !GetModuleFileNameW(self, path, ARRAYSIZE(path))) return;
    const std::unique_ptr<std::remove_pointer_t<HWINEVENTHOOK>, decltype(&UnhookWinEvent)> windowEvents(
        SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, ExplorerWindowShown, GetCurrentProcessId(),
                        0, WINEVENT_OUTOFCONTEXT), UnhookWinEvent);
    const auto stop = state->stop.get();
    while (state->active) {
        const bool hasWindow = HasExplorerWindow();
        if (hasWindow) {
            const auto framework = GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll");
            const auto initialize = framework ? reinterpret_cast<decltype(&InitializeXamlDiagnosticsEx)>(
                GetProcAddress(framework, "InitializeXamlDiagnosticsEx")) : nullptr;
            if (initialize) {
                for (unsigned i = 1; i <= 128 && state->active; ++i) {
                    wchar_t endpoint[64]{};
                    swprintf_s(endpoint, L"WinUIVisualDiagConnection%u", i);
                    const auto result = initialize(endpoint, GetCurrentProcessId(), L"", path, commandsTap, nullptr);
                    if (result != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) return;
                }
            }
        }
        const DWORD timeout = hasWindow ? 500 : (windowEvents ? INFINITE : 1000);
        const auto result = MsgWaitForMultipleObjectsEx(1, &stop, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) return;
        if (result == WAIT_OBJECT_0 + 1) {
            MSG message{};
            while (state->active && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) DispatchMessageW(&message);
        }
    }
}

void StartNativeCommands() noexcept try {
    const bool menu = Wh_GetIntSetting(L"contextMenu") != 0;
    const bool bar = Wh_GetIntSetting(L"commandBar") != 0;
    if ((!menu && !bar) || !nativePinStore) return;
    auto state = std::make_shared<CommandUi>(*nativePinStore, menu, bar);
    if (!state->stop) return;
    {
        const std::scoped_lock lock(commandUiMutex);
        commandUi = state;
    }
    StartCommandWorker<std::shared_ptr<CommandUi>, ConnectCommandTree>(std::move(state));
} catch (...) {}

void StopNativeCommands() noexcept {
    std::shared_ptr<CommandUi> state;
    {
        const std::scoped_lock lock(commandUiMutex);
        state = std::exchange(commandUi, {});
    }
    if (state) {
        state->active = false;
        SetEvent(state->stop.get());
    }
}
#endif

struct StorageFunctions {
    NativeOrdering ordering;
    NativeGrouping grouping;
    DrawItemImage draw;
};

bool IsStorageCode(HMODULE storage, const auto address) {
    MEMORY_BASIC_INFORMATION region{};
    return address && VirtualQuery(reinterpret_cast<const void*>(address), &region, sizeof(region)) &&
           region.AllocationBase == storage && region.State == MEM_COMMIT &&
           !(region.Protect & PAGE_GUARD) &&
           (region.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
}

bool UsesStorageOperands(HMODULE storage, const auto address, std::initializer_list<std::string_view> operands) {
    if (!IsStorageCode(storage, address)) return false;
    const auto pc = reinterpret_cast<DWORD64>(address);
    DWORD64 base = 0;
    const auto function = RtlLookupFunctionEntry(pc, &base, nullptr);
    if (!function || base != reinterpret_cast<DWORD64>(storage) || base + function->BeginAddress != pc ||
        function->EndAddress <= function->BeginAddress) return false;

    // Decode only the beginning of the already resolved function. Check the
    // operands that expose its private layout, without using code to find addresses.
    const auto size = std::min<size_t>(function->EndAddress - function->BeginAddress, 1024);
    auto expected = operands.begin();
    auto* code = reinterpret_cast<unsigned char*>(address);
    constexpr size_t maxInstructionSize = 15;
    for (size_t offset = 0; offset + maxInstructionSize <= size; ) {
        WH_DISASM_RESULT instruction{};
        if (!Wh_Disasm(code + offset, &instruction) || !instruction.length || instruction.length > size - offset) break;
        if (instruction.length == expected->size() && std::memcmp(code + offset, expected->data(), instruction.length) == 0 &&
            ++expected == operands.end()) return true;
        offset += instruction.length;
    }
    return false;
}

std::optional<StorageFunctions> FindStorageSymbols(HMODULE storage, SetGrouping setGrouping, bool indicator) {
    using namespace std::string_view_literals;
    StorageFunctions native{};
    native.grouping.setGrouping = setGrouping;
    void* merge = nullptr;
    void* imageIndex = nullptr;
    // windows.storage.dll
    const WindhawkUtils::SYMBOL_HOOK hooks[]{
        {{L"static  _DSA_MergeSort()"}, &native.ordering.sort},
        {{L"static  _DSA_MergeSort2()"}, &merge},
        {{L"int __cdecl DVCompareColumns(struct tagITEMKEY const *,struct tagITEMKEY const *,__int64)"}, &native.ordering.compare},
        {{L"public: virtual long __cdecl CDVCategoryQueueItem::Run(struct IServiceProvider *)"}, &native.grouping.batch},
        {{L"public: virtual long __cdecl CBaseCategorizer::SetCategory(struct ICategorizableItem *,struct _tagpropertykey const &,struct IShellFolder2 *,enum SETCAT_FLAGS)"}, &native.grouping.assign},
        {{L"protected: virtual long __cdecl CBaseCategorizer::OnGetCategoryDescription(unsigned long,struct ICategoryDescription * *)"}, &native.grouping.rangeInfo},
        {{L"protected: virtual long __cdecl CDetailCategorizer::OnGetCategoryDescription(unsigned long,struct ICategoryDescription * *)"}, &native.grouping.textInfo},
        {{L"private: long __cdecl CDefCollection::_AddUpdateGroups(struct ICategoryDescriptionArray *,int *)"}, &native.grouping.apply},
        {{L"protected: long __cdecl CBaseCategorizer::AddCategory(unsigned long)"}, &native.grouping.remember},
        {{L"long __cdecl CreateCategoryDescriptionWithUIntSortOrder(unsigned long,unsigned __int64,struct _tagpropertykey const &,enum CATEGORYINFO_FLAGS,struct IFilterCondition *,unsigned short *,struct ICategoryDescription * *)"}, &native.grouping.make},
        {{L"protected: virtual long __cdecl CFilterConditionCategorizer::OnGetItemCategory(struct ICategorizableItem *,struct IShellFolder2 *,struct tagPROPVARIANT *)"}, &native.grouping.rangeCategory},
        {{L"protected: virtual long __cdecl CDetailCategorizer::OnGetItemCategory(struct ICategorizableItem *,struct IShellFolder2 *,struct tagPROPVARIANT *)"}, &native.grouping.textCategory},
        {{L"public: virtual long __cdecl CDefImageRenderer::Draw(struct HDC__ *,struct tagRECT const *)"}, &native.draw, nullptr, true},
        {{L"private: int __cdecl CDefImageRenderer::_GetImageIndex(void)"}, &imageIndex, nullptr, true},
    };
    constexpr size_t indicatorSymbolCount = 2;
    const auto count = std::size(hooks) - (indicator ? 0 : indicatorSymbolCount);
    if (!WindhawkUtils::HookSymbols(storage, hooks, count)) return std::nullopt;

    // Merge request count and scratch, callback and context, then the compare
    // context's folder and item store. The store's bind method is at slot 8.
    if (!UsesStorageOperands(storage, native.ordering.sort, {"\x8b\x29"sv, "\x48\x63\x59\x10"sv, "\x4c\x8d\x77\x28"sv}) ||
        !UsesStorageOperands(storage, merge, {"\x48\x8b\xd9"sv, "\x48\x8b\x43\x18"sv, "\x4c\x8b\x43\x20"sv}) ||
        !UsesStorageOperands(storage, native.ordering.compare,
                             {"\x49\x8b\xf8"sv, "\x48\x8b\x4f\x08"sv, "\x4c\x8b\x07"sv, "\x48\x8b\x40\x40"sv})) {
        Wh_Log(L"The Windows.Storage sort request or comparer layout has changed");
        return std::nullopt;
    }

    // Verify the category, row and group-array calls the grouping hooks mirror.
    constexpr auto slot3 = "\x48\x8b\x40\x18"sv;
    if (!UsesStorageOperands(storage, native.grouping.assign, {"\x48\x8b\x40\x30"sv, "\x48\x8b\x40\x28"sv}) ||
        !UsesStorageOperands(storage, native.grouping.rangeCategory, {slot3}) ||
        !UsesStorageOperands(storage, native.grouping.textCategory, {slot3}) ||
        !UsesStorageOperands(storage, native.grouping.apply, {slot3, "\x48\x8b\x40\x20"sv, slot3}) ||
        !IsStorageCode(storage, native.grouping.remember) || !IsStorageCode(storage, native.grouping.make)) {
        Wh_Log(L"The Windows.Storage grouping interfaces have changed");
        return std::nullopt;
    }

    // The image services and item offsets, plus the compositor's draw/fill slots.
    if (indicator && (!UsesStorageOperands(storage, native.draw,
            {"\x4c\x8b\xf1"sv, "\x49\x8b\x4e\x20"sv, "\x48\x8b\x80\x80\0\0\0"sv,
             "\x49\x8b\x56\x28"sv, "\x48\x8b\x80\x88\0\0\0"sv}) ||
        !UsesStorageOperands(storage, imageIndex, {"\x48\x8b\x49\x28"sv, "\x48\x8b\x01"sv, slot3}))) {
        native.draw = nullptr;
        Wh_Log(L"The Windows.Storage indicator renderer layout has changed");
    }
    return native;
}

[[maybe_unused]] std::optional<StorageFunctions> FindStorageFunctions(HMODULE storage, SetGrouping setGrouping, bool indicator) {
    if (!storage || !setGrouping) return std::nullopt;
    if (const auto ordering = FindNativeOrdering(storage)) {
        if (const auto grouping = FindNativeGrouping(storage, setGrouping)) {
            return StorageFunctions{*ordering, *grouping, indicator ? FindKnownImageRenderer(storage) : nullptr};
        }
    }
    return FindStorageSymbols(storage, setGrouping, indicator);
}

[[maybe_unused]] SetGrouping FindSetGrouping(HMODULE shell) {
    if (const auto known = FindKnownSetGrouping(shell)) return known;
    SetGrouping setGrouping = nullptr;
    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[]{
        {{L"public: virtual long __cdecl CDefView::SetGroupBy(struct _tagpropertykey const &,int)"}, &setGrouping},
    };
    return shell && WindhawkUtils::HookSymbols(shell, shell32DllHooks, std::size(shell32DllHooks)) ? setGrouping : nullptr;
}

[[maybe_unused]] PaintFolderImage FindFolderImagePaint(HMODULE frame) {
    if (const auto known = FindKnownFolderImagePaint(frame)) return known;
    PaintFolderImage paint = nullptr;
    const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[]{
        {{L"public: virtual void __cdecl UIImage::Paint(struct HDC__ *,struct tagRECT const *,struct tagRECT const *,struct tagRECT *,struct tagRECT *)"},
         &paint},
    };
    return frame && WindhawkUtils::HookSymbols(frame, explorerFrameDllHooks, std::size(explorerFrameDllHooks)) ? paint : nullptr;
}

#ifndef EXPLORER_ITEM_PINS_TEST
SortViewBatch nativeSortOriginal = nullptr;
CompareViewItems nativeCompare = nullptr;
NativeGrouping nativeGrouping{};
SetGrouping nativeSetGroupingTarget = nullptr;
DrawItemImage nativeImageOriginal = nullptr;
DrawItemImage inactiveIndicatorHook = nullptr;
PaintFolderImage nativeFolderImagePaintOriginal = nullptr;
HMODULE ownedExplorerFrame = nullptr;

BOOL NativeSortHook(ViewSortBatch* batch) {
    return SortWithPins(*nativePinStore, nativeSortOriginal, nativeCompare, batch);
}

HRESULT NativeGroupBatchHook(const unsigned char* batch, void* result) {
    return GroupWithPins(*nativePinStore, nativeGrouping.batch, batch, result);
}

HRESULT NativeAssignGroupHook(IUnknown* category, IUnknown* row, void* item, IShellFolder2* context, DWORD flags) {
    return AssignWithPins(nativeGrouping, category, row, item, context, flags);
}

HRESULT NativeRangeInfoHook(IUnknown* category, DWORD group, IUnknown** output) {
    return GroupInfoWithPins(nativeGrouping.make, nativeGrouping.rangeInfo, category, group, output);
}

HRESULT NativeTextInfoHook(IUnknown* category, DWORD group, IUnknown** output) {
    return GroupInfoWithPins(nativeGrouping.make, nativeGrouping.textInfo, category, group, output);
}

HRESULT NativeApplyGroupsHook(const unsigned char* view, GroupList* groups, UINT* added) {
    return ApplyWithPins(nativeGrouping, view, groups, added);
}

HRESULT NativeSetGroupingHook(IFolderView2* view, REFPROPERTYKEY key, BOOL ascending) {
    return SetGroupingWithPins(*nativePinStore, nativeSetGroupingTarget, nativeGrouping.setGrouping, view, key, ascending);
}

void NativeFolderImagePaintHook(void* image, HDC dc, const RECT* bounds, const RECT* clip, RECT* content, RECT* paint) {
    const FolderImagePaintScope scope;
    nativeFolderImagePaintOriginal(image, dc, bounds, clip, content, paint);
}

HRESULT NativeImageHook(const unsigned char* image, HDC dc, const RECT* rect) {
    return DrawWithIndicator(*nativePinStore, indicatorPins, nativeImageOriginal, image, dc, rect,
                             FolderImagePaintScope::Active());
}

void HookNativeIndicator(DrawItemImage draw) {
    if (!draw) return;
    auto frame = GetModuleHandleW(L"ExplorerFrame.dll");
    if (!frame) {
        ownedExplorerFrame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        frame = ownedExplorerFrame;
    }
    const auto paint = FindFolderImagePaint(frame);
    if (!paint) return;
    if (!WindhawkUtils::SetFunctionHook(draw, NativeImageHook, &nativeImageOriginal)) {
        Wh_Log(L"Could not hook the pinned indicator renderer");
        return;
    }
    if (!WindhawkUtils::SetFunctionHook(paint, NativeFolderImagePaintHook, &nativeFolderImagePaintOriginal)) {
        inactiveIndicatorHook = draw;
        Wh_Log(L"Could not hook the folder image painter");
    }
}
#endif

}  // namespace

}  // namespace explorer_item_pins

#ifndef EXPLORER_ITEM_PINS_TEST
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
extern "C" __declspec(dllexport) HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID iid, void** object) try {
    using namespace explorer_item_pins;
    *object = nullptr;
    if (clsid != commandsTap) return CLASS_E_CLASSNOTAVAILABLE;
    return winrt::make<CommandTapFactory>().as(iid, object);
} catch (...) { return winrt::to_hresult(); }
#pragma clang diagnostic pop

void Wh_ModAfterInit() {
    using namespace explorer_item_pins;
    // Windhawk allows removing hooks once Wh_ModInit has returned.
    if (const auto target = std::exchange(inactiveIndicatorHook, nullptr);
        target && Wh_RemoveFunctionHook(reinterpret_cast<void*>(target))) Wh_ApplyHookOperations();
    StartNativeCommands();
}
void Wh_ModBeforeUninit() { explorer_item_pins::StopNativeCommands(); }
void Wh_ModUninit() {
    if (auto frame = std::exchange(explorer_item_pins::ownedExplorerFrame, nullptr)) FreeLibrary(frame);
}
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
BOOL Wh_ModInit() {
    using namespace explorer_item_pins;
    using WindhawkUtils::SetFunctionHook;
    const auto storage = GetModuleHandleW(L"Windows.Storage.dll");
    const auto setGrouping = FindSetGrouping(GetModuleHandleW(L"Shell32.dll"));
    const auto native = FindStorageFunctions(storage, setGrouping, Wh_GetIntSetting(L"pinIndicator") != 0);
    if (!native) {
        Wh_Log(L"Compatible sorting and grouping functions are unavailable");
        return FALSE;
    }

    try {
        nativePinStore.emplace(PinStore::DefaultPath());
    } catch (...) {
        return FALSE;
    }
    nativeCompare = native->ordering.compare;
    nativeGrouping = native->grouping;
    nativeSetGroupingTarget = setGrouping;
    if (!SetFunctionHook(native->ordering.sort, NativeSortHook, &nativeSortOriginal) ||
        !SetFunctionHook(native->grouping.batch, NativeGroupBatchHook, &nativeGrouping.batch) ||
        !SetFunctionHook(native->grouping.assign, NativeAssignGroupHook, &nativeGrouping.assign) ||
        !SetFunctionHook(native->grouping.rangeInfo, NativeRangeInfoHook, &nativeGrouping.rangeInfo) ||
        !SetFunctionHook(native->grouping.textInfo, NativeTextInfoHook, &nativeGrouping.textInfo) ||
        !SetFunctionHook(native->grouping.apply, NativeApplyGroupsHook, &nativeGrouping.apply) ||
        !SetFunctionHook(setGrouping, NativeSetGroupingHook, &nativeGrouping.setGrouping)) return FALSE;
    HookNativeIndicator(native->draw);
    return TRUE;
}
#endif
