// ==WindhawkMod==
// @id              skip-mark-of-the-web
// @name            Skip Mark of the Web on Downloads
// @description     Stops browsers from tagging selected downloads (PDF by default) as "from the Internet", so the Explorer preview pane works
// @version         1.0.0
// @author          javierpetrucci
// @github          https://github.com/javierpetrucci
// @license         MIT
// @include         chrome.exe
// @include         msedge.exe
// @include         firefox.exe
// @include         brave.exe
// @include         vivaldi.exe
// @include         opera.exe
// @include         chromium.exe
// @architecture    x86
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Skip Mark of the Web on Downloads

When a browser saves a file, Windows attaches a `Zone.Identifier` NTFS alternate
data stream to it, known as the Mark of the Web (MOTW). Because of that stream,
the File Explorer preview pane refuses to preview the file, for example a PDF in
the Downloads folder: "The file you are attempting to preview could be harmful."

This mod makes browsers skip that stream for the file extensions you choose. By
default only `pdf` is affected. Every other download is still tagged as usual.

## How it works

The mod hooks `NtCreateFile` inside the browser. When the browser opens the
`Zone.Identifier` stream of a matching file **for writing**, the open is
redirected to `\Device\Null`. The browser believes the file was tagged
successfully, so there are no errors and no retries, but the stream is never
created on disk. Reads and deletes of existing streams are not touched.

Chromium-based browsers apply the tag through `IAttachmentExecute::Save`, which
can run in a utility process, so the mod is loaded into every process of the
browser. Firefox writes the stream directly and is covered as well.

## Settings

* **Extensions**: comma-separated list without the dot, for example
  `pdf, epub, txt`. A leading dot is accepted too.
* **Only when the path contains** (optional): only skip the tag when the file
  path contains this text, case-insensitive. For example `\Downloads\`.

Executable, script, shortcut, macro-enabled and disk-image types (`exe`, `msi`,
`dll`, `bat`, `cmd`, `ps1`, `vbs`, `js`, `hta`, `lnk`, `iso`, `docm`, `xlsm`,
and similar) are always ignored, even if listed. Windows keeps tagging them.

## Notes and limitations

* Only new downloads are affected. Files that are already tagged can be
  unblocked with `Unblock-File` in PowerShell, or from the file Properties
  dialog.
* Without MOTW, Windows and Office can no longer tell that the file came from
  the Internet. Only add file types you are comfortable opening without that
  warning.
* To cover another program (for example an email or chat client), add it under
  `Details` -> `Advanced` -> `Custom process inclusion list`.
* Disable the mod to restore the default behavior. Nothing on disk is modified.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extensions: pdf
  $name: Extensions
  $description: >-
    Comma-separated list of file extensions that should not get the Mark of the
    Web, without the dot. For example, pdf, epub, txt. Executable and script
    types are always ignored.
- pathContains: ""
  $name: Only when the path contains
  $description: >-
    Optional. If not empty, the tag is skipped only when the file path contains
    this text (case-insensitive). For example, \Downloads\
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <winternl.h>

#include <cwctype>
#include <string>
#include <vector>

#ifndef FILE_OPEN
#define FILE_OPEN 0x00000001
#endif
#ifndef FILE_NON_DIRECTORY_FILE
#define FILE_NON_DIRECTORY_FILE 0x00000040
#endif
#ifndef FILE_SYNCHRONOUS_IO_ALERT
#define FILE_SYNCHRONOUS_IO_ALERT 0x00000010
#endif
#ifndef FILE_SYNCHRONOUS_IO_NONALERT
#define FILE_SYNCHRONOUS_IO_NONALERT 0x00000020
#endif
#ifndef OBJ_CASE_INSENSITIVE
#define OBJ_CASE_INSENSITIVE 0x00000040
#endif

namespace {

SRWLOCK g_lock = SRWLOCK_INIT;
std::vector<std::wstring> g_extensions;  // Lowercase, without the dot.
std::wstring g_pathFilter;               // Lowercase. Empty means any path.

constexpr WCHAR kZoneStream[] = L":Zone.Identifier";
constexpr size_t kZoneStreamLen = ARRAYSIZE(kZoneStream) - 1;
constexpr WCHAR kDataSuffix[] = L":$DATA";
constexpr size_t kDataSuffixLen = ARRAYSIZE(kDataSuffix) - 1;

// Extensions of in-progress downloads. They are skipped when looking for the
// real extension, in case the browser tags the file before renaming it.
const WCHAR* const kTempExtensions[] = {
    L"crdownload", L"part", L"download", L"tmp", L"opdownload", L"partial",
};

// Types that must always keep the Mark of the Web, even if listed in the
// settings: executables, scripts, shortcuts, macro-enabled Office files,
// installers, disk images and similar.
const WCHAR* const kAlwaysTaggedExtensions[] = {
    L"exe",   L"msi",    L"msp",   L"mst",         L"dll",        L"sys",
    L"scr",   L"com",    L"pif",   L"bat",         L"cmd",        L"ps1",
    L"psm1",  L"vbs",    L"vbe",   L"js",          L"jse",        L"wsf",
    L"wsh",   L"hta",    L"lnk",   L"url",         L"scf",        L"reg",
    L"cpl",   L"msc",    L"jar",   L"appx",        L"appxbundle", L"msix",
    L"msixbundle", L"appinstaller", L"application", L"gadget",     L"chm",
    L"iso",   L"img",    L"vhd",   L"vhdx",        L"docm",       L"dotm",
    L"xlsm",  L"xlsb",   L"xlam",  L"pptm",        L"ppam",       L"ppsm",
    L"sldm",  L"xll",    L"wll",   L"one",         L"library-ms", L"search-ms",
    L"settingcontent-ms", L"website",
};

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (WCHAR)towlower(c);
    }
    return s;
}

bool EndsWithNoCase(const WCHAR* s,
                    size_t len,
                    const WCHAR* suffix,
                    size_t suffixLen) {
    return len >= suffixLen &&
           _wcsnicmp(s + len - suffixLen, suffix, suffixLen) == 0;
}

bool IsInList(const std::wstring& ext, const WCHAR* const* list, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (ext == list[i]) {
            return true;
        }
    }
    return false;
}

std::wstring GetExtension(std::wstring path) {
    for (int pass = 0; pass < 2; pass++) {
        size_t slash = path.find_last_of(L"\\/");
        size_t dot = path.find_last_of(L'.');
        if (dot == std::wstring::npos ||
            (slash != std::wstring::npos && dot < slash)) {
            return L"";
        }

        std::wstring ext = ToLower(path.substr(dot + 1));
        if (pass == 0 &&
            IsInList(ext, kTempExtensions, ARRAYSIZE(kTempExtensions))) {
            path.resize(dot);
            continue;
        }

        return ext;
    }

    return L"";
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
            if (!current.empty()) {
                std::wstring ext = ToLower(current);
                if (IsInList(ext, kAlwaysTaggedExtensions,
                             ARRAYSIZE(kAlwaysTaggedExtensions))) {
                    Wh_Log(L"Ignoring extension that always keeps MOTW: %s",
                           ext.c_str());
                } else {
                    extensions.push_back(std::move(ext));
                }
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

    std::wstring pathFilter;
    {
        auto setting = WindhawkUtils::StringSetting::make(L"pathContains");
        pathFilter = ToLower(setting.get());
    }

    size_t count = extensions.size();

    AcquireSRWLockExclusive(&g_lock);
    g_extensions = std::move(extensions);
    g_pathFilter = std::move(pathFilter);
    ReleaseSRWLockExclusive(&g_lock);

    Wh_Log(L"Settings loaded: %zu extension(s)", count);
}

// Returns true if this open is a write to the Zone.Identifier stream of a file
// that should be left untagged. The fast path only looks at the end of the
// name, since this runs for every NtCreateFile call in the browser.
bool ShouldRedirect(POBJECT_ATTRIBUTES objectAttributes,
                    ACCESS_MASK desiredAccess,
                    ULONG createDisposition,
                    std::wstring* skippedPath) {
    PUNICODE_STRING name = objectAttributes->ObjectName;
    if (!name || !name->Buffer) {
        return false;
    }

    const WCHAR* s = name->Buffer;
    size_t len = name->Length / sizeof(WCHAR);

    if (len > kDataSuffixLen &&
        EndsWithNoCase(s, len, kDataSuffix, kDataSuffixLen)) {
        len -= kDataSuffixLen;
    }
    if (!EndsWithNoCase(s, len, kZoneStream, kZoneStreamLen)) {
        return false;
    }

    // Only writes and creations. Reads and deletions (unblocking) pass through.
    const ACCESS_MASK writeMask =
        FILE_WRITE_DATA | FILE_APPEND_DATA | GENERIC_WRITE | GENERIC_ALL;
    if (!(desiredAccess & writeMask) && createDisposition == FILE_OPEN) {
        return false;
    }

    std::wstring path(s, len - kZoneStreamLen);  // Without ":Zone.Identifier".

    // The open can be relative to a handle, which may be the file itself.
    if (objectAttributes->RootDirectory) {
        WCHAR rootPath[2048];
        DWORD rootPathLen = GetFinalPathNameByHandleW(
            (HANDLE)objectAttributes->RootDirectory, rootPath,
            ARRAYSIZE(rootPath), FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
        if (rootPathLen == 0 || rootPathLen >= ARRAYSIZE(rootPath)) {
            return false;
        }

        std::wstring fullPath(rootPath, rootPathLen);
        if (!path.empty()) {
            if (path[0] != L'\\') {
                fullPath += L'\\';
            }
            fullPath += path;
        }
        path = std::move(fullPath);
    }

    std::wstring ext = GetExtension(path);
    if (ext.empty()) {
        return false;
    }

    bool extMatch = false;
    bool pathMatch = true;

    AcquireSRWLockShared(&g_lock);
    for (const auto& e : g_extensions) {
        if (e == ext) {
            extMatch = true;
            break;
        }
    }
    if (extMatch && !g_pathFilter.empty()) {
        pathMatch = ToLower(path).find(g_pathFilter) != std::wstring::npos;
    }
    ReleaseSRWLockShared(&g_lock);

    if (!extMatch || !pathMatch) {
        return false;
    }

    *skippedPath = std::move(path);
    return true;
}

using NtCreateFile_t = NTSTATUS(NTAPI*)(PHANDLE FileHandle,
                                        ACCESS_MASK DesiredAccess,
                                        POBJECT_ATTRIBUTES ObjectAttributes,
                                        PIO_STATUS_BLOCK IoStatusBlock,
                                        PLARGE_INTEGER AllocationSize,
                                        ULONG FileAttributes,
                                        ULONG ShareAccess,
                                        ULONG CreateDisposition,
                                        ULONG CreateOptions,
                                        PVOID EaBuffer,
                                        ULONG EaLength);
NtCreateFile_t NtCreateFile_Original;

NTSTATUS NTAPI NtCreateFile_Hook(PHANDLE FileHandle,
                                 ACCESS_MASK DesiredAccess,
                                 POBJECT_ATTRIBUTES ObjectAttributes,
                                 PIO_STATUS_BLOCK IoStatusBlock,
                                 PLARGE_INTEGER AllocationSize,
                                 ULONG FileAttributes,
                                 ULONG ShareAccess,
                                 ULONG CreateDisposition,
                                 ULONG CreateOptions,
                                 PVOID EaBuffer,
                                 ULONG EaLength) {
    std::wstring skippedPath;
    if (ObjectAttributes &&
        ShouldRedirect(ObjectAttributes, DesiredAccess, CreateDisposition,
                       &skippedPath)) {
        static WCHAR nullDevicePath[] = L"\\Device\\Null";

        UNICODE_STRING nullName;
        nullName.Buffer = nullDevicePath;
        nullName.Length = (USHORT)(sizeof(nullDevicePath) - sizeof(WCHAR));
        nullName.MaximumLength = (USHORT)sizeof(nullDevicePath);

        OBJECT_ATTRIBUTES nullAttributes = {};
        nullAttributes.Length = sizeof(nullAttributes);
        nullAttributes.ObjectName = &nullName;
        nullAttributes.Attributes = OBJ_CASE_INSENSITIVE;

        ACCESS_MASK access =
            DesiredAccess & ~(DELETE | WRITE_DAC | WRITE_OWNER);
        ULONG options = (CreateOptions & (FILE_SYNCHRONOUS_IO_ALERT |
                                          FILE_SYNCHRONOUS_IO_NONALERT)) |
                        FILE_NON_DIRECTORY_FILE;

        NTSTATUS status = NtCreateFile_Original(
            FileHandle, access, &nullAttributes, IoStatusBlock, nullptr,
            FILE_ATTRIBUTE_NORMAL, ShareAccess, FILE_OPEN, options, nullptr,
            0);
        if (NT_SUCCESS(status)) {
            Wh_Log(L"Skipped MOTW: %s", skippedPath.c_str());
            return status;
        }

        // Redirecting failed, fall back to the normal behavior.
    }

    return NtCreateFile_Original(FileHandle, DesiredAccess, ObjectAttributes,
                                 IoStatusBlock, AllocationSize, FileAttributes,
                                 ShareAccess, CreateDisposition, CreateOptions,
                                 EaBuffer, EaLength);
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    void* pNtCreateFile =
        ntdll ? (void*)GetProcAddress(ntdll, "NtCreateFile") : nullptr;
    if (!pNtCreateFile) {
        Wh_Log(L"Failed to find ntdll.NtCreateFile");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(pNtCreateFile, (void*)NtCreateFile_Hook,
                            (void**)&NtCreateFile_Original)) {
        Wh_Log(L"Failed to hook NtCreateFile");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
