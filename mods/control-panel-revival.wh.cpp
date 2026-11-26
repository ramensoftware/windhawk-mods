// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival
// @description     Stops Windows 11 from redirecting Control Panel pages (System, Troubleshooting, Fonts, ...) to the Settings app
// @version         3.4.1
// @author          AdmXP8
// @include         explorer.exe
// @include         rundll32.exe
// @architecture    x86-64
// @compilerOptions -lshlwapi -lntdll
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
can revive:
advance sharing settings
troubleshooting,fonts,system,installed updates,default programs,devices and Printers
properties on this pc right click menu wich leads to the system applet
troublshooting link on network sharing center
task links that lead to a applet that is in the mod(ex:troubleshooting)
things that this mod cannot do:
BRO WHAT ELSE YOU NEDD(unhiding)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extraGuids: [""]
  $name: Extra GUIDs to un redirect
- passThroughNames: [""]
  $name: Applets to leave redirected
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <wchar.h>
#include <windhawk_utils.h>

//to in bakhs kafie appletai khe mikhai redirect nashan ro ezafe koni
static const WCHAR* kRedirectGuids[] = {
    L"::{BB06C0E4-D293-4F75-8A90-CB05B6477EEE}",  //System
    L"::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}",  //Troubleshooting
    L"::{D450A8A1-9568-45C7-9C0E-B4F9FB4537BD}",  //Installed Updates
    L"::{17CD9488-1228-4B2F-88CE-4298E93E0966}",  //Default Programs
    L"::{BD84B380-8CA2-1069-AB1D-08000948F534}",  //Fonts
    L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}",  //Devices and Printers
};

static const WCHAR kNetAdvancedSuffix[] =
    L"\\::{8E908FC9-BECC-40F6-915B-F4CA0E70D03D}\\Advanced";//advance sharing settings link on network and sharing center applet

static const int kMaxExtra = 32;
static WCHAR g_extraGuids[kMaxExtra][64];
static volatile LONG g_extraGuidCount = 0;
static WCHAR g_passNames[kMaxExtra][64];
static volatile LONG g_passNameCount = 0;

static void LoadList(const WCHAR* key, WCHAR (*dst)[64], volatile LONG* count) {
    InterlockedExchange(count, 0);
    LONG n = 0;
    for (int i = 0; i < kMaxExtra; i++) {
        PCWSTR s = Wh_GetStringSetting(L"%s[%d]", key, i);
        bool has = s && *s;
        if (has) {
            wcsncpy(dst[n], s, 63);
            dst[n][63] = 0;
            n++;
        }
        Wh_FreeStringSetting(s);
        if (!has) break;
    }
    InterlockedExchange(count, n);
}

static void LoadSettings() {
    LoadList(L"extraGuids", g_extraGuids, &g_extraGuidCount);
    LoadList(L"passThroughNames", g_passNames, &g_passNameCount);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

static thread_local bool g_inLog = false;
static volatile LONG g_skipLogged = 0;
static volatile LONG g_mapLogged = 0;
static HMODULE g_selfModule = nullptr;

static bool IsRedirectGuid(const WCHAR* s) {
    for (auto g : kRedirectGuids) {
        if (_wcsicmp(s, g) == 0) return true;
    }
    LONG n = g_extraGuidCount;
    for (LONG i = 0; i < n; i++) {
        if (_wcsicmp(s, g_extraGuids[i]) == 0) return true;
    }
    return false;
}

static bool IsPassThroughName(const WCHAR* name) {
    if (!name) return false;
    LONG n = g_passNameCount;
    for (LONG i = 0; i < n; i++) {
        if (_wcsicmp(name, g_passNames[i]) == 0) return true;
    }
    return false;
}

static HMODULE ModuleOf(void* addr) {
    HMODULE h = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)addr, &h)) {
        return nullptr;
    }
    return h;
}

static void FormatAddr(void* addr, WCHAR* out, size_t cch) {
    HMODULE h = ModuleOf(addr);
    WCHAR path[MAX_PATH] = L"?";
    if (h) GetModuleFileNameW(h, path, MAX_PATH);
    const WCHAR* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    _snwprintf(out, cch, L"%ls+0x%llX", name,
               h ? (unsigned long long)((BYTE*)addr - (BYTE*)h) : 0ULL);
    out[cch - 1] = 0;
}

static BYTE* volatile g_efBase = nullptr;
static BYTE* volatile g_efEnd = nullptr;

static bool InExplorerFrame(void* addr) {
    if (!g_efBase) {
        HMODULE h = GetModuleHandleW(L"explorerframe.dll");
        if (!h) return false;
        auto dos = (IMAGE_DOS_HEADER*)h;
        auto nt = (IMAGE_NT_HEADERS*)((BYTE*)h + dos->e_lfanew);
        g_efEnd = (BYTE*)h + nt->OptionalHeader.SizeOfImage;
        g_efBase = (BYTE*)h;
    }
    return (BYTE*)addr >= g_efBase && (BYTE*)addr < g_efEnd;
}
static bool IsHookLayerModule(HMODULE h) {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(h, path, MAX_PATH)) return false;
    WCHAR win[MAX_PATH];
    UINT n = GetWindowsDirectoryW(win, MAX_PATH);
    if (n && _wcsnicmp(path, win, n) == 0) {
        return _wcsicmp(path + n, L"\\dxgi.dll") == 0;
    }
    return true;
}

__attribute__((noinline)) static bool CallerChainIsExplorerFrame(bool log) {
    void* frames[12];
    USHORT n = RtlCaptureStackBackTrace(0, 12, frames, nullptr);
    bool result = false;
    bool decided = false;
    USHORT shown = 0;
    for (USHORT i = 0; i < n; i++) {
        HMODULE h = ModuleOf(frames[i]);
        if (h == g_selfModule) continue;
        if (log && shown < 6) {
            WCHAR where[96];
            FormatAddr(frames[i], where, 96);
            Wh_Log(L"SKIPBT #%u %s", (unsigned)shown, where);
            shown++;
        }
        if (decided) continue;
        if (!h) continue;
        if (InExplorerFrame(frames[i])) {
            result = true;
            decided = true;
            continue;
        }
        if (IsHookLayerModule(h)) continue;
        decided = true;
    }
    return result;
}

//kernelbase.dll export
using CompareStringOrdinal_t = decltype(&CompareStringOrdinal);
CompareStringOrdinal_t CompareStringOrdinal_orig;

int WINAPI CompareStringOrdinal_hook(LPCWCH a, int ca, LPCWCH b, int cb,
                                     BOOL ignoreCase) {
    int r = CompareStringOrdinal_orig(a, ca, b, cb, ignoreCase);
    if (r != CSTR_EQUAL || g_inLog) return r;

    const WCHAR* hit = nullptr;
    if (b && cb < 0 && IsRedirectGuid(b)) hit = b;
    else if (a && ca < 0 && IsRedirectGuid(a)) hit = a;
    if (!hit) return r;

    void* ret = __builtin_return_address(0);
    const WCHAR* via = nullptr;
    if (InExplorerFrame(ret)) via = L"direct";
    else if (CallerChainIsExplorerFrame(false)) via = L"relayed";

    if (via) {
        WCHAR where[96];
        FormatAddr(ret, where, 96);
        g_inLog = true;
        Wh_Log(L"BLOCK CSO %s via=%s caller=%s", hit, via, where);
        g_inLog = false;
        return CSTR_GREATER_THAN;
    }

    if (InterlockedIncrement(&g_skipLogged) <= 8) {
        WCHAR where[96];
        FormatAddr(ret, where, 96);
        g_inLog = true;
        Wh_Log(L"SKIP CSO %s (not from explorerframe.dll) caller=%s", hit, where);
        CallerChainIsExplorerFrame(true);
        g_inLog = false;
    }
    return r;
}

//network advanced sharing suffix
using FindStringOrdinal_t = decltype(&FindStringOrdinal);
FindStringOrdinal_t FindStringOrdinal_orig;

int WINAPI FindStringOrdinal_hook(DWORD flags, LPCWSTR src, int cchSrc,
                                  LPCWSTR val, int cchVal, BOOL ignoreCase) {
    int r = FindStringOrdinal_orig(flags, src, cchSrc, val, cchVal, ignoreCase);
    if (r >= 0 && val && cchVal < 0 && val[0] == L'\\' && !g_inLog &&
        _wcsicmp(val, kNetAdvancedSuffix) == 0) {
        void* ret = __builtin_return_address(0);
        if (InExplorerFrame(ret) || CallerChainIsExplorerFrame(false)) {
            g_inLog = true;
            Wh_Log(L"BLOCK FSO %s", val);
            g_inLog = false;
            return -1;
        }
    }
    return r;
}

bool (*MapLegacyName_orig)(void*, LPCWSTR, LPWSTR, UINT, bool*);

bool MapLegacyName_hook(void* pThis, LPCWSTR legacy, LPWSTR out, UINT u, bool* changed) {
    bool r = MapLegacyName_orig(pThis, legacy, out, u, changed);
    bool ch = changed ? *changed : false;
    bool mapped = r || ch;
    bool toSettings = mapped && out && StrStrIW(out, L"ms-settings:") != nullptr &&
                      !IsPassThroughName(legacy);

    if (!g_inLog && InterlockedIncrement(&g_mapLogged) <= 150) {
        g_inLog = true;
        Wh_Log(L"MAPLEGACY name=%s result=%d changed=%d new=%s %s",
               legacy ? legacy : L"(null)", (int)r, (int)ch,
               (mapped && out) ? out : L"(none)",
               toSettings ? L"-> SUPPRESSED" : L"-> passed");
        g_inLog = false;
    }

    if (toSettings) {
        if (changed) *changed = false;
        if (out) *out = L'\0';
        return false;
    }
    return r;
}

const WindhawkUtils::SYMBOL_HOOK shell32Hooks[] = {
    {
        {L"private: bool __cdecl COpenControlPanel::_MapLegacyName(unsigned short const *,unsigned short *,unsigned int,bool *)"},
        &MapLegacyName_orig,
        MapLegacyName_hook,
        true,
    },
};

BOOL Wh_ModInit() {
    g_selfModule = ModuleOf((void*)&Wh_ModInit);
    LoadSettings();

    HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
    void* csoKb = kb ? (void*)GetProcAddress(kb, "CompareStringOrdinal") : nullptr;
    void* fsoKb = kb ? (void*)GetProcAddress(kb, "FindStringOrdinal") : nullptr;
    Wh_SetFunctionHook(csoKb ? csoKb : (void*)CompareStringOrdinal,
                       (void*)CompareStringOrdinal_hook,
                       (void**)&CompareStringOrdinal_orig);
    Wh_SetFunctionHook(fsoKb ? fsoKb : (void*)FindStringOrdinal,
                       (void*)FindStringOrdinal_hook,
                       (void**)&FindStringOrdinal_orig);

    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    bool okSym = shell32 && WindhawkUtils::HookSymbols(shell32, shell32Hooks,
                                                        ARRAYSIZE(shell32Hooks));
    Wh_Log(L"Revival 2 loaded (_MapLegacyName hook: %d)", (int)okSym);
    return TRUE;
}
//az noskhe ghabli kheili behtare v slop ham nist khodam sakhtam
