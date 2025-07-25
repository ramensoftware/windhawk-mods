// ==WindhawkMod==
// @id              hide-dotfiles-explorer
// @name            Hide Dotfiles (Explorer Only)
// @description     Hide dotfiles (files starting with .) in Windows Explorer only.
// @version         1.0
// @author          @danalec
// @github          https://github.com/danalec
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi
// ==/WindhawkMod==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

const GUID IID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const GUID IID_IEnumIDList = {0x000214F2, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

typedef HRESULT (WINAPI *IShellFolder_EnumObjects_t)(IShellFolder* pThis, HWND hwnd, SHCONTF grfFlags, IEnumIDList** ppenumIDList);
IShellFolder_EnumObjects_t IShellFolder_EnumObjects_Original;

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
        if (riid == IID_IUnknown || riid == IID_IEnumIDList) {
            *ppvObject = static_cast<IEnumIDList*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_cRef); }
    
    STDMETHODIMP_(ULONG) Release() {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0) delete this;
        return cRef;
    }

    STDMETHODIMP Next(ULONG celt, LPITEMIDLIST* rgelt, ULONG* pceltFetched) {
        if (!m_pOriginal || !rgelt) return E_FAIL;
        
        ULONG fetched = 0;
        while (fetched < celt) {
            LPITEMIDLIST pidl = nullptr;
            ULONG itemsFetched = 0;
            
            HRESULT hr = m_pOriginal->Next(1, &pidl, &itemsFetched);
            if (hr != S_OK || itemsFetched == 0) break;
            
            bool shouldInclude = true;
            if (m_pFolder) {
                STRRET strret;
                if (SUCCEEDED(m_pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING | SHGDN_INFOLDER, &strret))) {
                    WCHAR szPath[MAX_PATH];
                    if (SUCCEEDED(StrRetToBufW(&strret, pidl, szPath, MAX_PATH))) {
                        WCHAR* pFileName = PathFindFileNameW(szPath);
                        if (pFileName && pFileName[0] == L'.') shouldInclude = false;
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
        
        if (pceltFetched) *pceltFetched = fetched;
        return (fetched == celt) ? S_OK : S_FALSE;
    }

    STDMETHODIMP Skip(ULONG celt) {
        if (!m_pOriginal) return E_FAIL;
        ULONG skipped = 0;
        while (skipped < celt) {
            LPITEMIDLIST pidl = nullptr;
            ULONG itemsFetched = 0;
            HRESULT hr = m_pOriginal->Next(1, &pidl, &itemsFetched);
            if (hr != S_OK || itemsFetched == 0) return S_FALSE;
            
            bool isVisible = true;
            if (m_pFolder) {
                STRRET strret;
                if (SUCCEEDED(m_pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING | SHGDN_INFOLDER, &strret))) {
                    WCHAR szPath[MAX_PATH];
                    if (SUCCEEDED(StrRetToBufW(&strret, pidl, szPath, MAX_PATH))) {
                        WCHAR* pFileName = PathFindFileNameW(szPath);
                        if (pFileName && pFileName[0] == L'.') isVisible = false;
                    }
                }
            }
            CoTaskMemFree(pidl);
            if (isVisible) skipped++;
        }
        return S_OK;
    }
    
    STDMETHODIMP Reset() { return m_pOriginal ? m_pOriginal->Reset() : E_FAIL; }
    
    STDMETHODIMP Clone(IEnumIDList** ppenum) {
        if (!m_pOriginal || !ppenum) return E_FAIL;
        IEnumIDList* pCloned = nullptr;
        HRESULT hr = m_pOriginal->Clone(&pCloned);
        if (SUCCEEDED(hr)) {
            *ppenum = new DotfileFilterEnum(pCloned, m_pFolder);
            pCloned->Release();
        }
        return hr;
    }
};

HRESULT WINAPI IShellFolder_EnumObjects_Hook(IShellFolder* pThis, HWND hwnd, SHCONTF grfFlags, IEnumIDList** ppenumIDList) {
    HRESULT hr = IShellFolder_EnumObjects_Original(pThis, hwnd, grfFlags, ppenumIDList);
    if (SUCCEEDED(hr) && ppenumIDList && *ppenumIDList) {
        IEnumIDList* pOriginal = *ppenumIDList;
        *ppenumIDList = new DotfileFilterEnum(pOriginal, pThis);
        pOriginal->Release();
    }
    return hr;
}

BOOL Wh_ModInit() {
    HMODULE hShell32 = LoadLibrary(L"shell32.dll");
    if (!hShell32) return FALSE;
    
    IShellFolder* pDesktop = nullptr;
    HRESULT hr = SHGetDesktopFolder(&pDesktop);
    if (FAILED(hr) || !pDesktop) return FALSE;
    
    void** vtable = *(void***)pDesktop;
    pDesktop->Release();
    
    Wh_SetFunctionHook(vtable[4], (void*)IShellFolder_EnumObjects_Hook, (void**)&IShellFolder_EnumObjects_Original);
    return TRUE;
}

void Wh_ModUninit() {}