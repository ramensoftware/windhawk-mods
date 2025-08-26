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

![Screenshot](https://i.imgur.com/3PYbQe7.png)

## How It Works
This mod **filters files directly from directory listings** at the system level.
It does **not** modify file attributes (like the "hidden" attribute) and **does not** depend on Windows Explorer's "Show hidden files" setting.
Files are completely hidden from view regardless of Explorer's hidden file visibility settings.

## Key Benefits
- **Independent of Explorer settings**: Works regardless of whether "Show hidden files" is enabled or disabled
- **No file attribute modification**: Files remain unchanged on disk
- **System-wide filtering**: Affects all Explorer views and file dialogs
- **Performance optimized**: Uses low-level NT API hooks for minimal overhead

## Configuration
You can exclude specific files from being hidden using the dotfile whitelist, or specify 
additional files to always hide regardless of whether they start with a dot.

### Filename Matching
- **Case-insensitive**: Filenames are matched regardless of case (e.g., `.Gitignore` will match `.gitignore`)
- **Exact matching**: Full filename must match exactly (no partial matches)

### Settings
- **Dotfile Whitelist**: List of dotfiles to show (e.g., `.gitignore`, `.env`)
- **Always Hide**: List of filenames to always hide, even if they don't start with a dot (e.g., `desktop.ini`, `Thumbs.db`)

Examples:
- To show `.gitignore`: add `.gitignore` to dotfile whitelist
- To hide `desktop.ini`: add `desktop.ini` to always-hide list
- `.env.local` will not be shown if only `.env` is whitelisted

## Important Notes
- **No file attributes are modified** - hidden files retain their original attributes
- **Independent of Explorer settings** - works regardless of "Show hidden files" setting
- **Other applications** can still access these files normally
- **Command Prompt/PowerShell** will still show these files unless specifically filtered
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dotfileWhitelist: [""]
  $name: Dotfile Whitelist
  $description: List of dotfiles to show
- alwaysHide: [""]
  $name: Always Hide
  $description: List of filenames to always hide regardless of dotfile status
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

struct {
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
    
    auto caseInsensitiveCompare = [](std::wstring_view lhs, std::wstring_view rhs) noexcept -> bool {
        return lhs.size() == rhs.size() && 
               std::ranges::equal(lhs, rhs, [](wchar_t a, wchar_t b) noexcept {
                   return ::towlower(a) == ::towlower(b);
               });
    };
    
    if (fileName[0] == L'.') {
        return !std::ranges::any_of(g_settings.dotfileWhitelist, 
            [fileName, &caseInsensitiveCompare](const std::wstring& item) {
                return caseInsensitiveCompare(fileName, std::wstring_view(item));
            });
    }
    
    return std::ranges::any_of(g_settings.alwaysHide, 
        [fileName, &caseInsensitiveCompare](const std::wstring& item) {
            return caseInsensitiveCompare(fileName, std::wstring_view(item));
        });
}



template<typename FileInfoType>
void FilterFilesInDirectory(void* FileInformation, ULONG_PTR* bytesReturned) noexcept {
    if (!FileInformation || !bytesReturned || *bytesReturned == 0) {
        return;
    }
    
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