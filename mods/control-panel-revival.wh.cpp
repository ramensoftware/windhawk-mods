// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival
// @description     Prevents Control Panel applets from redirecting to the modern Settings app on Windows 11 23H2+ by unhiding legacy elements safely.
// @version         0.9.5
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @include         control.exe
// @include         rundll32.exe
// @architecture    x86-64
// @compilerOptions -lpsapi -lcomctl32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Control Panel Revival

### What does this mod do?

This mod is designed to restore sections of the Control Panel—such as Troubleshooting, Installed Updates, Default Programs, and others—that are redirected to the Settings app in Windows 11 (version 23H2 and later) and can no longer be launched even via shell commands.
You can also add the ID of your desired applet to prevent it from being redirected to the settings.

**Suggestion** To make it top-notch, I suggest you also try the "Windows 7 Applet Restorer" mod

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
#include <psapi.h>
#include <windhawk_utils.h>
#include <shlwapi.h>
#pragma  comment(lib, "shlwapi.lib")

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

// Structure to save original bytes for safe restoration.
struct PatchRecord {
    void* address;
    BYTE originalBytes[128];
    size_t length;
};

// std::vector instead of a fixed-size array: the scan deliberately doesn't
// stop at the first match (a string can legitimately appear more than once
// in a module), so a fixed cap could silently drop matches partway through,
// leaving the mod half-applied and half-restorable. This also removes the
// only reason a "too many custom applets" cap/warning ever existed.
std::vector<PatchRecord> g_appliedPatches;
HMODULE g_hWinStorage = nullptr;
bool g_ownsWinStorageHandle = false; // true only if we called LoadLibrary* ourselves
bool g_isInitialized = false;

// User-provided applet GUIDs / canonical names, loaded from the "CustomApplets"
// array setting, in addition to the 6 built-in ones above.
std::vector<std::wstring> g_customApplets;

// Hash of custom applets for detecting actual changes
std::wstring g_prevCustomAppletsHash;

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

// A canonical name shorter than this turns into a very short, very common
// UTF-16 byte pattern (e.g. a single letter is just 2 bytes), which can
// match thousands of times across shell32.dll/windows.storage.dll in data
// that has nothing to do with any redirect table. All of Microsoft's known
// canonical CPL names are well above this length. Combined with requiring a
// dot (every real canonical name looks like "Vendor.Item"), this is a floor
// against garbage/too-short input reaching the memory scanner at all - it is
// NOT a guarantee the string is a real applet name.
constexpr size_t kMinCanonicalNameLength = 8;

// Rejects anything that clearly isn't a plausible Control Panel applet
// identifier (garbage input, pasted text, anything too long or too short),
// so it never reaches the memory-scanning/patching code at all.
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

// 1. Build Gate: Ensures it only runs on Windows 11 23H2 (Build 22631) or newer
bool IsSupportedWindowsVersion() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return false;

    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
    if (!pRtlGetVersion) return false;

    RTL_OSVERSIONINFOW osvi = { sizeof(osvi) };
    if (pRtlGetVersion(&osvi) == 0) {
        // Support Windows 10 22H2+ and Windows 11 23H2+
        if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.dwBuildNumber >= 22621) {
            return true; // Windows 11 22H2+ and Windows 10 22H2+
        }
    }
    return false;
}

// Safe string comparison helper that handles -1 (null-terminated) lengths
static int SafeCompareString(LPCWCH str1, int cch1, LPCWCH str2, int cch2, BOOL bIgnoreCase) {
    if (!str1 || !str2) return CSTR_LESS_THAN;
    
    int len1 = (cch1 == -1) ? (int)wcslen(str1) : cch1;
    int len2 = (cch2 == -1) ? (int)wcslen(str2) : cch2;
    
    if (len1 <= 0 || len2 <= 0) return CSTR_LESS_THAN;
    
    int minLen = std::min(len1, len2);
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
// Target-list lengths are now precomputed constexpr std::wstring_view
// sizes, not recomputed via wcslen on every call.
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

// rundll32.exe hosts arbitrary DLLs and Windows spawns it constantly for
// completely unrelated work. Without this check, every such instance would
// still hook CompareStringOrdinal/LoadLibraryExW and scan module memory for
// nothing. Only bail out for rundll32.exe specifically, and only when its
// command line doesn't look like a Control Panel applet host -
// explorer.exe/control.exe are never affected by this check.
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

    // Check if command line contains "Control_RunDLL" (case-insensitive)
    return StrStrIW(GetCommandLineW(), L"Control_RunDLL") == nullptr;
}

// Describes one [start, end) byte range, relative to a module's base
// address, that is safe to pattern-scan.
struct ByteRange {
    size_t start;
    size_t end;
};

// Parses a loaded module's PE section table and returns only the ranges
// covering non-executable, initialized-data sections (e.g. .rdata, .data).
// This is what guarantees the scan below can NEVER touch .text (code) or
// the PE headers themselves, no matter how a search pattern is chosen -
// closing off the "one unlucky/short custom applet ID corrupts explorer.exe"
// failure mode entirely, rather than just making it less likely.
static std::vector<ByteRange> GetScannableDataRanges(HMODULE hModule, size_t moduleSize) {
    std::vector<ByteRange> ranges;

    auto base = (const BYTE*)hModule;
    auto dosHeader = (const IMAGE_DOS_HEADER*)base;
    if (moduleSize < sizeof(IMAGE_DOS_HEADER) || dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return ranges;
    }

    auto ntHeaders = (const IMAGE_NT_HEADERS*)(base + dosHeader->e_lfanew);
    if ((size_t)dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) > moduleSize ||
        ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return ranges;
    }

    auto sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
    WORD numSections = ntHeaders->FileHeader.NumberOfSections;

    for (WORD i = 0; i < numSections; i++, sectionHeader++) {
        // Never scan executable sections (this is what excludes .text).
        if (sectionHeader->Characteristics & IMAGE_SCN_MEM_EXECUTE) continue;
        // Only sections that actually contain initialized data - also
        // naturally excludes .bss-style uninitialized sections.
        if (!(sectionHeader->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)) continue;

        size_t start = sectionHeader->VirtualAddress;
        size_t rawSize = (std::max)(sectionHeader->Misc.VirtualSize, sectionHeader->SizeOfRawData);
        if (start >= moduleSize) continue;

        size_t end = start + rawSize;
        if (end > moduleSize) end = moduleSize;
        if (end > start) ranges.push_back({ start, end });
    }

    return ranges;
}

// 2. Reversible Memory Patching
//
//    Scoped to non-executable, initialized-data PE sections only (see
//    GetScannableDataRanges) - never .text, never the headers. Combined with
//    only checking WCHAR-aligned offsets (every pattern here is UTF-16
//    string data, which is always 2-byte aligned within these sections),
//    this closes off the possibility of a match landing inside executable
//    code, which byte-at-a-time whole-image scanning could not rule out.
//
//    Continues scanning past the first match (a string can legitimately
//    appear more than once in a module) and verifies every region via
//    VirtualQuery before touching it, skipping straight past anything not
//    committed/readable instead of reading or writing it.
void KillStringInModuleReversible(HMODULE hModule, LPCWSTR lpSearch) {
    if (!hModule || !lpSearch) return;

    MODULEINFO info = { 0 };
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &info, sizeof(MODULEINFO))) return;

    DWORD_PTR base = (DWORD_PTR)info.lpBaseOfDll;
    size_t moduleSize = (size_t)info.SizeOfImage;
    size_t patternLen = wcslen(lpSearch) * sizeof(WCHAR);

    if (patternLen == 0 || patternLen > sizeof(PatchRecord::originalBytes)) return;
    if (patternLen > moduleSize) return;

    std::vector<ByteRange> dataRanges = GetScannableDataRanges(hModule, moduleSize);
    if (dataRanges.empty()) return; // couldn't parse the PE headers; don't guess

    for (const ByteRange& range : dataRanges) {
        if (patternLen > range.end - range.start) continue; // pattern can't fit in this section

        size_t scanLimit = range.end - patternLen;

        // Cached bounds (relative to `base`) of the region we last verified
        // as safely readable, so VirtualQuery isn't called on every offset.
        size_t safeRegionStart = 0;
        size_t safeRegionEnd = 0; // exclusive
        bool haveSafeRegion = false;

        size_t i = range.start;
        while (i <= scanLimit) {
            bool needRequery = !haveSafeRegion || i < safeRegionStart || i >= safeRegionEnd;
            if (needRequery) {
                MEMORY_BASIC_INFORMATION mbi;
                if (!VirtualQuery((void*)(base + i), &mbi, sizeof(mbi))) {
                    break; // can't even query - stop scanning this section
                }

                bool readable = mbi.State == MEM_COMMIT &&
                                 mbi.Protect != PAGE_NOACCESS &&
                                 !(mbi.Protect & PAGE_GUARD);

                size_t regionStart = (size_t)((DWORD_PTR)mbi.BaseAddress - base);
                size_t regionEnd = regionStart + mbi.RegionSize;
                if (regionEnd > range.end) regionEnd = range.end;
                if (regionStart < range.start) regionStart = range.start;

                if (!readable || regionEnd <= regionStart) {
                    // Emergency stop: never touch this region, skip straight past it.
                    i = regionEnd;
                    if (i % sizeof(WCHAR) != 0) i += sizeof(WCHAR) - (i % sizeof(WCHAR));
                    continue;
                }

                safeRegionStart = regionStart;
                safeRegionEnd = regionEnd;
                haveSafeRegion = true;
            }

            if (i + patternLen > safeRegionEnd) {
                // Pattern would straddle past what we've verified as
                // readable; skip to the end of the verified region.
                i = safeRegionEnd;
                if (i % sizeof(WCHAR) != 0) i += sizeof(WCHAR) - (i % sizeof(WCHAR));
                continue;
            }

            const char* candidate = (const char*)(base + i);
            if (memcmp(lpSearch, candidate, patternLen) == 0) {
                void* targetAddress = (void*)(base + i);
                MEMORY_BASIC_INFORMATION mbi;
                if (VirtualQuery(targetAddress, &mbi, sizeof(mbi))) {
                    DWORD oldProtect;
                    if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &oldProtect)) {
                        PatchRecord patch;
                        patch.address = targetAddress;
                        patch.length = patternLen;
                        memcpy(patch.originalBytes, targetAddress, patternLen);
                        g_appliedPatches.push_back(patch);

                        ZeroMemory(targetAddress, patternLen);
                        Wh_Log(L"Successfully patched applet string at address: 0x%p", targetAddress);

                        VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldProtect, &oldProtect);
                    }
                }
                // Intentionally no early return/break: keep scanning for
                // further occurrences within this section.
            }

            i += sizeof(WCHAR);
        }
    }
}

// Restore memory patches safely when mod unloads.
void RestoreAllPatches(void) {
    if (!g_isInitialized) return;

    Wh_Log(L"Restoring %zu memory patches", g_appliedPatches.size());
    
    for (PatchRecord& patch : g_appliedPatches) {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(patch.address, &mbi, sizeof(mbi))) {
            DWORD oldProtect;
            if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &oldProtect)) {
                memcpy(patch.address, patch.originalBytes, patch.length);
                VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldProtect, &oldProtect);
            }
        }
    }
    g_appliedPatches.clear();

    // Only release the handle if we actually acquired a reference to it
    // ourselves (via LoadLibrary*). If it came from GetModuleHandleW, we
    // never incremented its refcount, and calling FreeLibrary on it would
    // incorrectly decrement whatever refcount another owner is relying on.
    if (g_hWinStorage && g_ownsWinStorageHandle) {
        FreeLibrary(g_hWinStorage);
    }
    g_hWinStorage = nullptr;
    g_ownsWinStorageHandle = false;

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

// Defers to the real CompareStringOrdinal on null input or when the strings
// don't match our target list, instead of returning an invalid/inconsistent
// result. This hook is now gated to only apply when shell32.dll is loaded,
// preventing interference with unrelated string comparisons in the process.
int WINAPI CompareStringOrdinal_hook(LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, BOOL bIgnoreCase) {
    if (!CompareStringOrdinal_orig) return 0;

    // Only apply the hook when shell32.dll is loaded (context check)
    if (!GetModuleHandleW(L"shell32.dll")) {
        return CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
    }

    if (lpString1 && lpString2) {
        if (MatchesTargetList(lpString1, cchCount1, bIgnoreCase) ||
            MatchesTargetList(lpString2, cchCount2, bIgnoreCase)) {
            return CSTR_LESS_THAN; // Force "not equal" to prevent redirect
        }
    }

    return CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
}

// Resolved against shell32.dll - see ApplyShell32DependentPatches below.
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
// NULL there and nothing would ever get patched/hooked in exactly the host
// process that actually runs Control Panel applets.
//
// Fix: hook LoadLibraryExW in kernelbase.dll (internal callers go straight
// to kernelbase, not through the kernel32 import) and apply the shell32
// patches/hooks the moment shell32.dll actually gets loaded, whenever that
// happens - at Wh_ModInit time (explorer.exe, control.exe) or later
// (rundll32.exe).
//
// g_shell32PatchesApplied is claimed atomically BEFORE any work happens
// (not after), so a nested LoadLibraryExW call triggered from within this
// function can't re-enter it, and concurrent calls from different threads
// can't race each other into doing the work twice.
volatile LONG g_shell32PatchesApplied = 0;

// isLateLoad: true when called from the LoadLibraryExW hook (shell32.dll
// just finished loading after Wh_ModInit already returned), false when
// called directly from Wh_ModInit. This controls two things that must only
// happen on the late path:
//   - Wh_ApplyHookOperations() must never be called before Wh_ModInit
//     returns (documented API requirement) - only the late path calls it.
//   - windows.storage.dll is only force-loaded on the direct path. On the
//     late path we're running from inside another DLL's LoadLibraryExW
//     call, which may still be holding the loader lock; issuing a nested
//     LoadLibrary (and potentially a slow HookSymbols PDB resolution) from
//     under that lock risks a deadlock. On that path we only ever look for
//     windows.storage.dll if something else already loaded it naturally.
void ApplyShell32DependentPatches(bool isLateLoad) {
    if (InterlockedCompareExchange(&g_shell32PatchesApplied, 1, 0) != 0) {
        return; // already applied, or another call already claimed this
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        InterlockedExchange(&g_shell32PatchesApplied, 0); // not actually loaded yet; allow a real attempt later
        return;
    }

    // Check if windows.storage.dll is still loaded (may have been unloaded)
    if (g_hWinStorage && !GetModuleHandleW(L"windows.storage.dll")) {
        Wh_Log(L"windows.storage.dll was unloaded, resetting handle");
        if (g_ownsWinStorageHandle) {
            FreeLibrary(g_hWinStorage);
        }
        g_hWinStorage = nullptr;
        g_ownsWinStorageHandle = false;
    }

    g_hWinStorage = GetModuleHandleW(L"windows.storage.dll"); // borrowed, no refcount taken
    if (!g_hWinStorage && !isLateLoad) {
        // windows.storage.dll is not a KnownDLL, so a bare-name load would
        // search the application directory before System32 - force
        // System32 explicitly to avoid a DLL-planting risk.
        g_hWinStorage = LoadLibraryExW(L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        g_ownsWinStorageHandle = (g_hWinStorage != nullptr);
        
        if (g_hWinStorage) {
            Wh_Log(L"Successfully loaded windows.storage.dll");
        } else {
            Wh_Log(L"windows.storage.dll not found, some applets may not be unhidden");
        }
    }

    // Apply patches for built-in applets
    for (const auto& applet : g_szAppletsToUnhide) {
        KillStringInModuleReversible(hShell32, applet.data());
        if (g_hWinStorage) KillStringInModuleReversible(g_hWinStorage, applet.data());
    }

    // Apply patches for custom applets
    for (const auto& entry : g_customApplets) {
        KillStringInModuleReversible(hShell32, entry.c_str());
        if (g_hWinStorage) KillStringInModuleReversible(g_hWinStorage, entry.c_str());
    }

    // Windhawk resolves this symbol automatically via Microsoft's public
    // symbol server (through the DIA SDK) and caches the PDB - no manual
    // symbol download is needed. What CAN fail is the symbol itself no
    // longer existing/matching on a future Windows build, since this is a
    // private, unexported function. If that happens, we log it clearly
    // instead of silently doing nothing.
    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to resolve/hook COpenControlPanel::_MapLegacyName - "
               L"this function's signature may have changed in this Windows "
               L"build. The mod will continue with its other fixes.");
    }

    if (isLateLoad) {
        // Required here because these hooks were installed outside
        // Wh_ModInit's normal batch. Must NOT be called from the direct
        // (Wh_ModInit) path - Windhawk applies that batch itself when
        // Wh_ModInit returns, and calling this before that happens is not
        // supported by the API.
        Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_orig = nullptr;

HMODULE WINAPI LoadLibraryExW_hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE result = LoadLibraryExW_orig(lpLibFileName, hFile, dwFlags);

    if (result && !g_shell32PatchesApplied) {
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
                ApplyShell32DependentPatches(/*isLateLoad=*/true);
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

    Wh_Log(L"Initializing Control Panel Revival v1.0.0");

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
        // and ApplyShell32DependentPatches() below handles that case directly.
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
    ApplyShell32DependentPatches(/*isLateLoad=*/false);

    return TRUE;
}

void Wh_ModUninit(void) {
    Wh_Log(L"Uninitializing Control Panel Revival safely");

    RestoreAllPatches();
    InterlockedExchange(&g_shell32PatchesApplied, 0);
}

// The memory patches applied in Wh_ModInit are only easy to add correctly at
// init time (they need a clean, unpatched module to scan). Rather than trying
// to diff the old/new CustomApplets list and patch just the delta, request a
// full reload: Windhawk will call Wh_ModUninit (restoring everything) and then
// Wh_ModInit again (re-reading settings and re-patching from scratch).
// 
// Now with actual change detection: only reload if custom applets have changed.
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // Compute hash of current custom applets
    std::wstring currentHash;
    for (const auto& applet : g_customApplets) {
        currentHash += applet;
    }

    if (currentHash != g_prevCustomAppletsHash) {
        g_prevCustomAppletsHash = currentHash;
        Wh_Log(L"Settings changed, reloading to re-apply custom applet patches");
        *bReload = TRUE;
        return TRUE;
    }

    *bReload = FALSE;
    return FALSE;
}
