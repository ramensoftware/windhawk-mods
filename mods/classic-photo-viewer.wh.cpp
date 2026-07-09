// ==WindhawkMod==
// @id              classic-photo-viewer
// @name            Classic Windows Photo Viewer Redirect
// @description     Redirects image opening at runtime to the classic Photo Viewer (shimgvw.dll), without modifying the Registry
// @version         1.1.4
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @compilerOptions -lshell32 -lshlwapi -lwintrust -lcrypt32
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
- Automatic security verification: At startup, the mod verifies the presence and integrity of the Microsoft digital signature on the required system files (shimgvw.dll and rundll32.exe). If any file is missing or the signature is invalid, the mod automatically deactivates for security.
- Supported formats: Covers major raster formats including JPG, JPEG, JFIF, PNG, BMP, DIB, GIF, TIF, TIFF, ICO, WDP, and JXR.

### Known Limitations
- The redirection only applies to file openings executed through Windows File Explorer.
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
  $name: Extensions to redirect
  $description: Image formats for which to force opening with the classic Photo Viewer
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#ifndef WINVER
#define WINVER 0x0601
#endif
#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x06010000
#endif

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <wintrust.h>
#include <softpub.h>
#include <wincrypt.h>
#include <mscat.h>
#include <string>
#include <vector>
#include <algorithm>

// ---------------------------------------------------------------------------
// Runtime function pointers
// ---------------------------------------------------------------------------
typedef BOOL (WINAPI *PFN_CryptCATAdminAcquireContext2)(
    HCATADMIN*, const GUID*, PCWSTR,
    PCCERT_STRONG_SIGN_PARA, DWORD);

typedef BOOL (WINAPI *PFN_CryptCATAdminCalcHashFromFileHandle2)(
    HCATADMIN, HANDLE, DWORD*, BYTE*, DWORD);

static PFN_CryptCATAdminAcquireContext2         pfnAcquireContext2   = nullptr;
static PFN_CryptCATAdminCalcHashFromFileHandle2 pfnCalcHashFromHandle2 = nullptr;

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
std::vector<std::wstring> g_extensions;
std::wstring              g_shimgvwPath;
bool                      g_photoViewerAvailable = false;
thread_local bool         g_inRedirect           = false;

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
                        L".dib", L".gif",  L".tif",  L".tiff", L".ico"};
    }
}

// ---------------------------------------------------------------------------
// Publisher check
// ---------------------------------------------------------------------------
bool CheckPublisherIsMicrosoft(HANDLE hWVTStateData) {
    if (!hWVTStateData) return false;

    CRYPT_PROVIDER_DATA* provData = WTHelperProvDataFromStateData(hWVTStateData);
    if (!provData || provData->csSigners == 0) return false;

    CRYPT_PROVIDER_SGNR* signer =
        WTHelperGetProvSignerFromChain(provData, 0, FALSE, 0);
    if (!signer || !signer->pChainContext ||
        !signer->pChainContext->rgpChain ||
        !signer->pChainContext->rgpChain[0] ||
        !signer->pChainContext->rgpChain[0]->rgpElement ||
        !signer->pChainContext->rgpChain[0]->rgpElement[0]) {
        return false;
    }

    PCCERT_CONTEXT certContext =
        signer->pChainContext->rgpChain[0]->rgpElement[0]->pCertContext;
    if (!certContext) return false;

    DWORD nameLen = CertGetNameStringW(
        certContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr, nullptr, 0);
    if (nameLen == 0) return false;

    std::vector<WCHAR> subject(nameLen + 1);
    if (!CertGetNameStringW(certContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr,
                            subject.data(), nameLen)) {
        return false;
    }

    std::wstring publisher(subject.data());
    Wh_Log(L"Signer: %s", publisher.c_str());

    return publisher.find(L"Microsoft") != std::wstring::npos;
}

// ---------------------------------------------------------------------------
// Catalog-based signature verification
// Compatible with both legacy APIs and SHA-256 APIs.
// ---------------------------------------------------------------------------
bool VerifyCatalogSignature(const std::wstring& filePath) {
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Cannot open file for catalog verification: %s", filePath.c_str());
        return false;
    }

    HCATADMIN hCatAdmin  = nullptr;
    bool usedModernApi   = false;

    // Try SHA-256 context first (loaded dynamically)
    if (pfnAcquireContext2 &&
        pfnAcquireContext2(&hCatAdmin, nullptr, L"SHA256", nullptr, 0)) {
        usedModernApi = true;
    } else {
        // Fallback: legacy SHA-1 context
        if (!CryptCATAdminAcquireContext(&hCatAdmin, nullptr, 0)) {
            Wh_Log(L"CryptCATAdminAcquireContext failed: %lu", GetLastError());
            CloseHandle(hFile);
            return false;
        }
    }

    // Compute file hash -------------------------------------------------------
    DWORD hashSize = 0;
    std::vector<BYTE> hash;

    if (usedModernApi && pfnCalcHashFromHandle2) {
        pfnCalcHashFromHandle2(hCatAdmin, hFile, &hashSize, nullptr, 0);
        if (hashSize == 0) {
            Wh_Log(L"Failed to get hash size (Win8 API) for %s", filePath.c_str());
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            CloseHandle(hFile);
            return false;
        }
        hash.resize(hashSize);
        if (!pfnCalcHashFromHandle2(hCatAdmin, hFile, &hashSize, hash.data(), 0)) {
            Wh_Log(L"CryptCATAdminCalcHashFromFileHandle2 failed: %lu", GetLastError());
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            CloseHandle(hFile);
            return false;
        }
    } else {
        CryptCATAdminCalcHashFromFileHandle(hFile, &hashSize, nullptr, 0);
        if (hashSize == 0) {
            Wh_Log(L"Failed to get hash size (legacy API) for %s", filePath.c_str());
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            CloseHandle(hFile);
            return false;
        }
        hash.resize(hashSize);
        if (!CryptCATAdminCalcHashFromFileHandle(hFile, &hashSize, hash.data(), 0)) {
            Wh_Log(L"CryptCATAdminCalcHashFromFileHandle failed: %lu", GetLastError());
            CryptCATAdminReleaseContext(hCatAdmin, 0);
            CloseHandle(hFile);
            return false;
        }
    }

    CloseHandle(hFile);

    // Build hex string of hash ------------------------------------------------
    std::wstring hashStr;
    hashStr.reserve(hashSize * 2);
    for (DWORD i = 0; i < hashSize; i++) {
        WCHAR hex[3];
        swprintf_s(hex, L"%02X", hash[i]);
        hashStr += hex;
    }

    // Enumerate catalogs containing this hash ---------------------------------
    HCATINFO hCatInfo = CryptCATAdminEnumCatalogFromHash(
        hCatAdmin, hash.data(), hashSize, 0, nullptr);

    if (!hCatInfo) {
        Wh_Log(L"No system catalog found containing hash of %s", filePath.c_str());
        CryptCATAdminReleaseContext(hCatAdmin, 0);
        return false;
    }

    CATALOG_INFO catInfo = {};
    catInfo.cbStruct = sizeof(catInfo);
    if (!CryptCATCatalogInfoFromContext(hCatInfo, &catInfo, 0)) {
        Wh_Log(L"CryptCATCatalogInfoFromContext failed: %lu", GetLastError());
        CryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
        CryptCATAdminReleaseContext(hCatAdmin, 0);
        return false;
    }

    Wh_Log(L"File is member of catalog: %s", catInfo.wszCatalogFile);

    // Build WINTRUST_CATALOG_INFO ---------------------------------------------
    WINTRUST_CATALOG_INFO wtCatInfo = {};
    wtCatInfo.cbStruct              = sizeof(wtCatInfo);
    wtCatInfo.pcwszCatalogFilePath  = catInfo.wszCatalogFile;
    wtCatInfo.pcwszMemberTag        = hashStr.c_str();
    wtCatInfo.pcwszMemberFilePath   = filePath.c_str();
    wtCatInfo.hMemberFile           = nullptr;
    wtCatInfo.pbCalculatedFileHash  = hash.data();
    wtCatInfo.cbCalculatedFileHash  = hashSize;
#if _WIN32_WINNT >= 0x0602
    wtCatInfo.hCatAdmin             = hCatAdmin;
#endif

    WINTRUST_DATA wtData       = {};
    wtData.cbStruct            = sizeof(wtData);
    wtData.dwUnionChoice       = WTD_CHOICE_CATALOG;
    wtData.pCatalog            = &wtCatInfo;
    wtData.dwUIChoice          = WTD_UI_NONE;
    wtData.fdwRevocationChecks = WTD_REVOKE_NONE;
    wtData.dwStateAction       = WTD_STATEACTION_VERIFY;
    wtData.dwProvFlags         = WTD_CACHE_ONLY_URL_RETRIEVAL;

    GUID policyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG status     = WinVerifyTrust(nullptr, &policyGUID, &wtData);

    bool valid = (status == ERROR_SUCCESS);
    if (valid) {
        valid = CheckPublisherIsMicrosoft(wtData.hWVTStateData);
        if (!valid)
            Wh_Log(L"Catalog is valid but signer is not Microsoft");
        else
            Wh_Log(L"Catalog-based Microsoft signature verified for %s", filePath.c_str());
    } else {
        Wh_Log(L"Catalog signature verification failed (0x%08lX)", status);
    }

    wtData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &policyGUID, &wtData);

    CryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
    CryptCATAdminReleaseContext(hCatAdmin, 0);

    return valid;
}

// ---------------------------------------------------------------------------
// Embedded + catalog verification
// ---------------------------------------------------------------------------
bool VerifyMicrosoftSignature(const std::wstring& filePath) {
    WINTRUST_FILE_INFO fileInfo = {};
    fileInfo.cbStruct           = sizeof(WINTRUST_FILE_INFO);
    fileInfo.pcwszFilePath      = filePath.c_str();

    WINTRUST_DATA wtData       = {};
    wtData.cbStruct            = sizeof(WINTRUST_DATA);
    wtData.dwUIChoice          = WTD_UI_NONE;
    wtData.fdwRevocationChecks = WTD_REVOKE_NONE;
    wtData.dwUnionChoice       = WTD_CHOICE_FILE;
    wtData.pFile               = &fileInfo;
    wtData.dwStateAction       = WTD_STATEACTION_VERIFY;
    wtData.dwProvFlags         = WTD_CACHE_ONLY_URL_RETRIEVAL;

    GUID policyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG status     = WinVerifyTrust(nullptr, &policyGUID, &wtData);

    if (status == ERROR_SUCCESS) {
        bool microsoft = CheckPublisherIsMicrosoft(wtData.hWVTStateData);

        wtData.dwStateAction = WTD_STATEACTION_CLOSE;
        WinVerifyTrust(nullptr, &policyGUID, &wtData);

        if (microsoft) {
            Wh_Log(L"Embedded Microsoft signature verified: %s", filePath.c_str());
            return true;
        }
        Wh_Log(L"Embedded signature valid but signer is not Microsoft");
        return false;
    }

    wtData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &policyGUID, &wtData);

    if (status == (LONG)TRUST_E_NOSIGNATURE) {
        Wh_Log(L"No embedded signature for %s, trying catalog-based verification...",
               filePath.c_str());
        return VerifyCatalogSignature(filePath);
    }

    Wh_Log(L"WinVerifyTrust failed for %s (0x%08lX)", filePath.c_str(), status);
    return false;
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

    Wh_Log(L"Found shimgvw.dll, verifying digital signature...");
    if (!VerifyMicrosoftSignature(g_shimgvwPath)) {
        Wh_Log(L"SECURITY ERROR: shimgvw.dll signature is invalid or missing. "
               L"The file may have been tampered with. Mod will NOT activate.");
        return false;
    }

    std::wstring rundll32Path = std::wstring(sysDir) + L"\\rundll32.exe";
    attrs = GetFileAttributesW(rundll32Path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"rundll32.exe not found in %s", sysDir);
        return false;
    }

    Wh_Log(L"Found rundll32.exe, verifying digital signature...");
    if (!VerifyMicrosoftSignature(rundll32Path)) {
        Wh_Log(L"SECURITY ERROR: rundll32.exe signature is invalid or missing. "
               L"Mod will NOT activate.");
        return false;
    }

    Wh_Log(L"All security checks passed. Photo Viewer is available and verified.");
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
// Hook
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
    sei.lpFile            = L"rundll32.exe";
    sei.lpParameters      = params.c_str();
    sei.lpDirectory       = pExecInfo->lpDirectory;
    sei.nShow             = pExecInfo->nShow ? pExecInfo->nShow : SW_SHOWNORMAL;

    g_inRedirect = true;
    BOOL result  = ShellExecuteExW_Original(&sei);
    g_inRedirect = false;

    if (result) {
        pExecInfo->hProcess = sei.hProcess;
        pExecInfo->hInstApp = reinterpret_cast<HINSTANCE>(33);
        if (sei.hProcess)
            CloseHandle(sei.hProcess);
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

    // Dynamically resolve Win8+ catalog APIs (safe on Win7 - returns nullptr)
    HMODULE hWintrust = GetModuleHandleW(L"wintrust.dll");
    if (!hWintrust) hWintrust = LoadLibraryW(L"wintrust.dll");
    if (hWintrust) {
        pfnAcquireContext2 = reinterpret_cast<PFN_CryptCATAdminAcquireContext2>(
            GetProcAddress(hWintrust, "CryptCATAdminAcquireContext2"));
        pfnCalcHashFromHandle2 =
            reinterpret_cast<PFN_CryptCATAdminCalcHashFromFileHandle2>(
                GetProcAddress(hWintrust, "CryptCATAdminCalcHashFromFileHandle2"));
    }

    if (pfnAcquireContext2 && pfnCalcHashFromHandle2)
        Wh_Log(L"Win8+ catalog APIs available (SHA-256 support enabled)");
    else
        Wh_Log(L"Win8+ catalog APIs not found, using legacy SHA-1 catalog verification");

    LoadSettings();

    g_photoViewerAvailable = CheckPhotoViewerAvailable();
    if (!g_photoViewerAvailable) {
        Wh_Log(L"Mod will remain inactive: security checks failed.");
        return TRUE;
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"Cannot get handle to shell32.dll");
        g_photoViewerAvailable = false;
        return TRUE;
    }

    FARPROC pShellExecuteExW = GetProcAddress(hShell32, "ShellExecuteExW");
    if (!pShellExecuteExW) {
        Wh_Log(L"Cannot resolve ShellExecuteExW");
        g_photoViewerAvailable = false;
        return TRUE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(pShellExecuteExW),
            reinterpret_cast<void*>(ShellExecuteExW_Hook),
            reinterpret_cast<void**>(&ShellExecuteExW_Original))) {
        Wh_Log(L"Failed to install hook on ShellExecuteExW");
        g_photoViewerAvailable = false;
        return TRUE;
    }

    Wh_Log(L"Mod active: %zu extensions redirected", g_extensions.size());
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Classic Photo Viewer Redirect - Deactivated");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
