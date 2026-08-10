// ==WindhawkMod==
// @id              taskbar-separators-prototype
// @name            Taskbar Separators - Prototype
// @description     Creates genuine independently reorderable taskbar separator pins.
// @version         0.1
// @author          meteoni
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -lshell32 -lpropsys -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Prototype for genuine Windows 11 taskbar separators.

Each configured separator is represented by its own real pinned Shell shortcut.
The mod:
1. Writes a small embedded purple vertical-bar icon to its Windhawk storage folder.
2. Creates one uniquely identified .lnk per configured separator.
3. Pins each shortcut with the private PinManager COM interface.
4. Moves each pin to its configured position.
5. Unpins the separators and deletes the generated files when the mod unloads.

The position setting is 1-based for the user: 1 means the first pinned position.

This is a prototype. XAML restyling/suppression of normal button behavior will be
added separately.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- identifierPrefix: WindhawkSeparator-8F31A7D2
  $name: Separator identifier
  $description: >-
    File/display-name prefix for generated separator shortcuts. Unsupported
    filename/regex characters are replaced with underscores.
- separators:
    - - index: 5
        $name: Position
        $description: 1 = first pinned taskbar position.
    - - index: 10
        $name: Position
        $description: 1 = first pinned taskbar position.
  $name: Separators
  $description: Add or remove entries to control how many separators are created.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>

#include <algorithm>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// Private taskbar PinManager COM ABI, verified on the current Windows build.
// -----------------------------------------------------------------------------

static const CLSID CLSID_PinManager = {
    0xA5C8D635, 0xB4ED, 0x452B,
    {0x81, 0x09, 0x95, 0x01, 0x78, 0x10, 0x96, 0xD1}
};

static const IID IID_IPinManagerInterop2 = {
    0x87D9E034, 0x56D0, 0x4F8C,
    {0xBE, 0x59, 0x99, 0x7B, 0x01, 0x75, 0x47, 0x10}
};

static const IID IID_IPinManagerInterop3 = {
    0x27216D91, 0x78B8, 0x406D,
    {0x85, 0xC4, 0x5F, 0xDB, 0xF5, 0x1C, 0xD1, 0x6B}
};

enum PINNEDLISTMODIFYCALLER : int {
    // Observed from a native modern taskbar unpin on this build.
    PMC_JUMPVIEWBROKER = 11,

    PMC_TASKBANDINSERT = 23,
    PMC_TASKBANDMODIFY = 24,
    PMC_TASKBANDPIN = 25,
    PMC_TASKBANDPINGROUP = 26,
    PMC_TASKBANDREORDER = 27,
};

struct IPinManagerInterop2 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE PinItemToTaskbarShim(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE PinItemFromTrustedCaller(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyPrependDefaultTaskbarLayout() = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyAppendDefaultTaskbarLayout() = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyInPlaceTaskbarLayout(
        int layoutType) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyReorderTaskbarLayout(
        int layoutType,
        int value) = 0;

    virtual HRESULT STDMETHODCALLTYPE UnpinTaskbarItem(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE UpdatePinnedTaskbarItem(
        PCUIDLIST_ABSOLUTE oldPidl,
        PCUIDLIST_ABSOLUTE newPidl,
        PINNEDLISTMODIFYCALLER caller) = 0;
};

struct IPinManagerInterop3 : IPinManagerInterop2 {
    virtual HRESULT STDMETHODCALLTYPE MoveTaskbarPin(
        PCUIDLIST_ABSOLUTE pidl,
        int index,
        PINNEDLISTMODIFYCALLER caller) = 0;
};

// -----------------------------------------------------------------------------
// Settings/state.
// -----------------------------------------------------------------------------

struct SeparatorSetting {
    int ordinal;       // Stable within this loaded settings snapshot: 1, 2, ...
    int targetIndex;   // Zero-based value passed to MoveTaskbarPin.
};

struct Settings {
    std::wstring identifierPrefix;
    std::vector<SeparatorSetting> separators;
};

static Settings g_settings;
static std::wstring g_storagePath;
static std::wstring g_iconPath;

// Internal AppUserModelID namespace. Kept independent from the user-visible
// filename prefix so user changes don't accidentally create invalid AppIDs.
static constexpr wchar_t kInternalAppIdPrefix[] =
    L"Windhawk.TaskbarSeparator.8F31A7D2";

// -----------------------------------------------------------------------------
// Embedded icon.
//
// Valid 16x16, 4-bpp ICO.
// Palette index 1 is #A020F0; only the two center columns are opaque.
// -----------------------------------------------------------------------------

static constexpr unsigned char kSeparatorIcon[] = {
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10, 0x10, 0x00, 0x01, 0x00,
    0x04, 0x00, 0x28, 0x01, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x20, 0xA0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00,
    0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F,
    0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F,
    0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F,
    0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F,
    0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F,
    0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00,
};

static_assert(sizeof(kSeparatorIcon) == 318);

// -----------------------------------------------------------------------------
// Small helpers.
// -----------------------------------------------------------------------------

static std::wstring JoinPath(
    const std::wstring& directory,
    const std::wstring& fileName) {
    if (directory.empty()) {
        return fileName;
    }

    if (directory.back() == L'\\' || directory.back() == L'/') {
        return directory + fileName;
    }

    return directory + L"\\" + fileName;
}

static std::wstring SanitizePrefix(std::wstring value) {
    // Keep this intentionally restrictive:
    // - safe as a Windows filename
    // - easy to use later in a XAML regex
    // - no escaping surprises
    for (wchar_t& ch : value) {
        const bool allowed =
            (ch >= L'a' && ch <= L'z') ||
            (ch >= L'A' && ch <= L'Z') ||
            (ch >= L'0' && ch <= L'9') ||
            ch == L'-' ||
            ch == L'_';

        if (!allowed) {
            ch = L'_';
        }
    }

    while (!value.empty() &&
           (value.back() == L' ' || value.back() == L'.')) {
        value.pop_back();
    }

    if (value.empty()) {
        value = L"WindhawkSeparator-8F31A7D2";
    }

    // Keep generated paths and future XAML selectors reasonably short.
    constexpr size_t kMaxPrefixLength = 64;
    if (value.size() > kMaxPrefixLength) {
        value.resize(kMaxPrefixLength);
    }

    return value;
}

static std::wstring GetSeparatorBaseName(int ordinal) {
    return g_settings.identifierPrefix + L"-" + std::to_wstring(ordinal);
}

static std::wstring GetSeparatorShortcutPath(int ordinal) {
    return JoinPath(
        g_storagePath,
        GetSeparatorBaseName(ordinal) + L".lnk");
}

static std::wstring GetSeparatorAppId(int ordinal) {
    return std::wstring(kInternalAppIdPrefix) +
           L"." +
           std::to_wstring(ordinal);
}

static bool FileExists(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES &&
           !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static bool DeleteFileIfPresent(const std::wstring& path) {
    if (!FileExists(path)) {
        return true;
    }

    if (DeleteFileW(path.c_str())) {
        return true;
    }

    Wh_Log(
        L"[FILES] DeleteFileW('%s') failed error=%u",
        path.c_str(),
        GetLastError());

    return false;
}

static bool WriteBinaryFile(
    const std::wstring& path,
    const void* data,
    DWORD size) {
    HANDLE file = CreateFileW(
        path.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(
            L"[FILES] CreateFileW('%s') failed error=%u",
            path.c_str(),
            GetLastError());
        return false;
    }

    DWORD written = 0;
    BOOL ok = WriteFile(
        file,
        data,
        size,
        &written,
        nullptr);

    DWORD error = ok ? ERROR_SUCCESS : GetLastError();
    CloseHandle(file);

    if (!ok || written != size) {
        Wh_Log(
            L"[FILES] WriteFile('%s') failed/incomplete "
            L"error=%u written=%u expected=%u",
            path.c_str(),
            error,
            written,
            size);
        return false;
    }

    return true;
}

static bool LoadSettings() {
    g_settings = {};

    PCWSTR prefixSetting = Wh_GetStringSetting(L"identifierPrefix");
    if (prefixSetting) {
        g_settings.identifierPrefix = SanitizePrefix(prefixSetting);
        Wh_FreeStringSetting(prefixSetting);
    } else {
        g_settings.identifierPrefix =
            L"WindhawkSeparator-8F31A7D2";
    }

    // Wh_GetIntSetting returns 0 for a missing setting.
    // Positions are therefore deliberately user-facing 1-based values.
    constexpr int kMaxSeparators = 64;

    for (int i = 0; i < kMaxSeparators; i++) {
        int position =
            Wh_GetIntSetting(L"separators[%d].index", i);

        if (position == 0) {
            break;
        }

        if (position < 1) {
            Wh_Log(
                L"[SETTINGS] Ignoring separator %d: invalid position=%d",
                i + 1,
                position);
            continue;
        }

        g_settings.separators.push_back({
            .ordinal = i + 1,
            .targetIndex = position - 1,
        });
    }

    Wh_Log(
        L"[SETTINGS] prefix='%s' separators=%zu",
        g_settings.identifierPrefix.c_str(),
        g_settings.separators.size());

    for (const auto& separator : g_settings.separators) {
        Wh_Log(
            L"[SETTINGS] separator=%d targetIndex=%d (user position=%d)",
            separator.ordinal,
            separator.targetIndex,
            separator.targetIndex + 1);
    }

    return true;
}

static bool InitializeStoragePath() {
    wchar_t buffer[32768] = {};

    size_t chars =
        Wh_GetModStoragePath(buffer, ARRAYSIZE(buffer));

    if (!chars || !buffer[0]) {
        Wh_Log(L"[FILES] Wh_GetModStoragePath failed");
        return false;
    }

    g_storagePath = buffer;

    // The API provides a usable mod storage directory, but this is harmless
    // if the directory already exists.
    if (!CreateDirectoryW(g_storagePath.c_str(), nullptr)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            Wh_Log(
                L"[FILES] CreateDirectoryW('%s') failed error=%u",
                g_storagePath.c_str(),
                error);
            return false;
        }
    }

    g_iconPath =
        JoinPath(
            g_storagePath,
            g_settings.identifierPrefix + L".ico");

    Wh_Log(L"[FILES] storage='%s'", g_storagePath.c_str());
    Wh_Log(L"[FILES] icon='%s'", g_iconPath.c_str());

    return true;
}

// -----------------------------------------------------------------------------
// Shortcut creation.
// -----------------------------------------------------------------------------

static HRESULT SetShortcutAppId(
    IShellLinkW* shellLink,
    const std::wstring& appId) {
    IPropertyStore* propertyStore = nullptr;

    HRESULT hr = shellLink->QueryInterface(
        IID_PPV_ARGS(&propertyStore));

    if (FAILED(hr)) {
        return hr;
    }

    PROPVARIANT value;
    PropVariantInit(&value);

    hr = InitPropVariantFromString(
        appId.c_str(),
        &value);

    if (SUCCEEDED(hr)) {
        hr = propertyStore->SetValue(
            PKEY_AppUserModel_ID,
            value);
    }

    if (SUCCEEDED(hr)) {
        hr = propertyStore->Commit();
    }

    PropVariantClear(&value);
    propertyStore->Release();

    return hr;
}

static HRESULT CreateSeparatorShortcut(
    int ordinal,
    const std::wstring& shortcutPath) {
    IShellLinkW* shellLink = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_ShellLink,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&shellLink));

    if (FAILED(hr)) {
        return hr;
    }

    wchar_t systemDirectory[MAX_PATH] = {};

    UINT systemDirectoryLength =
        GetSystemDirectoryW(
            systemDirectory,
            ARRAYSIZE(systemDirectory));

    if (!systemDirectoryLength ||
        systemDirectoryLength >= ARRAYSIZE(systemDirectory)) {
        shellLink->Release();
        return HRESULT_FROM_WIN32(GetLastError());
    }

    std::wstring target =
        JoinPath(systemDirectory, L"cmd.exe");

    // If the raw test button is accidentally clicked before XAML styling is
    // added, launch a hidden cmd.exe that exits immediately.
    hr = shellLink->SetPath(target.c_str());

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetArguments(L"/d /c exit");
    }

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetShowCmd(SW_HIDE);
    }

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetIconLocation(
            g_iconPath.c_str(),
            0);
    }

    std::wstring description =
        GetSeparatorBaseName(ordinal);

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetDescription(
            description.c_str());
    }

    // Give every separator a distinct AppUserModelID so the Shell considers
    // them distinct taskbar identities even though they share cmd.exe as the
    // harmless launch target.
    if (SUCCEEDED(hr)) {
        hr = SetShortcutAppId(
            shellLink,
            GetSeparatorAppId(ordinal));
    }

    if (SUCCEEDED(hr)) {
        IPersistFile* persistFile = nullptr;

        hr = shellLink->QueryInterface(
            IID_PPV_ARGS(&persistFile));

        if (SUCCEEDED(hr)) {
            hr = persistFile->Save(
                shortcutPath.c_str(),
                TRUE);

            persistFile->Release();
        }
    }

    shellLink->Release();

    Wh_Log(
        L"[FILES] Create shortcut #%d '%s' hr=0x%08X",
        ordinal,
        shortcutPath.c_str(),
        static_cast<unsigned int>(hr));

    return hr;
}

// -----------------------------------------------------------------------------
// PIDL + PinManager helpers.
// -----------------------------------------------------------------------------

static HRESULT GetPidlForPath(
    const std::wstring& path,
    PIDLIST_ABSOLUTE* pidl) {
    *pidl = nullptr;

    SFGAOF attrs = 0;

    HRESULT hr = SHParseDisplayName(
        path.c_str(),
        nullptr,
        pidl,
        0,
        &attrs);

    Wh_Log(
        L"[PIDL] '%s' -> hr=0x%08X pidl=%p",
        path.c_str(),
        static_cast<unsigned int>(hr),
        *pidl);

    return hr;
}

static HRESULT CreatePinManager(
    IPinManagerInterop3** pinManager) {
    *pinManager = nullptr;

    IPinManagerInterop2* interop2 = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_PinManager,
        nullptr,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_IPinManagerInterop2,
        reinterpret_cast<void**>(&interop2));

    if (FAILED(hr)) {
        Wh_Log(
            L"[PIN] CoCreateInstance(IPinManagerInterop2) "
            L"failed hr=0x%08X",
            static_cast<unsigned int>(hr));
        return hr;
    }

    hr = interop2->QueryInterface(
        IID_IPinManagerInterop3,
        reinterpret_cast<void**>(pinManager));

    interop2->Release();

    Wh_Log(
        L"[PIN] QueryInterface(IPinManagerInterop3) "
        L"hr=0x%08X ptr=%p",
        static_cast<unsigned int>(hr),
        *pinManager);

    return hr;
}

// -----------------------------------------------------------------------------
// Creation / positioning.
// -----------------------------------------------------------------------------

static bool CreateAndPinSeparators() {
    if (g_settings.separators.empty()) {
        Wh_Log(L"[PIN] No separators configured");
        return true;
    }

    if (!WriteBinaryFile(
            g_iconPath,
            kSeparatorIcon,
            static_cast<DWORD>(sizeof(kSeparatorIcon)))) {
        return false;
    }

    // First create every backing shortcut, before mutating the taskbar.
    for (const auto& separator : g_settings.separators) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        HRESULT hr =
            CreateSeparatorShortcut(
                separator.ordinal,
                shortcutPath);

        if (FAILED(hr)) {
            Wh_Log(
                L"[PIN] Failed to create separator #%d",
                separator.ordinal);
            return false;
        }
    }

    IPinManagerInterop3* pinManager = nullptr;

    HRESULT hr = CreatePinManager(&pinManager);
    if (FAILED(hr) || !pinManager) {
        return false;
    }

    // Pin all shortcuts first. PinItemFromTrustedCaller is the modern working
    // path on this build; PinItemToTaskbarShim was observed to be a no-op.
    bool success = true;

    for (const auto& separator : g_settings.separators) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            success = false;
            continue;
        }

        Wh_Log(
            L"[PIN] Pin separator #%d",
            separator.ordinal);

        HRESULT pinHr =
            pinManager->PinItemFromTrustedCaller(
                pidl,
                PMC_TASKBANDPIN);

        Wh_Log(
            L"[PIN] PinItemFromTrustedCaller #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(pinHr));

        if (FAILED(pinHr)) {
            success = false;
        }

        CoTaskMemFree(pidl);
    }

    // Moving in ascending destination order makes multiple requested positions
    // deterministic as items are pulled forward from their initial appended
    // locations.
    std::vector<SeparatorSetting> moveOrder =
        g_settings.separators;

    std::stable_sort(
        moveOrder.begin(),
        moveOrder.end(),
        [](const SeparatorSetting& a, const SeparatorSetting& b) {
            return a.targetIndex < b.targetIndex;
        });

    for (const auto& separator : moveOrder) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            success = false;
            continue;
        }

        Wh_Log(
            L"[MOVE] separator #%d -> index=%d",
            separator.ordinal,
            separator.targetIndex);

        HRESULT moveHr =
            pinManager->MoveTaskbarPin(
                pidl,
                separator.targetIndex,
                PMC_TASKBANDREORDER);

        Wh_Log(
            L"[MOVE] MoveTaskbarPin #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(moveHr));

        if (FAILED(moveHr)) {
            success = false;
        }

        CoTaskMemFree(pidl);
    }

    pinManager->Release();

    return success;
}

// -----------------------------------------------------------------------------
// Destruction / cleanup.
// -----------------------------------------------------------------------------

static bool UnpinAndDeleteSeparators() {
    if (g_settings.separators.empty()) {
        DeleteFileIfPresent(g_iconPath);
        return true;
    }

    IPinManagerInterop3* pinManager = nullptr;

    HRESULT hr = CreatePinManager(&pinManager);

    if (FAILED(hr) || !pinManager) {
        Wh_Log(
            L"[CLEANUP] Can't acquire PinManager; "
            L"leaving backing files in place to avoid broken pins");
        return false;
    }

    bool allUnpinned = true;

    // Reverse order isn't required because unpinning is PIDL-based, but it
    // minimizes visual/index churn while several separators disappear.
    for (auto it = g_settings.separators.rbegin();
         it != g_settings.separators.rend();
         ++it) {
        const auto& separator = *it;

        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            Wh_Log(
                L"[CLEANUP] Can't resolve separator #%d; "
                L"keeping its shortcut",
                separator.ordinal);
            allUnpinned = false;
            continue;
        }

        Wh_Log(
            L"[CLEANUP] Unpin separator #%d",
            separator.ordinal);

        // 11 is the exact caller value observed during the native modern
        // Notepad unpin trace on this build, and was already tested with
        // UnpinTaskbarItem successfully.
        HRESULT unpinHr =
            pinManager->UnpinTaskbarItem(
                pidl,
                PMC_JUMPVIEWBROKER);

        Wh_Log(
            L"[CLEANUP] UnpinTaskbarItem #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(unpinHr));

        CoTaskMemFree(pidl);

        if (SUCCEEDED(unpinHr)) {
            DeleteFileIfPresent(shortcutPath);
        } else {
            allUnpinned = false;
        }
    }

    pinManager->Release();

    if (allUnpinned) {
        DeleteFileIfPresent(g_iconPath);
    } else {
        Wh_Log(
            L"[CLEANUP] At least one unpin failed; "
            L"keeping shared icon '%s'",
            g_iconPath.c_str());
    }

    return allUnpinned;
}

// -----------------------------------------------------------------------------
// Windhawk lifetime.
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[INIT] Taskbar separator prototype loading");

    LoadSettings();

    if (!InitializeStoragePath()) {
        return FALSE;
    }

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(hrInit);

    if (hrInit == RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"[INIT] COM already initialized with another "
            L"apartment model; continuing");
    } else if (FAILED(hrInit)) {
        Wh_Log(
            L"[INIT] CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(hrInit));
        return FALSE;
    }

    bool success =
        CreateAndPinSeparators();

    if (shouldUninitialize) {
        CoUninitialize();
    }

    if (!success) {
        Wh_Log(
            L"[INIT] One or more prototype operations failed; "
            L"mod remains loaded so unload can still clean up");
    }

    Wh_Log(L"[INIT] Taskbar separator prototype loaded");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[UNINIT] Taskbar separator prototype unloading");

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(hrInit);

    if (hrInit == RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"[UNINIT] COM already initialized with another "
            L"apartment model; continuing");
    } else if (FAILED(hrInit)) {
        Wh_Log(
            L"[UNINIT] CoInitializeEx failed hr=0x%08X; "
            L"can't safely remove separators",
            static_cast<unsigned int>(hrInit));
        return;
    }

    UnpinAndDeleteSeparators();

    if (shouldUninitialize) {
        CoUninitialize();
    }

    Wh_Log(L"[UNINIT] Taskbar separator prototype unloaded");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // Important: don't overwrite g_settings here. The old instance must retain
    // its old filenames/ordinals so Wh_ModUninit can remove exactly what it
    // created. Windhawk will then reload the mod and Wh_ModInit will read the
    // new settings.
    *bReload = TRUE;
    return TRUE;
}
