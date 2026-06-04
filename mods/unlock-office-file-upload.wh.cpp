// ==WindhawkMod==
// @id              unlock-office-file-upload
// @name            Unlock Office Files for Upload
// @description     Transparently copies locked Office files (docx, xlsx, pptx, etc.) to a temp location when an upload or send is attempted while they're open in Word/Excel/PowerPoint. The copy is silently deleted after use.
// @version         0.1
// @author          Loerei
// @github          https://github.com/loerei
// @homepage        https://github.com/loerei/windhawk-unlock-office-file-upload
// @include         *
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
redirects the open call to the copy. The copy is automatically deleted once the
handle is closed.

**What you get:** The uploader receives the last-saved version of the file with
no prompts and no need to close Word first.

**Limitation:** The copy reflects the last *saved* state. Unsaved in-memory
edits in Word are not included — this is inherent to how Windows file locking
works.
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
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <mutex>


// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

std::vector<std::wstring> g_extensions;

void LoadSettings() {
    g_extensions.clear();

    int count = 0;
    while (true) {
        auto key = std::wstring(L"extensions[") + std::to_wstring(count) + L"]";
        PCWSTR val = Wh_GetStringSetting(key.c_str());
        if (!val || val[0] == L'\0') {
            Wh_FreeStringSetting(val);
            break;
        }
        std::wstring ext(val);
        Wh_FreeStringSetting(val);
        // Normalise to lowercase
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        g_extensions.push_back(std::move(ext));
        ++count;
    }

    // Fallback defaults if settings are empty (first run / no settings file)
    if (g_extensions.empty()) {
        for (auto* e : {L".docx", L".docm", L".doc",
                        L".xlsx", L".xlsm", L".xls",
                        L".pptx", L".pptm", L".ppt"}) {
            g_extensions.push_back(e);
        }
    }
}

// ---------------------------------------------------------------------------
// Temp handle tracking
// ---------------------------------------------------------------------------

struct TempEntry {
    std::wstring tempPath;
};

std::unordered_map<HANDLE, TempEntry> g_tempHandles;
std::mutex g_mutex;

void TrackHandle(HANDLE h, std::wstring tempPath) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_tempHandles[h] = {std::move(tempPath)};
}

// Returns temp path if handle was tracked, empty string otherwise.
std::wstring UntrackHandle(HANDLE h) {
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_tempHandles.find(h);
    if (it == g_tempHandles.end())
        return {};
    std::wstring path = std::move(it->second.tempPath);
    g_tempHandles.erase(it);
    return path;
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
    // Make sure the dot is after the last separator
    LPCWSTR sep = wcsrchr(path, L'\\');
    if (!sep) sep = wcsrchr(path, L'/');
    if (sep && dot < sep)
        return {};
    std::wstring ext(dot);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

static bool IsTrackedExtension(const std::wstring& ext) {
    for (const auto& e : g_extensions) {
        if (e == ext)
            return true;
    }
    return false;
}

// Build a unique temp path in %TEMP%, e.g. C:\Users\...\AppData\Local\Temp\~ul_12345_filename.docx
static std::wstring MakeTempPath(LPCWSTR originalPath) {
    wchar_t tempDir[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, tempDir))
        return {};

    // Extract just the filename (without path)
    LPCWSTR sep = wcsrchr(originalPath, L'\\');
    if (!sep) sep = wcsrchr(originalPath, L'/');
    LPCWSTR fname = sep ? sep + 1 : originalPath;

    wchar_t result[MAX_PATH];
    // Use tick count + thread id for uniqueness; good enough for temp files
    DWORD uid = GetTickCount() ^ (GetCurrentThreadId() << 16);
    if (swprintf_s(result, MAX_PATH, L"%s~ul_%08X_%s", tempDir, uid, fname) < 0)
        return {};

    return result;
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
    // Fast path: let everything through first.
    HANDLE h = CreateFileW_Original(
        lpFileName, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile);

    // If the call succeeded, nothing to do.
    if (h != INVALID_HANDLE_VALUE)
        return h;

    // Only intercept sharing violations on read/existing-file opens.
    if (GetLastError() != ERROR_SHARING_VIOLATION)
        return h;

    // Only intercept opens for reading (uploads/sends never need GENERIC_WRITE).
    if (dwDesiredAccess & GENERIC_WRITE)
        return h;

    // Only intercept tracked extensions.
    std::wstring ext = GetExtension(lpFileName);
    if (ext.empty() || !IsTrackedExtension(ext))
        return h;

    // --- Sharing violation on a tracked Office file ---
    Wh_Log(L"Sharing violation on tracked file: %s \u2014 attempting temp copy", lpFileName);

    std::wstring tempPath = MakeTempPath(lpFileName);
    if (tempPath.empty()) {
        Wh_Log(L"Failed to build temp path for: %s", lpFileName);
        SetLastError(ERROR_SHARING_VIOLATION);
        return INVALID_HANDLE_VALUE;
    }

    // CopyFile opens the source with FILE_SHARE_READ | FILE_SHARE_WRITE |
    // FILE_SHARE_DELETE internally, which is compatible with Word's handle.
    if (!CopyFileW(lpFileName, tempPath.c_str(), /*bFailIfExists=*/FALSE)) {
        DWORD err = GetLastError();
        Wh_Log(L"CopyFileW failed for %s (err=%u)", lpFileName, err);
        SetLastError(ERROR_SHARING_VIOLATION);
        return INVALID_HANDLE_VALUE;
    }

    // Open the temp copy; this will always succeed (no other process has it).
    HANDLE hTemp = CreateFileW_Original(
        tempPath.c_str(), dwDesiredAccess,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile);

    if (hTemp == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        Wh_Log(L"Failed to open temp copy %s (err=%u)", tempPath.c_str(), err);
        DeleteFileW(tempPath.c_str());
        SetLastError(ERROR_SHARING_VIOLATION);
        return INVALID_HANDLE_VALUE;
    }

    Wh_Log(L"Redirected %s -> %s", lpFileName, tempPath.c_str());

    // Track so we can delete the temp file when the handle is closed.
    TrackHandle(hTemp, std::move(tempPath));

    // Clear the sharing violation error; caller sees success.
    SetLastError(ERROR_SUCCESS);
    return hTemp;
}

// ---------------------------------------------------------------------------
// CloseHandle hook — delete temp copy when the uploader is done
// ---------------------------------------------------------------------------

using CloseHandle_t = decltype(&CloseHandle);
CloseHandle_t CloseHandle_Original;

BOOL WINAPI CloseHandle_Hook(HANDLE hObject) {
    std::wstring tempPath = UntrackHandle(hObject);

    BOOL result = CloseHandle_Original(hObject);

    if (!tempPath.empty()) {
        // Handle is now closed; safe to delete the temp file.
        if (DeleteFileW(tempPath.c_str())) {
            Wh_Log(L"Deleted temp copy: %s", tempPath.c_str());
        } else {
            Wh_Log(L"Failed to delete temp copy: %s (err=%u)", tempPath.c_str(), GetLastError());
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!kernel32) {
        Wh_Log(L"Failed to get kernel32.dll handle");
        return FALSE;
    }

    auto* pfnCreateFileW = (void*)GetProcAddress(kernel32, "CreateFileW");
    auto* pfnCloseHandle = (void*)GetProcAddress(kernel32, "CloseHandle");

    if (!pfnCreateFileW || !pfnCloseHandle) {
        Wh_Log(L"Failed to locate CreateFileW or CloseHandle");
        return FALSE;
    }

    Wh_SetFunctionHook(pfnCreateFileW, (void*)CreateFileW_Hook,
                       (void**)&CreateFileW_Original);
    Wh_SetFunctionHook(pfnCloseHandle, (void*)CloseHandle_Hook,
                       (void**)&CloseHandle_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    // Unhooking is handled by Windhawk. Clean up any leftover temp files.
    std::lock_guard<std::mutex> lock(g_mutex);
    for (auto& [h, entry] : g_tempHandles) {
        DeleteFileW(entry.tempPath.c_str());
    }
    g_tempHandles.clear();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
