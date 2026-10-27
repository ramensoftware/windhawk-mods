// ==WindhawkMod==
// @id              nested-tray-drawer
// @name            Nested Tray Drawer
// @description     Collect rarely used tray icons in a second drawer.
// @version         0.1.9
// @author          MEI
// @github          https://github.com/Meidios
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32 -lversion -ladvapi32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Nested Tray Drawer

**Experimental release. Only the inspected configuration below is supported.**

Add a second drawer inside Windows' hidden notification-icons drawer. Collect
rarely used icons there while retaining the original app icons and native input
handlers. Both drawers can remain visible together.

![Nested Tray Drawer feature overview](https://raw.githubusercontent.com/Meidios/nested-tray-drawer/0d4ae2b9b09aba9a907541d0855e86a55a51e711/assets/feature-overview-0edbded10469.png)

*Concept illustration; panel styling and example icons are illustrative.*

## Supported environment

This is an experimental release for one inspected Windows configuration:

| Component | Verified configuration |
| --- | --- |
| Windows | Windows 11 23H2, build 22631.5126, x64 |
| Windhawk | 1.7.3 |
| Taskbar.View.dll | 2125.4507.10.0 |
| taskbar.dll | PE timestamp 10195DD3; image size 3B6000 |

Other Windows builds, including other 23H2 updates, are not claimed as supported.
Windows 10, ARM64, 24H2 and 25H2 have not been validated. Unsupported
Taskbar.View.dll versions are rejected; unsupported identity layouts leave icons
visible. Do not remove the version checks to force installation on another build.

## Use

1. Click the normal taskbar chevron to open the hidden-icons drawer.
2. Drag an icon from that drawer onto the added chevron and release it. Repeat
   to collect more icons; earlier collected icons are retained.
3. Click the added chevron to open or close the second drawer. An empty drawer
   does not open. Use the real app icons and their context menus inside it.
4. Right-click the added chevron and choose **Restore all collected icons** to
   return all icons and explicitly clear the saved collection.

There is no right-click icon picker. Disabling the mod removes its UI and
restores the native tray while preserving saved membership.

## Saved records and switching installation source

Starting with 0.1.8, local and catalog builds share one per-Windows-user record:
`HKEY_CURRENT_USER\Software\Meidios\Windhawk\NestedTrayDrawer`, value
`CollectedIconsV1`. It is independent of Windhawk's `local@` mod-ID prefix.

On first use, existing valid records are imported from this mod's old Windhawk
storage. An installed Windhawk local copy can also be imported by the catalog
build. For a portable Windhawk copy, update the local mod to 0.1.8 first.
Existing destination records always win, including an explicitly cleared
collection; old backups are never merged back automatically. The old record is
retained as a backup and is not kept in two-way sync.

Disable the local copy before enabling the catalog copy. A per-process guard
prevents two copies of 0.1.8 or newer from attaching at once. Older copies do not
implement that guard and must remain disabled. Switching back to 0.1.7 or earlier
uses its old backup, which may no longer reflect your latest organization.

Membership is saved before icons are moved. Missing apps keep their records and
are matched when they return. A GUID identifies an icon when available; otherwise
its exact executable path and native UID are used. Ambiguous identities stay
visible. App updates that change those identifiers can require recollection.

Records contain app GUIDs or encoded executable paths and UIDs; encoding is not
encryption. The mod does not transmit records or change Windows' tray settings.
Uninstalling the mod leaves this user-owned record in place. Use **Restore all
collected icons** before uninstalling to clear it.

## Known limits and test coverage

- One app menu did not expose recognized native menu state and the drawers closed
  after the bounded menu-start grace. Other tested menus retained both drawers.
  Custom and non-foreground app menus are not universally supported.
- Drag collection, multiple icons, simultaneous drawers, recognized app menus,
  mod reload restoration and icon disappearance/reappearance were tested.
- A complete Explorer restart was not performed. The same initialization recovery
  path is implemented, but a full process-restart test is still outstanding.
- Full left-click/double-click/keyboard, drag-cancel, DPI, multiple-monitor and
  coexistence testing with other taskbar mods is not complete.
- There is no supported individual return gesture or drag out of the second
  drawer. The restore-all command returns every collected icon.
- Version 0.1.9 refines the footer tint, divider, spacing and rounded button.
  Footer appearance was checked on the inspected dark-theme configuration.
  Light/high-contrast appearance and live theme switching are not yet validated.

When reporting an issue, include Windows build, the two module versions, Windhawk
version and reproduction steps. Remove private paths or app identifiers from logs.

## Credits

Native overflow access is adapted from m417z's GPLv3
[Taskbar notification icon spacing](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-notification-icon-spacing.wh.cpp).
This mod retains its GPL-3.0 declaration. No external files are needed to compile
this single-file mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable the second drawer
  $description: Collect icons by dropping first-drawer icons onto the added chevron.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <commctrl.h>
#include <winver.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;
using Controls::Button;
using Controls::ContentPresenter;
using Controls::Flyout;
using Controls::Grid;
using Controls::ItemsControl;
using winrt::Windows::Foundation::IInspectable;

std::atomic<bool> g_unloading;
std::atomic<bool> g_enabled{true};
std::atomic<bool> g_runtimeAvailable{true};
std::atomic<int> g_hookState;  // 0: waiting, 1: resolving, 2: ready, 3: failed.
std::atomic<HWND> g_uiWindow;
std::atomic<DWORD> g_uiThread;

bool Chinese() {
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_CHINESE;
}

PCWSTR Text(PCWSTR en, PCWSTR zh) {
    return Chinese() ? zh : en;
}

void LogException(PCWSTR operation) noexcept {
    // Called only inside an exception handler.
    if (!g_runtimeAvailable)
        return;
    Wh_Log(L"%s failed: HRESULT=%08X", operation, static_cast<unsigned>(winrt::to_hresult()));
}

FrameworkElement FindDescendant(DependencyObject const& parent, PCWSTR className) {
    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        auto element = child.try_as<FrameworkElement>();
        if (element && winrt::get_class_name(element) == className)
            return element;
        if (auto found = FindDescendant(child, className))
            return found;
    }
    return nullptr;
}

struct SavedProperty {
    DependencyObject target{nullptr};
    DependencyProperty property{nullptr};
    IInspectable value{nullptr};
    Data::Binding binding{nullptr};

    SavedProperty() = default;
    SavedProperty(DependencyObject const& object, DependencyProperty const& key)
        : target(object), property(key), value(object.ReadLocalValue(key)) {
        if (auto element = object.try_as<FrameworkElement>()) {
            if (auto expression = element.GetBindingExpression(key))
                binding = expression.ParentBinding();
        }
    }

    void Restore() const {
        if (!target)
            return;
        if (binding)
            Data::BindingOperations::SetBinding(target, property, binding);
        else if (value == DependencyProperty::UnsetValue())
            target.ClearValue(property);
        else
            target.SetValue(property, value);
    }
};

struct IconCandidate {
    IInspectable model{nullptr};
};

constexpr UINT kCollectDroppedIconMessage = WM_APP + 0x732;
constexpr UINT kRefreshCollectionMessage = WM_APP + 0x733;
constexpr UINT kRefreshAppearanceMessage = WM_APP + 0x734;

// BEGIN COLLECTION RECORD
constexpr size_t kMaxCollectionChars = 262144;
constexpr size_t kMaxCollectionKeys = 512;

bool IsHex(wchar_t ch) {
    return (ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f');
}

bool ValidCollectionKey(std::wstring const& key) {
    if (key.starts_with(L"g:") && key.size() == 40) {
        if (key[2] != L'{' || key[39] != L'}')
            return false;
        for (size_t i = 3; i < 39; ++i) {
            if (i == 11 || i == 16 || i == 21 || i == 26) {
                if (key[i] != L'-')
                    return false;
            } else if (!IsHex(key[i]))
                return false;
        }
        return key != L"g:{00000000-0000-0000-0000-000000000000}";
    }
    if (!key.starts_with(L"p:") || key.size() > 32768)
        return false;
    auto colon = key.find(L':', 2);
    if (colon == std::wstring::npos || colon < 3 || colon > 12)
        return false;
    std::uint64_t uid{};
    for (size_t i = 2; i < colon; ++i) {
        if (key[i] < L'0' || key[i] > L'9')
            return false;
        uid = uid * 10 + key[i] - L'0';
        if (uid > 0xffffffffu)
            return false;
    }
    if (colon > 3 && key[2] == L'0')
        return false;
    if (key.size() == colon + 1 || (key.size() - colon - 1) % 4)
        return false;
    return std::all_of(key.begin() + colon + 1, key.end(), IsHex);
}

std::wstring PathCollectionKey(std::wstring const& lowerPath, uint32_t uid) {
    if (lowerPath.empty() || lowerPath.size() > 4096)
        return {};
    constexpr wchar_t digits[] = L"0123456789abcdef";
    std::wstring result = L"p:" + std::to_wstring(uid) + L":";
    for (wchar_t ch : lowerPath) {
        if (!ch || ch < 32 || ch > 0xffff)
            return {};
        for (int shift : {12, 8, 4, 0})
            result += digits[(ch >> shift) & 15];
    }
    return result;
}

bool ParseCollectionRecord(std::wstring const& text, std::vector<std::wstring>& keys) {
    keys.clear();
    if (text.empty())
        return true;  // A new installation has no stored value.
    if (text.size() > kMaxCollectionChars || !text.starts_with(L"NTD1\n"))
        return false;
    for (size_t pos = 5; pos < text.size();) {
        auto end = text.find(L'\n', pos);
        if (end == std::wstring::npos || end == pos || keys.size() >= kMaxCollectionKeys)
            return false;
        auto key = text.substr(pos, end - pos);
        if (!ValidCollectionKey(key) || std::find(keys.begin(), keys.end(), key) != keys.end())
            return false;
        keys.push_back(std::move(key));
        pos = end + 1;
    }
    return true;
}

bool SerializeCollectionRecord(std::vector<std::wstring> const& keys, std::wstring& text) {
    text = L"NTD1\n";
    if (keys.size() > kMaxCollectionKeys)
        return false;
    for (size_t i = 0; i < keys.size(); ++i) {
        if (!ValidCollectionKey(keys[i]) ||
            std::find(keys.begin(), keys.begin() + i, keys[i]) != keys.begin() + i)
            return false;
        text += keys[i] + L"\n";
        if (text.size() > kMaxCollectionChars)
            return false;
    }
    return true;
}

std::vector<size_t> MatchCollectionKeys(std::vector<std::wstring> const& live,
                                        std::vector<std::wstring> const& saved) {
    std::vector<size_t> matches;
    for (size_t i = 0; i < live.size(); ++i) {
        if (!live[i].empty() && std::count(live.begin(), live.end(), live[i]) == 1 &&
            std::find(saved.begin(), saved.end(), live[i]) != saved.end())
            matches.push_back(i);
    }
    return matches;
}
// END COLLECTION RECORD

constexpr WCHAR kCollectionValue[] = L"CollectedIconsV1";
HANDLE g_collectionMutex;
HANDLE g_instanceGuard;
bool g_cleanupCompleted;

// BEGIN INSTANCE GUARD
HANDLE AcquireDrawerInstance(DWORD processId) {
    auto name = L"Local\\Windhawk.NestedTrayDrawer.Instance.v1." + std::to_wstring(processId);
    SetLastError(ERROR_SUCCESS);
    auto handle = CreateEventW(nullptr, TRUE, FALSE, name.c_str());
    auto error = GetLastError();
    if (handle && error == ERROR_ALREADY_EXISTS) {
        CloseHandle(handle);
        return nullptr;
    }
    return handle;
}
// END INSTANCE GUARD

struct CollectionLock {
    bool acquired{};
    CollectionLock() {
        if (!g_collectionMutex)
            return;
        auto result = WaitForSingleObject(g_collectionMutex, 1000);
        acquired = result == WAIT_OBJECT_0 || result == WAIT_ABANDONED;
    }
    ~CollectionLock() {
        if (acquired)
            ReleaseMutex(g_collectionMutex);
    }
};

// BEGIN COLLECTION STORAGE
// Shared only by this mod's local/catalog copies, under the current user's hive.
// The destination's presence is the migration marker, including a valid empty
// collection. Never resurrect an older Windhawk backup after an explicit clear.
constexpr WCHAR kUserCollectionPath[] = L"Software\\Meidios\\Windhawk\\NestedTrayDrawer";
constexpr WCHAR kLegacyLocalStoragePath[] =
    L"SOFTWARE\\Windhawk\\Engine\\ModsWritable\\local@nested-tray-drawer\\LocalStorage";

enum class RecordState { Missing, Present, Error };
struct StoredCollection {
    RecordState state{RecordState::Missing};
    std::wstring text;
};

bool ValidStoredCollection(std::wstring const& text) {
    std::vector<std::wstring> keys;
    // An existing empty registry string is corrupt, not a missing destination.
    return !text.empty() && ParseCollectionRecord(text, keys);
}

StoredCollection ReadRegistryCollection(HKEY hive, PCWSTR path) {
    HKEY key{};
    auto error = RegOpenKeyExW(hive, path, 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key);
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
        return {};
    if (error != ERROR_SUCCESS)
        return {RecordState::Error, {}};
    DWORD bytes{}, type{};
    error = RegQueryValueExW(key, kCollectionValue, nullptr, &type, nullptr, &bytes);
    if (error == ERROR_FILE_NOT_FOUND) {
        RegCloseKey(key);
        return {};
    }
    if (error != ERROR_SUCCESS || type != REG_SZ || bytes < sizeof(wchar_t) ||
        bytes > (kMaxCollectionChars + 1) * sizeof(wchar_t) || bytes % sizeof(wchar_t)) {
        RegCloseKey(key);
        return {RecordState::Error, {}};
    }
    std::vector<wchar_t> buffer(bytes / sizeof(wchar_t));
    auto capacity = bytes;
    error = RegQueryValueExW(key, kCollectionValue, nullptr, &type,
                             reinterpret_cast<BYTE*>(buffer.data()), &bytes);
    RegCloseKey(key);
    if (error != ERROR_SUCCESS || type != REG_SZ || bytes != capacity || buffer.back() != L'\0')
        return {RecordState::Error, {}};
    std::wstring text(buffer.data(), buffer.size() - 1);
    if (text.find(L'\0') != std::wstring::npos || !ValidStoredCollection(text))
        return {RecordState::Error, {}};
    return {RecordState::Present, std::move(text)};
}

bool SaveRegistryCollection(HKEY hive, PCWSTR path, std::wstring const& text) {
    if (!ValidStoredCollection(text))
        return false;
    HKEY key{};
    if (RegCreateKeyExW(hive, path, 0, nullptr, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, nullptr, &key,
                        nullptr) != ERROR_SUCCESS)
        return false;
    auto result = RegSetValueExW(key, kCollectionValue, 0, REG_SZ,
                                 reinterpret_cast<const BYTE*>(text.c_str()),
                                 static_cast<DWORD>((text.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

enum class MigrationChoice { KeepDestination, ImportOwn, ImportLocal, CreateEmpty, Refuse };

MigrationChoice ChooseCollectionMigration(StoredCollection const& destination,
                                          StoredCollection const& own,
                                          StoredCollection const& local) {
    if (destination.state == RecordState::Error)
        return MigrationChoice::Refuse;
    if (destination.state == RecordState::Present)
        return ValidStoredCollection(destination.text) ? MigrationChoice::KeepDestination
                                                       : MigrationChoice::Refuse;
    if (own.state == RecordState::Error)
        return MigrationChoice::Refuse;
    if (own.state == RecordState::Present)
        return ValidStoredCollection(own.text) ? MigrationChoice::ImportOwn
                                               : MigrationChoice::Refuse;
    if (local.state == RecordState::Error)
        return MigrationChoice::Refuse;
    if (local.state == RecordState::Present)
        return ValidStoredCollection(local.text) ? MigrationChoice::ImportLocal
                                                 : MigrationChoice::Refuse;
    return MigrationChoice::CreateEmpty;
}

bool BootstrapCollectionAt(HKEY hive,
                           PCWSTR path,
                           StoredCollection const& own,
                           StoredCollection const& local,
                           MigrationChoice& choice) {
    auto destination = ReadRegistryCollection(hive, path);
    choice = ChooseCollectionMigration(destination, own, local);
    switch (choice) {
        case MigrationChoice::KeepDestination:
            return true;
        case MigrationChoice::ImportOwn:
            return SaveRegistryCollection(hive, path, own.text);
        case MigrationChoice::ImportLocal:
            return SaveRegistryCollection(hive, path, local.text);
        case MigrationChoice::CreateEmpty:
            return SaveRegistryCollection(hive, path, L"NTD1\n");
        case MigrationChoice::Refuse:
            return false;
    }
    return false;
}

bool ReadCollection(std::vector<std::wstring>& keys) {
    auto stored = ReadRegistryCollection(HKEY_CURRENT_USER, kUserCollectionPath);
    return stored.state == RecordState::Present && ParseCollectionRecord(stored.text, keys);
}

bool WriteCollection(std::vector<std::wstring> const& keys) {
    std::wstring text;
    return SerializeCollectionRecord(keys, text) &&
           SaveRegistryCollection(HKEY_CURRENT_USER, kUserCollectionPath, text);
}
// END COLLECTION STORAGE

bool InitializeCollectionStorage() {
    CollectionLock lock;
    if (!lock.acquired)
        return false;
    auto destination = ReadRegistryCollection(HKEY_CURRENT_USER, kUserCollectionPath);
    if (destination.state != RecordState::Missing) {
        if (destination.state == RecordState::Present)
            return true;
        Wh_Log(L"Existing user collection is invalid/unreadable; refusing to replace it");
        return false;
    }
    std::vector<wchar_t> buffer(kMaxCollectionChars + 1);
    auto size = Wh_GetStringValue(kCollectionValue, buffer.data(), buffer.size());
    StoredCollection own;
    if (size)
        own = {RecordState::Present, std::wstring(buffer.data(), size)};
    StoredCollection local;
    // Read only this mod's old namespace. Portable users can bootstrap by
    // updating their local copy first, whose own Wh_GetStringValue works there.
    if (own.state == RecordState::Missing)
        local = ReadRegistryCollection(HKEY_LOCAL_MACHINE, kLegacyLocalStoragePath);
    MigrationChoice choice;
    if (!BootstrapCollectionAt(HKEY_CURRENT_USER, kUserCollectionPath, own, local, choice)) {
        Wh_Log(L"Could not migrate collection; original Windhawk records left untouched");
        return false;
    }
    auto confirmed = ReadRegistryCollection(HKEY_CURRENT_USER, kUserCollectionPath);
    if (confirmed.state != RecordState::Present)
        return false;
    std::vector<std::wstring> keys;
    if (!ParseCollectionRecord(confirmed.text, keys))
        return false;
    Wh_Log(L"User collection ready: migration=%d entries=%zu", static_cast<int>(choice),
           keys.size());
    return true;
}

IInspectable NativeObjectProperty(IInspectable const& object, winrt::guid const& iid, size_t slot) {
    if (!object)
        return nullptr;
    winrt::com_ptr<IUnknown> iface;
    if (FAILED(reinterpret_cast<IUnknown*>(winrt::get_abi(object))
                   ->QueryInterface(reinterpret_cast<GUID const&>(iid), iface.put_void())))
        return nullptr;
    auto table = *reinterpret_cast<void***>(iface.get());
    using Getter = HRESULT(WINAPI*)(void*, void**);
    IInspectable result{nullptr};
    winrt::check_hresult(
        reinterpret_cast<Getter>(table[slot])(iface.get(), winrt::put_abi(result)));
    return result;
}

std::wstring GuidCollectionKey(GUID const& guid) {
    constexpr GUID empty{};
    if (IsEqualGUID(guid, empty))
        return {};
    wchar_t text[40]{};
    if (!StringFromGUID2(guid, text, ARRAYSIZE(text)))
        return {};
    for (auto& ch : text)
        if (ch >= L'A' && ch <= L'F')
            ch += L'a' - L'A';
    return std::wstring(L"g:") + text;
}

std::wstring StableIconKey(IInspectable const& model) {
    // Exact local IIDs/slots and the settings-ID field were verified from the
    // matching public PDBs and binary. Never apply this layout to another build.
    constexpr winrt::guid viewIid{
        0x916b7ac1, 0xb2ac, 0x52b1, {0xa2, 0xc1, 0xd2, 0x22, 0xf8, 0xab, 0x28, 0x99}};
    constexpr winrt::guid configIid{
        0x009ef3d8, 0x9cc1, 0x5550, {0xba, 0xe5, 0x98, 0xdc, 0x72, 0x68, 0x5d, 0x18}};
    constexpr winrt::guid modelIid{
        0x9ad44317, 0xfbb6, 0x5376, {0xa1, 0x4e, 0x2b, 0xb4, 0xfa, 0x66, 0xc7, 0x65}};
    constexpr GUID iconIid{
        0x6130d2bf, 0x470d, 0x5859, {0xa5, 0xaf, 0x95, 0x17, 0xeb, 0x7d, 0x37, 0xe7}};
    auto taskbar = GetModuleHandleW(L"taskbar.dll");
    if (!taskbar)
        return {};
    auto base = reinterpret_cast<const BYTE*>(taskbar);
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->FileHeader.TimeDateStamp != 0x10195dd3 || nt->OptionalHeader.SizeOfImage != 0x3b6000)
        return {};
    auto config = NativeObjectProperty(model, viewIid, 6);
    auto dataModel = NativeObjectProperty(config, configIid, 6);
    auto icon = NativeObjectProperty(dataModel, modelIid, 6);
    if (!icon)
        return {};
    winrt::com_ptr<IUnknown> native;
    if (FAILED(reinterpret_cast<IUnknown*>(winrt::get_abi(icon))
                   ->QueryInterface(iconIid, native.put_void())))
        return {};
    if (*reinterpret_cast<void**>(native.get()) != base + 0x26f068)
        return {};
    struct IdentityFields {
        uint32_t uid;
        GUID guid;
    } fields{};
    static_assert(sizeof(IdentityFields) == 20);
    std::uint64_t settingsId{};
    SIZE_T copied{};
    auto object = reinterpret_cast<const BYTE*>(native.get());
    if (!ReadProcessMemory(GetCurrentProcess(), object + 64, &fields, sizeof(fields), &copied) ||
        copied != sizeof(fields) ||
        !ReadProcessMemory(GetCurrentProcess(), object + 152, &settingsId, sizeof(settingsId),
                           &copied) ||
        copied != sizeof(settingsId))
        return {};
    if (auto key = GuidCollectionKey(fields.guid); !key.empty())
        return key;
    auto subkey = L"Control Panel\\NotifyIconSettings\\" + std::to_wstring(settingsId);
    DWORD uid{}, size = sizeof(uid);
    if (RegGetValueW(HKEY_CURRENT_USER, subkey.c_str(), L"UID", RRF_RT_REG_DWORD, nullptr, &uid,
                     &size) != ERROR_SUCCESS ||
        uid != fields.uid)
        return {};
    wchar_t path[4097]{};
    size = sizeof(path);
    if (RegGetValueW(HKEY_CURRENT_USER, subkey.c_str(), L"ExecutablePath",
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, path, &size) != ERROR_SUCCESS)
        return {};
    auto length = wcsnlen(path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path))
        return {};
    std::wstring lower(length, L'\0');
    if (!LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_LOWERCASE, path, static_cast<int>(length),
                       lower.data(), static_cast<int>(lower.size()), nullptr, nullptr, 0))
        return {};
    return PathCollectionKey(lower, uid);
}

// BEGIN CHEVRON DROP POLICY
bool AcceptChevronDrop(bool ownedSource,
                       bool samePointer,
                       bool available,
                       double x,
                       double y,
                       double width,
                       double height) {
    return ownedSource && samePointer && available && std::isfinite(x) && std::isfinite(y) &&
           std::isfinite(width) && std::isfinite(height) && width > 0 && height > 0 && x >= 0 &&
           y >= 0 && x < width && y < height;
}
// END CHEVRON DROP POLICY

// BEGIN MENU HOLD POLICY
// Pure lifetime policy: only an explicit inner-icon context-menu gesture may
// arm this guard. An unrelated native menu must never keep these drawers open.
struct MenuHoldPolicy {
    bool armed{};
    bool observed{};
    std::uintptr_t owner{};
    std::uint64_t deadline{};
    std::uint64_t lastMenuTick{};

    void Reset() { *this = {}; }
    void Arm(std::uint64_t now) {
        Reset();
        armed = true;
        deadline = now + 1000;
    }
    bool Observe(std::uint64_t now, bool inMenu, std::uintptr_t menuOwner) {
        if (!armed)
            return false;
        if (inMenu) {
            if (observed && owner && menuOwner && owner != menuOwner) {
                Reset();
                return false;
            }
            observed = true;
            if (menuOwner)
                owner = menuOwner;
            lastMenuTick = now;
            return true;
        }
        // Briefly allow the application's async menu creation and focus return.
        if ((!observed && now < deadline) || (observed && now - lastMenuTick < 150))
            return true;
        Reset();
        return false;
    }
};
// END MENU HOLD POLICY

struct DrawerState {
    Grid root{nullptr};
    ItemsControl nativeItems{nullptr};
    Controls::Border footer{nullptr};
    winrt::Windows::UI::ViewManagement::AccessibilitySettings accessibility{nullptr};
    Button chevron{nullptr};
    Flyout inner{nullptr};
    ItemsControl innerItems{nullptr};
    ItemsControl sourceMirror{nullptr};
    Controls::MenuFlyout picker{nullptr};
    std::vector<IInspectable> selectedModels;
    std::vector<IInspectable> sourceSnapshot;
    IInspectable expectedSource{nullptr};
    int64_t sourcePropertyToken{};
    bool sourcePropertyRegistered{};
    bool partitioned{};
    bool refreshQueued{};
    bool refreshDeferred{};
    bool nativeDragActive{};
    IconCandidate dragSource;
    IconCandidate pendingDrop;
    void* dragSourceView{};
    uint32_t dragPointerId{};
    bool dragOverChevron{};
    std::unique_ptr<SavedProperty> originalItemsSource;
    std::vector<SavedProperty> rootProperties;
    uint32_t originalRowCount{};
    winrt::event_token click{}, rightTap{}, loaded{}, unloaded{}, opened{}, closed{};
    winrt::event_token itemsChanged{}, innerLayout{}, innerClosing{}, previewKey{}, menuTick{},
        footerLoaded{}, appearanceChanged{};
    DispatcherTimer menuTimer{nullptr};
    MenuHoldPolicy menuHold;
    bool deferredNativeDismiss{};
    unsigned layoutReports{};
    bool open{};
    bool changing{};
    bool closing{};
    HWND outerWindow{};
    void* nativeManager{};
};

// XAML objects must never be released from global destructors at process exit.
// All normal-unload cleanup runs explicitly on the owning UI thread.
[[clang::no_destroy]] std::unique_ptr<DrawerState> g_drawer;

void QueueCollectionRefresh() {
    if (!g_drawer || g_unloading || !g_runtimeAvailable || g_drawer->closing ||
        g_drawer->refreshQueued)
        return;
    g_drawer->refreshQueued = PostMessageW(g_uiWindow, kRefreshCollectionMessage, 0, 0) != FALSE;
}

using NativeDismiss_t = void(WINAPI*)(void*);
NativeDismiss_t LightDismissConditions_Original;
NativeDismiss_t HideAnimation_Original;

void ResetMenuHold(DrawerState& state) {
    if (state.menuTimer)
        state.menuTimer.Stop();
    state.menuHold.Reset();
    state.deferredNativeDismiss = false;
}

bool ShouldHoldForMenu(DrawerState& state) {
    if (g_unloading || !g_runtimeAvailable || state.changing || state.closing || !state.open ||
        !state.menuHold.armed)
        return false;
    GUITHREADINFO info{};
    info.cbSize = sizeof(info);
    bool inMenu = GetGUIThreadInfo(0, &info) &&
                  (info.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE | GUI_SYSTEMMENUMODE));
    bool wasObserved = state.menuHold.observed;
    bool hold = state.menuHold.Observe(GetTickCount64(), inMenu,
                                       reinterpret_cast<std::uintptr_t>(info.hwndMenuOwner));
    if (!wasObserved && state.menuHold.observed) {
        Wh_Log(L"Owned context gesture entered native menu mode; flags=%lu", info.flags);
    }
    return hold;
}

void ArmMenuHold() {
    if (!g_drawer || g_unloading || !g_runtimeAvailable || !g_drawer->open)
        return;
    auto& state = *g_drawer;
    state.menuHold.Arm(GetTickCount64());
    state.deferredNativeDismiss = false;
    state.menuTimer.Start();
    Wh_Log(L"Inner-icon context gesture armed bounded native-menu hold");
}

bool DeferNativeDismiss(void* manager, PCWSTR source) {
    if (g_uiThread != GetCurrentThreadId() || !g_drawer || g_drawer->nativeManager != manager ||
        !ShouldHoldForMenu(*g_drawer))
        return false;
    if (!g_drawer->deferredNativeDismiss)
        Wh_Log(L"Deferring native %s for owned app menu", source);
    g_drawer->deferredNativeDismiss = true;
    return true;
}

void RestoreSelected(DrawerState& state, bool restoreBinding = true) {
    if (state.dragOverChevron)
        VisualStateManager::GoToState(state.chevron, L"Normal", true);
    state.dragOverChevron = false;
    state.pendingDrop = {};
    state.dragSource = {};
    state.dragSourceView = nullptr;
    if (!state.originalItemsSource)
        return;
    state.changing = true;
    ResetMenuHold(state);
    state.inner.Hide();
    // Remove the inner generated view before restoring the outer binding, so
    // a native item model has at most one active view throughout the transfer.
    state.innerItems.ItemsSource(nullptr);
    state.innerItems.UpdateLayout();
    if (state.sourceMirror && state.itemsChanged.value) {
        state.sourceMirror.Items().VectorChanged(state.itemsChanged);
        state.itemsChanged = {};
    }
    if (restoreBinding)
        state.originalItemsSource->Restore();
    state.nativeItems.UpdateLayout();
    state.originalItemsSource.reset();
    state.sourceMirror = nullptr;
    state.selectedModels.clear();
    state.sourceSnapshot.clear();
    state.expectedSource = nullptr;
    state.partitioned = false;
    state.changing = false;
    Wh_Log(L"Original native ItemsSource/binding restored; outer items=%u",
           state.nativeItems.Items().Size());
}

IconCandidate OuterCandidateFromSource(DrawerState& state, IInspectable const& source) {
    auto element = source.try_as<DependencyObject>();
    ContentPresenter presenter{nullptr};
    for (unsigned depth = 0; element && depth < 64; ++depth) {
        if (element == state.nativeItems) {
            if (!presenter)
                break;
            auto model = presenter.Content();
            // Validate against the active source, not a detached old view.
            uint32_t index{};
            if (model && state.nativeItems.Items().IndexOf(model, index)) {
                return {model};
            }
            break;
        }
        if (auto current = element.try_as<ContentPresenter>())
            presenter = current;
        element = Media::VisualTreeHelper::GetParent(element);
    }
    return {};
}

void ClearDragSource(DrawerState& state) {
    if (state.dragOverChevron)
        VisualStateManager::GoToState(state.chevron, L"Normal", true);
    state.dragOverChevron = false;
    state.dragSource = {};
    state.dragSourceView = nullptr;
    state.dragPointerId = 0;
    state.nativeDragActive = false;
    if (state.refreshDeferred)
        QueueCollectionRefresh();
}

bool IsChevronDrop(DrawerState& state,
                   void* sourceView,
                   Input::PointerRoutedEventArgs const& args) {
    auto point = args.GetCurrentPoint(state.chevron).Position();
    uint32_t index{};
    bool owned = state.dragSourceView == sourceView && state.dragSource.model &&
                 state.nativeItems.Items().IndexOf(state.dragSource.model, index);
    return AcceptChevronDrop(
        owned, args.Pointer().PointerId() == state.dragPointerId,
        !state.changing && !state.closing && state.root.IsLoaded() && state.chevron.IsLoaded() &&
            state.chevron.IsEnabled() && IsWindowVisible(state.outerWindow),
        point.X, point.Y, state.chevron.ActualWidth(), state.chevron.ActualHeight());
}

// BEGIN TRAY PARTITION
// Validate model identity before changing either visual tree.
template <typename Model>
bool PartitionTrayModels(std::vector<Model> const& all,
                         std::vector<Model> const& selected,
                         std::vector<Model>& ordinary,
                         std::vector<Model>& rare) {
    ordinary.clear();
    rare.clear();
    if (all.empty() || all.size() > 512)
        return false;
    for (size_t i = 0; i < selected.size(); ++i) {
        if (std::count(all.begin(), all.end(), selected[i]) != 1 ||
            std::find(selected.begin(), selected.begin() + i, selected[i]) != selected.begin() + i)
            return false;
    }
    for (auto const& model : all) {
        if (std::find(selected.begin(), selected.end(), model) != selected.end())
            rare.push_back(model);
        else
            ordinary.push_back(model);
    }
    return true;
}
// END TRAY PARTITION

bool TrackNativeSource(DrawerState& state) {
    auto source = state.nativeItems.ItemsSource();
    if (!source)
        return false;
    state.originalItemsSource =
        std::make_unique<SavedProperty>(state.nativeItems, ItemsControl::ItemsSourceProperty());
    state.sourceMirror = ItemsControl();
    state.sourceMirror.ItemsSource(source);
    state.expectedSource = source;
    state.itemsChanged =
        state.sourceMirror.Items().VectorChanged([](auto const&, auto const&) noexcept {
            if (g_unloading || !g_runtimeAvailable)
                return;
            QueueCollectionRefresh();
        });
    return true;
}

std::vector<std::wstring> LiveCollectionKeys(std::vector<IInspectable> const& models) {
    std::vector<std::wstring> keys;
    for (auto const& model : models) {
        try {
            keys.push_back(StableIconKey(model));
        } catch (...) {
            keys.emplace_back();
        }
    }
    return keys;
}

void RefreshClassification(DrawerState& state) {
    if (g_unloading || !g_enabled || state.closing || state.changing)
        return;
    if (state.nativeDragActive || ShouldHoldForMenu(state)) {
        state.refreshDeferred = true;
        return;
    }
    state.refreshDeferred = false;
    try {
        if (state.originalItemsSource && state.nativeItems.ItemsSource() != state.expectedSource) {
            // Windows replaced its source. Do not restore an obsolete binding
            // over the replacement; capture the new source before filtering.
            RestoreSelected(state, false);
        }
        if (!state.originalItemsSource && !TrackNativeSource(state))
            return;
        auto allItems = state.sourceMirror.Items();
        uint32_t count = allItems.Size();
        if (count > 512) {
            RestoreSelected(state);
            Wh_Log(L"Native collection exceeds supported size; saved membership retained");
            return;
        }
        std::vector<IInspectable> all, selected, ordinary, rare;
        for (uint32_t i = 0; i < count; ++i)
            all.push_back(allItems.GetAt(i));
        std::vector<std::wstring> saved;
        if (!ReadCollection(saved)) {
            saved.clear();
            Wh_Log(
                L"Invalid/unsupported saved collection; leaving icons visible and record untouched");
        }
        auto keys = LiveCollectionKeys(all);
        Wh_Log(L"Persistent identity scan: total=%zu identified=%zu", keys.size(),
               static_cast<size_t>(std::count_if(keys.begin(), keys.end(),
                                                 [](auto const& key) { return !key.empty(); })));
        for (auto index : MatchCollectionKeys(keys, saved))
            selected.push_back(all[index]);
        if (state.sourceSnapshot == all && state.selectedModels == selected)
            return;
        if (!selected.empty() && !PartitionTrayModels(all, selected, ordinary, rare)) {
            throw winrt::hresult_error(E_UNEXPECTED);
        }
        state.changing = true;
        ResetMenuHold(state);
        state.innerItems.ItemsSource(nullptr);
        state.innerItems.UpdateLayout();
        if (selected.empty()) {
            state.inner.Hide();
            if (state.partitioned)
                state.originalItemsSource->Restore();
        } else {
            auto itemTemplate = state.nativeItems.ItemTemplate();
            if (!itemTemplate)
                throw winrt::hresult_error(E_UNEXPECTED);
            state.nativeItems.ItemsSource(nullptr);
            state.nativeItems.UpdateLayout();
            state.innerItems.ItemTemplate(itemTemplate);
            state.innerItems.ItemTemplateSelector(state.nativeItems.ItemTemplateSelector());
            state.innerItems.ItemContainerStyle(state.nativeItems.ItemContainerStyle());
            state.innerItems.ItemsPanel(state.nativeItems.ItemsPanel());
            state.innerItems.DataContext(state.nativeItems.DataContext());
            state.nativeItems.ItemsSource(
                winrt::single_threaded_observable_vector<IInspectable>(std::move(ordinary)));
            state.nativeItems.UpdateLayout();
            state.innerItems.ItemsSource(
                winrt::single_threaded_observable_vector<IInspectable>(std::move(rare)));
        }
        state.selectedModels = std::move(selected);
        state.sourceSnapshot = std::move(all);
        state.partitioned = !state.selectedModels.empty();
        state.expectedSource = state.nativeItems.ItemsSource();
        state.layoutReports = 0;
        state.changing = false;
        Wh_Log(L"Persistent collection reconciled: saved=%zu original=%u outer=%u inner=%u",
               saved.size(), count, state.nativeItems.Items().Size(),
               state.innerItems.Items().Size());
    } catch (...) {
        LogException(L"Reconcile saved collection");
        RestoreSelected(state);
    }
}

bool SelectIcon(IconCandidate const& candidate) {
    if (!g_drawer || g_unloading || !g_enabled || !candidate.model)
        return false;
    auto& state = *g_drawer;
    if (state.changing || state.closing)
        return false;
    if (std::find(state.selectedModels.begin(), state.selectedModels.end(), candidate.model) !=
        state.selectedModels.end())
        return true;
    RefreshClassification(state);
    if (!state.sourceMirror)
        return false;
    auto key = StableIconKey(candidate.model);
    auto keys = LiveCollectionKeys(state.sourceSnapshot);
    if (key.empty() || std::count(keys.begin(), keys.end(), key) != 1) {
        Wh_Log(L"No unique persistent identity for dropped icon; left visible");
        return false;
    }
    {
        CollectionLock lock;
        std::vector<std::wstring> saved;
        if (!lock.acquired || !ReadCollection(saved))
            return false;
        if (std::find(saved.begin(), saved.end(), key) == saved.end())
            saved.push_back(key);
        // Save before hiding the icon. Unload/exit never clears these records.
        if (!WriteCollection(saved)) {
            Wh_Log(L"Could not persist collection; icon left visible");
            return false;
        }
        Wh_Log(L"Collection saved after explicit drop: entries=%zu", saved.size());
    }
    RefreshClassification(state);
    return std::find(state.selectedModels.begin(), state.selectedModels.end(), candidate.model) !=
           state.selectedModels.end();
}

void ShowRestoreMenu() {
    if (!g_drawer || g_unloading)
        return;
    auto& state = *g_drawer;
    state.picker.Items().Clear();

    auto restore = Controls::MenuFlyoutItem();
    restore.Text(Text(L"Restore all collected icons", L"还原全部收纳图标"));
    std::vector<std::wstring> saved;
    restore.IsEnabled(!ReadCollection(saved) || !saved.empty());
    restore.Click([](auto const&, auto const&) noexcept {
        if (g_unloading || !g_runtimeAvailable)
            return;
        try {
            if (!g_drawer)
                return;
            {
                CollectionLock lock;
                if (!lock.acquired || !WriteCollection({})) {
                    Wh_Log(L"Could not clear saved collection; presentation retained");
                    return;
                }
            }
            RefreshClassification(*g_drawer);
            Wh_Log(L"User restored all icons; saved membership explicitly cleared");
        } catch (...) {
            LogException(L"Restore collected icons");
        }
    });
    state.picker.Items().Append(restore);
    state.picker.ShowAt(state.chevron);
}

void RemoveDrawer() {
    if (!g_drawer)
        return;
    auto& state = *g_drawer;
    state.closing = true;
    ResetMenuHold(state);
    state.picker.Hide();
    state.inner.Hide();
    RestoreSelected(state);
    if (state.click.value)
        state.chevron.Click(state.click);
    if (state.rightTap.value)
        state.chevron.RightTapped(state.rightTap);
    if (state.loaded.value)
        state.root.Loaded(state.loaded);
    if (state.unloaded.value)
        state.root.Unloaded(state.unloaded);
    if (state.opened.value)
        state.inner.Opened(state.opened);
    if (state.closed.value)
        state.inner.Closed(state.closed);
    if (state.innerClosing.value)
        state.inner.Closing(state.innerClosing);
    if (state.previewKey.value)
        state.innerItems.PreviewKeyDown(state.previewKey);
    if (state.menuTick.value)
        state.menuTimer.Tick(state.menuTick);
    if (state.footerLoaded.value)
        state.footer.Loaded(state.footerLoaded);
    if (state.appearanceChanged.value)
        state.root.ActualThemeChanged(state.appearanceChanged);
    if (state.innerLayout.value)
        state.innerItems.LayoutUpdated(state.innerLayout);
    if (state.sourcePropertyRegistered)
        state.nativeItems.UnregisterPropertyChangedCallback(ItemsControl::ItemsSourceProperty(),
                                                            state.sourcePropertyToken);
    uint32_t index;
    if (state.root.Children().IndexOf(state.footer, index))
        state.root.Children().RemoveAt(index);
    for (auto it = state.rootProperties.rbegin(); it != state.rootProperties.rend(); ++it)
        it->Restore();
    while (state.root.RowDefinitions().Size() > state.originalRowCount)
        state.root.RowDefinitions().RemoveAtEnd();
    state.picker.Items().Clear();
    state.inner.Content(nullptr);
    g_drawer.reset();
    Wh_Log(L"Prototype controls removed on owning UI thread");
}

void UpdateDrawerAppearance(DrawerState& state) {
    using winrt::Windows::UI::Color;
    using namespace winrt::Windows::UI::ViewManagement;
    bool dark = state.root.ActualTheme() == ElementTheme::Dark;
    auto brush = [](Color color) { return Media::SolidColorBrush(color); };
    if (state.accessibility.HighContrast()) {
        UISettings colors;
        auto background = colors.GetColorValue(UIColorType::Background);
        auto foreground = colors.GetColorValue(UIColorType::Foreground);
        state.footer.Background(brush(background));
        state.footer.BorderBrush(brush(foreground));
        state.chevron.Background(brush(background));
        state.chevron.BorderBrush(brush(foreground));
        state.chevron.Foreground(brush(foreground));
    } else {
        state.footer.Background(brush(dark ? Color{18, 122, 156, 189} : Color{8, 59, 85, 112}));
        state.footer.BorderBrush(brush(dark ? Color{38, 122, 156, 189} : Color{32, 59, 85, 112}));
        state.chevron.Background(brush(dark ? Color{22, 255, 255, 255} : Color{10, 0, 0, 0}));
        state.chevron.BorderBrush(brush(dark ? Color{20, 255, 255, 255} : Color{16, 0, 0, 0}));
        state.chevron.ClearValue(Controls::Control::ForegroundProperty());
    }
}

Controls::Border CreateDrawerFooter() {
    // Create native controls directly, avoiding standalone XAML resource parsing
    // in Explorer's unpackaged island.
    // The Button retains its Windows template, including hover/focus states.
    Controls::Border footer;
    footer.Height(40);
    footer.Padding(Thickness{8, 3, 8, 4});
    footer.BorderThickness(Thickness{0, 1, 0, 0});
    footer.CornerRadius(CornerRadius{0, 0, 8, 8});
    Button button;
    button.Width(40);
    button.Height(32);
    button.MinWidth(0);
    button.MinHeight(0);
    button.Padding(Thickness{0, 0, 0, 0});
    button.BorderThickness(Thickness{1, 1, 1, 1});
    button.CornerRadius(CornerRadius{8, 8, 8, 8});
    button.HorizontalAlignment(HorizontalAlignment::Right);
    button.VerticalAlignment(VerticalAlignment::Center);
    footer.Child(button);
    return footer;
}

void InstallDrawer(Grid const& root) {
    if (g_unloading || !g_enabled || !root || root.Name() != L"OverflowRootGrid")
        return;
    if (g_drawer && g_drawer->root == root)
        return;
    if (g_drawer)
        RemoveDrawer();
    auto items =
        FindDescendant(root, L"Windows.UI.Xaml.Controls.ItemsControl").try_as<ItemsControl>();
    if (!items || root.RowDefinitions().Size() > 1) {
        Wh_Log(L"Unsupported overflow layout; nothing changed");
        return;
    }

    auto state = std::make_unique<DrawerState>();
    state->root = root;
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM parameter) -> BOOL {
            wchar_t className[128];
            if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
                wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0) {
                *reinterpret_cast<HWND*>(parameter) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&state->outerWindow));
    state->nativeItems = items;
    state->originalRowCount = root.RowDefinitions().Size();
    state->accessibility = winrt::Windows::UI::ViewManagement::AccessibilitySettings();
    state->footer = CreateDrawerFooter();
    state->chevron = state->footer.Child().as<Button>();
    state->inner = Flyout();
    state->innerItems = ItemsControl();
    state->picker = Controls::MenuFlyout();
    state->menuTimer = DispatcherTimer();
    state->menuTimer.Interval(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(100)});
    auto arrow = Controls::FontIcon();
    arrow.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
    arrow.Glyph(L"\xE70E");  // Official ChevronUp font asset, not a literal caret.
    arrow.FontSize(12);
    state->chevron.Content(arrow);
    Automation::AutomationProperties::SetName(
        state->chevron, Text(L"Rarely used tray icons", L"较少使用的托盘图标"));
    state->inner.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
    // A plain Flyout defaults to rendering inside the XAML island HWND. The
    // tray island is only as large as the outer drawer, so that default can
    // replace/cover its visible surface. Use the same native windowed-popup
    // capability that MenuFlyout uses, rather than overriding light dismiss.
    state->inner.ShouldConstrainToRootBounds(false);
    state->innerItems.MinWidth(40);
    state->innerItems.MinHeight(40);
    state->inner.Content(state->innerItems);

    g_drawer = std::move(state);
    auto& current = *g_drawer;
    struct InstallRollback {
        bool committed{};
        ~InstallRollback() noexcept {
            if (!committed) {
                try {
                    RemoveDrawer();
                } catch (...) {
                    LogException(L"Roll back incomplete attachment");
                }
            }
        }
    } rollback;
    if (root.RowDefinitions().Size() == 0) {
        auto row = Controls::RowDefinition();
        row.Height(GridLength{1, GridUnitType::Auto});
        root.RowDefinitions().Append(row);
    }
    auto footerRow = Controls::RowDefinition();
    footerRow.Height(GridLength{1, GridUnitType::Auto});
    root.RowDefinitions().Append(footerRow);
    Grid::SetRow(current.footer, 1);
    for (auto const& child : root.Children()) {
        if (child == items)
            continue;
        current.rootProperties.emplace_back(child, Grid::RowSpanProperty());
        if (auto element = child.try_as<FrameworkElement>())
            Grid::SetRowSpan(element, 2);
    }
    current.click = current.chevron.Click([](auto const&, auto const&) noexcept {
        try {
            if (!g_drawer || g_unloading)
                return;
            auto& state = *g_drawer;
            if (state.selectedModels.empty())
                return;
            if (state.open)
                state.inner.Hide();
            else {
                state.inner.XamlRoot(state.root.XamlRoot());
                // Anchor to the entire outer surface, not the footer button:
                // native Top placement must clear all of the outer drawer.
                Controls::Primitives::FlyoutShowOptions options;
                options.Placement(Controls::Primitives::FlyoutPlacementMode::TopEdgeAlignedRight);
                state.inner.ShowAt(state.root, options);
            }
        } catch (...) {
            LogException(L"Open inner drawer");
        }
    });
    current.rightTap = current.chevron.RightTapped(
        [](auto const&, Input::RightTappedRoutedEventArgs const& args) noexcept {
            try {
                args.Handled(true);
                ShowRestoreMenu();
            } catch (...) {
                LogException(L"Open restore menu");
            }
        });
    current.opened = current.inner.Opened([](auto const&, auto const&) noexcept {
        if (g_unloading || !g_runtimeAvailable)
            return;
        if (g_drawer) {
            g_drawer->open = true;
            Wh_Log(
                L"Inner opened: constrained=%d outerWindowVisible=%d; native dismissal unchanged",
                g_drawer->inner.IsConstrainedToRootBounds(),
                IsWindowVisible(g_drawer->outerWindow));
        }
    });
    current.innerClosing = current.inner.Closing(
        [](auto const&, Controls::Primitives::FlyoutBaseClosingEventArgs const& args) noexcept {
            if (g_unloading || !g_runtimeAvailable)
                return;
            try {
                if (g_drawer && ShouldHoldForMenu(*g_drawer)) {
                    args.Cancel(true);
                    Wh_Log(L"Inner light-dismiss deferred while owned app menu is active");
                }
            } catch (...) {
                LogException(L"Keep inner drawer during app menu");
            }
        });
    current.previewKey = current.innerItems.PreviewKeyDown(
        [](auto const&, Input::KeyRoutedEventArgs const& args) noexcept {
            if (g_unloading || !g_runtimeAvailable)
                return;
            try {
                using winrt::Windows::System::VirtualKey;
                if (args.Key() == VirtualKey::Application ||
                    (args.Key() == VirtualKey::F10 && (GetKeyState(VK_SHIFT) & 0x8000)))
                    ArmMenuHold();
            } catch (...) {
                LogException(L"Arm keyboard context menu");
            }
        });
    current.menuTick = current.menuTimer.Tick([](auto const&, auto const&) noexcept {
        if (g_unloading || !g_runtimeAvailable || !g_drawer)
            return;
        try {
            auto& state = *g_drawer;
            if (ShouldHoldForMenu(state))
                return;
            bool replay = state.deferredNativeDismiss;
            void* manager = state.nativeManager;
            ResetMenuHold(state);
            if (state.refreshDeferred)
                QueueCollectionRefresh();
            Wh_Log(L"Context-menu hold ended; native dismissal reevaluation=%d", replay);
            if (replay && manager && LightDismissConditions_Original) {
                LightDismissConditions_Original(manager);
            }
        } catch (...) {
            LogException(L"Release context-menu hold");
        }
    });
    current.innerLayout = current.innerItems.LayoutUpdated([](auto const&, auto const&) noexcept {
        if (g_unloading || !g_runtimeAvailable || !g_drawer || !g_drawer->open ||
            g_drawer->layoutReports >= 3)
            return;
        try {
            auto& state = *g_drawer;
            auto icon = FindDescendant(state.innerItems, L"SystemTray.NotifyIconView");
            Wh_Log(
                L"Inner layout: items=%u nativeView=%d loaded=%d size=%.1fx%.1f modelMatch=%d outerLoaded=%d outerWindowVisible=%d",
                state.innerItems.Items().Size(), !!icon, icon ? icon.IsLoaded() : false,
                icon ? icon.ActualWidth() : 0.0, icon ? icon.ActualHeight() : 0.0,
                icon ? std::find(state.selectedModels.begin(), state.selectedModels.end(),
                                 icon.DataContext()) != state.selectedModels.end()
                     : false,
                state.root.IsLoaded(), IsWindowVisible(state.outerWindow));
            ++state.layoutReports;
        } catch (...) {
            LogException(L"Inspect generated inner icon");
        }
    });
    current.closed = current.inner.Closed([](auto const&, auto const&) noexcept {
        if (g_unloading || !g_runtimeAvailable)
            return;
        if (g_drawer) {
            g_drawer->open = false;
            ResetMenuHold(*g_drawer);
        }
        Wh_Log(L"Inner drawer closed through native Flyout lifecycle");
    });
    current.loaded = root.Loaded([](auto const&, auto const&) noexcept {
        if (g_unloading || !g_runtimeAvailable)
            return;
        Wh_Log(L"Native outer drawer loaded");
        QueueCollectionRefresh();
    });
    current.unloaded = root.Unloaded([](auto const&, auto const&) noexcept {
        if (g_unloading || !g_runtimeAvailable)
            return;
        try {
            if (!g_drawer || g_drawer->changing || g_drawer->closing)
                return;
            if (g_drawer->open)
                Wh_Log(L"COEXISTENCE NOT VERIFIED: outer unloaded while inner was open");
            g_drawer->inner.Hide();
            g_drawer->picker.Hide();
        } catch (...) {
            LogException(L"Outer drawer unloaded");
        }
    });
    current.footerLoaded = current.footer.Loaded([](auto const&, auto const&) noexcept {
        if (g_unloading || !g_runtimeAvailable || !g_drawer)
            return;
        try {
            UpdateDrawerAppearance(*g_drawer);
            Wh_Log(L"Drawer footer loaded: surface=%.1fx%.1f button=%.1fx%.1f theme=%d",
                   g_drawer->footer.ActualWidth(), g_drawer->footer.ActualHeight(),
                   g_drawer->chevron.ActualWidth(), g_drawer->chevron.ActualHeight(),
                   static_cast<int>(g_drawer->footer.ActualTheme()));
        } catch (...) {
            LogException(L"Refresh loaded footer appearance");
        }
    });
    current.appearanceChanged = root.ActualThemeChanged([](auto const&, auto const&) noexcept {
        if (!g_unloading && g_runtimeAvailable)
            PostMessageW(g_uiWindow, kRefreshAppearanceMessage, 0, 0);
    });
    // Avoid an off-thread AccessibilitySettings delegate outliving this DLL.
    // High contrast is sampled when the footer loads and on UI theme changes.
    UpdateDrawerAppearance(current);
    root.Children().Append(current.footer);
    current.sourcePropertyToken = current.nativeItems.RegisterPropertyChangedCallback(
        ItemsControl::ItemsSourceProperty(), [](auto const&, auto const&) noexcept {
            if (g_drawer && !g_drawer->changing && !g_drawer->closing)
                QueueCollectionRefresh();
        });
    current.sourcePropertyRegistered = true;
    QueueCollectionRefresh();
    rollback.committed = true;
    Wh_Log(L"Nested tray prototype entry point attached");
}

using Initialize_t = void(WINAPI*)(void*);
Initialize_t Initialize_Original;

constexpr UINT kRemoveDrawerMessage = WM_APP + 0x731;
constexpr WCHAR kUiClass[] = L"Windhawk_NestedTrayDrawer_0_1_0";
HMODULE g_selfModule;

LRESULT CALLBACK UiWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == kRefreshAppearanceMessage) {
        if (!g_unloading && g_runtimeAvailable && g_drawer && !g_drawer->closing) {
            try {
                UpdateDrawerAppearance(*g_drawer);
            } catch (...) {
                LogException(L"Refresh drawer appearance");
            }
        }
        return 0;
    }
    if (message == kRefreshCollectionMessage) {
        if (g_unloading || !g_runtimeAvailable || !g_drawer)
            return 0;
        g_drawer->refreshQueued = false;
        try {
            RefreshClassification(*g_drawer);
        } catch (...) {
            LogException(L"Refresh saved collection");
        }
        return 0;
    }
    if (message == kCollectDroppedIconMessage) {
        if (g_unloading || !g_runtimeAvailable || !g_drawer)
            return 0;
        try {
            // No model pointers or callbacks are carried in the queued message.
            // Defer until native pointer-release and capture cleanup finish.
            auto candidate = std::move(g_drawer->pendingDrop);
            g_drawer->pendingDrop = {};
            if (candidate.model && SelectIcon(candidate)) {
                Wh_Log(L"Chevron drop committed; outer=%u inner=%u",
                       g_drawer->nativeItems.Items().Size(), g_drawer->innerItems.Items().Size());
            }
        } catch (...) {
            LogException(L"Collect dropped tray icon");
        }
        return 0;
    }
    if (message == kRemoveDrawerMessage) {
        if (!g_runtimeAvailable)
            return 0;
        try {
            RemoveDrawer();
            return 1;
        } catch (...) {
            LogException(L"UI-thread cleanup");
            return 0;
        }
    }
    if (message == WM_CLOSE) {
        DestroyWindow(window);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool RememberUiThread() {
    if (IsWindow(g_uiWindow))
        return g_uiThread == GetCurrentThreadId();
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = UiWindowProc;
    windowClass.hInstance = g_selfModule;
    windowClass.lpszClassName = kUiClass;
    if (!RegisterClassW(&windowClass))
        return false;
    auto window = CreateWindowExW(0, kUiClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                  g_selfModule, nullptr);
    if (!window) {
        UnregisterClassW(kUiClass, g_selfModule);
        return false;
    }
    g_uiThread = GetCurrentThreadId();
    g_uiWindow = window;
    return true;
}

void WINAPI Initialize_Hook(void* self) {
    Initialize_Original(self);
    if (g_unloading || !g_enabled)
        return;
    try {
        // The composed root slot is taken from the official tray-spacing mod.
        // This prototype requires the exact inspected module version below.
        Grid root{nullptr};
        auto unknown = reinterpret_cast<IUnknown**>(self)[5];
        if (!unknown ||
            FAILED(unknown->QueryInterface(winrt::guid_of<Grid>(), winrt::put_abi(root))))
            return;
        if (!RememberUiThread()) {
            Wh_Log(L"No UI-thread cleanup endpoint; attachment skipped");
            return;
        }
        InstallDrawer(root);
        if (g_drawer && g_drawer->root == root)
            g_drawer->nativeManager = self;
    } catch (...) {
        LogException(L"Attach to native overflow");
    }
}

using Hide_t = void(WINAPI*)(void*);
Hide_t Hide_Original;
Hide_t HideWindow_Original;
void WINAPI HideWindow_Hook(void* self) {
    if (DeferNativeDismiss(self, L"HideWindow"))
        return;
    bool inspect = !g_unloading && g_uiThread == GetCurrentThreadId() && g_drawer;
    if (inspect)
        Wh_Log(L"Native HideWindow: innerOpen=%d outerVisibleBefore=%d", g_drawer->open,
               IsWindowVisible(g_drawer->outerWindow));
    HideWindow_Original(self);
    if (inspect && g_drawer)
        Wh_Log(L"Native HideWindow finished: outerVisibleAfter=%d",
               IsWindowVisible(g_drawer->outerWindow));
}

void WINAPI LightDismissConditions_Hook(void* self) {
    if (!DeferNativeDismiss(self, L"light-dismiss conditions"))
        LightDismissConditions_Original(self);
}

void WINAPI HideAnimation_Hook(void* self) {
    if (!DeferNativeDismiss(self, L"hide animation"))
        HideAnimation_Original(self);
}

using NotifyPointerPressed_t = void(WINAPI*)(void*, void* const*);
NotifyPointerPressed_t NotifyPointerPressed_Original;
void WINAPI NotifyPointerPressed_Hook(void* self, void* const* args) {
    if (!g_unloading && g_runtimeAvailable && g_uiThread == GetCurrentThreadId() && g_drawer &&
        args && *args) {
        try {
            IInspectable event{nullptr};
            winrt::copy_from_abi(event, *args);
            auto pointerArgs = event.as<Input::PointerRoutedEventArgs>();
            auto& state = *g_drawer;
            ClearDragSource(state);
            if (pointerArgs.GetCurrentPoint(state.nativeItems).Properties().IsLeftButtonPressed()) {
                auto candidate = OuterCandidateFromSource(state, pointerArgs.OriginalSource());
                if (candidate.model) {
                    state.dragSource = std::move(candidate);
                    state.dragSourceView = self;
                    state.dragPointerId = pointerArgs.Pointer().PointerId();
                }
            }
            auto element = pointerArgs.OriginalSource().try_as<DependencyObject>();
            bool inInner = false;
            for (int i = 0; element && i < 64; ++i) {
                if (element == g_drawer->innerItems) {
                    inInner = true;
                    break;
                }
                element = Media::VisualTreeHelper::GetParent(element);
            }
            if (state.open && inInner &&
                pointerArgs.GetCurrentPoint(g_drawer->innerItems)
                    .Properties()
                    .IsRightButtonPressed())
                ArmMenuHold();
        } catch (...) {
            LogException(L"Identify inner-icon context gesture");
        }
    }
    // Always preserve the native handler, event arguments and app notification.
    NotifyPointerPressed_Original(self, args);
}

using NativeDrag_t = void(WINAPI*)(void*, void*, void* const*);
NativeDrag_t DragMove_Original;
NativeDrag_t DoDrop_Original;
NativeDismiss_t EndDrag_Original;

void WINAPI DragMove_Hook(void* manager, void* sourceView, void* const* args) {
    DragMove_Original(manager, sourceView, args);
    if (g_unloading || !g_runtimeAvailable || g_uiThread != GetCurrentThreadId() || !g_drawer ||
        !args || !*args)
        return;
    try {
        IInspectable event{nullptr};
        winrt::copy_from_abi(event, *args);
        auto& state = *g_drawer;
        state.nativeDragActive = state.dragSourceView == sourceView;
        bool over = IsChevronDrop(state, sourceView, event.as<Input::PointerRoutedEventArgs>());
        if (over != state.dragOverChevron) {
            state.dragOverChevron = over;
            VisualStateManager::GoToState(state.chevron, over ? L"PointerOver" : L"Normal", true);
        }
    } catch (...) {
        LogException(L"Inspect native drag over chevron");
    }
}

void WINAPI EndDrag_Hook(void* manager) {
    EndDrag_Original(manager);
    if (!g_unloading && g_runtimeAvailable && g_uiThread == GetCurrentThreadId() && g_drawer) {
        try {
            ClearDragSource(*g_drawer);
        } catch (...) {
            LogException(L"Clear native drag source");
        }
    }
}

void WINAPI DoDrop_Hook(void* manager, void* sourceView, void* const* args) {
    IconCandidate dropped;
    if (!g_unloading && g_enabled && g_runtimeAvailable && g_uiThread == GetCurrentThreadId() &&
        g_drawer && args && *args && IsWindow(g_uiWindow)) {
        try {
            IInspectable event{nullptr};
            winrt::copy_from_abi(event, *args);
            if (IsChevronDrop(*g_drawer, sourceView, event.as<Input::PointerRoutedEventArgs>()))
                dropped = g_drawer->dragSource;
        } catch (...) {
            LogException(L"Validate native drop target");
        }
    }
    if (!dropped.model) {
        DoDrop_Original(manager, sourceView, args);
        return;
    }
    // The original DoDrop ends via this same native routine. End the drag
    // without issuing a Windows reorder/pin operation on this custom target.
    EndDrag_Hook(manager);
    if (!g_drawer || g_unloading || !g_enabled)
        return;
    g_drawer->pendingDrop = std::move(dropped);
    if (!PostMessageW(g_uiWindow, kCollectDroppedIconMessage, 0, 0)) {
        g_drawer->pendingDrop = {};
        Wh_Log(L"Could not queue collection; icon left in original drawer");
    } else
        Wh_Log(L"Native outer-icon drag released over nested chevron");
}
void WINAPI Hide_Hook(void* self) {
    try {
        if (g_uiThread == GetCurrentThreadId() && g_drawer) {
            if (g_drawer->open)
                Wh_Log(L"Native outer Hide requested while inner is open; inspect coexistence");
            g_drawer->inner.Hide();
            g_drawer->picker.Hide();
        }
    } catch (...) {
        LogException(L"Native outer hide");
    }
    // Do not suppress native close requests or keep an orphaned inner window.
    Hide_Original(self);
}

bool SupportedModule(HMODULE module) {
    auto resource = FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    auto loaded = resource ? LoadResource(module, resource) : nullptr;
    auto bytes = loaded ? LockResource(loaded) : nullptr;
    VS_FIXEDFILEINFO* version{};
    UINT size{};
    return bytes && VerQueryValueW(bytes, L"\\", reinterpret_cast<void**>(&version), &size) &&
           size >= sizeof(*version) && version->dwFileVersionMS == MAKELONG(4507, 2125) &&
           version->dwFileVersionLS == MAKELONG(0, 10);
}

bool HookTrayModule(HMODULE module) {
    int expected = 0;
    if (!g_hookState.compare_exchange_strong(expected, 1))
        return g_hookState == 2;
    if (!SupportedModule(module)) {
        Wh_Log(L"Unsupported Taskbar.View.dll version; prototype requires 2125.4507.10.0");
        g_hookState = 3;
        return false;
    }
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::InitializeIfNeeded(void))"},
         &Initialize_Original,
         Initialize_Hook},
        {{LR"(public: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::Hide(void))"},
         &Hide_Original,
         Hide_Hook},
        {{LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::HideWindow(void))"},
         &HideWindow_Original,
         HideWindow_Hook},
        {{LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::OnLightDismissConditionsChanged(void))"},
         &LightDismissConditions_Original,
         LightDismissConditions_Hook},
        {{LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::HideAnimation(void))"},
         &HideAnimation_Original,
         HideAnimation_Hook},
        {{LR"(private: virtual void __cdecl winrt::SystemTray::implementation::NotifyIconView::OnPointerPressed(struct winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const &))"},
         &NotifyPointerPressed_Original,
         NotifyPointerPressed_Hook},
        {{LR"(public: void __cdecl winrt::SystemTray::implementation::DragDropManager::DragMove(struct winrt::SystemTray::implementation::NotifyIconView const &,struct winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const &))"},
         &DragMove_Original,
         DragMove_Hook},
        {{LR"(public: void __cdecl winrt::SystemTray::implementation::DragDropManager::DoDrop(struct winrt::SystemTray::implementation::NotifyIconView const &,struct winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const &))"},
         &DoDrop_Original,
         DoDrop_Hook},
        {{LR"(public: void __cdecl winrt::SystemTray::implementation::DragDropManager::EndDrag(void))"},
         &EndDrag_Original,
         EndDrag_Hook},
    };
    bool ok = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
    g_hookState = ok ? 2 : 3;
    Wh_Log(L"Native overflow symbol resolution: %d", ok);
    return ok;
}

using LoadLibrary_t = decltype(&LoadLibraryExW);
LoadLibrary_t LoadLibrary_Original;
HMODULE WINAPI LoadLibrary_Hook(LPCWSTR file, HANDLE handle, DWORD flags) {
    auto module = LoadLibrary_Original(file, handle, flags);
    if (module && g_hookState == 0 && !g_unloading &&
        GetModuleHandleW(L"Taskbar.View.dll") == module) {
        if (HookTrayModule(module))
            Wh_ApplyHookOperations();
    }
    return module;
}

// A message-only window survives popup dismissal, so cleanup never depends on
// the temporary native overflow HWND. No queued function pointers are used.
bool RemoveOnUiThread() {
    DWORD thread = g_uiThread;
    HWND window = g_uiWindow;
    if (!thread)
        return !g_drawer;
    if (thread == GetCurrentThreadId()) {
        RemoveDrawer();
        return true;
    }
    if (!IsWindow(window) || GetWindowThreadProcessId(window, nullptr) != thread)
        return false;
    return SendMessageW(window, kRemoveDrawerMessage, 0, 0) == 1;
}

BOOL Wh_ModInit() {
    // Named-object presence is independent of callback thread ownership. Hold
    // it until hooks are removed, rather than holding a mutex across callbacks.
    std::unique_ptr<void, decltype(&CloseHandle)> instance(
        AcquireDrawerInstance(GetCurrentProcessId()), CloseHandle);
    if (!instance) {
        Wh_Log(L"Another copy already owns this Explorer; disable it before enabling this copy");
        return FALSE;
    }
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&UiWindowProc), &g_selfModule))
        return FALSE;
    g_enabled = Wh_GetIntSetting(L"enabled") != 0;
    auto kernel = GetModuleHandleW(L"kernelbase.dll");
    auto load = reinterpret_cast<LoadLibrary_t>(GetProcAddress(kernel, "LoadLibraryExW"));
    if (!load || !WindhawkUtils::SetFunctionHook(load, LoadLibrary_Hook, &LoadLibrary_Original))
        return FALSE;
    auto tray = GetModuleHandleW(L"Taskbar.View.dll");
    if (tray && !HookTrayModule(tray))
        return FALSE;
    g_collectionMutex =
        CreateMutexW(nullptr, FALSE, L"Local\\Windhawk.NestedTrayDrawer.Collection.v1");
    if (!g_collectionMutex)
        return FALSE;
    if (!InitializeCollectionStorage()) {
        CloseHandle(g_collectionMutex);
        g_collectionMutex = nullptr;
        return FALSE;
    }
    g_instanceGuard = instance.release();
    Wh_Log(L"Experimental drawer initialized; open the hidden-icons drawer to attach");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_hookState == 0) {
        auto tray = GetModuleHandleW(L"Taskbar.View.dll");
        if (tray && HookTrayModule(tray))
            Wh_ApplyHookOperations();
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    bool ok = RemoveOnUiThread();
    g_cleanupCompleted = ok;
    if (ok) {
        HWND window = g_uiWindow;
        if (IsWindow(window))
            SendMessageW(window, WM_CLOSE, 0, 0);
        UnregisterClassW(kUiClass, g_selfModule);
        g_uiWindow = nullptr;
        g_uiThread = 0;
    } else {
        // A disconnected XAML dispatcher must not leave delegate code pointing
        // into an unmapped DLL. Keep inert code resident on exceptional failure.
        HMODULE pinned;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                           reinterpret_cast<LPCWSTR>(&UiWindowProc), &pinned);
        Wh_Log(L"Cleanup failed; inert prototype code retained until Explorer exits");
    }
    Wh_Log(L"UI-thread cleanup completed: %d", ok);
    if (g_collectionMutex) {
        CloseHandle(g_collectionMutex);
        g_collectionMutex = nullptr;
    }
    g_runtimeAvailable = false;
}

void Wh_ModUninit() {
    // Called after the engine removes hooks. A failed UI cleanup keeps the
    // inert DLL and guard resident until process exit, blocking another copy.
    if (g_cleanupCompleted && g_instanceGuard) {
        CloseHandle(g_instanceGuard);
        g_instanceGuard = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    g_enabled = Wh_GetIntSetting(L"enabled") != 0;
    if (!g_enabled)
        RemoveOnUiThread();
}
