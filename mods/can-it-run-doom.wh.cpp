// ==WindhawkMod==
// @id              can-it-run-doom
// @name            Can It Run Doom
// @description     A deliberately absurd Windhawk experiment made to see how far you can push the idea of running Doom on everything
// @version         1.0
// @author          communism420
// @github          https://github.com/communism420
// @include         windhawk.exe
// @architecture    amd64
// @compilerOptions -lshell32 -lbcrypt
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Can It Run Doom

This deliberately absurd mod was created to test how far you can go trying to
run Doom on everything.

To keep the Windows shell isolated, the mod uses Windhawk's dedicated tool-mod
pattern and runs in a separate `windhawk.exe` process instead of inside
`explorer.exe`.

By default it can:
- prepare a Doom-compatible bundle in `%USERPROFILE%\CanItRunDoom_files`
- show a startup notification that launches Doom when clicked
- prefer a user-supplied `DOOM.WAD`, `DOOM1.WAD`, or `DOOM2.WAD` if one is
  placed in the install directory

External dependencies:
- Chocolate Doom 3.1.1 from its official GitHub release
- Freedoom 0.13.0 Phase 1 from its official GitHub release

The commercial Doom IWAD is not redistributable, so the automatic setup uses
Freedoom by default. External downloads can be disabled for offline use in the
mod settings. The downloaded archives are version-pinned and SHA-256 verified
before extraction.

Final runtime files are stored in the configured install directory. Temporary
download files are kept in Windhawk's mod storage directory. The mod doesn't
modify Windows settings or system files, and it doesn't automatically delete
the install directory because it may contain user-supplied IWAD files.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ShowStartupNotification: true
  $name: Show startup notification
  $description: Show a clickable notification after the Doom bundle is ready.
- AllowExternalDownloads: true
  $name: Allow external downloads
  $description: Download missing Chocolate Doom and Freedoom files from their official GitHub releases. Disable this for offline use.
- InstallDirectory: %USERPROFILE%\\CanItRunDoom_files
  $name: Install directory
  $description: Folder where the Doom bundle is stored.
- NotificationTimeoutMs: 15000
  $name: Notification timeout (ms)
  $description: How long the startup notification stays visible before it disappears.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <shlobj.h>

#include <algorithm>
#include <string>
#include <vector>

namespace {

struct Settings {
    bool showStartupNotification = true;
    bool allowExternalDownloads = true;
    int notificationTimeoutMs = 15000;
    std::wstring installDirectory = L"%USERPROFILE%\\CanItRunDoom_files";
};

struct InstallPaths {
    std::wstring root;
    std::wstring engineDir;
    std::wstring gameDir;
    std::wstring scratchDir;
    std::wstring engineArchive;
    std::wstring gameArchive;
    std::wstring engineVersionFile;
    std::wstring gameVersionFile;
    std::wstring exePath;
    std::wstring freedoomWad;
    std::wstring freedoomExtractedDir;
};

constexpr UINT kTrayIconId = 1;
constexpr UINT kTrayCallback = WM_APP + 1;
constexpr DWORD kShellWaitTimeoutMs = 60000;
constexpr DWORD kShellWaitStepMs = 500;
constexpr PCWSTR kWindowClassName = L"CanItRunDoomWindow";
constexpr PCWSTR kDefaultInstallDirectory = L"%USERPROFILE%\\CanItRunDoom_files";
constexpr PCWSTR kEngineUrl =
    L"https://github.com/chocolate-doom/chocolate-doom/releases/download/"
    L"chocolate-doom-3.1.1/chocolate-doom-3.1.1-win64.zip";
constexpr PCWSTR kGameUrl =
    L"https://github.com/freedoom/freedoom/releases/download/"
    L"v0.13.0/freedoom-0.13.0.zip";
constexpr PCWSTR kEngineSha256 =
    L"58C34C61AE954493FCE5FF01FD553898240A0DE25658AA97B43AC9510C49581F";
constexpr PCWSTR kGameSha256 =
    L"3F9B264F3E3CE503B4FB7F6BDCB1F419D93C7B546F4DF3E874DD878DB9688F59";
constexpr char kEngineVersion[] = "chocolate-doom-3.1.1-win64";
constexpr char kGameVersion[] = "freedoom-0.13.0-phase1";

HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;
HWND g_window = nullptr;
bool g_trayAdded = false;
SRWLOCK g_settingsLock = SRWLOCK_INIT;
Settings g_settings;

std::wstring JoinPath(const std::wstring& left, const std::wstring& right) {
    if (left.empty()) return right;
    if (right.empty()) return left;
    if (left.back() == L'\\' || left.back() == L'/') return left + right;
    return left + L"\\" + right;
}

std::wstring ExpandEnv(PCWSTR value) {
    DWORD count = ExpandEnvironmentStringsW(value, nullptr, 0);
    if (!count) return value ? value : L"";
    std::wstring result(count, L'\0');
    if (!ExpandEnvironmentStringsW(value, result.data(), count)) return value;
    result.resize(count - 1);
    return result;
}

std::wstring EscapePs(const std::wstring& value) {
    std::wstring escaped;
    escaped.reserve(value.size());
    for (wchar_t ch : value) {
        escaped.push_back(ch);
        if (ch == L'\'') escaped.push_back(L'\'');
    }
    return escaped;
}

std::wstring QuoteArg(const std::wstring& value) {
    if (value.find_first_of(L" \t\"") == std::wstring::npos) return value;
    std::wstring quoted = L"\"";
    size_t backslashes = 0;
    for (wchar_t ch : value) {
        if (ch == L'\\') {
            ++backslashes;
            continue;
        }
        if (ch == L'"') {
            quoted.append(backslashes * 2 + 1, L'\\');
            quoted.push_back(L'"');
            backslashes = 0;
            continue;
        }
        quoted.append(backslashes, L'\\');
        backslashes = 0;
        quoted.push_back(ch);
    }
    quoted.append(backslashes * 2, L'\\');
    quoted.push_back(L'"');
    return quoted;
}

bool ExistsFile(const std::wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool ExistsDir(const std::wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool EnsureDir(const std::wstring& path) {
    if (ExistsDir(path)) return true;
    int result = SHCreateDirectoryExW(nullptr, path.c_str(), nullptr);
    return result == ERROR_SUCCESS || result == ERROR_ALREADY_EXISTS ||
           result == ERROR_FILE_EXISTS;
}

bool DeleteTree(const std::wstring& path) {
    if (!ExistsDir(path) && !ExistsFile(path)) return true;
    std::wstring from = path;
    from.push_back(L'\0');
    from.push_back(L'\0');
    SHFILEOPSTRUCTW op = {};
    op.wFunc = FO_DELETE;
    op.pFrom = from.c_str();
    op.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    return SHFileOperationW(&op) == 0;
}

bool IsStopping() {
    return g_stopEvent && WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}

void ReportStatus(const std::wstring& message, bool interactive) {
    Wh_Log(L"%s", message.c_str());
    if (interactive) {
        MessageBoxW(IsWindow(g_window) ? g_window : nullptr, message.c_str(),
                    L"Can It Run Doom",
                    MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    }
}

std::wstring GetSettingString(PCWSTR name, PCWSTR fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = (value && value[0]) ? value : fallback;
    Wh_FreeStringSetting(value);
    return result;
}

Settings LoadSettings() {
    Settings settings;
    settings.showStartupNotification =
        Wh_GetIntSetting(L"ShowStartupNotification") != 0;
    settings.allowExternalDownloads =
        Wh_GetIntSetting(L"AllowExternalDownloads") != 0;
    settings.installDirectory =
        GetSettingString(L"InstallDirectory", kDefaultInstallDirectory);
    int timeoutMs = Wh_GetIntSetting(L"NotificationTimeoutMs");
    settings.notificationTimeoutMs =
        timeoutMs > 0 ? std::clamp(timeoutMs, 3000, 30000) : 15000;
    return settings;
}

void ReloadSettings() {
    Settings settings = LoadSettings();
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(settings);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings settings = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return settings;
}

std::wstring GetModStoragePath() {
    std::wstring buffer(32768, L'\0');
    size_t length = Wh_GetModStoragePath(buffer.data(), buffer.size());
    if (!length || length >= buffer.size()) return L"";
    buffer.resize(length);
    return buffer;
}

InstallPaths GetPaths(const Settings& settings) {
    InstallPaths paths;
    paths.root = ExpandEnv(settings.installDirectory.c_str());
    if (paths.root.empty()) paths.root = ExpandEnv(kDefaultInstallDirectory);
    paths.engineDir = JoinPath(paths.root, L"engine");
    paths.gameDir = JoinPath(paths.root, L"game");

    std::wstring modStoragePath = GetModStoragePath();
    if (modStoragePath.empty()) {
        paths.scratchDir = JoinPath(paths.root, L"_windhawk_cache");
    } else {
        paths.scratchDir = JoinPath(modStoragePath, L"downloads");
    }

    paths.engineArchive =
        JoinPath(paths.scratchDir, L"chocolate-doom-3.1.1-win64.zip");
    paths.gameArchive = JoinPath(paths.scratchDir, L"freedoom-0.13.0.zip");
    paths.engineVersionFile = JoinPath(paths.engineDir, L".bundle-version");
    paths.gameVersionFile = JoinPath(paths.gameDir, L".bundle-version");
    paths.exePath = JoinPath(paths.engineDir, L"chocolate-doom.exe");
    paths.freedoomWad = JoinPath(paths.gameDir, L"freedoom1.wad");
    paths.freedoomExtractedDir = JoinPath(paths.scratchDir, L"freedoom-0.13.0");
    return paths;
}

bool ReadSmallAsciiFile(const std::wstring& path, std::string* text) {
    text->clear();
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0 || size.QuadPart > 4096) {
        CloseHandle(file);
        return false;
    }

    text->resize(static_cast<size_t>(size.QuadPart));
    DWORD bytesRead = 0;
    bool ok = ReadFile(file, text->data(), static_cast<DWORD>(text->size()), &bytesRead,
                       nullptr) != FALSE;
    CloseHandle(file);
    if (!ok || bytesRead != text->size()) {
        text->clear();
        return false;
    }
    return true;
}

bool WriteSmallAsciiFile(const std::wstring& path, const char* text) {
    HANDLE file =
        CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    DWORD bytesWritten = 0;
    size_t textLen = static_cast<size_t>(lstrlenA(text));
    bool ok = WriteFile(file, text, static_cast<DWORD>(textLen), &bytesWritten,
                        nullptr) != FALSE;
    CloseHandle(file);
    return ok && bytesWritten == textLen;
}

bool VersionFileMatches(const std::wstring& path, const char* expected) {
    std::string actual;
    return ReadSmallAsciiFile(path, &actual) && actual == expected;
}

bool EngineFilesPresent(const InstallPaths& paths) {
    static const PCWSTR requiredFiles[] = {
        L"chocolate-doom.exe",
        L"SDL2.dll",
        L"SDL2_mixer.dll",
        L"SDL2_net.dll",
        L"SDL3.dll",
    };

    for (PCWSTR file : requiredFiles) {
        if (!ExistsFile(JoinPath(paths.engineDir, file))) return false;
    }
    return true;
}

bool HasUserSuppliedWad(const InstallPaths& paths) {
    static const PCWSTR names[] = {
        L"DOOM.WAD",
        L"doom.wad",
        L"DOOM1.WAD",
        L"doom1.wad",
        L"DOOM2.WAD",
        L"doom2.wad",
    };

    for (PCWSTR name : names) {
        if (ExistsFile(JoinPath(paths.root, name))) return true;
    }
    return false;
}

std::wstring PickWad(const InstallPaths& paths) {
    static const PCWSTR names[] = {
        L"DOOM.WAD",
        L"doom.wad",
        L"DOOM1.WAD",
        L"doom1.wad",
        L"DOOM2.WAD",
        L"doom2.wad",
    };

    for (PCWSTR name : names) {
        std::wstring candidate = JoinPath(paths.root, name);
        if (ExistsFile(candidate)) return candidate;
    }

    return paths.freedoomWad;
}

bool IsEngineInstalled(const InstallPaths& paths) {
    return VersionFileMatches(paths.engineVersionFile, kEngineVersion) &&
           EngineFilesPresent(paths);
}

bool IsGameInstalled(const InstallPaths& paths) {
    if (HasUserSuppliedWad(paths)) return true;
    return VersionFileMatches(paths.gameVersionFile, kGameVersion) &&
           ExistsFile(paths.freedoomWad);
}

bool IsBundleReady(const InstallPaths& paths) {
    return IsEngineInstalled(paths) && IsGameInstalled(paths);
}

bool RunHidden(const std::wstring& app, const std::wstring& args,
               const std::wstring& cwd = L"") {
    std::wstring command = QuoteArg(app);
    if (!args.empty()) command += L" " + args;

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = {};
    std::wstring mutableCommand = command;
    if (!CreateProcessW(app.c_str(), mutableCommand.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr,
                        cwd.empty() ? nullptr : cwd.c_str(), &si, &pi)) {
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return exitCode == 0;
}

bool ExtractZip(const std::wstring& zipPath, const std::wstring& destPath) {
    wchar_t systemDir[MAX_PATH];
    if (!GetSystemDirectoryW(systemDir, _countof(systemDir))) return false;

    std::wstring tarPath = JoinPath(systemDir, L"tar.exe");
    if (ExistsFile(tarPath) &&
        RunHidden(tarPath, L"-xf " + QuoteArg(zipPath) + L" -C " + QuoteArg(destPath),
                  destPath)) {
        return true;
    }

    std::wstring powershellPath =
        JoinPath(systemDir, L"WindowsPowerShell\\v1.0\\powershell.exe");
    if (!ExistsFile(powershellPath)) return false;

    std::wstring script =
        L"& { Expand-Archive -LiteralPath '" + EscapePs(zipPath) +
        L"' -DestinationPath '" + EscapePs(destPath) + L"' -Force }";
    return RunHidden(
        powershellPath,
        L"-NoLogo -NoProfile -NonInteractive -WindowStyle Hidden -Command " +
            QuoteArg(script),
        destPath);
}

bool BcryptSucceeded(NTSTATUS status) {
    return status >= 0;
}

bool ComputeFileSha256(const std::wstring& path, BYTE hash[32]) {
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hashHandle = nullptr;
    HANDLE file = INVALID_HANDLE_VALUE;
    bool ok = false;

    do {
        if (!BcryptSucceeded(
                BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM,
                                            nullptr, 0))) {
            break;
        }

        DWORD objectSize = 0;
        DWORD resultSize = 0;
        if (!BcryptSucceeded(BCryptGetProperty(
                algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&objectSize),
                sizeof(objectSize), &resultSize, 0))) {
            break;
        }

        std::vector<BYTE> hashObject(objectSize);
        if (!BcryptSucceeded(BCryptCreateHash(algorithm, &hashHandle,
                                              hashObject.data(), objectSize, nullptr, 0,
                                              0))) {
            break;
        }

        file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) break;

        BYTE buffer[8192];
        bool readOk = true;
        while (true) {
            DWORD bytesRead = 0;
            if (!ReadFile(file, buffer, sizeof(buffer), &bytesRead, nullptr)) {
                readOk = false;
                break;
            }

            if (bytesRead == 0) break;

            if (!BcryptSucceeded(BCryptHashData(hashHandle, buffer, bytesRead, 0))) {
                readOk = false;
                break;
            }
        }

        if (!readOk) break;
        if (!BcryptSucceeded(BCryptFinishHash(hashHandle, hash, 32, 0))) break;
        ok = true;
    } while (false);

    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (hashHandle) BCryptDestroyHash(hashHandle);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    return ok;
}

std::wstring BytesToUpperHex(const BYTE* bytes, size_t size) {
    static constexpr wchar_t kHexDigits[] = L"0123456789ABCDEF";
    std::wstring text;
    text.reserve(size * 2);
    for (size_t i = 0; i < size; i++) {
        text.push_back(kHexDigits[(bytes[i] >> 4) & 0x0F]);
        text.push_back(kHexDigits[bytes[i] & 0x0F]);
    }
    return text;
}

bool VerifyFileSha256(const std::wstring& path, PCWSTR expectedHash) {
    BYTE hash[32];
    if (!ComputeFileSha256(path, hash)) return false;
    return BytesToUpperHex(hash, 32) == expectedHash;
}

bool DownloadFile(PCWSTR url, const std::wstring& destPath, PCWSTR expectedHash,
                  std::wstring* error) {
    DeleteFileW(destPath.c_str());

    WH_GET_URL_CONTENT_OPTIONS options = {};
    options.optionsSize = sizeof(options);
    options.targetFilePath = destPath.c_str();

    const WH_URL_CONTENT* content = Wh_GetUrlContent(url, &options);
    int statusCode = content ? content->statusCode : 0;
    if (content) Wh_FreeUrlContent(content);

    if (statusCode != 200 || !ExistsFile(destPath)) {
        *error = L"Failed to download " + std::wstring(url);
        DeleteFileW(destPath.c_str());
        return false;
    }

    if (!VerifyFileSha256(destPath, expectedHash)) {
        *error = L"SHA-256 verification failed for " + destPath;
        DeleteFileW(destPath.c_str());
        return false;
    }

    return true;
}

bool InstallEngine(const InstallPaths& paths, std::wstring* error) {
    if (!EnsureDir(paths.root) || !EnsureDir(paths.scratchDir)) {
        *error = L"Failed to create " + paths.root;
        return false;
    }

    if (ExistsDir(paths.engineDir) && !DeleteTree(paths.engineDir)) {
        *error = L"Failed to clean " + paths.engineDir;
        return false;
    }
    if (!EnsureDir(paths.engineDir)) {
        *error = L"Failed to prepare " + paths.engineDir;
        return false;
    }

    DeleteFileW(paths.engineVersionFile.c_str());
    if (!DownloadFile(kEngineUrl, paths.engineArchive, kEngineSha256, error)) {
        return false;
    }

    if (IsStopping()) {
        *error = L"Installation stopped.";
        DeleteFileW(paths.engineArchive.c_str());
        return false;
    }

    if (!ExtractZip(paths.engineArchive, paths.engineDir) ||
        !EngineFilesPresent(paths) ||
        !WriteSmallAsciiFile(paths.engineVersionFile, kEngineVersion)) {
        *error =
            L"Failed to install the Chocolate Doom bundle into " + paths.engineDir;
        DeleteFileW(paths.engineArchive.c_str());
        DeleteFileW(paths.engineVersionFile.c_str());
        return false;
    }

    DeleteFileW(paths.engineArchive.c_str());
    return true;
}

bool InstallFreedoom(const InstallPaths& paths, std::wstring* error) {
    if (HasUserSuppliedWad(paths)) return true;

    if (!EnsureDir(paths.root) || !EnsureDir(paths.scratchDir)) {
        *error = L"Failed to create " + paths.root;
        return false;
    }

    if (ExistsDir(paths.gameDir) && !DeleteTree(paths.gameDir)) {
        *error = L"Failed to clean " + paths.gameDir;
        return false;
    }
    if (!EnsureDir(paths.gameDir)) {
        *error = L"Failed to prepare " + paths.gameDir;
        return false;
    }

    DeleteFileW(paths.gameVersionFile.c_str());
    if (!DownloadFile(kGameUrl, paths.gameArchive, kGameSha256, error)) {
        return false;
    }

    if (IsStopping()) {
        *error = L"Installation stopped.";
        DeleteFileW(paths.gameArchive.c_str());
        return false;
    }

    if (ExistsDir(paths.freedoomExtractedDir) &&
        !DeleteTree(paths.freedoomExtractedDir)) {
        *error = L"Failed to clean temporary Freedoom files in " + paths.scratchDir;
        DeleteFileW(paths.gameArchive.c_str());
        return false;
    }
    if (!ExtractZip(paths.gameArchive, paths.scratchDir)) {
        *error = L"Failed to extract Freedoom into temporary storage";
        DeleteFileW(paths.gameArchive.c_str());
        return false;
    }

    std::wstring extractedWad =
        JoinPath(paths.freedoomExtractedDir, L"freedoom1.wad");
    if (!ExistsFile(extractedWad) ||
        !CopyFileW(extractedWad.c_str(), paths.freedoomWad.c_str(), FALSE) ||
        !WriteSmallAsciiFile(paths.gameVersionFile, kGameVersion)) {
        *error = L"Failed to prepare freedoom1.wad in " + paths.gameDir;
        DeleteFileW(paths.gameArchive.c_str());
        DeleteTree(paths.freedoomExtractedDir);
        DeleteFileW(paths.gameVersionFile.c_str());
        return false;
    }

    DeleteTree(paths.freedoomExtractedDir);
    DeleteFileW(paths.gameArchive.c_str());
    return true;
}

bool EnsureInstalled(const Settings& settings, std::wstring* error) {
    InstallPaths paths = GetPaths(settings);
    if (!IsEngineInstalled(paths)) {
        if (!settings.allowExternalDownloads) {
            *error = L"Chocolate Doom is missing and external downloads are disabled.";
            return false;
        }
        if (!InstallEngine(paths, error)) return false;
    }

    if (!IsGameInstalled(paths)) {
        if (!settings.allowExternalDownloads) {
            *error = L"No IWAD is available and external downloads are disabled.";
            return false;
        }
        if (!InstallFreedoom(paths, error)) return false;
    }

    return IsBundleReady(paths);
}

bool LaunchDoom(bool interactive) {
    Settings settings = GetSettingsSnapshot();
    InstallPaths paths = GetPaths(settings);
    std::wstring error;

    if (!IsBundleReady(paths) && !EnsureInstalled(settings, &error)) {
        ReportStatus(error, interactive);
        return false;
    }

    std::wstring wad = PickWad(paths);
    SHELLEXECUTEINFOW executeInfo = {};
    executeInfo.cbSize = sizeof(executeInfo);
    std::wstring args = L"-iwad " + QuoteArg(wad);
    executeInfo.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    executeInfo.lpVerb = L"open";
    executeInfo.lpFile = paths.exePath.c_str();
    executeInfo.lpParameters = args.c_str();
    executeInfo.lpDirectory = paths.engineDir.c_str();
    executeInfo.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&executeInfo)) {
        ReportStatus(L"Failed to launch Doom from " + paths.root, interactive);
        return false;
    }

    return true;
}

bool WaitForShellTray() {
    DWORD waited = 0;
    while (waited <= kShellWaitTimeoutMs) {
        if (IsStopping()) return false;
        if (FindWindowW(L"Shell_TrayWnd", nullptr)) return true;
        WaitForSingleObject(g_stopEvent, kShellWaitStepMs);
        waited += kShellWaitStepMs;
    }

    Wh_Log(L"Shell tray window wasn't ready within %lu ms", kShellWaitTimeoutMs);
    return false;
}

void RemoveTray(HWND hwnd) {
    if (!g_trayAdded) return;
    NOTIFYICONDATAW iconData = {};
    iconData.cbSize = sizeof(iconData);
    iconData.hWnd = hwnd;
    iconData.uID = kTrayIconId;
    Shell_NotifyIconW(NIM_DELETE, &iconData);
    g_trayAdded = false;
}

bool AddHiddenTrayIcon(HWND hwnd) {
    if (g_trayAdded) return true;

    NOTIFYICONDATAW iconData = {};
    iconData.cbSize = sizeof(iconData);
    iconData.hWnd = hwnd;
    iconData.uID = kTrayIconId;
    iconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_STATE;
    iconData.uCallbackMessage = kTrayCallback;
    iconData.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    iconData.dwState = NIS_HIDDEN;
    iconData.dwStateMask = NIS_HIDDEN;
    wcsncpy_s(iconData.szTip, L"Can It Run Doom", _TRUNCATE);

    if (!Shell_NotifyIconW(NIM_ADD, &iconData)) return false;

    NOTIFYICONDATAW versionData = {};
    versionData.cbSize = sizeof(versionData);
    versionData.hWnd = hwnd;
    versionData.uID = kTrayIconId;
    versionData.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &versionData);

    g_trayAdded = true;
    return true;
}

bool ShowStartupBalloon(HWND hwnd, int timeoutMs) {
    if (!AddHiddenTrayIcon(hwnd)) return false;

    NOTIFYICONDATAW iconData = {};
    iconData.cbSize = sizeof(iconData);
    iconData.hWnd = hwnd;
    iconData.uID = kTrayIconId;
    iconData.uFlags = NIF_INFO;
    iconData.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
    iconData.uTimeout = static_cast<UINT>(timeoutMs);
    wcsncpy_s(iconData.szInfoTitle, L"Can It Run Doom", _TRUNCATE);
    wcsncpy_s(iconData.szInfo, L"Click this notification to launch Doom.",
              _TRUNCATE);
    return Shell_NotifyIconW(NIM_MODIFY, &iconData) != FALSE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == kTrayCallback) {
        switch (lParam) {
            case NIN_BALLOONUSERCLICK:
            case WM_LBUTTONUP:
            case NIN_SELECT:
            case NIN_KEYSELECT:
                LaunchDoom(true);
                DestroyWindow(hwnd);
                return 0;

            case NIN_BALLOONHIDE:
            case NIN_BALLOONTIMEOUT:
                DestroyWindow(hwnd);
                return 0;
        }
    }

    if (message == WM_CLOSE) {
        DestroyWindow(hwnd);
        return 0;
    }

    if (message == WM_DESTROY) {
        RemoveTray(hwnd);
        if (g_window == hwnd) g_window = nullptr;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool RunNotificationLoop(const Settings& settings) {
    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.lpszClassName = kWindowClassName;
    if (!RegisterClassW(&windowClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }

    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, kWindowClassName,
                                L"Can It Run Doom", WS_POPUP, 0, 0, 0, 0,
                                nullptr, nullptr, windowClass.hInstance, nullptr);
    if (!hwnd) return false;

    g_window = hwnd;
    if (!ShowStartupBalloon(hwnd, settings.notificationTimeoutMs)) {
        DestroyWindow(hwnd);
        return false;
    }

    HANDLE handles[] = {g_stopEvent};
    while (true) {
        DWORD waitResult =
            MsgWaitForMultipleObjects(1, handles, FALSE, INFINITE, QS_ALLINPUT);
        if (waitResult == WAIT_FAILED) {
            if (IsWindow(hwnd)) DestroyWindow(hwnd);
            return false;
        }

        if (waitResult == WAIT_OBJECT_0 && IsWindow(hwnd)) {
            DestroyWindow(hwnd);
        }

        if (waitResult == WAIT_OBJECT_0 + 1 || waitResult == WAIT_OBJECT_0) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) return true;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        if (!IsWindow(hwnd)) return true;
    }
}

DWORD WINAPI WorkerThreadProc(void*) {
    if (!WaitForShellTray()) return 0;

    Settings settings = GetSettingsSnapshot();
    InstallPaths paths = GetPaths(settings);
    if (!IsBundleReady(paths)) {
        if (!settings.allowExternalDownloads) {
            Wh_Log(L"Doom bundle isn't ready and external downloads are disabled");
            return 0;
        }

        std::wstring error;
        if (!EnsureInstalled(settings, &error)) {
            if (!IsStopping()) ReportStatus(error, false);
            return 0;
        }
    }

    settings = GetSettingsSnapshot();
    if (!settings.showStartupNotification) return 0;

    if (!RunNotificationLoop(settings) && !IsStopping()) {
        ReportStatus(L"Failed to show the startup notification.", false);
    }

    return 0;
}

BOOL WhTool_ModInit() {
    ReloadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    ReloadSettings();
}

void WhTool_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_window) PostMessageW(g_window, WM_CLOSE, 0, 0);

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

}  // namespace

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
