// ==WindhawkMod==
// @id              recycle-bin-file-preview
// @name            Recycle Bin File Preview
// @description     Preview deleted files in the Recycle Bin using the native File Explorer Preview Pane (Alt+P), without restoring them.
// @version         1.0.0
// @author          Luiz Bizzio
// @github          https://github.com/luizbizzio
// @homepage        https://github.com/luizbizzio/recycle-bin-file-preview
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshell32 -lshlwapi -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Recycle Bin File Preview

Use the built-in Windows File Explorer Preview Pane (**Alt + P**) to view
files in the Recycle Bin without restoring them.

![Recycle Bin File Preview before and after](https://raw.githubusercontent.com/luizbizzio/recycle-bin-file-preview/main/assets/recycle-bin-file-preview-comparison.png)

The image compares the default Recycle Bin view with the preview enabled
by this mod.

## How to use

1. Open the Recycle Bin in File Explorer.
2. Press **Alt + P** to show the Preview Pane.
3. Select a file to see its preview.

The mod uses your Windows file associations and installed preview handlers.
It does not add new preview handlers or codecs.

## Limitations

- File types without preview support in normal Explorer folders may still
  show **No preview available** in the Recycle Bin.
- The mod does not let you browse inside deleted folders.
- It does not restore, copy, edit or delete your files.
- Windows updates can change how the Recycle Bin works.

## Troubleshooting

Enable **Debug logging** in the mod settings and look for messages starting
with `RBP:` in the Windhawk log. Keep logging off during normal use.

## Project

[Source code and issue reports](https://github.com/luizbizzio/recycle-bin-file-preview)

Version **1.0.0**. Author: **Luiz Bizzio**. Licensed under **MIT**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DebugLogging: false
  $name: Debug logging
  $description: Show technical logs for troubleshooting. Leave disabled during normal use.
*/
// ==/WindhawkModSettings==

// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Luiz Bizzio

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <atomic>
#include <string>
#include <cwchar>
#include <cwctype>

// The preview association fallback is the core feature, not a user setting.
// Keep diagnostics thread-safe because Explorer calls COM methods on
// multiple threads.
static std::atomic<bool> g_debugLogging{false};

// Deliberately define GUIDs locally. Some Windhawk/LLVM SDK combinations
// declare these GUIDs but lack the imported GUID object at link time.
static const GUID kBhidAssociationArray = {0xbea9ef17, 0x82f1, 0x4f60,
    {0x92, 0x84, 0x4f, 0x8d, 0xb7, 0x5c, 0x3b, 0xe9}};
static const GUID kIidQueryAssociations = {0xc46ca590, 0x3c3f, 0x11d2,
    {0xbe, 0xe6, 0x00, 0x00, 0xf8, 0x05, 0xca, 0x57}};
static const GUID kClsidQueryAssociations = {0xa07034fd, 0x6caa, 0x4954,
    {0xac, 0x3f, 0x97, 0xa2, 0x72, 0x16, 0xf9, 0x8a}};
static constexpr wchar_t kPreviewHandlerType[] =
    L"{8895b1c6-b41f-4c1c-a562-0d564250836f}";

static PIDLIST_ABSOLUTE g_recycleBin = nullptr;
static HMODULE g_hookModulePin = nullptr;
static std::atomic<unsigned> g_assocCalls{0};
static std::atomic<unsigned> g_verifiedItems{0};
static std::atomic<unsigned> g_hasRegisteredPreview{0};
static std::atomic<unsigned> g_replacementCreated{0};
static std::atomic<unsigned> g_nativeHasPreview{0};
static std::atomic<unsigned> g_overrides{0};
static std::atomic<unsigned> g_skipped{0};
static std::atomic<unsigned> g_failed{0};
static thread_local bool g_inHook = false;

using BindToHandler_t = HRESULT (STDMETHODCALLTYPE *)(
    IShellItem*, IBindCtx*, REFGUID, REFIID, void**);
static BindToHandler_t g_originalBind = nullptr;

struct ReentryGuard {
    ReentryGuard() { g_inHook = true; }
    ~ReentryGuard() { g_inHook = false; }
};

static void LoadSettings() {
    g_debugLogging.store(Wh_GetIntSetting(L"DebugLogging") != 0,
                         std::memory_order_relaxed);
}

static bool IsRecycleChild(PCIDLIST_ABSOLUTE pidl) {
    return pidl && g_recycleBin &&
        ILIsParent(g_recycleBin, pidl, FALSE) &&
        !ILIsEqual(g_recycleBin, pidl);
}

// Confirm this is a real, regular $R file in the Recycle Bin namespace.
// This is deliberately stricter than checking a filename or a virtual path.
static bool IsPhysicalDeletedFile(const wchar_t* path) {
    if (!path || !StrStrIW(path, L"\\$Recycle.Bin\\"))
        return false;
    const wchar_t* filename = wcsrchr(path, L'\\');
    if (!filename || filename[1] != L'$' ||
        (filename[2] != L'R' && filename[2] != L'r') ||
        !filename[3])
        return false;
    DWORD attr = GetFileAttributesW(path);
    return attr != INVALID_FILE_ATTRIBUTES &&
        !(attr & FILE_ATTRIBUTE_DIRECTORY) &&
        !(attr & FILE_ATTRIBUTE_REPARSE_POINT);
}

static bool GetPhysicalPathFromPidl(
    PCIDLIST_ABSOLUTE pidl, std::wstring& physical) {
    if (!IsRecycleChild(pidl))
        return false;
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    HRESULT hr = SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child);
    if (FAILED(hr) || !parent || !child) {
        if (parent) parent->Release();
        return false;
    }

    STRRET name{};
    wchar_t* value = nullptr;
    hr = parent->GetDisplayNameOf(child, SHGDN_FORPARSING, &name);
    if (SUCCEEDED(hr))
        hr = StrRetToStrW(&name, child, &value);

    const bool ok = SUCCEEDED(hr) && IsPhysicalDeletedFile(value);
    if (ok) physical.assign(value);
    if (value) CoTaskMemFree(value);
    parent->Release();
    return ok;
}

static bool GetDeletedPhysicalPath(IShellItem* item, std::wstring& physical) {
    if (!item) return false;
    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = SHGetIDListFromObject(item, &pidl);
    if (FAILED(hr) || !pidl) return false;
    const bool ok = GetPhysicalPathFromPidl(pidl, physical);
    CoTaskMemFree(pidl);
    return ok;
}

static bool ParseExtension(const wchar_t* name, std::wstring& extension) {
    if (!name) return false;
    const wchar_t* dot = wcsrchr(name, L'.');
    const wchar_t* slash = wcsrchr(name, L'\\');
    if (!dot || dot == name || !dot[1] || (slash && dot < slash))
        return false;
    size_t len = wcslen(dot);
    if (len < 2 || len > 17) return false;
    for (size_t i = 1; i < len; ++i) {
        const wchar_t c = dot[i];
        if (!((c >= L'A' && c <= L'Z') ||
              (c >= L'a' && c <= L'z') ||
              (c >= L'0' && c <= L'9') || c == L'_' || c == L'-'))
            return false;
    }
    extension.assign(dot);
    for (wchar_t& c : extension) c = towlower(c);
    return true;
}

// Prefer the original display name, not the synthetic $Rxxx filename.
// Require the physical extension to match as a conservative sanity check.
static bool GetOriginalExtension(IShellItem* item,
                                 const std::wstring& physical,
                                 std::wstring& extension) {
    PWSTR displayName = nullptr;
    HRESULT hr = item->GetDisplayName(SIGDN_NORMALDISPLAY, &displayName);
    std::wstring visibleExt;
    bool valid = SUCCEEDED(hr) && displayName &&
                 ParseExtension(displayName, visibleExt);
    if (displayName) CoTaskMemFree(displayName);
    if (!valid) {
        displayName = nullptr;
        hr = item->GetDisplayName(SIGDN_PARENTRELATIVEEDITING, &displayName);
        valid = SUCCEEDED(hr) && displayName &&
                ParseExtension(displayName, visibleExt);
        if (displayName) CoTaskMemFree(displayName);
    }
    if (!valid) return false;

    std::wstring physicalExt;
    if (!ParseExtension(physical.c_str(), physicalExt)) return false;
    if (visibleExt != physicalExt) return false;
    extension = visibleExt;
    return true;
}

static bool GetRegisteredPreview(const wchar_t* extension, GUID& clsid) {
    wchar_t value[64] = {};
    DWORD length = ARRAYSIZE(value);
    HRESULT hr = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_SHELLEXTENSION,
                                   extension, kPreviewHandlerType,
                                   value, &length);
    return SUCCEEDED(hr) && SUCCEEDED(CLSIDFromString(value, &clsid));
}

static bool QueryPreviewFromAssoc(IQueryAssociations* assoc, GUID& clsid) {
    if (!assoc) return false;
    wchar_t value[64] = {};
    DWORD length = ARRAYSIZE(value);
    HRESULT hr = assoc->GetString(ASSOCF_NONE, ASSOCSTR_SHELLEXTENSION,
                                  kPreviewHandlerType, value, &length);
    return SUCCEEDED(hr) && SUCCEEDED(CLSIDFromString(value, &clsid));
}

// Produce the same *kind* of association object Explorer obtains for a
// regular file type. This object is scoped to the intercepted COM call;
// the registry, handler registrations and file bytes are not changed.
static HRESULT CreateExtensionAssociations(const wchar_t* extension,
                                            IQueryAssociations** result) {
    if (!result) return E_POINTER;
    *result = nullptr;
    IQueryAssociations* qa = nullptr;
    HRESULT hr = AssocCreate(kClsidQueryAssociations,
                             kIidQueryAssociations,
                             reinterpret_cast<void**>(&qa));
    if (FAILED(hr)) {
        if (qa) qa->Release();
        return hr;
    }
    if (!qa) return E_UNEXPECTED;
    hr = qa->Init(ASSOCF_INIT_DEFAULTTOSTAR, extension, nullptr, nullptr);
    if (SUCCEEDED(hr)) {
        *result = qa;
    } else {
        qa->Release();
    }
    return hr;
}

static HRESULT STDMETHODCALLTYPE BindToHandler_Hook(
    IShellItem* self, IBindCtx* ctx, REFGUID bhid, REFIID riid, void** out) {
    if (!g_originalBind) return E_UNEXPECTED;
    if (g_inHook || !out ||
        !IsEqualGUID(bhid, kBhidAssociationArray) ||
        !IsEqualIID(riid, kIidQueryAssociations)) {
        return g_originalBind(self, ctx, bhid, riid, out);
    }

    ReentryGuard guard;
    const bool debug = g_debugLogging.load(std::memory_order_relaxed);
    const unsigned requestNumber = debug ?
        (g_assocCalls.fetch_add(1, std::memory_order_relaxed) + 1) : 0;

    std::wstring physical;
    if (!GetDeletedPhysicalPath(self, physical))
        return g_originalBind(self, ctx, bhid, riid, out);
    if (debug) g_verifiedItems.fetch_add(1, std::memory_order_relaxed);

    std::wstring extension;
    if (!GetOriginalExtension(self, physical, extension)) {
        if (debug) {
            g_skipped.fetch_add(1, std::memory_order_relaxed);
            if (requestNumber <= 30)
                Wh_Log(L"RBP: association #%u: original extension not verified", requestNumber);
        }
        return g_originalBind(self, ctx, bhid, riid, out);
    }

    // Preserve the native binding when it succeeds. The fallback must only
    // run when the Recycle Bin's association lookup fails.
    *out = nullptr;
    HRESULT nativeHr = g_originalBind(self, ctx, bhid, riid, out);
    if (SUCCEEDED(nativeHr)) {
        if (debug) {
            GUID handler{};
            if (QueryPreviewFromAssoc(static_cast<IQueryAssociations*>(*out), handler))
                g_nativeHasPreview.fetch_add(1, std::memory_order_relaxed);
            g_skipped.fetch_add(1, std::memory_order_relaxed);
            if (requestNumber <= 30)
                Wh_Log(L"RBP: association #%u ext=%s nativeHr=0x%08X: preserved",
                    requestNumber, extension.c_str(), static_cast<unsigned>(nativeHr));
        }
        return nativeHr;
    }

    IQueryAssociations* replacement = nullptr;
    HRESULT createHr = CreateExtensionAssociations(extension.c_str(), &replacement);
    if (FAILED(createHr) || !replacement) {
        if (replacement) replacement->Release();
        if (debug) {
            g_failed.fetch_add(1, std::memory_order_relaxed);
            if (requestNumber <= 30)
                Wh_Log(L"RBP: association #%u ext=%s: fallback failed hr=0x%08X",
                    requestNumber, extension.c_str(), static_cast<unsigned>(createHr));
        }
        return nativeHr;
    }

    // Diagnostics can require registry lookups. Never run these optional
    // queries in the normal Explorer hot path with Debug logging disabled.
    if (debug) {
        g_replacementCreated.fetch_add(1, std::memory_order_relaxed);
        GUID handler{};
        if (GetRegisteredPreview(extension.c_str(), handler))
            g_hasRegisteredPreview.fetch_add(1, std::memory_order_relaxed);
        const bool handlerFound = QueryPreviewFromAssoc(replacement, handler);
        if (requestNumber <= 30) {
            wchar_t contentType[128] = {};
            DWORD contentTypeLength = ARRAYSIZE(contentType);
            HRESULT mimeHr = replacement->GetString(
                ASSOCF_NONE, ASSOCSTR_CONTENTTYPE, nullptr,
                contentType, &contentTypeLength);
            Wh_Log(L"RBP: association #%u ext=%s nativeHr=0x%08X handler=%d mime=%s",
                requestNumber, extension.c_str(), static_cast<unsigned>(nativeHr),
                handlerFound ? 1 : 0,
                SUCCEEDED(mimeHr) ? contentType : L"(none)");
        }
    }

    // The failed native binding should normally return nullptr. Release an
    // unexpected non-null COM output before returning a replacement object.
    if (*out) reinterpret_cast<IUnknown*>(*out)->Release();
    *out = replacement;
    if (debug) {
        g_overrides.fetch_add(1, std::memory_order_relaxed);
        if (requestNumber <= 30)
            Wh_Log(L"RBP: association #%u replaced with normal extension associations",
                requestNumber);
    }
    return S_OK;
}

// Find the exact IShellItem implementation for deleted entries, as in v0.2.1.
// Hooks are installed once at initialization on that implementation's vtable.
static IShellItem* FindRecycleItem(unsigned* checked, unsigned* physicalOK) {
    *checked = *physicalOK = 0;
    IShellFolder* desktop = nullptr;
    IShellFolder* folder = nullptr;
    IEnumIDList* enumerator = nullptr;
    IShellItem* result = nullptr;

    HRESULT hr = SHGetDesktopFolder(&desktop);
    if (SUCCEEDED(hr) && desktop)
        hr = desktop->BindToObject(g_recycleBin, nullptr,
                                   IID_PPV_ARGS(&folder));
    if (SUCCEEDED(hr) && folder)
        hr = folder->EnumObjects(nullptr,
                                 SHCONTF_FOLDERS | SHCONTF_NONFOLDERS,
                                 &enumerator);
    if (SUCCEEDED(hr) && enumerator) {
        PITEMID_CHILD child = nullptr;
        while (*checked < 10 && enumerator->Next(1, &child, nullptr) == S_OK) {
            ++*checked;
            PIDLIST_ABSOLUTE full = ILCombine(g_recycleBin, child);
            std::wstring physical;
            if (full && GetPhysicalPathFromPidl(full, physical)) {
                ++*physicalOK;
                if (!result)
                    SHCreateItemFromIDList(full, IID_PPV_ARGS(&result));
            }
            if (full) CoTaskMemFree(full);
            CoTaskMemFree(child);
            child = nullptr;
        }
    }
    if (enumerator) enumerator->Release();
    if (folder) folder->Release();
    if (desktop) desktop->Release();
    return result;
}

BOOL Wh_ModInit() {
    LoadSettings();
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool unwindCOM = SUCCEEDED(coHr);
    if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"RBP: COM initialization failed hr=0x%08X",
               static_cast<unsigned>(coHr));
        return FALSE;
    }
    HRESULT hr = SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder,
                                        KF_FLAG_DEFAULT, nullptr,
                                        &g_recycleBin);
    if (FAILED(hr) || !g_recycleBin) {
        Wh_Log(L"RBP: failed to get Recycle Bin PIDL hr=0x%08X",
               static_cast<unsigned>(hr));
        if (g_recycleBin) {
            CoTaskMemFree(g_recycleBin);
            g_recycleBin = nullptr;
        }
        if (unwindCOM) CoUninitialize();
        return FALSE;
    }

    unsigned checked = 0, physicalOK = 0;
    IShellItem* item = FindRecycleItem(&checked, &physicalOK);
    if (g_debugLogging.load(std::memory_order_relaxed))
        Wh_Log(L"RBP: v1.0.0 probe checked=%u physical=%u fallbackOnNativeFailure=1",
               checked, physicalOK);

    // The folder item is only a fallback when the bin is empty. Its vtable
    // might differ from the child item implementation on some Windows builds.
    if (!item) {
        if (g_debugLogging.load(std::memory_order_relaxed))
            Wh_Log(L"RBP: no deleted file found during initialization; using folder fallback");
        SHCreateItemFromIDList(g_recycleBin, IID_PPV_ARGS(&item));
    }

    bool installed = false;
    if (item) {
        void** vtable = *reinterpret_cast<void***>(item);
        // IShellItem: IUnknown(0..2), BindToHandler(3).
        if (vtable && vtable[3] &&
            GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                reinterpret_cast<LPCWSTR>(vtable[3]), &g_hookModulePin)) {
            installed = Wh_SetFunctionHook(vtable[3],
                reinterpret_cast<void*>(BindToHandler_Hook),
                reinterpret_cast<void**>(&g_originalBind)) != FALSE;
            if (!installed) {
                FreeLibrary(g_hookModulePin);
                g_hookModulePin = nullptr;
            }
        }
        item->Release();
    }
    if (g_debugLogging.load(std::memory_order_relaxed) || !installed)
        Wh_Log(L"RBP: association hook installed=%d", installed ? 1 : 0);
    // Windhawk won't invoke Wh_ModUninit when Wh_ModInit returns FALSE.
    // Clean up initialization resources here on the failed path.
    if (!installed) {
        if (g_hookModulePin) {
            FreeLibrary(g_hookModulePin);
            g_hookModulePin = nullptr;
        }
        if (g_recycleBin) {
            CoTaskMemFree(g_recycleBin);
            g_recycleBin = nullptr;
        }
    }
    if (unwindCOM) CoUninitialize();
    return installed ? TRUE : FALSE;
}

void Wh_ModSettingsChanged() { LoadSettings(); }

void Wh_ModUninit() {
    if (g_debugLogging.load(std::memory_order_relaxed))
        Wh_Log(L"RBP: totals associations=%u verifiedItems=%u extensionHasPreview=%u nativeHasPreview=%u created=%u overrides=%u skipped=%u failed=%u",
            g_assocCalls.load(), g_verifiedItems.load(),
            g_hasRegisteredPreview.load(), g_nativeHasPreview.load(),
            g_replacementCreated.load(), g_overrides.load(), g_skipped.load(), g_failed.load());
    if (g_recycleBin) {
        CoTaskMemFree(g_recycleBin);
        g_recycleBin = nullptr;
    }
    if (g_hookModulePin) {
        FreeLibrary(g_hookModulePin);
        g_hookModulePin = nullptr;
    }
}
