// ==WindhawkMod==
// @id              control-panel-revival-admxp8
// @name            Control Panel Revival
// @description     Prevents Control Panel applets from redirecting to the modern Settings app on Windows 11 23H2+ by unhiding legacy elements safely.
// @version         0.9.7
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @include         control.exe
// @include         rundll32.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lshlwapi
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Control Panel Revival

### What does this mod do?

This mod is designed to restore sections of the Control Panel—such as Troubleshooting, Installed Updates, Default Programs, and others—that are redirected to the Settings app in Windows 11 (version 23H2 and later) and can no longer be launched even via shell commands.
You can also add the ID of your desired applet to prevent it from being redirected to the settings.

**How it works:** the mod only hooks two functions - `COpenControlPanel::_MapLegacyName` (scoped to a configurable list of applet IDs; every other legacy name resolves normally) and `CompareStringOrdinal` (only overrides a result that was genuinely "equal" for a targeted string; every other comparison in the process keeps its real result). It does **not** patch or modify any module's memory - earlier versions did, but testing showed the two hooks alone are sufficient, so the memory-patching code was removed entirely.

**Difference from `settings-to-control-panel`:** that mod also hooks `_MapLegacyName`, but its behavior differs for the applets this mod targets. For Troubleshooting, it launches `msdt.exe` directly instead of opening the applet itself; for Installed Updates and Default Programs, it has no mechanism at all to stop the redirect to Settings. This mod specifically restores the classic in-Control-Panel behavior for those items.

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
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <cwctype>

// These applets exist but they redirect to the modern Settings app on 23H2+.
// constexpr std::wstring_view (rather than LPCWSTR) so .size() is computed
// once at compile time instead of via wcslen() on every single
// CompareStringOrdinal call in the process (see MatchesTargetList below).
constexpr std::wstring_view g_szAppletsToUnhide[] = {
    L"::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}", // System
    L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}", // Devices and Printers
    L"::{d450a8a1-9568-45c7-9c0e-b4f9fb4537bd}", // Installed Updates
    L"::{17cd9488-1228-4b2f-88ce-4298e93e0966}", // Default Programs
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
constexpr size_t kMaxAppletIdLength = 128;

// A canonical name shorter than this is unlikely to be a real Control Panel
// canonical name (all of Microsoft's known ones are well above this
// length), and requiring a dot matches the "Vendor.Item" shape every real
// canonical name has. This is a basic sanity floor, not real validation.
constexpr size_t kMinCanonicalNameLength = 8;

// Rejects anything that clearly isn't a plausible Control Panel applet
// identifier (garbage input, pasted text, anything absurdly long or short).
static bool IsPlausibleAppletId(const std::wstring& id) {
    if (id.empty() || id.size() > kMaxAppletIdLength) {
        return false;
    }

    if (id.rfind(L"::{", 0) == 0) {
        // Expect exactly "::{" + 36-char GUID + "}"
        if (id.size() != 3 + 36 + 1 || id.back() != L'}') {
            return false;
        }
        return LooksLikeBareGuid(id.substr(3, 36));
    }

    // Otherwise, treat it as a canonical name: must start with a letter,
    // contain only letters/digits/dots, contain at least one dot, and meet
    // the minimum length above.
    if (id.size() < kMinCanonicalNameLength || !iswalpha(id.front())) {
        return false;
    }
    bool hasDot = false;
    for (wchar_t c : id) {
        if (c == L'.') {
            hasDot = true;
        } else if (!iswalnum(c)) {
            return false;
        }
    }
    return hasDot;
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

// rundll32.exe hosts arbitrary DLLs and Windows spawns it constantly for
// completely unrelated work. Without this check, every such instance would
// still hook CompareStringOrdinal/LoadLibraryExW for nothing. Only bail out
// for rundll32.exe specifically, and only when its command line doesn't
// look like a Control Panel applet host - explorer.exe/control.exe are
// never affected by this check.
static bool ShouldBailOutForThisProcess() {
    wchar_t exePath[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    if (len == 0 || len >= ARRAYSIZE(exePath)) {
        return false; // couldn't determine the process name; don't risk bailing incorrectly
    }

    LPCWSTR exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;

    if (_wcsicmp(exeName, L"rundll32.exe") != 0) {
        return false; // not rundll32.exe - always proceed (explorer.exe / control.exe)
    }

    return StrStrIW(GetCommandLineW(), L"Control_RunDLL") == nullptr;
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
    bool isTargeted = false;

    if (pszLegacyName) {
        for (const auto& applet : g_szAppletsToUnhide) {
            if (applet.compare(pszLegacyName) == 0) { isTargeted = true; break; }
        }
        if (!isTargeted) {
            for (const auto& name : g_szCanonicalNames) {
                if (name.compare(pszLegacyName) == 0) { isTargeted = true; break; }
            }
        }
        if (!isTargeted) {
            for (const auto& entry : g_customApplets) {
                if (entry == pszLegacyName) { isTargeted = true; break; }
            }
        }
    }

    if (isTargeted) {
        if (nameChanged) *nameChanged = false;
        if (pszNewName && uUnused > 0) *pszNewName = L'\0';
        return false;
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
static bool MatchesTargetList(LPCWCH str, int cch, BOOL bIgnoreCase) {
    if (!str) return false;
    int len = GetEffectiveLength(str, cch);
    if (len <= 0) return false;

    auto checkList = [&](const std::wstring_view* list, size_t count) -> bool {
        for (size_t i = 0; i < count; i++) {
            if ((size_t)len != list[i].size()) continue; // length mismatch: can't be this entry
            int cmp = SafeCompareString(str, cch, list[i].data(), (int)list[i].size(), bIgnoreCase);
            if (cmp == 0) return true;
        }
        return false;
    };

    if (checkList(g_szAppletsToUnhide, ARRAYSIZE(g_szAppletsToUnhide))) return true;
    if (checkList(g_szCanonicalNames, ARRAYSIZE(g_szCanonicalNames))) return true;

    for (const auto& entry : g_customApplets) {
        if ((size_t)len != entry.size()) continue;
        int cmp = SafeCompareString(str, cch, entry.c_str(), (int)entry.size(), bIgnoreCase);
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
int WINAPI CompareStringOrdinal_hook(LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, BOOL bIgnoreCase) {
    if (!CompareStringOrdinal_orig) return 0;

    int result = CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);

    if (result == CSTR_EQUAL && lpString1 && lpString2 &&
        (MatchesTargetList(lpString1, cchCount1, bIgnoreCase) ||
         MatchesTargetList(lpString2, cchCount2, bIgnoreCase))) {
        return CSTR_LESS_THAN; // Force "not equal" to prevent redirect
    }

    return result;
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

// rundll32.exe does NOT statically import shell32.dll - it loads it later,
// at runtime, when asked to run "shell32.dll,Control_RunDLL". Wh_ModInit
// runs before that happens, so GetModuleHandleW(L"shell32.dll") returns
// NULL there and the _MapLegacyName hook would never get installed in
// exactly the host process that actually runs Control Panel applets.
//
// Fix: hook LoadLibraryExW in kernelbase.dll (internal callers go straight
// to kernelbase, not through the kernel32 import) and install the shell32
// hook the moment shell32.dll actually gets loaded, whenever that happens -
// at Wh_ModInit time (explorer.exe, control.exe) or later (rundll32.exe).
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

    if (result && !g_shell32HookApplied) {
        // Resource-only loads don't map the module the normal way (no
        // import resolution, no DllMain) - not a meaningful "shell32 is now
        // usable" signal, so skip them rather than act on them.
        constexpr DWORD kResourceOnlyFlags = LOAD_LIBRARY_AS_DATAFILE |
                                              LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                              LOAD_LIBRARY_AS_IMAGE_RESOURCE;
        if ((dwFlags & kResourceOnlyFlags) == 0) {
            // Compare the returned handle against shell32.dll's actual base
            // address instead of matching the requested file name string:
            // shell32 can arrive as a static dependency of some other DLL
            // loaded through this same API, in which case lpLibFileName
            // would be that other DLL's name/path, not "shell32.dll" - a
            // name-based check would miss that case entirely.
            if (result == GetModuleHandleW(L"shell32.dll")) {
                ApplyShell32Hooks(/*isLateLoad=*/true);
            }
        }
    }

    return result;
}

BOOL Wh_ModInit(void) {
    if (!IsSupportedWindowsVersion()) {
        Wh_Log(L"Control Panel Revival: Unsupported Windows build. Mod bypassed.");
        return FALSE;
    }

    if (ShouldBailOutForThisProcess()) {
        Wh_Log(L"This rundll32.exe instance isn't hosting a Control Panel applet; mod bypassed for this process.");
        return FALSE;
    }

    Wh_Log(L"Initializing Control Panel Revival v1.1.0");

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

        // Covers processes where shell32.dll hasn't been loaded yet at this
        // point (notably rundll32.exe). If it's already loaded
        // (explorer.exe, control.exe), this hook simply won't fire for it
        // and ApplyShell32Hooks() below handles that case directly.
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

    // Handles the common case: shell32.dll is already loaded (explorer.exe,
    // control.exe). If it isn't loaded yet in this process (rundll32.exe),
    // this is a no-op for now and LoadLibraryExW_hook picks it up later.
    ApplyShell32Hooks(/*isLateLoad=*/false);

    return TRUE;
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
