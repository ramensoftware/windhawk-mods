// ==WindhawkMod==
// @id           visual-studio-anti-rich-header
// @name         Visual Studio Anti-Rich-Header
// @description  Prevent the Visual Studio linker from embedding the Rich header into new executables
// @version      1.1
// @author       m417z
// @github       https://github.com/m417z
// @twitter      https://twitter.com/m417z
// @homepage     https://m417z.com/
// @include      link.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Visual Studio Anti-Rich-Header

Prevent the Visual Studio linker from embedding the Rich header into new
executables.

![Screenshot](https://i.imgur.com/7ZeEfYK.png) \
*A Rich header example*
*/
// ==/WindhawkModReadme==

#include <algorithm>
#include <string_view>

using namespace std::string_view_literals;

template <typename ForwardIt1, typename ForwardIt2>
ForwardIt1 SearchSingleMatch(ForwardIt1 first,
                             ForwardIt1 last,
                             ForwardIt2 searchFirst,
                             ForwardIt2 searchLast) {
    // First match.
    auto pos = std::search(first, last, searchFirst, searchLast);
    if (pos == last) {
        return last;
    }

    // Look for another match after the first.
    auto next = std::search(std::next(pos), last, searchFirst, searchLast);
    if (next != last) {
        // More than one match.
        return last;
    }

    return pos;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

#ifdef _WIN64
    // mov dword ptr ds:[r14],68636952
    constexpr std::string_view targetBytes = "\x41\xC7\x06\x52\x69\x63\x68"sv;

    // xor eax,eax ; nops
    constexpr std::string_view targetPatch = "\x31\xC0\x90\x90\x90\x90\x90"sv;
#else
    // mov dword ptr ds:[ebx],68636952
    constexpr std::string_view targetBytes = "\xC7\x03\x52\x69\x63\x68"sv;

    // xor eax,eax ; nops
    constexpr std::string_view targetPatch = "\x31\xC0\x90\x90\x90\x90"sv;
#endif

    static_assert(targetBytes.size() == targetPatch.size());

    char* pbExecutable = (char*)GetModuleHandle(nullptr);

    IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)pbExecutable;
    IMAGE_NT_HEADERS* pNtHeader =
        (IMAGE_NT_HEADERS*)((char*)pDosHeader + pDosHeader->e_lfanew);
    IMAGE_SECTION_HEADER* pSectionHeader =
        (IMAGE_SECTION_HEADER*)((char*)&pNtHeader->OptionalHeader +
                                pNtHeader->FileHeader.SizeOfOptionalHeader);

    char* from = pbExecutable + pSectionHeader[0].VirtualAddress;
    char* to = from + pSectionHeader[0].SizeOfRawData;

    std::string_view searchRegion(from, to - from);
    auto searchPos = SearchSingleMatch(searchRegion.begin(), searchRegion.end(),
                                       targetBytes.begin(), targetBytes.end());
    if (searchPos != searchRegion.end()) {
        void* pos = (void*)&*searchPos;

        Wh_Log(L"Patching at %p", pos);

        DWORD dwOldProtect;
        VirtualProtect(pos, targetPatch.size(), PAGE_EXECUTE_READWRITE,
                       &dwOldProtect);
        memcpy(pos, targetPatch.data(), targetPatch.size());
        VirtualProtect(pos, targetPatch.size(), dwOldProtect, &dwOldProtect);
    } else {
        bool proceed =
            MessageBox(nullptr,
                       L"link.exe is incompatible, Rich header won't be "
                       L"removed. Proceed?",
                       L"Windhawk mod: Visual Studio Anti-Rich-Header",
                       MB_ICONWARNING | MB_TOPMOST | MB_YESNO) == IDYES;
        if (!proceed) {
            ExitProcess(1);
        }
    }

    return TRUE;
}
