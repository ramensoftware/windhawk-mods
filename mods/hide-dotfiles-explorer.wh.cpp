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
This mod hides files and folders starting with a dot (.) in Windows Explorer and Desktop, 
similar to Unix/Linux systems.

![Screenshot](https://i.imgur.com/3PYbQe7.png)

## How It Works
This mod **filters files directly from directory listings** at the system level. It does **not** modify file attributes (like the "hidden" attribute) and **does not** depend on Windows Explorer's "Show hidden files" setting. Files are completely hidden from view regardless of Explorer's hidden file visibility settings.

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
- dotfileWhitelist: []
  $name: Dotfile Whitelist
  $description: List of dotfiles to show
- alwaysHide: []
  $name: Always Hide
  $description: List of filenames to always hide regardless of dotfile status
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <winternl.h>
#include <windhawk_utils.h>
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
    
    // Parse dotfile whitelist
    for (int i = 0;; i++) {
        PCWSTR item = Wh_GetStringSetting(L"dotfileWhitelist[%d]", i);
        bool hasItem = *item;
        if (hasItem) {
            g_settings.dotfileWhitelist.emplace_back(item);
        }
        Wh_FreeStringSetting(item);
        if (!hasItem) {
            break;
        }
    }
    
    // Parse always hide list
    for (int i = 0;; i++) {
        PCWSTR item = Wh_GetStringSetting(L"alwaysHide[%d]", i);
        bool hasItem = *item;
        if (hasItem) {
            g_settings.alwaysHide.emplace_back(item);
        }
        Wh_FreeStringSetting(item);
        if (!hasItem) {
            break;
        }
    }
}

bool ShouldHideFile(const WCHAR* fileName) {
    if (!fileName || !fileName[0]) return false;
    
    if (wcscmp(fileName, L".") == 0 || wcscmp(fileName, L"..") == 0) {
        return false;
    }
    
    if (fileName[0] == L'.') {
        std::wstring fileNameLower(fileName);
        std::ranges::transform(fileNameLower, fileNameLower.begin(), ::towlower);
        
        return std::ranges::find_if(g_settings.dotfileWhitelist, 
            [&fileNameLower](const std::wstring& item) {
                std::wstring itemLower(item);
                std::ranges::transform(itemLower, itemLower.begin(), ::towlower);
                return itemLower == fileNameLower;
            }) == g_settings.dotfileWhitelist.end();
    }
    
    std::wstring fileNameLower(fileName);
    std::ranges::transform(fileNameLower, fileNameLower.begin(), ::towlower);
    
    return std::ranges::find_if(g_settings.alwaysHide, 
        [&fileNameLower](const std::wstring& item) {
            std::wstring itemLower(item);
            std::ranges::transform(itemLower, itemLower.begin(), ::towlower);
            return itemLower == fileNameLower;
        }) != g_settings.alwaysHide.end();
}



template<typename FileInfoType>
void FilterFilesInDirectory(void* FileInformation, ULONG_PTR* bytesReturned) {
    if (!FileInformation || !bytesReturned || *bytesReturned == 0) {
        return;
    }
    
    FileInfoType* currentEntry = static_cast<FileInfoType*>(FileInformation);
    FileInfoType* writeEntry = currentEntry;
    ULONG_PTR totalBytesRead = 0;
    ULONG_PTR totalBytesWritten = 0;
    
    while (totalBytesRead < *bytesReturned) {
        ULONG nextEntryOffset = currentEntry->NextEntryOffset;
        ULONG currentEntrySize = nextEntryOffset ? nextEntryOffset : (*bytesReturned - totalBytesRead);
        
        ULONG fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
        std::wstring fileName(currentEntry->FileName, fileNameLength);
        
        bool shouldHide = false;
        if (fileNameLength > 0 && fileName != L"." && fileName != L"..") {
            shouldHide = ShouldHideFile(fileName.c_str());
        }
        
        if (!shouldHide) {
            if (writeEntry != currentEntry) {
                memmove(writeEntry, currentEntry, currentEntrySize);
            }
            if (nextEntryOffset == 0) {
                writeEntry->NextEntryOffset = 0;
            }
            totalBytesWritten += currentEntrySize;
            writeEntry = reinterpret_cast<FileInfoType*>(reinterpret_cast<BYTE*>(writeEntry) + currentEntrySize);
        }
        
        totalBytesRead += currentEntrySize;
        if (nextEntryOffset == 0) break;
        
        currentEntry = reinterpret_cast<FileInfoType*>(
            reinterpret_cast<BYTE*>(currentEntry) + nextEntryOffset);
    }
    
    if (totalBytesWritten > 0) {
        FileInfoType* lastEntry = static_cast<FileInfoType*>(FileInformation);
        while (lastEntry->NextEntryOffset && 
               (reinterpret_cast<BYTE*>(lastEntry) + lastEntry->NextEntryOffset < 
                reinterpret_cast<BYTE*>(FileInformation) + totalBytesWritten)) {
            lastEntry = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<BYTE*>(lastEntry) + lastEntry->NextEntryOffset);
        }
        lastEntry->NextEntryOffset = 0;
    }
    
    *bytesReturned = totalBytesWritten;
}

void ProcessDirectoryListing(LPVOID FileInformation, FILE_INFORMATION_CLASS FileInformationClass, ULONG_PTR* bytesReturned) {
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
    BOOLEAN RestartScan) {
    
    NTSTATUS status = NtQueryDirectoryFile_Original(
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
    PUNICODE_STRING FileName) {
    
    NTSTATUS status = NtQueryDirectoryFileEx_Original(
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
    
    HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtDll) {
        return FALSE;
    }
    
    NtQueryDirectoryFile_Original = (NtQueryDirectoryFile_t)GetProcAddress(hNtDll, "NtQueryDirectoryFile");
    NtQueryDirectoryFileEx_Original = (NtQueryDirectoryFileEx_t)GetProcAddress(hNtDll, "NtQueryDirectoryFileEx");
    if (!NtQueryDirectoryFile_Original || !NtQueryDirectoryFileEx_Original) {
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook((void*)NtQueryDirectoryFile_Original, (void*)NtQueryDirectoryFile_Hook, (void**)&NtQueryDirectoryFile_Original)) {
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook((void*)NtQueryDirectoryFileEx_Original, (void*)NtQueryDirectoryFileEx_Hook, (void**)&NtQueryDirectoryFileEx_Original)) {
        return FALSE;
    }
    
    return TRUE;
}

void Wh_ModSettingsChanged() {
    ParseSettings();
}