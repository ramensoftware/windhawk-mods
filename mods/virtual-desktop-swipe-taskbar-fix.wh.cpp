// ==WindhawkMod==
// @id              virtual-desktop-swipe-taskbar-fix
// @name            Virtual Desktop Swipe Taskbar Fix
// @description     Restores taskbar presence in virtual desktop swipe previews.
// @version         1.0.5
// @author          meteoni
// @github          https://github.com/Meteoni
// @license         MIT
// @include         explorer.exe
// @architecture    amd64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Virtual Desktop Swipe Taskbar Fix

Restores the missing incoming taskbar bitmap during a touchpad virtual desktop swipe
on Windows 11.

![The issue](https://raw.githubusercontent.com/Meteoni/meteoni-assets/main/virtual-desktop-swipe-taskbar-fix/virtual-desktop-swipe-taskbar-fix.png)
_The issue_

## Limitation

While a swipe is held, both previews can show the current desktop's taskbar
buttons, instead of reflecting the incoming taskbar's current state. This is
because Windows has not yet committed the switch during a swipe gesture, and
thumbnails taken at that time inevitably look the same as on the outgoing
desktop.

Keyboard switching avoids this because it freezes the outgoing taskbar preview
and updates the actual taskbar during the transition, allowing the live incoming
preview to show the destination desktop's buttons.

ARM64 is not supported: native methods are resolved through symbols, but internal
member offsets are discovered from those methods at initialization. Initialization
stops if the required symbols or layout cannot be recognized.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <unknwn.h>
#include <cstddef>
#include <array>
#include <map>
#include <cstdio>
#include <cwchar>
#include <optional>
#include <regex>
#include <string>
#include <vector>

using GetTrayBitmapResult = HRESULT (*)(void*, const void*, int, const RECT&, void*, IUnknown*);
using InitializeGestureWindow = HRESULT (*)(void*, unsigned long);
using GetTrayWindowForMonitor = HWND (*)(HMONITOR);
using AddThumbnailToCollection = HRESULT (*)(void*, unsigned int, IUnknown*);
using CreateWindowThumbnail = HRESULT (*)(void*, IUnknown*, HWND, DWORD,
                                        const SIZE*, RECT*, const GUID&, void**);
using SetThumbnailRect = HRESULT (*)(IUnknown*, const RECT&, int, void*);

GetTrayBitmapResult g_getTrayBitmapResultOriginal;
InitializeGestureWindow g_initializeGestureWindow;
GetTrayWindowForMonitor g_getTrayWindowForMonitor;
AddThumbnailToCollection g_addThumbnailToCollection;
CreateWindowThumbnail g_createWindowThumbnail;
SetThumbnailRect g_setThumbnailRect;
void* g_switchThumbnailFactoryVftable;
void* g_createCustomPaintThumbnail;

struct GestureLayout {
    size_t window;
    size_t bitmap;
    size_t customPaint;
    size_t contextType;
    size_t factory;
    size_t parent;
    size_t firstCollection;
    size_t secondCollection;
    unsigned int firstCollectionSelector;
} g_gestureLayout;

struct NativeInstruction {
    uintptr_t address;
    size_t length;
    std::string text;
    std::vector<uintptr_t> successors;
};

bool ReadFunctionControlFlow(void* function, size_t instructionLimit,
                             std::vector<NativeInstruction>& instructions,
                             const void* nativeImage = nullptr) {
    DWORD64 imageBase = 0;
    auto* entry = RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(function), &imageBase, nullptr);
    IMAGE_DOS_HEADER dos{};
    IMAGE_NT_HEADERS64 nt{};
    if (!entry || !ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(imageBase),
            &dos, sizeof(dos), nullptr) || dos.e_magic != IMAGE_DOS_SIGNATURE ||
        dos.e_lfanew < 0 || dos.e_lfanew > 0x100000 ||
        !ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(imageBase + dos.e_lfanew),
            &nt, sizeof(nt), nullptr) || nt.Signature != IMAGE_NT_SIGNATURE ||
        nt.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        nt.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt.OptionalHeader.SizeOfImage < sizeof(nt)) {
        Wh_Log(L"Function unwind metadata unavailable for %p", function);
        return false;
    }
    uintptr_t codeBase = nativeImage ? reinterpret_cast<uintptr_t>(nativeImage) : imageBase;
    if (nativeImage) {
        IMAGE_DOS_HEADER copyDos{};
        IMAGE_NT_HEADERS64 copyNt{};
        if (!ReadProcessMemory(GetCurrentProcess(), nativeImage, &copyDos, sizeof(copyDos), nullptr) ||
            copyDos.e_magic != dos.e_magic || copyDos.e_lfanew != dos.e_lfanew ||
            !ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(codeBase + dos.e_lfanew),
                &copyNt, sizeof(copyNt), nullptr) || copyNt.Signature != nt.Signature ||
            copyNt.FileHeader.Machine != nt.FileHeader.Machine ||
            copyNt.FileHeader.TimeDateStamp != nt.FileHeader.TimeDateStamp ||
            copyNt.OptionalHeader.SizeOfImage != nt.OptionalHeader.SizeOfImage ||
            copyNt.OptionalHeader.CheckSum != nt.OptionalHeader.CheckSum) {
            Wh_Log(L"Native image differs from the loaded shell module");
            return false;
        }
    }

    // Chained unwind entries identify separated regions of the same function.
    // Do not infer ownership from proximity or follow an arbitrary external jump.
    auto primaryEntry = [&](RUNTIME_FUNCTION region) -> std::optional<RUNTIME_FUNCTION> {
        for (int depth = 0; depth < 8; ++depth) {
            BYTE header[4]{};
            if (region.BeginAddress >= region.EndAddress || region.EndAddress > nt.OptionalHeader.SizeOfImage ||
                region.UnwindData > nt.OptionalHeader.SizeOfImage - sizeof(header) ||
                !ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(imageBase + region.UnwindData),
                    header, sizeof(header), nullptr) || ((header[0] & 7) != 1 && (header[0] & 7) != 2)) return std::nullopt;
            if (!(header[0] & (UNW_FLAG_CHAININFO << 3))) return region;
            size_t chainOffset = region.UnwindData + 4 + ((header[2] + 1) & ~1u) * 2;
            if (chainOffset > nt.OptionalHeader.SizeOfImage - sizeof(region) ||
                !ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(imageBase + chainOffset),
                    &region, sizeof(region), nullptr)) return std::nullopt;
        }
        return std::nullopt;
    };
    auto primary = primaryEntry(*entry);
    if (!primary) {
        Wh_Log(L"Invalid function unwind chain for %p", function);
        return false;
    }
    std::vector<std::pair<uintptr_t, uintptr_t>> regions{
        {imageBase + entry->BeginAddress, imageBase + entry->EndAddress}};
    std::map<uintptr_t, size_t> decodedAddresses;
    const std::regex directBranch(R"(0x[0-9a-f]+)");
    std::vector<uintptr_t> pending{reinterpret_cast<uintptr_t>(function)};
    while (!pending.empty()) {
        uintptr_t address = pending.back();
        pending.pop_back();
        while (!decodedAddresses.contains(address)) {
            uintptr_t regionEnd = 0;
            for (const auto& [begin, end] : regions) {
                if (address >= begin && address < end) { regionEnd = end; break; }
            }
            if (!regionEnd) {
                DWORD64 targetImage = 0;
                auto* target = RtlLookupFunctionEntry(address, &targetImage, nullptr);
                auto owner = target && targetImage == imageBase ? primaryEntry(*target) : std::nullopt;
                if (!owner || owner->BeginAddress != primary->BeginAddress ||
                    owner->EndAddress != primary->EndAddress || owner->UnwindData != primary->UnwindData) {
                    Wh_Log(L"Branch outside verified function regions: %p", reinterpret_cast<void*>(address));
                    return false;
                }
                regions.emplace_back(imageBase + target->BeginAddress, imageBase + target->EndAddress);
                regionEnd = regions.back().second;
            }
            if (instructions.size() >= instructionLimit) {
                Wh_Log(L"Function instruction budget exceeded for %p", function);
                return false;
            }
            WH_DISASM_RESULT result{};
            if (!Wh_Disasm(reinterpret_cast<void*>(codeBase + address - imageBase), &result) || !result.length ||
                result.length > 15 || result.length > regionEnd - address) {
                Wh_Log(L"Failed to disassemble instruction at %p", reinterpret_cast<void*>(address));
                return false;
            }
            // Reject branches into an instruction, including instructions decoded earlier.
            auto following = decodedAddresses.upper_bound(address);
            bool overlapsPrevious = false;
            if (following != decodedAddresses.begin()) {
                const auto& previous = instructions[std::prev(following)->second];
                overlapsPrevious = previous.address + previous.length > address;
            }
            if ((following != decodedAddresses.end() && following->first < address + result.length) ||
                overlapsPrevious) {
                Wh_Log(L"Overlapping instruction boundaries at %p", reinterpret_cast<void*>(address));
                return false;
            }
            NativeInstruction instruction{address, result.length, result.text, {}};
            for (char& character : instruction.text) {
                if (character >= 'A' && character <= 'Z') character += 'a' - 'A';
            }
            bool terminates = instruction.text.starts_with("ret") || instruction.text == "int3" || instruction.text == "ud2";
            bool jump = instruction.text.starts_with('j') || instruction.text.starts_with("loop");
            if (jump) {
                auto operand = instruction.text.substr(instruction.text.find(' ') + 1);
                if (!std::regex_match(operand, directBranch)) {
                    Wh_Log(L"Indirect branch not supported at %p", reinterpret_cast<void*>(address));
                    return false;
                }
                uintptr_t target = std::stoull(operand, nullptr, 0);
                if (target < codeBase || target - codeBase >= nt.OptionalHeader.SizeOfImage) {
                    Wh_Log(L"Branch outside native image at %p", reinterpret_cast<void*>(address));
                    return false;
                }
                target = imageBase + target - codeBase;
                instruction.successors.push_back(target);
                if (!instruction.text.starts_with("jmp ")) {
                    instruction.successors.push_back(address + result.length);
                    pending.push_back(target);
                }
            } else if (!terminates) {
                instruction.successors.push_back(address + result.length);
            }
            decodedAddresses.emplace(address, instructions.size());
            instructions.push_back(std::move(instruction));
            if (instructions.back().successors.empty()) break;
            // Conditional fallthrough first; ret only terminates its own path.
            address = instructions.back().successors.back();
        }
    }
    return true;
}

bool FindCreationMembers(const std::vector<NativeInstruction>& instructions,
                         size_t& factoryOffset, size_t& parentOffset, const std::string& owner) {
    enum class Kind { Unknown, Object, Member, Vtable, CreationMethod, Ambiguous };
    struct Value {
        Kind kind = Kind::Unknown;
        intptr_t offset = 0;
        bool mayCreate = false;
        bool operator==(const Value&) const = default;
    };
    using Registers = std::array<Value, 16>;
    const char* registerNames[16][5]{
        {"rax", "eax", "ax", "al", "ah"}, {"rcx", "ecx", "cx", "cl", "ch"},
        {"rdx", "edx", "dx", "dl", "dh"}, {"rbx", "ebx", "bx", "bl", "bh"},
        {"rsp", "esp", "sp", "spl", nullptr}, {"rbp", "ebp", "bp", "bpl", nullptr},
        {"rsi", "esi", "si", "sil", nullptr}, {"rdi", "edi", "di", "dil", nullptr},
        {"r8", "r8d", "r8w", "r8b", nullptr}, {"r9", "r9d", "r9w", "r9b", nullptr},
        {"r10", "r10d", "r10w", "r10b", nullptr}, {"r11", "r11d", "r11w", "r11b", nullptr},
        {"r12", "r12d", "r12w", "r12b", nullptr}, {"r13", "r13d", "r13w", "r13b", nullptr},
        {"r14", "r14d", "r14w", "r14b", nullptr}, {"r15", "r15d", "r15w", "r15b", nullptr},
    };
    auto registerIndex = [&](const std::string& operand) {
        for (int index = 0; index < 16; ++index) {
            for (const char* name : registerNames[index]) if (name && operand == name) return index;
        }
        return -1;
    };
    auto merge = [](Value left, Value right) {
        if (left == right) return left;
        return Value{Kind::Ambiguous, 0, left.mayCreate || right.mayCreate};
    };
    std::regex memoryLoad(R"((?:qword ptr )?\[(r\w+)(?:([+-])(0x[0-9a-f]+))?\])");
    std::map<size_t, bool> creationSlots;
    auto isCreationSlot = [&](size_t offset) {
        if (offset >= 0x10000 || offset % alignof(void*)) return false;
        auto [slot, inserted] = creationSlots.try_emplace(offset, false);
        if (inserted) {
            void* method = nullptr;
            slot->second = ReadProcessMemory(GetCurrentProcess(),
                static_cast<const std::byte*>(g_switchThumbnailFactoryVftable) + offset,
                &method, sizeof(method), nullptr) && method == g_createCustomPaintThumbnail;
        }
        return slot->second;
    };
    std::map<uintptr_t, size_t> addresses;
    for (size_t index = 0; index < instructions.size(); ++index) addresses.emplace(instructions[index].address, index);
    // Identify the owner-parent call sites. The special context passes the
    // adjacent-desktop argument instead; its stack spill is not a member offset.
    std::map<uintptr_t, size_t> ends;
    for (size_t index = 0; index < instructions.size(); ++index) ends.emplace(instructions[index].address + instructions[index].length, index);
    std::vector<std::optional<size_t>> callParents(instructions.size());
    std::regex parentLoad(R"(mov rdx, (?:qword ptr )?\[)" + owner + R"(\+(0x[0-9a-f]+)\])");
    std::regex thisArgument(R"(mov rcx, r\w+)");
    for (size_t index = 0; index < instructions.size(); ++index) {
        if (!instructions[index].text.starts_with("call ")) continue;
        auto method = ends.find(instructions[index].address);
        if (method == ends.end() || !instructions[method->second].text.starts_with("mov rax, ")) continue;
        auto self = ends.find(instructions[method->second].address);
        if (self == ends.end() || !std::regex_match(instructions[self->second].text, thisArgument)) continue;
        auto parent = ends.find(instructions[self->second].address);
        std::smatch load;
        if (parent != ends.end() && std::regex_match(instructions[parent->second].text, load, parentLoad)) callParents[index] = std::stoull(load[1], nullptr, 0);
    }
    std::vector<std::optional<Registers>> incoming(instructions.size());
    Registers entry{};
    entry[1].kind = Kind::Object; // rcx is the gesture object at entry.
    incoming[0] = entry;
    std::vector<size_t> pending{0};
    size_t transfers = 0;
    while (!pending.empty()) {
        if (++transfers > instructions.size() * 64) {
            Wh_Log(L"Factory register analysis budget exceeded");
            return false;
        }
        size_t index = pending.back();
        pending.pop_back();
        Registers state = *incoming[index];
        const std::string& text = instructions[index].text;
        size_t separator = text.find(' ');
        std::string opcode = text.substr(0, separator);
        std::string operands = separator == std::string::npos ? "" : text.substr(separator + 1);
        size_t comma = operands.find(", ");
        std::string destination = operands.substr(0, comma);
        std::string source = comma == std::string::npos ? "" : operands.substr(comma + 2);
        int targetRegister = registerIndex(destination);
        int sourceRegister = registerIndex(source);
        if (opcode == "call") {
            // Windows x64 calls preserve nonvolatile registers, not their arguments.
            for (int reg : {0, 1, 2, 8, 9, 10, 11}) state[reg] = {};
        } else if (opcode == "mov" && targetRegister >= 0) {
            Value value;
            std::smatch load;
            if (destination == registerNames[targetRegister][0]) {
                if (sourceRegister >= 0 && source == registerNames[sourceRegister][0]) {
                    value = state[sourceRegister];
                } else if (std::regex_match(source, load, memoryLoad)) {
                    int base = registerIndex(load[1]);
                    intptr_t offset = load[3].matched ? std::stoll(load[3], nullptr, 0) : 0;
                    if (load[2] == "-") offset = -offset;
                    if (base >= 0 && offset > -0x10000 && offset < 0x10000) {
                        Value pointer = state[base];
                        if (pointer.kind == Kind::Object && offset >= 0) value = {Kind::Member, offset, false};
                        else if (pointer.kind == Kind::Member && !offset) value = {Kind::Vtable, pointer.offset, false};
                        else if (pointer.kind == Kind::Vtable && offset >= 0 && isCreationSlot(offset)) value = {Kind::CreationMethod, pointer.offset, true};
                        else if (pointer.kind == Kind::Ambiguous) {
                            value = pointer;
                            value.mayCreate |= offset >= 0 && isCreationSlot(offset);
                        } else if (pointer.kind == Kind::Unknown && offset >= 0 && isCreationSlot(offset)) value.mayCreate = true;
                    }
                }
            }
            state[targetRegister] = value;
        } else if (opcode.starts_with("cmov") && targetRegister >= 0) {
            Value alternative = sourceRegister >= 0 && source == registerNames[sourceRegister][0] ? state[sourceRegister] : Value{};
            state[targetRegister] = destination == registerNames[targetRegister][0] ?
                merge(state[targetRegister], alternative) : Value{};
        } else if (opcode.starts_with("loop")) {
            state[1] = {}; // LOOP decrements rcx, unlike an ordinary conditional jump.
        } else if (opcode == "cmp" || opcode == "test" || opcode == "push" || opcode == "nop" ||
                   opcode.starts_with('j') || opcode.starts_with("ret") || opcode == "int3" || opcode == "ud2") {
            // These instructions do not write a tracked pointer register.
        } else if (opcode == "mul" || opcode == "imul" || opcode == "div" || opcode == "idiv" || opcode == "cpuid" ||
                   opcode == "xchg" || opcode == "xadd" || opcode.starts_with("cmpxchg")) {
            state = {}; // Implicit or multiple outputs are outside this narrow analysis.
        } else if (targetRegister >= 0) {
            state[targetRegister] = {}; // Includes partial-register writes and arithmetic.
        } else if (opcode == "mov" || opcode == "movaps" || opcode == "movups" ||
                   opcode == "movdqa" || opcode == "movdqu" || opcode == "xorps" ||
                   opcode == "and" || opcode == "or" || opcode == "xor" || opcode == "add" ||
                   opcode == "sub" || opcode == "inc" || opcode == "dec" || opcode == "lea") {
            // Memory and SIMD destinations do not replace the pointer registers.
        } else {
            state = {}; // Unknown side effects must not preserve a guessed definition.
        }
        for (uintptr_t successor : instructions[index].successors) {
            size_t next = addresses.at(successor);
            if (!incoming[next]) {
                incoming[next] = state;
                pending.push_back(next);
            } else {
                bool changed = false;
                for (size_t reg = 0; reg < state.size(); ++reg) {
                    Value combined = merge((*incoming[next])[reg], state[reg]);
                    if (combined != (*incoming[next])[reg]) { (*incoming[next])[reg] = combined; changed = true; }
                }
                if (changed) pending.push_back(next);
            }
        }
    }
    std::optional<intptr_t> factory;
    std::optional<intptr_t> parent;
    for (size_t index = 0; index < instructions.size(); ++index) {
        if (!callParents[index]) continue;
        const Registers& arguments = *incoming[index];
        const Value& method = arguments[0];
        if (!method.mayCreate) continue;
        if (method.kind == Kind::Ambiguous || arguments[1].kind == Kind::Ambiguous || arguments[2].kind == Kind::Ambiguous) {
            Wh_Log(L"Ambiguous thumbnail creation arguments at %p", reinterpret_cast<void*>(instructions[index].address));
            return false;
        }
        if (method.kind != Kind::CreationMethod || arguments[1].kind != Kind::Member || arguments[2].kind != Kind::Member) {
            Wh_Log(L"Unverified thumbnail creation arguments at %p", reinterpret_cast<void*>(instructions[index].address));
            return false;
        }
        if (arguments[1].offset != method.offset || arguments[2].offset != static_cast<intptr_t>(*callParents[index]) ||
            (factory && *factory != method.offset) || (parent && *parent != arguments[2].offset)) {
            Wh_Log(L"Conflicting factory or parent definitions at thumbnail creation");
            return false;
        }
        factory = method.offset;
        parent = arguments[2].offset;
    }
    if (!factory || !parent) {
        Wh_Log(L"Thumbnail creation arguments not recognized");
        return false;
    }
    factoryOffset = static_cast<size_t>(*factory);
    parentOffset = static_cast<size_t>(*parent);
    return true;
}

bool DiscoverGestureLayout(void* bitmapResult, void* initialize, void* addThumbnail,
                           GestureLayout& layout) {
    // Windhawk can retain its entry-point redirection after a hook is removed.
    // Decode the matching on-disk image, without running its code or following hooks.
    // Unwind boundaries and discovered member offsets still refer to the loaded DLL.
    DWORD64 imageBase = 0;
    if (!RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(bitmapResult), &imageBase, nullptr)) return false;
    wchar_t modulePath[MAX_PATH]{};
    DWORD pathLength = GetModuleFileNameW(reinterpret_cast<HMODULE>(imageBase), modulePath, ARRAYSIZE(modulePath));
    if (!pathLength || pathLength >= ARRAYSIZE(modulePath)) return false;
    struct NativeImage {
        HANDLE file = INVALID_HANDLE_VALUE;
        HANDLE mapping = nullptr;
        void* view = nullptr;
        ~NativeImage() {
            if (view) UnmapViewOfFile(view);
            if (mapping) CloseHandle(mapping);
            if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        }
    } native;
    native.file = CreateFileW(modulePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (native.file != INVALID_HANDLE_VALUE) {
        native.mapping = CreateFileMappingW(native.file, nullptr, PAGE_READONLY | SEC_IMAGE_NO_EXECUTE, 0, 0, nullptr);
        if (native.mapping) native.view = MapViewOfFile(native.mapping, FILE_MAP_READ, 0, 0, 0);
    }
    if (!native.view) {
        Wh_Log(L"Unable to map native shell code: %lu", GetLastError());
        return false;
    }
    std::string code[3];
    std::vector<NativeInstruction> graphs[3];
    void* functions[]{bitmapResult, initialize, addThumbnail};
    const wchar_t* methodNames[]{L"GetTrayBitmapResult", L"Initialize", L"AddThumbnailToThumbnailCollection"};
    const size_t limits[]{1024, 2048, 512};
    for (size_t index = 0; index < ARRAYSIZE(functions); ++index) {
        if (!ReadFunctionControlFlow(functions[index], limits[index], graphs[index], native.view)) {
            Wh_Log(L"Control-flow discovery failed for %s", methodNames[index]);
            return false;
        }
        uintptr_t previousEnd = 0;
        for (const auto& instruction : graphs[index]) {
            // Keep disjoint paths from forming an artificial multi-instruction match.
            if (previousEnd != instruction.address) code[index] += "#;";
            code[index] += instruction.text + ';';
            previousEnd = instruction.address + instruction.length;
            if (instruction.text.starts_with("jmp ") || instruction.text.starts_with("ret")) previousEnd = 0;
        }
    }
    auto offsetFrom = [](const std::string& instructions, const std::string& pattern,
                         size_t group = 1) -> std::optional<size_t> {
        std::smatch match;
        if (!std::regex_search(instructions, match, std::regex(pattern))) return std::nullopt;
        size_t offset = std::stoull(match[group], nullptr, 0);
        if (offset >= 0x10000) return std::nullopt;
        return offset;
    };

    std::smatch self;
    // Example: mov rsi, rcx. rsi holds the gesture object in subsequent accesses.
    if (!std::regex_search(code[0], self, std::regex(R"(mov (r\w+), rcx;)"))) {
        Wh_Log(L"Gesture object register not found");
        return false;
    }
    const std::string owner = self[1];
    const std::string ownerMember = R"(\[)" + owner + R"(\+(0x[0-9a-f]+)\])";

    // Example: mov rcx, [rdx+0x10]; xor ebx, ebx; test rcx, rcx.
    // The bitmap load and null check must use the same register.
    auto bitmap = offsetFrom(code[0],
        R"(mov (r\w+), (?:qword ptr )?\[rdx\+(0x[0-9a-f]+)\];(?:xor \w+, \w+;)?test \1, \1;)", 2);
    // Example: cmp [rsi+0x08], rbx. This checks the destination HWND for null.
    auto window = offsetFrom(code[0], R"(cmp (?:qword ptr )?)" + ownerMember + R"(, \w+;)");
    // Example: mov [rcx+0xE4], edx. Initialize stores the context-type argument.
    auto contextType = offsetFrom(code[1], R"(mov (?:dword ptr )?\[rcx\+(0x[0-9a-f]+)\], edx;)");
    if (!bitmap) {
        Wh_Log(L"Bitmap handle offset not found");
        return false;
    }
    if (!window) {
        Wh_Log(L"Gesture window offset not found");
        return false;
    }
    if (!contextType) {
        Wh_Log(L"Context-type offset not found");
        return false;
    }

    size_t factoryOffset = 0;
    size_t parentOffset = 0;
    if (!FindCreationMembers(graphs[0], factoryOffset, parentOffset, owner)) return false;
    std::optional<size_t> factory = factoryOffset;
    std::optional<size_t> parent = parentOffset;
    size_t customPaint = SIZE_MAX;
    std::smatch flag;
    // Example: lea r13, [rsi+0xE0]; ...; cmp [r13], bl.
    // The flag address must be followed by a byte comparison through it.
    if (std::regex_search(code[0], flag,
            std::regex(R"(lea (r\w+), \[)" + owner + R"(\+(0x[0-9a-f]+)\];)"))) {
        if (!std::regex_search(code[0],
                std::regex(R"(cmp (?:byte ptr )?\[)" + flag[1].str() + R"(\], (?:[abcd]l|r\d+b);)"))) {
            Wh_Log(L"Custom-paint flag comparison not found");
            return false;
        }
        customPaint = std::stoull(flag[2], nullptr, 0);
        if (customPaint >= 0x10000) {
            Wh_Log(L"Invalid custom-paint offset: 0x%zx", customPaint);
            return false;
        }
    } else {
        // Older builds have no custom-paint flag, only the context-type checks.
        if (std::regex_search(code[0], std::regex("byte ptr \\[" + owner))) {
            Wh_Log(L"Unrecognized custom-paint flag access");
            return false;
        }
        char typeOffset[32]{};
        snprintf(typeOffset, sizeof(typeOffset), "%zx", *contextType);
        std::regex typeBranch("cmp dword ptr \\[" + owner + "\\+0x0*" +
                              typeOffset + R"(\], 0x0*2;)");
        // Example: cmp dword ptr [r14+0xE0], 0x02. Require both mode checks.
        auto branches = std::distance(std::sregex_iterator(code[0].begin(), code[0].end(), typeBranch), std::sregex_iterator());
        if (branches != 2) {
            Wh_Log(L"Expected two context-type checks, found %td", branches);
            return false;
        }
    }

    // Windows owns both thumbnail arrays. Either can receive the adjacent desktop.
    std::smatch collectionOwner;
    std::smatch selectorRegister;
    // Example: mov rsi, rcx; mov ebx, edx. Some builds keep rcx and edx instead.
    const std::string collectionBase = std::regex_search(code[2], collectionOwner,
        std::regex(R"(mov (r\w+), rcx;)")) ? collectionOwner[1].str() : "rcx";
    const std::string selector = std::regex_search(code[2], selectorRegister,
        std::regex(R"(mov (e\w+), edx;)")) ? selectorRegister[1].str() : "edx";
    // Example: cmp ebx, 3; jnz ... . Equality selects the first array.
    auto firstCollectionSelector = offsetFrom(code[2], "cmp " + selector +
        R"(, (0x[0-9a-f]+|[0-9]+);(?:jne|jnz) )");
    if (!firstCollectionSelector) {
        Wh_Log(L"Thumbnail collection selector not found");
        return false;
    }
    std::smatch collectionTarget;
    // Example: lea rcx, [rsi+0x40] / lea rcx, [rsi+0x60].
    // Both branches use the same destination register for the array address.
    if (!std::regex_search(code[2], collectionTarget,
            std::regex(R"(lea (r\w+), \[)" + collectionBase + R"(\+0x[0-9a-f]+\];)"))) {
        Wh_Log(L"Thumbnail collection address calculation not found");
        return false;
    }
    std::regex collectionPattern("lea " + collectionTarget[1].str() + R"(, \[)" + collectionBase +
                                 R"(\+(0x[0-9a-f]+)\];)");
    std::vector<size_t> collections;
    for (auto it = std::sregex_iterator(code[2].begin(), code[2].end(), collectionPattern);
         it != std::sregex_iterator(); ++it) {
        size_t offset = std::stoull((*it)[1], nullptr, 0);
        if (collections.empty() || collections.back() != offset) collections.push_back(offset);
    }
    if (collections.size() != 2) {
        Wh_Log(L"Expected two thumbnail collection offsets, found %zu", collections.size());
        return false;
    }
    for (size_t offset : {*bitmap, *window, *factory, *parent, collections[0], collections[1]}) {
        if (!offset || offset >= 0x10000 || offset % alignof(void*)) {
            Wh_Log(L"Invalid pointer member offset: 0x%zx", offset);
            return false;
        }
    }
    if (*factory == *parent || collections[0] == collections[1] || customPaint == *contextType) {
        Wh_Log(L"Discovered member offsets overlap");
        return false;
    }
    layout = {*window, *bitmap, customPaint, *contextType, *factory, *parent,
              collections[0], collections[1], static_cast<unsigned int>(*firstCollectionSelector)};
    return true;
}

HRESULT GetTrayBitmapResultHook(void* gestureWindow, const void* bitmapData,
                               int desktopOffset, const RECT& bitmapRect,
                               void* thumbnails, IUnknown* adjacentDesktop) {
    auto* windowBytes = static_cast<std::byte*>(gestureWindow);
    HWND destination = *reinterpret_cast<HWND*>(windowBytes + g_gestureLayout.window);
    wchar_t destinationClass[64]{};
    GetClassNameW(destination, destinationClass, ARRAYSIZE(destinationClass));
    if (wcscmp(destinationClass, L"VirtualDesktopGestureSwitcher") != 0) {
        return g_getTrayBitmapResultOriginal(gestureWindow, bitmapData, desktopOffset,
                                            bitmapRect, thumbnails, adjacentDesktop);
    }
    HBITMAP bitmap = *reinterpret_cast<HBITMAP const*>(
        static_cast<const std::byte*>(bitmapData) + g_gestureLayout.bitmap);
    if (bitmap ||
        (thumbnails != windowBytes + g_gestureLayout.firstCollection &&
         thumbnails != windowBytes + g_gestureLayout.secondCollection) ||
        (g_gestureLayout.customPaint != SIZE_MAX &&
         *reinterpret_cast<const bool*>(windowBytes + g_gestureLayout.customPaint)) ||
        *reinterpret_cast<const int*>(windowBytes + g_gestureLayout.contextType) == 2) {
        return g_getTrayBitmapResultOriginal(gestureWindow, bitmapData, desktopOffset,
                                            bitmapRect, thumbnails, adjacentDesktop);
    }

    auto* factory = *reinterpret_cast<void**>(windowBytes + g_gestureLayout.factory);
    IUnknown* parent = *reinterpret_cast<IUnknown**>(windowBytes + g_gestureLayout.parent);
    // Check the concrete factory type without dereferencing an invalid candidate.
    void* factoryVftable = nullptr;
    bool verifiedFactory = factory && ReadProcessMemory(GetCurrentProcess(), factory,
        &factoryVftable, sizeof(factoryVftable), nullptr) &&
        factoryVftable == g_switchThumbnailFactoryVftable;
    HWND taskbar = g_getTrayWindowForMonitor(MonitorFromWindow(destination, MONITOR_DEFAULTTONEAREST));
    IUnknown* taskbarThumbnail = nullptr;
    RECT destinationRect{};
    HRESULT replacementResult = E_NOTIMPL;
    if (verifiedFactory && parent && taskbar && IsWindow(destination) && GetWindowRect(taskbar, &destinationRect)) {
        RECT sourceRect{0, 0, destinationRect.right - destinationRect.left,
                       destinationRect.bottom - destinationRect.top};
        if (sourceRect.right > 0 && sourceRect.bottom > 0) {
            // MapWindowPoints can return zero on success. Check last error then.
            SetLastError(ERROR_SUCCESS);
            int mapped = MapWindowPoints(nullptr, destination,
                                         reinterpret_cast<POINT*>(&destinationRect), 2);
            if (mapped != 0 || GetLastError() == ERROR_SUCCESS) {
                const GUID thumbnailInterface{0x47267895, 0xA4F8, 0x4012,
                                              {0x9E, 0x98, 0x08, 0x7C, 0x61, 0x9E, 0x7C, 0xFC}};
                replacementResult = g_createWindowThumbnail(
                    factory, parent, taskbar, 0, nullptr, &sourceRect, thumbnailInterface,
                    reinterpret_cast<void**>(&taskbarThumbnail));
                if (SUCCEEDED(replacementResult) && taskbarThumbnail) {
                    destinationRect.left += desktopOffset;
                    destinationRect.right += desktopOffset;
                    replacementResult = g_setThumbnailRect(taskbarThumbnail, destinationRect, 0, nullptr);
                    if (SUCCEEDED(replacementResult)) {
                        // Append to the callback's array, regardless of swipe direction.
                        unsigned int collectionSelector = g_gestureLayout.firstCollectionSelector;
                        if (thumbnails == windowBytes + g_gestureLayout.secondCollection) ++collectionSelector;
                        replacementResult = g_addThumbnailToCollection(
                            gestureWindow, collectionSelector, taskbarThumbnail);
                    }
                } else if (SUCCEEDED(replacementResult)) {
                    replacementResult = E_UNEXPECTED;
                }
            } else {
                replacementResult = HRESULT_FROM_WIN32(GetLastError());
            }
        }
    }
    // The native append AddRefs the thumbnail. Release our factory reference.
    if (taskbarThumbnail) taskbarThumbnail->Release();
    if (SUCCEEDED(replacementResult)) {
        Wh_Log(L"Replaced taskbar=%p collection=0x%zx offset=%d rect=(%ld,%ld,%ld,%ld)", taskbar,
               thumbnails == windowBytes + g_gestureLayout.firstCollection ?
                   g_gestureLayout.firstCollection : g_gestureLayout.secondCollection, desktopOffset,
               destinationRect.left, destinationRect.top, destinationRect.right, destinationRect.bottom);
        return S_OK;
    }
    Wh_Log(L"Thumbnail replacement failed: 0x%08lX", static_cast<unsigned long>(replacementResult));
    return g_getTrayBitmapResultOriginal(gestureWindow, bitmapData, desktopOffset,
                                        bitmapRect, thumbnails, adjacentDesktop);
}

BOOL Wh_ModInit() {
    HMODULE shell = GetModuleHandleW(L"twinui.pcshell.dll");
    HMODULE thumbnails = GetModuleHandleW(L"twinui.dll");
    // Load missing shell modules during early Explorer startup.
    // Keep the acquired references for the process lifetime; reuse on mod reload.
    if (!shell) shell = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!thumbnails) thumbnails = LoadLibraryExW(L"twinui.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!shell || !thumbnails) {
        Wh_Log(L"Shell modules unavailable: twinui.pcshell=%p twinui=%p", shell, thumbnails);
        return FALSE;
    }
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK shellSymbols[]{
        {{L"protected: long __cdecl VirtualDesktopGestureWindow::GetTrayBitmapResult(struct TrayBitmapForVDThumbnailData const &,int,struct Geometry::CRect const &,class CCoSimpleArray<class Microsoft::WRL::ComPtr<struct IDCompThumbnail>,4294967294,class CSimpleArrayStandardCompareHelper<class Microsoft::WRL::ComPtr<struct IDCompThumbnail> > > &,struct IDCompThumbnail *)",
          L"private: long __cdecl VirtualDesktopGestureWindow::GetTrayBitmapResult(struct TrayBitmapForVDThumbnailData const &,int,struct Geometry::CRect const &,class CCoSimpleArray<class Microsoft::WRL::ComPtr<struct IDCompThumbnail>,4294967294,class CSimpleArrayStandardCompareHelper<class Microsoft::WRL::ComPtr<struct IDCompThumbnail> > > &,struct IDCompThumbnail *)"}, &g_getTrayBitmapResultOriginal},
        {{L"public: long __cdecl VirtualDesktopGestureWindow::Initialize(unsigned long)"}, &g_initializeGestureWindow},
        {{L"public: long __cdecl VirtualDesktopGestureWindow::AddThumbnailToThumbnailCollection(unsigned int,struct IDCompThumbnail *)"}, &g_addThumbnailToCollection},
        {{L"struct HWND__ * __cdecl TaskbarHelpers::GetTrayWindowForMonitor(struct HMONITOR__ *)"}, &g_getTrayWindowForMonitor},
    };
    // twinui.dll
    WindhawkUtils::SYMBOL_HOOK thumbnailSymbols[]{
        {{L"const CSwitchThumbnailFactory::`vftable'"}, &g_switchThumbnailFactoryVftable},
        {{L"public: virtual long __cdecl CSwitchThumbnailFactory::CreateCustomPaintThumbnail(struct IDCompThumbnail *,struct tagSIZE,enum CustomPaintThumbnailFlags,struct _GUID const &,void * *)"}, &g_createCustomPaintThumbnail},
        {{L"public: virtual long __cdecl CSwitchThumbnailFactory::CreateThumbnail(struct IDCompThumbnail *,struct HWND__ *,unsigned long,struct tagSIZE const *,struct tagRECT *,struct _GUID const &,void * *)"}, &g_createWindowThumbnail},
        {{L"protected: virtual long __cdecl CDCompAbstractThumbnail::SetRect(struct tagRECT const &,enum DCompThumbnailAnimation,struct IUIAnimationStoryboardEventHandler2 *)"}, &g_setThumbnailRect},
    };
    if (!WindhawkUtils::HookSymbols(shell, shellSymbols, ARRAYSIZE(shellSymbols)) ||
        !WindhawkUtils::HookSymbols(thumbnails, thumbnailSymbols, ARRAYSIZE(thumbnailSymbols))) return FALSE;
    try {
        if (!DiscoverGestureLayout(reinterpret_cast<void*>(g_getTrayBitmapResultOriginal),
                reinterpret_cast<void*>(g_initializeGestureWindow),
                reinterpret_cast<void*>(g_addThumbnailToCollection), g_gestureLayout)) {
            return FALSE;
        }
    } catch (const std::exception& exception) {
        Wh_Log(L"Layout discovery failed: %S", exception.what());
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(g_getTrayBitmapResultOriginal,
            GetTrayBitmapResultHook, &g_getTrayBitmapResultOriginal)) return FALSE;
    Wh_Log(L"Initialized: window=0x%zx bitmap=0x%zx customPaint=0x%zx type=0x%zx factory=0x%zx parent=0x%zx collections=(0x%zx,0x%zx) firstSelector=%u",
           g_gestureLayout.window, g_gestureLayout.bitmap, g_gestureLayout.customPaint,
           g_gestureLayout.contextType, g_gestureLayout.factory, g_gestureLayout.parent,
           g_gestureLayout.firstCollection, g_gestureLayout.secondCollection, g_gestureLayout.firstCollectionSelector);
    return TRUE;
}
