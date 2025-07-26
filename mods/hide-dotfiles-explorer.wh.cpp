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
You can exclude specific paths from having their dotfiles hidden. In the mod settings, add paths separated by commas:
- Single path: `C:\Projects`
- Multiple paths: `C:\Projects, D:\Work`
- Paths with spaces: `"C:\Program Files", "D:\My Documents"`

## Restarting Explorer after enabling
Run this PowerShell command to restart Explorer and clear the icon cache:
```powershell
Stop-Process -Name explorer -Force; Remove-Item "$env:LOCALAPPDATA\IconCache.db" -Force -EA 0; Start-Process explorer
```
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- excludePaths: ""
  $name: Exclude Paths
  $description: Paths where dotfiles will remain visible (comma-separated, use quotes for paths with spaces)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <vector>
#include <string>

const GUID IID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const GUID IID_IEnumIDList = {0x000214F2, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

typedef HRESULT (WINAPI *IShellFolder_EnumObjects_t)(IShellFolder* pThis, HWND hwnd, SHCONTF grfFlags, IEnumIDList** ppenumIDList);
IShellFolder_EnumObjects_t IShellFolder_EnumObjects_Original;

std::vector<std::wstring> g_excludePaths;

void ParseExcludePaths(const WCHAR* paths) {
    g_excludePaths.clear();
    if (!paths || !*paths) return;
    
    std::wstring input(paths);
    size_t pos = 0;
    bool inQuotes = false;
    std::wstring current;
    
    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] == L'"') {
            inQuotes = !inQuotes;
        } else if (input[i] == L',' && !inQuotes) {
            if (!current.empty()) {
                g_excludePaths.push_back(current);
                current.clear();
            }
        } else if (input[i] != L' ' || !current.empty() || inQuotes) {
            current += input[i];
        }
    }
    
    if (!current.empty()) {
        g_excludePaths.push_back(current);
    }
}

bool IsPathExcluded(const WCHAR* path) {
    if (!path || g_excludePaths.empty()) return false;
    
    std::wstring fullPath(path);
    for (auto& p : fullPath) p = towlower(p);
    
    for (const auto& excludePath : g_excludePaths) {
        std::wstring excl = excludePath;
        for (auto& p : excl) p = towlower(p);
        
        if (fullPath.find(excl) != std::wstring::npos) {
            return true;
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
        
        if (m_pOriginal) {
            return m_pOriginal->QueryInterface(riid, ppvObject);
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
        
        while (fetched < celt) {
            LPITEMIDLIST pidl = nullptr;
            ULONG itemsFetched = 0;
            
            HRESULT hr = m_pOriginal->Next(1, &pidl, &itemsFetched);
            if (hr != S_OK || !pidl || itemsFetched == 0) {
                break;
            }
            
            bool shouldInclude = true;
            
            if (m_pFolder && pidl) {
                WCHAR szFullPath[MAX_PATH] = {0};
                STRRET strret = {0};
                
                hr = m_pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &strret);
                if (SUCCEEDED(hr)) {
                    StrRetToBufW(&strret, pidl, szFullPath, ARRAYSIZE(szFullPath));
                }
                
                if (!IsPathExcluded(szFullPath)) {
                    strret = {0};
                    hr = m_pFolder->GetDisplayNameOf(pidl, SHGDN_INFOLDER | SHGDN_FORPARSING, &strret);
                    
                    if (SUCCEEDED(hr)) {
                        WCHAR szName[MAX_PATH] = {0};
                        hr = StrRetToBufW(&strret, pidl, szName, ARRAYSIZE(szName));
                        
                        if (SUCCEEDED(hr) && szName[0] != L'\0') {
                            WCHAR* pFileName = PathFindFileNameW(szName);
                            if (pFileName && pFileName[0] == L'.') {
                                shouldInclude = false;
                            }
                        }
                    }
                }
            }
            
            if (shouldInclude) {
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
            if (hr != S_OK || !pidl || itemsFetched == 0) {
                return S_FALSE;
            }
            
            bool isVisible = true;
            
            if (m_pFolder && pidl) {
                WCHAR szFullPath[MAX_PATH] = {0};
                STRRET strret = {0};
                
                hr = m_pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &strret);
                if (SUCCEEDED(hr)) {
                    StrRetToBufW(&strret, pidl, szFullPath, ARRAYSIZE(szFullPath));
                }
                
                if (!IsPathExcluded(szFullPath)) {
                    strret = {0};
                    hr = m_pFolder->GetDisplayNameOf(pidl, SHGDN_INFOLDER | SHGDN_FORPARSING, &strret);
                    
                    if (SUCCEEDED(hr)) {
                        WCHAR szName[MAX_PATH] = {0};
                        hr = StrRetToBufW(&strret, pidl, szName, ARRAYSIZE(szName));
                        
                        if (SUCCEEDED(hr) && szName[0] != L'\0') {
                            WCHAR* pFileName = PathFindFileNameW(szName);
                            if (pFileName && pFileName[0] == L'.') {
                                isVisible = false;
                            }
                        }
                    }
                }
            }
            
            CoTaskMemFree(pidl);
            
            if (isVisible) {
                skipped++;
            }
        }
        
        return S_OK;
    }
    
    STDMETHODIMP Reset() { 
        return m_pOriginal ? m_pOriginal->Reset() : E_FAIL; 
    }
    
    STDMETHODIMP Clone(IEnumIDList** ppenum) {
        if (!m_pOriginal || !ppenum) return E_INVALIDARG;
        
        *ppenum = nullptr;
        
        IEnumIDList* pCloned = nullptr;
        HRESULT hr = m_pOriginal->Clone(&pCloned);
        
        if (SUCCEEDED(hr) && pCloned) {
            *ppenum = new DotfileFilterEnum(pCloned, m_pFolder);
            pCloned->Release();
            
            if (!*ppenum) {
                return E_OUTOFMEMORY;
            }
        }
        
        return hr;
    }
};

HRESULT WINAPI IShellFolder_EnumObjects_Hook(IShellFolder* pThis, HWND hwnd, SHCONTF grfFlags, IEnumIDList** ppenumIDList) {
    HRESULT hr = IShellFolder_EnumObjects_Original(pThis, hwnd, grfFlags, ppenumIDList);
    
    if (SUCCEEDED(hr) && ppenumIDList && *ppenumIDList) {
        IEnumIDList* pOriginal = *ppenumIDList;
        DotfileFilterEnum* pFiltered = new DotfileFilterEnum(pOriginal, pThis);
        
        if (pFiltered) {
            *ppenumIDList = pFiltered;
            pOriginal->Release();
        }
    }
    
    return hr;
}

struct {
    PCWSTR excludePaths;
} settings;

void LoadSettings() {
    settings.excludePaths = Wh_GetStringSetting(L"excludePaths");
    ParseExcludePaths(settings.excludePaths);
}

BOOL Wh_ModInit() {
    LoadSettings();
    
    IShellFolder* pDesktop = nullptr;
    HRESULT hr = SHGetDesktopFolder(&pDesktop);
    
    if (FAILED(hr) || !pDesktop) {
        return FALSE;
    }
    
    void** vtable = *(void***)pDesktop;
    
    if (!Wh_SetFunctionHook(vtable[4], (void*)IShellFolder_EnumObjects_Hook, (void**)&IShellFolder_EnumObjects_Original)) {
        pDesktop->Release();
        return FALSE;
    }
    
    pDesktop->Release();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_FreeStringSetting(settings.excludePaths);
}

void Wh_ModSettingsChanged() {
    Wh_FreeStringSetting(settings.excludePaths);
    LoadSettings();
}