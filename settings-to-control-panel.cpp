// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    Forces classic Control Panel to open instead of Windows 10/11 Settings app using native components
// @version        8.0.7
// @author         babamohammed
// @include        explorer.exe
// @compilerOptions -lshell32 -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Redirect Settings → Control Panel (v8.0.7)

This mod intercepts modern `ms-settings:` URI protocols and forces Windows to open their classic Control Panel equivalents. It relies entirely on native Windows components and legacy CLSIDs without requiring any third-party external programs.

### Compatibility & Limitations:
- **Windows 10:** Highly effective (~70% redirection success rate), as most legacy control panels are fully intact and accessible via standard shell hooks.
- **Windows 11:** Limited effectiveness (acts as a ~5% baseline restoration). Microsoft has deeply hardcoded or deprecated many legacy CLSIDs, bypassing classic shell activation methods. However, key elements like native network properties (`ncpa.cpl`) and specific dialog mappings still function.

### Key Features:
- **Smart Desktop Personalization Hook:** Contextual awareness based on `hwnd` detection. Clicking "Personalize" from the desktop right-click menu correctly targets the main classic Personalization window, while clicking "Desktop Background" inside an existing shell folder still brings up the wallpaper page.
- **CreateProcess Interception:** Intercepts modern overrides on commands like `control system` to dynamically fall back onto legacy System Properties (`sysdm.cpl`).
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <algorithm>
#include <unordered_map>

static std::unordered_map<std::wstring, std::wstring> g_mappings;

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

static void InitMappings() {
    const std::wstring kPers = L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}";
    const std::wstring kWall = kPers + L"\\pageWallpaper";

    g_mappings = {
        // NOTE: ms-settings:personalization-background is handled specially via hwnd
        // detection in the hook — it is intentionally absent from this table.

        // === Personalization ===
        {L"ms-settings:personalization-background-wallpaper",  kWall},
        {L"ms-settings:personalization-background-slideshow",  kWall},
        {L"ms-settings:background",                            kWall},
        {L"ms-settings:personalization",                       kPers},
        {L"ms-settings:personalization-colors",                kPers + L"\\pageColorization"},
        {L"ms-settings:colors",                                kPers + L"\\pageColorization"},
        {L"ms-settings:themes",                                kPers},
        {L"ms-settings:lockscreen",                            kPers},

        // === System ===
        {L"ms-settings:system",                       L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"},
        {L"ms-settings:display",                      L"desk.cpl"},
        {L"ms-settings:display-advanced",             L"shell:::{B2C761C6-29BC-4f19-9251-E6195265BAF1}"},
        {L"ms-settings:sound",                        L"mmsys.cpl"},
        {L"ms-settings:notifications",                L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"},
        {L"ms-settings:about",                        L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"},
        {L"ms-settings:sysinfo",                      L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"},
        {L"ms-settings:backup",                       L"shell:::{B98A2BEA-7D42-4558-8BD1-832F41BAC6FD}"},
        {L"ms-settings:optionalfeatures",             L"OptionalFeatures.exe"},
        {L"ms-settings:devicemanager",                L"devmgmt.msc"},
        {L"ms-settings:screenrotation",               L"shell:::{5ea4f148-308c-46d7-98a9-49041b1dd468}"},

        // === Network & Internet ===
        {L"ms-settings:network",                      L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-wifi",                 L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-ethernet",             L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-vpn",                  L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-proxy",                L"inetcpl.cpl,,4"},
        {L"ms-settings:network-status",               L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:datausage",                    L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},

        // === Accounts ===
        {L"ms-settings:yourinfo",                     L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:emailandaccounts",             L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:signinoptions",                L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:otherusers",                   L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:workplace",                    L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:family",                       L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},

        // === Time & Language ===
        {L"ms-settings:dateandtime",                  L"timedate.cpl"},
        {L"ms-settings:regionlanguage",               L"intl.cpl"},

        // === Ease of Access ===
        {L"ms-settings:easeofaccess",                 L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-highcontrast",    L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-closedcaptioning", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},

        // === Hardware ===
        {L"ms-settings:printers",                     L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:printers-scanners",            L"shell:::{2227A280-3AEA-1069-A2DE-08002B30309D}"},
        {L"ms-settings:mousetouchpad",                L"main.cpl"},
        {L"ms-settings:devices-touchpad",             L"main.cpl"},
        {L"ms-settings:keyboard",                     L"main.cpl"},
        {L"ms-settings:typing",                       L"main.cpl,,1"},
        {L"ms-settings:bluetooth",                    L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:usb",                          L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:autoplay",                     L"shell:::{9C60DE1E-E5FC-40f4-A487-460851A8D915}"},
        {L"ms-settings:powersleep",                   L"powercfg.cpl"},
        {L"ms-settings:pen",                          L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},

        // === Apps ===
        {L"ms-settings:appsfeatures",                 L"appwiz.cpl"},
        {L"ms-settings:defaultapps",                  L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}"},

        // === Security ===
        {L"ms-settings:firewall",                     L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},

        // === Direct CLSID mappings ===
        {L"shell:::{f20df4e5-ea01-41a2-b02a-dcbd92d4696e}", kWall},
    };
}

static std::wstring NormalizeUri(const std::wstring& uri) {
    std::wstring result = ToLower(uri);

    size_t pos = result.find(L"ms-settings://");
    if (pos != std::wstring::npos)
        result = L"ms-settings:" + result.substr(pos + 15);

    pos = result.find(L'?');
    if (pos != std::wstring::npos)
        result = result.substr(0, pos);

    while (!result.empty() && result.back() == L'/')
        result.pop_back();

    return result;
}

static bool IsMsSettings(const wchar_t* s) {
    if (!s) return false;
    return ToLower(s).find(L"ms-settings:") != std::wstring::npos;
}

static bool IsShellClsid(const wchar_t* s) {
    if (!s) return false;
    return ToLower(s).find(L"shell:::") != std::wstring::npos;
}

static void LaunchTarget(const std::wstring& command) {
    Wh_Log(L"[EXEC] %s", command.c_str());

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    PROCESS_INFORMATION pi = {};

    std::wstring cmdLine;
    if (command.find(L".exe") != std::wstring::npos || command.find(L".msc") != std::wstring::npos) {
        cmdLine = command;
    } else if (command.find(L"shell:::") == 0) {
        cmdLine = L"explorer.exe " + command;
    } else if (command.empty()) {
        cmdLine = L"control.exe";
    } else {
        cmdLine = L"control.exe " + command;
    }

    std::wstring mutableCmd = cmdLine;
    if (CreateProcessW(nullptr, mutableCmd.data(), nullptr, nullptr, FALSE,
                       0, nullptr, nullptr, &si, &pi)) {
        if (pi.hProcess) CloseHandle(pi.hProcess);
        if (pi.hThread)  CloseHandle(pi.hThread);
    } else {
        Wh_Log(L"[ERROR] CreateProcess failed for: %s (error: %lu)", cmdLine.c_str(), GetLastError());
    }
}

static bool IsPersonalizationWindow(HWND hwnd) {
    if (!hwnd) {
        Wh_Log(L"[HWND] hwnd is NULL -> context menu, not Personalization window");
        return false;
    }

    HWND h = hwnd;
    while (h) {
        wchar_t cls[256]   = {};
        wchar_t title[512] = {};
        GetClassNameW(h, cls, 256);
        GetWindowTextW(h, title, 512);

        Wh_Log(L"[HWND] %p  class='%s'  title='%s'", h, cls, title);

        std::wstring c = ToLower(cls);
        std::wstring t = ToLower(title);

        if (c == L"progman" || c == L"workerw" || c == L"shelldll_defview") {
            Wh_Log(L"[HWND] Desktop window class -> context menu");
            return false;
        }

        if (c == L"cabinetwclass") {
            Wh_Log(L"[HWND] CabinetWClass with title='%s' -> Personalization window", title);
            return true;
        }

        if (t.find(L"personaliz") != std::wstring::npos) {
            Wh_Log(L"[HWND] Title match -> Personalization window: %s", title);
            return true;
        }

        HWND parent = GetParent(h);
        if (!parent || parent == h) break;
        h = parent;
    }

    Wh_Log(L"[HWND] No Personalization window found in chain -> context menu");
    return false;
}

static std::wstring ResolvePersonalizationBackground(HWND hwnd) {
    const std::wstring kPers = L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}";
    const std::wstring kWall = kPers + L"\\pageWallpaper";

    if (IsPersonalizationWindow(hwnd)) {
        Wh_Log(L"[RESOLVE] personalization-background -> wallpaper (called from inside Personalization)");
        return kWall;
    } else {
        Wh_Log(L"[RESOLVE] personalization-background -> Personalization (called from context menu)");
        return kPers;
    }
}

// ── Hook: ShellExecuteExW ────────────────────────────────────────────────────

using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
ShellExecuteExW_t ShellExecuteExW_orig;

BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    if (pei) {
        Wh_Log(L"[DEBUG] hwnd=%p  lpFile=%s  lpParameters=%s",
               pei->hwnd,
               pei->lpFile       ? pei->lpFile       : L"(null)",
               pei->lpParameters ? pei->lpParameters : L"(null)");

        const wchar_t* file   = pei->lpFile;
        const wchar_t* params = pei->lpParameters;
        const wchar_t* rawUri = nullptr;
        std::wstring uri;

        if (IsMsSettings(file) || IsMsSettings(params)) {
            rawUri = IsMsSettings(file) ? file : params;
            uri = NormalizeUri(rawUri);
        } else if (IsShellClsid(file) || IsShellClsid(params)) {
            rawUri = IsShellClsid(file) ? file : params;
            uri = ToLower(rawUri);
        }

        if (!uri.empty()) {
            Wh_Log(L"[HOOK] %s", uri.c_str());

            std::wstring target;

            if (uri == L"ms-settings:personalization-background") {
                target = ResolvePersonalizationBackground(pei->hwnd);
            } else {
                auto it = g_mappings.find(uri);
                if (it != g_mappings.end()) {
                    target = it->second;
                    Wh_Log(L"[MAP] %s -> %s", uri.c_str(), target.c_str());
                } else {
                    Wh_Log(L"[FALLBACK] %s -> control.exe", uri.c_str());
                }
            }

            LaunchTarget(target);
            if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
            return TRUE;
        }
    }
    return ShellExecuteExW_orig(pei);
}

// ── Hook: ShellExecuteW ──────────────────────────────────────────────────────

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
ShellExecuteW_t ShellExecuteW_orig;

HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR op, LPCWSTR file,
                                     LPCWSTR params, LPCWSTR dir, INT show) {
    Wh_Log(L"[DEBUG-W] hwnd=%p  file=%s  params=%s",
           hwnd,
           file   ? file   : L"(null)",
           params ? params : L"(null)");

    const wchar_t* rawUri = nullptr;
    std::wstring uri;

    if (IsMsSettings(file) || IsMsSettings(params)) {
        rawUri = IsMsSettings(file) ? file : params;
        uri = NormalizeUri(rawUri);
    } else if (IsShellClsid(file) || IsShellClsid(params)) {
        rawUri = IsShellClsid(file) ? file : params;
        uri = ToLower(rawUri);
    }

    if (!uri.empty()) {
        Wh_Log(L"[HOOK-W] %s", uri.c_str());

        std::wstring target;

        if (uri == L"ms-settings:personalization-background") {
            target = ResolvePersonalizationBackground(hwnd);
        } else {
            auto it = g_mappings.find(uri);
            if (it != g_mappings.end()) {
                target = it->second;
                Wh_Log(L"[MAP-W] %s -> %s", uri.c_str(), target.c_str());
            } else {
                Wh_Log(L"[FALLBACK-W] %s -> control.exe", uri.c_str());
            }
        }

        LaunchTarget(target);
        return (HINSTANCE)42;
    }
    return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
}

// ── Hook: CreateProcessW ─────────────────────────────────────────────────────

using CreateProcessW_t = BOOL(WINAPI*)(
    LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
    BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION
);
CreateProcessW_t CreateProcessW_orig = nullptr;

BOOL WINAPI CreateProcessW_hook(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation)
{
    std::wstring cmdLine = lpCommandLine ? ToLower(std::wstring(lpCommandLine)) : L"";
    
    if (cmdLine.find(L"control") != std::wstring::npos && 
        cmdLine.find(L"system") != std::wstring::npos) {
        Wh_Log(L"[HOOK-CPW] Intercepted control system command: %s", lpCommandLine);
        LaunchTarget(L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}");
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    
    return CreateProcessW_orig(
        lpApplicationName, lpCommandLine,
        lpProcessAttributes, lpThreadAttributes,
        bInheritHandles, dwCreationFlags,
        lpEnvironment, lpCurrentDirectory,
        lpStartupInfo, lpProcessInformation
    );
}

// ── Windhawk Entry Points ────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"[v8.0.7] Redirect Settings to Control Panel - Initializing...");

    InitMappings();
    Wh_Log(L"[v8.0.7] %zu URI mappings loaded", g_mappings.size());

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"[v8.0.7] ERROR: Could not load shell32.dll");
        return FALSE;
    }

    auto pExW = (void*)GetProcAddress(hShell32, "ShellExecuteExW");
    auto pW   = (void*)GetProcAddress(hShell32, "ShellExecuteW");

    if (!pExW || !pW) {
        Wh_Log(L"[v8.0.7] ERROR: Required functions not found in shell32.dll");
        return FALSE;
    }

    bool ok1 = Wh_SetFunctionHook(pExW, (void*)ShellExecuteExW_hook, (void**)&ShellExecuteExW_orig);
    bool ok2 = Wh_SetFunctionHook(pW,   (void*)ShellExecuteW_hook,   (void**)&ShellExecuteW_orig);

    Wh_Log(L"[v8.0.7] Hook results: ShellExecuteExW=%d  ShellExecuteW=%d", ok1, ok2);

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) hKernel32 = LoadLibraryW(L"kernel32.dll");
    if (hKernel32) {
        auto pCPW = (void*)GetProcAddress(hKernel32, "CreateProcessW");
        if (pCPW) {
            bool ok3 = Wh_SetFunctionHook(pCPW, (void*)CreateProcessW_hook, (void**)&CreateProcessW_orig);
            Wh_Log(L"[v8.0.7] CreateProcessW hook: %d", ok3);
        }
    }

    if (!ok1 && !ok2) {
        Wh_Log(L"[v8.0.7] ERROR: Failed to install any hooks");
        return FALSE;
    }

    Wh_Log(L"[v8.0.7] Ready!");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[v8.0.7] Unloaded.");
}

void Wh_ModSettingsChanged() {}
