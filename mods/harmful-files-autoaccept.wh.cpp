// ==WindhawkMod==
// @id              harmful-files-autoaccept
// @name            Skip harmful files confirmation
// @description     Bypasses the Shell harmful-files query before dialog construction.
// @version         0.2.2
// @author          Arimodu
// @github          https://github.com/Arimodu
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Skip harmful files confirmation

![The harmful-files dialog crossed out with a red X](https://raw.githubusercontent.com/Arimodu/windhawk-mod-assets/main/harmful-files-preview.png)

Hooks shell32.dll's internal CSecurityZoneChecker::ShowSecurityZoneDialog
method. When the selected warning is the generic harmful-files confirmation,
the hook returns S_OK and IDOK before Windows constructs a dialog.

The warning is identified through Shell's action/policy table and string
resource IDs 50272, 50273 and 50274. Denial dialogs and other warning variants
continue through the original method. If the table layout cannot be verified,
the mod refuses to initialize.

This lets the file operation proceed despite the source's untrusted zone.
Tested on Windows 11 with Explorer drag-and-drop. The hook requires x64 Shell
symbols and a verified warning-table layout.

Internet zone settings and Zone.Identifier streams are unchanged. Disable the
mod to restore the warning. Windows DLL files are not modified.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_utils.h>
#include <cstdint>
#include <cstring>
#include <string>

struct WarningEntry {
    DWORD action;
    DWORD policy;
    ULONG_PTR heading;
    ULONG_PTR content;
    ULONG_PTR expanded;
    ULONG_PTR expandedControl;
    ULONG_PTR footer;
    ULONG_PTR buttons;
    ULONG_PTR buttonCount;
};
static_assert(sizeof(WarningEntry) == 64);
const WarningEntry* warningTable = nullptr;
using ShowDialog_t = HRESULT(__cdecl*)(void*, DWORD, DWORD, PCWSTR, HWND,
                                      IUnknown*, int*);
ShowDialog_t original = nullptr;

void Trace(const std::wstring& text) {
    Wh_Log(L"%s", text.c_str());
}

const WarningEntry* SelectWarning(DWORD action, DWORD policy) {
    for (unsigned i = 0; i < 10; ++i) {
        if (warningTable[i].action == action && warningTable[i].policy == policy)
            return &warningTable[i];
    }
    return &warningTable[policy == 1 ? 0 : 1];
}

HRESULT __cdecl ShowDialog_Hook(void* self, DWORD action, DWORD policy,
                                PCWSTR path, HWND owner, IUnknown* unknown,
                                int* result) {
    const auto entry = SelectWarning(action, policy);
    if (policy == 1 && result && entry->heading == 50272 &&
        entry->content == 50273 && entry->footer == 50274) {
        *result = IDOK;
        Trace(L"BYPASS action=" + std::to_wstring(action) + L" policy=" +
              std::to_wstring(policy) + L" hr=0 result=1; no dialog constructed");
        return S_OK;
    }
    Trace(L"FORWARD action=" + std::to_wstring(action) + L" policy=" +
          std::to_wstring(policy));
    return original(self, action, policy, path, owner, unknown, result);
}

const WarningEntry* FindTable(HMODULE module, void* function) {
    auto base = reinterpret_cast<BYTE*>(module);
    auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    ULONG_PTR end = reinterpret_cast<ULONG_PTR>(base) + nt->OptionalHeader.SizeOfImage;
    auto cursor = static_cast<BYTE*>(function);
    for (unsigned offset = 0; offset < 192;) {
        WH_DISASM_RESULT instruction{};
        if (!Wh_Disasm(cursor + offset, &instruction) || !instruction.length) break;
        auto p = cursor + offset;
        if (instruction.length == 7 && p[0] == 0x4C && p[1] == 0x8D && p[2] == 0x0D) {
            int32_t displacement;
            std::memcpy(&displacement, p + 3, sizeof(displacement));
            ULONG_PTR address = reinterpret_cast<ULONG_PTR>(p + 7) + displacement;
            if (address < reinterpret_cast<ULONG_PTR>(base) ||
                address > end - 10 * sizeof(WarningEntry)) return nullptr;
            auto table = reinterpret_cast<const WarningEntry*>(address);
            if (table[0].action != 0 || table[0].policy != 1 ||
                table[0].heading != 50272 || table[0].content != 50273 ||
                table[0].footer != 50274 || table[1].action != 0 ||
                table[1].policy != 3 || table[1].heading != 50269) return nullptr;
            for (unsigned i = 0; i < 10; ++i) {
                if ((table[i].policy != 1 && table[i].policy != 3) ||
                    table[i].heading < 50240 || table[i].heading > 50280)
                    return nullptr;
            }
            return table;
        }
        offset += static_cast<unsigned>(instruction.length);
    }
    return nullptr;
}

BOOL Wh_ModInit() {
    HMODULE shell = GetModuleHandleW(L"shell32.dll");
    if (!shell) return FALSE;
    void* target = nullptr;
    // shell32.dll
    const WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"public: virtual long __cdecl CSecurityZoneChecker::ShowSecurityZoneDialog(unsigned long,unsigned long,unsigned short const *,struct HWND__ *,struct IUnknown *,int *)"},
         &target, nullptr},
    };
    if (!WindhawkUtils::HookSymbols(shell, symbols, ARRAYSIZE(symbols)))
        return FALSE;
    if (!target || !(warningTable = FindTable(shell, target))) {
        Trace(L"INIT FAILED: symbol/table validation");
        return FALSE;
    }
    BOOL ok = Wh_SetFunctionHook(target, reinterpret_cast<void*>(ShowDialog_Hook),
                                 reinterpret_cast<void**>(&original));
    Trace(ok ? L"INIT 0.2.2: internal Shell bypass installed; table validated" :
               L"INIT FAILED: hook registration");
    return ok;
}