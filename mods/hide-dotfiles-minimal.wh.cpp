// ==WindhawkMod==
// @id              hide-dotfiles-minimal
// @name            Hide Dotfiles Minimal
// @description     Hide files and folders starting with . in Windows Explorer and Desktop
// @version         1.0.0
// @author          USLTD
// @github          https://github.com/USLTD
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Dotfiles Minimal

Makes files and folders whose names start with `.` hidden in **Windows
Explorer**
(*and Desktop*), similar to Unix/Linux systems.

The mod does not remove directory entries from enumeration and does not modify
file attributes on disk. It only adds `FILE_ATTRIBUTE_HIDDEN` to the directory
information returned to **Windows Explorer**.

This means paths such as:

    C:\Users\<User>\.dotfile

remain accessible directly, and shell operations such as drag-and-drop and
archive extraction can still target those directories normally.

The behavior follows **Windows Explorer**'s hidden-file handling. If `Show
hidden files` is enabled in **Windows Explorer**, dotfiles will be visible.
*/
// ==/WindhawkModReadme==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>

#include <cstddef>
#include <string_view>

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

typedef NTSTATUS(NTAPI* NtQueryDirectoryFile_t)(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_opt_ PCUNICODE_STRING FileName,
    _In_ BOOLEAN RestartScan);

typedef NTSTATUS(NTAPI* NtQueryDirectoryFileEx_t)(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass,
    _In_ ULONG QueryFlags,
    _In_opt_ PCUNICODE_STRING FileName);

NtQueryDirectoryFile_t NtQueryDirectoryFile_Original = nullptr;
NtQueryDirectoryFileEx_t NtQueryDirectoryFileEx_Original = nullptr;

template <typename T>
void HideDotFiles(void* fileInformation, ULONG_PTR bytesReturned) noexcept {
    if (!fileInformation || bytesReturned == 0) {
        return;
    }

    auto* current = static_cast<T*>(fileInformation);

    const BYTE* bufferBegin = static_cast<const BYTE*>(fileInformation);

    const BYTE* bufferEnd = bufferBegin + bytesReturned;

    ULONG_PTR bytesRead = 0;

    constexpr size_t fileNameOffset = offsetof(T, FileName);

    while (bytesRead < bytesReturned) {
        const size_t remaining = static_cast<size_t>(bytesReturned - bytesRead);

        const ULONG nextEntryOffset = current->NextEntryOffset;

        // For all but the last entry, NextEntryOffset is the size
        // of this particular record. For the last entry, the record
        // occupies the remainder of the returned buffer.
        const size_t currentEntrySize =
            nextEntryOffset != 0 ? static_cast<size_t>(nextEntryOffset)
                                 : remaining;

        // Validate the current record before accessing FileName.
        if (currentEntrySize > remaining || currentEntrySize < fileNameOffset) {
            break;
        }

        // NextEntryOffset of a non-final entry must point beyond
        // the current record and remain inside the returned buffer.
        if (nextEntryOffset != 0) {
            if (nextEntryOffset < fileNameOffset ||
                nextEntryOffset > remaining) {
                break;
            }
        }

        // FileNameLength is measured in bytes and must be WCHAR-aligned.
        if ((current->FileNameLength & (sizeof(WCHAR) - 1)) != 0) {
            break;
        }

        // The filename must fit inside THIS directory record,
        // not merely somewhere in the remaining output buffer.
        if (current->FileNameLength > currentEntrySize - fileNameOffset) {
            break;
        }

        const size_t fileNameLength = current->FileNameLength / sizeof(WCHAR);

        if (fileNameLength != 0) {
            const std::wstring_view fileName(current->FileName, fileNameLength);

            // Do not hide the special "." and ".." entries.
            if (fileName.front() == L'.' && fileName != L"." &&
                fileName != L"..") {
                current->FileAttributes |= FILE_ATTRIBUTE_HIDDEN;
            }
        }

        bytesRead += currentEntrySize;

        if (nextEntryOffset == 0) {
            break;
        }

        BYTE* next = reinterpret_cast<BYTE*>(current) + nextEntryOffset;

        if (next <= bufferBegin || next >= bufferEnd) {
            break;
        }

        current = reinterpret_cast<T*>(next);
    }
}

void ProcessDirectoryListing(LPVOID fileInformation,
                             FILE_INFORMATION_CLASS informationClass,
                             ULONG_PTR bytesReturned) noexcept {
    switch (informationClass) {
        case FileDirectoryInformation:
            HideDotFiles<FILE_DIRECTORY_INFORMATION>(fileInformation,
                                                     bytesReturned);
            break;

        case FileFullDirectoryInformation:
            HideDotFiles<FILE_FULL_DIR_INFORMATION>(fileInformation,
                                                    bytesReturned);
            break;

        case FileBothDirectoryInformation:
            HideDotFiles<FILE_BOTH_DIR_INFORMATION>(fileInformation,
                                                    bytesReturned);
            break;

        case FileIdBothDirectoryInformation:
            HideDotFiles<FILE_ID_BOTH_DIR_INFORMATION>(fileInformation,
                                                       bytesReturned);
            break;

        case FileIdFullDirectoryInformation:
            HideDotFiles<FILE_ID_FULL_DIR_INFORMATION>(fileInformation,
                                                       bytesReturned);
            break;

        case FileNamesInformation:
            // FILE_NAMES_INFORMATION does not contain FileAttributes,
            // so there is no safe in-place way to mark the entry hidden
            // without changing the requested information class or
            // removing/repacking entries.
            break;

        default:
            break;
    }
}

NTSTATUS NTAPI
NtQueryDirectoryFile_Hook(HANDLE FileHandle,
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
        FileInformation, Length, FileInformationClass, ReturnSingleEntry,
        FileName, RestartScan);

    // Do not inspect the output buffer for asynchronous operations.
    // STATUS_PENDING must pass through untouched.
    if (status != STATUS_SUCCESS || IoStatusBlock == nullptr ||
        FileInformation == nullptr) {
        return status;
    }

    const ULONG_PTR bytesReturned =
        static_cast<ULONG_PTR>(IoStatusBlock->Information);

    if (bytesReturned == 0 || bytesReturned > static_cast<ULONG_PTR>(Length)) {
        return status;
    }

    ProcessDirectoryListing(FileInformation, FileInformationClass,
                            bytesReturned);

    return status;
}

NTSTATUS NTAPI
NtQueryDirectoryFileEx_Hook(HANDLE FileHandle,
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
        FileInformation, Length, FileInformationClass, QueryFlags, FileName);

    // Do not inspect the output buffer for asynchronous operations.
    // STATUS_PENDING must pass through untouched.
    if (status != STATUS_SUCCESS || IoStatusBlock == nullptr ||
        FileInformation == nullptr) {
        return status;
    }

    const ULONG_PTR bytesReturned =
        static_cast<ULONG_PTR>(IoStatusBlock->Information);

    if (bytesReturned == 0 || bytesReturned > static_cast<ULONG_PTR>(Length)) {
        return status;
    }

    ProcessDirectoryListing(FileInformation, FileInformationClass,
                            bytesReturned);

    return status;
}

BOOL Wh_ModInit() {
    HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");

    if (!hNtDll) {
        Wh_Log(L"Failed to get ntdll.dll handle");
        return FALSE;
    }

    NtQueryDirectoryFile_Original = reinterpret_cast<NtQueryDirectoryFile_t>(
        GetProcAddress(hNtDll, "NtQueryDirectoryFile"));

    NtQueryDirectoryFileEx_Original =
        reinterpret_cast<NtQueryDirectoryFileEx_t>(
            GetProcAddress(hNtDll, "NtQueryDirectoryFileEx"));

    if (!NtQueryDirectoryFile_Original || !NtQueryDirectoryFileEx_Original) {
        Wh_Log(L"Failed to locate NtQueryDirectoryFile functions");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(NtQueryDirectoryFile_Original),
            reinterpret_cast<void*>(NtQueryDirectoryFile_Hook),
            reinterpret_cast<void**>(&NtQueryDirectoryFile_Original))) {
        Wh_Log(L"Failed to hook NtQueryDirectoryFile");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(NtQueryDirectoryFileEx_Original),
            reinterpret_cast<void*>(NtQueryDirectoryFileEx_Hook),
            reinterpret_cast<void**>(&NtQueryDirectoryFileEx_Original))) {
        Wh_Log(L"Failed to hook NtQueryDirectoryFileEx");
        return FALSE;
    }

    return TRUE;
}
