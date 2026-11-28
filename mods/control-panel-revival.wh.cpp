// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival
// @description     Stops Windows 11 from redirecting Control Panel pages (System, Troubleshooting, Fonts, ...) to the Settings app
// @version         4.0.0
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @include         rundll32.exe
// @architecture    x86-64
// @compilerOptions -lshlwapi -lntdll
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Control Panel Revival

Windows redirects several classic Control Panel pages to the Settings app.
This mod lets the classic pages open again.

## What it restores

* System, including the This PC > Properties link
* Troubleshooting, Installed Updates, Default Programs, Fonts, Devices and Printers
* Network and Sharing Center > advanced sharing settings
* Control Panel task links that lead to those pages

Everything else is left alone: a comparison is only changed when Control
Panel's own UI code makes it, so unrelated shell behaviour keeps working.

## What it does not do

It does not unhide Control Panel items that Windows hides on purpose. For
those there is the **Applet GUIDs to try to unhide** setting below, plus the
separate `restore-classic-cpls` mod, which covers some of them.

### Tested on:

* Windows 11 25h2 , 24h2
* Windows 10 1709

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extraGuids: [""]
  $name: Extra GUIDs to un redirect
  $description: >-
    One entry per row. GUIDs can be written with or without braces and with or
    without the "::" prefix; canonical names (Microsoft.Something) also work.
    An empty row ends the list.
- unhideGuids: [""]
  $name: Applet GUIDs to try to unhide (experimental)
  $description: >-
    Same format as above. Items listed here have the comparison that marks
    them hidden blocked, including when shell32 asks for it. Best effort only.
- passThroughNames: [""]
  $name: Applets to leave redirected
  $description: >-
    Canonical names (for example Microsoft.WindowsUpdate) whose redirect to
    Settings should be kept, so the click does not end up doing nothing.
- discoveryMode: false
  $name: Discovery mode (diagnostics)
  $description: >-
    Logs every equal comparison that involves a "::{GUID}"-looking string,
    with the module and offset of the caller. Verbose.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <wchar.h>
#include <windhawk_utils.h>

#if defined(__GNUC__) || defined(__clang__)
#define CPR_NOINLINE __attribute__((noinline))
#else
#define CPR_NOINLINE __declspec(noinline)
#endif

//targets
static const WCHAR* kRedirectGuids[] = {
    L"::{BB06C0E4-D293-4F75-8A90-CB05B6477EEE}",  //System
    L"::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}",  //Troubleshooting
    L"::{D450A8A1-9568-45C7-9C0E-B4F9FB4537BD}",  //Installed Updates
    L"::{17CD9488-1228-4B2F-88CE-4298E93E0966}",  //Default Programs
    L"::{BD84B380-8CA2-1069-AB1D-08000948F534}",  //Fonts
    L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}",  //Devices and Printers
};

//target...
static const WCHAR kDevicesAndPrinters[] =
    L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}";  //devices and printers

//another target
static const WCHAR kNetAdvancedSuffix[] =
    L"\\::{8E908FC9-BECC-40F6-915B-F4CA0E70D03D}\\Advanced";  //advance sharing settings (it's important for homegroup if u can port it)

#define SCOPE_EF       0x1u
#define SCOPE_SHELL32  0x2u

static const int kMaxExtra = 32;
static const int kMaxIdLen = 64;

static WCHAR g_extraGuids[kMaxExtra][kMaxIdLen];
static volatile LONG g_extraGuidCount = 0;
static WCHAR g_unhideGuids[kMaxExtra][kMaxIdLen];
static volatile LONG g_unhideGuidCount = 0;
static WCHAR g_passNames[kMaxExtra][kMaxIdLen];
static volatile LONG g_passNameCount = 0;
static volatile LONG g_discoveryMode = 0;

static size_t BoundedLen(const WCHAR* s, size_t maxLen) {
    size_t n = 0;
    while (n < maxLen && s[n]) n++;
    return n;
}

static bool IsHexW(WCHAR c) {
    return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') ||
           (c >= L'A' && c <= L'F');
}

static bool LooksLikeBareGuid(const WCHAR* s, size_t len) {
    if (len != 36) return false;
    for (size_t i = 0; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (s[i] != L'-') return false;
        } else if (!IsHexW(s[i])) {
            return false;
        }
    }
    return true;
}

static bool NormalizeAppletId(const WCHAR* raw, WCHAR* dst, size_t cchDst) {
    if (!raw || !dst || cchDst < 8) return false;

    const WCHAR* begin = raw;
    while (*begin == L' ' || *begin == L'\t') begin++;
    const WCHAR* end = begin + wcslen(begin);
    while (end > begin && (end[-1] == L' ' || end[-1] == L'\t' ||
                           end[-1] == L'\r' || end[-1] == L'\n')) {
        end--;
    }
    size_t len = (size_t)(end - begin);
    if (len == 0 || len >= cchDst) return false;

    WCHAR tmp[128];
    if (len >= ARRAYSIZE(tmp)) return false;
    wmemcpy(tmp, begin, len);
    tmp[len] = 0;

    if (wcsncmp(tmp, L"::", 2) == 0) {
        wcscpy(dst, tmp);
        return true;
    }
    if (tmp[0] == L'{' && tmp[len - 1] == L'}') {
        _snwprintf(dst, cchDst, L"::%s", tmp);
        dst[cchDst - 1] = 0;
        return true;
    }
    if (LooksLikeBareGuid(tmp, len)) {
        _snwprintf(dst, cchDst, L"::{%s}", tmp);
        dst[cchDst - 1] = 0;
        return true;
    }
    wcscpy(dst, tmp);
    return true;
}

static void LoadList(const WCHAR* key, WCHAR (*dst)[kMaxIdLen],
                     volatile LONG* count, bool normalize) {
    InterlockedExchange(count, 0);
    LONG n = 0;
    for (int i = 0; i < kMaxExtra; i++) {
        PCWSTR s = Wh_GetStringSetting(L"%s[%d]", key, i);
        bool has = s && *s;
        if (has) {
            WCHAR norm[kMaxIdLen];
            const WCHAR* value = s;
            if (normalize) {
                if (NormalizeAppletId(s, norm, kMaxIdLen)) {
                    value = norm;
                    if (_wcsicmp(norm, s) != 0) {
                        Wh_Log(L"%s[%d]: \"%s\" -> \"%s\"", key, i, s, norm);
                    }
                } else {
                    Wh_Log(L"%s[%d]: ignoring unusable entry (\"%s\")", key, i, s);
                    value = nullptr;
                }
            }
            if (value) {
                size_t len = wcslen(value);
                if (len >= (size_t)kMaxIdLen) {
                    Wh_Log(L"%s[%d]: ignoring entry, too long (%u chars)",
                           key, i, (unsigned)len);
                } else if (n < kMaxExtra) {
                    wcscpy(dst[n], value);
                    n++;
                } else {
                    Wh_Log(L"%s: more than %d entries, ignoring the rest",
                           key, kMaxExtra);
                }
            }
        }
        Wh_FreeStringSetting(s);
        if (!has) break;
    }
    InterlockedExchange(count, n);
}

static void LoadSettings() {
    LoadList(L"extraGuids", g_extraGuids, &g_extraGuidCount, true);
    LoadList(L"unhideGuids", g_unhideGuids, &g_unhideGuidCount, true);
    LoadList(L"passThroughNames", g_passNames, &g_passNameCount, false);
    InterlockedExchange(&g_discoveryMode,
                        Wh_GetIntSetting(L"discoveryMode") ? 1 : 0);
    Wh_Log(L"settings: extra=%d unhide=%d passthrough=%d discovery=%d",
           (int)g_extraGuidCount, (int)g_unhideGuidCount,
           (int)g_passNameCount, (int)g_discoveryMode);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

static thread_local bool g_inLog = false;
static volatile LONG g_skipLogged = 0;
static volatile LONG g_discLogged = 0;
static volatile LONG g_mapLogged = 0;

static HMODULE g_selfModule = nullptr;
static HMODULE g_modExplorerFrame = nullptr;
static HMODULE g_modShell32 = nullptr;

static HMODULE ModuleOf(void* addr) {
    HMODULE h = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)addr, &h)) {
        return nullptr;
    }
    return h;
}

static DWORD ModuleScopeOf(HMODULE h) {
    if (!h) return 0;
    if (!g_modExplorerFrame) g_modExplorerFrame = GetModuleHandleW(L"explorerframe.dll");
    if (!g_modShell32) g_modShell32 = GetModuleHandleW(L"shell32.dll");
    if (h == g_modExplorerFrame) return SCOPE_EF;
    if (h == g_modShell32) return SCOPE_SHELL32;
    return 0;
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

static bool IsHookLayerModule(HMODULE h) {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(h, path, MAX_PATH)) return false;
    WCHAR win[MAX_PATH];
    UINT n = GetWindowsDirectoryW(win, MAX_PATH);
    if (n && _wcsnicmp(path, win, n) == 0) {
        return _wcsicmp(path + n, L"\\dxgi.dll") == 0;  //EP
    }
    return true;
}

CPR_NOINLINE static DWORD WalkCallerScope(bool log) {
    void* frames[16];
    USHORT n = RtlCaptureStackBackTrace(0, 16, frames, nullptr);
    DWORD result = 0;
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
        DWORD scope = ModuleScopeOf(h);
        if (scope) {
            result = scope;
            decided = true;
            continue;
        }
        if (IsHookLayerModule(h)) continue;
        decided = true;
    }
    return result;
}

static DWORD ResolveCallerScope(void* ret, const WCHAR** viaOut) {
    HMODULE h = ModuleOf(ret);
    if (h && h != g_selfModule) {
        DWORD scope = ModuleScopeOf(h);
        if (scope) {
            if (viaOut) *viaOut = L"direct";
            return scope;
        }
        if (!IsHookLayerModule(h)) {
            if (viaOut) *viaOut = L"foreign";
            return 0;
        }
    }
    DWORD scope = WalkCallerScope(false);
    if (viaOut) *viaOut = scope ? L"relayed" : L"foreign";
    return scope;
}

static bool MatchTarget(const WCHAR* s, int cch, DWORD* scopeOut) {
    if (!s) return false;

    size_t len;
    if (cch < 0) {
        len = BoundedLen(s, kMaxIdLen - 1);
        if (len == 0) return false;
    } else {
        len = (size_t)cch;
        if (len == 0 || len >= (size_t)kMaxIdLen) return false;
    }

    WCHAR buf[kMaxIdLen];
    wmemcpy(buf, s, len);
    buf[len] = 0;
    len = BoundedLen(buf, len);
    while (len > 0 && (buf[len - 1] == L' ' || buf[len - 1] == L'\t')) {
        buf[--len] = 0;
    }
    if (len < 8) return false;

    for (auto g : kRedirectGuids) {
        if (_wcsicmp(buf, g) == 0) {
            DWORD scope = SCOPE_EF;
            if (_wcsicmp(buf, kDevicesAndPrinters) == 0) scope |= SCOPE_SHELL32;
            *scopeOut = scope;
            return true;
        }
    }

    LONG n = g_extraGuidCount;
    for (LONG i = 0; i < n; i++) {
        if (_wcsicmp(buf, g_extraGuids[i]) == 0) {
            *scopeOut = SCOPE_EF;
            return true;
        }
    }

    n = g_unhideGuidCount;
    for (LONG i = 0; i < n; i++) {
        if (_wcsicmp(buf, g_unhideGuids[i]) == 0) {
            *scopeOut = SCOPE_EF | SCOPE_SHELL32;
            return true;
        }
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

static void CopyForLog(const WCHAR* s, int cch, WCHAR* dst, size_t cchDst) {
    if (!s || cchDst == 0) {
        if (cchDst) dst[0] = 0;
        return;
    }
    size_t len = (cch < 0) ? BoundedLen(s, cchDst - 1)
                           : (cch < (int)cchDst ? (size_t)cch : cchDst - 1);
    wmemcpy(dst, s, len);
    dst[len] = 0;
}

using CompareStringOrdinal_t = decltype(&CompareStringOrdinal);
CompareStringOrdinal_t CompareStringOrdinal_orig;

int WINAPI CompareStringOrdinal_hook(LPCWCH a, int ca, LPCWCH b, int cb,
                                     BOOL ignoreCase) {
    int r = CompareStringOrdinal_orig(a, ca, b, cb, ignoreCase);
    if (r != CSTR_EQUAL || g_inLog) return r;

    void* ret = __builtin_return_address(0);

    if (g_discoveryMode) {
        const WCHAR* cand = nullptr;
        int candCch = 0;
        if (b && cb != 0 && b[0] == L':' && b[1] == L':' && b[2] == L'{') {
            cand = b; candCch = cb;
        } else if (a && ca != 0 && a[0] == L':' && a[1] == L':' && a[2] == L'{') {
            cand = a; candCch = ca;
        }
        if (cand && InterlockedIncrement(&g_discLogged) <= 500) {
            WCHAR outBuf[80], where[96];
            CopyForLog(cand == b ? a : b, cand == b ? ca : cb, outBuf,
                       ARRAYSIZE(outBuf));
            FormatAddr(ret, where, 96);
            const WCHAR* via = nullptr;
            DWORD scope = ResolveCallerScope(ret, &via);
            g_inLog = true;
            Wh_Log(L"DISC %s == \"%s\" caller=%s scope=%u via=%s", cand, outBuf,
                   where, (unsigned)scope, via ? via : L"?");
            g_inLog = false;
        }
    }

    DWORD needScope = 0;
    const WCHAR* hit = nullptr;
    if (b && MatchTarget(b, cb, &needScope)) hit = b;
    else if (a && MatchTarget(a, ca, &needScope)) hit = a;
    if (!hit) return r;

    const WCHAR* via = nullptr;
    DWORD callerScope = ResolveCallerScope(ret, &via);
    if (callerScope & needScope) {
        WCHAR where[96];
        FormatAddr(ret, where, 96);
        if (InterlockedIncrement(&g_skipLogged) <= 200) {
            g_inLog = true;
            Wh_Log(L"BLOCK CSO %s via=%s caller=%s", hit,
                   via ? via : L"?", where);
            g_inLog = false;
        }
        return CSTR_GREATER_THAN;
    }

    if (InterlockedIncrement(&g_skipLogged) <= 8) {
        WCHAR where[96];
        FormatAddr(ret, where, 96);
        g_inLog = true;
        Wh_Log(L"SKIP CSO %s (caller scope %u not in %u) caller=%s", hit,
               (unsigned)callerScope, (unsigned)needScope, where);
        WalkCallerScope(true);
        g_inLog = false;
    }
    return r;
}

using FindStringOrdinal_t = decltype(&FindStringOrdinal);
FindStringOrdinal_t FindStringOrdinal_orig;

int WINAPI FindStringOrdinal_hook(DWORD flags, LPCWSTR src, int cchSrc,
                                  LPCWSTR val, int cchVal, BOOL ignoreCase) {
    int r = FindStringOrdinal_orig(flags, src, cchSrc, val, cchVal, ignoreCase);
    if (r >= 0 && val && cchVal < 0 && val[0] == L'\\' && !g_inLog &&
        _wcsicmp(val, kNetAdvancedSuffix) == 0) {
        const WCHAR* via = nullptr;
        DWORD scope = ResolveCallerScope(__builtin_return_address(0), &via);
        if (scope & SCOPE_EF) {
            g_inLog = true;
            Wh_Log(L"BLOCK FSO %s via=%s", val, via);
            g_inLog = false;
            return -1;
        }
    }
    return r;
}

bool (*MapLegacyName_orig)(void*, LPCWSTR, LPWSTR, UINT, bool*);

static bool ContainsMsSettings(const WCHAR* s, size_t maxLen) {
    static const WCHAR kUri[] = L"ms-settings:";
    const size_t kUriLen = ARRAYSIZE(kUri) - 1;
    size_t len = BoundedLen(s, maxLen);
    if (len < kUriLen) return false;
    for (size_t i = 0; i + kUriLen <= len; i++) {
        if (_wcsnicmp(s + i, kUri, kUriLen) == 0) return true;
    }
    return false;
}

bool MapLegacyName_hook(void* pThis, LPCWSTR legacy, LPWSTR out, UINT u, bool* changed) {
    bool r = MapLegacyName_orig(pThis, legacy, out, u, changed);
    bool ch = changed ? *changed : false;

    bool toSettings = (r && out && ch) ? ContainsMsSettings(out, 512) : false;

    if (!g_inLog && InterlockedIncrement(&g_mapLogged) <= 150) {
        WCHAR outBuf[128];
        CopyForLog(out, (r && out) ? (int)ARRAYSIZE(outBuf) : 0, outBuf,
                   ARRAYSIZE(outBuf));
        g_inLog = true;
        Wh_Log(L"MAPLEGACY name=%s result=%d changed=%d new=%s %s",
               legacy ? legacy : L"(null)", (int)r, (int)ch, outBuf,
               toSettings ? L"-> SUPPRESSED" : L"-> passed");
        g_inLog = false;
    }

    //redirect break
    if (toSettings && !IsPassThroughName(legacy)) {
        if (changed) *changed = false;
        if (out) *out = L'\0';//by bye
        return false;
    }
    return r;
}

const WindhawkUtils::SYMBOL_HOOK user32DllHooks[] = {
    {
        {L"private: bool __cdecl COpenControlPanel::_MapLegacyName(unsigned short const *,unsigned short *,unsigned int,bool *)"},
        &MapLegacyName_orig,
        MapLegacyName_hook,
        true,
    },
};

static void LogOsBuild() {
    typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto pRtlGetVersion = ntdll
        ? (RtlGetVersionPtr)GetProcAddress(ntdll, "RtlGetVersion")
        : nullptr;
    if (!pRtlGetVersion) return;
    RTL_OSVERSIONINFOW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    if (pRtlGetVersion(&osvi) == 0) {
        Wh_Log(L"OS %lu.%lu build %lu", osvi.dwMajorVersion,
               osvi.dwMinorVersion, osvi.dwBuildNumber);
    }
}

BOOL Wh_ModInit() {
    g_selfModule = ModuleOf((void*)&Wh_ModInit);
    LogOsBuild();
    LoadSettings();

    HMODULE kb = GetModuleHandleW(L"kernelbase.dll");

    void* csoKb = kb ? (void*)GetProcAddress(kb, "CompareStringOrdinal") : nullptr;
    void* cso = csoKb ? csoKb : (void*)CompareStringOrdinal;
    bool csoOk = Wh_SetFunctionHook(cso, (void*)CompareStringOrdinal_hook,
                                    (void**)&CompareStringOrdinal_orig);
    Wh_Log(L"CompareStringOrdinal hook: %d at %p (%s)", (int)csoOk, cso,
           csoKb ? L"kernelbase" : L"import");

    void* fsoKb = kb ? (void*)GetProcAddress(kb, "FindStringOrdinal") : nullptr;
    void* fso = fsoKb ? fsoKb : (void*)FindStringOrdinal;
    bool fsoOk = Wh_SetFunctionHook(fso, (void*)FindStringOrdinal_hook,
                                    (void**)&FindStringOrdinal_orig);
    Wh_Log(L"FindStringOrdinal hook: %d at %p (%s)", (int)fsoOk, fso,
           fsoKb ? L"kernelbase" : L"import");

    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    bool okSym = shell32 && WindhawkUtils::HookSymbols(shell32, user32DllHooks,
                                                      ARRAYSIZE(user32DllHooks));
    Wh_Log(L"cpr loaded correct (_MapLegacyName hook: %d)", (int)okSym);

    if (!csoOk && !fsoOk && !okSym) {
        Wh_Log(L"All hooks failed disabling the mod in this process");
        return FALSE;
    }
    return TRUE;
}
