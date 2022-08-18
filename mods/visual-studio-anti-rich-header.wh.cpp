// ==WindhawkMod==
// @id           visual-studio-anti-rich-header
// @name         Visual Studio Anti-Rich-Header
// @description  Prevent the Visual Studio linker from embedding the Rich header into new executables
// @version      1.0.2
// @author       m417z
// @github       https://github.com/m417z
// @twitter      https://twitter.com/m417z
// @homepage     https://m417z.com/
// @include      link.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Visual Studio Anti-Rich-Header

Prevent the Visual Studio linker from embedding the Rich header into new executables.
*/
// ==/WindhawkModReadme==

#include <regex>
#include <string>
#include <string_view>

using namespace std::string_literals;

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

#ifdef _WIN64
    std::string targetRegex =
        R"(\x41\x8B\xC7)"                         // mov eax,r15d
        R"(\x49\x8B\x73\x28)"                     // mov rsi,qword ptr ds:[r11+28]
        R"(\x49\x8B\x7B\x30)"                     // mov rdi,qword ptr ds:[r11+30]
        R"(\x4D\x8B\x63\x38)"                     // mov r12,qword ptr ds:[r11+38]
        R"(\x41\xC7\x06\x52\x69\x63\x68\x49\x8B)" // mov dword ptr ds:[r14],68636952
        ;
    std::string targetPatch = "\x31\xC0\x90"s; // xor eax,eax ; nop
#else
    std::string targetRegex =
        R"(\x8B\x44\x24.)"            // mov eax,dword ptr ss:[esp+??]
        R"(\x5F)"                     // pop edi
        R"(\x5E)"                     // pop esi
        R"(()"
        // VS 2019
        R"(\xC7\x03\x52\x69\x63\x68)" // mov dword ptr ds:[ebx],68636952
        R"(\x89\x4B\x04)"             // mov dword ptr ds:[ebx+4],ecx
        R"(|)"
        // VS 2022
        R"(\x89\x59\x04)"             // mov dword ptr ds:[ecx+4],ebx
        R"(\xC7\x01\x52\x69\x63\x68)" // mov dword ptr ds:[ecx],68636952
        R"())"
        R"(\x5B)"                     // pop ebx
        ;
    std::string targetPatch = "\x31\xC0\x90\x90"s; // xor eax,eax ; nop ; nop
#endif

    char* pbExecutable = (char*)GetModuleHandle(nullptr);

    IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)pbExecutable;
    IMAGE_NT_HEADERS* pNtHeader = (IMAGE_NT_HEADERS*)((char*)pDosHeader + pDosHeader->e_lfanew);
    IMAGE_SECTION_HEADER* pSectionHeader = (IMAGE_SECTION_HEADER*)((char*)&pNtHeader->OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

    char* from = pbExecutable + pSectionHeader[0].VirtualAddress;
    char* to = from + pSectionHeader[0].SizeOfRawData;

    std::string_view search(from, to - from);
    std::match_results<std::string_view::const_iterator> match;
    std::regex regex(ReplaceAll(targetRegex, ".", R"([\s\S])"));
    if (std::regex_search(search.begin(), search.end(), match, regex)) {
        auto pos = from + match.position(0);

        DWORD dlOldProtect;
        VirtualProtect(pos, targetPatch.size(), PAGE_EXECUTE_READWRITE, &dlOldProtect);
        memcpy(pos, targetPatch.data(), targetPatch.size());
        VirtualProtect(pos, targetPatch.size(), dlOldProtect, &dlOldProtect);
    }
    else {
        bool proceed = MessageBox(
            nullptr,
            L"link.exe is incompatible, Rich header won't be removed. Proceed?",
            L"Windhawk mod: Visual Studio Anti-Rich-Header",
            MB_ICONWARNING | MB_TOPMOST | MB_YESNO
        ) == IDYES;
        if (!proceed) {
            ExitProcess(1);
        }
    }

    return TRUE;
}
