// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival
// @description     Prevents Control Panel applets from redirecting to the modern Settings app on Windows 11 23H2+ by unhiding legacy elements safely.
// @version         1.0.0
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

This mod restores Control Panel applets - Troubleshooting, Installed Updates,
Default Programs, Devices and Printers, Fonts, and System - that Windows 11
(23H2 and later) redirects to the Settings app instead of opening directly,
even when launched via a shell command. You can also add your own applet IDs
to protect them from being redirected.

**Note:** This mod does not reveal hidden Control Panel applets. It only
restores applets that are already present in Control Panel but currently
redirect to Settings.

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

**Difference from `settings-to-control-panel`:** that mod also hooks the
same underlying function, but doesn't stop the redirect for Installed
Updates or Default Programs, and opens `msdt.exe` directly for
Troubleshooting instead of the classic applet. This mod restores the
original in-Control-Panel behavior for all of those.

**Known limitation:** one of the two hooks this mod installs
(`CompareStringOrdinal`) is not scoped to a specific caller - see the code
comments for the technical detail and the scoping attempts that were tried.
In practice this only ever changes the outcome of a comparison that was
already reporting two specific applet-identifier strings as equal, so the
practical risk is low, but it isn't a hard guarantee.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CustomApplets: [""]
  $name: Custom applet IDs
  $description: >-
    Add extra Control Panel applets to unhide, in addition to the built-in
    ones. For GUID-based applets you can enter just the GUID, with or without
    braces (e.g. BB06C0E4-D293-4f75-8A90-CB05B6477EEE or
    {BB06C0E4-D293-4f75-8A90-CB05B6477EEE}) — the "::" prefix is added
    automatically. Canonical names (e.g. Microsoft.SomeApplet) or bare
    legacy keywords (e.g. system) can be entered as-is. One entry per row.
    IMPORTANT: an empty row ends the list — any rows after a blank one are
    ignored, so don't leave gaps in the middle. The mod reloads
    automatically after saving.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

#include <string>
#include <string_view>
#include <vector>
#include <cwctype>
#include <exception>

// These applets exist but they redirect to the modern Settings app on 23H2+.
// constexpr std::wstring_view (rather than LPCWSTR) so .size() is computed
// once at compile time instead of via wcslen() on every single
// CompareStringOrdinal call in the process (see MatchesBuiltInList below).
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

// Bare legacy keywords Control Panel can also hand to _MapLegacyName for
// these same six applets, alongside (or instead of) the canonical/GUID
// forms above.
constexpr std::wstring_view g_szBareLegacyNames[] = {
    L"troubleshooting",
    L"installedupdates",
    L"defaultprograms",
    L"devicesandprinters",
    L"fonts",
    L"system",
};

bool g_isInitialized = false;

// User-provided applet GUIDs / canonical names / bare keywords, loaded from
// the "CustomApplets" array setting. Only ever checked against
// _MapLegacyName's legacy-name parameter (see COpenControlPanel__MapLegacyName_hook)
// - deliberately NEVER checked in CompareStringOrdinal_hook. A non-matching
// _MapLegacyName call is harmless (it just falls through to the real
// implementation), but arbitrary user input feeding a process-wide string
// comparison override has no such safety net, so that path is restricted to
// the small, vetted built-in list only.
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
// Canonical names (e.g. "Microsoft.System") and bare legacy keywords (e.g.
// "system") are left untouched, since neither uses the "::" prefix.
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
    return raw; // assume canonical name or bare legacy keyword
}

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
                    Wh_Log(L"Ignoring custom applet entry #%d (\"%s\"): too long", i, value);
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

// Ensures the mod only runs on Windows 11 23H2 (build 22631) or newer,
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

static bool NameMatches(const std::wstring_view& candidate, LPCWSTR name, size_t nameLen) {
    return candidate.size() == nameLen && _wcsnicmp(candidate.data(), name, candidate.size()) == 0;
}

// Only suppresses the mapping for names actually in our target list;
// everything else falls through to the real implementation. This keeps
// legacy->canonical name resolution intact for every Control Panel item
// this mod doesn't care about, and avoids clobbering whatever
// settings-to-control-panel's own whitelist is doing if both mods are
// enabled together. All comparisons are case-insensitive.
bool COpenControlPanel__MapLegacyName_hook(void *pThis, LPCWSTR pszLegacyName, LPWSTR pszNewName, UINT uUnused, bool *nameChanged) {
    try {
        bool isTargeted = false;

        if (pszLegacyName) {
            size_t legacyLen = wcslen(pszLegacyName);

            for (const auto& applet : g_szAppletsToUnhide) {
                if (NameMatches(applet, pszLegacyName, legacyLen)) { isTargeted = true; break; }
            }
            if (!isTargeted) {
                for (const auto& name : g_szCanonicalNames) {
                    if (NameMatches(name, pszLegacyName, legacyLen)) { isTargeted = true; break; }
                }
            }
            if (!isTargeted) {
                for (const auto& name : g_szBareLegacyNames) {
                    if (NameMatches(name, pszLegacyName, legacyLen)) { isTargeted = true; break; }
                }
            }
            if (!isTargeted) {
                for (const auto& entry : g_customApplets) {
                    if (entry.size() == legacyLen && _wcsnicmp(entry.c_str(), pszLegacyName, entry.size()) == 0) { isTargeted = true; break; }
                }
            }
        }

        if (isTargeted) {
            if (nameChanged) *nameChanged = false;
            if (pszNewName && uUnused > 0) *pszNewName = L'\0';
            return false;
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

static int GetEffectiveLength(LPCWCH str, int cch) {
    return (cch == -1) ? (int)wcslen(str) : cch;
}

// Checks ONLY the built-in, vetted list (GUIDs + canonical names + bare
// keywords) - deliberately excludes g_customApplets. This is the list
// CompareStringOrdinal_hook is allowed to influence; user-supplied strings
// are only ever matched against _MapLegacyName's legacy-name parameter,
// never against this process-wide comparison primitive.
static bool MatchesBuiltInList(LPCWCH str, int cch) {
    if (!str) return false;
    int len = GetEffectiveLength(str, cch);
    if (len <= 0) return false;

    auto checkList = [&](const std::wstring_view* list, size_t count) -> bool {
        for (size_t i = 0; i < count; i++) {
            if ((size_t)len != list[i].size()) continue;
            if (_wcsnicmp(str, list[i].data(), list[i].size()) == 0) return true;
        }
        return false;
    };

    return checkList(g_szAppletsToUnhide, ARRAYSIZE(g_szAppletsToUnhide)) ||
           checkList(g_szCanonicalNames, ARRAYSIZE(g_szCanonicalNames)) ||
           checkList(g_szBareLegacyNames, ARRAYSIZE(g_szBareLegacyNames));
}

// FIX: only ever overrides a result that was genuinely CSTR_EQUAL for one
// of the built-in target strings - every non-equal comparison anywhere in
// the process keeps its true result. CustomApplets entries are deliberately
// excluded here (see g_customApplets above) - only the built-in, vetted
// list can trigger this override.
//
// This is intentionally NOT scoped to a specific caller. Four separate
// scoping attempts were made and each broke the mod's actual functionality
// in testing: (1) return-address matching against shell32.dll, (2) the
// same against shell32.dll-or-windows.storage.dll, (3) a multi-frame stack
// walk that empirically added explorerframe.dll to the accepted set, and
// (4) a thread-local flag armed for the duration of the
// COpenControlPanel::_MapLegacyName call on the same thread. That the
// temporal approach (4) also failed is itself informative: it suggests the
// actual redirect-decision comparison doesn't happen synchronously nested
// inside _MapLegacyName's call on the same thread at all - it may be
// deferred (a posted message handled later), happen on a different thread,
// or go through a call path that doesn't involve _MapLegacyName in the
// first place. Given that, the hook is left unscoped for now. We're open
// to a properly-scoped fix if the actual call site can be identified (e.g.
// via a debugger breakpoint on the unhooked CompareStringOrdinal, rather
// than detection from inside the hook).
int WINAPI CompareStringOrdinal_hook(LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, BOOL bIgnoreCase) {
    if (!CompareStringOrdinal_orig) return 0;

    try {
        int result = CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);

        if (result == CSTR_EQUAL && lpString1 && lpString2 &&
            (MatchesBuiltInList(lpString1, cchCount1) || MatchesBuiltInList(lpString2, cchCount2))) {
            return CSTR_LESS_THAN; // Force "not equal" to prevent redirect
        }

        return result;
    } catch (...) {
        Wh_Log(L"CompareStringOrdinal_hook: caught an exception, falling back to the real comparison result");
        return CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
    }
}

// Resolved against shell32.dll - see ApplyShell32Hooks below. Marked
// non-optional (the last field): this is the only hook this mod installs,
// so if the symbol can't be resolved on some future Windows build, the mod
// is entirely non-functional and that must be a reported failure, not a
// silently-accepted no-op.
const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        { L"private: bool __cdecl COpenControlPanel::_MapLegacyName(unsigned short const *,unsigned short *,unsigned int,bool *)" },
        (void**)&COpenControlPanel__MapLegacyName_orig,
        (void*)COpenControlPanel__MapLegacyName_hook,
        false
    }
};

// control.exe/explorer.exe aren't guaranteed to have shell32.dll loaded yet
// at Wh_ModInit time in every scenario. Fix: hook LoadLibraryExW in
// kernelbase.dll and install the shell32 hook the moment shell32.dll
// actually gets loaded, whenever that happens - at Wh_ModInit time or later.
volatile LONG g_shell32HookApplied = 0;

void ApplyShell32Hooks(bool isLateLoad) {
    if (InterlockedCompareExchange(&g_shell32HookApplied, 1, 0) != 0) {
        return; // already applied, or another call already claimed this
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        InterlockedExchange(&g_shell32HookApplied, 0); // not actually loaded yet; allow a real attempt later
        return;
    }

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to resolve/hook COpenControlPanel::_MapLegacyName - "
               L"this function's signature may have changed in this Windows build. "
               L"The mod will not be able to unhide any applets in this process.");
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
            constexpr DWORD kResourceOnlyFlags = LOAD_LIBRARY_AS_DATAFILE |
                                                  LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                                  LOAD_LIBRARY_AS_IMAGE_RESOURCE;
            if ((dwFlags & kResourceOnlyFlags) == 0) {
                if (result == GetModuleHandleW(L"shell32.dll")) {
                    ApplyShell32Hooks(/*isLateLoad=*/true);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"LoadLibraryExW_hook: caught an exception while checking for shell32.dll");
    }

    return result;
}

BOOL Wh_ModInit(void) {
    if (!IsSupportedWindowsVersion()) {
        Wh_Log(L"Control Panel Revival: Unsupported Windows build. Mod bypassed.");
        return FALSE;
    }

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
    g_isInitialized = false;
    InterlockedExchange(&g_shell32HookApplied, 0);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed, reloading to re-apply applet targeting");
    *bReload = TRUE;
    return TRUE;
}
