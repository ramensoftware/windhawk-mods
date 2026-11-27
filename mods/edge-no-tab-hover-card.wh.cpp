// ==WindhawkMod==
// @id              edge-no-tab-hover-card
// @name            Edge - Disable tab hover descriptions
// @description     Suppress the complete native tab hover card in Microsoft Edge.
// @version         0.3.1
// @author          belst-n
// @github          https://github.com/belst-n
// @include         msedge.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lversion
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Edge tab hover descriptions

Hide the title, site, memory usage and preview that Edge shows when you hover
over a horizontal or vertical tab. This also hides the text popup that Edge
shows when tab preview images are turned off.

To restore the popup, turn off "Suppress tab hover cards" or disable the mod.
Move the pointer off the tab and hover again to see the change. If a card
stays on screen after installation, restarting Edge can clear it.

## Compatibility

Requires x64 Edge on Windows. Version 0.3.0 was tested on Edge
158.0.4370.16. If an Edge update changes either hover-card function enough
that the mod cannot identify both, Edge continues to show its usual popup.

## Diagnostics

"Write diagnostic status" saves the process ID, Edge and mod versions, hook
status, function addresses relative to msedge.dll, suppression setting, and
sampled call counts in the mod's local storage folder. Diagnostics are off
by default. The status file contains no tab titles or URLs.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- suppressCards: true
  $name: Suppress tab hover cards
- diagnostics: false
  $name: Write diagnostic status
  $description: Record resolver state and sampled call counts in local mod storage.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace {
struct Image {
    BYTE* base = nullptr;
    DWORD size = 0;
    std::vector<IMAGE_SECTION_HEADER> sections;
    const RUNTIME_FUNCTION* functions = nullptr;
    size_t functionCount = 0;

    bool Contains(DWORD rva, size_t length) const {
        return rva < size && length <= static_cast<size_t>(size - rva);
    }
    bool Executable(DWORD rva, size_t length) const {
        if (!Contains(rva, length)) return false;
        for (const auto& section : sections) {
            if ((section.Characteristics & IMAGE_SCN_MEM_EXECUTE) &&
                rva >= section.VirtualAddress &&
                rva - section.VirtualAddress < section.Misc.VirtualSize &&
                length <= section.Misc.VirtualSize - (rva - section.VirtualAddress))
                return true;
        }
        return false;
    }
    bool Initialize(HMODULE module) {
        base = reinterpret_cast<BYTE*>(module);
        const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0 || dos->e_lfanew > 4096)
            return false;
        const auto nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE ||
            nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
            nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
            nt->OptionalHeader.SizeOfImage < 0x1000 ||
            nt->OptionalHeader.SizeOfImage > 0x40000000 ||
            nt->FileHeader.NumberOfSections > 96 ||
            nt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_EXCEPTION)
            return false;
        size = nt->OptionalHeader.SizeOfImage;
        auto header = IMAGE_FIRST_SECTION(nt);
        if (!Contains(static_cast<DWORD>(reinterpret_cast<const BYTE*>(header) - base),
                      nt->FileHeader.NumberOfSections * sizeof(*header))) return false;
        sections.assign(header, header + nt->FileHeader.NumberOfSections);
        for (const auto& section : sections)
            if (section.Misc.VirtualSize && !Contains(section.VirtualAddress, section.Misc.VirtualSize))
                return false;
        auto directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
        if (!directory.Size || directory.Size % sizeof(RUNTIME_FUNCTION) ||
            !Contains(directory.VirtualAddress, directory.Size)) return false;
        functions = reinterpret_cast<const RUNTIME_FUNCTION*>(base + directory.VirtualAddress);
        functionCount = directory.Size / sizeof(RUNTIME_FUNCTION);
        return true;
    }
    const RUNTIME_FUNCTION* FindFunction(DWORD rva) const {
        auto end = functions + functionCount;
        auto it = std::upper_bound(functions, end, rva,
            [](DWORD value, const RUNTIME_FUNCTION& function) { return value < function.BeginAddress; });
        if (it == functions) return nullptr;
        --it;
        return rva >= it->BeginAddress && rva < it->EndAddress ? it : nullptr;
    }
    bool Root(RUNTIME_FUNCTION function, RUNTIME_FUNCTION& root) const {
        for (int depth = 0; depth < 8; ++depth) {
            if (function.EndAddress <= function.BeginAddress ||
                !Executable(function.BeginAddress, function.EndAddress - function.BeginAddress) ||
                !Contains(function.UnwindData, 4)) return false;
            const BYTE* unwind = base + function.UnwindData;
            unsigned flags = unwind[0] >> 3;
            if ((unwind[0] & 7) != 1 || flags > 4) return false;
            if (!(flags & UNW_FLAG_CHAININFO)) { root = function; return true; }
            // UNWIND_CODE records occupy two bytes, padded to a DWORD boundary.
            DWORD offset = 4 + ((static_cast<DWORD>(unwind[2]) + 1) & ~1u) * 2;
            if (!Contains(function.UnwindData, offset + sizeof(function))) return false;
            std::memcpy(&function, unwind + offset, sizeof(function));
        }
        return false; // Cyclic or unexpectedly deep chain.
    }
};

#ifdef EDGE_HOVERCARD_TEST
size_t TestInstructionLength(const BYTE* code, size_t remaining);
#endif
size_t InstructionLength(const Image& image, DWORD rva, size_t remaining) {
#ifdef EDGE_HOVERCARD_TEST
    return TestInstructionLength(image.base + rva, remaining);
#else
    if (!image.Contains(rva, 15)) return 0;
    WH_DISASM_RESULT result{};
    if (!Wh_Disasm(image.base + rva, &result) || result.length > remaining)
        return 0;
    return result.length;
#endif
}

// Edge can put the marker in a cold block with a separate unwind record.
struct Reference {
    RUNTIME_FUNCTION block;
    RUNTIME_FUNCTION root;
};
std::vector<DWORD> FindMarker(const Image& image, const char* marker) {
    std::vector<DWORD> found;
    size_t length = std::strlen(marker) + 1;
    for (const auto& section : image.sections) {
        if (!(section.Characteristics & IMAGE_SCN_MEM_READ) ||
            (section.Characteristics & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_MEM_EXECUTE)))
            continue;
        const BYTE* first = image.base + section.VirtualAddress;
        const BYTE* last = first + section.Misc.VirtualSize;
        while (first < last) {
            auto match = std::search(first, last, marker, marker + length);
            if (match == last) break;
            // A standalone string, not a suffix embedded in another identifier.
            if (match == image.base + section.VirtualAddress || match[-1] == 0)
                found.push_back(static_cast<DWORD>(match - image.base));
            if (found.size() > 1) return found;
            first = match + length;
        }
    }
    return found;
}

bool OnInstructionBoundary(const Image& image, const RUNTIME_FUNCTION& block, DWORD rva) {
    if (block.EndAddress <= block.BeginAddress || block.EndAddress - block.BeginAddress > 0x10000)
        return false;
    DWORD cursor = block.BeginAddress;
    while (cursor < rva) {
        size_t length = InstructionLength(image, cursor, block.EndAddress - cursor);
        if (!length || length > rva - cursor) return false;
        cursor += static_cast<DWORD>(length);
    }
    return cursor == rva;
}

std::vector<Reference> FindReferences(const Image& image, DWORD marker) {
    std::vector<Reference> found;
    for (const auto& section : image.sections) {
        if (!(section.Characteristics & IMAGE_SCN_MEM_EXECUTE) || section.Misc.VirtualSize < 7)
            continue;
        const BYTE* code = image.base + section.VirtualAddress;
        for (DWORD i = 0; i <= section.Misc.VirtualSize - 7; ++i) {
            // LEA r64,[RIP+disp32]. A subsequent boundary check rejects matches
            // that merely occur inside another instruction's immediate bytes.
            if ((code[i] != 0x48 && code[i] != 0x4c) || code[i + 1] != 0x8d ||
                (code[i + 2] & 0xc7) != 0x05) continue;
            int32_t displacement;
            std::memcpy(&displacement, code + i + 3, sizeof(displacement));
            DWORD rva = section.VirtualAddress + i;
            if (static_cast<int64_t>(rva) + 7 + displacement != marker) continue;
            const auto function = image.FindFunction(rva);
            RUNTIME_FUNCTION root;
            if (!function || !image.Executable(function->BeginAddress,
                    function->EndAddress - function->BeginAddress) ||
                !image.Root(*function, root) || !OnInstructionBoundary(image, *function, rva))
                continue;
            found.push_back({*function, root});
            if (found.size() > 1) return found;
        }
    }
    return found;
}

bool EntryMatches(const Image& image, DWORD rva) {
    // Verified common Win64 entry in Edge 157 and 158: saved nonvolatile regs,
    // 0x90 stack frame, GS cookie, then test this->hover_card_ at offset 0x48.
    // Only the RIP displacement to the GS cookie is ignored.
    constexpr BYTE prefix[] = {0x41,0x57,0x41,0x56,0x56,0x57,0x53,
        0x48,0x81,0xec,0x90,0,0,0,0x48,0x8b,0x05};
    constexpr BYTE suffix[] = {0x48,0x31,0xe0,0x48,0x89,0x84,0x24,
        0x88,0,0,0,0x48,0x83,0x79,0x48,0};
    return image.Executable(rva, 37) &&
        !std::memcmp(image.base + rva, prefix, sizeof(prefix)) &&
        !std::memcmp(image.base + rva + 21, suffix, sizeof(suffix));
}

bool CallerMatches(const Image& image, const Reference& update, DWORD showRva) {
    DWORD cursor = update.block.BeginAddress;
    unsigned calls = 0;
    DWORD rcxAt = 0, edxAt = 0, r8At = 0;
    while (cursor < update.block.EndAddress) {
        size_t remaining = update.block.EndAddress - cursor;
        size_t length = InstructionLength(image, cursor, remaining);
        if (!length) return false;
        const BYTE* code = image.base + cursor;
        // Argument assignments seen in the verified callers. The target is a
        // pointer in R8, the initial-show flag an integer in EDX, this in RCX.
        if (length == 3 && code[0] == 0x48 && code[1] == 0x89 &&
            (code[2] == 0xd9 || code[2] == 0xf9)) rcxAt = cursor;
        if (length == 2 && code[0] == 0x89 &&
            (code[1] == 0xea || code[1] == 0xda)) edxAt = cursor;
        if (length == 3 && code[0] == 0x49 && code[1] == 0x89 && code[2] == 0xf0)
            r8At = cursor;
        if (length == 5 && (code[0] == 0xe8 || code[0] == 0xe9)) {
            int32_t displacement;
            std::memcpy(&displacement, code + 1, sizeof(displacement));
            if (static_cast<int64_t>(cursor) + 5 + displacement == showRva &&
                rcxAt && edxAt && r8At && cursor - rcxAt < 96 &&
                cursor - edxAt < 96 && cursor - r8At < 96) ++calls;
        }
        cursor += static_cast<DWORD>(length);
    }
    return calls == 1;
}

constexpr BYTE kAnimating = 3;

bool ControllerEntryMatches(const Image& image, DWORD rva) {
    constexpr BYTE prefix[] = {0x41,0x56,0x56,0x57,0x53,0x48,0x83,0xec,0x78,0x48,0x8b,0x05};
    // Skip the cookie displacement. Check the hide-lock branch, including
    // the event it selects, before passing that event to UpdateHoverCard.
    constexpr BYTE suffix[] = {0x48,0x31,0xe0,0x48,0x89,0x44,0x24,0x70,
        0x83,0xb9,0x14,0x01,0,0,0,0x41,0xb9,kAnimating,0,0,0,
        0x45,0x0f,0x4e,0xc8,0x0f,0x9f,0xc0,0x48,0x85,0xd2};
    return image.Executable(rva, 47) &&
        !std::memcmp(image.base + rva, prefix, sizeof(prefix)) &&
        !std::memcmp(image.base + rva + 16, suffix, sizeof(suffix));
}

DWORD FindControllerEntry(const Image& image, DWORD updateOrShowEntry) {
    std::vector<DWORD> candidates;
    // The private UpdateOrShowCard routine is called by UpdateHoverCard.
    // Resolve this caller through unwind metadata, then check both its entry
    // and the three argument reloads immediately before its decoded call.
    constexpr BYTE reloads[] = {0x48,0x8b,0x4c,0x24,0x30,
        0x48,0x8b,0x54,0x24,0x28,0x44,0x8b,0x44,0x24,0x3c};
    for (const auto& section : image.sections) {
        if (!(section.Characteristics & IMAGE_SCN_MEM_EXECUTE) || section.Misc.VirtualSize < 20)
            continue;
        const BYTE* code = image.base + section.VirtualAddress;
        for (DWORD i = sizeof(reloads); i <= section.Misc.VirtualSize - 5; ++i) {
            if (code[i] != 0xe8 || std::memcmp(code + i - sizeof(reloads), reloads, sizeof(reloads)))
                continue;
            int32_t displacement;
            std::memcpy(&displacement, code + i + 1, 4);
            DWORD rva = section.VirtualAddress + i;
            if (static_cast<int64_t>(rva) + 5 + displacement != updateOrShowEntry) continue;
            const auto function = image.FindFunction(rva);
            RUNTIME_FUNCTION root;
            if (!function || !image.Root(*function, root) ||
                !OnInstructionBoundary(image, *function, rva) ||
                !ControllerEntryMatches(image, root.BeginAddress)) continue;
            if (std::find(candidates.begin(), candidates.end(), root.BeginAddress) == candidates.end())
                candidates.push_back(root.BeginAddress);
            if (candidates.size() > 1) return 0;
        }
    }
    return candidates.size() == 1 ? candidates[0] : 0;
}

struct Resolution { DWORD entry = 0; DWORD marker = 0; DWORD updateMarker = 0; bool chained = false; DWORD controller = 0; };
bool Resolve(HMODULE module, Resolution& result) {
    result = {};
    Image image;
    if (!image.Initialize(module)) return false;
    auto showMarkers = FindMarker(image, "FN_TabHoverCardBubble");
    auto updateMarkers = FindMarker(image, "UpdateOrShowCard");
    if (showMarkers.size() != 1 || updateMarkers.size() != 1) return false;
    auto showRefs = FindReferences(image, showMarkers[0]);
    auto updateRefs = FindReferences(image, updateMarkers[0]);
    if (showRefs.size() != 1 || updateRefs.size() != 1) return false;
    const auto& show = showRefs[0];
    const auto& update = updateRefs[0];
    if (show.root.BeginAddress == update.root.BeginAddress ||
        !EntryMatches(image, show.root.BeginAddress) ||
        !CallerMatches(image, update, show.root.BeginAddress)) return false;
    DWORD controller = FindControllerEntry(image, update.root.BeginAddress);
    if (!controller) return false;
    result = {show.root.BeginAddress, showMarkers[0], updateMarkers[0],
              show.root.BeginAddress != show.block.BeginAddress, controller};
    return true;
}

using ShowFn = void (*)(void*, bool, void*);
ShowFn g_original = nullptr;
using UpdateFn = void (*)(void*, void*, int);
UpdateFn g_updateOriginal = nullptr;
std::atomic_bool g_suppress{true}, g_diagnostics{false};
std::atomic_ulong g_blocked{0}, g_forwarded{0};
std::atomic_ulong g_redirected{0}, g_updates{0};
std::atomic<DWORD> g_entry{0};
std::atomic<DWORD> g_controllerEntry{0};
std::atomic<const char*> g_state{"initializing"};
HANDLE g_stop = nullptr, g_worker = nullptr;
wchar_t g_statusPath[MAX_PATH] = {};
char g_version[64] = "unknown";
SRWLOCK g_statusLock = SRWLOCK_INIT;

void WriteStatus() {
    if (!g_diagnostics.load() || !g_statusPath[0]) return;
    // Avoid blocking the browser UI behind a concurrent status writer.
    if (!TryAcquireSRWLockExclusive(&g_statusLock)) return;
    char text[512];
    int length = std::snprintf(text, sizeof(text),
        "mod_version=0.3.1\r\nedge_version=%s\r\npid=%lu\r\nstate=%s\r\n"
        "entry_rva=0x%08lX\r\ncontroller_rva=0x%08lX\r\nsuppress=%d\r\n"
        "blocked=%lu\r\nforwarded=%lu\r\nredirected=%lu\r\nupdates=%lu\r\n",
        g_version, GetCurrentProcessId(), g_state.load(), g_entry.load(),
        g_controllerEntry.load(), g_suppress.load() ? 1 : 0,
        g_blocked.load(), g_forwarded.load(), g_redirected.load(), g_updates.load());
    if (length > 0 && length < static_cast<int>(sizeof(text))) {
        HANDLE file = CreateFileW(g_statusPath, GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE) {
            DWORD written;
            WriteFile(file, text, length, &written, nullptr);
            CloseHandle(file);
        }
    }
    ReleaseSRWLockExclusive(&g_statusLock);
}
void Status(const char* state) { g_state.store(state); WriteStatus(); }
void UpdateHook(void* self, void* anchor, int updateType) {
    ++g_updates;
    if (g_suppress.load(std::memory_order_relaxed)) {
        unsigned long count = ++g_redirected;
        // Data-change events require an anchor. The hide-lock event accepts a
        // null anchor and lets Edge cancel its timer and clean up observers.
        g_updateOriginal(self, nullptr, kAnimating);
        if (count <= 10 || count % 100 == 0) WriteStatus();
        return;
    }
    g_updateOriginal(self, anchor, updateType);
}
void ShowHook(void* self, bool initial, void* target) {
    if (g_suppress.load(std::memory_order_relaxed)) {
        unsigned long count = ++g_blocked;
        if (count <= 3 || count % 100 == 0) WriteStatus();
        return;
    }
    unsigned long count = ++g_forwarded;
    if (count <= 3 || count % 100 == 0) WriteStatus();
    g_original(self, initial, target);
}

void DetectVersion(HMODULE module) {
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) return;
    DWORD ignored;
    DWORD size = GetFileVersionInfoSizeW(path, &ignored);
    if (!size || size > 1024 * 1024) return;
    std::vector<BYTE> buffer(size);
    if (!GetFileVersionInfoW(path, 0, size, buffer.data())) return;
    VS_FIXEDFILEINFO* info = nullptr;
    UINT infoSize = 0;
    if (VerQueryValueW(buffer.data(), L"\\", reinterpret_cast<void**>(&info), &infoSize) &&
        infoSize >= sizeof(*info) && info->dwSignature == 0xfeef04bd) {
        AcquireSRWLockExclusive(&g_statusLock);
        std::snprintf(g_version, sizeof(g_version), "%u.%u.%u.%u",
            HIWORD(info->dwProductVersionMS), LOWORD(info->dwProductVersionMS),
            HIWORD(info->dwProductVersionLS), LOWORD(info->dwProductVersionLS));
        ReleaseSRWLockExclusive(&g_statusLock);
    }
}

DWORD WINAPI Worker(void*) {
    HMODULE module = nullptr;
    for (int attempt = 0; attempt < 300; ++attempt) {
        if (WaitForSingleObject(g_stop, 0) != WAIT_TIMEOUT) return 0;
        module = GetModuleHandleW(L"msedge.dll");
        if (module) break;
        if (WaitForSingleObject(g_stop, 100) != WAIT_TIMEOUT) return 0;
    }
    if (!module) { Status("module-load-timeout"); return 0; }
    DetectVersion(module);
    Resolution resolved;
    if (!Resolve(module, resolved)) {
        Wh_Log(L"Hover card resolver refused the missing, ambiguous, or changed layout");
        Status("compatibility-check-refused"); return 0;
    }
    g_entry.store(resolved.entry);
    g_controllerEntry.store(resolved.controller);
    Wh_Log(L"ShowHoverCard RVA=%08X; chained unwind=%d", resolved.entry, resolved.chained);
    if (WaitForSingleObject(g_stop, 0) != WAIT_TIMEOUT) return 0;
    auto target = reinterpret_cast<BYTE*>(module) + resolved.entry;
    auto controller = reinterpret_cast<BYTE*>(module) + resolved.controller;
    if (!Wh_SetFunctionHook(target, reinterpret_cast<void*>(ShowHook),
                            reinterpret_cast<void**>(&g_original))) {
        Status("hook-registration-failed"); return 0;
    }
    if (!Wh_SetFunctionHook(controller, reinterpret_cast<void*>(UpdateHook),
                            reinterpret_cast<void**>(&g_updateOriginal))) {
        Wh_RemoveFunctionHook(target);
        Status("controller-hook-registration-failed"); return 0;
    }
    if (!Wh_ApplyHookOperations()) { Status("hook-application-failed"); return 0; }
    Status("hook-active");
    return 0;
}
void LoadSettings() {
    g_suppress.store(Wh_GetIntSetting(L"suppressCards") != 0);
    g_diagnostics.store(Wh_GetIntSetting(L"diagnostics") != 0);
}
} // namespace

BOOL Wh_ModInit() {
    int argc = 0;
    auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return FALSE;
    bool subprocess = false;
    for (int i = 1; i < argc; ++i)
        if (!wcsncmp(argv[i], L"--type=", 7) || !wcscmp(argv[i], L"--type"))
            subprocess = true;
    LocalFree(argv);
    if (subprocess) return FALSE;
    wchar_t storage[MAX_PATH];
    if (Wh_GetModStoragePath(storage, ARRAYSIZE(storage)))
        swprintf_s(g_statusPath, L"%ls\\status-%lu.txt", storage, GetCurrentProcessId());
    LoadSettings();
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop) return FALSE;
    Status("waiting-for-msedge-dll");
    return TRUE;
}
void Wh_ModAfterInit() {
    g_worker = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_worker) Status("worker-creation-failed");
}
void Wh_ModSettingsChanged() {
    LoadSettings();
    WriteStatus();
}
void Wh_ModBeforeUninit() {
    if (g_stop) SetEvent(g_stop);
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
}
void Wh_ModUninit() {
    Status("unloaded");
    if (g_stop) { CloseHandle(g_stop); g_stop = nullptr; }
}
