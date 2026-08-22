// ==WindhawkMod==
// @id              start-menu-open-real-file-location
// @name            Start Menu Open Real File Location
// @description     Makes Open file location select a Start menu shortcut's target instead of the shortcut itself
// @version         1.1.0
// @author          Alchemy
// @github          https://github.com/alchemyyy
// @license         MIT
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         SearchApp.exe
// @include         RuntimeBroker.exe
// @architecture    x86-64
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Open Real File Location

Changes `Open file location` for traditional desktop programs in the Start menu and Windows Search. Instead of opening the Start Menu Programs directory and selecting the program's `.lnk` file, Explorer opens the shortcut target's directory and selects the target.

The mod reads only the target stored in the shortcut. It doesn't search for a moved target or display link-resolution dialogs.

## Current Limitations and TODOs

- Packaged Microsoft Store apps don't normally have a conventional shortcut target and are left unchanged for now.
- If the target can't be read, Windows retains its original behavior and opens the shortcut's location.
- The direct shortcut target is selected. For wrapper shortcuts, this can be a launcher, script host, or another intermediate executable.
- There is currently no program blacklist, so shortcuts that point to common wrappers or launchers will be resolved there.
*/
// ==/WindhawkModReadme==

#include <windows.h>

#include <shlobj.h>
#include <shobjidl.h>
#include <wrl/client.h>

#include <memory>

#include <windhawk_utils.h>

namespace {

using SHOpenFolderAndSelectItemsFunction = HRESULT(WINAPI*)(
    PCIDLIST_ABSOLUTE folderPidl,
    UINT childCount,
    PCUITEMID_CHILD_ARRAY childPidls,
    DWORD flags);

struct CoTaskMemoryDeleter {
    void operator()(void* memory) const noexcept {
        CoTaskMemFree(memory);
    }
};

using UniquePIDL = std::unique_ptr<ITEMIDLIST, CoTaskMemoryDeleter>;
using UniqueWideString = std::unique_ptr<WCHAR, CoTaskMemoryDeleter>;

SHOpenFolderAndSelectItemsFunction s_windowsStorageOriginal = nullptr;
SHOpenFolderAndSelectItemsFunction s_shell32Original = nullptr;
HMODULE s_windowsStorageModule = nullptr;
bool s_isSearchProcess = false;

void ReleaseWindowsStorageModule() {
    if (!s_windowsStorageModule) {
        return;
    }

    FreeLibrary(s_windowsStorageModule);
    s_windowsStorageModule = nullptr;
}

bool IsAppResolverCaller(void* returnAddress) {
    HMODULE callerModule = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(returnAddress), &callerModule)) {
        return false;
    }

    HMODULE appResolverModule = GetModuleHandleW(L"appresolver.dll");
    return appResolverModule && callerModule == appResolverModule;
}

UniquePIDL ResolveShellLinkTarget(PCIDLIST_ABSOLUTE shortcutPidl) {
    Microsoft::WRL::ComPtr<IShellFolder> parentFolder;
    PCUITEMID_CHILD shortcutChildPidl = nullptr;
    HRESULT result = SHBindToParent(
        shortcutPidl, IID_PPV_ARGS(parentFolder.GetAddressOf()),
        &shortcutChildPidl);
    if (FAILED(result) || !parentFolder || !shortcutChildPidl) {
        return nullptr;
    }

    Microsoft::WRL::ComPtr<IShellLinkW> shellLink;
    result = parentFolder->GetUIObjectOf(
        nullptr, 1, &shortcutChildPidl, __uuidof(IShellLinkW), nullptr,
        reinterpret_cast<void**>(shellLink.GetAddressOf()));
    if (FAILED(result) || !shellLink) {
        return nullptr;
    }

    PIDLIST_ABSOLUTE targetPidl = nullptr;
    result = shellLink->GetIDList(&targetPidl);
    if (SUCCEEDED(result) && targetPidl) {
        return UniquePIDL(targetPidl);
    }
    CoTaskMemFree(targetPidl);

    WCHAR rawTargetPath[MAX_PATH] = {};
    result = shellLink->GetPath(rawTargetPath, ARRAYSIZE(rawTargetPath),
                                nullptr, SLGP_RAWPATH);
    if (FAILED(result) || !rawTargetPath[0]) {
        return nullptr;
    }

    WCHAR expandedTargetPath[MAX_PATH] = {};
    PCWSTR targetPath = rawTargetPath;
    DWORD expandedLength = ExpandEnvironmentStringsW(
        rawTargetPath, expandedTargetPath, ARRAYSIZE(expandedTargetPath));
    if (expandedLength > 0 &&
        expandedLength <= ARRAYSIZE(expandedTargetPath)) {
        targetPath = expandedTargetPath;
    }

    targetPidl = nullptr;
    result = SHParseDisplayName(targetPath, nullptr, &targetPidl, 0, nullptr);
    if (FAILED(result)) {
        CoTaskMemFree(targetPidl);
        return nullptr;
    }

    return UniquePIDL(targetPidl);
}

void LogRedirect(PCIDLIST_ABSOLUTE shortcutPidl,
                 PCIDLIST_ABSOLUTE targetPidl) {
    PWSTR shortcutName = nullptr;
    PWSTR targetName = nullptr;

    SHGetNameFromIDList(shortcutPidl, SIGDN_DESKTOPABSOLUTEPARSING,
                        &shortcutName);
    SHGetNameFromIDList(targetPidl, SIGDN_DESKTOPABSOLUTEPARSING,
                        &targetName);

    UniqueWideString shortcutNameOwner(shortcutName);
    UniqueWideString targetNameOwner(targetName);
    Wh_Log(L"Redirecting Open file location: %s -> %s",
           shortcutName ? shortcutName : L"(unknown shortcut)",
           targetName ? targetName : L"(unknown target)");
}

HRESULT HandleSHOpenFolderAndSelectItems(
    SHOpenFolderAndSelectItemsFunction originalFunction,
    PCIDLIST_ABSOLUTE folderPidl,
    UINT childCount,
    PCUITEMID_CHILD_ARRAY childPidls,
    DWORD flags,
    void* returnAddress) {
    if (!folderPidl || childCount != 0 || childPidls ||
        (!s_isSearchProcess && !IsAppResolverCaller(returnAddress))) {
        return originalFunction(folderPidl, childCount, childPidls, flags);
    }

    UniquePIDL targetPidl = ResolveShellLinkTarget(folderPidl);
    if (!targetPidl || ILIsEqual(folderPidl, targetPidl.get())) {
        return originalFunction(folderPidl, childCount, childPidls, flags);
    }

    LogRedirect(folderPidl, targetPidl.get());
    HRESULT result = originalFunction(targetPidl.get(), 0, nullptr, flags);

    if (FAILED(result)) {
        Wh_Log(L"Opening the shortcut target failed: 0x%08X; using the "
               L"original shortcut",
               static_cast<unsigned int>(result));
        return originalFunction(folderPidl, childCount, childPidls, flags);
    }

    return result;
}

HRESULT WINAPI WindowsStorageSHOpenFolderAndSelectItemsHook(
    PCIDLIST_ABSOLUTE folderPidl,
    UINT childCount,
    PCUITEMID_CHILD_ARRAY childPidls,
    DWORD flags) {
    return HandleSHOpenFolderAndSelectItems(
        s_windowsStorageOriginal, folderPidl, childCount, childPidls, flags,
        __builtin_return_address(0));
}

HRESULT WINAPI Shell32SHOpenFolderAndSelectItemsHook(
    PCIDLIST_ABSOLUTE folderPidl,
    UINT childCount,
    PCUITEMID_CHILD_ARRAY childPidls,
    DWORD flags) {
    return HandleSHOpenFolderAndSelectItems(
        s_shell32Original, folderPidl, childCount, childPidls, flags,
        __builtin_return_address(0));
}

}  // namespace

BOOL Wh_ModInit() {
    s_isSearchProcess = GetModuleHandleW(L"SearchHost.exe") ||
                        GetModuleHandleW(L"SearchApp.exe");

    s_windowsStorageModule = LoadLibraryExW(
        L"Windows.Storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE shell32Module = GetModuleHandleW(L"shell32.dll");

    FARPROC windowsStorageProcedure =
        s_windowsStorageModule
            ? GetProcAddress(s_windowsStorageModule,
                             "SHOpenFolderAndSelectItems")
            : nullptr;
    FARPROC shell32Procedure =
        shell32Module
            ? GetProcAddress(shell32Module, "SHOpenFolderAndSelectItems")
            : nullptr;

    bool hookInstalled = false;
    if (windowsStorageProcedure) {
        hookInstalled = WindhawkUtils::SetFunctionHook(
            reinterpret_cast<SHOpenFolderAndSelectItemsFunction>(
                windowsStorageProcedure),
            WindowsStorageSHOpenFolderAndSelectItemsHook,
            &s_windowsStorageOriginal);
        if (hookInstalled) {
            Wh_Log(
                L"Hooked Windows.Storage.dll!SHOpenFolderAndSelectItems");
        } else {
            Wh_Log(
                L"Failed to hook Windows.Storage.dll!"
                L"SHOpenFolderAndSelectItems");
        }
    }

    if (shell32Procedure &&
        shell32Procedure != windowsStorageProcedure) {
        bool shell32HookInstalled = WindhawkUtils::SetFunctionHook(
            reinterpret_cast<SHOpenFolderAndSelectItemsFunction>(
                shell32Procedure),
            Shell32SHOpenFolderAndSelectItemsHook, &s_shell32Original);
        hookInstalled = hookInstalled || shell32HookInstalled;
        if (shell32HookInstalled) {
            Wh_Log(L"Hooked shell32.dll!SHOpenFolderAndSelectItems");
        } else {
            Wh_Log(
                L"Failed to hook shell32.dll!"
                L"SHOpenFolderAndSelectItems");
        }
    }

    if (!hookInstalled) {
        Wh_Log(L"No SHOpenFolderAndSelectItems implementation was hooked");
        ReleaseWindowsStorageModule();
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    ReleaseWindowsStorageModule();
}
