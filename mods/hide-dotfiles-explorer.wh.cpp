// ==WindhawkMod==
// @id              hide-dotfiles-explorer
// @name            Hide Dotfiles (Explorer only) v1.0
// @description     Hide dotfiles and folders starting with . in Windows Explorer and Desktop
// @version         1.0.10
// @author          @danalec
// @github          https://github.com/danalec
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod hides files and folders starting with a dot (.) in Windows Explorer and Desktop, 
similar to Unix/Linux systems.

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
#include <atomic>
#include <vector>
#include <string>
#include <algorithm>

const GUID IID_IUnknown_Local = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const GUID IID_IEnumIDList_Local = {0x000214F2, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

typedef HRESULT (WINAPI *IShellFolder_EnumObjects_t)(void* pThis, HWND hwnd, SHCONTF grfFlags, IEnumIDList** ppenumIDList);
IShellFolder_EnumObjects_t IShellFolder_EnumObjects_Original;

std::mutex g_hookMutex;
std::atomic<bool> g_debugLogging{false};

struct {
    std::vector<std::wstring> dotfileWhitelist;
    std::vector<std::wstring> alwaysHide;
    bool debugMode;
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

std::wstring TrimWhitespace(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::wstring ToLowerCase(const std::wstring& str) {
    std::wstring result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::towlower);
    return result;
}

void ParseSettings() {
    g_settings.dotfileWhitelist.clear();
    g_settings.alwaysHide.clear();
    
    const WCHAR* dotfileWhitelist = Wh_GetStringSetting(L"dotfileWhitelist");
    if (dotfileWhitelist && *dotfileWhitelist) {
        std::wstring whitelistStr(dotfileWhitelist);
        size_t start = 0;
        size_t end = 0;
        
        while ((end = whitelistStr.find(L',', start)) != std::wstring::npos) {
            std::wstring item = TrimWhitespace(whitelistStr.substr(start, end - start));
            if (!item.empty()) {
                g_settings.dotfileWhitelist.push_back(item);
            }
            start = end + 1;
        }
        
        std::wstring item = TrimWhitespace(whitelistStr.substr(start));
        if (!item.empty()) {
            g_settings.dotfileWhitelist.push_back(item);
        }
    }
    Wh_FreeStringSetting(dotfileWhitelist);
    
    const WCHAR* alwaysHide = Wh_GetStringSetting(L"alwaysHide");
    if (alwaysHide && *alwaysHide) {
        std::wstring hideStr(alwaysHide);
        size_t start = 0;
        size_t end = 0;
        
        while ((end = hideStr.find(L',', start)) != std::wstring::npos) {
            std::wstring item = TrimWhitespace(hideStr.substr(start, end - start));
            if (!item.empty()) {
                g_settings.alwaysHide.push_back(item);
            }
            start = end + 1;
        }
        
        std::wstring item = TrimWhitespace(hideStr.substr(start));
        if (!item.empty()) {
            g_settings.alwaysHide.push_back(item);
        }
    }
    Wh_FreeStringSetting(alwaysHide);
}

bool ShouldHideFile(const WCHAR* fullPath, const WCHAR* fileName) {
    if (!fileName || !fileName[0]) return false;
    
    if (fileName[0] == L'.') {
        return std::ranges::find(g_settings.dotfileWhitelist, fileName) == g_settings.dotfileWhitelist.end();
    }
    
    return std::ranges::find(g_settings.alwaysHide, fileName) != g_settings.alwaysHide.end();
}

WCHAR g_currentPath[MAX_PATH] = {0};

void GetCurrentDirectoryFromHandle(HANDLE fileHandle) {
    WCHAR path[MAX_PATH] = {0};
    DWORD pathLen = GetFinalPathNameByHandleW(fileHandle, path, MAX_PATH, FILE_NAME_NORMALIZED);
    
    if (pathLen > 0 && pathLen < MAX_PATH) {
        if (wcsncmp(path, L"\\\\?\\", 4) == 0) {
            wcscpy_s(g_currentPath, MAX_PATH, path + 4);
        } else {
            wcscpy_s(g_currentPath, MAX_PATH, path);
        }
    }
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

    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (!ppvObject) return E_INVALIDARG;
        
        if (IsEqualIID(riid, IID_IUnknown_Local) || IsEqualIID(riid, IID_IEnumIDList_Local)) {
            *ppvObject = static_cast<IEnumIDList*>(this);
            AddRef();
            return S_OK;
        }
        
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() {
        return InterlockedIncrement(&m_cRef);
    }
    
    STDMETHODIMP_(ULONG) Release() {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0) {
            delete this;
        }
        return cRef;
    }

    STDMETHODIMP Next(ULONG celt, LPITEMIDLIST* rgelt, ULONG* pceltFetched) {
        if (!m_pOriginal || !rgelt) return E_FAIL;
        if (celt == 0) return S_OK;
        
        ULONG fetched = 0;
        
        while (fetched < celt) {
            LPITEMIDLIST pidl = nullptr;
            ULONG itemsFetched = 0;
            
            HRESULT hr = m_pOriginal->Next(1, &pidl, &itemsFetched);
            if (hr != S_OK || itemsFetched == 0 || !pidl) {
                break;
            }
            
            if (!ShouldHideItem(pidl)) {
                rgelt[fetched] = pidl;
                fetched++;
            } else {
                CoTaskMemFree(pidl);
            }
        }
        
        if (pceltFetched) {
            *pceltFetched = fetched;
        }
        
        return (fetched == celt) ? S_OK : S_FALSE;
    }

    STDMETHODIMP Skip(ULONG celt) {
        if (!m_pOriginal) return E_FAIL;
        
        ULONG skipped = 0;
        
        while (skipped < celt) {
            LPITEMIDLIST pidl = nullptr;
            ULONG itemsFetched = 0;
            
            HRESULT hr = m_pOriginal->Next(1, &pidl, &itemsFetched);
            if (hr != S_OK || itemsFetched == 0 || !pidl) {
                return S_FALSE;
            }
            
            if (!ShouldHideItem(pidl)) {
                skipped++;
            }
            
            CoTaskMemFree(pidl);
        }
        
        return S_OK;
    }
    
    STDMETHODIMP Reset() {
        return m_pOriginal ? m_pOriginal->Reset() : E_FAIL;
    }
    
    STDMETHODIMP Clone(IEnumIDList** ppenum) {
        if (!m_pOriginal || !ppenum) return E_FAIL;
        
        IEnumIDList* pCloned = nullptr;
        HRESULT hr = m_pOriginal->Clone(&pCloned);
        
        if (SUCCEEDED(hr) && pCloned) {
            *ppenum = new FileVisibilityFilterEnum(pCloned, m_pFolder);
            pCloned->Release();
            return S_OK;
        }
        
        return hr;
    }
};

HRESULT WINAPI IShellFolder_EnumObjects_Hook(
    void* pThis,
    HWND hwnd,
    SHCONTF grfFlags,
    IEnumIDList** ppenumIDList)
{
    HRESULT hr = IShellFolder_EnumObjects_Original(pThis, hwnd, grfFlags, ppenumIDList);
    
    if (SUCCEEDED(hr) && ppenumIDList && *ppenumIDList) {
        IShellFolder* pFolder = static_cast<IShellFolder*>(pThis);
        IEnumIDList* pOriginal = *ppenumIDList;
        *ppenumIDList = new FileVisibilityFilterEnum(pOriginal, pFolder);
        pOriginal->Release();
    }
    
    return hr;
}

template<typename FileInfoType>
void FilterFilesInDirectory(void* FileInformation, ULONG_PTR* bytesReturned) {
    FileInfoType* current = static_cast<FileInfoType*>(FileInformation);
    FileInfoType* previous = nullptr;
    FileInfoType* writePos = current;
    ULONG totalBytes = *bytesReturned;
    ULONG bytesWritten = 0;
    
    while (current && bytesWritten < totalBytes) {
        std::wstring_view fileName(current->FileName, current->FileNameLength / sizeof(WCHAR));
        
        if (fileName == L"." || fileName == L"..") {
            ULONG entrySize = current->NextEntryOffset ? current->NextEntryOffset : 
                (ULONG)((BYTE*)current - (BYTE*)writePos + sizeof(FILE_BOTH_DIR_INFORMATION) + current->FileNameLength);
            
            if (writePos != current) {
                memmove(writePos, current, entrySize);
            }
            
            bytesWritten += entrySize;
            writePos = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<LPBYTE>(writePos) + entrySize);
            
            if (current->NextEntryOffset == 0) {
                break;
            }
            
            current = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<LPBYTE>(current) + current->NextEntryOffset);
            continue;
        }
        
        WCHAR fullPath[MAX_PATH] = {0};
        if (g_currentPath[0]) {
            swprintf_s(fullPath, MAX_PATH, L"%s\\%.*s", 
                g_currentPath, 
                static_cast<int>(fileName.length()), 
                fileName.data());
            WCHAR absolutePath[MAX_PATH];
            if (GetFullPathNameW(fullPath, MAX_PATH, absolutePath, nullptr)) {
                wcscpy_s(fullPath, MAX_PATH, absolutePath);
            }
        }
        
        std::wstring fileNameStr(fileName);
        if (ShouldHideFile(fullPath[0] ? fullPath : nullptr, fileNameStr.c_str())) {
            if (current->NextEntryOffset == 0) {
                if (previous) {
                    previous->NextEntryOffset = 0;
                }
                break;
            }
            
            current = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<LPBYTE>(current) + current->NextEntryOffset);
            continue;
        } else {
            
            ULONG entrySize = current->NextEntryOffset ? current->NextEntryOffset : 
                (ULONG)((BYTE*)current - (BYTE*)writePos + sizeof(FILE_BOTH_DIR_INFORMATION) + current->FileNameLength);
            
            if (writePos != current) {
                memmove(writePos, current, entrySize);
            }
            
            bytesWritten += entrySize;
            writePos = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<LPBYTE>(writePos) + entrySize);
            previous = writePos;
            
            if (current->NextEntryOffset == 0) {
                break;
            }
            
            current = reinterpret_cast<FileInfoType*>(
                reinterpret_cast<LPBYTE>(current) + current->NextEntryOffset);
        }
    }
    
    *bytesReturned = bytesWritten;
}

void ProcessDirectoryListing(LPVOID FileInformation, FILE_INFORMATION_CLASS FileInformationClass, ULONG_PTR* bytesReturned) {
    switch (FileInformationClass) {
        case FileFullDirectoryInformation: {
            FilterFilesInDirectory<FILE_FULL_DIR_INFORMATION>(FileInformation, bytesReturned);
            break;
        }
        case FileBothDirectoryInformation: {
            FilterFilesInDirectory<FILE_BOTH_DIR_INFORMATION>(FileInformation, bytesReturned);
            break;
        }
        case FileIdBothDirectoryInformation: {
            FilterFilesInDirectory<FILE_ID_BOTH_DIR_INFORMATION>(FileInformation, bytesReturned);
            break;
        }
        case FileIdFullDirectoryInformation: {
            FilterFilesInDirectory<FILE_ID_FULL_DIR_INFORMATION>(FileInformation, bytesReturned);
            break;
        }
        case FileDirectoryInformation: {
            FilterFilesInDirectory<FILE_DIRECTORY_INFORMATION>(FileInformation, bytesReturned);
            break;
        }
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
    
    GetCurrentDirectoryFromHandle(FileHandle);
    
    NTSTATUS status = NtQueryDirectoryFile_Original(
        FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
        FileInformation, Length, FileInformationClass, ReturnSingleEntry,
        FileName, RestartScan);
    
    if (NT_SUCCESS(status) && FileInformation && IoStatusBlock) {
        ProcessDirectoryListing(FileInformation, FileInformationClass, &IoStatusBlock->Information);
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
    
    GetCurrentDirectoryFromHandle(FileHandle);
    
    NTSTATUS status = NtQueryDirectoryFileEx_Original(
        FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock,
        FileInformation, Length, FileInformationClass, QueryFlags, FileName);
    
    if (NT_SUCCESS(status) && FileInformation && IoStatusBlock) {
        ProcessDirectoryListing(FileInformation, FileInformationClass, &IoStatusBlock->Information);
    }
    
    return status;
}

void LoadSettings() {
    ParseSettings();
    
    g_settings.debugMode = Wh_GetIntSetting(L"DebugMode", 0) != 0;
    g_debugLogging = g_settings.debugMode;
}

BOOL Wh_ModInit() {
    LoadSettings();
    
    IShellFolder* pDesktop = nullptr;
    HRESULT hr = SHGetDesktopFolder(&pDesktop);
    if (SUCCEEDED(hr) && pDesktop) {
        void** vtable = *(void***)pDesktop;
        if (vtable && vtable[4]) { 
            Wh_SetFunctionHook(
                vtable[4],
                (void*)IShellFolder_EnumObjects_Hook,
                (void**)&IShellFolder_EnumObjects_Original
            );
        }
        pDesktop->Release();
    }
    
    HMODULE ntdllModule = GetModuleHandle(L"ntdll.dll");
    if (ntdllModule) {
        NtQueryDirectoryFile_t NtQueryDirectoryFile = (NtQueryDirectoryFile_t)GetProcAddress(
            ntdllModule, "NtQueryDirectoryFile");
        if (NtQueryDirectoryFile) {
            Wh_SetFunctionHook(
                (void*)NtQueryDirectoryFile,
                (void*)NtQueryDirectoryFile_Hook,
                (void**)&NtQueryDirectoryFile_Original
            );
        }
        
        NtQueryDirectoryFileEx_t NtQueryDirectoryFileEx = (NtQueryDirectoryFileEx_t)GetProcAddress(
            ntdllModule, "NtQueryDirectoryFileEx");
        if (NtQueryDirectoryFileEx) {
            Wh_SetFunctionHook(
                (void*)NtQueryDirectoryFileEx,
                (void*)NtQueryDirectoryFileEx_Hook,
                (void**)&NtQueryDirectoryFileEx_Original
            );
        }
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}