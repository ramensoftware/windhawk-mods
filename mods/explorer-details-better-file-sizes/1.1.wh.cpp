// ==WindhawkMod==
// @id              explorer-details-better-file-sizes
// @name            Better file sizes in Explorer details
// @description     Explorer always shows file sizes in KBs in details, make it show MB/GB (or, optionally, KiB/MiB/GiB) when appropriate, and optionally show folder sizes too
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @compilerOptions -lole32 -loleaut32 -lpropsys
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Better file sizes in Explorer details

Explorer always shows file sizes in KBs in details, make it show MB/GB when
appropriate.

Also, optionally make it use the International Electronic Commission terms (e.g.
KiB instead of KB). See also: [Why does Explorer use the term KB instead of
KiB?](https://devblogs.microsoft.com/oldnewthing/20090611-00/?p=17933).

Explorer also doesn't show folder sizes. The mod adds this ability, which can be
enabled in the mod settings. Since calculating folder sizes can be slow, it's
not enabled by default, and there's an option to enable it only while holding
the Shift key.

![Screenshot](https://i.imgur.com/5RIZS2Q.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- useIecTerms: false
  $name: Use IEC terms
  $description: >-
    Use the International Electronic Commission terms, e.g. KiB instead of KB
- calculateFolderSizes: disabled
  $name: Calculate folder sizes (can be slow)
  $options:
  - disabled: Disabled
  - withShiftKey: Enabled while holding the Shift key
  - always: Always enabled
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <optional>

#include <initguid.h>

#include <propsys.h>
#include <shobjidl.h>
#include <shtypes.h>
#include <winrt/base.h>

enum class CalculateFolderSizes {
    disabled,
    withShiftKey,
    always,
};

struct {
    bool useIecTerms;
    CalculateFolderSizes calculateFolderSizes;
} g_settings;

HMODULE g_propsysModule;

constexpr GUID KStorage = {0xB725F130,
                           0x47EF,
                           0x101A,
                           {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}};
constexpr PROPERTYKEY kPKEY_Size = {KStorage, 12};

class SizeCalculator : public INamespaceWalkCB2 {
   public:
    SizeCalculator() : m_totalSize(0) {}
    virtual ~SizeCalculator() {}

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppvObject) override {
        winrt::guid riidguid{riid};
        if (riidguid == winrt::guid_of<IUnknown>() ||
            riidguid == winrt::guid_of<INamespaceWalkCB>() ||
            riidguid == winrt::guid_of<INamespaceWalkCB2>()) {
            *ppvObject = static_cast<INamespaceWalkCB2*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // INamespaceWalkCB methods
    HRESULT STDMETHODCALLTYPE FoundItem(IShellFolder* psf,
                                        LPCITEMIDLIST pidl) override {
        winrt::com_ptr<IShellFolder2> psf2;
        HRESULT hr = psf->QueryInterface(IID_PPV_ARGS(psf2.put()));
        if (FAILED(hr)) {
            Wh_Log(L"Failed: %08X", hr);
            return hr;
        }

        VARIANT varSize;
        hr = psf2->GetDetailsEx(pidl, &kPKEY_Size, &varSize);
        if (SUCCEEDED(hr) && varSize.vt == VT_UI8) {
            m_totalSize += varSize.ullVal;
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EnterFolder(IShellFolder* /*psf*/,
                                          LPCITEMIDLIST /*pidl*/) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE LeaveFolder(IShellFolder* /*psf*/,
                                          LPCITEMIDLIST /*pidl*/) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE WalkComplete(HRESULT hr) override { return S_OK; }

    HRESULT STDMETHODCALLTYPE
    InitializeProgressDialog(LPWSTR* /*ppszTitle*/,
                             LPWSTR* /*ppszCancel*/) override {
        return E_NOTIMPL;
    }

    // Get the total size.
    ULONGLONG GetTotalSize() const { return m_totalSize; }

   private:
    ULONG m_refCount = 1;
    ULONGLONG m_totalSize;
};

std::optional<ULONGLONG> CalculateFolderSize(
    winrt::com_ptr<IShellFolder2> shellFolder,
    const ITEMID_CHILD* childItem) {
    // If a childItem is provided, bind to that subfolder
    if (childItem) {
        winrt::com_ptr<IShellFolder2> childFolder;
        HRESULT hr = shellFolder->BindToObject(childItem, nullptr,
                                               IID_PPV_ARGS(childFolder.put()));
        if (FAILED(hr)) {
            Wh_Log(L"Failed: %08X", hr);
            return std::nullopt;
        }
        shellFolder = childFolder;
    }

    // Create the namespace walker.
    winrt::com_ptr<INamespaceWalk> namespaceWalk;
    HRESULT hr = CoCreateInstance(CLSID_NamespaceWalker, nullptr, CLSCTX_INPROC,
                                  IID_PPV_ARGS(namespaceWalk.put()));
    if (FAILED(hr)) {
        Wh_Log(L"Failed: %08X", hr);
        return std::nullopt;
    }

    // Create the callback object.
    SizeCalculator* callback = new SizeCalculator();

    // Enumerate child items and sum sizes in the callback.
    hr = namespaceWalk->Walk(
        shellFolder.get(),
        NSWF_DONT_ACCUMULATE_RESULT | NSWF_DONT_TRAVERSE_LINKS, 10, callback);
    if (FAILED(hr)) {
        Wh_Log(L"Failed: %08X", hr);
        callback->Release();
        return std::nullopt;
    }

    ULONGLONG totalSize = callback->GetTotalSize();
    callback->Release();

    return totalSize;
}

using CFSFolder__GetSize_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                              const ITEMID_CHILD* itemidChild,
                                              const void* idFolder,
                                              PROPVARIANT* propVariant);
CFSFolder__GetSize_t CFSFolder__GetSize_Original;
HRESULT WINAPI CFSFolder__GetSize_Hook(void* pCFSFolder,
                                       const ITEMID_CHILD* itemidChild,
                                       const void* idFolder,
                                       PROPVARIANT* propVariant) {
    Wh_Log(L">");

    HRESULT ret = CFSFolder__GetSize_Original(pCFSFolder, itemidChild, idFolder,
                                              propVariant);
    if (ret != S_OK || propVariant->vt != VT_EMPTY) {
        return ret;
    }

    switch (g_settings.calculateFolderSizes) {
        case CalculateFolderSizes::disabled:
            return ret;

        case CalculateFolderSizes::withShiftKey:
            if (GetAsyncKeyState(VK_SHIFT) >= 0) {
                return ret;
            }
            break;

        case CalculateFolderSizes::always:
            break;
    }

    winrt::com_ptr<IShellFolder2> shellFolder2;
    HRESULT hr =
        ((IUnknown*)pCFSFolder)
            ->QueryInterface(IID_IShellFolder2, shellFolder2.put_void());
    if (FAILED(hr) || !shellFolder2) {
        Wh_Log(L"Failed");
        return S_OK;
    }

    auto folderSize = CalculateFolderSize(shellFolder2, itemidChild);
    if (folderSize) {
        propVariant->uhVal.QuadPart = *folderSize;
        propVariant->vt = VT_UI8;
        Wh_Log(L"Done: %I64u", propVariant->uhVal.QuadPart);
    }

    return S_OK;
}

using PSFormatForDisplayAlloc_t = decltype(&PSFormatForDisplayAlloc);
PSFormatForDisplayAlloc_t PSFormatForDisplayAlloc_Original;
HRESULT WINAPI PSFormatForDisplayAlloc_Hook(const PROPERTYKEY& key,
                                            const PROPVARIANT& propvar,
                                            PROPDESC_FORMAT_FLAGS pdff,
                                            PWSTR* ppszDisplay) {
    void* retAddress = __builtin_return_address(0);

    auto original = [=]() {
        return PSFormatForDisplayAlloc_Original(key, propvar, pdff,
                                                ppszDisplay);
    };

    HMODULE explorerFrame = GetModuleHandle(L"explorerframe.dll");
    if (!explorerFrame) {
        return original();
    }

    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)retAddress, &module) ||
        module != explorerFrame) {
        return original();
    }

    Wh_Log(L">");

    pdff &= ~PDFF_ALWAYSKB;
    return PSFormatForDisplayAlloc_Original(key, propvar, pdff, ppszDisplay);
}

using LoadStringW_t = decltype(&LoadStringW);
LoadStringW_t LoadStringW_Original;
int WINAPI LoadStringW_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPWSTR lpBuffer,
                            int cchBufferMax) {
    int ret = LoadStringW_Original(hInstance, uID, lpBuffer, cchBufferMax);
    if (!ret || hInstance != g_propsysModule || cchBufferMax == 0) {
        return ret;
    }

    PCWSTR newStr = nullptr;
    if (wcscmp(lpBuffer, L"%s KB") == 0) {
        newStr = L"%s KiB";
    } else if (wcscmp(lpBuffer, L"%s MB") == 0) {
        newStr = L"%s MiB";
    } else if (wcscmp(lpBuffer, L"%s GB") == 0) {
        newStr = L"%s GiB";
    } else if (wcscmp(lpBuffer, L"%s TB") == 0) {
        newStr = L"%s TiB";
    } else if (wcscmp(lpBuffer, L"%s PB") == 0) {
        newStr = L"%s PiB";
    } else if (wcscmp(lpBuffer, L"%s EB") == 0) {
        newStr = L"%s EiB";
    }

    if (!newStr) {
        return ret;
    }

    Wh_Log(L"> Overriding string %u: %s -> %s", uID, lpBuffer, newStr);
    wcsncpy_s(lpBuffer, cchBufferMax, newStr, cchBufferMax - 1);
    return wcslen(lpBuffer);
}

bool HookWindowsStorageSymbols() {
    HMODULE windowsStorageModule = LoadLibrary(L"windows.storage.dll");
    if (!windowsStorageModule) {
        Wh_Log(L"Failed to load windows.storage.dll");
        return false;
    }

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
        {
            {
#ifdef _WIN64
                LR"(protected: static long __cdecl CFSFolder::_GetSize(class CFSFolder *,struct _ITEMID_CHILD const __unaligned *,struct IDFOLDER const __unaligned *,struct tagPROPVARIANT *))",
#else
                LR"(protected: static long __stdcall CFSFolder::_GetSize(class CFSFolder *,struct _ITEMID_CHILD const *,struct IDFOLDER const *,struct tagPROPVARIANT *))",
#endif
            },
            &CFSFolder__GetSize_Original,
            CFSFolder__GetSize_Hook,
        },
    };

    return HookSymbols(windowsStorageModule, windowsStorageHooks,
                       ARRAYSIZE(windowsStorageHooks));
}

void LoadSettings() {
    g_settings.useIecTerms = Wh_GetIntSetting(L"useIecTerms");

    PCWSTR calculateFolderSizes = Wh_GetStringSetting(L"calculateFolderSizes");
    g_settings.calculateFolderSizes = CalculateFolderSizes::disabled;
    if (wcscmp(calculateFolderSizes, L"withShiftKey") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::withShiftKey;
    } else if (wcscmp(calculateFolderSizes, L"always") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::always;
    }
    Wh_FreeStringSetting(calculateFolderSizes);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WindhawkUtils::Wh_SetFunctionHookT(PSFormatForDisplayAlloc,
                                       PSFormatForDisplayAlloc_Hook,
                                       &PSFormatForDisplayAlloc_Original);

    if (g_settings.useIecTerms) {
        g_propsysModule = GetModuleHandle(L"propsys.dll");
        if (!g_propsysModule) {
            Wh_Log(L"Failed");
            return FALSE;
        }

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

        auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                         PCSTR targetName, void* hookFunction,
                                         void** originalFunction) {
            void* targetFunction =
                (void*)GetProcAddress(kernelBaseModule, targetName);
            if (!targetFunction) {
                targetFunction =
                    (void*)GetProcAddress(kernel32Module, targetName);
                if (!targetFunction) {
                    return FALSE;
                }
            }

            return Wh_SetFunctionHook(targetFunction, hookFunction,
                                      originalFunction);
        };

        setKernelFunctionHook("LoadStringW", (void*)LoadStringW_Hook,
                              (void**)&LoadStringW_Original);
    }

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled) {
        if (!HookWindowsStorageSymbols()) {
            Wh_Log(L"Failed hooking Windows Storage symbols");
            return false;
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}
