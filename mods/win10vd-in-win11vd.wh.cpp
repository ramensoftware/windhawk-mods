// ==WindhawkMod==
// @id              win10vd-in-win11vd
// @name            Windows 10 Virtual Desktop animations
// @description     Restores native Windows 10 virtual desktop switch animations on Windows 11
// @version         1.0
// @author          iamfromreallife
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod replaces windows 11 virtual desktop switch animations with the much faster and smoother ones from Windows 10.

It also adds an option to disable desktop indicator popups.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- removeDesktopPopups: false
  $name: Remove desktop indicator popups
  $description: Hides the desktop name/number popup without hiding volume or brightness indicators.
*/
// ==/WindhawkModSettings==

#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <objbase.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>

struct IVirtualDesktop;

namespace {
using CycleFn = HRESULT (*)(void*, int);
using SwitchFn = HRESULT (*)(void*, IVirtualDesktop*);
using SwitchMonitorFn = HRESULT (*)(void*, HMONITOR, IVirtualDesktop*);
using CurrentFn = HRESULT (*)(void*, IVirtualDesktop**);
using AdjacentFn = HRESULT (*)(void*, IVirtualDesktop*, int, IVirtualDesktop**);
using SpecialRouteFn = HRESULT (*)(void*, IVirtualDesktop*, BOOL*);
using BeginFn = HRESULT (WINAPI*)(int, const GUID*);
using WindowFn = HRESULT (WINAPI*)(HWND, int);
using EndFn = HRESULT (WINAPI*)(int);
using FlushFn = HRESULT (WINAPI*)();
using PopupFn = HRESULT (*)(void*, IVirtualDesktop*, int);
using PreferenceFn = bool (*)(void*);

CycleFn g_cycle = nullptr;
SwitchFn g_switch = nullptr;
SwitchFn g_animatedSwitch = nullptr;
SwitchMonitorFn g_switchMonitor = nullptr;
SwitchMonitorFn g_animatedMonitor = nullptr;
BeginFn g_begin = nullptr;
WindowFn g_window = nullptr;
EndFn g_end = nullptr;
FlushFn g_flush = nullptr;
PopupFn g_popup = nullptr;
using PopupOldFn = HRESULT (*)(void*, IVirtualDesktop*);
PopupOldFn g_popupOld = nullptr;
using IndicatorTextFn = HRESULT (WINAPI*)(void*, void*, bool);
IndicatorTextFn g_indicatorText = nullptr;
PreferenceFn g_preference = nullptr;
decltype(&SystemParametersInfoW) g_spi = nullptr;
struct NativeLayout {
    HMODULE shell = nullptr;
    size_t managerOffset = 0;
    size_t currentSlot = 0, adjacentSlot = 0;
    uintptr_t knownTable = 0;
    CurrentFn current = nullptr;
    AdjacentFn adjacent = nullptr;
    SpecialRouteFn special = nullptr;
};
NativeLayout g_layout;

std::atomic<bool> g_enabled{false};
std::atomic<bool> g_removePopups{false};
std::atomic_flag g_busy = ATOMIC_FLAG_INIT;
thread_local unsigned g_legacyDepth = 0;
thread_local int g_hotkeyDirection = -1;

constexpr GUID kTransitionGuid = {0xe322e3ee,0xc875,0x48e8,{0xbc,0x8a,0x6a,0x7a,0x87,0x97,0xa9,0x3c}};
constexpr int kTransitionType = 0x5a;
constexpr int LegacyTarget(int direction) { return direction == 0 ? 0x4c : 0x4b; }

struct DesktopRef {
    IVirtualDesktop* p = nullptr;
    ~DesktopRef() { if (p) reinterpret_cast<IUnknown*>(p)->Release(); }
};
struct BusyGuard { ~BusyGuard() { g_busy.clear(std::memory_order_release); } };
struct LegacyScope {
    LegacyScope() { ++g_legacyDepth; }
    ~LegacyScope() { --g_legacyDepth; }
};

struct HotkeyScope {
    int previous;
    explicit HotkeyScope(int direction) : previous(g_hotkeyDirection) { g_hotkeyDirection = direction; }
    ~HotkeyScope() { g_hotkeyDirection = previous; }
};

bool IsWindows11() {
    using RtlGetVersionFn = LONG (WINAPI*)(OSVERSIONINFOW*);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) return false;
    auto getVersion = reinterpret_cast<RtlGetVersionFn>(GetProcAddress(ntdll, "RtlGetVersion"));
    OSVERSIONINFOW v{};
    v.dwOSVersionInfoSize = sizeof(v);
    return getVersion && getVersion(&v) == 0 && v.dwMajorVersion == 10 && v.dwBuildNumber >= 22000;
}

HMODULE SystemModule(PCWSTR name) {
    HMODULE module = GetModuleHandleW(name);
    return module ? module : LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
}

template<typename T> bool ReadNative(uintptr_t address, T& value) {
    SIZE_T count = 0;
    return address && ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(address),
                                        &value, sizeof(value), &count) && count == sizeof(value);
}

bool MatchesNativeImage(HMODULE module, DWORD stamp, DWORD imageSize, const GUID& pdb) {
    const auto base = reinterpret_cast<uintptr_t>(module);
    IMAGE_DOS_HEADER dos{};
    IMAGE_NT_HEADERS64 nt{};
    if (!ReadNative(base, dos) || dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew < 0 || dos.e_lfanew > 4096 ||
        !ReadNative(base + dos.e_lfanew, nt) || nt.Signature != IMAGE_NT_SIGNATURE ||
        nt.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 || nt.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt.FileHeader.TimeDateStamp != stamp || nt.OptionalHeader.SizeOfImage != imageSize ||
        nt.OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_DEBUG) return false;
    const auto& dir = nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
    if (!dir.VirtualAddress || dir.Size > 4096 || dir.VirtualAddress >= imageSize || dir.Size > imageSize - dir.VirtualAddress) return false;
    struct CodeView { DWORD magic; GUID id; DWORD age; };
    static_assert(sizeof(CodeView) == 24);
    for (unsigned i = 0; i < dir.Size / sizeof(IMAGE_DEBUG_DIRECTORY); ++i) {
        IMAGE_DEBUG_DIRECTORY entry{};
        if (!ReadNative(base + dir.VirtualAddress + i * sizeof(entry), entry)) return false;
        if (entry.Type != IMAGE_DEBUG_TYPE_CODEVIEW || entry.SizeOfData < 24 ||
            entry.AddressOfRawData >= imageSize || 24 > imageSize - entry.AddressOfRawData) continue;
        CodeView cv{};
        if (ReadNative(base + entry.AddressOfRawData, cv) && cv.magic == 0x53445352 &&
            IsEqualGUID(cv.id, pdb) && cv.age == 1) return true;
    }
    return false;
}

template<size_t N> bool MatchesEntry(uintptr_t address, const unsigned char (&expected)[N]) {
    unsigned char actual[N]{};
    return ReadNative(address, actual) && std::memcmp(actual, expected, N) == 0;
}

bool ImageAddress(HMODULE module, uintptr_t address, bool executable) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (!module || !address || !VirtualQuery(reinterpret_cast<const void*>(address), &mbi, sizeof(mbi)) ||
        mbi.AllocationBase != module || mbi.State != MEM_COMMIT || (mbi.Protect & PAGE_GUARD)) return false;
    const DWORD p = mbi.Protect & 0xff;
    return executable ? (p == PAGE_EXECUTE || p == PAGE_EXECUTE_READ || p == PAGE_EXECUTE_READWRITE || p == PAGE_EXECUTE_WRITECOPY)
                      : (p == PAGE_READONLY);
}

struct CodeRange {
    uintptr_t address = 0;
    size_t size = 0;
    std::array<unsigned char, 4096> bytes{};
};
bool ReadFunction(HMODULE module, uintptr_t address, CodeRange& out) {
    if (!ImageAddress(module, address, true)) return false;
    using LookupFn = PRUNTIME_FUNCTION (WINAPI*)(DWORD64, PDWORD64, void*);
    auto lookup = reinterpret_cast<LookupFn>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlLookupFunctionEntry"));
    if (!lookup) return false;
    DWORD64 base = 0;
    auto entry = lookup(address, &base, nullptr);
    if (!entry || base != reinterpret_cast<uintptr_t>(module) || base + entry->BeginAddress != address ||
        entry->EndAddress <= entry->BeginAddress) return false;
    out.size = entry->EndAddress - entry->BeginAddress;
    out.address = address;
    if (out.size > out.bytes.size() || !ImageAddress(module, address + out.size - 1, true)) return false;
    SIZE_T count = 0;
    return ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(address), out.bytes.data(), out.size, &count) && count == out.size;
}

template<size_t N> bool Pattern(const CodeRange& code, size_t at, const int (&pattern)[N]) {
    if (at > code.size || N > code.size - at) return false;
    for (size_t j = 0; j < N; ++j)
        if (pattern[j] >= 0 && code.bytes[at+j] != pattern[j]) return false;
    return true;
}
template<typename T> T Operand(const CodeRange& code, size_t at) {
    T value{};
    if (at <= code.size && sizeof(T) <= code.size-at) std::memcpy(&value, code.bytes.data()+at, sizeof(T));
    return value;
}

bool DiscoverLayout(HMODULE shell, uintptr_t cycle, NativeLayout& layout) {
    CodeRange code{};
    if (!ReadFunction(shell, cycle, code)) return false;
    constexpr int managerLoad[] = {0x49,0x8b,0x7e,-1,0x48,0x8b,0x07,0x48,0x8b,0x98,-1,-1,-1,-1};
    constexpr int routeCall[] = {0x83,0x64,0x24,-1,0x00,0x4c,0x8d,0x44,0x24,-1,0x48,0x8b,0x54,0x24,-1,0xe8,-1,-1,-1,-1,0x85,0xc0,0x0f,0x85,-1,-1,-1,-1,0x39,0x44,0x24,-1,0x0f,0x84,-1,-1,-1,-1};
    constexpr int directionMap[] = {0x41,0x8b,0xc7,0xf7,0xd8,0x1b,0xf6,0xf7,0xde};
    constexpr int adjacentArgument[] = {0x44,0x8d,0x46,0x03};
    size_t loads[2]{}, loadCount = 0, route = 0, routeCount = 0;
    for (size_t i = 0; i < code.size; ++i) {
        if (Pattern(code,i,managerLoad)) {
            if (loadCount == 2) return false;
            loads[loadCount++] = i;
        }
        if (Pattern(code,i,routeCall)) { route = i; ++routeCount; }
    }
    if (loadCount != 2 || routeCount != 1 || loads[1] >= route) return false;
    const unsigned offset = code.bytes[loads[0]+3];
    const auto first = Operand<uint32_t>(code,loads[0]+10);
    const auto second = Operand<uint32_t>(code,loads[1]+10);
    if (offset == 0 || offset > 0x78 || offset % sizeof(void*) || offset != code.bytes[loads[1]+3] ||
        first % sizeof(void*) || second % sizeof(void*) || first < 3*sizeof(void*) || second <= first || second > 95*sizeof(void*)) return false;
    bool directionOk = false, adjacentOk = false;
    for (size_t i = loads[0]+sizeof(managerLoad)/sizeof(int); i < loads[1]; ++i)
        if (Pattern(code,i,directionMap)) directionOk = true;
    for (size_t i = loads[1]+sizeof(managerLoad)/sizeof(int); i < route; ++i)
        if (Pattern(code,i,adjacentArgument)) adjacentOk = true;
    if (!directionOk || !adjacentOk || code.bytes[route+3] != code.bytes[route+9] || code.bytes[route+3] != code.bytes[route+31]) return false;
    const int64_t branch1 = static_cast<int64_t>(route+28)+Operand<int32_t>(code,route+24);
    const int64_t branch2 = static_cast<int64_t>(route+38)+Operand<int32_t>(code,route+34);
    if (branch1 != branch2 || branch1 < 0 || branch1 >= static_cast<int64_t>(code.size)) return false;
    const auto special = code.address+route+20+static_cast<int64_t>(Operand<int32_t>(code,route+16));
    CodeRange helper{};
    constexpr int helperEntry[] = {0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x10,0x49,0x89,0x4b,0x08,0x57,0x48,0x83,0xec,0x20,0x49,0x8b,0xf8,0x48,0x8b,0xca,0x48,0x8b,0x02,0x49,0x83,0x63,0x08,0x00,0x4d,0x8d,0x43,0x08,0x48,0x8d,0x15,-1,-1,-1,-1,0x48,0x8b,0x00,0xe8,-1,-1,-1,-1};
    constexpr int helperMethod[] = {0x48,0x8b,0x4c,0x24,0x30,0x48,0x8b,0x01,0x48,0x8b,0xd7,0x48,0x8b,0x40,0x40,0xe8,-1,-1,-1,-1};
    if (!ReadFunction(shell,special,helper) || !Pattern(helper,0,helperEntry)) return false;
    bool methodOk = false;
    for (size_t i=sizeof(helperEntry)/sizeof(int); i<helper.size; ++i)
        if (Pattern(helper,i,helperMethod)) methodOk = true;
    if (!methodOk) return false;
    const auto iidAddress = special+41+static_cast<int64_t>(Operand<int32_t>(helper,37));
    constexpr GUID remoteIid = {0xc9be252c,0xe386,0x425e,{0xb3,0x3c,0xca,0x42,0x21,0x12,0xf5,0x1f}};
    GUID actual{};
    if (!ImageAddress(shell,iidAddress,false) || !ReadNative(iidAddress,actual) || !IsEqualGUID(actual,remoteIid)) return false;
    layout.managerOffset=offset;
    layout.currentSlot=first/sizeof(void*);
    layout.adjacentSlot=second/sizeof(void*);
    layout.special=reinterpret_cast<SpecialRouteFn>(special);
    return true;
}

HRESULT PopupHook(void* self, IVirtualDesktop* desktop, int type) {
    if (g_enabled.load() && g_removePopups.load()) return S_OK;
    return g_popup(self, desktop, type);
}

HRESULT PopupOldHook(void* self, IVirtualDesktop* desktop) {
    if (g_enabled.load() && g_removePopups.load()) return S_OK;
    return g_popupOld(self, desktop);
}

bool IsDesktopTextCaller(uintptr_t caller) {
    HMODULE twin = GetModuleHandleW(L"twinui.dll");
    return ImageAddress(twin, caller, true);
}

HRESULT DispatchIndicatorText(void* self, void* text, bool flag, uintptr_t caller) {
    if (g_enabled.load() && g_removePopups.load() && IsDesktopTextCaller(caller))
        return S_OK;
    return g_indicatorText(self, text, flag);
}

HRESULT WINAPI IndicatorTextHook(void* self, void* text, bool flag) {
    return DispatchIndicatorText(self, text, flag,
                                 reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
}

void InstallIndicatorTextHook() {
    HMODULE indicator = SystemModule(L"Windows.Internal.HardwareConfirmator.dll");
    if (!indicator) return;
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost,struct winrt::Windows::Internal::HardwareConfirmator::IHardwareConfirmatorHost>::ShowText(void *,bool)"},
         reinterpret_cast<void**>(&g_indicatorText), reinterpret_cast<void*>(IndicatorTextHook), true},
    };
    WindhawkUtils::HookSymbols(indicator, symbols, ARRAYSIZE(symbols));
}

bool SuppressNativeSlide() { return g_enabled.load() && g_legacyDepth != 0; }
template<typename Switch> bool RunLegacySwitch(HWND shell, int direction, Switch&& doSwitch, HRESULT& result) {
    LegacyScope scope;
    if (FAILED(g_begin(kTransitionType, &kTransitionGuid))) return false;
    bool requestOpen = true;
    auto completeRequest = [&]() -> HRESULT {
        if (!requestOpen) return S_OK;
        requestOpen = false;
        return g_end(kTransitionType);
    };
    const HRESULT submit = g_window(shell, LegacyTarget(direction));
    if (FAILED(submit)) {
        if (SUCCEEDED(completeRequest())) return false;
    } else if (FAILED(g_flush())) {
        completeRequest();
    }
    result = doSwitch();
    completeRequest();
    return true;
}
HWND LocalShellWindow() {
    HWND shell = GetShellWindow();
    DWORD pid = 0;
    return shell && GetWindowThreadProcessId(shell, &pid) && pid == GetCurrentProcessId() ? shell : nullptr;
}

template<typename Switch> bool TryNativeSelectedDesktop(Switch&& doSwitch, HRESULT& result) {
    if (!g_enabled.load() || g_layout.special || g_hotkeyDirection < 0 || g_hotkeyDirection > 1) return false;
    HWND shell = LocalShellWindow();
    if (!shell) return false;
    if (g_busy.test_and_set(std::memory_order_acquire)) { result = S_OK; return true; }
    BusyGuard busy;
    return RunLegacySwitch(shell,g_hotkeyDirection,doSwitch,result);
}
HRESULT AnimatedSwitchHook(void* self, IVirtualDesktop* desktop) {
    if (SuppressNativeSlide()) return g_switch(self, desktop);
    HRESULT result = S_OK;
    if (self && desktop && TryNativeSelectedDesktop([&]{return g_switch(self,desktop);},result)) return result;
    return g_animatedSwitch(self, desktop);
}
HRESULT AnimatedMonitorHook(void* self,HMONITOR monitor,IVirtualDesktop* desktop) {
    if (SuppressNativeSlide()) return g_switchMonitor(self,monitor,desktop);
    HRESULT result = S_OK;
    if (self && desktop && TryNativeSelectedDesktop([&]{return g_switchMonitor(self,monitor,desktop);},result)) return result;
    return g_animatedMonitor(self,monitor,desktop);
}
BOOL WINAPI SpiHook(UINT action, UINT param, PVOID output, UINT flags) {
    if (SuppressNativeSlide() && action == SPI_GETCLIENTAREAANIMATION && output) {
        *static_cast<BOOL*>(output) = FALSE;
        return TRUE;
    }
    return g_spi(action, param, output, flags);
}
bool PreferenceHook(void* self) {
    return SuppressNativeSlide() ? false : g_preference(self);
}

bool TryLegacyCycle(void* self, int direction, HRESULT& result) {
    if (!g_enabled.load() || !g_layout.special || !self || (direction != 0 && direction != 1)) return false;
    if (g_busy.test_and_set(std::memory_order_acquire)) { result = S_OK; return true; }
    BusyGuard busy;
    uintptr_t manager = 0, table = 0, current = 0, adjacent = 0;
    const auto& layout = g_layout;
    if (!ReadNative(reinterpret_cast<uintptr_t>(self) + layout.managerOffset, manager) || !manager ||
        !ReadNative(manager, table) || !ImageAddress(layout.shell,table,false) ||
        (layout.knownTable && table != layout.knownTable) ||
        !ReadNative(table + layout.currentSlot * sizeof(void*), current) || current != reinterpret_cast<uintptr_t>(layout.current) ||
        !ReadNative(table + layout.adjacentSlot * sizeof(void*), adjacent) || adjacent != reinterpret_cast<uintptr_t>(layout.adjacent)) return false;
    bool switchFound = false;
    if (layout.knownTable) {
        uintptr_t entry = 0;
        switchFound = ReadNative(table+9*sizeof(void*),entry) && entry==reinterpret_cast<uintptr_t>(g_switch);
    }
    for (size_t slot = 3; !layout.knownTable && slot < layout.currentSlot; ++slot) {
        uintptr_t entry = 0;
        const auto address = table + slot*sizeof(void*);
        if (!ImageAddress(layout.shell,address,false) || !ReadNative(address,entry) || !ImageAddress(layout.shell,entry,true)) break;
        if (entry == reinterpret_cast<uintptr_t>(g_switch)) { switchFound = true; break; }
    }
    if (!switchFound) return false;
    HWND shell = LocalShellWindow();
    if (!shell) return false;
    DesktopRef from, to;
    auto* mgr = reinterpret_cast<void*>(manager);
    result = reinterpret_cast<CurrentFn>(current)(mgr, &from.p);
    if (FAILED(result) || !from.p) return false;
    result = reinterpret_cast<AdjacentFn>(adjacent)(mgr, from.p, direction + 3, &to.p);
    if (FAILED(result) || !to.p || to.p == from.p) return true;
    BOOL specialRoute = FALSE;
    const HRESULT routeHr = layout.special(self, to.p, &specialRoute);
    if (routeHr == S_OK && specialRoute != FALSE) return false;
    if (!g_enabled.load()) return false;
    return RunLegacySwitch(shell,direction,[&]{return g_switch(mgr,to.p);},result);
}

HRESULT CycleHook(void* self, int direction) {
    if (SuppressNativeSlide()) return S_OK;
    HRESULT result = S_OK;
    if (TryLegacyCycle(self, direction, result)) return result;
    HotkeyScope scope(!g_layout.special && (direction == 0 || direction == 1) ? direction : -1);
    return g_cycle(self, direction);
}

bool ResolveDwm(HMODULE dwm) {
    auto begin = GetProcAddress(dwm, MAKEINTRESOURCEA(145));
    auto window = GetProcAddress(dwm, MAKEINTRESOURCEA(139));
    auto end = GetProcAddress(dwm, MAKEINTRESOURCEA(140));
    auto flush = GetProcAddress(dwm, "DwmFlush");
    if (!begin || !window || !end || !flush) return false;
    constexpr GUID pdb = {0xbbc0d51f,0xde94,0x8952,{0xbe,0x0c,0x45,0x7f,0xc4,0xdb,0x8c,0x0f}};
    const auto base = reinterpret_cast<uintptr_t>(dwm);
    if (MatchesNativeImage(dwm,0x08886f6a,0x2e000,pdb)) {
        if (reinterpret_cast<uintptr_t>(begin)!=base+0xb460 || reinterpret_cast<uintptr_t>(window)!=base+0x9e50 || reinterpret_cast<uintptr_t>(end)!=base+0xa9a0) return false;
    } else {
        void *namedBegin=nullptr,*namedWindow=nullptr,*namedEnd=nullptr;
        WindhawkUtils::SYMBOL_HOOK symbols[] = {
            {{L"DwmpBeginTransitionRequestWithGUID"}, &namedBegin, nullptr},
            {{L"DwmpTransitionWindow"}, &namedWindow, nullptr},
            {{L"DwmpEndTransitionRequest"}, &namedEnd, nullptr},
        };
        if (!WindhawkUtils::HookSymbols(dwm,symbols,ARRAYSIZE(symbols)) ||
            namedBegin!=reinterpret_cast<void*>(begin) || namedWindow!=reinterpret_cast<void*>(window) || namedEnd!=reinterpret_cast<void*>(end)) return false;
    }
    g_begin=reinterpret_cast<BeginFn>(begin);
    g_window=reinterpret_cast<WindowFn>(window);
    g_end=reinterpret_cast<EndFn>(end);
    g_flush=reinterpret_cast<FlushFn>(flush);
    return true;
}

bool ResolveShell(HMODULE shell, void*& cycle, void*& animated, void*& animatedMonitor) {
    NativeLayout layout{};
    layout.shell=shell;
    const auto base = reinterpret_cast<uintptr_t>(shell);
    constexpr GUID pdb = {0xb81d419f,0x7567,0x47a5,{0x6e,0x74,0xd3,0xe3,0xd2,0xd5,0x74,0xff}};
    if (MatchesNativeImage(shell,0x4630d73d,0x9ea000,pdb)) {
        constexpr unsigned char cycleEntry[] = {0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x41,0x56,0x41,0x57};
        constexpr unsigned char animatedEntry[] = {0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x81,0xec,0xa0,0x00};
        constexpr unsigned char switchEntry[] = {0x48,0x89,0x5c,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0xfa,0x48,0x8b,0xd9};
        constexpr unsigned char routeEntry[] = {0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x10,0x49,0x89,0x4b,0x08,0x57,0x48,0x83,0xec,0x20};
        if (!MatchesEntry(base+0x5bdba4,cycleEntry) || !MatchesEntry(base+0x5bad00,animatedEntry) || !MatchesEntry(base+0x5ba1d0,switchEntry) || !MatchesEntry(base+0x5bd740,routeEntry)) return false;
        cycle=reinterpret_cast<void*>(base+0x5bdba4);
        animated=reinterpret_cast<void*>(base+0x5bad00);
        g_switch=reinterpret_cast<SwitchFn>(base+0x5ba1d0);
        layout.managerOffset=0x18; layout.currentSlot=46; layout.adjacentSlot=48;
        layout.knownTable=base+0x7cd860;
        layout.current=reinterpret_cast<CurrentFn>(base+0x8c4b0);
        layout.adjacent=reinterpret_cast<AdjacentFn>(base+0x5b63b0);
        layout.special=reinterpret_cast<SpecialRouteFn>(base+0x5bd740);
    } else {
        WindhawkUtils::SYMBOL_HOOK symbols[] = {
            {{L"private: long __cdecl CVirtualDesktopHotkeyHandler::_CycleInDirection(enum VirtualDesktopSwitchDirection)"}, &cycle, nullptr},
            {{L"public: virtual long __cdecl CVirtualDesktopManager::SwitchDesktopWithAnimation(struct IVirtualDesktop *)"}, &animated, nullptr, true},
            {{L"public: virtual long __cdecl CVirtualDesktopManager::SwitchDesktop(struct IVirtualDesktop *)"}, reinterpret_cast<void**>(&g_switch), nullptr, true},
            {{L"public: virtual long __cdecl CVirtualDesktopManager::SwitchDesktopWithAnimation(struct HMONITOR__ *,struct IVirtualDesktop *)"}, &animatedMonitor, nullptr, true},
            {{L"public: virtual long __cdecl CVirtualDesktopManager::SwitchDesktop(struct HMONITOR__ *,struct IVirtualDesktop *)"}, reinterpret_cast<void**>(&g_switchMonitor), nullptr, true},
            {{L"public: virtual long __cdecl CVirtualDesktopManager::GetCurrentDesktopInternal(struct IVirtualDesktop * *)", L"public: virtual long __cdecl CVirtualDesktopManager::GetCurrentDesktop(struct IVirtualDesktop * *)"}, reinterpret_cast<void**>(&layout.current), nullptr, true},
            {{L"public: virtual long __cdecl CVirtualDesktopManager::GetAdjacentDesktopInternal(struct IVirtualDesktop *,unsigned int,struct IVirtualDesktop * *)", L"public: virtual long __cdecl CVirtualDesktopManager::GetAdjacentDesktop(struct IVirtualDesktop *,unsigned int,struct IVirtualDesktop * *)"}, reinterpret_cast<void**>(&layout.adjacent), nullptr, true},
        };
        if (!WindhawkUtils::HookSymbols(shell,symbols,ARRAYSIZE(symbols)) || !cycle) return false;
        if (!animated || !g_switch) { animated=nullptr; g_switch=nullptr; }
        if (!animatedMonitor || !g_switchMonitor) { animatedMonitor=nullptr; g_switchMonitor=nullptr; }
        if (!animated && !animatedMonitor) return false;
        if (animated && animatedMonitor && animated == animatedMonitor) return false;
        if (!(animated && layout.current && layout.adjacent && DiscoverLayout(shell,reinterpret_cast<uintptr_t>(cycle),layout))) {
            layout=NativeLayout{};
            layout.shell=shell;
        }

    }
    g_layout=layout;
    return true;
}

bool InstallLegacy(HMODULE shell, HMODULE dwm) {
    void *cycle=nullptr,*animated=nullptr,*animatedMonitor=nullptr;
    if (!ResolveShell(shell,cycle,animated,animatedMonitor) || !ResolveDwm(dwm)) return false;
    if (animated && (!Wh_SetFunctionHook(animated,reinterpret_cast<void*>(AnimatedSwitchHook),reinterpret_cast<void**>(&g_animatedSwitch)) || !g_animatedSwitch)) return false;
    if (animatedMonitor && (!Wh_SetFunctionHook(animatedMonitor,reinterpret_cast<void*>(AnimatedMonitorHook),reinterpret_cast<void**>(&g_animatedMonitor)) || !g_animatedMonitor)) return false;
    return Wh_SetFunctionHook(cycle,reinterpret_cast<void*>(CycleHook),reinterpret_cast<void**>(&g_cycle)) && g_cycle &&
           Wh_SetFunctionHook(reinterpret_cast<void*>(SystemParametersInfoW),reinterpret_cast<void*>(SpiHook),reinterpret_cast<void**>(&g_spi)) && g_spi;
}

void InstallOptionalHooks() {
    HMODULE twin = SystemModule(L"twinui.dll");
    if (!twin) return;
    constexpr GUID twinPdb = {0x912bce8e,0x937a,0x7ec4,{0x3d,0xa2,0x00,0x5c,0xee,0xd8,0x90,0x04}};
    const auto base = reinterpret_cast<uintptr_t>(twin);
    if (MatchesNativeImage(twin,0x81d8de5e,0x51e000,twinPdb)) {
        constexpr unsigned char popupEntry[] = {0x48,0x89,0x5c,0x24,0x08,0x55,0x56,0x57,0x41,0x56,0x41,0x57,0x48,0x8b,0xec};
        constexpr unsigned char preferenceEntry[] = {0x8a,0x81,0xa8,0x00,0x00,0x00,0xc3};
        if (MatchesEntry(base+0x35cf20,popupEntry))
            Wh_SetFunctionHook(reinterpret_cast<void*>(base+0x35cf20),reinterpret_cast<void*>(PopupHook),reinterpret_cast<void**>(&g_popup));
        if (MatchesEntry(base+0x1c2820,preferenceEntry))
            Wh_SetFunctionHook(reinterpret_cast<void*>(base+0x1c2820),reinterpret_cast<void*>(PreferenceHook),reinterpret_cast<void**>(&g_preference));
        if (g_popup && g_preference) return;
        if (!g_popup) {
            WindhawkUtils::SYMBOL_HOOK popupSymbols[] = {
                {{L"public: virtual long __cdecl CAudioFlyoutController::VirtualDesktopSwitched(struct IVirtualDesktop *,enum VirtualDesktopSwitchType)"}, reinterpret_cast<void**>(&g_popup), reinterpret_cast<void*>(PopupHook), true},
            };
            WindhawkUtils::HookSymbols(twin,popupSymbols,ARRAYSIZE(popupSymbols));
        }
        return;
    }
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"public: virtual long __cdecl CAudioFlyoutController::VirtualDesktopSwitched(struct IVirtualDesktop *)"}, reinterpret_cast<void**>(&g_popupOld), reinterpret_cast<void*>(PopupOldHook), true},
        {{L"public: virtual long __cdecl CAudioFlyoutController::VirtualDesktopSwitched(struct IVirtualDesktop *,enum VirtualDesktopSwitchType)"}, reinterpret_cast<void**>(&g_popup), reinterpret_cast<void*>(PopupHook), true},
        {{L"public: virtual bool __cdecl CSwitchThumbnailDeviceManager::AnimationsEnabled(void)"}, reinterpret_cast<void**>(&g_preference), reinterpret_cast<void*>(PreferenceHook), true},
    };
    WindhawkUtils::HookSymbols(twin,symbols,ARRAYSIZE(symbols));
}
} 

BOOL Wh_ModInit() {
    if (!IsWindows11()) return FALSE;
    HMODULE shell = SystemModule(L"twinui.pcshell.dll");
    HMODULE dwm = SystemModule(L"dwmapi.dll");
    if (!shell || !dwm || !InstallLegacy(shell, dwm)) return FALSE;
    g_removePopups = Wh_GetIntSetting(L"removeDesktopPopups") != 0;
    InstallOptionalHooks();
    InstallIndicatorTextHook();
    g_enabled = true;
    return TRUE;
}
void Wh_ModSettingsChanged() {
    g_removePopups = Wh_GetIntSetting(L"removeDesktopPopups") != 0;
}
void Wh_ModBeforeUninit() { g_enabled = false; }
