// ==WindhawkMod==
// @id              classic-photo-viewer
// @name            Classic Windows Photo Viewer Redirect
// @description     Redirects image opening at runtime to the classic Photo Viewer (shimgvw.dll), without modifying the Registry
// @version         1.4.0
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
This mod hooks `ShellExecuteExW` and, when it detects a call with the "open" verb
targeting a file with one of the configured image extensions, redirects the call
to launch the classic Photo Viewer (`shimgvw.dll` via `rundll32.exe`) instead of
whatever application would normally handle it.
### Screenshot
![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/photo.png)
### Key Features
- No Registry modifications: The mod does not alter file associations or create any keys in HKLM or HKCU.
- Purely runtime effect: The redirection occurs exclusively in memory. Disabling the mod instantly restores the default Windows behavior.
- Supported formats: Covers major raster formats including JPG, JPEG, JFIF, PNG, BMP, DIB, GIF, TIF, TIFF, ICO, WDP, and JXR.

### Known Limitations
- The redirection only applies when the file is opened via `ShellExecuteExW`, which is
  how the Explorer context menu's "Open" command works. This is the main path this
  mod is able to intercept.
- Double-click and Enter often go through app activation for the UWP Photos app
  instead of `ShellExecuteExW`, which this mod cannot intercept. In practice, the
  redirect mainly fires for the right-click → Open path.
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
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
std::vector<std::wstring> g_extensions;
std::wstring              g_shimgvwPath;
std::wstring              g_rundll32Path;
bool                      g_photoViewerAvailable = false;
thread_local bool         g_inRedirect           = false;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
void LoadSettings() {
    g_extensions.clear();
    for (int i = 0;; i++) {
        PCWSTR ext = Wh_GetStringSetting(L"extensions[%d]", i);
        if (!*ext) {
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
// ShellExecuteEx Hook
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

    g_photoViewerAvailable = CheckPhotoViewerAvailable();
    if (!g_photoViewerAvailable) {
        Wh_Log(L"Mod will remain inactive: required system files not found.");
        return TRUE;
    }

    // Hook ShellExecuteExW for direct redirection
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32) {
        FARPROC pShellExecuteExW = GetProcAddress(hShell32, "ShellExecuteExW");
        if (pShellExecuteExW) {
            if (WindhawkUtils::SetFunctionHook(
                    reinterpret_cast<ShellExecuteExW_t>(pShellExecuteExW),
                    ShellExecuteExW_Hook,
                    &ShellExecuteExW_Original)) {
                Wh_Log(L"Successfully hooked ShellExecuteExW");
            } else {
                Wh_Log(L"Failed to install hook on ShellExecuteExW");
                g_photoViewerAvailable = false;
            }
        }
    }

    Wh_Log(L"Mod active: %zu extensions redirected", g_extensions.size());
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Classic Photo Viewer Redirect - Deactivated");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
