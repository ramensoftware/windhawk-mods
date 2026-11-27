// ==WindhawkMod==
// @id              new-menu
// @name            New
// @description     Show, hide and add items in the "New" submenu of the Explorer context menu
// @description:tr-TR Gezgin sağ tık menüsündeki "Yeni" alt menüsünde öğeleri göster, gizle ve yenilerini ekle
// @version         1.0.0
// @author          isousluer
// @github          https://github.com/isousluer
// @homepage        https://github.com/isousluer/new-menu
// @include         explorer.exe
// @compilerOptions -ladvapi32 -lshlwapi -lshell32
// @license         MIT
// ==/WindhawkMod==

// Source code is published under The MIT License.
// https://github.com/isousluer/new-menu

// ==WindhawkModReadme==
/*
# New

Clean up the **New** submenu of the Explorer context menu (right-click on the
desktop or in a folder → **New**): hide the items you never use, and add your
own, such as a Markdown or Python file.

## Show and hide items

Every item of the New menu has an entry in the list **Items of the New menu**.
An item is identified by the **file extension** it creates, for example
`.bmp`, `.docx` or `.zip`. Two items are special:

| Item in the menu | Key      |
|------------------|----------|
| Folder           | `Folder` |
| Shortcut         | `.lnk`   |

Turn **Show** off to hide an item. Items that are not in the list follow the
**Show items that are not in the list** option. Turn it off to keep only the
items you list - handy when new programs keep adding entries.

**Which key does an item have?** In Windhawk, open the mod's **Advanced** tab,
set logging to verbose, then right-click the desktop and open **New**. The log
lists every item with its key, for example `New menu item: .bmp (shown)`.

## Add items

Use **Add items** to put a new entry in the menu:

- **Extension:** the extension of the new file, for example `.md`.
- **Name in the menu:** for example `Markdown document`. Leave it empty to use
  the file type's name.
- **Template file:** a file to copy for each new item, for example
  `%USERPROFILE%\Templates\letter.docx`. Leave it empty to create an empty
  file.
- **Icon:** optional, for example `%SystemRoot%\System32\imageres.dll,-102`.

An added item replaces the built-in item of the same extension, so you can
also give a built-in item a template or another name.

## How it works

The New menu is built from the `ShellNew` keys in the registry
(`HKEY_CLASSES_ROOT\.ext\ShellNew`). The mod hides an item by making its key
invisible to Explorer; it does not change or delete it. A few built-in items,
such as **Text Document** on Windows 11, have no such key; the mod removes
those from the menu by their name.

Added items are written to `HKEY_CURRENT_USER\Software\Classes\.ext\ShellNew`
and removed again when you remove them from the settings or disable the mod.
For an extension that has no file type yet, the mod also registers a file type
with the name you entered; it is removed together with the item. An existing
`ShellNew` key of your own is never overwritten.

Open and Save dialogs of other programs also have a New menu. There, hidden
items still show. Added items show too, except items that rename an existing
file type: their name can only be shown in Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- items:
  - - key: Folder
      $name: Item
      $name:tr-TR: Öğe
      $description: The extension the item creates, for example .bmp, or Folder for the Folder item
      $description:tr-TR: Öğenin oluşturduğu uzantı, örneğin .bmp; Klasör öğesi için Folder
    - show: true
      $name: Show
      $name:tr-TR: Göster
  - - key: .lnk
    - show: true
  - - key: .accdb
    - show: true
  - - key: .bmp
    - show: true
  - - key: .contact
    - show: true
  - - key: .docx
    - show: true
  - - key: .library-ms
    - show: true
  - - key: .pptx
    - show: true
  - - key: .pub
    - show: true
  - - key: .rtf
    - show: true
  - - key: .txt
    - show: true
  - - key: .xlsx
    - show: true
  - - key: .zip
    - show: true
  $name: Items of the New menu
  $name:tr-TR: Yeni menüsündeki öğeler
  $description: Turn Show off to hide an item. The mod's log lists the keys of all items in your menu.
  $description:tr-TR: Bir öğeyi gizlemek için Göster'i kapatın. Menünüzdeki tüm öğelerin anahtarları modun günlüğünde listelenir.
- showUnlisted: true
  $name: Show items that are not in the list
  $name:tr-TR: Listede olmayan öğeleri göster
  $description: Turn off to keep only the items in the list that are set to show
  $description:tr-TR: Yalnızca listede gösterilecek olarak işaretlenen öğeleri tutmak için kapatın
- newItems:
  - - extension: ""
      $name: Extension
      $name:tr-TR: Uzantı
      $description: For example .md
      $description:tr-TR: Örneğin .md
    - name: ""
      $name: Name in the menu
      $name:tr-TR: Menüdeki adı
      $description: Leave empty to use the name of the file type
      $description:tr-TR: Dosya türünün adını kullanmak için boş bırakın
    - template: ""
      $name: Template file
      $name:tr-TR: Şablon dosyası
      $description: Full path of a file to copy. Leave empty to create an empty file.
      $description:tr-TR: Kopyalanacak dosyanın tam yolu. Boş dosya oluşturmak için boş bırakın.
    - icon: ""
      $name: Icon
      $name:tr-TR: Simge
      $description: Optional, for example %SystemRoot%\System32\imageres.dll,-102
      $description:tr-TR: İsteğe bağlı, örneğin %SystemRoot%\System32\imageres.dll,-102
  $name: Add items
  $name:tr-TR: Öğe ekle
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winternl.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <algorithm>
#include <initializer_list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ============================================================================
// Helpers
// ============================================================================

// Set while the mod itself calls the registry, and while a hook runs, so the
// hooks never block or re-enter for those calls.
static thread_local bool t_bypass = false;

struct BypassScope {
    bool previous;
    BypassScope() : previous(t_bypass) { t_bypass = true; }
    ~BypassScope() { t_bypass = previous; }
};

static std::wstring ToLower(std::wstring_view s) {
    std::wstring r(s);
    if (!r.empty()) {
        CharLowerBuffW(r.data(), (DWORD)r.size());
    }
    return r;
}

static std::wstring Trim(std::wstring_view s) {
    size_t b = s.find_first_not_of(L" \t\"");
    if (b == std::wstring_view::npos) {
        return L"";
    }
    size_t e = s.find_last_not_of(L" \t\"");
    return std::wstring(s.substr(b, e - b + 1));
}

// Menu text without "&" accelerator marks, in lowercase, for comparing.
static std::wstring MenuTextKey(std::wstring_view text) {
    std::wstring r;
    r.reserve(text.size());
    for (WCHAR c : text) {
        if (c != L'&') {
            r += c;
        }
    }
    return ToLower(Trim(r));
}

static std::wstring ExpandEnv(const std::wstring& s) {
    if (s.find(L'%') == std::wstring::npos) {
        return s;
    }
    DWORD n = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (n == 0) {
        return s;
    }
    std::wstring r(n, L'\0');
    n = ExpandEnvironmentStringsW(s.c_str(), r.data(), n);
    r.resize(n ? n - 1 : 0);
    return r;
}

static std::wstring GetRegString(HKEY root, const std::wstring& path,
                                 PCWSTR value) {
    WCHAR buffer[512];
    DWORD size = sizeof(buffer);
    if (RegGetValueW(root, path.c_str(), value, RRF_RT_REG_SZ, nullptr, buffer,
                     &size) != ERROR_SUCCESS) {
        return L"";
    }
    return buffer;
}

static bool KeyExists(HKEY root, const std::wstring& path) {
    HKEY key;
    if (RegOpenKeyExW(root, path.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return false;
    }
    RegCloseKey(key);
    return true;
}

// The name Windows shows for a file type, for example "Text Document".
static std::wstring FriendlyTypeName(const std::wstring& ext) {
    WCHAR buffer[256];
    DWORD size = ARRAYSIZE(buffer);
    if (FAILED(AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_FRIENDLYDOCNAME,
                                 ext.c_str(), nullptr, buffer, &size))) {
        return L"";
    }
    return buffer;
}

// ============================================================================
// Settings
// ============================================================================

struct AddedItem {
    std::wstring ext;  // lowercase, starts with '.'
    std::wstring name;
    std::wstring templatePath;
    std::wstring icon;
};

struct Settings {
    // Normalized key (lowercase) -> show.
    std::unordered_map<std::wstring, bool> items;
    bool showUnlisted = true;
    std::vector<AddedItem> added;
    std::unordered_set<std::wstring> addedExts;
    // Menu texts (see MenuTextKey) of hidden items that have no ShellNew key,
    // so they can only be removed from the menu itself.
    std::unordered_set<std::wstring> hiddenMenuTexts;

    bool IsHidden(const std::wstring& key) const {
        if (addedExts.contains(key)) {
            return false;
        }
        auto it = items.find(key);
        return it != items.end() ? !it->second : !showUnlisted;
    }
};

static Settings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;

// Keys already written to the log, so each one is logged once.
static std::unordered_set<std::wstring> g_loggedKeys;
static SRWLOCK g_loggedLock = SRWLOCK_INIT;

// Built-in items that Windows adds without a ShellNew key.
constexpr PCWSTR kBuiltInItemsWithoutKey[] = {L".txt"};

static std::wstring GetStringSetting(const std::wstring& name) {
    PCWSTR value = Wh_GetStringSetting(name.c_str());
    std::wstring r = value ? value : L"";
    Wh_FreeStringSetting(value);
    return r;
}

static std::wstring IndexedName(PCWSTR array, int index, PCWSTR field) {
    return std::wstring(array) + L"[" + std::to_wstring(index) + L"]." + field;
}

// A key is the first part of the registry path below HKEY_CLASSES_ROOT:
// ".bmp", ".docx" or "folder". "bmp" is accepted for ".bmp".
static std::wstring NormalizeKey(const std::wstring& raw) {
    std::wstring k = ToLower(Trim(raw));
    if (k.empty() || k.find_first_of(L"\\/") != std::wstring::npos) {
        return L"";
    }
    if (k[0] != L'.' && k != L"folder") {
        k = L"." + k;
    }
    return k;
}

// Whether the New menu item of a key comes from a ShellNew key, either
// <key>\ShellNew or <key>\<ProgID>\ShellNew.
static bool HasShellNewKey(const std::wstring& key) {
    BypassScope bypass;
    if (KeyExists(HKEY_CLASSES_ROOT, key + L"\\ShellNew")) {
        return true;
    }
    std::wstring progId = GetRegString(HKEY_CLASSES_ROOT, key, nullptr);
    return !progId.empty() &&
           KeyExists(HKEY_CLASSES_ROOT, key + L"\\" + progId + L"\\ShellNew");
}

constexpr int kMaxArrayItems = 256;

static Settings LoadSettings() {
    Settings s;
    for (int i = 0; i < kMaxArrayItems; i++) {
        std::wstring key =
            NormalizeKey(GetStringSetting(IndexedName(L"items", i, L"key")));
        if (key.empty()) {
            continue;
        }
        s.items[key] =
            Wh_GetIntSetting(IndexedName(L"items", i, L"show").c_str()) != 0;
    }
    s.showUnlisted = Wh_GetIntSetting(L"showUnlisted") != 0;

    for (int i = 0; i < kMaxArrayItems; i++) {
        AddedItem item;
        item.ext = NormalizeKey(
            GetStringSetting(IndexedName(L"newItems", i, L"extension")));
        if (item.ext.size() < 2 || item.ext[0] != L'.' ||
            s.addedExts.contains(item.ext)) {
            continue;
        }
        item.name = Trim(GetStringSetting(IndexedName(L"newItems", i, L"name")));
        item.templatePath = ExpandEnv(
            Trim(GetStringSetting(IndexedName(L"newItems", i, L"template"))));
        item.icon = Trim(GetStringSetting(IndexedName(L"newItems", i, L"icon")));
        s.addedExts.insert(item.ext);
        s.added.push_back(std::move(item));
    }

    std::vector<std::wstring> candidates(std::begin(kBuiltInItemsWithoutKey),
                                         std::end(kBuiltInItemsWithoutKey));
    for (const auto& [key, show] : s.items) {
        candidates.push_back(key);
    }
    for (const auto& key : candidates) {
        if (key[0] != L'.' || !s.IsHidden(key) || HasShellNewKey(key)) {
            continue;
        }
        std::wstring name = FriendlyTypeName(key);
        if (!name.empty() && s.hiddenMenuTexts.insert(MenuTextKey(name)).second) {
            Wh_Log(L"Hiding %s by its name: %s", key.c_str(), name.c_str());
        }
    }
    return s;
}

// ============================================================================
// Registry hooks: make ShellNew keys of hidden items invisible
// ============================================================================

#ifndef STATUS_OBJECT_NAME_NOT_FOUND
#define STATUS_OBJECT_NAME_NOT_FOUND ((NTSTATUS)0xC0000034L)
#endif
#ifndef STATUS_BUFFER_OVERFLOW
#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)
#endif
#ifndef STATUS_BUFFER_TOO_SMALL
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif

constexpr ULONG kKeyNameInformation = 3;

using NtQueryKey_t = NTSTATUS(NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);
static NtQueryKey_t g_ntQueryKey;

// The full NT path of an open key, for example
// \REGISTRY\MACHINE\SOFTWARE\Classes\.bmp
static std::wstring GetKeyPath(HANDLE key) {
    if (!g_ntQueryKey) {
        return L"";
    }
    alignas(ULONG) BYTE stackBuffer[1024];
    std::vector<BYTE> heapBuffer;
    BYTE* buffer = stackBuffer;
    ULONG size = sizeof(stackBuffer);
    ULONG needed = 0;
    NTSTATUS status =
        g_ntQueryKey(key, kKeyNameInformation, buffer, size, &needed);
    if ((status == STATUS_BUFFER_OVERFLOW ||
         status == STATUS_BUFFER_TOO_SMALL) &&
        needed > size && needed < 0x10000) {
        heapBuffer.resize(needed);
        buffer = heapBuffer.data();
        size = needed;
        status = g_ntQueryKey(key, kKeyNameInformation, buffer, size, &needed);
    }
    if (!NT_SUCCESS(status)) {
        return L"";
    }
    ULONG nameLength = *(ULONG*)buffer;
    if (nameLength > size - sizeof(ULONG)) {
        return L"";
    }
    return std::wstring((const WCHAR*)(buffer + sizeof(ULONG)),
                        nameLength / sizeof(WCHAR));
}

// True for "ShellNew" and "...\ShellNew". This is the fast check that runs
// for every registry key Explorer opens.
static bool EndsWithShellNew(std::wstring_view name) {
    constexpr std::wstring_view kShellNew = L"shellnew";
    if (name.size() < kShellNew.size()) {
        return false;
    }
    if (name.size() > kShellNew.size() &&
        name[name.size() - kShellNew.size() - 1] != L'\\') {
        return false;
    }
    std::wstring_view tail = name.substr(name.size() - kShellNew.size());
    return CompareStringOrdinal(tail.data(), (int)tail.size(), kShellNew.data(),
                                (int)kShellNew.size(), TRUE) == CSTR_EQUAL;
}

struct ShellNewKey {
    std::wstring key;  // ".bmp" or "folder"
    bool machine;      // HKLM rather than the user's classes
};

// Parses a full NT path of a ShellNew key in the classes root:
//   \REGISTRY\MACHINE\SOFTWARE\Classes\<key>\ShellNew
//   \REGISTRY\MACHINE\SOFTWARE\Classes\<key>\<ProgID>\ShellNew
//   \REGISTRY\USER\<SID>_Classes\<key>\...
//   \REGISTRY\USER\<SID>\Software\Classes\<key>\...
static bool ParseShellNewPath(std::wstring_view fullPath, ShellNewKey* out) {
    std::wstring path = ToLower(fullPath);
    std::wstring_view rest;
    constexpr std::wstring_view kMachine =
        L"\\registry\\machine\\software\\classes\\";
    constexpr std::wstring_view kUser = L"\\registry\\user\\";
    if (path.starts_with(kMachine)) {
        out->machine = true;
        rest = std::wstring_view(path).substr(kMachine.size());
    } else if (path.starts_with(kUser)) {
        out->machine = false;
        std::wstring_view afterUser =
            std::wstring_view(path).substr(kUser.size());
        size_t slash = afterUser.find(L'\\');
        if (slash == std::wstring_view::npos) {
            return false;
        }
        std::wstring_view hive = afterUser.substr(0, slash);
        rest = afterUser.substr(slash + 1);
        if (!hive.ends_with(L"_classes")) {
            constexpr std::wstring_view kClasses = L"software\\classes\\";
            if (!rest.starts_with(kClasses)) {
                return false;
            }
            rest.remove_prefix(kClasses.size());
        }
    } else {
        return false;
    }
    constexpr std::wstring_view kWow = L"wow6432node\\";
    if (rest.starts_with(kWow)) {
        rest.remove_prefix(kWow.size());
    }

    std::vector<std::wstring_view> parts;
    while (!rest.empty()) {
        size_t slash = rest.find(L'\\');
        parts.push_back(rest.substr(0, slash));
        if (slash == std::wstring_view::npos) {
            break;
        }
        rest.remove_prefix(slash + 1);
    }
    if (parts.size() < 2 || parts.size() > 3 || parts.back() != L"shellnew" ||
        parts[0].empty()) {
        return false;
    }
    // <ProgID>\ShellNew only counts below an extension: "Folder\ShellNew" is
    // the Folder item, but "SomeProgID\Sub\ShellNew" is not a menu item.
    if (parts.size() == 3 && !parts[0].starts_with(L'.')) {
        return false;
    }
    out->key = std::wstring(parts[0]);
    return true;
}

static void LogKeyOnce(const std::wstring& key, bool hidden) {
    AcquireSRWLockExclusive(&g_loggedLock);
    bool inserted = g_loggedKeys.insert(key).second;
    ReleaseSRWLockExclusive(&g_loggedLock);
    if (inserted) {
        Wh_Log(L"New menu item: %s (%s)", key.c_str(),
               hidden ? L"hidden" : L"shown");
    }
}

struct HookCheck {
    bool relevant = false;  // the ShellNew key of a New menu item
    std::wstring key;
    bool hidden = false;
    bool block = false;
};

static HookCheck CheckOpen(POBJECT_ATTRIBUTES attributes) {
    HookCheck check;
    if (t_bypass || !attributes || !attributes->ObjectName ||
        !attributes->ObjectName->Buffer) {
        return check;
    }
    std::wstring_view name(attributes->ObjectName->Buffer,
                           attributes->ObjectName->Length / sizeof(WCHAR));
    if (!EndsWithShellNew(name)) {
        return check;
    }

    BypassScope bypass;
    std::wstring fullPath;
    if (attributes->RootDirectory) {
        fullPath = GetKeyPath(attributes->RootDirectory);
        if (fullPath.empty()) {
            return check;
        }
        if (!name.empty()) {
            fullPath += L'\\';
        }
    }
    fullPath += name;

    ShellNewKey parsed;
    if (!ParseShellNewPath(fullPath, &parsed)) {
        return check;
    }

    check.relevant = true;
    check.key = std::move(parsed.key);
    AcquireSRWLockShared(&g_settingsLock);
    if (g_settings.addedExts.contains(check.key)) {
        // An added item replaces the built-in one: hide the machine-wide key
        // so only the values the mod wrote for the user are used.
        check.hidden = false;
        check.block = parsed.machine;
    } else {
        check.hidden = g_settings.IsHidden(check.key);
        check.block = check.hidden;
    }
    ReleaseSRWLockShared(&g_settingsLock);
    return check;
}

// Runs the original open, then hides the key if needed. Opening it first
// means only keys that exist are logged: while building its list, Explorer
// looks for a ShellNew key below every file extension.
template <typename OpenFn>
static NTSTATUS FilterOpen(PHANDLE keyHandle, POBJECT_ATTRIBUTES attributes,
                           OpenFn open) {
    HookCheck check = CheckOpen(attributes);
    NTSTATUS status = open();
    if (!check.relevant || !NT_SUCCESS(status)) {
        return status;
    }
    {
        BypassScope bypass;
        LogKeyOnce(check.key, check.hidden);
    }
    if (check.block) {
        if (keyHandle && *keyHandle) {
            CloseHandle(*keyHandle);
            *keyHandle = nullptr;
        }
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }
    return status;
}

using NtOpenKey_t = NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);
static NtOpenKey_t NtOpenKey_Original;
static NTSTATUS NTAPI NtOpenKey_Hook(PHANDLE keyHandle,
                                     ACCESS_MASK desiredAccess,
                                     POBJECT_ATTRIBUTES objectAttributes) {
    return FilterOpen(keyHandle, objectAttributes, [&] {
        return NtOpenKey_Original(keyHandle, desiredAccess, objectAttributes);
    });
}

using NtOpenKeyEx_t =
    NTSTATUS(NTAPI*)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, ULONG);
static NtOpenKeyEx_t NtOpenKeyEx_Original;
static NTSTATUS NTAPI NtOpenKeyEx_Hook(PHANDLE keyHandle,
                                       ACCESS_MASK desiredAccess,
                                       POBJECT_ATTRIBUTES objectAttributes,
                                       ULONG openOptions) {
    return FilterOpen(keyHandle, objectAttributes, [&] {
        return NtOpenKeyEx_Original(keyHandle, desiredAccess, objectAttributes,
                                    openOptions);
    });
}

// ============================================================================
// Menu hooks: built-in items without a ShellNew key, and custom names
// ============================================================================

static bool IsCalledFromShell32(void* returnAddress) {
    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)returnAddress, &module)) {
        return false;
    }
    return module == GetModuleHandleW(L"shell32.dll");
}

static bool IsHiddenMenuText(PCWSTR text) {
    if (!text || IS_INTRESOURCE(text)) {
        return false;
    }
    AcquireSRWLockShared(&g_settingsLock);
    bool hidden = !g_settings.hiddenMenuTexts.empty() &&
                  g_settings.hiddenMenuTexts.contains(MenuTextKey(text));
    ReleaseSRWLockShared(&g_settingsLock);
    return hidden;
}

using InsertMenuItemW_t = BOOL(WINAPI*)(HMENU, UINT, BOOL, LPCMENUITEMINFOW);
static InsertMenuItemW_t InsertMenuItemW_Original;
static BOOL WINAPI InsertMenuItemW_Hook(HMENU menu, UINT item, BOOL byPosition,
                                        LPCMENUITEMINFOW info) {
    void* returnAddress = __builtin_return_address(0);
    if (info && info->dwTypeData &&
        ((info->fMask & MIIM_STRING) ||
         ((info->fMask & MIIM_TYPE) &&
          !(info->fType & (MFT_BITMAP | MFT_SEPARATOR | MFT_OWNERDRAW)))) &&
        IsHiddenMenuText(info->dwTypeData) &&
        IsCalledFromShell32(returnAddress)) {
        Wh_Log(L"Hid menu item %s", info->dwTypeData);
        // Report success, so the New menu goes on with the next item.
        return TRUE;
    }
    return InsertMenuItemW_Original(menu, item, byPosition, info);
}

// MenuText values the mod writes for added items: "@new-menu:<name>".
// Windows reads MenuText with SHLoadIndirectString, which only accepts
// resource references, so the mod resolves its own format.
constexpr std::wstring_view kMenuTextPrefix = L"@new-menu:";

using SHLoadIndirectString_t = HRESULT(WINAPI*)(PCWSTR, PWSTR, UINT, void**);
static SHLoadIndirectString_t SHLoadIndirectString_Original;
static HRESULT WINAPI SHLoadIndirectString_Hook(PCWSTR source, PWSTR output,
                                                UINT outputSize,
                                                void** reserved) {
    if (source && output && outputSize &&
        wcsncmp(source, kMenuTextPrefix.data(), kMenuTextPrefix.size()) == 0) {
        std::wstring_view name(source + kMenuTextPrefix.size());
        if (name.size() >= outputSize) {
            return E_NOT_SUFFICIENT_BUFFER;
        }
        name.copy(output, name.size());
        output[name.size()] = L'\0';
        return S_OK;
    }
    return SHLoadIndirectString_Original(source, output, outputSize, reserved);
}

// ============================================================================
// Added items: ShellNew keys written to HKCU\Software\Classes
// ============================================================================

// What the mod created for an added item, so it only ever removes its own.
struct CreatedKey {
    std::wstring ext;
    bool createdExtKey = false;  // HKCU\Software\Classes\<ext>
    bool setDefault = false;     // its default value (the ProgID)
    std::wstring progId;         // HKCU\Software\Classes\<ProgID>, if created
};

// Stored as one line per item: "<ext>|<createdExtKey>|<setDefault>|<progId>".
static std::vector<CreatedKey> LoadCreatedKeys() {
    std::vector<CreatedKey> keys;
    WCHAR buffer[8192];
    size_t len = Wh_GetStringValue(L"createdKeys", buffer, ARRAYSIZE(buffer));
    std::wstring_view s(buffer, wcsnlen(buffer, std::min(len, ARRAYSIZE(buffer) - 1)));
    while (!s.empty()) {
        size_t nl = s.find(L'\n');
        std::wstring_view line = s.substr(0, nl);
        s = nl == std::wstring_view::npos ? std::wstring_view()
                                          : s.substr(nl + 1);
        std::vector<std::wstring_view> fields;
        while (true) {
            size_t bar = line.find(L'|');
            fields.push_back(line.substr(0, bar));
            if (bar == std::wstring_view::npos) {
                break;
            }
            line.remove_prefix(bar + 1);
        }
        if (fields.size() != 4 || fields[0].size() < 2 ||
            fields[0][0] != L'.') {
            continue;
        }
        keys.push_back({std::wstring(fields[0]), fields[1] == L"1",
                        fields[2] == L"1", std::wstring(fields[3])});
    }
    return keys;
}

static void SaveCreatedKeys(const std::vector<CreatedKey>& keys) {
    std::wstring s;
    for (const auto& k : keys) {
        s += k.ext + L"|" + (k.createdExtKey ? L"1" : L"0") + L"|" +
             (k.setDefault ? L"1" : L"0") + L"|" + k.progId + L"\n";
    }
    if (s.empty()) {
        Wh_DeleteValue(L"createdKeys");
    } else {
        Wh_SetStringValue(L"createdKeys", s.c_str());
    }
}

static std::wstring ClassesPath(const std::wstring& name) {
    return L"Software\\Classes\\" + name;
}

static bool KeyIsEmpty(const std::wstring& path) {
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0, KEY_READ, &key) !=
        ERROR_SUCCESS) {
        return false;
    }
    DWORD subKeys = 0, values = 0;
    LONG r = RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, &subKeys,
                              nullptr, nullptr, &values, nullptr, nullptr,
                              nullptr, nullptr);
    RegCloseKey(key);
    return r == ERROR_SUCCESS && subKeys == 0 && values == 0;
}

static void DeleteKeyTree(const std::wstring& path) {
    LONG r = RegDeleteTreeW(HKEY_CURRENT_USER, path.c_str());
    if (r == ERROR_SUCCESS) {
        r = RegDeleteKeyW(HKEY_CURRENT_USER, path.c_str());
    }
    if (r != ERROR_SUCCESS && r != ERROR_FILE_NOT_FOUND) {
        Wh_Log(L"Could not remove HKCU\\%s: error %ld", path.c_str(), r);
    }
}

static bool SetRegString(const std::wstring& path, PCWSTR name,
                         const std::wstring& value, DWORD type = REG_SZ) {
    return RegSetKeyValueW(HKEY_CURRENT_USER, path.c_str(), name, type,
                           value.c_str(),
                           (DWORD)((value.size() + 1) * sizeof(WCHAR))) ==
           ERROR_SUCCESS;
}

// Returns whether file associations changed.
static bool RemoveCreatedKey(const CreatedKey& k) {
    std::wstring extPath = ClassesPath(k.ext);
    DeleteKeyTree(extPath + L"\\ShellNew");
    if (k.setDefault &&
        GetRegString(HKEY_CURRENT_USER, extPath, nullptr) == k.progId) {
        RegDeleteKeyValueW(HKEY_CURRENT_USER, extPath.c_str(), nullptr);
    }
    if (!k.progId.empty()) {
        DeleteKeyTree(ClassesPath(k.progId));
    }
    if (k.createdExtKey && KeyIsEmpty(extPath)) {
        RegDeleteKeyW(HKEY_CURRENT_USER, extPath.c_str());
    }
    return k.setDefault || !k.progId.empty();
}

// Windows only lists an extension in the New menu if it has a file type
// (ProgID). Registers one for extensions that have none. Returns whether
// file associations changed.
static bool EnsureFileType(const AddedItem& item, CreatedKey* k) {
    std::wstring extPath = ClassesPath(item.ext);
    std::wstring progId = GetRegString(HKEY_CLASSES_ROOT, item.ext, nullptr);
    bool changed = false;
    if (progId.empty() || k->setDefault) {
        progId = L"NewMenu" + item.ext;
        if (GetRegString(HKEY_CURRENT_USER, extPath, nullptr) != progId) {
            SetRegString(extPath, nullptr, progId);
            changed = true;
        }
        k->setDefault = true;
    }
    if (k->progId.empty() && KeyExists(HKEY_CLASSES_ROOT, progId)) {
        return changed;
    }
    // The mod owns this file type, so the item's name is the type's name,
    // which every program shows.
    std::wstring name = item.name;
    if (name.empty()) {
        name = FriendlyTypeName(item.ext);
    }
    if (GetRegString(HKEY_CURRENT_USER, ClassesPath(progId), nullptr) != name) {
        SetRegString(ClassesPath(progId), nullptr, name);
        changed = true;
    }
    k->progId = progId;
    return changed;
}

static bool WriteShellNewKey(const AddedItem& item, const CreatedKey& k) {
    std::wstring shellNewPath = ClassesPath(item.ext) + L"\\ShellNew";
    // Start from an empty key, so values of an older version of the item,
    // such as a template that was removed, don't remain.
    RegDeleteTreeW(HKEY_CURRENT_USER, shellNewPath.c_str());
    HKEY key;
    LONG r = RegCreateKeyExW(HKEY_CURRENT_USER, shellNewPath.c_str(), 0,
                             nullptr, 0, KEY_WRITE, nullptr, &key, nullptr);
    if (r != ERROR_SUCCESS) {
        Wh_Log(L"Could not create HKCU\\%s: error %ld", shellNewPath.c_str(),
               r);
        return false;
    }
    RegCloseKey(key);

    bool ok;
    if (!item.templatePath.empty()) {
        if (GetFileAttributesW(item.templatePath.c_str()) ==
            INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"Template of %s not found: %s", item.ext.c_str(),
                   item.templatePath.c_str());
        }
        ok = SetRegString(shellNewPath, L"FileName", item.templatePath);
    } else {
        ok = SetRegString(shellNewPath, L"NullFile", L"");
    }
    // The name of a file type the mod registered is the type's own name.
    // Other types get it through MenuText, which only Explorer resolves.
    if (!item.name.empty() && k.progId.empty()) {
        ok = SetRegString(shellNewPath, L"MenuText",
                          std::wstring(kMenuTextPrefix) + item.name) &&
             ok;
    }
    if (!item.icon.empty()) {
        ok = SetRegString(shellNewPath, L"IconPath", item.icon,
                          REG_EXPAND_SZ) &&
             ok;
    }
    return ok;
}

// Brings the registry in line with the "Add items" setting.
static void SyncAddedItems(const std::vector<AddedItem>& added) {
    BypassScope bypass;
    std::vector<CreatedKey> previous = LoadCreatedKeys();
    std::vector<CreatedKey> current;
    bool associationsChanged = false;

    for (const auto& k : previous) {
        bool wanted = std::any_of(added.begin(), added.end(), [&](const auto& a) {
            return a.ext == k.ext;
        });
        if (!wanted) {
            associationsChanged |= RemoveCreatedKey(k);
            Wh_Log(L"Removed added item %s", k.ext.c_str());
        }
    }

    for (const auto& item : added) {
        auto prev = std::find_if(previous.begin(), previous.end(),
                                 [&](const auto& k) { return k.ext == item.ext; });
        std::wstring extPath = ClassesPath(item.ext);
        CreatedKey k;
        if (prev != previous.end()) {
            k = *prev;
        } else {
            if (KeyExists(HKEY_CURRENT_USER, extPath + L"\\ShellNew")) {
                Wh_Log(L"Not adding %s: HKCU\\%s\\ShellNew already exists and "
                       L"was not created by the mod",
                       item.ext.c_str(), extPath.c_str());
                continue;
            }
            k.ext = item.ext;
            k.createdExtKey = !KeyExists(HKEY_CURRENT_USER, extPath);
        }
        associationsChanged |= EnsureFileType(item, &k);
        // Record the item before writing it, so it is removed later even if
        // writing fails halfway.
        current.push_back(k);
        SaveCreatedKeys(current);
        if (WriteShellNewKey(item, k)) {
            Wh_Log(L"Added item %s", item.ext.c_str());
        }
    }

    SaveCreatedKeys(current);
    if (associationsChanged) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
}

static void RemoveAllAddedItems() {
    BypassScope bypass;
    bool associationsChanged = false;
    for (const auto& k : LoadCreatedKeys()) {
        associationsChanged |= RemoveCreatedKey(k);
    }
    SaveCreatedKeys({});
    if (associationsChanged) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
}

// Explorer caches the list of New menu items. Deleting the cache makes it
// build the list again the next time the menu opens.
static void ClearNewMenuCache() {
    BypassScope bypass;
    constexpr PCWSTR kCache =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Discardable\\"
        L"PostSetup\\ShellNew";
    if (RegDeleteTreeW(HKEY_CURRENT_USER, kCache) == ERROR_SUCCESS) {
        RegDeleteKeyW(HKEY_CURRENT_USER, kCache);
    }
}

// Only the main Explorer process writes added items. Folder windows that run
// in their own process ("/factory" in the command line) leave them alone, so
// closing such a window doesn't remove them.
static bool IsMainExplorerProcess() {
    std::wstring commandLine = ToLower(GetCommandLineW());
    return commandLine.find(L"/factory") == std::wstring::npos &&
           commandLine.find(L"-embedding") == std::wstring::npos;
}

static bool g_isMainProcess;

static void ApplySettings() {
    Settings s = LoadSettings();
    std::vector<AddedItem> added = s.added;
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);

    AcquireSRWLockExclusive(&g_loggedLock);
    g_loggedKeys.clear();
    ReleaseSRWLockExclusive(&g_loggedLock);

    if (g_isMainProcess) {
        SyncAddedItems(added);
    }
}

// ============================================================================
// Windhawk entry points
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_isMainProcess = IsMainExplorerProcess();

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    HMODULE user32 = LoadLibraryW(L"user32.dll");
    g_ntQueryKey = (NtQueryKey_t)GetProcAddress(ntdll, "NtQueryKey");
    void* ntOpenKey = (void*)GetProcAddress(ntdll, "NtOpenKey");
    void* ntOpenKeyEx = (void*)GetProcAddress(ntdll, "NtOpenKeyEx");
    void* insertMenuItem =
        user32 ? (void*)GetProcAddress(user32, "InsertMenuItemW") : nullptr;
    // shell32 calls the kernelbase implementation (through
    // api-ms-win-core-string); shlwapi has a separate copy, used on older
    // Windows versions.
    void* loadIndirectString = nullptr;
    for (PCWSTR dll : {L"kernelbase.dll", L"shlwapi.dll"}) {
        HMODULE module = LoadLibraryW(dll);
        if (module &&
            (loadIndirectString =
                 (void*)GetProcAddress(module, "SHLoadIndirectString"))) {
            break;
        }
    }
    if (!g_ntQueryKey || !ntOpenKey || !ntOpenKeyEx || !insertMenuItem ||
        !loadIndirectString) {
        Wh_Log(L"Functions to hook not found");
        return FALSE;
    }

    ApplySettings();

    if (!Wh_SetFunctionHook(ntOpenKey, (void*)NtOpenKey_Hook,
                            (void**)&NtOpenKey_Original) ||
        !Wh_SetFunctionHook(ntOpenKeyEx, (void*)NtOpenKeyEx_Hook,
                            (void**)&NtOpenKeyEx_Original) ||
        !Wh_SetFunctionHook(insertMenuItem, (void*)InsertMenuItemW_Hook,
                            (void**)&InsertMenuItemW_Original) ||
        !Wh_SetFunctionHook(loadIndirectString,
                            (void*)SHLoadIndirectString_Hook,
                            (void**)&SHLoadIndirectString_Original)) {
        Wh_Log(L"Hooking failed");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    // The hooks are active now, so the rebuilt cache follows the settings.
    ClearNewMenuCache();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    ApplySettings();
    ClearNewMenuCache();
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (g_isMainProcess) {
        RemoveAllAddedItems();
    }
    ClearNewMenuCache();
}
