// ==WindhawkMod==
// @id              explorer-details-better-file-sizes
// @name            Better file sizes in Explorer details
// @description     Optional improvements: show folder sizes, use MB/GB for large files (by default, all sizes are shown in KBs), use IEC terms (such as KiB instead of KB)
// @version         1.4.1
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

![Screenshot](https://i.imgur.com/aEaxCWe.png)

This mod offers the following optional improvements to file sizes in File
Explorer's details view:

## Show folder sizes

Explorer doesn't show folder sizes. The mod adds this ability, which can be
enabled in the mod settings using one of the following methods:

### Via "Everything" integration

[Everything](https://www.voidtools.com/) is a search engine that locates files
and folders by filename instantly for Windows. It's a great tool which is useful
on its own, but it can also be used with this mod to instantly get folder sizes
without having to calculate them manually.

To show folder sizes via "Everything" integration:

* [Download](https://www.voidtools.com/downloads/) and install "Everything".
  * Note that the integration won't work with the Lite version.
* Enable folder size indexing in "Everything":
  * Open **"Everything"**.
  * From the Tools menu, click **Options**.
  * Click the **Indexes** tab.
  * Check **Index file size**.
  * Check **Index folder size**.
  * Click **OK**.
* Set **Show folder sizes** in the mod's settings to **Enabled via "Everything"
  integration**.

Note: "Everything" must be running for the integration to work.

### Calculated manually

If you prefer to avoid installing "Everything", you can enable folder sizes and
have them calculated manually. Since calculating folder sizes can be slow, it's
not enabled by default, and there's an option to enable it only while holding
the Shift key.

## Mix files and folders when sorting by size

When sorting by size, files end up in one separate chunk, and folders in
another. That's the default Explorer behavior, which also applies when sorting
by other columns. This option changes sorting by size to disable this
separation.

## Use MB/GB for large files

Explorer always shows file sizes in KBs in details, make it use MB/GB when
appropriate.

## Use IEC terms

Use the International Electronic Commission terms: KB → KiB, MB → MiB, GB → GiB.

For a curious read on the topic, check out the following old (2009) blog post by
a Microsoft employee: [Why does Explorer use the term KB instead of
KiB?](https://devblogs.microsoft.com/oldnewthing/20090611-00/?p=17933).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- calculateFolderSizes: disabled
  $name: Show folder sizes
  $description: >-
    The recommended option to enable folder sizes is via "Everything"
    integration. Refer to the mod description for the steps of installing and
    configuring "Everything".

    An alternative option is to calculate folder sizes manually. This can be
    slow, and there's an option to enable it only while holding the Shift key.
    In this case, folder sizes will only be shown if the Shift key is held when
    the list is loaded or refreshed. For example, select a folder, hold Shift
    and press Enter to navigate to it. Another example: hold Shift and click
    refresh, back or forward.
  $options:
  - disabled: Disabled
  - everything: Enabled via "Everything" integration
  - always: Enabled, calculated manually (can be slow)
  - withShiftKey: Enabled, calculated manually while holding the Shift key
- sortSizesMixFolders: true
  $name: Mix files and folders when sorting by size
  $description: >-
    By default, folders are kept separately from files when sorting
- disableKbOnlySizes: true
  $name: Use MB/GB for large files
  $description: >-
    By default, sizes are shown in KBs
- useIecTerms: false
  $name: Use IEC terms
  $description: >-
    Use the International Electronic Commission terms, such as KiB instead of KB
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include <initguid.h>

#include <comutil.h>
#include <propsys.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shtypes.h>
#include <winrt/base.h>

enum class CalculateFolderSizes {
    disabled,
    everything,
    withShiftKey,
    always,
};

struct {
    CalculateFolderSizes calculateFolderSizes;
    bool sortSizesMixFolders;
    bool disableKbOnlySizes;
    bool useIecTerms;
} g_settings;

HMODULE g_propsysModule;
std::atomic<int> g_hookRefCount;

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

// The partial version of the Everything SDK below and some of the code for
// querying Everything are based on code from SizeES: A Plugin for Fast,
// Persistent FolderSizes in x2 via Everything Search.
// https://forum.zabkat.com/viewtopic.php?t=12326

#pragma region everything_sdk
// clang-format off

// Severely reduced Everything_IPC.h, Copyright (C) 2022 David Carpenter
// https://www.voidtools.com/Everything-SDK.zip

#define EVERYTHING_IPC_WNDCLASSW				L"EVERYTHING_TASKBAR_NOTIFICATION"
#define EVERYTHING_IPC_WNDCLASSW_15A			L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)"

#define EVERYTHING_IPC_MATCHCASE				0x00000001
#define EVERYTHING_IPC_MATCHDIACRITICS			0x00000010
#define EVERYTHING_IPC_COPYDATA_QUERY2W			18
#define EVERYTHING_IPC_SORT_NAME_ASCENDING		1

#define EVERYTHING_IPC_QUERY2_REQUEST_SIZE		0x00000010

typedef struct {
	DWORD reply_hwnd;
	DWORD reply_copydata_message;
	DWORD search_flags;
	DWORD offset;
	DWORD max_results;
	DWORD request_flags;
	DWORD sort_type;
} EVERYTHING_IPC_QUERY2;

typedef struct {
	DWORD totitems;
	DWORD numitems;
	DWORD offset;
	DWORD request_flags;
	DWORD sort_type;
} EVERYTHING_IPC_LIST2;

typedef struct {
	DWORD flags;
	DWORD data_offset;
} EVERYTHING_IPC_ITEM2;

// clang-format on
#pragma endregion  // everything_sdk

constexpr DWORD kGsTimeoutIPC = 1000;

#define GS_SEARCH_PREFIX L"folder:wfn:\""
#define GS_SEARCH_SUFFIX L"\""

std::atomic<HWND> g_gsReceiverWnd;

struct {
    // Auto-reset event signalled when a reply to a search query is received.
    std::atomic<HANDLE> hEvent;
    DWORD dwID;
    bool bResult;
    int64_t liSize;
} g_gsReply;

std::mutex g_gsReplyMutex;
std::mutex g_gsReplyCopyDataMutex;
std::atomic<DWORD> g_gsReplyCounter;

enum : unsigned {
    ES_QUERY_OK,
    ES_QUERY_NO_MEMORY,
    ES_QUERY_NO_ES_IPC,
    ES_QUERY_NO_PLUGIN_IPC,
    ES_QUERY_TIMEOUT,
    ES_QUERY_REPLY_TIMEOUT,
    ES_QUERY_NO_RESULT,
};
PCWSTR g_gsQueryStatus[] = {
    L"<Ok>",          L"No Memory",     L"No ES IPC", L"No Plugin IPC",
    L"Query Timeout", L"Reply Timeout", L"No Result",
};

HANDLE g_everything4Wh_Thread;

unsigned Everything4Wh_GetFileSize(PCWSTR folderPath, int64_t* size) {
    *size = 0;

    HWND hReceiverWnd = g_gsReceiverWnd;

    if (!hReceiverWnd) {
        return ES_QUERY_NO_PLUGIN_IPC;
    }

    HWND hEverything = FindWindow(EVERYTHING_IPC_WNDCLASSW, nullptr);

    if (!hEverything) {
        hEverything = FindWindow(EVERYTHING_IPC_WNDCLASSW_15A, nullptr);
    }

    if (!hEverything) {
        return ES_QUERY_NO_ES_IPC;
    }

    // Prevent querying from within the Everything process to avoid deadlocks.
    DWORD dwEverythingProcessId = 0;
    GetWindowThreadProcessId(hEverything, &dwEverythingProcessId);
    if (dwEverythingProcessId == GetCurrentProcessId()) {
        return ES_QUERY_NO_ES_IPC;
    }

    DWORD dwSize = sizeof(EVERYTHING_IPC_QUERY2) +
                   sizeof(GS_SEARCH_PREFIX GS_SEARCH_SUFFIX) +
                   (wcslen(folderPath) * sizeof(WCHAR));
    std::vector<BYTE> queryBuffer(dwSize, 0);
    EVERYTHING_IPC_QUERY2* pQuery = (EVERYTHING_IPC_QUERY2*)queryBuffer.data();

    pQuery->reply_hwnd = (DWORD)(DWORD_PTR)hReceiverWnd;
    pQuery->reply_copydata_message = ++g_gsReplyCounter;
    // Always consider Pokémon distinct from Pokemon.
    pQuery->search_flags =
        EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHDIACRITICS;
    pQuery->offset = 0;
    pQuery->max_results = 1;
    pQuery->request_flags = EVERYTHING_IPC_QUERY2_REQUEST_SIZE;
    pQuery->sort_type = EVERYTHING_IPC_SORT_NAME_ASCENDING;

    swprintf_s((LPWSTR)(pQuery + 1),
               (dwSize - sizeof(*pQuery)) / sizeof(wchar_t), L"%s%s%s",
               GS_SEARCH_PREFIX, folderPath, GS_SEARCH_SUFFIX);

    COPYDATASTRUCT cds = {
        .dwData = EVERYTHING_IPC_COPYDATA_QUERY2W,
        .cbData = dwSize,
        .lpData = pQuery,
    };

    std::lock_guard<std::mutex> guard(g_gsReplyMutex);

    {
        std::lock_guard<std::mutex> copyDataGuard(g_gsReplyCopyDataMutex);
        g_gsReply.dwID = pQuery->reply_copydata_message;
    }

    unsigned result;

    ResetEvent(g_gsReply.hEvent);

    LRESULT lrPending = SendMessageTimeout(
        hEverything, WM_COPYDATA, (WPARAM)hReceiverWnd, (LPARAM)&cds,
        SMTO_BLOCK | SMTO_ABORTIFHUNG, kGsTimeoutIPC, nullptr);
    if (lrPending) {
        // Wait on ReceiverWndProc (below) to signal that the reply was
        // processed.
        DWORD waitResult = WaitForSingleObject(g_gsReply.hEvent, kGsTimeoutIPC);
        if (waitResult != WAIT_OBJECT_0) {
            result = ES_QUERY_REPLY_TIMEOUT;
        } else if (!g_gsReply.bResult) {
            result = ES_QUERY_NO_RESULT;
        } else {
            *size = g_gsReply.liSize;
            result = ES_QUERY_OK;
        }
    } else {
        result = ES_QUERY_TIMEOUT;
    }

    {
        std::lock_guard<std::mutex> copyDataGuard(g_gsReplyCopyDataMutex);
        g_gsReply.dwID = 0;
    }

    return result;
}

LRESULT CALLBACK Everything4Wh_ReceiverWndProc(HWND hWnd,
                                               UINT uMsg,
                                               WPARAM wParam,
                                               LPARAM lParam) {
    switch (uMsg) {
        case WM_COPYDATA: {
            std::lock_guard<std::mutex> guard(g_gsReplyCopyDataMutex);

            COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;

            if (pcds->dwData == g_gsReply.dwID) {
                EVERYTHING_IPC_LIST2* list =
                    (EVERYTHING_IPC_LIST2*)pcds->lpData;

                if (list->numitems) {
                    g_gsReply.liSize =
                        *(int64_t*)(((BYTE*)(list + 1)) +
                                    sizeof(EVERYTHING_IPC_ITEM2));
                    g_gsReply.bResult = true;
                } else {
                    g_gsReply.liSize = 0;
                    g_gsReply.bResult = false;
                }

                SetEvent(g_gsReply.hEvent);
            }

            return TRUE;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

DWORD WINAPI Everything4Wh_Thread(void* parameter) {
    constexpr WCHAR kClassName[] = "WindhawkEverythingReceiver_" WH_MOD_ID;

    HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hEvent) {
        Wh_Log(L"CreateEvent failed");
        return 1;
    }

    WNDCLASSEXW wc = {
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = Everything4Wh_ReceiverWndProc,
        .hInstance = GetCurrentModuleHandle(),
        .lpszClassName = kClassName,
    };

    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"RegisterClassEx failed");
        CloseHandle(hEvent);
        return 1;
    }

    HWND hReceiverWnd =
        CreateWindowEx(0, wc.lpszClassName, nullptr, 0, 0, 0, 0, 0,
                       HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!hReceiverWnd) {
        Wh_Log(L"CreateWindowEx failed");
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        CloseHandle(hEvent);
        return 1;
    }

    ChangeWindowMessageFilterEx(hReceiverWnd, WM_COPYDATA, MSGFLT_ALLOW,
                                nullptr);

    g_gsReply.hEvent = hEvent;
    g_gsReceiverWnd = hReceiverWnd;

    Wh_Log(L"hReceiverWnd=%08X", (DWORD)(ULONG_PTR)hReceiverWnd);

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            g_gsReceiverWnd = nullptr;
            DestroyWindow(hReceiverWnd);
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    g_gsReply.hEvent = nullptr;
    CloseHandle(hEvent);
    return 0;
}

LPCITEMIDLIST PIDLNext(LPCITEMIDLIST pidl) {
    return reinterpret_cast<LPCITEMIDLIST>(reinterpret_cast<const BYTE*>(pidl) +
                                           pidl->mkid.cb);
}

size_t PIDLSize(LPCITEMIDLIST pidl) {
    size_t s = 0;
    while (pidl->mkid.cb > 0) {
        s += pidl->mkid.cb;
        pidl = PIDLNext(pidl);
    }
    // We add 2 because an LPITEMIDLIST is terminated by two NULL bytes.
    return 2 + s;
}

std::vector<BYTE> PIDLToVector(const ITEMIDLIST* pidl) {
    if (!pidl) {
        return {};
    }

    const BYTE* ptr = reinterpret_cast<const BYTE*>(pidl);
    size_t size = PIDLSize(pidl);
    return std::vector<BYTE>(ptr, ptr + size);
}

thread_local winrt::com_ptr<IShellFolder2> g_cacheShellFolder;
thread_local std::map<std::vector<BYTE>, std::optional<ULONGLONG>>
    g_cacheShellFolderSizes;
thread_local DWORD g_cacheShellFolderLastUsedTickCount;

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

std::optional<ULONGLONG> CalculateFolderSize(IShellFolder2* shellFolder) {
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
        shellFolder, NSWF_DONT_ACCUMULATE_RESULT | NSWF_DONT_TRAVERSE_LINKS,
        255, callback);
    if (FAILED(hr)) {
        Wh_Log(L"Failed: %08X", hr);
        callback->Release();
        return std::nullopt;
    }

    ULONGLONG totalSize = callback->GetTotalSize();
    callback->Release();

    return totalSize;
}

bool GetFolderPathFromIShellFolder(IShellFolder2* shellFolder,
                                   WCHAR path[MAX_PATH]) {
    LPITEMIDLIST pidl;
    HRESULT hr = SHGetIDListFromObject(shellFolder, &pidl);
    if (SUCCEEDED(hr)) {
        bool succeeded = SHGetPathFromIDList(pidl, path);
        CoTaskMemFree(pidl);
        return succeeded;
    }
    return false;
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
    auto hookScope = hookRefCountScope();

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

        case CalculateFolderSizes::everything:
        case CalculateFolderSizes::always:
            break;
    }

    Wh_Log(L">");

    winrt::com_ptr<IShellFolder2> shellFolder2;
    HRESULT hr =
        ((IUnknown*)pCFSFolder)
            ->QueryInterface(IID_IShellFolder2, shellFolder2.put_void());
    if (FAILED(hr) || !shellFolder2) {
        Wh_Log(L"Failed: %08X", hr);
        return S_OK;
    }

    if (shellFolder2 != g_cacheShellFolder ||
        GetTickCount() - g_cacheShellFolderLastUsedTickCount > 1000) {
        g_cacheShellFolderSizes.clear();
    }

    g_cacheShellFolder = shellFolder2;

    auto [cacheIt, cacheMissing] = g_cacheShellFolderSizes.try_emplace(
        PIDLToVector(itemidChild), std::nullopt);

    if (cacheMissing) {
        winrt::com_ptr<IShellFolder2> childFolder;
        hr = shellFolder2->BindToObject(itemidChild, nullptr,
                                        IID_PPV_ARGS(childFolder.put()));
        if (FAILED(hr) || !childFolder) {
            Wh_Log(L"Failed: %08X", hr);
        } else if (g_settings.calculateFolderSizes ==
                   CalculateFolderSizes::everything) {
            WCHAR path[MAX_PATH];
            if (GetFolderPathFromIShellFolder(childFolder.get(), path)) {
                int64_t size;
                unsigned result = Everything4Wh_GetFileSize(path, &size);
                if (result == ES_QUERY_OK) {
                    cacheIt->second = size;
                } else {
                    Wh_Log(L"Failed to get size: %s for %s",
                           g_gsQueryStatus[result], path);
                }
            } else {
                Wh_Log(L"Failed to get path");
            }
        } else {
            cacheIt->second = CalculateFolderSize(childFolder.get());
        }
    } else {
        Wh_Log(L"Using cached size");
    }

    g_cacheShellFolderLastUsedTickCount = GetTickCount();

    std::optional<ULONGLONG> folderSize = cacheIt->second;
    if (folderSize) {
        propVariant->uhVal.QuadPart = *folderSize;
        propVariant->vt = VT_UI8;
        Wh_Log(L"Done: %I64u", propVariant->uhVal.QuadPart);
    }

    return S_OK;
}

using CFSFolder_MapColumnToSCID_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                     int column,
                                                     PROPERTYKEY* scid);
CFSFolder_MapColumnToSCID_t CFSFolder_MapColumnToSCID_Original;

using CFSFolder_GetDetailsEx_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                  const ITEMID_CHILD* itemid,
                                                  const PROPERTYKEY* scid,
                                                  VARIANT* value);
CFSFolder_GetDetailsEx_t CFSFolder_GetDetailsEx_Original;

using CFSFolder_CompareIDs_t =
    HRESULT(WINAPI*)(void* pCFSFolder,
                     int column,
                     const ITEMIDLIST_RELATIVE* itemid1,
                     const ITEMIDLIST_RELATIVE* itemid2);
CFSFolder_CompareIDs_t CFSFolder_CompareIDs_Original;
HRESULT WINAPI CFSFolder_CompareIDs_Hook(void* pCFSFolder,
                                         int column,
                                         const ITEMIDLIST_RELATIVE* itemid1,
                                         const ITEMIDLIST_RELATIVE* itemid2) {
    auto hookScope = hookRefCountScope();

    auto original = [=]() {
        return CFSFolder_CompareIDs_Original(pCFSFolder, column, itemid1,
                                             itemid2);
    };

    if (!itemid1 || !itemid2 || !g_settings.sortSizesMixFolders) {
        return original();
    }

    PROPERTYKEY columnSCID;
    if (FAILED(CFSFolder_MapColumnToSCID_Original(pCFSFolder, column,
                                                  &columnSCID)) ||
        !IsEqualPropertyKey(columnSCID, kPKEY_Size)) {
        return original();
    }

    _variant_t value1;
    if (FAILED(CFSFolder_GetDetailsEx_Original(pCFSFolder, itemid1, &columnSCID,
                                               value1.GetAddress())) ||
        value1.vt != VT_UI8) {
        return original();
    }

    _variant_t value2;
    if (FAILED(CFSFolder_GetDetailsEx_Original(pCFSFolder, itemid2, &columnSCID,
                                               value2.GetAddress())) ||
        value2.vt != VT_UI8) {
        return original();
    }

    ULONGLONG size1 = value1.ullVal;
    ULONGLONG size2 = value2.ullVal;

    if (size1 > size2) {
        return 1;
    } else if (size1 < size2) {
        return 0xFFFF;
    } else {
        return 0;
    }
}

using PSFormatForDisplayAlloc_t = decltype(&PSFormatForDisplayAlloc);
PSFormatForDisplayAlloc_t PSFormatForDisplayAlloc_Original;
HRESULT WINAPI PSFormatForDisplayAlloc_Hook(const PROPERTYKEY& key,
                                            const PROPVARIANT& propvar,
                                            PROPDESC_FORMAT_FLAGS pdff,
                                            PWSTR* ppszDisplay) {
    auto original = [=]() {
        return PSFormatForDisplayAlloc_Original(key, propvar, pdff,
                                                ppszDisplay);
    };

    PROPDESC_FORMAT_FLAGS pdffNew = pdff & ~PDFF_ALWAYSKB;
    if (pdffNew == pdff) {
        return original();
    }

    void* retAddress = __builtin_return_address(0);

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

    return PSFormatForDisplayAlloc_Original(key, propvar, pdffNew, ppszDisplay);
}

using PSFormatForDisplay_t = decltype(&PSFormatForDisplay);
PSFormatForDisplay_t PSFormatForDisplay_Original;
HRESULT WINAPI PSFormatForDisplay_Hook(const PROPERTYKEY& propkey,
                                       const PROPVARIANT& propvar,
                                       PROPDESC_FORMAT_FLAGS pdfFlags,
                                       LPWSTR pwszText,
                                       DWORD cchText) {
    auto original = [=]() {
        return PSFormatForDisplay_Original(propkey, propvar, pdfFlags, pwszText,
                                           cchText);
    };

    PROPDESC_FORMAT_FLAGS pdfFlagsNew = pdfFlags & ~PDFF_ALWAYSKB;
    if (pdfFlagsNew == pdfFlags) {
        return original();
    }

    void* retAddress = __builtin_return_address(0);

    HMODULE shell32 = GetModuleHandle(L"shell32.dll");
    if (!shell32) {
        return original();
    }

    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)retAddress, &module) ||
        module != shell32) {
        return original();
    }

    Wh_Log(L">");

    return PSFormatForDisplay_Original(propkey, propvar, pdfFlagsNew, pwszText,
                                       cchText);
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
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#endif
            },
            &CFSFolder_MapColumnToSCID_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const *,struct _tagpropertykey const *,struct tagVARIANT *))",
#endif
            },
            &CFSFolder_GetDetailsEx_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::CompareIDs(__int64,struct _ITEMIDLIST_RELATIVE const __unaligned *,struct _ITEMIDLIST_RELATIVE const __unaligned *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::CompareIDs(long,struct _ITEMIDLIST_RELATIVE const *,struct _ITEMIDLIST_RELATIVE const *))",
#endif
            },
            &CFSFolder_CompareIDs_Original,
            CFSFolder_CompareIDs_Hook,
        },
    };

    return HookSymbols(windowsStorageModule, windowsStorageHooks,
                       ARRAYSIZE(windowsStorageHooks));
}

void LoadSettings() {
    PCWSTR calculateFolderSizes = Wh_GetStringSetting(L"calculateFolderSizes");
    g_settings.calculateFolderSizes = CalculateFolderSizes::disabled;
    if (wcscmp(calculateFolderSizes, L"everything") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::everything;
    } else if (wcscmp(calculateFolderSizes, L"withShiftKey") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::withShiftKey;
    } else if (wcscmp(calculateFolderSizes, L"always") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::always;
    }
    Wh_FreeStringSetting(calculateFolderSizes);

    g_settings.sortSizesMixFolders = Wh_GetIntSetting(L"sortSizesMixFolders");
    g_settings.disableKbOnlySizes = Wh_GetIntSetting(L"disableKbOnlySizes");
    g_settings.useIecTerms = Wh_GetIntSetting(L"useIecTerms");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled) {
        if (!HookWindowsStorageSymbols()) {
            Wh_Log(L"Failed hooking Windows Storage symbols");
            return false;
        }
    }

    if (g_settings.disableKbOnlySizes) {
        WindhawkUtils::Wh_SetFunctionHookT(PSFormatForDisplayAlloc,
                                           PSFormatForDisplayAlloc_Hook,
                                           &PSFormatForDisplayAlloc_Original);

        // Used by older file dialogs, for example Regedit's export dialog.
        WindhawkUtils::Wh_SetFunctionHookT(PSFormatForDisplay,
                                           PSFormatForDisplay_Hook,
                                           &PSFormatForDisplay_Original);
    }

    if (g_settings.useIecTerms) {
        g_propsysModule = GetModuleHandle(L"propsys.dll");
        if (!g_propsysModule) {
            Wh_Log(L"Failed getting propsys.dll");
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

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_settings.calculateFolderSizes == CalculateFolderSizes::everything) {
        g_everything4Wh_Thread =
            CreateThread(nullptr, 0, Everything4Wh_Thread, nullptr, 0, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_everything4Wh_Thread) {
        PostThreadMessage(GetThreadId(g_everything4Wh_Thread), WM_APP, 0, 0);
        WaitForSingleObject(g_everything4Wh_Thread, INFINITE);
        CloseHandle(g_everything4Wh_Thread);
        g_everything4Wh_Thread = nullptr;
    }

    while (g_hookRefCount > 0) {
        Sleep(200);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}
