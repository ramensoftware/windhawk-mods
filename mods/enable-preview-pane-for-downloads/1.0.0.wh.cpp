// ==WindhawkMod==
// @id              enable-preview-pane-for-downloads
// @name            Enable Preview Pane for Downloaded Files
// @description     Lets the File Explorer preview pane show selected file types (PDF by default) that are marked as downloaded from the Internet, without removing the mark
// @version         1.0.0
// @author          javierpetrucci
// @github          https://github.com/javierpetrucci
// @homepage        https://github.com/javierpetrucci/windhawk-enable-preview-pane-for-downloads
// @license         MIT
// @include         explorer.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -lurlmon
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Enable Preview Pane for Downloaded Files

Files saved from a browser, an email client or a chat app usually get a
`Zone.Identifier` NTFS alternate data stream, known as the Mark of the Web
(MOTW), which places them in the Internet security zone. File Explorer refuses
to show such files in the preview pane: "The file you are attempting to preview
could harm your computer."

This mod lets the preview pane show files in the Internet zone for the file
extensions you choose. By default only `pdf` is affected.

Before, a PDF with the Mark of the Web shows the warning:

![Preview pane showing the warning](https://raw.githubusercontent.com/javierpetrucci/windhawk-enable-preview-pane-for-downloads/main/images/before.png)

After, the same PDF is previewed:

![Preview pane showing the PDF](https://raw.githubusercontent.com/javierpetrucci/windhawk-enable-preview-pane-for-downloads/main/images/after.png)

The Mark of the Web is **not** removed or modified. Files keep their
`Zone.Identifier` stream, so SmartScreen, Office Protected View, Microsoft
Defender and every other program that checks the mark behave as before.

## How it works

Before the preview pane loads a preview handler, File Explorer asks the URL
security manager (`IInternetSecurityManager::ProcessUrlAction`) whether the
`URLACTION_SHELL_PREVIEW` action is allowed for the file. In the Internet zone
the answer is "disallow", and the warning is shown instead of the preview.

The mod hooks that method inside `explorer.exe`. When the original answer is
not "allow", the answer is changed to "allow" only if all of these are true:

* The action is `URLACTION_SHELL_PREVIEW`. No other security action is changed.
* The file is on a local fixed, removable or RAM drive (`C:\...`). Network
  shares (`\\server\...`) and mapped network drives are never exempted.
* The path has no alternate data stream, the file itself is not a symbolic
  link or another type of reparse point, and no folder in the path is a
  directory symbolic link. Cloud placeholder files, such as OneDrive files,
  and directory junctions are allowed.
* The extension is in the list from the settings.
* The security manager maps the file to the Internet zone (zone 3). Files in
  the Restricted Sites zone, or in any other zone, are left alone.

## Settings

* **Extensions**: comma-separated list without the dot, for example
  `pdf, txt, png`. A leading dot is accepted too. Matching is not case
  sensitive.
* **Only when the path contains** (optional): only exempt files whose path
  contains this text, not case sensitive. For example `\Downloads\`.

## Types that are never exempted

The following extensions are ignored even if they are listed in the settings:

* Executables, libraries and installers: `exe`, `com`, `scr`, `pif`, `cpl`,
  `dll`, `ocx`, `sys`, `drv`, `efi`, `msi`, `msp`, `mst`, `msu`, `appx`,
  `appxbundle`, `msix`, `msixbundle`, `appinstaller`, `application`,
  `appref-ms`, `xbap`, `gadget`, `jar`, `msc`.
* Scripts and command files: `bat`, `cmd`, `ps1`, `psm1`, `psd1`, `ps1xml`,
  `pssc`, `psc1`, `vb`, `vbs`, `vbe`, `js`, `jse`, `ws`, `wsf`, `wsc`, `wsh`,
  `hta`, `sct`, `inf`, `ins`, `isp`, `reg`, `scf`, `shb`, `shs`, `mof`, `hlp`,
  `chm`, `diagcab`, `diagcfg`, `diagpkg`, `rdp`, `py`, `pyw`.
* Shortcuts and shell links: `lnk`, `url`, `website`, `library-ms`,
  `search-ms`, `searchconnector-ms`, `settingcontent-ms`.
* Office files with macros, including legacy formats that can carry VBA:
  `docm`, `dotm`, `xlsm`, `xltm`, `xlsb`, `xlam`, `pptm`, `potm`, `ppsm`,
  `ppam`, `sldm`, `doc`, `dot`, `xls`, `xlt`, `xla`, `ppt`, `pot`, `pps`,
  `ppa`, `mdb`, `mde`, `accdb`, `accde`, `ade`, `adp`, `xll`, `wll`, `one`.
* Archives: `zip`, `7z`, `rar`, `cab`, `tar`, `gz`, `tgz`, `bz2`, `tbz2`, `xz`,
  `txz`, `zst`, `lzh`, `lha`, `arj`, `z`, `wim`.
* Disk images: `iso`, `img`, `vhd`, `vhdx`, `vmdk`.

This list is a safety net, not a complete security policy. Only add file types
that you are comfortable previewing from untrusted sources.

## Notes and limitations

* Windows blocks these previews because they can leak credentials. A file
  that refers to a remote resource (`\\server\...`) can make the preview
  handler connect to that server and send it the user's NTLM hash. PDFs can
  do this, and `pdf` is the default. See Microsoft's
  [note on the change](https://support.microsoft.com/help/5070960). With the
  mod, previewing a listed file can leak the hash this way, just like opening
  it would.
* The preview handler registered for the type (for example the PDF previewer
  of Microsoft Edge, Adobe Acrobat or SumatraPDF) also parses the file, so a
  vulnerable preview handler can be exploited by a malicious file.
* A preview handler can opt in to previewing untrusted files with the
  `AutomaticallyPreviewUntrustedFiles` value on its CLSID key. Windows then
  shows the preview without this mod. The PDF previewer of Microsoft Edge does
  this. The PDF preview handlers of Adobe Acrobat Reader and SumatraPDF don't,
  so they need the mod.
* Tested by the author on Windows 11 build 26300 with the PDF preview handlers
  of SumatraPDF, Adobe Acrobat Reader and Microsoft Edge.
* Only the File Explorer preview pane in `explorer.exe` is covered. The preview
  pane in the Open and Save dialogs of other programs runs inside those
  programs. To cover one, add it under `Details` -> `Advanced` ->
  `Custom process inclusion list`. This was tested with the Open dialog of a
  program added to that list.
* Files reached through a directory symbolic link are never exempted, because
  the link can point to a network share. Directory junctions can only point to
  local folders, so they are allowed. Hard links are the same file. File
  Explorer doesn't preview file symbolic links at all, with or without the
  mod.
* The mod overrides the "disallow" answer for the listed types even when it
  comes from a Group Policy setting for the Internet zone.
* This targets the standard URL security manager of Windows (`urlmon.dll`). If
  a Windows update makes the preview pane use a different check, the mod has no
  effect and the warning is shown as usual.
* Disable the mod to restore the default behavior. Nothing on disk or in the
  registry is modified.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extensions: pdf
  $name: Extensions
  $description: >-
    Comma-separated list of file extensions to preview even when the file is
    marked as downloaded from the Internet, without the dot. For example, pdf,
    txt, png. Executable, script, macro-enabled, archive and similar types are
    always ignored.
- pathContains: ""
  $name: Only when the path contains
  $description: >-
    Optional. If not empty, only files whose path contains this text are
    exempted (not case sensitive). For example, \Downloads\
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shlwapi.h>
#include <urlmon.h>

#include <cstring>
#include <cwchar>
#include <string>
#include <vector>

namespace {

SRWLOCK g_lock = SRWLOCK_INIT;
std::vector<std::wstring> g_extensions;  // Without the dot.
std::wstring g_pathFilter;               // Empty means any path.

// Set while the hook calls back into the security manager.
thread_local bool g_insideHook = false;

// Types that are never exempted, even if listed in the settings.
const WCHAR* const kNeverExemptedExtensions[] = {
    // Executables, libraries and installers.
    L"exe", L"com", L"scr", L"pif", L"cpl", L"dll", L"ocx", L"sys", L"drv",
    L"efi", L"msi", L"msp", L"mst", L"msu", L"appx", L"appxbundle", L"msix",
    L"msixbundle", L"appinstaller", L"application", L"appref-ms", L"xbap",
    L"gadget", L"jar", L"msc",
    // Scripts and command files.
    L"bat", L"cmd", L"ps1", L"psm1", L"psd1", L"ps1xml", L"pssc", L"psc1",
    L"vb", L"vbs", L"vbe", L"js", L"jse", L"ws", L"wsf", L"wsc", L"wsh",
    L"hta", L"sct", L"inf", L"ins", L"isp", L"reg", L"scf", L"shb", L"shs",
    L"mof", L"hlp", L"chm", L"diagcab", L"diagcfg", L"diagpkg", L"rdp", L"py",
    L"pyw",
    // Shortcuts and shell links.
    L"lnk", L"url", L"website", L"library-ms", L"search-ms",
    L"searchconnector-ms", L"settingcontent-ms",
    // Office files with macros, and legacy formats that can carry VBA.
    L"docm", L"dotm", L"xlsm", L"xltm", L"xlsb", L"xlam", L"pptm", L"potm",
    L"ppsm", L"ppam", L"sldm", L"doc", L"dot", L"xls", L"xlt", L"xla", L"ppt",
    L"pot", L"pps", L"ppa", L"mdb", L"mde", L"accdb", L"accde", L"ade", L"adp",
    L"xll", L"wll", L"one",
    // Archives.
    L"zip", L"7z", L"rar", L"cab", L"tar", L"gz", L"tgz", L"bz2", L"tbz2",
    L"xz", L"txz", L"zst", L"lzh", L"lha", L"arj", L"z", L"wim",
    // Disk images.
    L"iso", L"img", L"vhd", L"vhdx", L"vmdk",
};

bool EqualsNoCase(const WCHAR* a, int aLen, const WCHAR* b, int bLen) {
    return CompareStringOrdinal(a, aLen, b, bLen, TRUE) == CSTR_EQUAL;
}

bool IsNeverExempted(const WCHAR* ext, int extLen) {
    for (const WCHAR* item : kNeverExemptedExtensions) {
        if (EqualsNoCase(ext, extLen, item, -1)) {
            return true;
        }
    }
    return false;
}

void LoadSettings() {
    std::vector<std::wstring> extensions;

    {
        auto setting = WindhawkUtils::StringSetting::make(L"extensions");
        std::wstring current;

        auto flush = [&]() {
            if (!current.empty() && current[0] == L'.') {
                current.erase(0, 1);
            }
            if (current.empty()) {
                return;
            }
            if (IsNeverExempted(current.c_str(), (int)current.size())) {
                Wh_Log(L"Ignoring extension that is never exempted: %s",
                       current.c_str());
            } else {
                extensions.push_back(current);
            }
            current.clear();
        };

        for (PCWSTR p = setting.get(); *p; p++) {
            if (*p == L',' || *p == L';' || *p == L' ' || *p == L'\t' ||
                *p == L'\r' || *p == L'\n') {
                flush();
            } else {
                current += *p;
            }
        }
        flush();
    }

    std::wstring pathFilter =
        WindhawkUtils::StringSetting::make(L"pathContains").get();

    size_t count = extensions.size();

    AcquireSRWLockExclusive(&g_lock);
    g_extensions = std::move(extensions);
    g_pathFilter = std::move(pathFilter);
    ReleaseSRWLockExclusive(&g_lock);

    Wh_Log(L"Settings loaded: %zu extension(s)", count);
}

// Returns true if the path matches the extension list and the path filter.
bool MatchesSettings(const WCHAR* path) {
    const WCHAR* name = path;
    for (const WCHAR* p = path; *p; p++) {
        if (*p == L'\\' || *p == L'/') {
            name = p + 1;
        }
    }

    const WCHAR* dot = wcsrchr(name, L'.');
    if (!dot || !dot[1]) {
        return false;
    }

    const WCHAR* ext = dot + 1;
    int extLen = (int)wcslen(ext);
    if (IsNeverExempted(ext, extLen)) {
        return false;
    }

    bool extMatch = false;
    bool pathMatch = true;

    AcquireSRWLockShared(&g_lock);
    for (const auto& item : g_extensions) {
        if (EqualsNoCase(ext, extLen, item.c_str(), (int)item.size())) {
            extMatch = true;
            break;
        }
    }
    if (extMatch && !g_pathFilter.empty()) {
        pathMatch = FindStringOrdinal(FIND_FROMSTART, path, -1,
                                      g_pathFilter.c_str(),
                                      (int)g_pathFilter.size(), TRUE) >= 0;
    }
    ReleaseSRWLockShared(&g_lock);

    return extMatch && pathMatch;
}

// Reads the directory entry of a file or folder. The item is not opened, and a
// reparse point is not followed.
bool GetDirectoryEntry(const WCHAR* path, WIN32_FIND_DATAW* findData) {
    HANDLE find = FindFirstFileExW(path, FindExInfoBasic, findData,
                                   FindExSearchNameMatch, nullptr, 0);
    if (find == INVALID_HANDLE_VALUE) {
        return false;
    }
    FindClose(find);
    return true;
}

// Returns true if no folder in an "X:\..." path is a directory symbolic link,
// which can point to a network share. The folders are checked from the drive
// root down, and the check stops at the first link, so nothing behind it is
// accessed.
bool HasNoSymbolicLinkFolders(const WCHAR* path) {
    std::wstring folder(path, 3);
    const WCHAR* name = path + 3;
    for (;;) {
        const WCHAR* end = name;
        while (*end && *end != L'\\' && *end != L'/') {
            end++;
        }
        if (!*end) {
            // The last component is the file itself.
            return true;
        }

        size_t length = end - name;
        if (length == 0 || (length == 1 && name[0] == L'.') ||
            (length == 2 && name[0] == L'.' && name[1] == L'.')) {
            return false;
        }

        folder.append(name, length);
        WIN32_FIND_DATAW findData;
        if (!GetDirectoryEntry(folder.c_str(), &findData)) {
            return false;
        }
        if ((findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
            findData.dwReserved0 == IO_REPARSE_TAG_SYMLINK) {
            return false;
        }

        folder += L'\\';
        name = end + 1;
    }
}

// Returns true if the path is a plain file on a local drive.
bool IsLocalPlainFile(const WCHAR* path) {
    // Only "X:\..." paths. UNC paths, device paths and relative paths are
    // rejected.
    if (!((path[0] >= L'A' && path[0] <= L'Z') ||
          (path[0] >= L'a' && path[0] <= L'z')) ||
        path[1] != L':' || (path[2] != L'\\' && path[2] != L'/')) {
        return false;
    }

    // No alternate data streams, wildcards or other unusual characters.
    if (wcspbrk(path + 2, L":*?\"<>|")) {
        return false;
    }

    WCHAR root[] = {path[0], L':', L'\\', L'\0'};
    switch (GetDriveTypeW(root)) {
        case DRIVE_FIXED:
        case DRIVE_REMOVABLE:
        case DRIVE_RAMDISK:
            break;
        default:
            return false;
    }

    if (!HasNoSymbolicLinkFolders(path)) {
        return false;
    }

    WIN32_FIND_DATAW findData;
    if (!GetDirectoryEntry(path, &findData)) {
        return false;
    }

    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        return false;
    }

    if (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        // Allow cloud placeholder files (OneDrive and similar), reject
        // symbolic links, mount points and other redirections.
        DWORD tag = findData.dwReserved0;
        if ((tag & 0xFFFF0FFF) != (DWORD)IO_REPARSE_TAG_CLOUD) {
            return false;
        }
    }

    return true;
}

// Converts a file URL or a path to a local file path. Returns an empty string
// for anything else.
std::wstring GetLocalPath(LPCWSTR url) {
    if (_wcsnicmp(url, L"file:", 5) != 0) {
        return url;
    }

    PWSTR path = nullptr;
    if (FAILED(PathCreateFromUrlAlloc(url, &path, 0)) || !path) {
        return std::wstring();
    }

    std::wstring result(path);
    LocalFree(path);
    return result;
}

using ProcessUrlAction_t = HRESULT(STDMETHODCALLTYPE*)(
    IInternetSecurityManager* This,
    LPCWSTR pwszUrl,
    DWORD dwAction,
    BYTE* pPolicy,
    DWORD cbPolicy,
    BYTE* pContext,
    DWORD cbContext,
    DWORD dwFlags,
    DWORD dwReserved);
ProcessUrlAction_t ProcessUrlAction_Original;

HRESULT STDMETHODCALLTYPE ProcessUrlAction_Hook(IInternetSecurityManager* This,
                                                LPCWSTR pwszUrl,
                                                DWORD dwAction,
                                                BYTE* pPolicy,
                                                DWORD cbPolicy,
                                                BYTE* pContext,
                                                DWORD cbContext,
                                                DWORD dwFlags,
                                                DWORD dwReserved) {
    HRESULT hr = ProcessUrlAction_Original(This, pwszUrl, dwAction, pPolicy,
                                           cbPolicy, pContext, cbContext,
                                           dwFlags, dwReserved);

    if (dwAction != URLACTION_SHELL_PREVIEW || g_insideHook || FAILED(hr) ||
        !This || !pwszUrl || !pPolicy || cbPolicy < sizeof(DWORD)) {
        return hr;
    }

    DWORD policy;
    memcpy(&policy, pPolicy, sizeof(policy));
    if (GetUrlPolicyPermissions(policy) == URLPOLICY_ALLOW) {
        return hr;
    }

    std::wstring path = GetLocalPath(pwszUrl);
    if (path.empty() || !MatchesSettings(path.c_str()) ||
        !IsLocalPlainFile(path.c_str())) {
        return hr;
    }

    // Only the Internet zone. The Restricted Sites zone, and files whose zone
    // can't be determined, keep the original answer.
    DWORD zone = URLZONE_INVALID;
    g_insideHook = true;
    HRESULT zoneHr = This->MapUrlToZone(pwszUrl, &zone, 0);
    g_insideHook = false;
    if (FAILED(zoneHr) || zone != URLZONE_INTERNET) {
        return hr;
    }

    policy = URLPOLICY_ALLOW;
    memcpy(pPolicy, &policy, sizeof(policy));

    Wh_Log(L"Allowed preview: %s", path.c_str());
    return S_OK;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(coInit) && coInit != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", (unsigned)coInit);
        return FALSE;
    }

    // The standard security manager of urlmon.dll is used both by
    // CoInternetCreateSecurityManager and by CoCreateInstance with
    // CLSID_InternetSecurityManager. Hooking the method it points to covers
    // every instance in the process.
    IInternetSecurityManager* manager = nullptr;
    HRESULT hr = CoInternetCreateSecurityManager(nullptr, &manager, 0);

    ProcessUrlAction_t target = nullptr;
    if (SUCCEEDED(hr) && manager) {
        // IUnknown (0-2), SetSecuritySite (3), GetSecuritySite (4),
        // MapUrlToZone (5), GetSecurityId (6), ProcessUrlAction (7).
        void** vtable = *reinterpret_cast<void***>(manager);
        target = reinterpret_cast<ProcessUrlAction_t>(vtable[7]);
        manager->Release();
    } else {
        Wh_Log(L"CoInternetCreateSecurityManager failed: 0x%08X",
               (unsigned)hr);
    }

    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }

    if (!target) {
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(target, ProcessUrlAction_Hook,
                                        &ProcessUrlAction_Original)) {
        Wh_Log(L"Failed to hook ProcessUrlAction");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
