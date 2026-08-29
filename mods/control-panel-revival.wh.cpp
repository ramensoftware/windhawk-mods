// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival
// @description     Prevents Control Panel applets from redirecting to the modern Settings app on Windows 11 23H2+ by unhiding legacy elements safely.
// @version         0.9.9
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Control Panel Revival

### What does this mod do?

This mod is designed to restore sections of the Control Panel—such as Troubleshooting, Installed Updates, Default Programs, and others—that are redirected to the Settings app in Windows 11 (version 23H2 and later) and can no longer be launched even via shell commands.
You can also add the ID of your desired applet to prevent it from being redirected to the settings.

**Note:** This mod is not designed to reveal hidden Control Panel applets; rather, its purpose is to restore applets that are currently present in the Control Panel but redirect to the Settings app.

### Example CustomApplets configuration:
```yaml
- "{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"  # System
- "Microsoft.Troubleshooting"              # Troubleshooting
- "BB06C0E4-D293-4f75-8A90-CB05B6477EEE"    # System (bare GUID, no braces)
```

**Before:**
![Before](https://raw.githubusercontent.com/AdmXP8/assets/main/Screen%20Recording%202026-08-27%20124304.gif)

**After:**
![After](https://raw.githubusercontent.com/AdmXP8/assets/main/Screen%20Recording%202026-08-27%20124458.gif)

**How it works:** the mod hooks two functions - `COpenControlPanel::_MapLegacyName` (scoped to a configurable list of applet IDs; every other legacy name resolves normally) and `CompareStringOrdinal` (only overrides a result that was genuinely "equal" for a targeted string; every other comparison in the process keeps its real result). It does **not** patch or modify any module's memory - earlier versions did, but testing showed the two hooks alone are sufficient, so the memory-patching code was removed entirely.

**A note on `CompareStringOrdinal` scoping:** we tried restricting the override to calls whose return address falls inside `shell32.dll` (and later, `shell32.dll` or `windows.storage.dll`), using `GetModuleHandleExW(..., GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, ...)` on the caller's return address. Both attempts broke the mod's actual functionality in testing, which means the real comparison this mod needs to influence isn't reliably reachable that way (most likely it happens through a COM/vtable call chain, or a helper whose return address doesn't resolve the way a direct call would). Given that, the hook is intentionally left unscoped by caller. The blast radius is still bounded in a few concrete ways: it never touches a comparison unless the real result was already `CSTR_EQUAL`; it requires an exact, full-length match against a small, specific set of applet-identifier strings (12 built-in + whatever the user adds in `CustomApplets`); and it never fires for a comparison that wasn't already reporting equality. We're open to a more surgical fix if a maintainer can point at the actual call site.

**Difference from `settings-to-control-panel`:** that mod also hooks `_MapLegacyName`, but its behavior differs for the applets this mod targets. For Troubleshooting, it launches `msdt.exe` directly instead of opening the applet itself; for Installed Updates and Default Programs, it has no mechanism at all to stop the redirect to Settings. This mod specifically restores the classic in-Control-Panel behavior for those items.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CustomApplets: [""]
  $name: Custom applet IDs
  $description: >-
    Add extra Control Panel applets to unhide, in addition to the 6 built-in
    ones. For GUID-based applets you can enter just the GUID, with or without
    braces (e.g. BB06C0E4-D293-4f75-8A90-CB05B6477EEE or
    {BB06C0E4-D293-4f75-8A90-CB05B6477EEE}) — the "::" prefix is added
    automatically. Canonical names (e.g. Microsoft.SomeApplet) must be at
    least 8 characters and contain a dot, same shape as Microsoft's own
    names — this is a safety floor, not a real validation of the name.
    One entry per row. IMPORTANT: an empty row ends the list — any rows
    after a blank one are ignored, so don't leave gaps in the middle. The
    mod reloads automatically after saving.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <exception>

// These applets exist but they redirect to the modern Settings app on 23H2+.
// constexpr std::wstring_view (rather than LPCWSTR) so .size() is computed
// once at compile time instead of via wcslen() on every single
// CompareStringOrdinal call in the process (see MatchesTargetList below).
constexpr std::wstring_view g_szAppletsToUnhide[] = {
    L"::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}", // System
    L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}", // Devices and Printers
    L"::{D450A8A1-9568-45C7-9C0E-B4F9FB4537BD}", // Installed Updates
    L"::{17CD9488-1228-4B2F-88CE-4298E93E0966}", // Default Programs
    L"::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}", // Troubleshooting
    L"::{BD84B380-8CA2-1069-AB1D-08000948F534}", // Fonts
};

constexpr std::wstring_view g_szCanonicalNames[] = {
    L"Microsoft.Troubleshooting",
    L"Microsoft.DevicesAndPrinters",
    L"Microsoft.System",
    L"Microsoft.InstalledUpdates",
    L"Microsoft.DefaultPrograms",
    L"Microsoft.Fonts"
};

bool g_isInitialized = false;

// User-provided applet GUIDs / canonical names, loaded from the "CustomApplets"
// array setting, in addition to the 6 built-in ones above.
std::vector<std::wstring> g_customApplets;

static std::wstring Trim(const std::wstring& s) {
    size_t start = s.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";
    size_t end = s.find_last_not_of(L" \t\r\n");
    return s.substr(start, end - start + 1);
}

// A "bare" GUID with no braces, e.g. BB06C0E4-D293-4f75-8A90-CB05B6477EEE
// (8-4-4-4-12 hex digits separated by dashes).
static bool LooksLikeBareGuid(const std::wstring& s) {
    if (s.size() != 36) return false;
    for (size_t i = 0; i < s.size(); i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (s[i] != L'-') return false;
        } else if (!iswxdigit(s[i])) {
            return false;
        }
    }
    return true;
}

// Lets the user type just the GUID (with or without braces), instead of
// requiring the "::{...}" CLSID-path syntax Explorer actually expects.
// Canonical names (e.g. "Microsoft.System") are left untouched, since those
// never use the "::" prefix to begin with.
static std::wstring NormalizeAppletId(const std::wstring& rawInput) {
    std::wstring raw = Trim(rawInput);
    if (raw.empty()) return raw;

    if (raw.rfind(L"::", 0) == 0) {
        return raw; // already has the "::" prefix, use as-is
    }
    if (raw.front() == L'{' && raw.back() == L'}') {
        return L"::" + raw; // bare "{GUID}" -> add the "::" prefix
    }
    if (LooksLikeBareGuid(raw)) {
        return L"::{" + raw + L"}"; // bare GUID, no braces -> add both
    }
    return raw; // assume canonical name (e.g. "Microsoft.System")
}

// A generous sanity cap - no memory-patch buffer to size against anymore
// (memory patching was removed after testing showed it wasn't needed), this
// just guards against absurd/garbage input reaching the hooks' comparisons.
//
// FIX: this used to also require >= 8 characters, a dot, and
// letters/digits/dots only for non-GUID entries. That rejected short bare
// legacy names Control Panel actually uses (e.g. "system", "fonts") and
// anything containing '-' or '_' - which meant a user could never add the
// bare spelling via CustomApplets even if it was exactly what was needed.
// Since the CompareStringOrdinal hook only ever fires on a genuine exact
// match now (see MatchesTargetList), a short/garbage entry can't do harm -
// it just never matches anything real.
constexpr size_t kMaxAppletIdLength = 128;

static bool IsPlausibleAppletId(const std::wstring& id) {
    return !id.empty() && id.size() <= kMaxAppletIdLength;
}

// Reads the CustomApplets[i] setting entries until an empty one is hit.
// NOTE: Wh_GetStringSetting returns "" (never NULL) past the end of a
// configured array, and there is no other way to ask the API "how many
// entries are there" - so an empty row is unavoidably treated as the end of
// the list rather than a skippable gap (this is stated in the setting's
// $description above).
void LoadCustomAppletSettings() {
    g_customApplets.clear();

    for (int i = 0; ; i++) {
        PCWSTR value = Wh_GetStringSetting(L"CustomApplets[%d]", i);
        bool hasValue = *value != L'\0';
        if (hasValue) {
            std::wstring normalized = NormalizeAppletId(value);
            if (!normalized.empty()) {
                if (IsPlausibleAppletId(normalized)) {
                    Wh_Log(L"Custom applet entry #%d: \"%s\" -> \"%s\"", i, value, normalized.c_str());
                    g_customApplets.push_back(std::move(normalized));
                } else {
                    Wh_Log(L"Ignoring custom applet entry #%d (\"%s\"): doesn't look like a valid applet ID/GUID, is too long, or is too short", i, value);
                }
            } else {
                Wh_Log(L"Ignoring empty custom applet entry #%d", i);
            }
        }
        Wh_FreeStringSetting(value);
        if (!hasValue) break;
    }

    Wh_Log(L"Loaded %zu custom applet ID(s) from settings", g_customApplets.size());
}

// Build Gate: Ensures it only runs on Windows 11 23H2 (Build 22631) or newer,
// matching @description.
bool IsSupportedWindowsVersion() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return false;

    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
    if (!pRtlGetVersion) return false;

    RTL_OSVERSIONINFOW osvi = { sizeof(osvi) };
    if (pRtlGetVersion(&osvi) == 0) {
        if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.dwBuildNumber >= 22631) {
            return true;
        }
    }
    return false;
}

// Hooks
bool (*COpenControlPanel__MapLegacyName_orig)(void *, LPCWSTR, LPWSTR, UINT, bool *) = nullptr;

// Only suppresses the mapping for names actually in our target list;
// everything else falls through to the real implementation. This keeps
// legacy->canonical name resolution intact for every Control Panel item
// this mod doesn't care about, and avoids clobbering whatever
// settings-to-control-panel's own whitelist is doing if both mods are
// enabled together.
bool COpenControlPanel__MapLegacyName_hook(void *pThis, LPCWSTR pszLegacyName, LPWSTR pszNewName, UINT uUnused, bool *nameChanged) {
    // Same reasoning as CompareStringOrdinal_hook: never let a C++
    // exception escape into shell32's non-C++ call frames.
    try {
        bool isTargeted = false;

        // FIX: was using exact (case-sensitive) comparison here
        // (wstring_view::compare / operator==), which meant a differently-
        // cased spelling of a target string would silently never match -
        // notably two of the built-in GUIDs used to be stored in lowercase
        // while Windows' own StringFromGUID2 always generates uppercase
        // hex. CLSID strings and canonical names are case-insensitive in
        // Windows, so this now uses _wcsicmp for all three lists.
        if (pszLegacyName) {
            for (const auto& applet : g_szAppletsToUnhide) {
                if (applet.size() == wcslen(pszLegacyName) &&
                    _wcsnicmp(applet.data(), pszLegacyName, applet.size()) == 0) { isTargeted = true; break; }
            }
            if (!isTargeted) {
                for (const auto& name : g_szCanonicalNames) {
                    if (name.size() == wcslen(pszLegacyName) &&
                        _wcsnicmp(name.data(), pszLegacyName, name.size()) == 0) { isTargeted = true; break; }
                }
            }
            if (!isTargeted) {
                for (const auto& entry : g_customApplets) {
                    if (entry.size() == wcslen(pszLegacyName) &&
                        _wcsnicmp(entry.c_str(), pszLegacyName, entry.size()) == 0) { isTargeted = true; break; }
                }
            }
        }

        if (isTargeted) {
            if (nameChanged) *nameChanged = false;
            if (pszNewName && uUnused > 0) *pszNewName = L'\0';
            return false;
        }

        // TEMPORARY DIAGNOSTIC (remove once the bare legacy-name spellings
        // are known): settings-to-control-panel's whitelist covers BOTH the
        // canonical form (e.g. "Microsoft.System") and a bare legacy
        // keyword (e.g. "system") for each entry, because Control Panel can
        // hand either one to this function. This mod currently only checks
        // the Microsoft.* / ::{GUID} forms above, so any bare-keyword call
        // for one of our 6 target applets silently falls through here
        // instead of being caught. Open each target applet once with this
        // build installed, then check Wh_Log for the exact strings Windows
        // sent - add whichever ones matter to CustomApplets (now unrestricted
        // in length/shape - see IsPlausibleAppletId) or to the built-in list.
        if (pszLegacyName) {
            Wh_Log(L"_MapLegacyName: unmatched legacy name reached the hook: \"%s\"", pszLegacyName);
        }
    } catch (...) {
        Wh_Log(L"COpenControlPanel__MapLegacyName_hook: caught an exception, falling back to the original implementation");
    }

    if (COpenControlPanel__MapLegacyName_orig) {
        return COpenControlPanel__MapLegacyName_orig(pThis, pszLegacyName, pszNewName, uUnused, nameChanged);
    }
    return false;
}

using CompareStringOrdinal_t = decltype(&CompareStringOrdinal);
CompareStringOrdinal_t CompareStringOrdinal_orig = nullptr;

// Safe string comparison helper that handles -1 (null-terminated) lengths.
static int SafeCompareString(LPCWCH str1, int cch1, LPCWCH str2, int cch2, BOOL bIgnoreCase) {
    if (!str1 || !str2) return CSTR_LESS_THAN;

    int len1 = (cch1 == -1) ? (int)wcslen(str1) : cch1;
    int len2 = (cch2 == -1) ? (int)wcslen(str2) : cch2;

    if (len1 <= 0 || len2 <= 0) return CSTR_LESS_THAN;

    int minLen = (std::min)(len1, len2);
    if (bIgnoreCase) {
        return _wcsnicmp(str1, str2, minLen);
    } else {
        return wcsncmp(str1, str2, minLen);
    }
}

// Returns the string's length as CompareStringOrdinal itself would interpret it:
// a non-negative cch is used as-is; -1 means "null terminated, compute the length".
static int GetEffectiveLength(LPCWCH str, int cch) {
    return (cch == -1) ? (int)wcslen(str) : cch;
}

// Compares using the *known* length from cchCount1/cchCount2 instead of
// wcscmp/_wcsicmp, which would assume the input was null-terminated (it
// might not be - CompareStringOrdinal callers can pass substrings).
// Target-list lengths are precomputed constexpr std::wstring_view sizes,
// not recomputed via wcslen on every call.
//
// FIX: always compares case-insensitively, regardless of the caller's
// bIgnoreCase. This function is only ever reached after the real
// CompareStringOrdinal already reported the two input strings as equal (see
// CompareStringOrdinal_hook below) - so matching that already-equal value
// against our target list case-insensitively can only make a real match
// MORE likely to be recognized, never incorrectly less strict. This also
// protects against any casing difference between how Windows happens to
// generate a GUID string internally (StringFromGUID2 always produces
// uppercase hex) and however a user typed a CustomApplets entry.
static bool MatchesTargetList(LPCWCH str, int cch, BOOL /*bIgnoreCase*/) {
    if (!str) return false;
    int len = GetEffectiveLength(str, cch);
    if (len <= 0) return false;

    auto checkList = [&](const std::wstring_view* list, size_t count) -> bool {
        for (size_t i = 0; i < count; i++) {
            if ((size_t)len != list[i].size()) continue; // length mismatch: can't be this entry
            int cmp = SafeCompareString(str, cch, list[i].data(), (int)list[i].size(), /*bIgnoreCase=*/TRUE);
            if (cmp == 0) return true;
        }
        return false;
    };

    if (checkList(g_szAppletsToUnhide, ARRAYSIZE(g_szAppletsToUnhide))) return true;
    if (checkList(g_szCanonicalNames, ARRAYSIZE(g_szCanonicalNames))) return true;

    for (const auto& entry : g_customApplets) {
        if ((size_t)len != entry.size()) continue;
        int cmp = SafeCompareString(str, cch, entry.c_str(), (int)entry.size(), /*bIgnoreCase=*/TRUE);
        if (cmp == 0) return true;
    }

    return false;
}

// Always computes the REAL result first via the original function, and only
// overrides it when that real result was CSTR_EQUAL (i.e. only turns a
// "these are equal" answer into "not equal" for our target strings). Every
// non-equal comparison anywhere in the process keeps its true, correct
// ordering - this does not fully fix cmp(A,A) for a target string A (it
// still won't report CSTR_EQUAL for itself), but it avoids corrupting
// comparisons between unrelated strings.
//
// NOTE: caller-module scoping was attempted here (restricting the override
// to calls returning into shell32.dll, then shell32.dll-or-windows.storage.dll)
// and broke the mod's actual functionality both times - see the README's
// "note on CompareStringOrdinal scoping" for details. Reverted intentionally.
int WINAPI CompareStringOrdinal_hook(LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, BOOL bIgnoreCase) {
    if (!CompareStringOrdinal_orig) return 0;

    // A hook function is called from non-C++ trampolines (kernelbase's own
    // dispatch code). If a C++ exception (e.g. std::bad_alloc from a
    // std::vector/std::wstring operation) escaped this function
    // uncaught, it would unwind into stack frames that don't know how to
    // handle it - almost certainly crashing the host process outright.
    // Catch anything here and fall back to the real, unmodified comparison.
    try {
        int result = CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);

        if (result == CSTR_EQUAL && lpString1 && lpString2 &&
            (MatchesTargetList(lpString1, cchCount1, bIgnoreCase) ||
             MatchesTargetList(lpString2, cchCount2, bIgnoreCase))) {
            // TEMPORARY DIAGNOSTIC (log-only, doesn't affect behavior):
            // earlier attempts to scope this override to a specific caller
            // module used only the immediate return address
            // (__builtin_return_address(0)), which can land in a hook
            // trampoline/thunk rather than the real caller - and both
            // attempts were made before the case-sensitivity bug above was
            // found, so a failed match there could just as easily explain
            // why scoping "didn't work". This walks a few stack frames and
            // logs module+offset for each, so a real caller-scoping attempt
            // can be re-evaluated with actual data instead of another guess.
            void* frames[6] = {};
            USHORT frameCount = RtlCaptureStackBackTrace(1, ARRAYSIZE(frames), frames, nullptr);
            for (USHORT i = 0; i < frameCount; i++) {
                HMODULE frameModule = nullptr;
                if (GetModuleHandleExW(
                        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                        (PCWSTR)frames[i], &frameModule) && frameModule) {
                    wchar_t modulePath[MAX_PATH];
                    GetModuleFileNameW(frameModule, modulePath, ARRAYSIZE(modulePath));
                    Wh_Log(L"CompareStringOrdinal match, frame %u: %s+0x%zX", i, modulePath,
                           (size_t)frames[i] - (size_t)frameModule);
                }
            }

            return CSTR_LESS_THAN; // Force "not equal" to prevent redirect
        }

        return result;
    } catch (...) {
        Wh_Log(L"CompareStringOrdinal_hook: caught an exception, falling back to the real comparison result");
        return CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
    }
}

// Resolved against shell32.dll - see ApplyShell32Hooks below.
const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        { L"private: bool __cdecl COpenControlPanel::_MapLegacyName(unsigned short const *,unsigned short *,unsigned int,bool *)" },
        (void**)&COpenControlPanel__MapLegacyName_orig,
        (void*)COpenControlPanel__MapLegacyName_hook,
        true
    }
};

// control.exe/explorer.exe aren't guaranteed to have shell32.dll loaded
// yet at Wh_ModInit time in every scenario. Fix: hook LoadLibraryExW in
// kernelbase.dll and install the shell32 hook the moment shell32.dll
// actually gets loaded, whenever that happens - at Wh_ModInit time or later.
//
// g_shell32HookApplied is claimed atomically BEFORE any work happens (not
// after), so concurrent calls from different threads can't race each other
// into doing the work twice.
volatile LONG g_shell32HookApplied = 0;

// isLateLoad: true when called from the LoadLibraryExW hook (shell32.dll
// just finished loading after Wh_ModInit already returned), false when
// called directly from Wh_ModInit.
//   - Wh_ApplyHookOperations() must never be called before Wh_ModInit
//     returns (documented API requirement) - only the late path calls it.
void ApplyShell32Hooks(bool isLateLoad) {
    if (InterlockedCompareExchange(&g_shell32HookApplied, 1, 0) != 0) {
        return; // already applied, or another call already claimed this
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        InterlockedExchange(&g_shell32HookApplied, 0); // not actually loaded yet; allow a real attempt later
        return;
    }

    // Windhawk resolves this symbol automatically via Microsoft's public
    // symbol server (through the DIA SDK) and caches the PDB - no manual
    // symbol download is needed. What CAN fail is the symbol itself no
    // longer existing/matching on a future Windows build, since this is a
    // private, unexported function. If that happens, we log it clearly
    // instead of silently doing nothing.
    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to resolve/hook COpenControlPanel::_MapLegacyName - "
               L"this function's signature may have changed in this Windows build.");
    }

    if (isLateLoad) {
        // Required here because this hook was installed outside
        // Wh_ModInit's normal batch. Must NOT be called from the direct
        // (Wh_ModInit) path - Windhawk applies that batch itself when
        // Wh_ModInit returns.
        Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_orig = nullptr;

HMODULE WINAPI LoadLibraryExW_hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE result = LoadLibraryExW_orig(lpLibFileName, hFile, dwFlags);

    try {
        if (result && !g_shell32HookApplied) {
            // Resource-only loads don't map the module the normal way (no
            // import resolution, no DllMain) - not a meaningful "shell32 is
            // now usable" signal, so skip them rather than act on them.
            constexpr DWORD kResourceOnlyFlags = LOAD_LIBRARY_AS_DATAFILE |
                                                  LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                                  LOAD_LIBRARY_AS_IMAGE_RESOURCE;
            if ((dwFlags & kResourceOnlyFlags) == 0) {
                // Compare the returned handle against shell32.dll's actual
                // base address instead of matching the requested file name
                // string: shell32 can arrive as a static dependency of some
                // other DLL loaded through this same API, in which case
                // lpLibFileName would be that other DLL's name/path, not
                // "shell32.dll" - a name-based check would miss that case
                // entirely.
                if (result == GetModuleHandleW(L"shell32.dll")) {
                    ApplyShell32Hooks(/*isLateLoad=*/true);
                }
            }
        }
    } catch (...) {
        // Never let an exception escape into the loader's call frames - the
        // real LoadLibraryExW result is still returned either way below.
        Wh_Log(L"LoadLibraryExW_hook: caught an exception while checking for shell32.dll");
    }

    return result;
}

BOOL Wh_ModInit(void) {
    if (!IsSupportedWindowsVersion()) {
        Wh_Log(L"Control Panel Revival: Unsupported Windows build. Mod bypassed.");
        return FALSE;
    }

    // Wraps all of initialization: this runs before the host process has
    // fully started, and a stray uncaught exception here would take
    // explorer.exe/control.exe down with it. Fail the mod's
    // own init cleanly instead - the process itself keeps running normally,
    // just without this mod's changes.
    try {
        Wh_Log(L"Initializing v%s", WH_MOD_VERSION);

        LoadCustomAppletSettings();

        HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (hKernelBase) {
            auto pCompareStringOrdinal = (CompareStringOrdinal_t)GetProcAddress(hKernelBase, "CompareStringOrdinal");
            if (pCompareStringOrdinal) {
                if (!WindhawkUtils::SetFunctionHook(
                        (void *)pCompareStringOrdinal,
                        (void *)CompareStringOrdinal_hook,
                        (void **)&CompareStringOrdinal_orig)) {
                    Wh_Log(L"Failed to hook CompareStringOrdinal; the configured applets may not be unhidden in this process");
                } else {
                    Wh_Log(L"Successfully hooked CompareStringOrdinal");
                }
            } else {
                Wh_Log(L"CompareStringOrdinal not found in kernelbase.dll");
            }

            // Covers a process where shell32.dll hasn't been loaded yet at
            // this point. If it's already loaded (the common case), this
            // hook simply won't fire and ApplyShell32Hooks() below handles
            // that case directly.
            auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(hKernelBase, "LoadLibraryExW");
            if (pLoadLibraryExW) {
                if (!WindhawkUtils::SetFunctionHook(
                        (void *)pLoadLibraryExW,
                        (void *)LoadLibraryExW_hook,
                        (void **)&LoadLibraryExW_orig)) {
                    Wh_Log(L"Failed to hook LoadLibraryExW; a late-loaded shell32.dll in this process won't be patched");
                } else {
                    Wh_Log(L"Successfully hooked LoadLibraryExW");
                }
            } else {
                Wh_Log(L"LoadLibraryExW not found in kernelbase.dll");
            }
        }

        g_isInitialized = true;

        // Handles the common case: shell32.dll is already loaded. If it
        // isn't loaded yet in this process, this is a no-op for now and
        // LoadLibraryExW_hook picks it up later.
        ApplyShell32Hooks(/*isLateLoad=*/false);

        return TRUE;
    } catch (const std::exception& e) {
        Wh_Log(L"Wh_ModInit: caught an exception during initialization: %S", e.what());
        return FALSE;
    } catch (...) {
        Wh_Log(L"Wh_ModInit: caught an unknown exception during initialization");
        return FALSE;
    }
}

void Wh_ModUninit(void) {
    Wh_Log(L"Uninitializing Control Panel Revival");

    // Nothing to restore: this mod no longer modifies any module's memory,
    // only installs function hooks, which Windhawk removes on its own.
    g_isInitialized = false;
    InterlockedExchange(&g_shell32HookApplied, 0);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed, reloading to re-apply applet targeting");
    *bReload = TRUE;
    return TRUE;
}
