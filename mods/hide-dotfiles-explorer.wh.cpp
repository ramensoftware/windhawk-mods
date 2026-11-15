// ==WindhawkMod==
// @id              hide-dotfiles-explorer
// @name            Hide Dotfiles (Explorer only)
// @description     Hide dotfiles and folders starting with . in Windows Explorer and Desktop
// @version         1.0.1
// @author          @danalec
// @github          https://github.com/danalec
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod hides files and folders starting with a dot (.) in Windows Explorer and Desktop, similar to Unix/Linux systems.

![Screenshot](https://i.imgur.com/y75mPVQ.png)

## How It Works
This mod **filters files directly from directory listings** at the system level.
It does **not** modify file attributes (like the "hidden" attribute) and **does not** depend on Windows Explorer's "Show hidden files" setting.
Files are completely hidden from view regardless of Explorer's hidden file visibility settings.

## Key Benefits
- **Independent of Explorer settings**: Works regardless of whether "Show hidden files" is enabled or disabled
- **No file attribute modification**: Files remain unchanged on disk
- **System-wide filtering**: Affects all Explorer views and file dialogs

## Configuration
You can exclude specific files from being hidden using the dotfile whitelist, or specify 
additional files to always hide regardless of whether they start with a dot.

### Display Modes
Choose how dotfiles should be handled:
- **Never show**: Files are completely hidden from directory listings (default)
- **Show as hidden**: Files are shown with the hidden attribute (like Unix `ls -a`)
- **Show as system**: Files are shown with both hidden and system attributes

### Filename Matching
- **Case-insensitive**: Filenames are matched regardless of case (e.g., `.Gitignore` will match `.gitignore`)
- **Pattern matching**: Supports wildcards using Windows pattern matching syntax
  - `*` matches zero or more characters
  - `?` matches exactly one character
  - Examples: `.env*` matches `.env`, `.env.local`, `.env.production`

### Settings
- **Display Mode**: How to handle dotfiles (Never show, Show as hidden, Show as system)
- **Dotfile Whitelist**: List of dotfile patterns to show (e.g., `.gitignore`, `.env*`, `.*.config`)
- **Always Hide**: List of filename patterns to always hide, even if they don't start with a dot (e.g., `desktop.ini`, `Thumbs.db`, `*.tmp`)

Examples:
- To show `.gitignore`: add `.gitignore` to dotfile whitelist
- To show all `.env*` files: add `.env*` to dotfile whitelist
- To hide `desktop.ini`: add `desktop.ini` to always-hide list
- To hide all `.tmp` files: add `*.tmp` to always-hide list
- `.env.local` will be shown if `.env*` is whitelisted

## Important Notes
- **No file attributes are modified** - hidden files retain their original attributes
- **Independent of Explorer settings** - works regardless of "Show hidden files" setting
- **Other applications** can still access these files normally
- **Command Prompt/PowerShell** will still show these files unless specifically filtered
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displayMode: neverShow
  $name: Display Mode
  $description: How to handle dotfiles in directory listings
  $options:
  - neverShow: Never show files (completely hidden)
  - showAsHidden: Show files as hidden (with hidden attribute)
  - showAsSystem: Show files as system (with hidden+system attributes)
- dotfileWhitelist: [""]
  $name: Dotfile Whitelist
  $description: List of dotfile patterns to show
- alwaysHide: [""]
  $name: Always Hide
  $description: List of filename patterns to always hide regardless of dotfile status
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <winternl.h>
#include <vector>
#include <string>
#include <algorithm>

enum class DisplayMode {
    NeverShow,
    ShowAsHidden,
    ShowAsSystem
};

struct {
    DisplayMode displayMode = DisplayMode::NeverShow;
    std::vector<std::wstring> dotfileWhitelist;
    std::vector<std::wstring> alwaysHide;
} g_settings;

typedef NTSTATUS (NTAPI* NtQueryDirectoryFile_t)(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    BOOLEAN ReturnSingleEntry,
    PUNICODE_STRING FileName,
    BOOLEAN RestartScan
);

NtQueryDirectoryFile_t NtQueryDirectoryFile_Original;

typedef NTSTATUS (NTAPI* NtQueryDirectoryFileEx_t)(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    ULONG QueryFlags,
    PUNICODE_STRING FileName
);

NtQueryDirectoryFileEx_t NtQueryDirectoryFileEx_Original;

void ParseSettings() {
    g_settings.dotfileWhitelist.clear();
    g_settings.alwaysHide.clear();
    
    PCWSTR displayModeStr = Wh_GetStringSetting(L"displayMode");
    if (wcscmp(displayModeStr, L"showAsHidden") == 0) {
        g_settings.displayMode = DisplayMode::ShowAsHidden;
    } else if (wcscmp(displayModeStr, L"showAsSystem") == 0) {
        g_settings.displayMode = DisplayMode::ShowAsSystem;
    } else {
        g_settings.displayMode = DisplayMode::NeverShow;
    }
    Wh_FreeStringSetting(displayModeStr);
    
    auto loadSettingList = [](const wchar_t* settingName, std::vector<std::wstring>& target) {
        for (int i = 0;; i++) {
            PCWSTR item = Wh_GetStringSetting(settingName, i);
            if (!*item) {
                Wh_FreeStringSetting(item);
                break;
            }
            target.emplace_back(item);
            Wh_FreeStringSetting(item);
        }
    };
    
    loadSettingList(L"dotfileWhitelist[%d]", g_settings.dotfileWhitelist);
    loadSettingList(L"alwaysHide[%d]", g_settings.alwaysHide);
}

bool ShouldHideFile(std::wstring_view fileName) noexcept {
    if (fileName.empty()) return false;
    
    if (fileName == L"." || fileName == L"..") {
        return false;
    }
    
    auto matchesPattern = [](std::wstring_view name, const std::vector<std::wstring>& patterns) noexcept -> bool {
        std::wstring temp(name);
        return std::ranges::any_of(patterns, [&temp](const std::wstring& pattern) {
            return PathMatchSpecW(temp.c_str(), pattern.c_str()) != FALSE;
        });
    };
    
    if (fileName[0] == L'.') {
        return !matchesPattern(fileName, g_settings.dotfileWhitelist);
    }
    
    return matchesPattern(fileName, g_settings.alwaysHide);
}

template<typename FileInfoType>
void FilterFilesInDirectory(void* FileInformation, ULONG_PTR* bytesReturned) noexcept {
    if (!FileInformation || !bytesReturned || *bytesReturned == 0) {
        return;
    }
    
    if (g_settings.displayMode == DisplayMode::NeverShow) {
        auto* currentEntry = static_cast<FileInfoType*>(FileInformation);
        auto* writeEntry = currentEntry;
        ULONG_PTR totalBytesRead = 0;
        ULONG_PTR totalBytesWritten = 0;
        
        const auto* const bufferEnd = reinterpret_cast<const BYTE*>(FileInformation) + *bytesReturned;
        
        while (totalBytesRead < *bytesReturned) {
            const ULONG nextEntryOffset = currentEntry->NextEntryOffset;
            const ULONG currentEntrySize = nextEntryOffset ? nextEntryOffset : (*bytesReturned - totalBytesRead);
            
            const ULONG fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
            const std::wstring_view fileName(currentEntry->FileName, fileNameLength);
            
            const bool shouldHide = fileNameLength > 0 && ShouldHideFile(fileName);
            
            if (!shouldHide) {
                if (writeEntry != currentEntry) {
                    std::memmove(writeEntry, currentEntry, currentEntrySize);
                }
                totalBytesWritten += currentEntrySize;
                writeEntry = reinterpret_cast<FileInfoType*>(reinterpret_cast<BYTE*>(writeEntry) + currentEntrySize);
            }
            
            totalBytesRead += currentEntrySize;
            
            if (nextEntryOffset == 0 || 
                reinterpret_cast<const BYTE*>(currentEntry) + nextEntryOffset >= bufferEnd) {
                break;
            }
            
            currentEntry = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<BYTE*>(currentEntry) + nextEntryOffset);
        }
        
        if (totalBytesWritten > 0) {
            auto* lastEntry = static_cast<FileInfoType*>(FileInformation);
            while (lastEntry->NextEntryOffset && 
                   reinterpret_cast<BYTE*>(lastEntry) + lastEntry->NextEntryOffset < 
                   reinterpret_cast<BYTE*>(FileInformation) + totalBytesWritten) {
                lastEntry = reinterpret_cast<FileInfoType*>(
                    reinterpret_cast<BYTE*>(lastEntry) + lastEntry->NextEntryOffset);
            }
            lastEntry->NextEntryOffset = 0;
        }
        
        *bytesReturned = totalBytesWritten;
    } else if constexpr (requires(FileInfoType t) { t.FileAttributes; }) {
            auto* currentEntry = static_cast<FileInfoType*>(FileInformation);
            ULONG_PTR totalBytesRead = 0;
            const auto* const bufferEnd = reinterpret_cast<const BYTE*>(FileInformation) + *bytesReturned;
            
            while (totalBytesRead < *bytesReturned) {
                const ULONG nextEntryOffset = currentEntry->NextEntryOffset;
                const ULONG currentEntrySize = nextEntryOffset ? nextEntryOffset : (*bytesReturned - totalBytesRead);
                
                const ULONG fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
                const std::wstring_view fileName(currentEntry->FileName, fileNameLength);
                
                if (fileNameLength > 0 && ShouldHideFile(fileName)) {
                    DWORD newAttributes = 0;
                    switch (g_settings.displayMode) {
                        case DisplayMode::ShowAsHidden:
                            newAttributes = FILE_ATTRIBUTE_HIDDEN;
                            break;
                        case DisplayMode::ShowAsSystem:
                            newAttributes = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
                            break;
                        default:
                            break;
                    }
                    
                    if (newAttributes != 0) {
                        currentEntry->FileAttributes |= newAttributes;
                    }
                }
                
                totalBytesRead += currentEntrySize;
                
                if (nextEntryOffset == 0 || 
                    reinterpret_cast<const BYTE*>(currentEntry) + nextEntryOffset >= bufferEnd) {
                    break;
                }
                
                currentEntry = reinterpret_cast<FileInfoType*>(
                    reinterpret_cast<BYTE*>(currentEntry) + nextEntryOffset);
            }
        }
}

void ProcessDirectoryListing(LPVOID FileInformation, FILE_INFORMATION_CLASS FileInformationClass, ULONG_PTR* bytesReturned) noexcept {
    switch (FileInformationClass) {
        case FileDirectoryInformation:
            FilterFilesInDirectory<FILE_DIRECTORY_INFORMATION>(FileInformation, bytesReturned);
            break;
        case FileFullDirectoryInformation:
            FilterFilesInDirectory<FILE_FULL_DIR_INFORMATION>(FileInformation, bytesReturned);
            break;
        case FileBothDirectoryInformation:
            FilterFilesInDirectory<FILE_BOTH_DIR_INFORMATION>(FileInformation, bytesReturned);
            break;
        case FileNamesInformation:
            FilterFilesInDirectory<FILE_NAMES_INFORMATION>(FileInformation, bytesReturned);
            break;
        case FileIdBothDirectoryInformation:
            FilterFilesInDirectory<FILE_ID_BOTH_DIR_INFORMATION>(FileInformation, bytesReturned);
            break;
        case FileIdFullDirectoryInformation:
            FilterFilesInDirectory<FILE_ID_FULL_DIR_INFORMATION>(FileInformation, bytesReturned);
            break;
        default:
            break;
    }
}

NTSTATUS NTAPI NtQueryDirectoryFile_Hook(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    BOOLEAN ReturnSingleEntry,
    PUNICODE_STRING FileName,
    BOOLEAN RestartScan) noexcept {
    
    const NTSTATUS status = NtQueryDirectoryFile_Original(
        FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
        FileInformation, Length, FileInformationClass,
        ReturnSingleEntry, FileName, RestartScan);
    
    if (NT_SUCCESS(status) && IoStatusBlock && FileInformation) {
        ULONG_PTR bytesReturned = IoStatusBlock->Information;
        ProcessDirectoryListing(FileInformation, FileInformationClass, &bytesReturned);
        IoStatusBlock->Information = bytesReturned;
    }
    
    return status;
}

NTSTATUS NTAPI NtQueryDirectoryFileEx_Hook(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    ULONG QueryFlags,
    PUNICODE_STRING FileName) noexcept {
    
    const NTSTATUS status = NtQueryDirectoryFileEx_Original(
        FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
        FileInformation, Length, FileInformationClass,
        QueryFlags, FileName);
    
    if (NT_SUCCESS(status) && IoStatusBlock && FileInformation) {
        ULONG_PTR bytesReturned = IoStatusBlock->Information;
        ProcessDirectoryListing(FileInformation, FileInformationClass, &bytesReturned);
        IoStatusBlock->Information = bytesReturned;
    }
    
    return status;
}

BOOL Wh_ModInit() {
    ParseSettings();
    
    const HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtDll) {
        return FALSE;
    }
    
    NtQueryDirectoryFile_Original = reinterpret_cast<NtQueryDirectoryFile_t>(
        GetProcAddress(hNtDll, "NtQueryDirectoryFile"));
    NtQueryDirectoryFileEx_Original = reinterpret_cast<NtQueryDirectoryFileEx_t>(
        GetProcAddress(hNtDll, "NtQueryDirectoryFileEx"));
    
    if (!NtQueryDirectoryFile_Original || !NtQueryDirectoryFileEx_Original) {
        return FALSE;
    }
    
    return Wh_SetFunctionHook(reinterpret_cast<void*>(NtQueryDirectoryFile_Original), 
                              reinterpret_cast<void*>(NtQueryDirectoryFile_Hook), 
                              reinterpret_cast<void**>(&NtQueryDirectoryFile_Original)) &&
           Wh_SetFunctionHook(reinterpret_cast<void*>(NtQueryDirectoryFileEx_Original), 
                              reinterpret_cast<void*>(NtQueryDirectoryFileEx_Hook), 
                              reinterpret_cast<void**>(&NtQueryDirectoryFileEx_Original));
}

void Wh_ModSettingsChanged() {
    ParseSettings();
}