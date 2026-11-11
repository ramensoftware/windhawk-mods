// ==WindhawkMod==
// @id              lockscreen-background
// @name            Animated Wallpaper for the Windows Lock Screen
// @description     Use local videos, GIFs, and images as the Windows 11 lock screen and optional sign-in wallpaper.
// @version         0.2.16
// @author          sunnyturtle01
// @github          https://github.com/quyle231003
// @license         GPL-3.0
// @include         LockApp.exe
// @include         explorer.exe
// @include         LogonUI.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lshell32 -ladvapi32 -luuid -luserenv -lmfplat -lmfreadwrite -lmfuuid -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Animated Wallpaper for the Windows Lock Screen

Play local MP4 videos, animated GIFs, and images on the Windows lock screen.
Copy a file's path from File Explorer, paste it in **Settings**, and save.
Sign-in setup instructions are beside **Play animated wallpaper on the sign-in screen**.

![Lock screen and sign-in preview](https://raw.githubusercontent.com/quyle231003/windhawk-mods/lockscreen-preview/lockscreen-preview.gif)

<details>
<summary>Compatibility and storage</summary>

- **Clock Styler** also hides the default clock and widgets. When using both
  mods, leave **Hide lock screen overlays** off here and let Clock Styler
  manage them. Its custom clock can remain visible. If both mods manage the
  same Windows elements, turning one option off may reveal them until the
  other reapplies its settings. For a wallpaper-only view, disable Clock
  Styler and enable this mod's hide switch. This guidance is based on source
  inspection. The combination has not been tested across Windows builds.
- **Files** must be smaller than 100 MiB for MP4, 10 MiB for GIF, or 20 MiB
  for PNG/JPEG. MP4 support depends on installed Windows codecs.
- **Sign-in playback** loops the complete prepared clip at 18 fps. The frame
  cache uses up to 512 MiB. Long clips are downscaled, and Windows wallpaper
  is used if preparation fails or the cache is unavailable.
- **Storage** includes a per-user media copy and a sign-in cache in Windhawk
  mod storage. The source file keeps its original permissions. Caches stay
  when the mod is disabled and are intended to be removed with the mod.
  The sign-in cache belongs to the first Windows account that creates it.
  Earlier test builds may have left LockscreenMotionBackground folders in
  Public Pictures or ProgramData.
- The Windows visual tree can vary between builds. The first Win+L after
  recompilation can sometimes open the PIN screen directly.

</details>

## Credits

Sign-in UI discovery and the writable-bitmap approach were informed by
[Lock Screen Live Wallpaper](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/lock-screen-live-wallpaper.wh.cpp)
by grn15RenS (GPL-3.0). UI discovery was also informed by
[Lock Screen Clock Styler](https://github.com/ali0zdas/windhawk-mod-lock-screen-clock-styler)
by ali0zdas (MIT). Distributed under [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.html).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- BackgroundEnabled: true
  $name: Enable custom background
- BackgroundPath: ""
  $name: Background file path
  $description: |
    In File Explorer, right-click a local MP4, GIF, PNG or JPEG and choose Copy as path.
    Paste it here and click Save. Example C:\Users\Public\Pictures\wallpaper.mp4.
    Clear this field to use the Windows background.
    Preparing the optional sign-in wallpaper can take a moment after saving.
- BackgroundFit: fill
  $name: Background fit
  $options:
    - fill: Fill (may crop)
    - fit: Fit (show the entire file)
- HideLockScreenOverlays: false
  $name: Hide lock screen overlays
  $description: |
    Hide the Windows clock, date, widgets, music controls, Spotlight tips and status icons before sign-in.
    Turn this off when using Lock Screen Clock Styler to manage the clock and widgets.
    A custom clock from another mod can remain visible.
- SignInBackground: false
  $name: Play animated wallpaper on the sign-in screen
  $description: |
    Optional. Complete these steps to show animated wallpaper behind the PIN box.
    1. Windhawk Portable users must exit Windhawk and open windhawk.exe with Run as administrator.
    2. In Windhawk Settings > Advanced settings > More advanced settings > Process inclusion list, add LogonUI.exe, then Save and restart Windhawk.
    3. In Windows Settings > Personalization > Lock screen, turn on Show the lock screen background picture on the sign-in screen.
    4. Turn this option on, click Save and allow time to prepare the video. The mod log shows Sign-in wallpaper ready when finished.
    Windows wallpaper is shown while the prepared cache is unavailable.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#undef GetCurrentTime
#undef min
#undef max
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <windhawk_utils.h>
#include <shcore.h>
#include <algorithm>
#include <atomic>
#include <string>
#include <memory>
#include <chrono>
#include <string_view>
#include <utility>


namespace x = winrt::Windows::UI::Xaml;
namespace c = winrt::Windows::UI::Xaml::Controls;
namespace m = winrt::Windows::UI::Xaml::Media;
namespace imaging = winrt::Windows::UI::Xaml::Media::Imaging;

HANDLE stopEvent = nullptr, worker = nullptr;

// SPDX-License-Identifier: GPL-3.0-only
// Modified sign-in implementation, 2026. See the README for attribution.
// Media import helpers.
// File preparation runs with the signed-in user's rights in Explorer.
#include <aclapi.h>
#include <sddl.h>
#include <shlobj.h>
#include <userenv.h>
#include <vector>

namespace mediaPicker {
std::wstring userSid, lockAppSid, mediaKey;
struct Settings {
    std::wstring source;
    bool enabled, signIn, fit;
    bool hideOverlays;
    uint64_t revision;
};
struct SettingsStore {
    SRWLOCK lock = SRWLOCK_INIT;
    std::shared_ptr<const Settings> current;

    std::shared_ptr<const Settings> load() {
        AcquireSRWLockShared(&lock);
        auto snapshot = current;
        ReleaseSRWLockShared(&lock);
        return snapshot;
    }

    void store(std::shared_ptr<const Settings> snapshot) {
        AcquireSRWLockExclusive(&lock);
        current.swap(snapshot);
        ReleaseSRWLockExclusive(&lock);
    }
} settings;
std::wstring storageRoot;
HANDLE settingsEvent = nullptr;

struct Handle {
    HANDLE value = INVALID_HANDLE_VALUE;
    explicit Handle(HANDLE h) : value(h) {}
    ~Handle() { if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value); }
    Handle(Handle const&) = delete;
    Handle& operator=(Handle const&) = delete;
    explicit operator bool() const { return value && value != INVALID_HANDLE_VALUE; }
};
struct LocalMemory {
    HLOCAL value = nullptr;
    ~LocalMemory() { if (value) LocalFree(value); }
};
void checkWin32(bool ok) {
    if (!ok) winrt::throw_hresult(HRESULT_FROM_WIN32(GetLastError()));
}
bool stopping() { return stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0; }

std::wstring currentUserSid() {
    HANDLE tokenRaw = nullptr;
    checkWin32(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenRaw));
    Handle token(tokenRaw);
    DWORD size = 0;
    GetTokenInformation(token.value, TokenUser, nullptr, 0, &size);
    if (!size) winrt::throw_last_error();
    std::vector<BYTE> bytes(size);
    checkWin32(GetTokenInformation(token.value, TokenUser, bytes.data(), size, &size));
    PWSTR text = nullptr;
    checkWin32(ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(bytes.data())->User.Sid, &text));
    LocalMemory freeText{reinterpret_cast<HLOCAL>(text)};
    return text;
}
void initIdentity() {
    userSid = currentUserSid();
    // Microsoft documents deriving a packaged app SID from its package family.
    // No AppContainer profile is created or modified here.
    PSID packageSid = nullptr;
    winrt::check_hresult(DeriveAppContainerSidFromAppContainerName(
        L"Microsoft.LockApp_cw5n1h2txyewy", &packageSid));
    PWSTR packageText = nullptr;
    BOOL converted = ConvertSidToStringSidW(packageSid, &packageText);
    DWORD conversionError = converted ? ERROR_SUCCESS : GetLastError();
    FreeSid(packageSid);
    if (!converted) winrt::throw_hresult(HRESULT_FROM_WIN32(conversionError));
    LocalMemory freeText{reinterpret_cast<HLOCAL>(packageText)};
    lockAppSid = packageText;
    mediaKey =
        L"ChosenMedia.v2." + userSid;
}
std::wstring cacheFolder();
bool isManagedFilename(std::wstring const& name);

bool isManagedCopy(std::wstring const& path, std::wstring const& folder) {
    const auto prefix = folder + L"\\";
    if (!path.starts_with(prefix)) return false;
    const auto name = path.substr(prefix.size());
    return name.find_first_of(L"\\/:") == std::wstring::npos && isManagedFilename(name);
}
std::wstring mediaRecord(bool* legacy = nullptr) {
    wchar_t value[32768]{};
    Wh_GetStringValue(mediaKey.c_str(), value, ARRAYSIZE(value));
    std::wstring record(value);
    if (legacy) *legacy = record.find(L'\n') != std::wstring::npos;
    // Read the installed version's old records during upgrade.
    if (record.find(L'*') == std::wstring::npos) {
        std::replace(record.begin(), record.end(), L'\n', L'*');
    }
    return record;
}
std::wstring chosenPath(
    std::wstring const& selectedSource
) {
    const auto entry = mediaRecord();
    const auto first = entry.find(L'*');
    if (first == std::wstring::npos || entry.substr(0, first) != selectedSource) return {};
    const auto second = entry.find(L'*', first + 1);
    if (second == std::wstring::npos || entry.find(L'*', second + 1) != std::wstring::npos) return {};
    auto copy = entry.substr(second + 1);
    return isManagedCopy(copy, cacheFolder()) ? copy : L"";
}

std::wstring extension(std::wstring const& path) {
    auto dot = path.find_last_of(L'.');
    if (dot == std::wstring::npos) return {};
    std::wstring ext = path.substr(dot);
    for (auto& ch : ext) ch = static_cast<wchar_t>(towlower(ch));
    return ext;
}
ULONGLONG fileLimit(std::wstring const& ext) {
    if (ext == L".mp4") return 100ull * 1024 * 1024;
    if (ext == L".gif") return 10ull * 1024 * 1024;
    if (ext == L".png" || ext == L".jpg" || ext == L".jpeg") return 20ull * 1024 * 1024;
    return 0;
}
bool plainLocalPath(std::wstring const& path) {
    if (path.size() < 4 || path[1] != L':' || path[2] != L'\\') return false;
    wchar_t drive[] = {path[0], L':', L'\\', 0};
    if (GetDriveTypeW(drive) != DRIVE_FIXED) return false;
    // Reject links/junctions anywhere in the managed destination path.
    for (size_t n = path.find(L'\\', 3);; n = path.find(L'\\', n + 1)) {
        auto part = n == std::wstring::npos ? path : path.substr(0, n);
        DWORD attr = GetFileAttributesW(part.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_REPARSE_POINT)) return false;
        if (n == std::wstring::npos) break;
    }
    return true;
}

struct Security {
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    explicit Security(bool directory) {
        // User/SYSTEM/admins manage the copy. Only LockApp receives read only;
        // directory execute is traverse permission, not permission to write.
        std::wstring sddl = L"D:P(A;OICI;FA;;;SY)(A;OICI;FA;;;BA)(A;OICI;FA;;;" + userSid +
            L")(A;OICI;" + (directory ? std::wstring(L"0x1200a9") : std::wstring(L"0x120089")) + L";;;" + lockAppSid + L")";
        checkWin32(ConvertStringSecurityDescriptorToSecurityDescriptorW(
            sddl.c_str(), SDDL_REVISION_1, &descriptor, nullptr));
    }
    ~Security() { if (descriptor) LocalFree(descriptor); }
    SECURITY_ATTRIBUTES attributes() {
        return SECURITY_ATTRIBUTES{sizeof(SECURITY_ATTRIBUTES), descriptor, FALSE};
    }
};
struct CacheLock {
    HANDLE mutex = nullptr;
    CacheLock() {
        auto sddl = L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;" + userSid + L")";
        PSECURITY_DESCRIPTOR descriptor = nullptr;
        checkWin32(ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(),
            SDDL_REVISION_1, &descriptor, nullptr));
        LocalMemory freeDescriptor{reinterpret_cast<HLOCAL>(descriptor)};
        SECURITY_ATTRIBUTES attributes{sizeof(SECURITY_ATTRIBUTES), descriptor, FALSE};
        auto name = L"Global\\LockscreenMotionBackground.MediaImport.v1." + userSid;
        mutex = CreateMutexW(&attributes, FALSE, name.c_str());
        checkWin32(mutex != nullptr);
        for (;;) {
            DWORD wait = WaitForSingleObject(mutex, 100);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_ABANDONED) return;
            if (wait == WAIT_FAILED || stopping()) {
                DWORD error = wait == WAIT_FAILED ? GetLastError() : ERROR_CANCELLED;
                CloseHandle(mutex); mutex = nullptr;
                winrt::throw_hresult(HRESULT_FROM_WIN32(error));
            }
        }
    }
    ~CacheLock() { if (mutex) { ReleaseMutex(mutex); CloseHandle(mutex); } }
    CacheLock(CacheLock const&) = delete;
    CacheLock& operator=(CacheLock const&) = delete;
};
// Returns a non-delete-shared handle: a different process can't swap the cache
// directory for a link while this process is importing/cleaning media.
HANDLE openManagedFolder(std::wstring const& folder, bool create) {
    auto slash = folder.find_last_of(L'\\');
    if (slash == std::wstring::npos || !plainLocalPath(folder.substr(0, slash)))
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME));
    Security security(true);
    auto attributes = security.attributes();
    if (create && !CreateDirectoryW(folder.c_str(), &attributes) && GetLastError() != ERROR_ALREADY_EXISTS)
        winrt::throw_last_error();
    Handle handle(CreateFileW(folder.c_str(), READ_CONTROL | FILE_READ_ATTRIBUTES
        ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr));
    checkWin32(static_cast<bool>(handle));
    BY_HANDLE_FILE_INFORMATION info{};
    checkWin32(GetFileInformationByHandle(handle.value, &info));
    if (!(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
        winrt::throw_hresult(E_ACCESSDENIED);
    PSID owner = nullptr;
    PSECURITY_DESCRIPTOR existing = nullptr;
    DWORD error = GetSecurityInfo(handle.value, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
        &owner, nullptr, nullptr, nullptr, &existing);
    LocalMemory freeExisting{reinterpret_cast<HLOCAL>(existing)};
    if (error) winrt::throw_hresult(HRESULT_FROM_WIN32(error));
    PSID expected = nullptr;
    checkWin32(ConvertStringSidToSidW(userSid.c_str(), &expected));
    LocalMemory freeExpected{reinterpret_cast<HLOCAL>(expected)};
    if (!owner || !EqualSid(owner, expected)) winrt::throw_hresult(E_ACCESSDENIED);
    HANDLE result = handle.value;
    handle.value = INVALID_HANDLE_VALUE;
    return result;
}
std::wstring cacheFolder() {
    return storageRoot + L"\\media-" + userSid;
}

std::wstring importFile(std::wstring const& source, std::wstring const& folder
    , std::wstring& record
) {
    auto ext = extension(source);
    ULONGLONG limit = fileLimit(ext);
    if (!limit) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
    // The normal user explicitly selected this file. Open it with that user's
    // existing rights; never take ownership, impersonate or request elevation.
    Handle input(CreateFileW(source.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    checkWin32(static_cast<bool>(input));
    if (GetFileType(input.value) != FILE_TYPE_DISK) winrt::throw_hresult(E_INVALIDARG);
    LARGE_INTEGER size{};
    checkWin32(GetFileSizeEx(input.value, &size));
    if (size.QuadPart <= 0 || static_cast<ULONGLONG>(size.QuadPart) >= limit)
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE));
    Handle directory(openManagedFolder(folder, true));
    FILETIME modified{};
    checkWin32(GetFileTime(input.value, nullptr, nullptr, &modified));
    const auto ticks = (static_cast<uint64_t>(modified.dwHighDateTime) << 32) | modified.dwLowDateTime;
    // '*' cannot occur in a validated source, copy filename or stamp.
    record = source + L'*' + std::to_wstring(size.QuadPart) + L':' + std::to_wstring(ticks) + L'*';
    bool legacy = false;
    const auto cached = mediaRecord(&legacy);
    if (cached.starts_with(record)) {
        auto copy = cached.substr(record.size());
        WIN32_FILE_ATTRIBUTE_DATA info{};
        if (isManagedCopy(copy, folder) &&
            GetFileAttributesExW(copy.c_str(), GetFileExInfoStandard, &info) &&
            !(info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) &&
            !info.nFileSizeHigh && info.nFileSizeLow == size.QuadPart) {
            // Migrate validated legacy records without copying the media.
            if (legacy && !Wh_SetStringValue(mediaKey.c_str(), cached.c_str())) {
                winrt::throw_hresult(E_FAIL);
            }
            record.clear();
            Wh_Log(L"Reusing unchanged lock screen media copy");
            return copy;
        }
    }
    GUID guid{};
    winrt::check_hresult(CoCreateGuid(&guid));
    wchar_t guidText[40]{};
    if (!StringFromGUID2(guid, guidText, ARRAYSIZE(guidText))) winrt::throw_hresult(E_FAIL);
    std::wstring destination = folder + L"\\media-" + guidText + ext;
    bool created = false;
    try {
        Security security(false);
        auto attributes = security.attributes();
        Handle output(CreateFileW(destination.c_str(), GENERIC_WRITE, 0, &attributes,
            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr));
        checkWin32(static_cast<bool>(output));
        created = true;
        std::vector<BYTE> buffer(256 * 1024);
        LONGLONG copied = 0;
        for (;;) {
            if (stopping()) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_CANCELLED));
            DWORD read = 0;
            checkWin32(ReadFile(input.value, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr));
            if (!read) break;
            DWORD written = 0;
            checkWin32(WriteFile(output.value, buffer.data(), read, &written, nullptr));
            if (read != written) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_WRITE_FAULT));
            copied += read;
            if (copied > size.QuadPart) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_FILE_INVALID));
        }
        if (copied != size.QuadPart) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_FILE_INVALID));
        checkWin32(FlushFileBuffers(output.value));
    } catch (...) {
        if (created) DeleteFileW(destination.c_str());
        throw;
    }
    return destination;
}

bool isManagedFilename(std::wstring const& name) {
    // media-{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}.ext
    if (name.size() < 48 || name.rfind(L"media-{", 0) != 0 || name[43] != L'}' || name[44] != L'.') return false;
    GUID guid{};
    auto text = name.substr(6, 38);
    return SUCCEEDED(CLSIDFromString(text.c_str(), &guid)) && fileLimit(extension(name)) != 0;
}
void prune(std::wstring const& folder, std::wstring const& keep
    , std::wstring const& source
) {
    try {
        CacheLock lock;
        auto selected = chosenPath(
            source
        );
        Handle directory(openManagedFolder(folder, false));
        WIN32_FIND_DATAW data{};
        HANDLE search = FindFirstFileW((folder + L"\\media-*").c_str(), &data);
        if (search == INVALID_HANDLE_VALUE) return;
        do {
            if (data.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) continue;
            auto path = folder + L"\\" + data.cFileName;
            if (_wcsicmp(path.c_str(), keep.c_str()) != 0 &&
                _wcsicmp(path.c_str(), selected.c_str()) != 0 && isManagedFilename(data.cFileName))
                DeleteFileW(path.c_str()); // A playing file stays until a later prune.
        } while (FindNextFileW(search, &data));
        FindClose(search);
    } catch (...) { /* Missing/unavailable cache is harmless. */ }
}

std::wstring sourceSetting() {
    auto value = WindhawkUtils::StringSetting::make(L"BackgroundPath");
    std::wstring path(value.get() ? value.get() : L"");
    const auto start = path.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return {};
    path = path.substr(start, path.find_last_not_of(L" \t\r\n") - start + 1);
    // Windows' Copy as path can include double quotes.
    if (path.size() >= 2 && path.front() == L'"' && path.back() == L'"')
        path = path.substr(1, path.size() - 2);
    return path;
}

std::shared_ptr<Settings> readSettings() {
    auto old = settings.load();
    auto fit = WindhawkUtils::StringSetting::make(L"BackgroundFit");
    return std::make_shared<Settings>(Settings{sourceSetting(), Wh_GetIntSetting(L"BackgroundEnabled") != 0,
        Wh_GetIntSetting(L"SignInBackground") != 0, fit.get() && wcscmp(fit.get(), L"fit") == 0,
        Wh_GetIntSetting(L"HideLockScreenOverlays") != 0,
        old ? old->revision + 1 : 1});
}

void applySource(std::wstring const& source) {
    CacheLock lock;
    if (source.empty()) {
        if (!Wh_SetStringValue(mediaKey.c_str(), L""))
            winrt::throw_hresult(E_FAIL);
        return;
    }
    if (source.find(L':', 2) != std::wstring::npos ||
        source.find_first_of(L"\"*?\r\n") != std::wstring::npos ||
        !plainLocalPath(source))
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME));

    std::wstring record;
    auto managed = importFile(source, cacheFolder(), record);
    if (record.empty()) return;
    record += managed;
    if (stopping() || !Wh_SetStringValue(mediaKey.c_str(), record.c_str())) {
        DeleteFileW(managed.c_str());
        if (!stopping()) winrt::throw_hresult(E_FAIL);
    }
}


} // namespace mediaPicker
// Shared file format for the VM-only sign-in player and the host pack builder.
#include <cstdint>

namespace signInPack {
constexpr uint32_t magic = 0x33465951; // QYF3
constexpr uint32_t maxWidth = 720, maxHeight = 405, fps = 18;
constexpr uint64_t maxCacheBytes = 512ull * 1024 * 1024;

struct Header {
    uint32_t tag, width, height, count;

    bool valid() const {
        return tag == magic && width && width <= maxWidth && height && height <= maxHeight &&
               count &&
               static_cast<uint64_t>(count) * frameBytes() <= maxCacheBytes;
    }
    uint32_t frameBytes() const { return width * height * 4; }
};
static_assert(sizeof(Header) == 16);
}

#include <objidl.h>
#include <gdiplus.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <winrt/base.h>
#include <cmath>
#include <functional>

namespace signInEncode {
using signInPack::Header;
using Pixels = std::vector<BYTE>;
using FrameSink = std::function<void(Pixels const&)>;

bool validVideoSize(UINT width, UINT height) {
    return width && height && width <= 8192 && height <= 8192 &&
        static_cast<uint64_t>(width) * height <= 32'000'000;
}

RECT pictureArea(IMFMediaType* type, UINT width, UINT height, UINT nativeWidth, UINT nativeHeight) {
    MFVideoArea aperture{};
    UINT32 bytes = 0;
    if (SUCCEEDED(type->GetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE, reinterpret_cast<BYTE*>(&aperture),
            sizeof(aperture), &bytes)) && bytes == sizeof(aperture) &&
        !aperture.OffsetX.fract && !aperture.OffsetY.fract && aperture.OffsetX.value >= 0 &&
        aperture.OffsetY.value >= 0 && aperture.Area.cx > 0 && aperture.Area.cy > 0 &&
        static_cast<int64_t>(aperture.OffsetX.value) + aperture.Area.cx <= width &&
        static_cast<int64_t>(aperture.OffsetY.value) + aperture.Area.cy <= height) {
        return {aperture.OffsetX.value, aperture.OffsetY.value,
            aperture.OffsetX.value + aperture.Area.cx, aperture.OffsetY.value + aperture.Area.cy};
    }
    // Some decoders add a few alignment rows without reporting an aperture.
    if (width == nativeWidth && height >= nativeHeight && height - nativeHeight < 16) height = nativeHeight;
    return {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
}

struct Animation {
    Header header{signInPack::magic, 0, 0, 0};
    Pixels frames;
    FrameSink sink;

    void setSize(UINT width, UINT height
        , uint64_t expectedFrames = 0
    ) {
        if (!validVideoSize(width, height)) winrt::throw_hresult(E_INVALIDARG);
        double scale = std::min({1.0, double(signInPack::maxWidth) / width,
                                     double(signInPack::maxHeight) / height});
        if (expectedFrames) {
            const double availablePixels = double(signInPack::maxCacheBytes) / expectedFrames / 4;
            const double minimumScale = std::min(scale, std::min(480.0 / width, 270.0 / height));
            const double budgetScale = std::sqrt(availablePixels / (double(width) * height));
            scale = std::max(minimumScale, std::min(scale, budgetScale));
        }
        header.width = std::max(1u, static_cast<UINT>(width * scale));
        header.height = std::max(1u, static_cast<UINT>(height * scale));
        if (expectedFrames > signInPack::maxCacheBytes / header.frameBytes())
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE));
    }

    void append(Pixels const& frame) {
        if (header.count >= signInPack::maxCacheBytes / header.frameBytes())
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE));
        if (sink) sink(frame);
        else frames.insert(frames.end(), frame.begin(), frame.end());
        ++header.count;
    }
};

void resizeRgb32(BYTE const* top, LONG stride, UINT width, UINT height,
                 Header const& target, Pixels& output) {
    // Map the centers of output pixels back into the source, using 8-bit weights.
    auto coordinate = [](UINT index, UINT sourceSize, UINT outputSize) {
        const int center = static_cast<int>((2 * index + 1) * sourceSize * 128 / outputSize) - 128;
        return static_cast<UINT>(std::clamp(center, 0, static_cast<int>((sourceSize - 1) * 256)));
    };
    UINT columns[signInPack::maxWidth]{};
    for (UINT x = 0; x < target.width; ++x) columns[x] = coordinate(x, width, target.width);
    for (UINT y = 0; y < target.height; ++y) {
        const UINT v = coordinate(y, height, target.height);
        const UINT y0 = v / 256, y1 = std::min(y0 + 1, height - 1);
        const UINT wy = v % 256;
        auto upper = top + static_cast<ptrdiff_t>(y0) * stride;
        auto lower = top + static_cast<ptrdiff_t>(y1) * stride;
        for (UINT x = 0; x < target.width; ++x) {
            const UINT u = columns[x];
            const UINT x0 = u / 256 * 4, x1 = std::min(u / 256 + 1, width - 1) * 4;
            const UINT wx = u % 256;
            auto dst = output.data() + (y * target.width + x) * 4;
            for (UINT channel = 0; channel < 3; ++channel) {
                const UINT a = upper[x0 + channel] * (256 - wx) + upper[x1 + channel] * wx;
                const UINT b = lower[x0 + channel] * (256 - wx) + lower[x1 + channel] * wx;
                dst[channel] = static_cast<BYTE>((a * (256 - wy) + b * wy + 32768) / 65536);
            }
            dst[3] = 255;
        }
    }
}

struct MediaFoundation {
    MediaFoundation() { winrt::check_hresult(MFStartup(MF_VERSION)); }
    ~MediaFoundation() { MFShutdown(); }
};

void checkStop(bool (*stopping)()) {
    if (stopping && stopping()) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_CANCELLED));
}

Animation video(wchar_t const* input, bool (*stopping)() = nullptr
    , FrameSink sink = {}
) {
    MediaFoundation mf;
    winrt::com_ptr<IMFAttributes> attrs;
    winrt::check_hresult(MFCreateAttributes(attrs.put(), 1));
    winrt::check_hresult(attrs->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE));
    winrt::com_ptr<IMFSourceReader> reader;
    winrt::check_hresult(MFCreateSourceReaderFromURL(input, attrs.get(), reader.put()));
    constexpr DWORD stream = MF_SOURCE_READER_FIRST_VIDEO_STREAM;
    winrt::check_hresult(reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE));
    winrt::check_hresult(reader->SetStreamSelection(stream, TRUE));
    winrt::com_ptr<IMFMediaType> type;
    winrt::check_hresult(MFCreateMediaType(type.put()));
    winrt::check_hresult(type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
    winrt::check_hresult(type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
    winrt::check_hresult(reader->SetCurrentMediaType(stream, nullptr, type.get()));
    type = nullptr;
    winrt::check_hresult(reader->GetCurrentMediaType(stream, type.put()));
    UINT32 width = 0, height = 0;
    winrt::check_hresult(MFGetAttributeSize(type.get(), MF_MT_FRAME_SIZE, &width, &height));
    if (!validVideoSize(width, height)) winrt::throw_hresult(MF_E_INVALIDMEDIATYPE);
    const UINT nativeWidth = width, nativeHeight = height;
    auto area = pictureArea(type.get(), width, height, nativeWidth, nativeHeight);
    Animation animation;
    constexpr LONGLONG ticksPerSecond = 10'000'000;
    PROPVARIANT duration{};
    uint64_t expectedFrames = 0;
    if (SUCCEEDED(reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &duration)) &&
        duration.vt == VT_UI8) {
        const auto ticks = duration.uhVal.QuadPart;
        if (ticks < UINT64_MAX / signInPack::fps)
            expectedFrames = ticks / ticksPerSecond * signInPack::fps +
                ((ticks % ticksPerSecond) * signInPack::fps + ticksPerSecond - 1) / ticksPerSecond;
    }
    PropVariantClear(&duration);
    animation.sink = std::move(sink);
    // Leave room for rounding at the final sample boundary.
    auto getStride = [&] {
        UINT32 strideAttr = 0;
        if (SUCCEEDED(type->GetUINT32(MF_MT_DEFAULT_STRIDE, &strideAttr))) return static_cast<LONG>(strideAttr);
        LONG stride = 0;
        winrt::check_hresult(MFGetStrideForBitmapInfoHeader(MFVideoFormat_RGB32.Data1, width, &stride));
        return stride;
    };
    LONG stride = getStride();

    Pixels previous(animation.header.frameBytes());
    auto resize = [&](BYTE const* top, LONG rowStride) {
        const auto offset = static_cast<ptrdiff_t>(area.top) * rowStride + area.left * 4;
        resizeRgb32(top + offset, rowStride, area.right - area.left, area.bottom - area.top, animation.header, previous);
    };
    auto decode = [&](IMFSample* sample, LONG sampleStride) {
        winrt::com_ptr<IMFMediaBuffer> buffer;
        winrt::check_hresult(sample->ConvertToContiguousBuffer(buffer.put()));
        if (auto planar = buffer.try_as<IMF2DBuffer>()) {
            BYTE* top = nullptr;
            LONG rowStride = 0;
            winrt::check_hresult(planar->Lock2D(&top, &rowStride));
            const auto pitch = std::abs(static_cast<int64_t>(rowStride));
            const bool valid = top && pitch >= width * 4 && pitch <= 65536;
            if (valid) resize(top, rowStride);
            planar->Unlock2D();
            if (!valid) winrt::throw_hresult(E_FAIL);
        } else {
            const auto pitch = std::abs(static_cast<int64_t>(sampleStride));
            if (pitch < width * 4 || pitch > 65536) winrt::throw_hresult(E_INVALIDARG);
            BYTE* bytes = nullptr;
            DWORD length = 0;
            winrt::check_hresult(buffer->Lock(&bytes, nullptr, &length));
            const bool valid = bytes && length >= pitch * (height - 1) + width * 4;
            if (valid) {
                auto top = sampleStride < 0 ? bytes + pitch * (height - 1) : bytes;
                resize(top, sampleStride);
            }
            buffer->Unlock();
            if (!valid) winrt::throw_hresult(E_FAIL);
        }
    };
    LONGLONG origin = 0, last = 0;
    bool haveFrame = false;
    LONGLONG tail = ticksPerSecond / signInPack::fps;
    unsigned emptyReads = 0;
    for (;;) {
        checkStop(stopping);
        winrt::com_ptr<IMFSample> sample;
        DWORD flags = 0;
        LONGLONG time = 0;
        winrt::check_hresult(reader->ReadSample(stream, 0, nullptr, &flags, &time, sample.put()));
        if ((flags & MF_SOURCE_READERF_ENDOFSTREAM) && !sample) break;
        if (flags & MF_SOURCE_READERF_ERROR) winrt::throw_hresult(E_FAIL);
        if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED) {
            type = nullptr;
            winrt::check_hresult(reader->GetCurrentMediaType(stream, type.put()));
            UINT32 newWidth = 0, newHeight = 0;
            GUID subtype{};
            winrt::check_hresult(MFGetAttributeSize(type.get(), MF_MT_FRAME_SIZE, &newWidth, &newHeight));
            winrt::check_hresult(type->GetGUID(MF_MT_SUBTYPE, &subtype));
            if (!validVideoSize(newWidth, newHeight) || subtype != MFVideoFormat_RGB32)
                winrt::throw_hresult(MF_E_INVALIDMEDIATYPE);
            width = newWidth;
            height = newHeight;
            area = pictureArea(type.get(), width, height, nativeWidth, nativeHeight);
            stride = getStride();
        }
        if (!sample) {
            if (++emptyReads == 2400) winrt::throw_hresult(E_FAIL);
            continue;
        }
        emptyReads = 0;
        if (!haveFrame) origin = time;
        if (!haveFrame) {
            animation.setSize(area.right - area.left, area.bottom - area.top, expectedFrames ? expectedFrames + 2 : 0);
            previous.resize(animation.header.frameBytes());
        }
        if (time < INT64_MIN / 4 || time > INT64_MAX / 4)
            winrt::throw_hresult(E_INVALIDARG);
        time = std::max(time - origin, last);
        while (haveFrame && animation.header.count * ticksPerSecond / signInPack::fps < time) {
            checkStop(stopping);
            animation.append(previous);
        }
        LONGLONG duration = 0;
        if (SUCCEEDED(sample->GetSampleDuration(&duration)) && duration > 0 && duration < INT64_MAX / 2) tail = duration;
        else if (time > last) tail = time - last;
        // Release Media Foundation's sample before requesting another decoded frame.
        decode(sample.get(), stride);
        last = time;
        haveFrame = true;
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
    }
    while (haveFrame && animation.header.count * ticksPerSecond / signInPack::fps < last + tail) {
        checkStop(stopping);
        animation.append(previous);
    }
    return animation;
}

struct GdiPlus {
    ULONG_PTR token = 0;
    GdiPlus() {
        Gdiplus::GdiplusStartupInput input;
        if (Gdiplus::GdiplusStartup(&token, &input, nullptr) != Gdiplus::Ok) winrt::throw_hresult(E_FAIL);
    }
    ~GdiPlus() { Gdiplus::GdiplusShutdown(token); }
};

Animation picture(wchar_t const* input, bool gif, bool (*stopping)() = nullptr
    , FrameSink sink = {}
) {
    using namespace Gdiplus;
    GdiPlus gdiplus;
    Image image(input);
    if (image.GetLastStatus() != Ok) winrt::throw_hresult(E_FAIL);
    Animation animation;
    animation.sink = std::move(sink);
    const UINT count = gif ? image.GetFrameCount(&FrameDimensionTime) : 1;
    std::vector<BYTE> properties;
    PropertyItem* delay = nullptr;
    if (gif) {
        const auto size = image.GetPropertyItemSize(PropertyTagFrameDelay);
        if (size && size < 1024 * 1024) {
            properties.resize(size);
            auto item = reinterpret_cast<PropertyItem*>(properties.data());
            if (image.GetPropertyItem(PropertyTagFrameDelay, size, item) == Ok &&
                item->type == PropertyTagTypeLong && item->value) delay = item;
        }
    }
    auto frameDelay = [&](UINT index) {
        uint32_t centiseconds = 10;
        if (delay && index < delay->length / sizeof(centiseconds)) {
            memcpy(&centiseconds, static_cast<BYTE const*>(delay->value) + index * sizeof(centiseconds), sizeof(centiseconds));
            centiseconds = std::clamp(centiseconds, 2u, 65535u);
        }
        return centiseconds * 10;
    };
    uint64_t totalTime = 0;
    for (UINT index = 0; index < count; ++index) {
        checkStop(stopping);
        totalTime += frameDelay(index);
    }
    const auto expectedFrames = count == 1 ? 1 : (totalTime * signInPack::fps + 999) / 1000;
    animation.setSize(image.GetWidth(), image.GetHeight(), expectedFrames);
    auto const& target = animation.header;
    Pixels frame(target.frameBytes());
    Bitmap canvas(target.width, target.height, target.width * 4, PixelFormat32bppARGB, frame.data());
    Graphics graphics(&canvas);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBilinear);
    uint64_t elapsed = 0;
    for (UINT index = 0; index < count; ++index) {
        checkStop(stopping);
        if (count > 1 && image.SelectActiveFrame(&FrameDimensionTime, index) != Ok) winrt::throw_hresult(E_FAIL);
        graphics.Clear(Color::Black);
        if (graphics.DrawImage(&image, 0, 0, static_cast<INT>(target.width),
                               static_cast<INT>(target.height)) != Ok) winrt::throw_hresult(E_FAIL);
        graphics.Flush(FlushIntentionSync);
        elapsed += frameDelay(index);
        while ((count == 1 && target.count == 0) ||
               (count > 1 && static_cast<uint64_t>(target.count) * 1000 / signInPack::fps < elapsed)) {
            checkStop(stopping);
            animation.append(frame);
        }
    }
    return animation;
}

} // namespace signInEncode

// The signed-in user's Explorer prepares a bounded, machine-wide pixel cache.
namespace signInCache {
using mediaPicker::Handle;
using mediaPicker::LocalMemory;
using mediaPicker::checkWin32;

struct CacheHeader {
    signInPack::Header image;
    uint64_t selection;
    uint64_t revision;
};
static_assert(sizeof(CacheHeader) == 32);


thread_local uint64_t preparationRevision;
bool cancelPreparation() {
    return mediaPicker::stopping() || mediaPicker::settings.load()->revision != preparationRevision;
}

uint64_t selectionKey(std::wstring const& source) {
    if (source.empty()) return 0;
    // Match the current setting to its cached frames, without storing the path.
    uint64_t hash = 14695981039346656037ull;
    for (wchar_t ch : source) hash = (hash ^ static_cast<uint16_t>(ch)) * 1099511628211ull;
    return hash ? hash : 1;
}

std::wstring folderPath() {
    return mediaPicker::storageRoot + L"\\sign-in";
}

bool matchesPath(HANDLE handle, std::wstring const& path) {
    wchar_t finalPath[MAX_PATH]{};
    const DWORD count = GetFinalPathNameByHandleW(handle, finalPath, ARRAYSIZE(finalPath), FILE_NAME_NORMALIZED);
    return count && count < ARRAYSIZE(finalPath) && _wcsicmp(finalPath, (L"\\\\?\\" + path).c_str()) == 0;
}

HANDLE openFolder(std::wstring const& path) {
    Handle folder(CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES | READ_CONTROL,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr));
    if (!folder) return INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION info{};
    if (!GetFileInformationByHandle(folder.value, &info) || !(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) || !matchesPath(folder.value, path)) {
        SetLastError(ERROR_ACCESS_DENIED);
        return INVALID_HANDLE_VALUE;
    }
    return std::exchange(folder.value, INVALID_HANDLE_VALUE);
}

std::wstring filePath(std::wstring const& folder) { return folder + L"\\sign-in.qframes"; }

bool read(std::wstring const& folder, uint64_t key, signInPack::Header* image = nullptr,
          HANDLE* framesFile = nullptr
          , uint64_t revision = 0
) {
    const auto path = filePath(folder);
    Handle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    BY_HANDLE_FILE_INFORMATION info{};
    CacheHeader header{};
    DWORD count = 0;
    if (!file || GetFileType(file.value) != FILE_TYPE_DISK ||
        !GetFileInformationByHandle(file.value, &info) || info.nNumberOfLinks != 1 ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DIRECTORY)) ||
        !matchesPath(file.value, path) || !ReadFile(file.value, &header, sizeof(header), &count, nullptr) ||
        count != sizeof(header) || !header.image.valid() || header.selection != key) return false;
    if (revision && header.revision != revision) return false;
    const DWORD size = header.image.frameBytes() * header.image.count;
    if (info.nFileSizeHigh || info.nFileSizeLow != sizeof(header) + size) return false;
    if (image && framesFile) {
        *image = header.image;
        *framesFile = std::exchange(file.value, INVALID_HANDLE_VALUE);
    }
    return true;
}

void prepare(std::wstring const& source, std::wstring const& imported
    , uint64_t settingsRevision
) {
    preparationRevision = settingsRevision;
    signInEncode::checkStop(cancelPreparation);
    mediaPicker::CacheLock lock;
    const auto folder = folderPath();
    // Limit the cache to the Windows account that created it, SYSTEM and administrators.
    const auto sddl = L"O:" + mediaPicker::userSid + L"D:P(A;OICI;FA;;;" + mediaPicker::userSid +
        L")(A;OICI;FA;;;BA)(A;CI;0x1200a9;;;SY)(A;OIIO;FR;;;SY)";
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    checkWin32(ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(), SDDL_REVISION_1, &descriptor, nullptr));
    LocalMemory freeDescriptor{reinterpret_cast<HLOCAL>(descriptor)};
    SECURITY_ATTRIBUTES attributes{sizeof(attributes), descriptor, FALSE};
    if (!CreateDirectoryW(folder.c_str(), &attributes) && GetLastError() != ERROR_ALREADY_EXISTS)
        winrt::throw_last_error();
    Handle directory(openFolder(folder));
    if (!directory) winrt::throw_hresult(E_ACCESSDENIED);
    PSID owner = nullptr;
    PSECURITY_DESCRIPTOR existing = nullptr;
    const DWORD error = GetSecurityInfo(directory.value, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
        &owner, nullptr, nullptr, nullptr, &existing);
    LocalMemory freeExisting{reinterpret_cast<HLOCAL>(existing)};
    PSID expected = nullptr;
    checkWin32(ConvertStringSidToSidW(mediaPicker::userSid.c_str(), &expected));
    LocalMemory freeExpected{reinterpret_cast<HLOCAL>(expected)};
    if (error || !owner || !EqualSid(owner, expected)) winrt::throw_hresult(E_ACCESSDENIED);
    const auto key = selectionKey(source);
    WIN32_FIND_DATAW data{};
    HANDLE search = FindFirstFileW((folder + L"\\{*.tmp").c_str(), &data);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            const std::wstring name(data.cFileName);
            GUID id{};
            if (!(data.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) &&
                name.size() == 42 && SUCCEEDED(CLSIDFromString(name.substr(0, 38).c_str(), &id)))
                DeleteFileW((folder + L"\\" + name).c_str());
        } while (FindNextFileW(search, &data));
        FindClose(search);
    }
    const auto revision = selectionKey(imported);
    if (read(folder, key, nullptr, nullptr, revision)) return;
    Wh_Log(L"Preparing sign-in wallpaper from the selected media");

    winrt::init_apartment();
    struct Apartment { ~Apartment() { winrt::uninit_apartment(); } } apartment;
    const auto ext = mediaPicker::extension(imported);

    GUID id{};
    winrt::check_hresult(CoCreateGuid(&id));
    wchar_t name[40]{};
    if (!StringFromGUID2(id, name, ARRAYSIZE(name))) winrt::throw_hresult(E_FAIL);
    const auto temporary = folder + L"\\" + name + L".tmp";
    CacheHeader header{};
    const auto fileSddl = L"O:" + mediaPicker::userSid + L"D:P(A;;FA;;;" + mediaPicker::userSid +
        L")(A;;FA;;;BA)(A;;FR;;;SY)";
    PSECURITY_DESCRIPTOR fileDescriptor = nullptr;
    checkWin32(ConvertStringSecurityDescriptorToSecurityDescriptorW(fileSddl.c_str(), SDDL_REVISION_1,
        &fileDescriptor, nullptr));
    LocalMemory freeFileDescriptor{reinterpret_cast<HLOCAL>(fileDescriptor)};
    SECURITY_ATTRIBUTES fileAttributes{sizeof(fileAttributes), fileDescriptor, FALSE};
    bool created = false;
    try {
        {
            Handle file(CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, &fileAttributes, CREATE_NEW,
                FILE_ATTRIBUTE_NORMAL, nullptr));
            checkWin32(static_cast<bool>(file));
            created = true;
            const auto write = [&](void const* data, DWORD size) {
                DWORD written = 0;
                checkWin32(WriteFile(file.value, data, size, &written, nullptr));
                if (written != size) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_WRITE_FAULT));
            };
            write(&header, sizeof(header));
            const auto writeFrame = [&](signInEncode::Pixels const& pixels) {
                write(pixels.data(), static_cast<DWORD>(pixels.size()));
            };
            auto stopping = cancelPreparation;
            auto animation = ext == L".mp4" ? signInEncode::video(imported.c_str(), stopping, writeFrame)
                : signInEncode::picture(imported.c_str(), ext == L".gif", stopping, writeFrame);
            if (!animation.header.valid()) winrt::throw_hresult(E_FAIL);
            signInEncode::checkStop(stopping);
            header = {animation.header, key, revision};
            LARGE_INTEGER start{};
            checkWin32(SetFilePointerEx(file.value, start, nullptr, FILE_BEGIN));
            write(&header, sizeof(header));
            checkWin32(FlushFileBuffers(file.value));
        }
        signInEncode::checkStop(cancelPreparation);
        checkWin32(MoveFileExW(temporary.c_str(), filePath(folder).c_str(), MOVEFILE_REPLACE_EXISTING));
    } catch (...) {
        if (created) DeleteFileW(temporary.c_str());
        throw;
    }
    Wh_Log(L"Sign-in wallpaper ready (%u frames, %ux%u)", header.image.count, header.image.width, header.image.height);
}
} // namespace signInCache

namespace mediaPicker {

DWORD WINAPI run(void*) {
    // Use the desktop Explorer instance for this user's media cache.
    while (!stopping()) {
        if (HWND shell = GetShellWindow()) {
            DWORD pid = 0;
            GetWindowThreadProcessId(shell, &pid);
            if (pid != GetCurrentProcessId()) return 0;
            break;
        }
        WaitForSingleObject(stopEvent, 250);
    }
    const HANDLE events[]{stopEvent, settingsEvent};
    do {
        const auto config = settings.load();
        try {
            if (config->enabled || config->source.empty()) {
                applySource(config->source);
                prune(cacheFolder(), L"", config->source);
                Wh_Log(L"Lock screen wallpaper selection processed");
            }
            if (!stopping() && config->enabled && config->signIn && !config->source.empty()) {
                auto imported = chosenPath(config->source);
                if (!imported.empty()) {
                    const bool lowered = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
                    struct RestorePriority {
                        bool active;
                        ~RestorePriority() { if (active) SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END); }
                    } restorePriority{lowered};
                    signInCache::prepare(config->source, imported, config->revision);
                }
            }
        } catch (winrt::hresult_error const& error) {
            if (!stopping() && error.code() != HRESULT_FROM_WIN32(ERROR_CANCELLED))
                Wh_Log(L"Could not prepare wallpaper (0x%08X)", static_cast<unsigned>(error.code().value));
        } catch (...) { if (!stopping()) Wh_Log(L"Could not prepare wallpaper"); }
    } while (WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE) == WAIT_OBJECT_0 + 1);

    return 0;
}
} // namespace mediaPicker

// All XAML state belongs to the LockApp UI thread.
[[clang::no_destroy]] c::Grid mediaRoot{nullptr};
[[clang::no_destroy]] x::UIElement mediaLayer{nullptr};
[[clang::no_destroy]] x::UIElement placementContent{nullptr};
[[clang::no_destroy]] c::MediaElement player{nullptr};
[[clang::no_destroy]] std::wstring loadedPath;
[[clang::no_destroy]] x::UIElement pathContent{nullptr};
[[clang::no_destroy]] std::wstring visiblePath;
ULONGLONG nextPathCheck = 0;
[[clang::no_destroy]] std::wstring visibleSource;
ULONGLONG nextPlacementLog = 0;
bool loadedFit = false, attempted = false, videoReady = false;
ULONGLONG videoDeadline = 0;
ULONGLONG nextMediaAttempt = 0, resumeDeadline = 0;
unsigned mediaFailures = 0;

bool initializeStorage(bool verifyAncestors) {
    if (!mediaPicker::storageRoot.empty()) return true;
    wchar_t path[32768]{};
    if (!Wh_GetModStoragePath(path, ARRAYSIZE(path))) return false;
    // LockApp only has read access to its media leaf. It need not be able to
    // query the attributes of every directory above the Windhawk storage path.
    if (verifyAncestors && !mediaPicker::plainLocalPath(path)) return false;
    mediaPicker::storageRoot = path;
    return true;
}

void clearMedia() {
    try { if (player) player.Stop(); } catch (...) {}
    try {
        if (mediaRoot && mediaLayer) {
            auto children = mediaRoot.Children();
            uint32_t index = 0;
            if (children.IndexOf(mediaLayer, index)) children.RemoveAt(index);
        }
    } catch (...) {}
    player = nullptr;
    mediaLayer = nullptr;
    mediaRoot = nullptr;
    videoReady = false;
    videoDeadline = 0;
    resumeDeadline = 0;
}

void scheduleMediaRetry() {
    clearMedia();
    ++mediaFailures;
    attempted = mediaFailures >= 4;
    if (!attempted) {
        nextMediaAttempt = GetTickCount64() + (mediaFailures == 1 ? 1000 : 4000);
        Wh_Log(L"Retrying LockApp wallpaper (failure %u of 4)", mediaFailures);
    }
}

struct HiddenOverlay {
    winrt::weak_ref<x::FrameworkElement> element;
    winrt::Windows::Foundation::IInspectable localValue;
    x::Data::Binding binding;
};

[[clang::no_destroy]] std::vector<HiddenOverlay> hiddenOverlays;
[[clang::no_destroy]] winrt::weak_ref<x::UIElement> overlayContent;
ULONGLONG nextOverlayScan = 0;

void restoreOverlays() {
    const auto property = x::UIElement::VisibilityProperty();
    for (const auto& hidden : hiddenOverlays) {
        try {
            auto element = hidden.element.get();
            if (!element || element.Visibility() != x::Visibility::Collapsed ||
                element.GetBindingExpression(property)) {
                continue;
            }
            if (hidden.binding) {
                element.SetBinding(property, hidden.binding);
            } else if (hidden.localValue == x::DependencyProperty::UnsetValue()) {
                element.ClearValue(property);
            } else {
                element.SetValue(property, hidden.localValue);
            }
        } catch (...) {}
    }
    hiddenOverlays.clear();
    overlayContent = {};
    nextOverlayScan = 0;
}

void scanOverlays(x::DependencyObject const& node, int depth, int& remaining) {
    if (!node || depth > 48 || remaining <= 0) return;
    --remaining;
    if (auto element = node.try_as<x::FrameworkElement>()) {
        static constexpr std::wstring_view names[] = {
            L"TimeAndDatePanel", L"HotspotContainer", L"StatusProviderPanel",
            L"NetworkIcon", L"NetworkIconV2", L"BatteryStatusProvider", L"NewBatteryIcon",
            L"WidgetCanvasPanel", L"WidgetsContainer", L"WidgetGroupPanel",
            L"LockWidgetCanvas", L"AdaptiveCardDetailContainer",
            L"MediaControlsContainer", L"MediaTransportControls",
            L"AlarmsIconPanel", L"AlarmsIconPanelPadding",
        };
        const auto name = element.Name();
        const bool statusBackplate = name == L"BackplateBorder" && element.try_as<c::Border>();
        if (statusBackplate ||
            std::find(std::begin(names), std::end(names), std::wstring_view(name)) != std::end(names)) {
            if (element.Visibility() != x::Visibility::Collapsed) {
                const auto property = x::UIElement::VisibilityProperty();
                const auto expression = element.GetBindingExpression(property);
                HiddenOverlay original{
                    winrt::make_weak(element),
                    element.ReadLocalValue(property),
                    expression ? expression.ParentBinding() : nullptr,
                };
                const auto saved = std::find_if(hiddenOverlays.begin(), hiddenOverlays.end(),
                    [&](const HiddenOverlay& entry) { return entry.element.get() == element; });
                if (saved == hiddenOverlays.end()) {
                    hiddenOverlays.push_back(std::move(original));
                } else {
                    *saved = std::move(original);
                }
                element.Visibility(x::Visibility::Collapsed);
            }
            return;
        }
    }
    const int count = m::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count && remaining > 0; ++i) {
        scanOverlays(m::VisualTreeHelper::GetChild(node, i), depth + 1, remaining);
    }
}

void syncOverlays(x::UIElement const& content, bool enabled) {
    if (!enabled || content != overlayContent.get()) {
        if (nextOverlayScan) restoreOverlays();
        if (!enabled) return;
        overlayContent = winrt::make_weak(content);
    }
    const auto now = GetTickCount64();
    if (now < nextOverlayScan) return;
    nextOverlayScan = now + 1000;
    std::erase_if(hiddenOverlays, [](const HiddenOverlay& entry) { return !entry.element.get(); });
    const auto previousCount = hiddenOverlays.size();
    int remaining = 1500;
    scanOverlays(content, 0, remaining);
    if (hiddenOverlays.size() > previousCount) {
        Wh_Log(L"Lock screen overlays hidden (%zu elements)", hiddenOverlays.size());
    }
}

void resetMediaUi() {
    clearMedia();
    placementContent = nullptr;
    loadedPath.clear();
    attempted = false;
    nextMediaAttempt = 0;
    mediaFailures = 0;
    pathContent = nullptr;
    visiblePath.clear();
    nextPathCheck = 0;
    visibleSource.clear();
    nextPlacementLog = 0;
}

void resetUi() {
    if (nextOverlayScan) restoreOverlays();
    resetMediaUi();
}

struct Placement {
    c::Grid root{nullptr};
    c::Grid fallback{nullptr};
    x::FrameworkElement clockAnchor{nullptr};
};

void scanPlacement(x::DependencyObject const& node, Placement& placement, int depth) {
    if (!node || depth > 72 || (placement.root && placement.clockAnchor)) return;
    if (auto element = node.try_as<x::FrameworkElement>()) {
        auto name = element.Name();
        if (name == L"LockRootGrid" && !placement.root) placement.root = element.try_as<c::Grid>();
        else if (name == L"MainPageContainer" && !placement.fallback) placement.fallback = element.try_as<c::Grid>();
        else if (name == L"TimeAndDatePanel" && !placement.clockAnchor) placement.clockAnchor = element;
    }
    int count = m::VisualTreeHelper::GetChildrenCount(node);
    for (int index = 0; index < count; ++index)
        scanPlacement(m::VisualTreeHelper::GetChild(node, index), placement, depth + 1);
}

bool insertMedia(c::Grid const& root, x::FrameworkElement const& clockAnchor,
                 x::FrameworkElement const& media) {
    if (!root || !clockAnchor) return false;
    x::DependencyObject child = clockAnchor;
    while (child) {
        auto parent = m::VisualTreeHelper::GetParent(child);
        if (parent == root) break;
        child = parent;
    }
    if (!child) return false;
    auto layerAnchor = child.try_as<x::UIElement>();
    auto children = root.Children();
    uint32_t index = 0;
    if (!layerAnchor || !children.IndexOf(layerAnchor, index)) return false;
    media.IsHitTestVisible(false);
    media.HorizontalAlignment(x::HorizontalAlignment::Stretch);
    media.VerticalAlignment(x::VerticalAlignment::Stretch);
    c::Grid::SetRowSpan(media, std::max(1u, root.RowDefinitions().Size()));
    c::Grid::SetColumnSpan(media, std::max(1u, root.ColumnDefinitions().Size()));
    children.InsertAt(index, media);
    mediaLayer = media;
    mediaRoot = root;
    return true;
}

void loadMedia(Placement const& place, std::wstring const& path, bool fit
) {
    auto ext = mediaPicker::extension(path);
    const auto limit = mediaPicker::fileLimit(ext);
    WIN32_FILE_ATTRIBUTE_DATA info{};
    if (!limit || !GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ||
        info.nFileSizeHigh || !info.nFileSizeLow || info.nFileSizeLow >= limit)
        winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));

    winrt::Windows::Storage::Streams::IRandomAccessStream stream{nullptr};
    winrt::check_hresult(CreateRandomAccessStreamOnFile(path.c_str(), 0,
        winrt::guid_of<winrt::Windows::Storage::Streams::IRandomAccessStream>(), winrt::put_abi(stream)));
    auto stretch = fit ? m::Stretch::Uniform : m::Stretch::UniformToFill;
    auto insert = [&](x::FrameworkElement const& media) {
        if (insertMedia(place.root, place.clockAnchor, media)) return;
        if (!insertMedia(place.fallback, place.clockAnchor, media))
            winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
    };
    if (ext == L".mp4") {
        c::MediaElement video;
        video.IsMuted(true);
        video.AreTransportControlsEnabled(false);
        video.IsLooping(true);
        video.AutoPlay(true);
        video.Stretch(stretch);
        // Keep the stock wallpaper visible until Windows starts playback.
        video.Opacity(0);
        video.SetSource(stream, L"video/mp4");
        insert(video);
        player = video;
        videoDeadline = GetTickCount64() + 12000;
        player.Play();
        return;
    }
    imaging::BitmapImage bitmap;
    bitmap.AutoPlay(true);
    bitmap.SetSource(stream);
    c::Image image;
    image.Stretch(stretch);
    image.Source(bitmap);
    insert(image);
    Wh_Log(L"Image background applied");
}

void refreshUi() {
    auto window = x::Window::Current();
    auto content = window ? window.Content() : nullptr;
    const auto config = mediaPicker::settings.load();
    if (!content) {
        resetUi();
        return;
    }
    syncOverlays(content, config->hideOverlays);
    if (!config->enabled || config->source.empty()) {
        resetMediaUi();
        return;
    }
    if (content != pathContent) {
        pathContent = content;
        visiblePath.clear();
        nextPathCheck = 0;
    }
    if (visibleSource != config->source) {
        visibleSource = config->source;
        visiblePath.clear();
        nextPathCheck = 0;
    }
    const auto now = GetTickCount64();
    if (visiblePath.empty() && now >= nextPathCheck) {
        if (!initializeStorage(false)) {
            nextPathCheck = now + 3000;
            Wh_Log(L"Waiting for Windhawk's LockApp storage path");
            return;
        }
        visiblePath = mediaPicker::chosenPath(
            config->source
        );
        nextPathCheck = now + 500;
        if (visiblePath.empty() && now >= nextPlacementLog) {
            Wh_Log(L"Waiting for the selected LockApp wallpaper copy");
            nextPlacementLog = now + 5000;
        }
    }
    auto const& path = visiblePath;
    if (path.empty()) {
        return;
    }
    const bool fit = config->fit;
    if (mediaLayer && path == loadedPath && content == placementContent && fit != loadedFit) {
        const auto stretch = fit ? m::Stretch::Uniform : m::Stretch::UniformToFill;
        if (player) player.Stretch(stretch);
        else if (auto image = mediaLayer.try_as<c::Image>()) image.Stretch(stretch);
        loadedFit = fit;
    }
    if (content != placementContent || path != loadedPath || fit != loadedFit ||
        (mediaLayer && m::VisualTreeHelper::GetParent(mediaLayer) != mediaRoot)) {
        clearMedia();
        placementContent = content;
        loadedPath = path;
        loadedFit = fit;
        attempted = false;
        nextMediaAttempt = 0;
        mediaFailures = 0;
    }
    if (!attempted
        && now >= nextMediaAttempt
    ) {
        Placement placement;
        scanPlacement(content, placement, 0);
        if ((!placement.root && !placement.fallback) || !placement.clockAnchor) {
            if (now >= nextPlacementLog) {
                Wh_Log(L"Waiting for LockApp wallpaper layer (root=%d, clock=%d)",
                    static_cast<bool>(placement.root || placement.fallback), static_cast<bool>(placement.clockAnchor));
                nextPlacementLog = now + 5000;
            }
            return;
        }
        attempted = true;
        try { loadMedia(placement, path, fit); }
        catch (winrt::hresult_error const& error) {
            scheduleMediaRetry();
            Wh_Log(L"Could not load the background (0x%08X)", static_cast<unsigned>(error.code().value));
        } catch (...) {
            scheduleMediaRetry();
            Wh_Log(L"Could not load the background");
        }
    }
    if (!player) return;
    auto state = player.CurrentState();
    if (!videoReady && state == m::MediaElementState::Playing && player.NaturalVideoWidth() > 0) {
        player.Opacity(1);
        videoReady = true;
        mediaFailures = 0;
        Wh_Log(L"MP4 playing at %ux%u", player.NaturalVideoWidth(), player.NaturalVideoHeight());
    } else if ((!videoReady && GetTickCount64() >= videoDeadline) ||
               (videoReady && state == m::MediaElementState::Closed)) {
        Wh_Log(L"MP4 playback unavailable, state=%d; using the Windows background", static_cast<int>(state));
        scheduleMediaRetry();
    } else if (videoReady && (state == m::MediaElementState::Paused || state == m::MediaElementState::Stopped)) {
        if (!resumeDeadline) {
            resumeDeadline = now + 2000;
            player.Play();
        } else if (now >= resumeDeadline) {
            Wh_Log(L"LockApp video did not resume; reopening it");
            scheduleMediaRetry();
        }
    } else {
        resumeDeadline = 0;
    }
}

using UiProc = void(*)();
UINT runMessage = 0;
bool signInProcess = false;
std::atomic<HWND> dispatchWindow{nullptr};
std::atomic<HWND> lastWindow{nullptr};
std::atomic<UiProc> dispatchProc{nullptr};
std::atomic<bool> dispatched{false};

LRESULT CALLBACK callHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        auto message = reinterpret_cast<CWPSTRUCT const*>(lp);
        if (message && message->message == runMessage && message->hwnd == dispatchWindow.load()) {
            if (auto proc = dispatchProc.exchange(nullptr)) {
                try { proc(); dispatched = true; }
                catch (winrt::hresult_error const& error) {
                    Wh_Log(L"LockApp update failed (0x%08X)", static_cast<unsigned>(error.code().value));
                } catch (...) { Wh_Log(L"LockApp update failed"); }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

bool dispatch(HWND window, UiProc proc, bool requireResponse = false) {
    DWORD pid = 0;
    DWORD thread = GetWindowThreadProcessId(window, &pid);
    if (!thread || pid != GetCurrentProcessId()) return false;
    if (thread == GetCurrentThreadId()) { proc(); return true; }
    DWORD_PTR result = 0;
    if (!requireResponse && !SendMessageTimeoutW(window, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 300, &result))
        return false;
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, callHook, nullptr, thread);
    if (!hook) return false;
    dispatched = false;
    dispatchWindow = window;
    dispatchProc = proc;
    // This dispatch is synchronous so the temporary hook returns before unload.
    SendMessageW(window, runMessage, 0, 0);
    dispatchProc = nullptr;
    dispatchWindow = nullptr;
    UnhookWindowsHookEx(hook);
    return dispatched.load();
}

BOOL CALLBACK findWindow(HWND window, LPARAM value) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    wchar_t name[128]{};
    PCWSTR target = L"Windows.UI.Core.CoreWindow";
    if (signInProcess) target = L"LogonUI Logon Window";
    if (!signInProcess && !IsWindowVisible(window)) return TRUE;
    if (pid == GetCurrentProcessId() && GetClassNameW(window, name, ARRAYSIZE(name)) &&
        wcscmp(name, target) == 0) {
        *reinterpret_cast<HWND*>(value) = window;
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI watchLockScreen(void*) {
    bool wasVisible = false;
    while (WaitForSingleObject(stopEvent, 500) == WAIT_TIMEOUT) {
        HWND window = nullptr;
        EnumWindows(findWindow, reinterpret_cast<LPARAM>(&window));
        if (!window) {
            if (wasVisible) {
                auto previous = lastWindow.load();
                if (!IsWindow(previous) || dispatch(previous, resetUi)) {
                    wasVisible = false;
                }
            }
            continue;
        }
        if (!IsWindowVisible(window)) {
            if (wasVisible && dispatch(window, resetUi)) wasVisible = false;
            continue;
        }
        const auto config = mediaPicker::settings.load();
        if ((!config->enabled || config->source.empty()) && !config->hideOverlays) {
            if (wasVisible && dispatch(window, resetUi)) wasVisible = false;
            continue;
        }
        lastWindow = window;
        wasVisible = true;
        dispatch(window, refreshUi);
    }
    return 0;
}

// Optional sign-in player. LogonUI reads bounded raw pixels.
namespace signIn {
using signInPack::Header;

struct BufferBytes : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
};
constexpr GUID bufferBytesId{0x905a0fef, 0xbc53, 0x11df, {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};

uint64_t frameSelection = 0, paintedSelection = 0;
constexpr wchar_t lastStartKey[] = L"SignInLastStartTick.v1";
Header header{};
std::vector<BYTE> frames;
[[clang::no_destroy]] imaging::WriteableBitmap bitmap{nullptr};
std::vector<BYTE> openingPixels;
[[clang::no_destroy]] imaging::WriteableBitmap openingBitmap{nullptr};
std::atomic<bool> animationFrameReady{false};
[[clang::no_destroy]] imaging::WriteableBitmap buffers[]{nullptr, nullptr, nullptr};
unsigned bufferIndex = ARRAYSIZE(buffers) - 1;

bool isPlaybackImage(m::ImageSource const& source) {
    if (!source) return false;
    if (source == openingBitmap) return true;
    for (auto const& buffer : buffers) {
        if (source == buffer) return true;
    }
    return false;
}
ULONGLONG nextScan = 0;
std::atomic<bool> allowPlayback{false};
ULONGLONG credentialsVisibleSince = 0;
uint32_t shownFrame = UINT32_MAX;
std::atomic<uint32_t> requestedFrame{0};
SRWLOCK frameLock = SRWLOCK_INIT;

struct BackgroundImage {
    c::Image image{nullptr};
    m::ImageSource original{nullptr};
    m::Stretch stretch{};

    void restore() {
        if (image &&
            isPlaybackImage(image.Source())
        ) {
            image.Stretch(stretch);
            image.Source(original);
        }
        image = nullptr;
        original = nullptr;
    }

    void update(c::Image const& candidate, m::Stretch fit) {
        if (candidate != image) restore();
        if (!candidate) return;
        auto source = candidate.Source();
        if (source != bitmap) {
            if (!isPlaybackImage(source)) {
                original = source;
                stretch = candidate.Stretch();
                Wh_Log(L"Animated sign-in background attached");
            }
            image = candidate;
            image.Source(bitmap);
        }
        if (image && image.Stretch() != fit) image.Stretch(fit);
    }
};
[[clang::no_destroy]] BackgroundImage firstImage, secondImage;

bool enabled() {
    const auto config = mediaPicker::settings.load();
    return config->enabled && config->signIn && !config->source.empty();
}

bool loadPack(uint64_t key
    , HANDLE& framesFile
) {
    const auto path = signInCache::folderPath();
    mediaPicker::Handle directory(signInCache::openFolder(path));
    return directory && signInCache::read(path, key, &header, &framesFile);
}

void clear() {
    try { firstImage.restore(); } catch (...) { firstImage = {}; }
    try { secondImage.restore(); } catch (...) { secondImage = {}; }
    bitmap = nullptr;
    openingBitmap = nullptr;
    animationFrameReady = false;
    for (auto& buffer : buffers) buffer = nullptr;
    bufferIndex = ARRAYSIZE(buffers) - 1;
    shownFrame = UINT32_MAX;
    nextScan = 0;
    allowPlayback = false;
    credentialsVisibleSince = 0;
}

struct Scene {
    c::Image first{nullptr}, second{nullptr};
    bool status = false;
    bool credentialsVisible = false, lockVisible = false;
};

bool visibleInScene(x::FrameworkElement const& element) {
    if (element.ActualWidth() <= 0 || element.ActualHeight() <= 0) return false;
    x::DependencyObject node = element;
    for (int depth = 0; node && depth < 64; ++depth) {
        if (auto view = node.try_as<x::UIElement>(); view &&
            (view.Visibility() != x::Visibility::Visible || view.Opacity() <= 0)) return false;
        node = m::VisualTreeHelper::GetParent(node);
    }
    return true;
}

void findImages(x::DependencyObject const& node, Scene& scene, int depth, int& budget) {
    if (!node || depth > 64 || budget <= 0 || scene.status) return;
    --budget;
    if (auto element = node.try_as<x::FrameworkElement>()) {
        auto name = element.Name();
        if (name == L"LockLogonImage") scene.first = element.try_as<c::Image>();
        else if (name == L"SwapLockLogonImage") scene.second = element.try_as<c::Image>();
        else if (name == L"RequestCredentialUserControl" || name == L"LockScreenUserControl") {
            const bool visible = visibleInScene(element);
            if (name == L"RequestCredentialUserControl") scene.credentialsVisible |= visible;
            else scene.lockVisible |= visible;
        }
        else if (name == L"DisplayMessageUserControl" && element.Visibility() == x::Visibility::Visible &&
                 element.ActualWidth() > 0) scene.status = true;
    }
    const int count = m::VisualTreeHelper::GetChildrenCount(node);
    for (int index = 0; index < count && budget > 0 && !scene.status; ++index)
        findImages(m::VisualTreeHelper::GetChild(node, index), scene, depth + 1, budget);
}

bool failed = false;

BYTE* bitmapBytes(winrt::Windows::Storage::Streams::IBuffer const& buffer) {
    if (buffer.Capacity() < header.frameBytes()) winrt::throw_hresult(E_FAIL);
    winrt::com_ptr<BufferBytes> access;
    winrt::check_hresult(winrt::get_unknown(buffer)->QueryInterface(bufferBytesId, access.put_void()));
    BYTE* data = nullptr;
    winrt::check_hresult(access->Buffer(&data));
    if (!data) winrt::throw_hresult(E_FAIL);
    return data;
}

void showOpeningFrame(Scene const& scene) {
    if (!openingBitmap) {
        openingBitmap = imaging::WriteableBitmap(header.width, header.height);
        auto buffer = openingBitmap.PixelBuffer();
        memcpy(bitmapBytes(buffer), openingPixels.data(), header.frameBytes());
        openingBitmap.Invalidate();
        Wh_Log(L"Showing the opening sign-in still frame");
    }
    bitmap = openingBitmap;
    shownFrame = 0;
    const auto fit = mediaPicker::settings.load()->fit ? m::Stretch::Uniform : m::Stretch::UniformToFill;
    firstImage.update(scene.first, fit);
    secondImage.update(scene.second, fit);
}

void paint() {
    try {
        if (failed || !enabled()) { clear(); return; }
        if (paintedSelection != frameSelection) {
            clear();
            paintedSelection = frameSelection;
        }
        if (!frameSelection) return;
        const auto now = GetTickCount64();
        Scene scene;
        const bool scanNow = now >= nextScan;
        if (scanNow) {
            int budget = 1600;
            auto window = x::Window::Current();
            if (window) findImages(window.Content(), scene, 0, budget);
            if (scene.status || scene.lockVisible || !scene.credentialsVisible || (!scene.first && !scene.second)) {
                if (bitmap) clear();
                credentialsVisibleSince = 0;
                nextScan = now + 200;
                return;
            }
            if (!credentialsVisibleSince) credentialsVisibleSince = now;
            if (now - credentialsVisibleSince < 350) {
                nextScan = now + 175;
                showOpeningFrame(scene);
                return;
            }
            if (!allowPlayback.exchange(true)) Wh_Log(L"Sign-in screen settled; starting animated wallpaper");
            nextScan = now + 500;
        }
        if (!allowPlayback
            || !animationFrameReady
        ) return;
        const uint32_t frame = requestedFrame.load();
        if (frame != shownFrame) {
            // Publish a complete bitmap and keep the previous two available to XAML.
            const auto nextIndex = (bufferIndex + 1) % ARRAYSIZE(buffers);
            auto& next = buffers[nextIndex];
            if (!next) next = imaging::WriteableBitmap(header.width, header.height);
            auto buffer = next.PixelBuffer();
            auto data = bitmapBytes(buffer);
            if (!TryAcquireSRWLockShared(&frameLock)) return;
            const auto ready = requestedFrame.load();
            memcpy(data, frames.data(), header.frameBytes());
            ReleaseSRWLockShared(&frameLock);
            shownFrame = ready;
            next.Invalidate();
            bitmap = next;
            bufferIndex = nextIndex;
        }
        auto fit =
            mediaPicker::settings.load()->fit
            ? m::Stretch::Uniform : m::Stretch::UniformToFill;
        if (
            !scanNow
        ) {
            firstImage.update(firstImage.image, fit);
            secondImage.update(secondImage.image, fit);
            return;
        }
        firstImage.update(scene.status ? nullptr : scene.first, fit);
        secondImage.update(scene.status ? nullptr : scene.second, fit);
    } catch (...) {
        clear();
        failed = true;
        Wh_Log(L"Sign-in background failed; using Windows wallpaper");
    }
}

bool restartBackoff() {
    const DWORD tick = GetTickCount();
    const DWORD previous = static_cast<DWORD>(Wh_GetIntValue(lastStartKey, 0));
    if ((previous && tick - previous < 15000) || !Wh_SetIntValue(lastStartKey, static_cast<int>(tick ? tick : 1))) {
        Wh_Log(L"Sign-in background skipped by restart backoff");
        return true;
    }
    return false;
}

std::atomic<bool> frameQueued{false};

LRESULT CALLBACK frameHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && wp == PM_REMOVE) {
        auto message = reinterpret_cast<MSG const*>(lp);
        if (message && message->message == runMessage && message->hwnd == lastWindow.load() &&
            frameQueued.exchange(false) && !mediaPicker::stopping()) paint();
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

DWORD WINAPI watch(void*) {
    const auto key = signInCache::selectionKey(
        mediaPicker::settings.load()->source
    );
    mediaPicker::Handle frameFile(INVALID_HANDLE_VALUE);
    bool reported = false;
    while (!mediaPicker::stopping()) {
        try {
            if (loadPack(key, frameFile.value)) { frameSelection = key; break; }
        } catch (...) {}
        if (!reported) {
            Wh_Log(L"Waiting for the current sign-in wallpaper cache; using Windows wallpaper");
            reported = true;
        }
        WaitForSingleObject(stopEvent, 1000);
    }
    if (mediaPicker::stopping() || restartBackoff()) return 0;

    const auto readFrame = [&](uint32_t index, std::vector<BYTE>& destination) {
        LARGE_INTEGER offset{};
        offset.QuadPart = sizeof(signInCache::CacheHeader) + static_cast<uint64_t>(index) * header.frameBytes();
        DWORD count = 0;
        return SetFilePointerEx(frameFile.value, offset, nullptr, FILE_BEGIN) &&
            ReadFile(frameFile.value, destination.data(), header.frameBytes(), &count, nullptr) &&
            count == header.frameBytes();
    };
    std::vector<BYTE> staging;
    try {
        frames.resize(header.frameBytes());
        staging.resize(header.frameBytes());
        openingPixels.resize(header.frameBytes());
    } catch (...) { return 0; }
    // Read the first complete frame before the sign-in window starts drawing.
    if (!readFrame(0, frames)) return 0;
    memcpy(openingPixels.data(), frames.data(), header.frameBytes());
    requestedFrame = 0;
    uint32_t buffered = 0;
    HWND window = nullptr;
    while (!mediaPicker::stopping() && !window) {
        EnumWindows(findWindow, reinterpret_cast<LPARAM>(&window));
        if (!window) WaitForSingleObject(stopEvent,
            250
        );
    }
    if (!window || mediaPicker::stopping()) return 0;
    lastWindow = window;
    const DWORD thread = GetWindowThreadProcessId(window, nullptr);
    HHOOK hook = thread ? SetWindowsHookExW(WH_GETMESSAGE, frameHook, nullptr, thread) : nullptr;
    if (!hook) return 0;
    using namespace std::chrono;
    // Schedule against a fixed origin, so processing time does not shift future frames.
    auto origin = steady_clock::now();
    bool playing = false;
    const auto rate = header.count == 1 ? 4u : signInPack::fps;
    mediaPicker::Handle timer(CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_MODIFY_STATE | SYNCHRONIZE));
    HANDLE events[]{stopEvent, timer.value};
    constexpr int64_t microsecondsPerSecond = 1'000'000;
    while (!mediaPicker::stopping() && IsWindow(window)) {
        if (!allowPlayback) {
            playing = false;
            animationFrameReady = false;
            if (!frameQueued.exchange(true) && !PostMessageW(window, runMessage, 0, 0)) frameQueued = false;
            if (WaitForSingleObject(stopEvent, 175) != WAIT_TIMEOUT) break;
            continue;
        }
        if (!playing) {
            origin = steady_clock::now();
            playing = true;
        }
        const auto elapsed = duration_cast<microseconds>(steady_clock::now() - origin).count();
        const auto slot = elapsed * rate / microsecondsPerSecond;
        const auto next = static_cast<uint32_t>(slot % header.count);
        if (next != buffered) {
            if (!readFrame(next, staging)) {
                Wh_Log(L"Sign-in frame read failed; restoring Windows wallpaper");
                break;
            }
            AcquireSRWLockExclusive(&frameLock);
            frames.swap(staging);
            requestedFrame = next;
            ReleaseSRWLockExclusive(&frameLock);
            buffered = next;
        }
        animationFrameReady = true;
        if (!frameQueued.exchange(true) && !PostMessageW(window, runMessage, 0, 0)) frameQueued = false;
        const auto deadline = ((slot + 1) * microsecondsPerSecond + rate - 1) / rate;
        const auto remaining = deadline - duration_cast<microseconds>(steady_clock::now() - origin).count();
        if (remaining <= 0) continue;
        LARGE_INTEGER due{};
        due.QuadPart = -remaining * 10;
        if (timer && SetWaitableTimer(timer.value, &due, 0, nullptr, nullptr, FALSE)) {
            if (WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE) != WAIT_OBJECT_0 + 1) break;
        } else if (WaitForSingleObject(stopEvent, static_cast<DWORD>((remaining + 999) / 1000)) != WAIT_TIMEOUT) {
            break;
        }
    }

    UnhookWindowsHookEx(hook);
    if (IsWindow(window)) {
        // Wait for a hook invocation already on the UI thread before DLL unload.
        SendMessageW(window, WM_NULL, 0, 0);
        dispatch(window, clear, true);
    }
    return 0;
}
} // namespace signIn

BOOL Wh_ModInit() {
    try {
        mediaPicker::settings.store(mediaPicker::readSettings());
    } catch (...) { return FALSE; }
    wchar_t executable[32768]{};
    if (!GetModuleFileNameW(nullptr, executable, ARRAYSIZE(executable))) return FALSE;
    PCWSTR name = wcsrchr(executable, L'\\');
    name = name ? name + 1 : executable;
    const bool explorer = _wcsicmp(name, L"explorer.exe") == 0;
    auto start = explorer ? mediaPicker::run : watchLockScreen;
    signInProcess = _wcsicmp(name, L"LogonUI.exe") == 0;
    if (signInProcess) {
        if (!signIn::enabled()) return FALSE;
        start = signIn::watch;
    } else
    {
        if (!explorer && _wcsicmp(name, L"LockApp.exe") != 0) return FALSE;
        try { mediaPicker::initIdentity(); }
        catch (...) { Wh_Log(L"Could not initialize the media cache identity"); return FALSE; }
    }
    if (explorer || signInProcess) {
        try {
            if (!initializeStorage(true)) {
                Wh_Log(L"Windhawk mod storage is unavailable");
                return FALSE;
            }
        } catch (...) { return FALSE; }
    }
    runMessage = RegisterWindowMessageW(L"LockscreenMotionBackground_RunOnUi_v1");
    if (!runMessage) return FALSE;
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) return FALSE;
    if (explorer) {
        mediaPicker::settingsEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!mediaPicker::settingsEvent) {
            CloseHandle(stopEvent);
            stopEvent = nullptr;
            return FALSE;
        }
    }
    worker = CreateThread(nullptr, 0, start, nullptr, 0, nullptr);
    if (!worker) {
        if (mediaPicker::settingsEvent) CloseHandle(std::exchange(mediaPicker::settingsEvent, nullptr));
        CloseHandle(stopEvent);
        stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = FALSE;
    try {
        const auto old = mediaPicker::settings.load();
        auto next = mediaPicker::readSettings();
        if (signInProcess) {
            *reload = old->source != next->source || old->enabled != next->enabled ||
                old->signIn != next->signIn || old->fit != next->fit;
        } else {
            const bool overlayOnly = old->hideOverlays != next->hideOverlays &&
                old->source == next->source && old->enabled == next->enabled &&
                old->signIn == next->signIn && old->fit == next->fit;
            if (overlayOnly) next->revision = old->revision;
            mediaPicker::settings.store(std::move(next));
            if (overlayOnly) return TRUE;
            if (mediaPicker::settingsEvent) SetEvent(mediaPicker::settingsEvent);
            Wh_Log(L"Wallpaper settings applied without unloading LockApp or Explorer");
        }
    } catch (...) { Wh_Log(L"Could not apply wallpaper settings"); }
    return TRUE;
}

void Wh_ModUninit() {
    if (stopEvent) SetEvent(stopEvent);
    if (worker) {
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
        worker = nullptr;
    }
    if (HWND window = lastWindow.exchange(nullptr); window && IsWindow(window)) {
        auto cleanup = resetUi;
        // The sign-in worker removes its hook and restores the images before it exits.
        if (signInProcess) cleanup = nullptr;
        if (cleanup && !dispatch(window, cleanup))
            Wh_Log(L"Background window did not respond during cleanup");
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
    }
    if (mediaPicker::settingsEvent) CloseHandle(std::exchange(mediaPicker::settingsEvent, nullptr));
}
