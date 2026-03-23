// ==WindhawkMod==
// @id              context-menu-preloader
// @name            Context Menu Preloader
// @description     Instantly loads context menus. Includes RAM usage monitoring and proper thread cleanup.
// @version         1.0
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         explorer.exe
// @compilerOptions -lgdi32 -lole32 -luuid -lshlwapi -lpsapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Context Menu Preloader

Preloads your desired context menu handlers (txt; mp4; svg) and pins them to your RAM for nigh-instant loading of context menus.

Certain handlers are blocked by default because they interfere with Bluetooth.

Does not affect context menus inside applications.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- StartupDelay: 5
  $name: Startup Delay (Seconds)
  $description: Wait time before locking handlers.
- CustomExtensions: "pdf; mp4; mkv; docx; obj; html"
  $name: Custom Extensions
  $description: Extensions to preload and pin. Separate with each ';'.
- EnableDebug: false
  $name: Enable Debug Logging and RAM usage report.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <shlwapi.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <sstream>
#include <atomic>
#include <set>
#include <algorithm>
#include <mutex>

// -------------------------------------------------------------------------
// Globals
// -------------------------------------------------------------------------
static std::atomic<int> g_delay{5};
static std::atomic<bool> g_debug{false};
static std::atomic<bool> g_quitting{false}; 
static HANDLE g_hThread = NULL;             

static std::vector<std::wstring> g_customExts;
static std::set<std::wstring> g_pinnedDlls; 
static std::mutex g_pinMutex;

// -------------------------------------------------------------------------
// Logging
// -------------------------------------------------------------------------
void Log(LPCWSTR fmt, ...) {
    if (!g_debug.load(std::memory_order_relaxed)) return;
    va_list args;
    va_start(args, fmt);
    WCHAR buf[1024];
    vswprintf_s(buf, fmt, args);
    va_end(args);
    Wh_Log(L"[Accelerator] %s", buf);
}

// -------------------------------------------------------------------------
// Utils
// -------------------------------------------------------------------------
void ParseExtensions(const std::wstring& input) {
    g_customExts.clear();
    std::wstringstream ss(input);
    std::wstring item;
    while (std::getline(ss, item, L';')) {
        size_t first = item.find_first_not_of(L" ");
        if (first == std::string::npos) continue;
        size_t last = item.find_last_not_of(L" ");
        std::wstring cleanExt = item.substr(first, (last - first + 1));
        if (!cleanExt.empty() && cleanExt[0] == L'.') cleanExt = cleanExt.substr(1);
        if (!cleanExt.empty()) g_customExts.push_back(cleanExt);
    }
}

std::wstring ToLower(const std::wstring& input) {
    std::wstring out = input;
    std::transform(out.begin(), out.end(), out.begin(), towlower);
    return out;
}

// -------------------------------------------------------------------------
// RAM Monitoring
// -------------------------------------------------------------------------
SIZE_T GetRamUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

// -------------------------------------------------------------------------
// Blocklist & Pinner
// -------------------------------------------------------------------------
bool IsBlacklisted(const std::wstring& name, const std::wstring& dllPath) {
    std::wstring nameLower = ToLower(name);
    std::wstring pathLower = ToLower(dllPath);

    // 1. Nearby Sharing / Bluetooth / Network
    if (pathLower.find(L"ntshrui.dll") != std::wstring::npos) return true; 
    if (pathLower.find(L"filesyncshell64.dll") != std::wstring::npos) return true;
    if (pathLower.find(L"workfolders") != std::wstring::npos) return true;
    if (pathLower.find(L"sharing") != std::wstring::npos) return true;
    
    // 2. Managed / .NET
    if (pathLower.find(L"mscoree.dll") != std::wstring::npos) return true;

    // 3. System UI / Brokers
    if (pathLower.find(L"appresolver.dll") != std::wstring::npos) return true; 
    if (pathLower.find(L"acppage.dll") != std::wstring::npos) return true;
    if (pathLower.find(L"twinui") != std::wstring::npos) return true;
    if (pathLower.find(L"windows.share") != std::wstring::npos) return true;
    if (pathLower.find(L"datatransfer") != std::wstring::npos) return true;

    // 4. Casting / Bluetooth Legacy
    if (pathLower.find(L"playtomenu.dll") != std::wstring::npos) return true;
    if (nameLower.find(L"play to") != std::wstring::npos) return true;
    if (pathLower.find(L"bluetooth") != std::wstring::npos) return true;
    if (pathLower.find(L"fsquirt.dll") != std::wstring::npos) return true;

    // 5. Native
    if (pathLower.find(L"windows defender") != std::wstring::npos) return true;
    if (pathLower.find(L"shellext.dll") != std::wstring::npos) return true;
    if (pathLower.find(L"zipfldr.dll") != std::wstring::npos) return true;
    
    // 6. Printers
    if (nameLower.find(L"print") != std::wstring::npos) return true;

    return false;
}

void PinDLL(const std::wstring& dllPath, const std::wstring& debugName) {
    if (g_quitting.load()) return; 
    if (dllPath.empty()) return;
    
    std::wstring pathLower = ToLower(dllPath);

    // 1. Lock only to read the set
    {
        std::lock_guard<std::mutex> lock(g_pinMutex);
        if (g_pinnedDlls.count(pathLower)) return; 
    }

    if (IsBlacklisted(debugName, pathLower)) {
        Log(L"BLOCKED: %s", debugName.c_str());
        return;
    }

    if (pathLower.find(L"\\") != std::wstring::npos && GetFileAttributesW(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Log(L"Skipped (Not Found): %s", dllPath.c_str());
        return;
    }

    // Call LoadLibrary OUTSIDE the lock to prevent OS Loader deadlocks
    HMODULE hMod = LoadLibraryW(dllPath.c_str());
    if (hMod) {
        HMODULE hPinned = NULL;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
                              (LPCWSTR)hMod, &hPinned)) {
            // 2. Lock only to write to the set
            std::lock_guard<std::mutex> lock(g_pinMutex);
            g_pinnedDlls.insert(pathLower); 
            Log(L"Pinned: %s", dllPath.c_str());
        } else {
            FreeLibrary(hMod);
            Log(L"Failed to pin (Freed safely): %s", dllPath.c_str());
        }
    } else {
        Log(L"Failed to load: %s", dllPath.c_str());
    }
}

// -------------------------------------------------------------------------
// Registry Walker Logic
// -------------------------------------------------------------------------
HKEY OpenKey(HKEY hRoot, LPCWSTR path) {
    HKEY hKey = NULL;
    if (RegOpenKeyExW(hRoot, path, 0, KEY_READ, &hKey) == ERROR_SUCCESS) return hKey;
    return NULL;
}

std::wstring ReadDefaultValue(HKEY hKey, LPCWSTR subKey = NULL) {
    WCHAR buf[MAX_PATH];
    DWORD size = sizeof(buf);
    HKEY hTarget = hKey;
    
    if (subKey) {
        hTarget = OpenKey(hKey, subKey);
        if (!hTarget) return L"";
    }

    LONG res = RegQueryValueExW(hTarget, NULL, NULL, NULL, (LPBYTE)buf, &size);
    if (subKey) RegCloseKey(hTarget);
    
    if (res == ERROR_SUCCESS && size > 0) {
        // Calculate max characters the buffer can hold
        DWORD maxChars = sizeof(buf) / sizeof(WCHAR);
        DWORD charsRead = size / sizeof(WCHAR);
        
        // Ensure strictly safe null-termination
        if (charsRead < maxChars) {
            buf[charsRead] = L'\0';
        } else {
            buf[maxChars - 1] = L'\0';
        }
        return std::wstring(buf);
    }
    return L"";
}

void ProcessCLSID(const std::wstring& clsidStr, const std::wstring& parentName) {
    if (g_quitting.load()) return;
    if (clsidStr.empty()) return;
    if (clsidStr == L"{6af09ec9-b429-11d4-a1fb-0090273514e2}") return;
    if (clsidStr == L"{e2bf9676-5f8f-435c-97eb-11607a5bedf7}") return;

    std::wstring keyPath = L"CLSID\\" + clsidStr + L"\\InProcServer32";
    HKEY hKey = OpenKey(HKEY_CLASSES_ROOT, keyPath.c_str());
    if (hKey) {
        std::wstring dllPath = ReadDefaultValue(hKey);
        RegCloseKey(hKey);
        
        if (dllPath.empty()) return;

        // Strip quotes if they exist before expanding environment strings
        if (dllPath.front() == L'"') {
            size_t endQuote = dllPath.find(L'"', 1);
            if (endQuote != std::wstring::npos) {
                dllPath = dllPath.substr(1, endQuote - 1);
            }
        }

        // Strip the '@' prefix used by MUI string definitions
        if (!dllPath.empty() && dllPath.front() == L'@') {
            dllPath = dllPath.substr(1);
        }

        // Strip resource indices (e.g., ", -100" or ", 1")
        size_t commaPos = dllPath.find(L',');
        if (commaPos != std::wstring::npos) {
            dllPath = dllPath.substr(0, commaPos);
        }
        
        WCHAR expanded[MAX_PATH];
        ExpandEnvironmentStringsW(dllPath.c_str(), expanded, MAX_PATH);
        
        PinDLL(expanded, parentName.empty() ? clsidStr : parentName);
    }
}

void ScanKeyForHandlers(HKEY hRoot, const std::wstring& path) {
    if (g_quitting.load()) return;
    HKEY hKey = OpenKey(hRoot, path.c_str());
    if (!hKey) return;

    WCHAR subKeyName[256];
    DWORD index = 0;
    DWORD len = 256;

    while (RegEnumKeyExW(hKey, index, subKeyName, &len, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
        if (g_quitting.load()) break; // Exit loop fast
        
        std::wstring name = subKeyName;
        if (subKeyName[0] == L'{') {
            ProcessCLSID(subKeyName, L"");
        } else {
            HKEY hSub = OpenKey(hKey, subKeyName);
            if (hSub) {
                std::wstring val = ReadDefaultValue(hSub);
                if (!val.empty() && val[0] == L'{') ProcessCLSID(val, name);
                RegCloseKey(hSub);
            }
        }
        len = 256;
        index++;
    }
    RegCloseKey(hKey);
}

void ScanShellVerbs(HKEY hRoot, const std::wstring& path) {
    if (g_quitting.load()) return;
    HKEY hKey = OpenKey(hRoot, path.c_str());
    if (!hKey) return;

    WCHAR verbName[256];
    DWORD index = 0;
    DWORD len = 256;

    while (RegEnumKeyExW(hKey, index, verbName, &len, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
        if (g_quitting.load()) break;
        
        HKEY hVerb = OpenKey(hKey, verbName);
        if (hVerb) {
            // Check ExplorerCommandHandler (Modern Windows UI Handlers)
            WCHAR cmdHandler[256];
            DWORD hSize = sizeof(cmdHandler);
            if (RegQueryValueExW(hVerb, L"ExplorerCommandHandler", NULL, NULL, (LPBYTE)cmdHandler, &hSize) == ERROR_SUCCESS) {
                ProcessCLSID(std::wstring(cmdHandler), L"ExplorerCommandHandler");
            }

            // Check DropTarget (Drag & Drop UI Handlers)
            HKEY hDrop = OpenKey(hVerb, L"DropTarget");
            if (hDrop) {
                WCHAR clsid[256];
                DWORD cSize = sizeof(clsid);
                if (RegQueryValueExW(hDrop, L"CLSID", NULL, NULL, (LPBYTE)clsid, &cSize) == ERROR_SUCCESS) {
                    ProcessCLSID(std::wstring(clsid), L"DropTarget");
                }
                RegCloseKey(hDrop);
            }

            // Check command subkey for DelegateExecute (Heavy Background Handlers)
            HKEY hCmd = OpenKey(hVerb, L"command");
            if (hCmd) {
                WCHAR delegateId[256];
                DWORD dSize = sizeof(delegateId);
                if (RegQueryValueExW(hCmd, L"DelegateExecute", NULL, NULL, (LPBYTE)delegateId, &dSize) == ERROR_SUCCESS) {
                    ProcessCLSID(std::wstring(delegateId), L"DelegateExecute");
                }
                RegCloseKey(hCmd);
            }
            RegCloseKey(hVerb);
        }
        len = 256;
        index++;
    }
    RegCloseKey(hKey);
}

void OptimizeExtension(const std::wstring& ext) {
    if (g_quitting.load()) return;
    Log(L"Scanning .%s", ext.c_str());
    
    // Scan Legacy Handlers
    ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"." + ext + L"\\ShellEx\\ContextMenuHandlers");
    ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"SystemFileAssociations\\." + ext + L"\\ShellEx\\ContextMenuHandlers");

    // Scan Modern Handlers
    ScanShellVerbs(HKEY_CLASSES_ROOT, L"." + ext + L"\\shell");
    ScanShellVerbs(HKEY_CLASSES_ROOT, L"SystemFileAssociations\\." + ext + L"\\shell");

    HKEY hExtKey = OpenKey(HKEY_CLASSES_ROOT, (L"." + ext).c_str());
    if (hExtKey) {
        // Read explicit ProgID
        std::wstring progID = ReadDefaultValue(hExtKey);
        if (!progID.empty()) {
            ScanKeyForHandlers(HKEY_CLASSES_ROOT, progID + L"\\ShellEx\\ContextMenuHandlers");
            ScanShellVerbs(HKEY_CLASSES_ROOT, progID + L"\\shell");
        }

        // Read PerceivedType (Crucial for Audio, Video, and Document files)
        WCHAR perceived[64];
        DWORD size = sizeof(perceived);
        if (RegQueryValueExW(hExtKey, L"PerceivedType", NULL, NULL, (LPBYTE)perceived, &size) == ERROR_SUCCESS) {
            std::wstring pType(perceived);
            ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"SystemFileAssociations\\" + pType + L"\\ShellEx\\ContextMenuHandlers");
            ScanShellVerbs(HKEY_CLASSES_ROOT, L"SystemFileAssociations\\" + pType + L"\\shell");
        }
        
        RegCloseKey(hExtKey);
    }
}

// -------------------------------------------------------------------------
// Main Thread
// -------------------------------------------------------------------------
DWORD WINAPI WalkerThread(LPVOID) {
    int delay = g_delay.load();
    if (delay < 1) delay = 1;

    // Interruptible Sleep
    for (int i = 0; i < delay * 10; i++) {
        if (g_quitting.load()) return 0;
        Sleep(100);
    }

    Log(L"Startup (v0.2.8)");

    // Capture Baseline RAM
    SIZE_T startMem = GetRamUsage();

    // 1. Priority: Nilesoft
    HMODULE hShell = LoadLibraryW(L"shell.dll");
    if (hShell) {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(hShell, path, MAX_PATH);
        PinDLL(path, L"Nilesoft Shell");
    }

    // 2. Priority: Image Handlers (Force Pin)
    WCHAR sysDir[MAX_PATH];
    GetSystemDirectoryW(sysDir, MAX_PATH);
    std::wstring sysPath = sysDir;
    PinDLL(L"C:\\Program Files\\Windows Photo Viewer\\PhotoViewer.dll", L"PhotoViewer (Force)");
    PinDLL(sysPath + L"\\shimgvw.dll", L"Legacy Image Viewer");

    // 3. Global Handlers & Verbs
    ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"*\\ShellEx\\ContextMenuHandlers");
    ScanShellVerbs(HKEY_CLASSES_ROOT, L"*\\shell");
    
    ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"AllFileSystemObjects\\ShellEx\\ContextMenuHandlers");
    ScanShellVerbs(HKEY_CLASSES_ROOT, L"AllFileSystemObjects\\shell");
    
    ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"Directory\\Background\\ShellEx\\ContextMenuHandlers");
    ScanShellVerbs(HKEY_CLASSES_ROOT, L"Directory\\Background\\shell");
    
    ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"Directory\\ShellEx\\ContextMenuHandlers");
    ScanShellVerbs(HKEY_CLASSES_ROOT, L"Directory\\shell");
    
    ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"Folder\\ShellEx\\ContextMenuHandlers");
    ScanShellVerbs(HKEY_CLASSES_ROOT, L"Folder\\shell");

    // 4. System File Associations for Images
    ScanKeyForHandlers(HKEY_CLASSES_ROOT, L"SystemFileAssociations\\image\\ShellEx\\ContextMenuHandlers");

    // 5. Custom Extensions
    for (const auto& ext : g_customExts) {
        OptimizeExtension(ext);
    }

    // Capture Final RAM
    SIZE_T endMem = GetRamUsage();
    
    // Calculate Delta (safely)
    double deltaMB = 0.0;
    if (endMem > startMem) {
        deltaMB = (double)(endMem - startMem) / (1024.0 * 1024.0);
    }

    Log(L"Done. Total RAM Impact: %.2f MB", deltaMB);
    return 0;
}

// -------------------------------------------------------------------------
// Init
// -------------------------------------------------------------------------
void LoadSettings() {
    g_delay.store(Wh_GetIntSetting(L"StartupDelay"));
    g_debug.store(Wh_GetIntSetting(L"EnableDebug"));
    
    PCWSTR customExtsRaw = Wh_GetStringSetting(L"CustomExtensions");
    if (customExtsRaw) {
        ParseExtensions(customExtsRaw);
        Wh_FreeStringSetting(customExtsRaw);
    }
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_quitting.store(false);
    g_hThread = CreateThread(NULL, 0, WalkerThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    g_quitting.store(true);
    
    if (g_hThread) {
        // Wait infinitely. Because we check g_quitting religiously, it WILL exit.
        // A timeout risks crashing Explorer if the DLL unloads while the thread is alive.
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
}