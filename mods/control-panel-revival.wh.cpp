// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival
// @description     Prevents Control Panel applets from redirecting to the modern Settings app on Windows 11 23H2+ by unhiding legacy elements safely.
// @version         0.8.7
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @include         control.exe
// @include         rundll32.exe
// @compilerOptions -lpsapi -lcomctl32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Control Panel Revival

### what does this mod do?

This mod is designed to restore sections of the Control Panel—such as Troubleshooting, Installed Updates, Default Programs, and others—that are redirected to the Settings app in Windows 11 (version 23H2 and later) and can no longer be launched even via shell commands.
You can also add the ID of your desired applet to prevent it from being redirected to the settings.

**Suggestion** To make it top-notch, I suggest you also try the "Windows 7 Applet Restorer" mod

**Note:** This mod is not designed to reveal hidden Control Panel applets; rather, its purpose is to restore applets that are currently present in the Control Panel but redirect to the Settings app.

**Requirements:** **Explorer Patcher install** (Windows 11 modern updates aggressively redirect legacy control panel applets at the system level. While this mod hooks string comparisons and navigation routines in `shell32.dll` and `windows.storage.dll`, certain UI entry points rely on ExplorerPatcher to properly render and handle legacy Control Panel calls without triggering the forced Settings app redirect.)


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
    automatically. Canonical names (e.g. Microsoft.SomeApplet) can be entered
    as-is. One entry per row. Leave a row blank to ignore it. The mod reloads
    automatically after saving.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <psapi.h>
#include <windhawk_utils.h>

#include <string>
#include <vector>
#include <cwctype>

// These applets exist but they redirect to the modern Settings app on 23H2+
LPCWSTR g_szAppletsToUnhide[] = {
    L"::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}", // System
    L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}", // Devices and Printers
    L"::{d450a8a1-9568-45c7-9c0e-b4f9fb4537bd}", // Installed Updates
    L"::{17cd9488-1228-4b2f-88ce-4298e93e0966}", // Default Programs
    L"::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}", // Troubleshooting
    L"::{BD84B380-8CA2-1069-AB1D-08000948F534}", // Fonts
};

LPCWSTR g_szCanonicalNames[] = {
    L"Microsoft.Troubleshooting",
    L"Microsoft.DevicesAndPrinters",
    L"Microsoft.System",
    L"Microsoft.InstalledUpdates",
    L"Microsoft.DefaultPrograms",
    L"Microsoft.Fonts"
};

// Structure to save original bytes for safe restoration
struct PatchRecord {
    void* address;
    BYTE originalBytes[128];
    size_t length;
};

PatchRecord g_appliedPatches[64];
size_t g_appliedPatchCount = 0;
HMODULE g_hWinStorage = nullptr;
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

// The memory-patch buffer only holds up to 64 wide chars per entry
// (PatchRecord::originalBytes is 128 bytes == 64 * sizeof(WCHAR)), so
// anything longer can never be safely patched.
constexpr size_t kMaxAppletIdLength = 64;

// Rejects anything that clearly isn't a plausible Control Panel applet
// identifier (garbage input, pasted text, anything too long), so it never
// reaches the memory-scanning/patching code at all.
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

    // Otherwise, treat it as a canonical name: must start with a letter and
    // contain only letters, digits, and dots (e.g. "Microsoft.System").
    if (!iswalpha(id.front())) {
        return false;
    }
    for (wchar_t c : id) {
        if (!iswalnum(c) && c != L'.') {
            return false;
        }
    }
    return true;
}

// Safety ceiling for how many custom applets we'll track. The 6 built-in
// applets already reserve up to 12 slots in the fixed-size g_appliedPatches
// buffer (each is patched in up to 2 modules), leaving comfortable headroom
// for this many custom entries (each also patched in up to 2 modules).
constexpr size_t kMaxCustomApplets = 20;

// Shows a MessageBox on a separate thread so Wh_ModInit (and therefore the
// host process's startup, e.g. explorer.exe/rundll32.exe) never blocks
// waiting for the user to dismiss it.
DWORD WINAPI ShowWarningMessageBoxThreadProc(LPVOID param) {
    wchar_t* text = static_cast<wchar_t*>(param);
    MessageBoxW(nullptr, text, L"Control Panel Revival", MB_OK | MB_ICONWARNING);
    delete[] text;
    return 0;
}

void ShowWarningMessageBoxAsync(const wchar_t* text) {
    size_t len = wcslen(text) + 1;
    wchar_t* copy = new wchar_t[len];
    wcscpy_s(copy, len, text);

    HANDLE hThread = CreateThread(nullptr, 0, ShowWarningMessageBoxThreadProc, copy, 0, nullptr);
    if (hThread) {
        CloseHandle(hThread); // detach; the thread cleans up its own param
    } else {
        Wh_Log(L"Failed to create thread for warning message box");
        delete[] copy;
    }
}

// Reads the CustomApplets[i] setting entries until an empty one is hit
// (Wh_GetStringSetting returns "" past the end of the array, never null).
void LoadCustomAppletSettings() {
    g_customApplets.clear();

    for (int i = 0; ; i++) {
        PCWSTR value = Wh_GetStringSetting(L"CustomApplets[%d]", i);
        bool hasValue = value && *value;
        if (hasValue) {
            std::wstring normalized = NormalizeAppletId(value);
            if (!normalized.empty()) {
                if (IsPlausibleAppletId(normalized)) {
                    Wh_Log(L"Custom applet entry #%d: \"%s\" -> \"%s\"", i, value, normalized.c_str());
                    g_customApplets.push_back(std::move(normalized));
                } else {
                    Wh_Log(L"Ignoring custom applet entry #%d (\"%s\"): doesn't look like a valid applet ID/GUID, or is too long", i, value);
                }
            }
        }
        Wh_FreeStringSetting(value);
        if (!hasValue) break;
    }

    if (g_customApplets.size() > kMaxCustomApplets) {
        size_t ignoredCount = g_customApplets.size() - kMaxCustomApplets;
        g_customApplets.resize(kMaxCustomApplets);

        Wh_Log(L"Too many custom applets configured; keeping the first %zu and ignoring %zu", kMaxCustomApplets, ignoredCount);

        wchar_t message[256];
        swprintf_s(
            message,
            L"You've added too many custom applet IDs (limit: %zu).\n\n"
            L"Only the first %zu will be applied; the remaining %zu will be ignored.\n"
            L"Please remove some entries from the mod's settings.",
            kMaxCustomApplets, kMaxCustomApplets, ignoredCount
        );
        ShowWarningMessageBoxAsync(message);
    }

    Wh_Log(L"Loaded %zu custom applet ID(s) from settings", g_customApplets.size());
}

// 1. Build Gate: Ensures it only runs on Windows 11 23H2 (Build 22631) or newer
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

// 2. Reversible Memory Patching
//    FIX: no longer stops at the first match (return;) — keeps scanning so every
//    occurrence of the string table entry in the module gets neutralized.
//    FIX: guards against integer underflow when patternLen > size.
//    SAFETY TRIP: before reading or writing ANY byte, this verifies (via
//    VirtualQuery) that the containing memory region is committed, readable,
//    and not a guard page. If a region fails that check, the scan skips
//    straight past it without ever touching it - the memory equivalent of a
//    table saw's sensor retracting the blade the instant it senses skin,
//    rather than finding out the hard way. This can't predict every possible
//    crash, but it does eliminate the concrete risk of reading/writing into
//    unmapped or protected memory, which is the realistic failure mode here.
void KillStringInModuleReversible(HMODULE hModule, LPCWSTR lpSearch) {
    if (!hModule || !lpSearch) return;

    MODULEINFO info = { 0 };
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &info, sizeof(MODULEINFO))) return;

    DWORD_PTR base = (DWORD_PTR)info.lpBaseOfDll;
    size_t size = (size_t)info.SizeOfImage;
    size_t patternLen = wcslen(lpSearch) * sizeof(WCHAR);

    if (patternLen == 0 || patternLen > sizeof(PatchRecord::originalBytes)) return;
    if (patternLen > size) return; // would underflow below otherwise

    size_t scanLimit = size - patternLen;

    // Cached bounds (relative to `base`) of the region we last verified as
    // safely readable, so VirtualQuery isn't called on every single byte.
    size_t safeRegionStart = 0;
    size_t safeRegionEnd = 0; // exclusive
    bool haveSafeRegion = false;

    for (size_t i = 0; i <= scanLimit; i++) {
        if (!haveSafeRegion || i < safeRegionStart || (i + patternLen) > safeRegionEnd) {
            MEMORY_BASIC_INFORMATION mbi;
            if (!VirtualQuery((void*)(base + i), &mbi, sizeof(mbi))) {
                // Can't even query this address - stop the whole scan rather
                // than guess whether it's safe to keep going.
                break;
            }

            bool readable = mbi.State == MEM_COMMIT &&
                             mbi.Protect != PAGE_NOACCESS &&
                             !(mbi.Protect & PAGE_GUARD);

            size_t regionStart = (size_t)((DWORD_PTR)mbi.BaseAddress - base);
            size_t regionEnd = regionStart + mbi.RegionSize;
            if (regionEnd > size) regionEnd = size;

            if (!readable) {
                // Emergency stop: never touch this region. Jump past it entirely.
                i = (regionEnd > i) ? regionEnd - 1 : i; // -1: the for-loop's i++ lands us at regionEnd
                continue;
            }

            safeRegionStart = regionStart;
            safeRegionEnd = regionEnd;
            haveSafeRegion = true;

            if (i + patternLen > safeRegionEnd) {
                // Pattern would straddle into an unverified region - skip
                // this start offset instead of reading past what we checked.
                continue;
            }
        }

        const char* candidate = (const char*)(base + i);
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (((const char*)lpSearch)[j] != candidate[j]) {
                found = false;
                break;
            }
        }

        if (found) {
            void* targetAddress = (void*)(base + i);
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery(targetAddress, &mbi, sizeof(mbi))) {
                DWORD oldProtect;
                if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &oldProtect)) {
                    if (g_appliedPatchCount < ARRAYSIZE(g_appliedPatches)) {
                        g_appliedPatches[g_appliedPatchCount].address = targetAddress;
                        g_appliedPatches[g_appliedPatchCount].length = patternLen;
                        memcpy(g_appliedPatches[g_appliedPatchCount].originalBytes, targetAddress, patternLen);
                        g_appliedPatchCount++;
                    } else {
                        Wh_Log(L"Patch record buffer is full; skipping a match for \"%s\" to avoid an unsafe write", lpSearch);
                        VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldProtect, &oldProtect);
                        continue; // don't zero memory we can't guarantee we can restore later
                    }

                    ZeroMemory(targetAddress, patternLen);

                    VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldProtect, &oldProtect);
                }
            }
            // Intentionally no early return: keep scanning for further occurrences.
        }
    }
}

// Restore memory patches safely when mod unloads
void RestoreAllPatches(void) {
    if (!g_isInitialized) return;

    for (size_t i = 0; i < g_appliedPatchCount; i++) {
        PatchRecord& patch = g_appliedPatches[i];
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(patch.address, &mbi, sizeof(mbi))) {
            DWORD oldProtect;
            if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &oldProtect)) {
                memcpy(patch.address, patch.originalBytes, patch.length);
                VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldProtect, &oldProtect);
            }
        }
    }
    g_appliedPatchCount = 0;

    if (g_hWinStorage) {
        FreeLibrary(g_hWinStorage);
        g_hWinStorage = nullptr;
    }
    g_isInitialized = false;
}

// 3. Hooks
bool (*COpenControlPanel__MapLegacyName_orig)(void *, LPCWSTR, LPWSTR, UINT, bool *) = nullptr;
bool COpenControlPanel__MapLegacyName_hook(void *pThis, LPCWSTR pszLegacyName, LPWSTR pszNewName, UINT uUnused, bool *nameChanged) {
    if (nameChanged) *nameChanged = false;
    if (pszNewName && uUnused > 0) *pszNewName = L'\0';
    return false;
}

using CompareStringOrdinal_t = decltype(&CompareStringOrdinal);
CompareStringOrdinal_t CompareStringOrdinal_orig = nullptr;

// Returns the string's length as CompareStringOrdinal itself would interpret it:
// a non-negative cch is used as-is; -1 means "null terminated, compute the length".
static int GetEffectiveLength(LPCWCH str, int cch) {
    return (cch == -1) ? (int)wcslen(str) : cch;
}

// FIX: compares using the *known* length from cchCount1/cchCount2 instead of
// wcscmp/_wcsicmp, which assumed the input was null-terminated. The original
// code could read past the end of a non-null-terminated substring passed by
// a caller, risking a buffer over-read / crash.
static bool MatchesTargetList(LPCWCH str, int cch, BOOL bIgnoreCase) {
    if (!str) return false;
    int len = GetEffectiveLength(str, cch);
    if (len <= 0) return false;

    auto checkList = [&](LPCWSTR* list, size_t count) -> bool {
        for (size_t i = 0; i < count; i++) {
            size_t targetLen = wcslen(list[i]);
            if ((size_t)len != targetLen) continue; // length mismatch: can't be this entry
            int cmp = bIgnoreCase
                ? _wcsnicmp(str, list[i], targetLen)
                : wcsncmp(str, list[i], targetLen);
            if (cmp == 0) return true;
        }
        return false;
    };

    if (checkList(g_szAppletsToUnhide, ARRAYSIZE(g_szAppletsToUnhide))) return true;
    if (checkList(g_szCanonicalNames, ARRAYSIZE(g_szCanonicalNames))) return true;

    for (const auto& entry : g_customApplets) {
        if ((size_t)len != entry.size()) continue;
        int cmp = bIgnoreCase
            ? _wcsnicmp(str, entry.c_str(), entry.size())
            : wcsncmp(str, entry.c_str(), entry.size());
        if (cmp == 0) return true;
    }

    return false;
}

// FIX: on null input or when the strings don't match our target list, this now
// always defers to the real CompareStringOrdinal instead of returning 0 or the
// bogus ERROR_INVALID_PARAMETER (87) value, which is not a valid return code
// for this API and could confuse callers that branch on the result.
int WINAPI CompareStringOrdinal_hook(LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, BOOL bIgnoreCase) {
    if (!CompareStringOrdinal_orig) return 0;

    if (lpString1 && lpString2) {
        if (MatchesTargetList(lpString1, cchCount1, bIgnoreCase) ||
            MatchesTargetList(lpString2, cchCount2, bIgnoreCase)) {
            return CSTR_LESS_THAN;
        }
    }

    return CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
}

const WindhawkUtils::SYMBOL_HOOK user32DllHooks[] = {
    {
        { L"private: bool __cdecl COpenControlPanel::_MapLegacyName(unsigned short const *,unsigned short *,unsigned int,bool *)" },
        (void**)&COpenControlPanel__MapLegacyName_orig,
        (void*)COpenControlPanel__MapLegacyName_hook,
        true
    }
};

BOOL Wh_ModInit(void) {
    if (!IsSupportedWindowsVersion()) {
        Wh_Log(L"Control Panel Revival: Unsupported Windows build. Mod bypassed.");
        return FALSE;
    }

    Wh_Log(L"Initializing Control Panel Revival (Base-Derived & Safe)");

    LoadCustomAppletSettings();

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    g_hWinStorage = LoadLibraryW(L"windows.storage.dll");

    for (size_t i = 0; i < ARRAYSIZE(g_szAppletsToUnhide); i++) {
        if (hShell32) KillStringInModuleReversible(hShell32, g_szAppletsToUnhide[i]);
        if (g_hWinStorage) KillStringInModuleReversible(g_hWinStorage, g_szAppletsToUnhide[i]);
    }

    for (const auto& entry : g_customApplets) {
        if (hShell32) KillStringInModuleReversible(hShell32, entry.c_str());
        if (g_hWinStorage) KillStringInModuleReversible(g_hWinStorage, entry.c_str());
    }

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        auto pCompareStringOrdinal = (CompareStringOrdinal_t)GetProcAddress(hKernelBase, "CompareStringOrdinal");
        if (pCompareStringOrdinal) {
            if (!WindhawkUtils::SetFunctionHook(
                    (void *)pCompareStringOrdinal,
                    (void *)CompareStringOrdinal_hook,
                    (void **)&CompareStringOrdinal_orig)) {
                Wh_Log(L"Failed to hook CompareStringOrdinal; the configured applets may not be unhidden in this process");
            }
        } else {
            Wh_Log(L"CompareStringOrdinal not found in kernelbase.dll");
        }
    }

    if (hShell32) {
        // Note: Windhawk resolves this symbol automatically via Microsoft's
        // public symbol server (through the DIA SDK) and caches the PDB -
        // no manual symbol download is needed. What CAN fail is the symbol
        // itself no longer existing/matching on a future Windows build,
        // since this is a private, unexported function. If that happens,
        // we log it clearly instead of silently doing nothing, so it's easy
        // to diagnose from Wh_Log output rather than looking like the mod
        // just isn't working.
        if (!WindhawkUtils::HookSymbols(hShell32, user32DllHooks, ARRAYSIZE(user32DllHooks))) {
            Wh_Log(L"Failed to resolve/hook COpenControlPanel::_MapLegacyName - "
                   L"this function's signature may have changed in this Windows "
                   L"build. The mod will continue with its other fixes.");
        }
    }

    g_isInitialized = true;
    return TRUE;
}

void Wh_ModUninit(void) {
    Wh_Log(L"Uninitializing Control Panel Revival safely");
    RestoreAllPatches();
}

// The memory patches applied in Wh_ModInit are only easy to add correctly at
// init time (they need a clean, unpatched module to scan). Rather than trying
// to diff the old/new CustomApplets list and patch just the delta, request a
// full reload: Windhawk will call Wh_ModUninit (restoring everything) and then
// Wh_ModInit again (re-reading settings and re-patching from scratch).
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed, reloading to re-apply custom applet patches");
    *bReload = TRUE;
    return TRUE;
}
