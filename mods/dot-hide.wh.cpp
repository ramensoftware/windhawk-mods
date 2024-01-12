// ==WindhawkMod==
// @id              dot-hide
// @name            Dot Hide
// @description     Keep dot files and directories out of sight, Because it wouldn't happen in Linux...
// @version         1.0.0
// @author          Tomer Zait (realgam3)
// @github          https://github.com/realgam3
// @twitter         https://twitter.com/realgam3
// @homepage        https://realgam3.com/
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dot Hide
Dot Hide is a handy mod designed to bring the Linux-style hiding of dot files and directories to Windows.  
In Linux, files and directories starting with a dot ('.') are hidden by default, but this isn't the case in Windows.  
Dot Hide tackles this by automatically hiding these files and directories, creating a more streamlined and organized workspace.   

## Before
![Before](https://raw.githubusercontent.com/realgam3/dot-hide-wh/main/assets/img/before.png)

## After
![After](https://raw.githubusercontent.com/realgam3/dot-hide-wh/main/assets/img/after.png)

*/
// ==/WindhawkModReadme==

#include <string_view>
#include <winternl.h>
#include <windhawk_utils.h>

template<typename FileInfoType>
void HideFilesInDirectory(void* FileInformation) {
    FileInfoType* fileInformation = static_cast<FileInfoType *>(FileInformation);

    while (fileInformation) {
        std::wstring_view fileName(fileInformation->FileName, fileInformation->FileNameLength / sizeof(WCHAR));

        // fileName starts with . but not in [".", ".."]
        if (!(fileInformation->FileAttributes & FILE_ATTRIBUTE_HIDDEN) 
			&& fileName.length() >= 2 && fileName[0] == '.'
            && (fileName[1] != '.' || fileName.length() > 2)) {
            Wh_Log(L"Class->Hide: %.*s", static_cast<int>(fileName.length()), fileName.data());
            fileInformation->FileAttributes |= FILE_ATTRIBUTE_HIDDEN;
        }

        if (fileInformation->NextEntryOffset == 0) {
            break;
        }

        fileInformation = reinterpret_cast<FileInfoType *>(
            reinterpret_cast<LPBYTE>(fileInformation) + fileInformation->NextEntryOffset);
    }
}

void NtHideDirectoryFile(LPVOID FileInformation, FILE_INFORMATION_CLASS FileInformationClass) {
    switch (FileInformationClass) {
        case FileFullDirectoryInformation: {
            Wh_Log(L"Class: FileFullDirectoryInformation");
            HideFilesInDirectory<FILE_FULL_DIR_INFORMATION>(FileInformation);
            break;
        }

        case FileBothDirectoryInformation: {
            Wh_Log(L"Class: FileBothDirectoryInformation");
            HideFilesInDirectory<FILE_BOTH_DIR_INFORMATION>(FileInformation);
            break;
        }

        case FileIdBothDirectoryInformation: {
            Wh_Log(L"Class: FileIdBothDirectoryInformation");
            HideFilesInDirectory<FILE_ID_BOTH_DIR_INFORMATION>(FileInformation);
            break;
        }

        default: {
            Wh_Log(L"Class: ID-%d", FileInformationClass);
            break;
        }
    }
}

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

NTSTATUS NTAPI NtQueryDirectoryFile_Hook(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
                                          LPVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, LPVOID FileInformation,
                                          ULONG Length, FILE_INFORMATION_CLASS FileInformationClass,
                                          BOOLEAN ReturnSingleEntry, PUNICODE_STRING FileName, BOOLEAN RestartScan) {
    Wh_Log(L"NtQueryDirectoryFile_Hook called");

    NTSTATUS status = NtQueryDirectoryFile_Original(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
                                                    FileInformation, Length, FileInformationClass, ReturnSingleEntry,
                                                    FileName, RestartScan);
    if (NT_SUCCESS(status)) {
        NtHideDirectoryFile(FileInformation, FileInformationClass);
    }

    return status;
}

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

NTSTATUS NTAPI NtQueryDirectoryFileEx_Hook(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
                                            PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation,
                                            ULONG Length, FILE_INFORMATION_CLASS FileInformationClass, ULONG QueryFlags,
                                            PUNICODE_STRING FileName) {
    Wh_Log(L"NtQueryDirectoryFileEx_Hook called");

    NTSTATUS status = NtQueryDirectoryFileEx_Original(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
                                                      FileInformation, Length, FileInformationClass, QueryFlags,
                                                      FileName);
    if (NT_SUCCESS(status)) {
        NtHideDirectoryFile(FileInformation, FileInformationClass);
    }

    return status;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE ntdllModule = LoadLibrary(L"ntdll.dll");

    NtQueryDirectoryFile_t NtQueryDirectoryFile = (NtQueryDirectoryFile_t)GetProcAddress(
        ntdllModule, "NtQueryDirectoryFile");
    WindhawkUtils::Wh_SetFunctionHookT(NtQueryDirectoryFile, 
                                       NtQueryDirectoryFile_Hook,
                                       &NtQueryDirectoryFile_Original);

    NtQueryDirectoryFileEx_t NtQueryDirectoryFileEx = (NtQueryDirectoryFileEx_t)GetProcAddress(
        ntdllModule, "NtQueryDirectoryFileEx");
    WindhawkUtils::Wh_SetFunctionHookT(NtQueryDirectoryFileEx, 
                                       NtQueryDirectoryFileEx_Hook,
                                       &NtQueryDirectoryFileEx_Original);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
