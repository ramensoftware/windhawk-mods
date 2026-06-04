// ==WindhawkMod==
// @id              unlock-office-file-upload
// @name            Unlock Office Files for Upload
// @description     Transparently copies locked Office files (docx, xlsx, pptx, etc.) to a temp location when an upload or send is attempted while they're open in Word/Excel/PowerPoint. The OS deletes the copy automatically via FILE_FLAG_DELETE_ON_CLOSE.
// @version         0.2
// @author          Loerei
// @github          https://github.com/loerei
// @homepage        https://github.com/loerei/windhawk-unlock-office-file-upload
// @include         *
// @exclude         SearchIndexer.exe
// @exclude         explorer.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Unlock In-Use Office Files for Upload

When you try to upload or attach a `.docx`, `.xlsx`, `.pptx` (or their legacy
variants) while the file is open in Word/Excel/PowerPoint, you get:

> **This file is in use. Enter a new name or close the file that's open in
> another program.**

This mod silently intercepts that failure in the uploading process (Chrome,
Edge, Outlook, Teams, Discord, etc.), makes a temporary copy of the file, and
redirects the open call to the copy. The OS deletes the copy automatically when
the caller closes the handle (`FILE_FLAG_DELETE_ON_CLOSE`) — even on process
crash or abnormal exit.

**What you get:** The uploader receives the last-saved version of the file with
no prompts and no need to close Word first.

**Limitation:** The copy reflects the last *saved* state. Unsaved in-memory
edits in Word are not included — this is inherent to how Windows file locking
works.

**Scope note:** The mod injects into all processes (required since the uploader
can be any app). `SearchIndexer.exe` and `explorer.exe` are excluded via
`@exclude`. Session 0 (system service) processes are skipped at init time. The
`CreateFileW` hook has near-zero overhead on the success path — it returns
immediately when the original call succeeds.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extensions:
  - .docx
  - .docm
  - .doc
  - .xlsx
  - .xlsm
  - .xls
  - .pptx
  - .pptm
  - .ppt
  $name: File extensions to unlock
  $description: List of extensions (with leading dot, lowercase) that will be transparently copied when locked.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <cwctype>
#include <vector>
#include <string>
#include <algorithm>
#include <shared_mutex>

// ---------------------------------------------------------------------------
// Settings — guarded by a shared_mutex so LoadSettings (unique/writer lock)
// and IsTrackedExtension (shared/reader lock, called from the CreateFileW
// hook on arbitrary threads) don't race.
// ---------------------------------------------------------------------------

std::vector<std::wstring> g_extensions;
std::shared_mutex g_extensionsMutex;

void LoadSettings() {
    std::vector<std::wstring> fresh;

    int count = 0;
    while (true) {
        auto key = std::wstring(L"extensions[") + std::to_wstring(count) + L"]";
        PCWSTR val = Wh_GetStringSetting(key.c_str());
        // Wh_GetStringSetting never returns NULL; it returns L"" when unset.
        if (val[0] == L'\0') {
            Wh_FreeStringSetting(val);
            break;
        }
        std::wstring ext(val);
        Wh_FreeStringSetting(val);
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
        fresh.push_back(std::move(ext));
        ++count;
    }

    // Fallback defaults if settings are empty (first run / no settings file).
    if (fresh.empty()) {
        for (auto* e : {L".docx", L".docm", L".doc",
                        L".xlsx", L".xlsm", L".xls",
                        L".pptx", L".pptm", L".ppt"}) {
            fresh.push_back(e);
        }
    }

    std::unique_lock lock(g_extensionsMutex);
    g_extensions = std::move(fresh);
}

static bool IsTrackedExtension(const std::wstring& ext) {
    std::shared_lock lock(g_extensionsMutex);
    for (const auto& e : g_extensions) {
        if (e == ext)
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Returns the lowercase file extension including the dot, or empty string.
static std::wstring GetExtension(LPCWSTR path) {
    if (!path)
        return {};
    LPCWSTR dot = wcsrchr(path, L'.');
    if (!dot)
        return {};
    // Ensure the dot comes after the last path separator.
    LPCWSTR sep = wcsrchr(path, L'\\');
    if (!sep) sep = wcsrchr(path, L'/');
    if (sep && dot < sep)
        return {};
    std::wstring ext(dot);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return ext;
}

// Produces a guaranteed-unique temp path via GetTempFileNameW (uUnique=0).
// GetTempFileNameW creates an empty placeholder file; CopyFileW with
// bFailIfExists=FALSE overwrites it safely.
static bool MakeTempPath(wchar_t (&out)[MAX_PATH]) {
    wchar_t tempDir[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, tempDir))
        return false;
    // Prefix is capped at 3 characters; "ul_" fits exactly.
    return GetTempFileNameW(tempDir, L"ul_", 0, out) != 0;
}

// ---------------------------------------------------------------------------
// CreateFileW hook
// ---------------------------------------------------------------------------

using CreateFileW_t = decltype(&CreateFileW);
CreateFileW_t CreateFileW_Original;

HANDLE WINAPI CreateFileW_Hook(
    LPCWSTR               lpFileName,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile)
{
    // Fast path: let everything through first. Zero cost on success.
    HANDLE h = CreateFileW_Original(
        lpFileName, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile);

    if (h != INVALID_HANDLE_VALUE)
        return h;

    if (GetLastError() != ERROR_SHARING_VIOLATION)
        return h;

    // Only redirect read-only opens. Guard against write-intent opens that
    // use specific bits instead of the GENERIC_WRITE alias to avoid silent
    // write loss on a temp copy that disappears.
    constexpr DWORD kWriteBits = GENERIC_WRITE  | FILE_WRITE_DATA |
                                 FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA |
                                 FILE_APPEND_DATA;
    if (dwDesiredAccess & kWriteBits)
        return h;

    std::wstring ext = GetExtension(lpFileName);
    if (ext.empty() || !IsTrackedExtension(ext))
        return h;

    // --- Sharing violation on a tracked Office file ---
    Wh_Log(L"Sharing violation on: %s — attempting temp copy", lpFileName);

    wchar_t tempPath[MAX_PATH];
    if (!MakeTempPath(tempPath)) {
        Wh_Log(L"Failed to build temp path for: %s", lpFileName);
        SetLastError(ERROR_SHARING_VIOLATION);
        return INVALID_HANDLE_VALUE;
    }

    // CopyFileW opens the source with permissive share flags that are
    // compatible with Word's lock (FILE_SHARE_READ).
    // bFailIfExists=FALSE overwrites the empty placeholder from GetTempFileNameW.
    if (!CopyFileW(lpFileName, tempPath, /*bFailIfExists=*/FALSE)) {
        DWORD err = GetLastError();
        Wh_Log(L"CopyFileW failed for %s (err=%u)", lpFileName, err);
        DeleteFileW(tempPath);
        SetLastError(ERROR_SHARING_VIOLATION);
        return INVALID_HANDLE_VALUE;
    }

    // FILE_FLAG_DELETE_ON_CLOSE: the OS deletes the temp file when the last
    // handle to it is closed — including forced closes at process teardown,
    // so there is no leak on crash or abnormal exit.
    // No CloseHandle hook or handle-tracking map is needed.
    HANDLE hTemp = CreateFileW_Original(
        tempPath, dwDesiredAccess,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes | FILE_FLAG_DELETE_ON_CLOSE,
        hTemplateFile);

    if (hTemp == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        Wh_Log(L"Failed to open temp copy %s (err=%u)", tempPath, err);
        DeleteFileW(tempPath);
        SetLastError(ERROR_SHARING_VIOLATION);
        return INVALID_HANDLE_VALUE;
    }

    Wh_Log(L"Redirected %s -> %s (delete-on-close)", lpFileName, tempPath);

    SetLastError(ERROR_SUCCESS);
    return hTemp;
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    // Skip session 0 processes (system services, SYSTEM-context indexers,
    // etc.). They don't upload files and shouldn't be affected.
    DWORD sessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) ||
        sessionId == 0) {
        Wh_Log(L"Skipping: session 0 process");
        return FALSE;
    }

    LoadSettings();

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!kernel32) {
        Wh_Log(L"Failed to get kernel32.dll handle");
        return FALSE;
    }

    auto* pfnCreateFileW = (void*)GetProcAddress(kernel32, "CreateFileW");
    if (!pfnCreateFileW) {
        Wh_Log(L"Failed to locate CreateFileW");
        return FALSE;
    }

    Wh_SetFunctionHook(pfnCreateFileW, (void*)CreateFileW_Hook,
                       (void**)&CreateFileW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    // No temp-file cleanup needed: FILE_FLAG_DELETE_ON_CLOSE handles it,
    // including on process crash or abnormal exit.
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
