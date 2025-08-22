// ==WindhawkMod==
// @id              hide-dotfiles-explorer
// @name            Hide Dotfiles (Explorer only) v1.0.1
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
- **Case-sensitive**: Filenames must match exactly including case (e.g., `.Gitignore` will not match `.gitignore`)
- **Exact matching**: Full filename must match exactly (no partial matches)

### Settings
- **Dotfile Whitelist**: Comma-separated list of dotfiles to show (e.g., `.gitignore,.env`)
- **Always Hide**: Comma-separated list of filenames to always hide, even if they don't start with a dot (e.g., `desktop.ini,Thumbs.db`)

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
- dotfileWhitelist: ""
  $name: Dotfile Whitelist
  $description: Comma-separated list of dotfiles to show
- alwaysHide: ""
  $name: Always Hide
  $description: Comma-separated list of filenames to always hide regardless of dotfile status
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <winternl.h>
#include <windhawk_utils.h>
#include <mutex>
#include <vector>
#include <string>
#include <algorithm>

std::mutex g_hookMutex;

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

typedef HRESULT (WINAPI* SHCreateShellFolderView_t)(LPCITEMIDLIST pidl, IShellFolderView** ppv);
SHCreateShellFolderView_t SHCreateShellFolderView_Original;

typedef HANDLE (WINAPI* FindFirstFileW_t)(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
FindFirstFileW_t FindFirstFileW_Original;

typedef BOOL (WINAPI* FindNextFileW_t)(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);
FindNextFileW_t FindNextFileW_Original;

HANDLE WINAPI FindFirstFileW_Hook(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
BOOL WINAPI FindNextFileW_Hook(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);

std::wstring TrimWhitespace(const std::wstring& str) {
    size_t start = str.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";
    size_t end = str.find_last_not_of(L" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::wstring ToLowerCase(const std::wstring& str) {
    std::wstring result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::towlower);
    return result;
}

std::wstring ParseQuotedString(const std::wstring& str) {
    if (str.length() >= 2 && str.front() == L'"' && str.back() == L'"') {
        return str.substr(1, str.length() - 2);
    }
    return str;
}

void ParseSettings(const std::wstring& dotfileWhitelistStr, const std::wstring& alwaysHideStr) {
    g_settings.dotfileWhitelist.clear();
    g_settings.alwaysHide.clear();
    
    auto parseList = [](const std::wstring& str, std::vector<std::wstring>& target) {
        if (str.empty()) return;
        
        std::wstring current;
        current.reserve(str.size());
        
        for (wchar_t c : str) {
            if (c == L',') {
                current = TrimWhitespace(current);
                if (!current.empty()) {
                    current = ParseQuotedString(current);
                    target.push_back(std::move(current));
                }
                current.clear();
            } else {
                current += c;
            }
        }
        
        current = TrimWhitespace(current);
        if (!current.empty()) {
            current = ParseQuotedString(current);
            target.push_back(std::move(current));
        }
    };
    
    parseList(dotfileWhitelistStr, g_settings.dotfileWhitelist);
    parseList(alwaysHideStr, g_settings.alwaysHide);
}

bool ShouldHideFile(const WCHAR* fullPath, const WCHAR* fileName) {
    if (!fileName || !fileName[0]) return false;
    
    if (fileName[0] == L'.') {
        // Case-insensitive comparison for dotfiles
        std::wstring fileNameLower(fileName);
        std::ranges::transform(fileNameLower, fileNameLower.begin(), ::towlower);
        
        return std::ranges::find_if(g_settings.dotfileWhitelist, 
            [&fileNameLower](const std::wstring& item) {
                std::wstring itemLower(item);
                std::ranges::transform(itemLower, itemLower.begin(), ::towlower);
                return itemLower == fileNameLower;
            }) == g_settings.dotfileWhitelist.end();
    }
    
    // Case-insensitive comparison for always hide
    std::wstring fileNameLower(fileName);
    std::ranges::transform(fileNameLower, fileNameLower.begin(), ::towlower);
    
    return std::ranges::find_if(g_settings.alwaysHide, 
        [&fileNameLower](const std::wstring& item) {
            std::wstring itemLower(item);
            std::ranges::transform(itemLower, itemLower.begin(), ::towlower);
            return itemLower == fileNameLower;
        }) != g_settings.alwaysHide.end();
}

thread_local WCHAR g_currentPath[MAX_PATH] = {0};

bool GetCurrentDirectoryFromHandle(HANDLE fileHandle) {
    WCHAR path[MAX_PATH] = {0};
    DWORD pathLen = GetFinalPathNameByHandleW(fileHandle, path, MAX_PATH, FILE_NAME_NORMALIZED);
    
    if (pathLen > 0 && pathLen < MAX_PATH) {
        if (wcsncmp(path, L"\\\\?\\\\?", 4) == 0) {
            wcscpy_s(g_currentPath, MAX_PATH, path + 4);
        } else {
            wcscpy_s(g_currentPath, MAX_PATH, path);
        }
        return true;
    }
    
    g_currentPath[0] = L'\0';
    return false;
}

class FileVisibilityFilterEnum : public IEnumIDList {
private:
    IEnumIDList* m_pOriginal;
    IShellFolder* m_pFolder;
    LONG m_cRef;

    bool ShouldHideItem(PCIDLIST_RELATIVE pidl) {
        if (!m_pFolder || !pidl) return false;
    
        STRRET strret;
        if (FAILED(m_pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING | SHGDN_INFOLDER, &strret))) {
            return false;
        }
        
        WCHAR szName[MAX_PATH];
        if (FAILED(StrRetToBufW(&strret, pidl, szName, MAX_PATH))) {
            return false;
        }
        
        WCHAR* pFileName = PathFindFileNameW(szName);
        if (!pFileName) {
            pFileName = szName;
        }
        
        if (wcscmp(pFileName, L".") == 0 || wcscmp(pFileName, L"..") == 0) {
            return false;
        }
        
        WCHAR fullPath[MAX_PATH] = {0};
        if (SUCCEEDED(m_pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &strret))) {
            StrRetToBufW(&strret, pidl, fullPath, MAX_PATH);

            std::replace(fullPath, fullPath + wcslen(fullPath), L'/', L'\\');
        }
        bool shouldHide = ShouldHideFile(fullPath, pFileName);
        return shouldHide;
    }

public:
    FileVisibilityFilterEnum(IEnumIDList* pOriginal, IShellFolder* pFolder) 
        : m_pOriginal(pOriginal), m_pFolder(pFolder), m_cRef(1) {
        if (m_pOriginal) m_pOriginal->AddRef();
        if (m_pFolder) m_pFolder->AddRef();
    }

    virtual ~FileVisibilityFilterEnum() {
        if (m_pOriginal) m_pOriginal->Release();
        if (m_pFolder) m_pFolder->Release();
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override {
        if (!ppvObject) return E_INVALIDARG;
        
        if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IEnumIDList)) {
            *ppvObject = static_cast<IEnumIDList*>(this);
            AddRef();
            return S_OK;
        }
        
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_cRef);
    }
    
    STDMETHODIMP_(ULONG) Release() override {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0) {
            delete this;
        }
        return cRef;
    }

    STDMETHODIMP Next(ULONG celt, LPITEMIDLIST* rgelt, ULONG* pceltFetched) override {
        if (!m_pOriginal) return E_FAIL;
        
        ULONG fetched = 0;
        ULONG totalFetched = 0;
        
        while (totalFetched < celt) {
            LPITEMIDLIST pidl = nullptr;
            HRESULT hr = m_pOriginal->Next(1, &pidl, &fetched);
            
            if (hr != S_OK || !pidl) {
                break;
            }
            
            if (!ShouldHideItem(pidl)) {
                rgelt[totalFetched] = pidl;
                totalFetched++;
            } else {
                CoTaskMemFree(pidl);
            }
        }
        
        if (pceltFetched) {
            *pceltFetched = totalFetched;
        }
        
        return (totalFetched == celt) ? S_OK : S_FALSE;
    }

    STDMETHODIMP Skip(ULONG celt) override {
        if (!m_pOriginal) return E_FAIL;
        
        ULONG skipped = 0;
        while (skipped < celt) {
            LPITEMIDLIST pidl = nullptr;
            ULONG fetched = 0;
            HRESULT hr = m_pOriginal->Next(1, &pidl, &fetched);
            
            if (hr != S_OK || !pidl) {
                break;
            }
            
            if (!ShouldHideItem(pidl)) {
                skipped++;
            }
            
            CoTaskMemFree(pidl);
        }
        
        return (skipped == celt) ? S_OK : S_FALSE;
    }

    STDMETHODIMP Reset() override {
        return m_pOriginal ? m_pOriginal->Reset() : E_FAIL;
    }

    STDMETHODIMP Clone(IEnumIDList** ppenum) override {
        if (!ppenum || !m_pOriginal) return E_INVALIDARG;
        
        IEnumIDList* pCloned = nullptr;
        HRESULT hr = m_pOriginal->Clone(&pCloned);
        if (SUCCEEDED(hr)) {
            *ppenum = new FileVisibilityFilterEnum(pCloned, m_pFolder);
            pCloned->Release();
        }
        
        return hr;
    }
};



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
        ULONG nextEntryOffset = 0;
        WCHAR* fileName = nullptr;
        ULONG fileNameLength = 0;
        
        if constexpr (std::is_same_v<FileInfoType, FILE_DIRECTORY_INFORMATION>) {
            nextEntryOffset = currentEntry->NextEntryOffset;
            fileName = currentEntry->FileName;
            fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
        } else if constexpr (std::is_same_v<FileInfoType, FILE_FULL_DIR_INFORMATION>) {
            nextEntryOffset = currentEntry->NextEntryOffset;
            fileName = currentEntry->FileName;
            fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
        } else if constexpr (std::is_same_v<FileInfoType, FILE_BOTH_DIR_INFORMATION>) {
            nextEntryOffset = currentEntry->NextEntryOffset;
            fileName = currentEntry->FileName;
            fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
        } else if constexpr (std::is_same_v<FileInfoType, FILE_NAMES_INFORMATION>) {
            nextEntryOffset = currentEntry->NextEntryOffset;
            fileName = currentEntry->FileName;
            fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
        } else if constexpr (std::is_same_v<FileInfoType, FILE_ID_BOTH_DIR_INFORMATION>) {
            nextEntryOffset = currentEntry->NextEntryOffset;
            fileName = currentEntry->FileName;
            fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
        } else if constexpr (std::is_same_v<FileInfoType, FILE_ID_FULL_DIR_INFORMATION>) {
            nextEntryOffset = currentEntry->NextEntryOffset;
            fileName = currentEntry->FileName;
            fileNameLength = currentEntry->FileNameLength / sizeof(WCHAR);
        }
        
        ULONG currentEntrySize = nextEntryOffset;
        if (nextEntryOffset == 0) {
            currentEntrySize = *bytesReturned - totalBytesRead;
        }
        
        bool shouldHide = false;
        if (fileName && fileNameLength > 0) {
            std::wstring fileNameStr(fileName, fileNameLength);
            
            if (fileNameStr != L"." && fileNameStr != L"..") {
                shouldHide = ShouldHideFile(g_currentPath, fileNameStr.c_str());
            }
        }
        
        if (!shouldHide) {
            if (writeEntry != currentEntry) {
                memmove(writeEntry, currentEntry, currentEntrySize);
            }
            
            if (nextEntryOffset == 0) {
                if constexpr (std::is_same_v<FileInfoType, FILE_DIRECTORY_INFORMATION>) {
                    writeEntry->NextEntryOffset = 0;
                } else if constexpr (std::is_same_v<FileInfoType, FILE_FULL_DIR_INFORMATION>) {
                    writeEntry->NextEntryOffset = 0;
                } else if constexpr (std::is_same_v<FileInfoType, FILE_BOTH_DIR_INFORMATION>) {
                    writeEntry->NextEntryOffset = 0;
                } else if constexpr (std::is_same_v<FileInfoType, FILE_NAMES_INFORMATION>) {
                    writeEntry->NextEntryOffset = 0;
                } else if constexpr (std::is_same_v<FileInfoType, FILE_ID_BOTH_DIR_INFORMATION>) {
                    writeEntry->NextEntryOffset = 0;
                } else if constexpr (std::is_same_v<FileInfoType, FILE_ID_FULL_DIR_INFORMATION>) {
                    writeEntry->NextEntryOffset = 0;
                }
            }
            
            totalBytesWritten += currentEntrySize;
            writeEntry = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<BYTE*>(writeEntry) + currentEntrySize);
        }
        
        totalBytesRead += currentEntrySize;
        
        if (nextEntryOffset == 0) {
            break;
        }
        
        currentEntry = reinterpret_cast<FileInfoType*>(
            reinterpret_cast<BYTE*>(currentEntry) + nextEntryOffset);
    }
    
    if (totalBytesWritten > 0) {
        FileInfoType* lastEntry = static_cast<FileInfoType*>(FileInformation);
        ULONG_PTR bytesProcessed = 0;
        
        while (bytesProcessed < totalBytesWritten) {
            ULONG nextOffset = 0;
            if constexpr (std::is_same_v<FileInfoType, FILE_DIRECTORY_INFORMATION>) {
                nextOffset = lastEntry->NextEntryOffset;
            } else if constexpr (std::is_same_v<FileInfoType, FILE_FULL_DIR_INFORMATION>) {
                nextOffset = lastEntry->NextEntryOffset;
            } else if constexpr (std::is_same_v<FileInfoType, FILE_BOTH_DIR_INFORMATION>) {
                nextOffset = lastEntry->NextEntryOffset;
            } else if constexpr (std::is_same_v<FileInfoType, FILE_NAMES_INFORMATION>) {
                nextOffset = lastEntry->NextEntryOffset;
            } else if constexpr (std::is_same_v<FileInfoType, FILE_ID_BOTH_DIR_INFORMATION>) {
                nextOffset = lastEntry->NextEntryOffset;
            } else if constexpr (std::is_same_v<FileInfoType, FILE_ID_FULL_DIR_INFORMATION>) {
                nextOffset = lastEntry->NextEntryOffset;
            }
            
            if (nextOffset == 0) {
                break;
            }
            
            bytesProcessed += nextOffset;
            lastEntry = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<BYTE*>(lastEntry) + nextOffset);
        }
        
        if constexpr (std::is_same_v<FileInfoType, FILE_DIRECTORY_INFORMATION>) {
            lastEntry->NextEntryOffset = 0;
        } else if constexpr (std::is_same_v<FileInfoType, FILE_FULL_DIR_INFORMATION>) {
            lastEntry->NextEntryOffset = 0;
        } else if constexpr (std::is_same_v<FileInfoType, FILE_BOTH_DIR_INFORMATION>) {
            lastEntry->NextEntryOffset = 0;
        } else if constexpr (std::is_same_v<FileInfoType, FILE_NAMES_INFORMATION>) {
            lastEntry->NextEntryOffset = 0;
        } else if constexpr (std::is_same_v<FileInfoType, FILE_ID_BOTH_DIR_INFORMATION>) {
            lastEntry->NextEntryOffset = 0;
        } else if constexpr (std::is_same_v<FileInfoType, FILE_ID_FULL_DIR_INFORMATION>) {
            lastEntry->NextEntryOffset = 0;
        }
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
        if (GetCurrentDirectoryFromHandle(FileHandle)) {
            ULONG_PTR bytesReturned = IoStatusBlock->Information;
            ProcessDirectoryListing(FileInformation, FileInformationClass, &bytesReturned);
            IoStatusBlock->Information = bytesReturned;
        }
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
        if (GetCurrentDirectoryFromHandle(FileHandle)) {
            ULONG_PTR bytesReturned = IoStatusBlock->Information;
            ProcessDirectoryListing(FileInformation, FileInformationClass, &bytesReturned);
            IoStatusBlock->Information = bytesReturned;
        }
    }
    
    return status;
}

void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_hookMutex);
    
    std::wstring dotfileWhitelistStr = Wh_GetStringSetting(L"dotfileWhitelist");
    std::wstring alwaysHideStr = Wh_GetStringSetting(L"alwaysHide");
    
    ParseSettings(dotfileWhitelistStr, alwaysHideStr);
}

BOOL Wh_ModInit() {
    std::wstring dotfileWhitelistStr = Wh_GetStringSetting(L"dotfileWhitelist");
    std::wstring alwaysHideStr = Wh_GetStringSetting(L"alwaysHide");
    ParseSettings(dotfileWhitelistStr, alwaysHideStr);
    
    HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtDll) {
        return FALSE;
    }
    
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) {
        return FALSE;
    }
    
    NtQueryDirectoryFile_Original = (NtQueryDirectoryFile_t)GetProcAddress(hNtDll, "NtQueryDirectoryFile");
    NtQueryDirectoryFileEx_Original = (NtQueryDirectoryFileEx_t)GetProcAddress(hNtDll, "NtQueryDirectoryFileEx");
    FindFirstFileW_Original = (FindFirstFileW_t)GetProcAddress(hKernel32, "FindFirstFileW");
    FindNextFileW_Original = (FindNextFileW_t)GetProcAddress(hKernel32, "FindNextFileW");
    
    if (!NtQueryDirectoryFile_Original || !NtQueryDirectoryFileEx_Original || 
        !FindFirstFileW_Original || !FindNextFileW_Original) {
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook((void*)NtQueryDirectoryFile_Original, (void*)NtQueryDirectoryFile_Hook, (void**)&NtQueryDirectoryFile_Original)) {
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook((void*)NtQueryDirectoryFileEx_Original, (void*)NtQueryDirectoryFileEx_Hook, (void**)&NtQueryDirectoryFileEx_Original)) {
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook((void*)FindFirstFileW_Original, (void*)FindFirstFileW_Hook, (void**)&FindFirstFileW_Original)) {
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook((void*)FindNextFileW_Original, (void*)FindNextFileW_Hook, (void**)&FindNextFileW_Original)) {
        return FALSE;
    }
    
    return TRUE;
}



HANDLE WINAPI FindFirstFileW_Hook(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) {
    HANDLE hFind = FindFirstFileW_Original(lpFileName, lpFindFileData);
    
    if (hFind != INVALID_HANDLE_VALUE && lpFindFileData) {
        WCHAR fileName[MAX_PATH];
        wcscpy_s(fileName, MAX_PATH, lpFindFileData->cFileName);
        
        if (ShouldHideFile(lpFileName, fileName)) {
            while (FindNextFileW_Original(hFind, lpFindFileData)) {
                wcscpy_s(fileName, MAX_PATH, lpFindFileData->cFileName);
                if (!ShouldHideFile(lpFileName, fileName)) {
                    return hFind;
                }
            }
            FindClose(hFind);
            return INVALID_HANDLE_VALUE;
        }
    }

    return hFind;
}

BOOL WINAPI FindNextFileW_Hook(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData) {
    BOOL result = FindNextFileW_Original(hFindFile, lpFindFileData);
    
    while (result && lpFindFileData) {
        WCHAR fileName[MAX_PATH];
        wcscpy_s(fileName, MAX_PATH, lpFindFileData->cFileName);
        
        if (!ShouldHideFile(nullptr, fileName)) {
            return TRUE;
        }
        
        result = FindNextFileW_Original(hFindFile, lpFindFileData);
    }
    
    return result;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}