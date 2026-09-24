// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival
// @description     Prevents Control Panel applets from redirecting to the modern Settings app on Windows 11 23H2+ by unhiding legacy elements safely
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
to protect them from being redirected. Each of the six built-in applets can
also be individually toggled off in the settings if you don't want this mod
to affect it.

**Note:** This mod does not reveal hidden Control Panel applets. It only
restores applets that are already present in Control Panel but currently
redirect to Settings(e.g troubleshooting,default programs,installed updates and more).

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

**Difference from other mods** Other mods hook this same function but within a new window, so they cannot intercept new applet redirects—such as those for troubleshooting, installed updates, default programs, and so on.

**Relationship to `settings-to-control-panel` ("Redirect Settings to Control Panel"):** that mod also hooks the same private `COpenControlPanel::_MapLegacyName` function, but opens its targeted applets in a **new, separate window** rather than preventing the redirect entirely. This mod loads the applet **in-place, inside the existing Control Panel window**, which is what allows it to also cover Troubleshooting, Installed Updates, and Default Programs - three applets that, on 23H2+, can no longer be launched in a new window at all (not even via `shell:::`), so `settings-to-control-panel`'s mechanism cannot restore them. Both mods hooking the same private symbol is expected; if you run both together, only one mod's outcome will apply for any applet targeted by both.

**Known limitations**  Clicking on task links that lead to applets currently in the mod results in no action being taken. This issue will be fixed in future versions.

**If there are any bugs in the mod, please report them to me**

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- EnableSystem: true
  $name: System
  $description: >-
    Prevent the System applet (::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}) from
    redirecting to Settings.
- EnableDevicesAndPrinters: true
  $name: Devices and Printers
  $description: >-
    Prevent the Devices and Printers applet
    (::{A8A91A66-3A7D-4424-8D24-04E180695C7A}) from redirecting to Settings.
- EnableInstalledUpdates: true
  $name: Installed Updates
  $description: >-
    Prevent the Installed Updates applet
    (::{D450A8A1-9568-45C7-9C0E-B4F9FB4537BD}) from redirecting to Settings.
- EnableDefaultPrograms: true
  $name: Default Programs
  $description: >-
    Prevent the Default Programs applet
    (::{17CD9488-1228-4B2F-88CE-4298E93E0966}) from redirecting to Settings.
- EnableTroubleshooting: true
  $name: Troubleshooting
  $description: >-
    Prevent the Troubleshooting applet
    (::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}) from redirecting to Settings.
- EnableFonts: true
  $name: Fonts
  $description: >-
    Prevent the Fonts applet (::{BD84B380-8CA2-1069-AB1D-08000948F534}) from
    redirecting to Settings.
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

// Each built-in applet this mod can unhide, bundled with an `enabled` flag
// loaded from its own on/off setting (see LoadAppletToggleSettings below).
// Not constexpr anymore since `enabled` is only known at runtime, but the
// three identifier forms (GUID, canonical name, bare legacy keyword) are
// still fixed string literals.
struct BuiltInApplet {
    std::wstring_view guid;
    std::wstring_view canonicalName;
    std::wstring_view bareName;
    bool enabled;
};

BuiltInApplet g_builtInApplets[] = {
    { L"::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}", L"Microsoft.System",             L"system",             true }, // System
    { L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}", L"Microsoft.DevicesAndPrinters", L"devicesandprinters", true }, // Devices and Printers
    { L"::{D450A8A1-9568-45C7-9C0E-B4F9FB4537BD}", L"Microsoft.InstalledUpdates",   L"installedupdates",   true }, // Installed Updates
    { L"::{17CD9488-1228-4B2F-88CE-4298E93E0966}", L"Microsoft.DefaultPrograms",    L"defaultprograms",    true }, // Default Programs
    { L"::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}", L"Microsoft.Troubleshooting",    L"troubleshooting",    true }, // Troubleshooting
    { L"::{BD84B380-8CA2-1069-AB1D-08000948F534}", L"Microsoft.Fonts",              L"fonts",              true }, // Fonts
};

// Reads the six EnableXxx toggle settings and updates g_builtInApplets in
// place. A disabled applet is skipped entirely by both hooks below, exactly
// as if it had never been in the built-in list to begin with.
void LoadAppletToggleSettings() {
    g_builtInApplets[0].enabled = Wh_GetIntSetting(L"EnableSystem") != 0;
    g_builtInApplets[1].enabled = Wh_GetIntSetting(L"EnableDevicesAndPrinters") != 0;
    g_builtInApplets[2].enabled = Wh_GetIntSetting(L"EnableInstalledUpdates") != 0;
    g_builtInApplets[3].enabled = Wh_GetIntSetting(L"EnableDefaultPrograms") != 0;
    g_builtInApplets[4].enabled = Wh_GetIntSetting(L"EnableTroubleshooting") != 0;
    g_builtInApplets[5].enabled = Wh_GetIntSetting(L"EnableFonts") != 0;

    Wh_Log(L"Applet toggles: System=%d DevicesAndPrinters=%d InstalledUpdates=%d DefaultPrograms=%d Troubleshooting=%d Fonts=%d",
           g_builtInApplets[0].enabled, g_builtInApplets[1].enabled, g_builtInApplets[2].enabled,
           g_builtInApplets[3].enabled, g_builtInApplets[4].enabled, g_builtInApplets[5].enabled);
}

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
//
// Each built-in applet is skipped entirely if its EnableXxx setting is off
// (see g_builtInApplets/LoadAppletToggleSettings) - a disabled applet's
// GUID/canonical-name/bare-keyword forms are treated as if they weren't in
// the list at all, so its real (redirecting) behavior is left untouched.
bool COpenControlPanel__MapLegacyName_hook(void *pThis, LPCWSTR pszLegacyName, LPWSTR pszNewName, UINT uUnused, bool *nameChanged) {
    try {
        bool isTargeted = false;

        if (pszLegacyName) {
            size_t legacyLen = wcslen(pszLegacyName);

            for (const auto& applet : g_builtInApplets) {
                if (!applet.enabled) continue;
                if (NameMatches(applet.guid, pszLegacyName, legacyLen) ||
                    NameMatches(applet.canonicalName, pszLegacyName, legacyLen) ||
                    NameMatches(applet.bareName, pszLegacyName, legacyLen)) {
                    isTargeted = true;
                    break;
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
// keywords), skipping any applet whose EnableXxx setting is off - and
// deliberately excludes g_customApplets, same as before. This is the list
// CompareStringOrdinal_hook is allowed to influence; user-supplied strings
// are only ever matched against _MapLegacyName's legacy-name parameter,
// never against this process-wide comparison primitive.
static bool MatchesBuiltInList(LPCWCH str, int cch) {
    if (!str) return false;
    int len = GetEffectiveLength(str, cch);
    if (len <= 0) return false;

    auto matchesOne = [&](const std::wstring_view& candidate) -> bool {
        return (size_t)len == candidate.size() && _wcsnicmp(str, candidate.data(), candidate.size()) == 0;
    };

    for (const auto& applet : g_builtInApplets) {
        if (!applet.enabled) continue;
        if (matchesOne(applet.guid) || matchesOne(applet.canonicalName) || matchesOne(applet.bareName)) {
            return true;
        }
    }
    return false;
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

// FIX (regressed from an earlier round, restored here): checks whether
// shell32.dll is actually loaded BEFORE claiming the flag, not after. The
// previous ordering (claim first, then check, then reset on failure) had a
// real race: thread A could claim the flag during an unrelated DLL load,
// find shell32 not loaded yet, and reset the flag back to 0 - while thread
// B, which loaded shell32.dll concurrently, had already failed the
// compare-exchange (because A's claim was still in effect at that instant)
// and returned early. Neither thread would end up installing the hook, and
// nothing would retry until some other DLL happened to load afterward.
// Checking first and only claiming once shell32 is confirmed loaded closes
// that window.
void ApplyShell32Hooks(bool isLateLoad) {
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        return; // not loaded yet; a later LoadLibraryExW call will retry
    }

    if (InterlockedCompareExchange(&g_shell32HookApplied, 1, 0) != 0) {
        return; // already applied, or another call already claimed this
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

// FIX (regressed from an earlier round, restored here): no longer compares
// the returned HMODULE against GetModuleHandleW(L"shell32.dll"). That
// comparison misses exactly the case it was meant to catch: if shell32.dll
// arrives as a static dependency of some OTHER DLL loaded through this same
// API, `result` is that other DLL's handle, not shell32's, so the
// comparison is always false and the retry never fires. ApplyShell32Hooks()
// already does its own GetModuleHandleW(L"shell32.dll") check internally
// and safely no-ops if it isn't loaded yet, so it's simplest and correct to
// just call it after any successful, non-resource-only load and let it
// decide.
HMODULE WINAPI LoadLibraryExW_hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE result = LoadLibraryExW_orig(lpLibFileName, hFile, dwFlags);

    try {
        if (result && !g_shell32HookApplied) {
            constexpr DWORD kResourceOnlyFlags = LOAD_LIBRARY_AS_DATAFILE |
                                                  LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                                  LOAD_LIBRARY_AS_IMAGE_RESOURCE;
            if ((dwFlags & kResourceOnlyFlags) == 0) {
                ApplyShell32Hooks(/*isLateLoad=*/true);
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

        LoadAppletToggleSettings();
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
    InterlockedExchange(&g_shell32HookApplied, 0);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed, reloading to re-apply applet targeting");
    *bReload = TRUE;
    return TRUE;
}
