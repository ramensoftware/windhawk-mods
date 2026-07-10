// ==WindhawkMod==
// @id              classic-photo-viewer
// @name            Classic Windows Photo Viewer Redirect
// @description     Redirects image opening at runtime to the classic Photo Viewer (shimgvw.dll), without modifying the Registry
// @version         1.3.1
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Windows Photo Viewer Redirect

This mod restores the classic Windows Photo Viewer from Windows 7 without altering system configurations.

### How it works
When you open an image from File Explorer (via double-click, Enter, or the context menu), this mod intercepts the request and directly launches the classic Photo Viewer instead of the default modern Photos application.

### Key Features
- No Registry modifications: The mod does not alter file associations or create any keys in HKLM or HKCU.
- Purely runtime effect: The redirection occurs exclusively in memory. Disabling the mod instantly restores the default Windows behavior.
- Virtual registry hooking: Intercepts registry queries to make the system believe Photo Viewer is properly registered.
- Supported formats: Covers major raster formats including JPG, JPEG, JFIF, PNG, BMP, DIB, GIF, TIF, TIFF, ICO, WDP, and JXR.

### Known Limitations
- The redirection only applies to file openings executed through Windows File Explorer (via ShellExecuteEx).
- Double-click and Enter key may use app activation for UWP Photos app instead of ShellExecuteEx, which this mod cannot intercept.
- Modern formats not natively supported by the legacy viewer (such as HEIC or WebP) are ignored by the hook and will continue to open with the default modern application.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extensions:
  - .jpg
  - .jpeg
  - .jfif
  - .png
  - .bmp
  - .dib
  - .gif
  - .tif
  - .tiff
  - .ico
  - .wdp
  - .jxr
  $name: Extensions to redirect
  $description: Image formats for which to force opening with the classic Photo Viewer
- LockTimeoutMs: 0
  $name: File lock timeout (ms)
  $description: Time to wait for file lock to be released (0 to disable, 100-5000 ms). If timeout expires, opens with default viewer.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <unordered_map>

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
std::vector<std::wstring> g_extensions;
std::wstring              g_shimgvwPath;
std::wstring              g_rundll32Path;
bool                      g_photoViewerAvailable = false;
thread_local bool         g_inRedirect           = false;
int                       g_lockTimeoutMs        = 0;

// Virtual registry values to intercept
std::unordered_map<std::wstring, std::wstring> g_virtualRegistry;

// Original API functions
typedef LONG (WINAPI *REGQUERYVALUEEXW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
REGQUERYVALUEEXW pOriginalRegQueryValueExW = nullptr;

// ---------------------------------------------------------------------------
// Virtual Registry Setup
// ---------------------------------------------------------------------------
void SetupVirtualRegistry() {
    // Simulate the registry keys that would normally be created by the .reg file
    // These make the system believe Photo Viewer is properly registered
    
    // File associations for Photo Viewer
    g_virtualRegistry[L".jpg"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".jpeg"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".jfif"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".png"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".bmp"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".dib"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".gif"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".tif"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".tiff"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".ico"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".wdp"] = L"PhotoViewer.FileAssoc.Tiff";
    g_virtualRegistry[L".jxr"] = L"PhotoViewer.FileAssoc.Tiff";
    
    // Additional ProgID registrations that Windows might look for
    g_virtualRegistry[L"PhotoViewer.FileAssoc.Tiff"] = L"Windows Photo Viewer";
    
    Wh_Log(L"Virtual registry configured with %zu entries", g_virtualRegistry.size());
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
void LoadSettings() {
    g_extensions.clear();
    for (int i = 0;; i++) {
        PCWSTR ext = Wh_GetStringSetting(L"extensions[%d]", i);
        if (!ext || !*ext) {
            Wh_FreeStringSetting(ext);
            break;
        }
        std::wstring e = ext;
        Wh_FreeStringSetting(ext);
        for (auto& c : e) c = towlower(c);
        g_extensions.push_back(e);
    }

    if (g_extensions.empty()) {
        g_extensions = {L".jpg", L".jpeg", L".jfif", L".png", L".bmp",
                        L".dib", L".gif",  L".tif",  L".tiff", L".ico",
                        L".wdp", L".jxr"};
    }

    g_lockTimeoutMs = Wh_GetIntSetting(L"LockTimeoutMs");
    if (g_lockTimeoutMs < 0) g_lockTimeoutMs = 0;
    if (g_lockTimeoutMs > 5000) g_lockTimeoutMs = 5000;
}

// ---------------------------------------------------------------------------
// Availability check
// ---------------------------------------------------------------------------
bool CheckPhotoViewerAvailable() {
    WCHAR sysDir[MAX_PATH];
    if (!GetSystemDirectoryW(sysDir, MAX_PATH)) {
        Wh_Log(L"Cannot determine System32 directory");
        return false;
    }

    g_shimgvwPath = std::wstring(sysDir) + L"\\shimgvw.dll";
    DWORD attrs = GetFileAttributesW(g_shimgvwPath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"shimgvw.dll not found in %s", sysDir);
        return false;
    }

    g_rundll32Path = std::wstring(sysDir) + L"\\rundll32.exe";
    attrs = GetFileAttributesW(g_rundll32Path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"rundll32.exe not found in %s", sysDir);
        return false;
    }

    Wh_Log(L"Photo Viewer is available (%s, %s)", g_shimgvwPath.c_str(),
           g_rundll32Path.c_str());
    return true;
}

// ---------------------------------------------------------------------------
// Extension helper
// ---------------------------------------------------------------------------
bool IsTargetImageExtension(const std::wstring& path) {
    PCWSTR ext = PathFindExtensionW(path.c_str());
    if (!ext || !*ext) return false;

    std::wstring extLower = ext;
    for (auto& c : extLower) c = towlower(c);

    return std::find(g_extensions.begin(), g_extensions.end(), extLower)
           != g_extensions.end();
}

// ---------------------------------------------------------------------------
// File lock detection
// ---------------------------------------------------------------------------
enum class FileAccessResult {
    Accessible,
    Locked,
    Error
};

FileAccessResult CheckFileAccessible(const std::wstring& filePath) {
    HANDLE hFile = CreateFileW(
        filePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        return FileAccessResult::Accessible;
    }
    
    DWORD error = GetLastError();
    if (error == ERROR_SHARING_VIOLATION) {
        Wh_Log(L"File is locked by another process: %s", filePath.c_str());
        return FileAccessResult::Locked;
    }
    
    Wh_Log(L"Cannot access file %s (Error: %d)", filePath.c_str(), error);
    return FileAccessResult::Error;
}

bool WaitForFileUnlock(const std::wstring& filePath, int timeoutMs) {
    if (timeoutMs <= 0) return false;
    
    int elapsed = 0;
    const int checkInterval = 100;
    
    while (elapsed < timeoutMs) {
        if (CheckFileAccessible(filePath) == FileAccessResult::Accessible) {
            Wh_Log(L"File lock released after %d ms: %s", elapsed, filePath.c_str());
            return true;
        }
        Sleep(checkInterval);
        elapsed += checkInterval;
    }
    
    Wh_Log(L"Timeout waiting for file unlock (%d ms): %s", timeoutMs, filePath.c_str());
    return false;
}

// ---------------------------------------------------------------------------
// Registry API Hooks (Virtual Registry)
// ---------------------------------------------------------------------------
LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, 
                                  LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
    // Check if we need to intercept this registry query
    if (lpValueName && g_photoViewerAvailable) {
        std::wstring valueName(lpValueName);
        
        // Convert to lowercase for case-insensitive comparison
        for (auto& c : valueName) c = towlower(c);
        
        auto it = g_virtualRegistry.find(valueName);
        if (it != g_virtualRegistry.end()) {
            Wh_Log(L"Intercepting registry query for: %s", lpValueName);
            
            std::wstring virtualValue = it->second;
            DWORD dataSize = (virtualValue.length() + 1) * sizeof(WCHAR);
            
            if (lpType)
                *lpType = REG_SZ;
            
            if (lpData && lpcbData && *lpcbData >= dataSize) {
                memcpy(lpData, virtualValue.c_str(), dataSize);
                *lpcbData = dataSize;
            } else if (lpcbData) {
                *lpcbData = dataSize;
            }
            
            return ERROR_SUCCESS;
        }
        
        // Also check for ProgID lookups (without the dot)
        if (valueName.find(L"photoviewer.fileassoc") != std::wstring::npos) {
            Wh_Log(L"Intercepting ProgID registry query for: %s", lpValueName);
            
            if (lpType)
                *lpType = REG_SZ;
            
            std::wstring virtualValue = L"Windows Photo Viewer";
            DWORD dataSize = (virtualValue.length() + 1) * sizeof(WCHAR);
            
            if (lpData && lpcbData && *lpcbData >= dataSize) {
                memcpy(lpData, virtualValue.c_str(), dataSize);
                *lpcbData = dataSize;
            } else if (lpcbData) {
                *lpcbData = dataSize;
            }
            
            return ERROR_SUCCESS;
        }
    }
    
    // Call the original function for all other queries
    return pOriginalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

// ---------------------------------------------------------------------------
// ShellExecuteEx Hook (with fallback for file locks)
// ---------------------------------------------------------------------------
using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_Original;

BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* pExecInfo) {
    if (g_inRedirect || !g_photoViewerAvailable || !pExecInfo || !pExecInfo->lpFile)
        return ShellExecuteExW_Original(pExecInfo);

    bool isOpenVerb = !pExecInfo->lpVerb || !*pExecInfo->lpVerb ||
                      _wcsicmp(pExecInfo->lpVerb, L"open") == 0;
    if (!isOpenVerb)
        return ShellExecuteExW_Original(pExecInfo);

    std::wstring filePath = pExecInfo->lpFile;
    if (!IsTargetImageExtension(filePath))
        return ShellExecuteExW_Original(pExecInfo);

    DWORD attrs = GetFileAttributesW(filePath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY))
        return ShellExecuteExW_Original(pExecInfo);

    // Check if file is locked by another program
    FileAccessResult access = CheckFileAccessible(filePath);
    if (access == FileAccessResult::Locked) {
        Wh_Log(L"File locked, attempting to wait: %s", filePath.c_str());
        
        if (g_lockTimeoutMs > 0) {
            if (!WaitForFileUnlock(filePath, g_lockTimeoutMs)) {
                Wh_Log(L"File remains locked after timeout, falling back to default viewer: %s", 
                       filePath.c_str());
                return ShellExecuteExW_Original(pExecInfo);
            }
        } else {
            Wh_Log(L"File locked and timeout disabled, falling back to default viewer: %s", 
                   filePath.c_str());
            return ShellExecuteExW_Original(pExecInfo);
        }
    } else if (access == FileAccessResult::Error) {
        Wh_Log(L"File access error, falling back to default viewer: %s", filePath.c_str());
        return ShellExecuteExW_Original(pExecInfo);
    }

    Wh_Log(L"Redirecting to Photo Viewer: %s", filePath.c_str());

    std::wstring params = L"\"" + g_shimgvwPath +
                          L"\",ImageView_Fullscreen \"" + filePath + L"\"";

    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.fMask             = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd              = pExecInfo->hwnd;
    sei.lpVerb            = L"open";
    sei.lpFile            = g_rundll32Path.c_str();
    sei.lpParameters      = params.c_str();
    sei.lpDirectory       = NULL;  // Use system directory, not the image's directory
    sei.nShow             = pExecInfo->nShow ? pExecInfo->nShow : SW_SHOWNORMAL;

    g_inRedirect = true;
    BOOL result  = ShellExecuteExW_Original(&sei);
    g_inRedirect = false;

    if (result) {
        if (pExecInfo->fMask & SEE_MASK_NOCLOSEPROCESS) {
            pExecInfo->hProcess = sei.hProcess;
        } else {
            if (sei.hProcess)
                CloseHandle(sei.hProcess);
            pExecInfo->hProcess = nullptr;
        }
        pExecInfo->hInstApp = reinterpret_cast<HINSTANCE>(33);
        return TRUE;
    }

    Wh_Log(L"Redirect failed, using default handler");
    return ShellExecuteExW_Original(pExecInfo);
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Classic Photo Viewer Redirect - Initializing");

    LoadSettings();
    SetupVirtualRegistry();

    g_photoViewerAvailable = CheckPhotoViewerAvailable();
    if (!g_photoViewerAvailable) {
        Wh_Log(L"Mod will remain inactive: required system files not found.");
        return TRUE;
    }

    // Hook registry APIs for virtual registry
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        FARPROC pRegQueryValueExW = GetProcAddress(hKernelBase, "RegQueryValueExW");
        if (pRegQueryValueExW) {
            if (Wh_SetFunctionHook(
                    reinterpret_cast<void*>(pRegQueryValueExW),
                    reinterpret_cast<void*>(RegQueryValueExWHook),
                    reinterpret_cast<void**>(&pOriginalRegQueryValueExW))) {
                Wh_Log(L"Successfully hooked RegQueryValueExW for virtual registry");
            } else {
                Wh_Log(L"Failed to hook RegQueryValueExW");
            }
        }
    }

    // Hook ShellExecuteExW for direct redirection
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32) {
        FARPROC pShellExecuteExW = GetProcAddress(hShell32, "ShellExecuteExW");
        if (pShellExecuteExW) {
            if (Wh_SetFunctionHook(
                    reinterpret_cast<void*>(pShellExecuteExW),
                    reinterpret_cast<void*>(ShellExecuteExW_Hook),
                    reinterpret_cast<void**>(&ShellExecuteExW_Original))) {
                Wh_Log(L"Successfully hooked ShellExecuteExW");
            } else {
                Wh_Log(L"Failed to install hook on ShellExecuteExW");
                g_photoViewerAvailable = false;
            }
        }
    }

    Wh_Log(L"Mod active: %zu extensions redirected, lock timeout: %d ms", 
           g_extensions.size(), g_lockTimeoutMs);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Classic Photo Viewer Redirect - Deactivated");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
