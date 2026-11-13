// ==WindhawkMod==
// @id              doom-hotkey
// @name            Can It Run Doom?
// @description     Launch Doom from anywhere with a customizable keyboard shortcut.
// @version         0.4.0
// @author          bardelyne
// @github          https://github.com/bardelyne
// @include         windhawk.exe
// @license         GPL-2.0-or-later
// @compilerOptions -luser32 -lshell32 -lole32 -luuid -lbcrypt -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Can It Run Doom?

Press **Ctrl+Alt+Shift+D** to launch Doom fullscreen. Press it again to return to the running game.

![Freedoom Phase 1 title screen and main menu](https://raw.githubusercontent.com/bardelyne/doom-hotkey/fcb4b5729067da4cd6110db02d3d3294b8234789/assets/doom-hotkey-freedoom-menu.png)

## Play Freedoom

Enable **Download official game files**, select a Freedoom campaign, and press the shortcut. The first launch may take a moment to download about 34 MB from the official Crispy Doom and Freedoom GitHub releases. The mod checks SHA-256 hashes before using the files. Later launches reuse the verified files and work offline.

The downloaded engine requires x64 Windows or x64 emulation on Windows 11 on Arm. Setup uses the Windows `tar.exe` utility. If downloads are unavailable, set **Doom executable** to a local Doom source port that accepts `-fullscreen`. Use **Game WAD** for your own Doom or Freedoom game data, or leave it blank if your engine finds its own game files.

## Storage and uninstall

Game files are cached under `%LOCALAPPDATA%\Windhawk Doom\Runtime`. Settings and saves for the downloaded engine are in `%LOCALAPPDATA%\Windhawk Doom\Profile`. Windhawk leaves these folders when you uninstall the mod. Close the game before deleting the cache, and keep `Profile` if you want your settings and saves. Test builds before 0.4.0 may also have data in the old versioned folder under `Windhawk Doom`.

Changes apply when saved. If Windows cannot register the new shortcut, the previous shortcut stays active. The hotkey runs in a dedicated Windhawk process. Doom runs in its own process.

## Credits

- **id Software** created DOOM and released its engine source. [DOOM source release](https://github.com/id-Software/DOOM)
- **Simon Howard and the Chocolate Doom contributors** developed the source port that Crispy Doom builds on. [Chocolate Doom](https://github.com/chocolate-doom/chocolate-doom)
- **Fabian Greffrath and the Crispy Doom contributors** maintain the engine used by this mod. [Crispy Doom 7.2.0](https://github.com/fabiangreffrath/crispy-doom/releases/tag/crispy-doom-7.2)
- **The Freedoom contributors** created the campaigns, graphics, sounds, and music. [Freedoom 0.13.0](https://github.com/freedoom/freedoom/releases/tag/v0.13.0)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shortcut: Ctrl+Alt+Shift+D
  $name: Launch shortcut
  $description: A modifier plus a letter, number or function key. Example Ctrl+Alt+F9.
- downloadOfficialFiles: false
  $name: Download official game files
  $description: Allow downloads from the official Crispy Doom and Freedoom releases when the shortcut is pressed and verified files are missing.
- campaign: phase1
  $name: Freedoom campaign
  $description: Used when both Doom executable and Game WAD are blank.
  $options:
    - phase1: Freedoom Phase 1
    - phase2: Freedoom Phase 2
- enginePath: ""
  $name: Doom executable
  $description: Full local drive path. Leave blank to use the official Crispy Doom release. Environment variables such as %LOCALAPPDATA% are supported.
- iwadPath: ""
  $name: Game WAD
  $description: Full local drive path to an IWAD file. Leave blank to use the selected campaign with downloaded Crispy Doom or let a custom engine find its own game data.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <shlobj.h>
#include <winhttp.h>
#include <windhawk_utils.h>

#include <array>
#include <cwctype>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace DoomRuntime {

struct Asset {
    PCWSTR member;
    PCWSTR name;
    DWORD size;
    const char* sha256;
};

constexpr Asset kEngineFiles[] = {
    {L"SDL2.dll", L"SDL2.dll", 2384384,
     "933b128eeec721aede9e36fbdb3472c5715c36db8b17d092ad32f2be3f1af7b8"},
    {L"SDL2_mixer.dll", L"SDL2_mixer.dll", 176640,
     "871e62daf0f44f4a02ead4fca28241e115533f3ff04b08dfade6e8475af63693"},
    {L"SDL2_net.dll", L"SDL2_net.dll", 25088,
     "685408cf3205dd3a5997938d9bb97857d8b48afb221b115d05e2ec6864521bf6"},
    {L"SDL3.dll", L"SDL3.dll", 4014592,
     "5d021ede45c98a0aa9d42811bcc6a3a6d5500fa25ab0c612fe0cf75ed4d21533"},
    {L"crispy-doom.exe", L"crispy-doom.exe", 1331200,
     "3e8d50b10c5594e4326cf21043b414864dea546c491f742a5f75f6745777685c"},
    {L"libFLAC.dll", L"libFLAC.dll", 457216,
     "2911d4c7356e44fcd24527beb06993bbb699e4f3f459c32ed3fb579d4b91b6ec"},
    {L"libfluidsynth-3.dll", L"libfluidsynth-3.dll", 807424,
     "3463c79bce4ccccc52b301d3ab12a37eeabe7adb2821e55dfbf14fc203bd458c"},
    {L"libgcc_s_seh-1.dll", L"libgcc_s_seh-1.dll", 148480,
     "f11959ff0a34e71119d69f7bc52f25c94e2e9325168dd44f758cce2e002db998"},
    {L"libgomp-1.dll", L"libgomp-1.dll", 322560,
     "4ee684ca971a30b1db8bf1bdb11ec47360e1b2a7ca3554af4fd6a9b8256be44e"},
    {L"libiconv-2.dll", L"libiconv-2.dll", 1139200,
     "5131b719129b29b4c9a485420b76f3ac8e927cd1447967f4c134651696212a4e"},
    {L"libmp3lame-0.dll", L"libmp3lame-0.dll", 325120,
     "d3af6429e604be0451d1caa9eadf933ffc280f1d84deee239b142e413f932640"},
    {L"libmpg123-0.dll", L"libmpg123-0.dll", 445952,
     "89d1f029d38be4070b78caf735af7216a837a97c3cb181c009a94c3334c7907f"},
    {L"libogg-0.dll", L"libogg-0.dll", 39936,
     "27912efbd55c92fa20eea3f66502043f97e74dc32fa7836c0408afe5a9b6f452"},
    {L"libopus-0.dll", L"libopus-0.dll", 488960,
     "106a34fe8d17ec30b59914aefdbee37c95446fe1d468c5b4126e59afe482d19d"},
    {L"libopusfile-0.dll", L"libopusfile-0.dll", 52736,
     "23d4a2e83bd8e58ecb355f86d8fe8f2a3c49dd5a9b0a15d3dd426eafb7fd12d6"},
    {L"libpng16-16.dll", L"libpng16-16.dll", 250880,
     "9b8d6099f6cbe21ef39be80062c2c75905c921f3d221e4b91588ef7ebb14c65d"},
    {L"libportaudio.dll", L"libportaudio.dll", 249344,
     "4dfab3f64f5f251b11aa64c22560d06d7558614bb5d0c2511fe31d7169ee4a38"},
    {L"libreadline8.dll", L"libreadline8.dll", 283136,
     "f62a3cf7d2cd41eeba8521a8b86f5c49f62fd77d35b85ba1ed9bfb8176d517f9"},
    {L"libsamplerate-0.dll", L"libsamplerate-0.dll", 1502208,
     "b853f9ec802475e2ba9fb1f49d2c6b7e33d60c050833ae5b8984b4280ab34068"},
    {L"libsndfile-1.dll", L"libsndfile-1.dll", 702976,
     "d4732b7acb4b00db52394821a617dfa6f6fa1fbaa0e06a538e8a09e47f17c715"},
    {L"libstdc++-6.dll", L"libstdc++-6.dll", 2651136,
     "e61cecdb7e64fe59525b84f81ec93f9a226784fab0fc4b0e11caba0ae26ceae5"},
    {L"libtermcap-0.dll", L"libtermcap-0.dll", 15360,
     "7c523aef5172ef976c362d1df8c69af05064f54668dc5a13b27e3bd28e97bb01"},
    {L"libvorbis-0.dll", L"libvorbis-0.dll", 176640,
     "bbab7afbeed422aa84bac69a46ace6a031eb7f0bb1530949ceac079e57c10ad9"},
    {L"libvorbisenc-2.dll", L"libvorbisenc-2.dll", 576000,
     "6f1d8713b10775a5246ee6384bc3be0814e789a22ac6c823ad492a8921416f71"},
    {L"libvorbisfile-3.dll", L"libvorbisfile-3.dll", 33792,
     "3b111c0079aa6cc094d7bd938f0f53f1442d7df2a2ba4157bed6760607cd07cb"},
    {L"libwavpack-1.dll", L"libwavpack-1.dll", 216576,
     "19da57929f151beede5f5e591dec8c56020215a78440776cd978488b64dd3e21"},
    {L"libwinpthread-1.dll", L"libwinpthread-1.dll", 60416,
     "1dd6c00a83edb9f848886e03ff99cc86fe3f9956692a6d47780e1328b7a2f8e3"},
    {L"libxmp.dll", L"libxmp.dll", 832512,
     "1a52a14b2755bf42481c5f71cce07218d61b7f03edec0b0fad8438420f3bdeb2"},
    {L"zlib1.dll", L"zlib1.dll", 124416,
     "ae69a6089d94b8cbd13e4271d8a4cf292020831deb41dceb36e6adcbc343127e"},
};

constexpr Asset kGameFiles[] = {
    {L"freedoom-0.13.0/COPYING.txt", L"Freedoom-COPYING.txt", 1644,
     "7c62f2c520769c798774f416637eec4921e5f0aafdac1245ae7c8a8cf65fe102"},
    {L"freedoom-0.13.0/freedoom1.wad", L"freedoom1.wad", 28795076,
     "7323bcc168c5a45ff10749b339960e98314740a734c30d4b9f3337001f9e703d"},
    {L"freedoom-0.13.0/freedoom2.wad", L"freedoom2.wad", 28787748,
     "a8772e088847032510d97ba2312406a6998f21cbab44d4ff10696faa9c0ecd4b"},
};

struct Release {
    PCWSTR path;
    DWORD size;
    const char* sha256;
    PCWSTR directory;
    const Asset* files;
    size_t count;
};

constexpr Release kEngine{
    L"/fabiangreffrath/crispy-doom/releases/download/crispy-doom-7.2/crispy-doom-7.2.0-win64.zip",
    9924686, "c04feca63c00fd4c381c53cce8444a2bb520020be38368ce80da60484c94f2ab",
    L"crispy-doom-7.2.0-x64", kEngineFiles, ARRAYSIZE(kEngineFiles),
};
constexpr Release kGame{
    L"/freedoom/freedoom/releases/download/v0.13.0/freedoom-0.13.0.zip",
    24143781, "3f9b264f3e3ce503b4fb7f6bdcb1f419d93c7b546f4df3e874dd878db9688f59",
    L"freedoom-0.13.0", kGameFiles, ARRAYSIZE(kGameFiles),
};

namespace fs = std::filesystem;
using FileHandle = std::unique_ptr<void, decltype(&CloseHandle)>;
using HttpHandle = std::unique_ptr<void, decltype(&WinHttpCloseHandle)>;
using Digest = std::array<UCHAR, 32>;

bool CanRunEngine() {
    SYSTEM_INFO system{};
    GetNativeSystemInfo(&system);
    if (system.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) return true;

    auto getMachineTypeAttributes = reinterpret_cast<decltype(&GetMachineTypeAttributes)>(
        GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetMachineTypeAttributes"));
    MACHINE_ATTRIBUTES attributes{};
    return getMachineTypeAttributes &&
           SUCCEEDED(getMachineTypeAttributes(IMAGE_FILE_MACHINE_AMD64, &attributes)) &&
           (attributes & UserEnabled);
}

bool EnsureDirectory(const fs::path& path) {
    if (!CreateDirectoryW(path.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) return false;
    const DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) &&
           !(attributes & FILE_ATTRIBUTE_REPARSE_POINT);
}

std::string HexDigest(const Digest& digest) {
    constexpr char kHex[] = "0123456789abcdef";
    std::string result;
    result.reserve(digest.size() * 2);
    for (const UCHAR byte : digest) {
        result += kHex[byte >> 4];
        result += kHex[byte & 15];
    }
    return result;
}

bool HashFile(const fs::path& path, std::string& checksum, HANDLE stop, LONGLONG expectedSize = -1) {
    HANDLE opened = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (opened == INVALID_HANDLE_VALUE) return false;
    FileHandle file(opened, CloseHandle);
    BY_HANDLE_FILE_INFORMATION info{};
    LARGE_INTEGER size{};
    if (!GetFileInformationByHandle(file.get(), &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ||
        !GetFileSizeEx(file.get(), &size) || (expectedSize >= 0 && size.QuadPart != expectedSize)) return false;

    BCRYPT_ALG_HANDLE algorithm{};
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) return false;
    BCRYPT_HASH_HANDLE hash{};
    bool ok = BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) == 0;
    std::array<UCHAR, 65536> buffer{};
    Digest digest{};
    while (ok) {
        DWORD read{};
        ok = WaitForSingleObject(stop, 0) == WAIT_TIMEOUT &&
             ReadFile(file.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr);
        if (!ok || !read) break;
        ok = BCryptHashData(hash, buffer.data(), read, 0) == 0;
    }
    if (ok) ok = BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0) == 0;
    if (hash) BCryptDestroyHash(hash);
    BCryptCloseAlgorithmProvider(algorithm, 0);
    if (ok) checksum = HexDigest(digest);
    return ok;
}

bool Matches(const fs::path& path, DWORD size, const char* expected, HANDLE stop) {
    std::string checksum;
    return HashFile(path, checksum, stop, size) && checksum == expected;
}

bool Download(const Release& release, const fs::path& archive, HANDLE stop) {
    HttpHandle session(WinHttpOpen(L"Windhawk Doom Hotkey", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                   WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0), WinHttpCloseHandle);
    if (!session || !WinHttpSetTimeouts(session.get(), 5000, 5000, 5000, 5000)) return false;
    HttpHandle connection(WinHttpConnect(session.get(), L"github.com", INTERNET_DEFAULT_HTTPS_PORT, 0),
                          WinHttpCloseHandle);
    if (!connection) return false;
    HttpHandle request(WinHttpOpenRequest(connection.get(), L"GET", release.path, nullptr,
                                         WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE),
                       WinHttpCloseHandle);
    DWORD redirect = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
    DWORD features = WINHTTP_DISABLE_COOKIES | WINHTTP_DISABLE_AUTHENTICATION;
    if (!request || !WinHttpSetOption(request.get(), WINHTTP_OPTION_REDIRECT_POLICY, &redirect, sizeof(redirect)) ||
        !WinHttpSetOption(request.get(), WINHTTP_OPTION_DISABLE_FEATURE, &features, sizeof(features)) ||
        !WinHttpSendRequest(request.get(), WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request.get(), nullptr)) return false;

    DWORD status{}, length = sizeof(status);
    if (!WinHttpQueryHeaders(request.get(), WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             WINHTTP_HEADER_NAME_BY_INDEX, &status, &length, WINHTTP_NO_HEADER_INDEX) ||
        status != HTTP_STATUS_OK) return false;

    HANDLE opened = CreateFileW(archive.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (opened == INVALID_HANDLE_VALUE) return false;
    FileHandle file(opened, CloseHandle);
    const ULONGLONG started = GetTickCount64();
    std::array<UCHAR, 65536> buffer{};
    DWORD total = 0;
    while (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT && GetTickCount64() - started < 180000) {
        DWORD read{}, written{};
        if (!WinHttpReadData(request.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &read)) return false;
        if (!read) return total == release.size;
        if (read > release.size - total || !WriteFile(file.get(), buffer.data(), read, &written, nullptr) ||
            written != read) return false;
        total += read;
    }
    return false;
}

bool ExtractRelease(const Release& release, const fs::path& archive, const fs::path& directory, HANDLE stop) {
    wchar_t windows[MAX_PATH]{};
    const UINT length = GetWindowsDirectoryW(windows, ARRAYSIZE(windows));
    if (!length || length >= ARRAYSIZE(windows)) return false;
    const fs::path root(windows);
    auto tar = root / L"Sysnative" / L"tar.exe";
    if (GetFileAttributesW(tar.c_str()) == INVALID_FILE_ATTRIBUTES) tar = root / L"System32" / L"tar.exe";

    std::wstring command = L"\"" + tar.wstring() + L"\" -xf \"" + archive.wstring() +
                           L"\" -C \"" + directory.wstring() + L"\" --";
    for (size_t i = 0; i < release.count; ++i) {
        command += L" \"" + std::wstring(release.files[i].member) + L"\"";
    }
    STARTUPINFOW startup{sizeof(startup)};
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(tar.c_str(), command.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                        nullptr, root.c_str(), &startup, &process)) {
        Wh_Log(L"Could not run Windows tar.exe (error %lu).", GetLastError());
        return false;
    }
    CloseHandle(process.hThread);
    FileHandle child(process.hProcess, CloseHandle);
    const HANDLE events[]{stop, child.get()};
    if (WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, 60000) != WAIT_OBJECT_0 + 1) {
        if (TerminateProcess(child.get(), 1)) WaitForSingleObject(child.get(), 5000);
        return false;
    }
    DWORD code{};
    return GetExitCodeProcess(child.get(), &code) && code == 0;
}

struct StagingDirectory {
    fs::path path;
    ~StagingDirectory() {
        if (!path.empty()) {
            std::error_code error;
            fs::remove_all(path, error);
        }
    }
};

bool EnsureRelease(const Release& release, const fs::path& cache, bool allowDownload, HANDLE stop) {
    const auto destination = cache / release.directory;
    if (!EnsureDirectory(destination)) return false;

    std::vector<const Asset*> missing;
    for (size_t i = 0; i < release.count; ++i) {
        const auto& file = release.files[i];
        if (!Matches(destination / file.name, file.size, file.sha256, stop)) missing.push_back(&file);
    }
    if (missing.empty()) return true;
    if (!allowDownload || WaitForSingleObject(stop, 0) != WAIT_TIMEOUT) return false;

    GUID id{};
    wchar_t name[40]{};
    if (FAILED(CoCreateGuid(&id)) || !StringFromGUID2(id, name, ARRAYSIZE(name))) return false;
    const auto temporary = cache / name;
    if (!CreateDirectoryW(temporary.c_str(), nullptr)) return false;
    const StagingDirectory stage{temporary};
    const auto archive = stage.path / L"release.zip";
    Wh_Log(L"Downloading official release: %ls", release.directory);
    if (!Download(release, archive, stop) || !Matches(archive, release.size, release.sha256, stop) ||
        !ExtractRelease(release, archive, stage.path, stop)) return false;

    for (const auto* file : missing) {
        if (!Matches(stage.path / file->member, file->size, file->sha256, stop)) return false;
    }
    for (const auto* file : missing) {
        if (WaitForSingleObject(stop, 0) != WAIT_TIMEOUT ||
            !MoveFileExW((stage.path / file->member).c_str(), (destination / file->name).c_str(),
                         MOVEFILE_REPLACE_EXISTING)) return false;
    }
    return true;
}

bool HasOnlyEngineFiles(const fs::path& directory) {
    std::error_code error;
    fs::directory_iterator entry(directory, error), end;
    for (; !error && entry != end; entry.increment(error)) {
        const auto extension = entry->path().extension().wstring();
        if (_wcsicmp(extension.c_str(), L".dll") != 0 && _wcsicmp(extension.c_str(), L".exe") != 0 &&
            _wcsicmp(extension.c_str(), L".local") != 0 && _wcsicmp(extension.c_str(), L".manifest") != 0) continue;
        const auto name = entry->path().filename().wstring();
        bool expected = false;
        for (const auto& file : kEngineFiles) {
            if (_wcsicmp(name.c_str(), file.name) == 0) {
                expected = true;
                break;
            }
        }
        if (!expected) {
            Wh_Log(L"Unexpected executable, DLL or loader configuration in the Doom runtime: %ls", name.c_str());
            return false;
        }
    }
    return !error;
}

bool Prepare(std::wstring& engine, std::wstring& wad, int campaign, bool allowDownload,
             HANDLE stop, std::wstring& profileArguments) {
    if (!CanRunEngine()) {
        Wh_Log(L"Official Crispy Doom requires x64 support. Set Doom executable to a compatible local engine.");
        return false;
    }

    PWSTR local{};
    const HRESULT folderResult = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, nullptr, &local);
    std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> localAppData(local, CoTaskMemFree);
    if (FAILED(folderResult) || !localAppData) return false;
    const auto root = fs::path(localAppData.get()) / L"Windhawk Doom";
    const auto cache = root / L"Runtime";
    const auto profile = root / L"Profile";
    const auto saves = profile / L"Saves";
    if (!EnsureDirectory(root) || !EnsureDirectory(cache) || !EnsureDirectory(profile) || !EnsureDirectory(saves)) {
        return false;
    }

    const auto engineDirectory = cache / kEngine.directory;
    if (!EnsureRelease(kEngine, cache, allowDownload, stop) || !HasOnlyEngineFiles(engineDirectory)) return false;

    fs::path saveDirectory;
    if (wad.empty()) {
        if (!EnsureRelease(kGame, cache, allowDownload, stop)) return false;
        const auto name = campaign == 2 ? L"freedoom2.wad" : L"freedoom1.wad";
        wad = (cache / kGame.directory / name).wstring();
        saveDirectory = saves / name;
    } else {
        // Keep the same saves if the user renames or moves the same IWAD.
        std::string checksum;
        if (!HashFile(wad, checksum, stop)) return false;
        saveDirectory = saves / (L"custom-" + std::wstring(checksum.begin(), checksum.end()));
    }
    if (!EnsureDirectory(saveDirectory)) return false;
    engine = (engineDirectory / L"crispy-doom.exe").wstring();
    profileArguments = L" -config \"" + (profile / L"default.cfg").wstring() + L"\" -extraconfig \"" +
                       (profile / L"crispy-doom.cfg").wstring() + L"\" -savedir \"" + saveDirectory.wstring() + L"\"";
    return true;
}

}  // namespace DoomRuntime

namespace {

struct Shortcut {
    UINT modifiers{};
    UINT key{};
    bool operator==(const Shortcut&) const = default;
};

std::wstring_view Trim(std::wstring_view text) {
    while (!text.empty() && iswspace(text.front())) text.remove_prefix(1);
    while (!text.empty() && iswspace(text.back())) text.remove_suffix(1);
    return text;
}

bool ParseShortcut(std::wstring_view text, Shortcut& result) {
    Shortcut shortcut;
    for (;;) {
        const auto split = text.find(L'+');
        const auto part = Trim(text.substr(0, split));
        std::wstring token(part);
        for (auto& ch : token) ch = static_cast<wchar_t>(towupper(ch));
        if (split == std::wstring_view::npos) {
            if (token.size() == 1 &&
                ((token[0] >= L'A' && token[0] <= L'Z') ||
                 (token[0] >= L'0' && token[0] <= L'9'))) {
                shortcut.key = token[0];
            } else if ((token.size() == 2 || token.size() == 3) && token[0] == L'F' &&
                       token[1] >= L'1' && token[1] <= L'9' &&
                       (token.size() == 2 || (token[2] >= L'0' && token[2] <= L'9'))) {
                UINT function = token[1] - L'0';
                if (token.size() == 3) function = function * 10 + token[2] - L'0';
                if (function <= 24 && function != 12) shortcut.key = VK_F1 + function - 1;
            }
            if (!shortcut.modifiers || !shortcut.key) return false;
            result = shortcut;
            return true;
        }
        const UINT modifier = token == L"CTRL" || token == L"CONTROL" ? MOD_CONTROL :
                              token == L"ALT" ? MOD_ALT :
                              token == L"SHIFT" ? MOD_SHIFT :
                              token == L"WIN" ? MOD_WIN : 0;
        if (!modifier || (shortcut.modifiers & modifier)) return false;
        shortcut.modifiers |= modifier;
        text.remove_prefix(split + 1);
    }
}

std::wstring GetSetting(PCWSTR key) {
    return WindhawkUtils::StringSetting::make(key).get();
}

std::wstring ExpandPath(std::wstring_view value) {
    value = Trim(value);
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"') {
        value.remove_prefix(1);
        value.remove_suffix(1);
    }
    if (value.empty() || value.find(L'"') != std::wstring_view::npos) return {};
    const std::wstring raw(value);
    DWORD needed = ExpandEnvironmentStringsW(raw.c_str(), nullptr, 0);
    if (!needed || needed > 32767) return {};
    std::wstring expanded(needed, L'\0');
    const DWORD written = ExpandEnvironmentStringsW(raw.c_str(), expanded.data(), needed);
    if (!written || written > needed) return {};
    expanded.resize(written - 1);
    return expanded;
}

bool IsFile(const std::wstring& path, std::wstring_view extension) {
    if (path.size() <= extension.size() || path.size() < 4 || path[1] != L':' ||
        (path[2] != L'\\' && path[2] != L'/') || path.find(L'"') != std::wstring::npos ||
        CompareStringOrdinal(path.data() + path.size() - extension.size(),
                             static_cast<int>(extension.size()), extension.data(),
                             static_cast<int>(extension.size()), TRUE) != CSTR_EQUAL) return false;
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL CALLBACK FocusGameWindow(HWND hwnd, LPARAM data) {
    DWORD windowPid{};
    GetWindowThreadProcessId(hwnd, &windowPid);
    if (windowPid != static_cast<DWORD>(data) || !IsWindowVisible(hwnd) ||
        GetWindow(hwnd, GW_OWNER)) return TRUE;
    if (IsIconic(hwnd)) ShowWindowAsync(hwnd, SW_RESTORE);
    SetForegroundWindow(hwnd);
    return FALSE;
}

void LaunchGame(HANDLE& game, HANDLE stop) {
    if (game) {
        if (WaitForSingleObject(game, 0) == WAIT_TIMEOUT) {
            EnumWindows(FocusGameWindow, static_cast<LPARAM>(GetProcessId(game)));
            return;
        }
        CloseHandle(game);
        game = nullptr;
    }

    const auto engineSetting = GetSetting(L"enginePath");
    auto engine = ExpandPath(engineSetting);
    const auto wadSetting = GetSetting(L"iwadPath");
    auto wad = ExpandPath(wadSetting);
    if (!Trim(wadSetting).empty() && !IsFile(wad, L".wad")) {
        Wh_Log(L"Game WAD must point to an existing local .wad file, or be empty.");
        return;
    }
    std::wstring profileArguments;
    if (Trim(engineSetting).empty()) {
        const int campaign = GetSetting(L"campaign") == L"phase2" ? 2 : 1;
        const bool allowDownload = Wh_GetIntSetting(L"downloadOfficialFiles") != 0;
        if (!DoomRuntime::Prepare(engine, wad, campaign, allowDownload, stop, profileArguments)) {
            Wh_Log(L"Could not prepare the official Doom files. Check Download official game files or set Doom executable.");
            return;
        }
    }
    if (!IsFile(engine, L".exe")) {
        Wh_Log(L"Set Doom executable to the full path of a local Doom engine executable.");
        return;
    }

    const auto directory = engine.substr(0, engine.find_last_of(L"\\/") + 1);
    std::wstring command = L"\"" + engine + L"\"";
    if (!wad.empty()) command += L" -iwad \"" + wad + L"\"";
    command += profileArguments;
    command += L" -fullscreen";
    if (WaitForSingleObject(stop, 0) != WAIT_TIMEOUT) return;
    STARTUPINFOW startup{sizeof(startup)};
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(engine.c_str(), command.data(), nullptr, nullptr, FALSE, 0,
                        nullptr, directory.c_str(), &startup, &process)) {
        Wh_Log(L"Doom could not start (Windows error %lu).", GetLastError());
        return;
    }
    CloseHandle(process.hThread);
    game = process.hProcess;
}

HANDLE g_stop{};
HANDLE g_reload{};
HANDLE g_thread{};

DWORD WINAPI RunHotkey(LPVOID) {
    MSG message{};
    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
    const HANDLE events[]{g_stop, g_reload};
    HANDLE game{};
    Shortcut current;
    int registeredId = 0;
    bool apply = true;

    while (WaitForSingleObject(g_stop, 0) == WAIT_TIMEOUT) {
        if (apply) {
            apply = false;
            Shortcut desired;
            if (!ParseShortcut(GetSetting(L"shortcut"), desired)) {
                Wh_Log(L"Invalid Doom shortcut. Use a modifier and a letter, number or function key.");
            } else if (desired != current) {
                const int nextId = registeredId == 1 ? 2 : 1;
                if (RegisterHotKey(nullptr, nextId, desired.modifiers | MOD_NOREPEAT, desired.key)) {
                    if (registeredId) UnregisterHotKey(nullptr, registeredId);
                    registeredId = nextId;
                    current = desired;
                } else {
                    Wh_Log(L"Could not register the Doom shortcut (Windows error %lu).", GetLastError());
                }
            }
        }

        const DWORD signal = MsgWaitForMultipleObjects(ARRAYSIZE(events), events, FALSE,
                                                       INFINITE, QS_ALLINPUT);
        if (signal == WAIT_OBJECT_0 || signal == WAIT_FAILED) break;
        if (signal == WAIT_OBJECT_0 + 1) apply = true;
        if (signal == WAIT_OBJECT_0 + ARRAYSIZE(events)) {
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (WaitForSingleObject(g_stop, 0) != WAIT_TIMEOUT) break;
                if (registeredId && message.message == WM_HOTKEY && message.hwnd == nullptr &&
                    message.wParam == static_cast<WPARAM>(registeredId) &&
                    LOWORD(message.lParam) == current.modifiers && HIWORD(message.lParam) == current.key) {
                    LaunchGame(game, g_stop);
                }
            }
        }
    }

    if (registeredId) UnregisterHotKey(nullptr, registeredId);
    if (game) CloseHandle(game);
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_reload = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stop && g_reload) g_thread = CreateThread(nullptr, 0, RunHotkey, nullptr, 0, nullptr);
    if (g_thread) return TRUE;
    if (g_stop) CloseHandle(g_stop);
    if (g_reload) CloseHandle(g_reload);
    return FALSE;
}

void WhTool_ModSettingsChanged() {
    SetEvent(g_reload);
}

void WhTool_ModUninit() {
    SetEvent(g_stop);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_reload);
    CloseHandle(g_stop);
}

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
