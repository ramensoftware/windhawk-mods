// ==WindhawkMod==
// @id              hide-dotfiles-explorer
// @name            Hide Dotfiles (Explorer Only)
// @description     Hide dotfiles (files starting with .) in Windows Explorer only. Configure excluded paths in settings.
// @version         1.0
// @author          @danalec
// @github          https://github.com/danalec
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod automatically hides files and folders that start with a dot (.) in Windows Explorer, similar to Unix/Linux systems.

## Features
- Hides all files/folders starting with "." (like .git, .vscode, .env, etc.)
- Works only in Windows Explorer (doesn't affect file dialogs or other applications)
- Configurable exclusion paths where dotfiles will remain visible

## Configuration
You can exclude specific paths, folders, or files from having their dotfiles hidden.
In the mod settings, add paths and/or files separated by commas:
- Exclude all dotfiles in a directory: `C:\Projects`
- Exclude specific dotfile by full path: `C:\Projects\.gitignore`
- Exclude specific dotfile everywhere: `.gitignore`
- Multiple exclusions: `.gitignore, .env, C:\Projects`
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- excludePaths: ""
  $name: Exclude
  $description: Paths/Files where dotfiles will remain visible (comma-separated, e.g. ".gitignore, .env, C:\Projects")
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propsys.h>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

const GUID IID_IShellFolder = {0x000214E6, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const GUID IID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const GUID IID_IEnumIDList = {0x000214F2, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const GUID IID_IPersistFolder2 = {0x1AC3D9F0, 0x175C, 0x11d1, {0x95, 0xBE, 0x00, 0x60, 0x97, 0x97, 0xEA, 0x4F}};

typedef HRESULT (WINAPI *SHCreateShellFolderView_t)(const SFV_CREATE* pcsfv, IShellView** ppsv);
typedef HRESULT (WINAPI *SHCreateDefaultExtractIcon_t)(REFIID riid, void** ppv);
typedef HRESULT (STDMETHODCALLTYPE *IShellFolder_EnumObjects_t)(IShellFolder* pThis, HWND hwnd, SHCONTF grfFlags, IEnumIDList** ppenumIDList);
typedef HRESULT (STDMETHODCALLTYPE *IShellFolder_CreateViewObject_t)(IShellFolder* pThis, HWND hwndOwner, REFIID riid, void** ppv);

SHCreateShellFolderView_t SHCreateShellFolderView_Original;
SHCreateDefaultExtractIcon_t SHCreateDefaultExtractIcon_Original;
std::unordered_set<void*> g_hookedVTables;

std::vector<std::wstring> g_excludePaths;

void ParseExcludePaths(const WCHAR* paths) {
    g_excludePaths.clear();
    if (!paths || !*paths) return;
    std::wstring input(paths);
    std::wstring current_path;
    bool in_quotes = false;
    for (wchar_t ch : input) {
        if (ch == L'"') {
            in_quotes = !in_quotes;
            continue;
        }
        if (ch == L',' && !in_quotes) {
            size_t first = current_path.find_first_not_of(L" \t");
            if (std::wstring::npos != first) {
                size_t last = current_path.find_last_not_of(L" \t");
                g_excludePaths.push_back(current_path.substr(first, (last - first + 1)));
            }
            current_path.clear();
        } else {
            current_path += ch;
        }
    }
    size_t first = current_path.find_first_not_of(L" \t");
    if (std::wstring::npos != first) {
        size_t last = current_path.find_last_not_of(L" \t");
        g_excludePaths.push_back(current_path.substr(first, (last - first + 1)));
    }
}

bool IsPathExcluded(PCWSTR path) {
    if (!path || g_excludePaths.empty()) return false;
    std::wstring fullPath(path);
    std::transform(fullPath.begin(), fullPath.end(), fullPath.begin(), ::towlower);
    PathRemoveBackslashW(&fullPath[0]);
    fullPath.resize(wcslen(fullPath.c_str()));
    
    for (const auto& excludePath : g_excludePaths) {
        std::wstring excl = excludePath;
        std::transform(excl.begin(), excl.end(), excl.begin(), ::towlower);
        PathRemoveBackslashW(&excl[0]);
        excl.resize(wcslen(excl.c_str()));
        
        if (fullPath == excl) {
            return true;
        }
        if (fullPath.find(excl, 0) == 0) {
            if (fullPath.length() > excl.length() && fullPath[excl.length()] == L'\\') {
                return true;
            }
        }
    }
    return false;
}

class DotfileFilterEnum : public IEnumIDList {
private:
    IEnumIDList* m_pOriginal;
    IShellFolder* m_pFolder;
    LONG m_cRef;
public:
    DotfileFilterEnum(IEnumIDList* pOriginal, IShellFolder* pFolder) : m_pOriginal(pOriginal), m_pFolder(pFolder), m_cRef(1) {
        if (m_pOriginal) m_pOriginal->AddRef();
        if (m_pFolder) m_pFolder->AddRef();
    }
    ~DotfileFilterEnum() {
        if (m_pOriginal) m_pOriginal->Release();
        if (m_pFolder) m_pFolder->Release();
    }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (!ppvObject) return E_INVALIDARG;
        *ppvObject = nullptr;
        if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IEnumIDList)) {
            *ppvObject = static_cast<IEnumIDList*>(this);
            AddRef();
            return S_OK;
        }
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
        if (!celt) return S_OK;
        ULONG fetched = 0;
        if (pceltFetched) *pceltFetched = 0;
        while (fetched < celt) {
            LPITEMIDLIST pidl = nullptr;
            ULONG itemsFetched = 0;
            HRESULT hr = m_pOriginal->Next(1, &pidl, &itemsFetched);
            if (hr != S_OK || itemsFetched == 0) {
                break;
            }
            bool shouldInclude = true;
            LPWSTR pszFullPath = nullptr;
            PIDLIST_ABSOLUTE pidlFolder = nullptr;
            IPersistFolder2* pPersist = nullptr;
            if (SUCCEEDED(m_pFolder->QueryInterface(IID_IPersistFolder2, (void**)&pPersist))) {
                if (SUCCEEDED(pPersist->GetCurFolder(&pidlFolder))) {
                    PIDLIST_ABSOLUTE pidlAbs = ILCombine(pidlFolder, pidl);
                    if (pidlAbs) {
                        if (SUCCEEDED(SHGetNameFromIDList(pidlAbs, SIGDN_FILESYSPATH, &pszFullPath))) {
                            PCWSTR pFileName = PathFindFileNameW(pszFullPath);
                            if (pFileName && pFileName[0] == L'.') {
                                shouldInclude = false;
                                if (IsPathExcluded(pszFullPath) || IsPathExcluded(pFileName)) {
                                    shouldInclude = true;
                                }
                            }
                        }
                        CoTaskMemFree(pidlAbs);
                    }
                    CoTaskMemFree(pidlFolder);
                }
                pPersist->Release();
            }
            if (pszFullPath) CoTaskMemFree(pszFullPath);
            if (shouldInclude) {
                rgelt[fetched++] = pidl;
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
        return m_pOriginal->Skip(celt);
    }
    STDMETHODIMP Reset() {
        return m_pOriginal ? m_pOriginal->Reset() : E_FAIL;
    }
    STDMETHODIMP Clone(IEnumIDList** ppenum) {
        if (!ppenum) return E_INVALIDARG;
        *ppenum = nullptr;
        if (!m_pOriginal) return E_FAIL;
        IEnumIDList* pClonedOriginal = nullptr;
        HRESULT hr = m_pOriginal->Clone(&pClonedOriginal);
        if (SUCCEEDED(hr) && pClonedOriginal) {
            DotfileFilterEnum* pClonedWrapper = new (std::nothrow) DotfileFilterEnum(pClonedOriginal, m_pFolder);
            pClonedOriginal->Release();
            if (pClonedWrapper) {
                *ppenum = pClonedWrapper;
                return S_OK;
            }
            return E_OUTOFMEMORY;
        }
        return hr;
    }
};

IShellFolder_EnumObjects_t* GetEnumObjectsPtr(IShellFolder* pFolder) {
    void** vtable = *(void***)pFolder;
    return (IShellFolder_EnumObjects_t*)&vtable[4];
}

IShellFolder_CreateViewObject_t* GetCreateViewObjectPtr(IShellFolder* pFolder) {
    void** vtable = *(void***)pFolder;
    return (IShellFolder_CreateViewObject_t*)&vtable[7];
}

HRESULT STDMETHODCALLTYPE IShellFolder_EnumObjects_Hook(IShellFolder* pThis, HWND hwnd, SHCONTF grfFlags, IEnumIDList** ppenumIDList) {
    IShellFolder_EnumObjects_t pfnOriginal = *GetEnumObjectsPtr(pThis);
    HRESULT hr = pfnOriginal(pThis, hwnd, grfFlags, ppenumIDList);
    if (SUCCEEDED(hr) && ppenumIDList && *ppenumIDList) {
        IEnumIDList* pOriginal = *ppenumIDList;
        DotfileFilterEnum* pFiltered = new (std::nothrow) DotfileFilterEnum(pOriginal, pThis);
        if (pFiltered) {
            *ppenumIDList = pFiltered;
            pOriginal->Release();
        } else {
            pOriginal->Release();
            *ppenumIDList = nullptr;
            hr = E_OUTOFMEMORY;
        }
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE IShellFolder_CreateViewObject_Hook(IShellFolder* pThis, HWND hwndOwner, REFIID riid, void** ppv) {
    IShellFolder_CreateViewObject_t pfnOriginal = *GetCreateViewObjectPtr(pThis);
    HRESULT hr = pfnOriginal(pThis, hwndOwner, riid, ppv);
    if (SUCCEEDED(hr) && ppv && *ppv && IsEqualIID(riid, IID_IShellView)) {
        void** vtable = *(void***)pThis;
        if (g_hookedVTables.find(vtable) == g_hookedVTables.end()) {
            if (Wh_SetFunctionHook((void*)pfnOriginal, (void*)IShellFolder_EnumObjects_Hook, nullptr)) {
                g_hookedVTables.insert(vtable);
            }
        }
    }
    return hr;
}

void HookIShellFolder(IShellFolder* pFolder) {
    if (!pFolder) return;
    void** vtable = *(void***)pFolder;
    if (g_hookedVTables.find(vtable) == g_hookedVTables.end()) {
        IShellFolder_EnumObjects_t* ppfnEnumObjects = GetEnumObjectsPtr(pFolder);
        IShellFolder_CreateViewObject_t* ppfnCreateViewObject = GetCreateViewObjectPtr(pFolder);
        if (Wh_SetFunctionHook((void*)*ppfnEnumObjects, (void*)IShellFolder_EnumObjects_Hook, nullptr)) {
            g_hookedVTables.insert(vtable);
        }
        Wh_SetFunctionHook((void*)*ppfnCreateViewObject, (void*)IShellFolder_CreateViewObject_Hook, nullptr);
    }
}

HRESULT WINAPI SHCreateShellFolderView_Hook(const SFV_CREATE* pcsfv, IShellView** ppsv) {
    if (pcsfv && pcsfv->pshf) {
        HookIShellFolder(pcsfv->pshf);
    }
    return SHCreateShellFolderView_Original(pcsfv, ppsv);
}

HRESULT WINAPI SHCreateDefaultExtractIcon_Hook(REFIID riid, void** ppv) {
    HRESULT hr = SHCreateDefaultExtractIcon_Original(riid, ppv);
    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
        HookIShellFolder(pDesktop);
        pDesktop->Release();
    }
    return hr;
}

void LoadSettings() {
    PCWSTR excludePaths = Wh_GetStringSetting(L"excludePaths");
    ParseExcludePaths(excludePaths);
    if (excludePaths) {
        Wh_FreeStringSetting(const_cast<WCHAR*>(excludePaths));
    }
}

BOOL Wh_ModInit() {
    LoadSettings();
    
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"shell32.dll"), "SHCreateShellFolderView"), 
                       (void*)SHCreateShellFolderView_Hook, 
                       (void**)&SHCreateShellFolderView_Original);
    
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandle(L"shell32.dll"), "SHCreateDefaultExtractIcon"), 
                       (void*)SHCreateDefaultExtractIcon_Hook, 
                       (void**)&SHCreateDefaultExtractIcon_Original);
    
    IShellFolder* pDesktop = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
        HookIShellFolder(pDesktop);
        pDesktop->Release();
    }
    
    return TRUE;
}

void Wh_ModUninit() {
    g_hookedVTables.clear();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}