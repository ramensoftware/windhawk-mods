// ==WindhawkMod==
// @id              installed-mods-backup
// @name            Installed Mods & Settings Backup
// @description     Backs up your Windhawk ModsSource folder and Registry settings to a timestamped folder on your Desktop when enabled or settings are saved.
// @version         1.0.6
// @author          acooza
// @github          acooza1
// @include         explorer.exe
// @compilerOptions -lshlwapi -ladvapi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Installed Mods & Settings Backup
This mod automatically creates a secure backup of your Windhawk `ModsSource` directory and your registry settings into a timestamped folder on your Desktop.

* **One-click backup:** Open the mod's **Settings** tab and click **Save settings** anytime you want to generate a fresh backup.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showNotification: true
  $name: Show confirmation popup
  $description: Displays a message box confirming when the backup succeeds or fails.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct {
    bool showNotification;
} g_settings;

void LoadSettings() {
    g_settings.showNotification = Wh_GetIntSetting(L"showNotification") != 0;
}

bool RunHiddenCommand(const std::wstring& cmdLine) {
    STARTUPINFOW si = { sizeof(si) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = { 0 };
    std::wstring mutableCmd = cmdLine;

    if (!CreateProcessW(
            nullptr,
            &mutableCmd[0],
            nullptr,
            nullptr,
            FALSE,
            CREATE_NO_WINDOW,
            nullptr,
            nullptr,
            &si,
            &pi)) {
        return false;
    }

    WaitForSingleObject(pi.hProcess, 15000);
    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return exitCode == 0;
}

void RunBackup(bool forceRun = false) {
    // 1. Kernel-level exclusive lock file in TEMP to prevent simultaneous execution across explorer processes
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring lockFilePath = std::wstring(tempPath) + L"WindhawkBackup.lock";

    HANDLE hLockFile = CreateFileW(
        lockFilePath.c_str(),
        GENERIC_WRITE,
        0, // No sharing -> exclusive lock!
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
        nullptr
    );

    if (hLockFile == INVALID_HANDLE_VALUE) {
        // Another explorer process is already running the backup right now. Silently exit.
        return;
    }

    wchar_t desktopPath[MAX_PATH];
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, SHGFP_TYPE_CURRENT, desktopPath))) {
        Wh_Log(L"Failed to resolve Desktop folder path.");
        CloseHandle(hLockFile);
        return;
    }

    // Check 60-second cooldown on startup (unless forced by clicking Save Settings)
    if (!forceRun) {
        std::error_code ec;
        for (const auto& entry : fs::directory_iterator(desktopPath, ec)) {
            if (entry.is_directory()) {
                std::wstring dirName = entry.path().filename().wstring();
                if (dirName.rfind(L"Windhawk_Backup_", 0) == 0) {
                    auto ltime = fs::last_write_time(entry, ec);
                    if (!ec) {
                        auto sctp = decltype(ltime)::clock::to_sys(ltime);
                        auto now = std::chrono::system_clock::now();
                        auto ageSecs = std::chrono::duration_cast<std::chrono::seconds>(now - sctp).count();
                        if (ageSecs >= 0 && ageSecs < 60) {
                            CloseHandle(hLockFile);
                            return;
                        }
                    }
                }
            }
        }
    }

    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t timestamp[64];
    swprintf_s(timestamp, L"%04d-%02d-%02d_%02d-%02d-%02d",
               st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    fs::path backupDir = fs::path(desktopPath) / (L"Windhawk_Backup_" + std::wstring(timestamp));

    std::error_code ec;
    fs::create_directories(backupDir, ec);
    if (ec) {
        Wh_Log(L"Failed to create backup directory: %s", backupDir.c_str());
        CloseHandle(hLockFile);
        return;
    }

    wchar_t programData[MAX_PATH];
    GetEnvironmentVariableW(L"ProgramData", programData, MAX_PATH);
    fs::path modsSourceDir = fs::path(programData) / L"Windhawk" / L"ModsSource";
    fs::path destModsDir = backupDir / L"ModsSource";

    bool filesCopied = false;
    if (fs::exists(modsSourceDir, ec)) {
        fs::copy(modsSourceDir, destModsDir,
                 fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
        filesCopied = !ec;
    }

    fs::path regFile = backupDir / L"Windhawk_Settings.reg";
    std::wstring regCmd = L"reg.exe export \"HKLM\\SOFTWARE\\Windhawk\" \"" + regFile.wstring() + L"\" /y";
    bool regExported = RunHiddenCommand(regCmd);

    if (filesCopied && regExported) {
        Wh_Log(L"Windhawk backup succeeded: %s", backupDir.c_str());
        if (g_settings.showNotification) {
            std::wstring msg = L"Windhawk mods and settings backed up to:\n\n" + backupDir.wstring();
            MessageBoxW(nullptr, msg.c_str(), L"Windhawk Backup Complete", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
        }
    } else {
        Wh_Log(L"Windhawk backup completed with warnings (Files: %d, Reg: %d)", filesCopied, regExported);
        if (g_settings.showNotification) {
            std::wstring msg = L"Backup finished with partial errors.\nCheck folder: " + backupDir.wstring() +
                               L"\n(Note: Portable installs do not use HKLM registry keys.)";
            MessageBoxW(nullptr, msg.c_str(), L"Windhawk Backup Warning", MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
        }
    }

    // Release the exclusive file lock
    CloseHandle(hLockFile);
}

DWORD WINAPI BackupThreadProc(LPVOID lpParam) {
    bool force = (lpParam != nullptr);
    RunBackup(force);
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Installed Mods Backup mod");
    LoadSettings();

    HANDLE hThread = CreateThread(nullptr, 0, BackupThreadProc, (LPVOID)FALSE, 0, nullptr);
    if (hThread) {
        CloseHandle(hThread);
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    // Clicking "Save settings" forces a new backup immediately
    HANDLE hThread = CreateThread(nullptr, 0, BackupThreadProc, (LPVOID)TRUE, 0, nullptr);
    if (hThread) {
        CloseHandle(hThread);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Installed Mods Backup mod");
}
