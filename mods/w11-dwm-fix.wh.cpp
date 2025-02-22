// ==WindhawkMod==
// @id              w11-dwm-fix
// @name            Bring Back the Borders!
// @description     Restores borders, corners, shadows, and more!
// @version         3.0.0
// @author          teknixstuff
// @github          https://github.com/teknixstuff
// @twitter         https://twitter.com/teknixstuff
// @homepage        https://teknixstuff.github.io/
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bring Back the Borders!
This mod restores the W10 backdrop effects, borders, shadows, and acrylic.
Based on Ittr's uDWM patches.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

/*
#include <vector>
#define REL(addr, offset) ((addr + offset + 4) + *(int32_t*)(addr + offset))
#define OFF(addr, offset) addr = addr + offset;

static uintptr_t FindPattern(const char* signature, uintptr_t base_address, int Relative = 0, int Offset = 0)
{
    static auto patternToByte = [](const char* pattern)
    {
        auto bytes = std::vector<int>{};
        const auto start = const_cast<char*>(pattern);
        const auto end = const_cast<char*>(pattern) + strlen(pattern);
        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else { bytes.push_back(strtoul(current, &current, 16)); }
        }
        return bytes;
    };
    const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
    const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);
    const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = patternToByte(signature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);
    const auto s = patternBytes.size();
    const auto d = patternBytes.data();
    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
            if (Relative)
            {
                REL(address, Relative);
            }
            if (Offset)
            {
                OFF(address, Offset);
            }
            return address;
        }
    }
    return NULL;
}
*/

char* s_pDesktopManagerInstance;

bool g_inCTopLevelWindow_UpdateWindowVisuals = false;
bool g_inCLegacyNonClientBackground_SetBorderColor = false;

bool (*__cdecl IsHighContrastMode_Original)();
bool __cdecl IsHighContrastMode_Hook() {
  if (g_inCTopLevelWindow_UpdateWindowVisuals) {
    return true;
  }
  if (g_inCLegacyNonClientBackground_SetBorderColor) {
    return true;
  }
  return IsHighContrastMode_Original();
}

void (*__cdecl SetSuppressBorderUpdates_Original)(LPVOID a1this, char a2);

long (*__cdecl UpdateWindowVisuals_Original)(LPVOID a1this);
long __cdecl UpdateWindowVisuals_Hook(LPVOID a1this) {
    g_inCTopLevelWindow_UpdateWindowVisuals = true;
    SetSuppressBorderUpdates_Original(a1this, true);
    long ret = UpdateWindowVisuals_Original(a1this);
    g_inCTopLevelWindow_UpdateWindowVisuals = false;
    return ret;
}

long (*__cdecl UpdateNCAreaGeometry_Original)(LPVOID a1this);
long __cdecl UpdateNCAreaGeometry_Hook(LPVOID a1this) {
    char oldContrast = (*(char**)s_pDesktopManagerInstance)[0x1A];
    (*(char**)s_pDesktopManagerInstance)[0x1A] = 1;
    long ret = UpdateNCAreaGeometry_Original(a1this);
    (*(char**)s_pDesktopManagerInstance)[0x1A] = oldContrast;
    return ret;
}

long (*__cdecl SetBorderColor_Original)(LPVOID a1this, LPVOID a2);
long __cdecl SetBorderColor_Hook(LPVOID a1this, LPVOID a2) {
    g_inCLegacyNonClientBackground_SetBorderColor = true;
    long ret = SetBorderColor_Original(a1this, a2);
    g_inCLegacyNonClientBackground_SetBorderColor = false;
    return ret;
}

long (*__cdecl CalculateBackgroundType_Original)(LPVOID a1this);
long __cdecl CalculateBackgroundType_Hook(LPVOID a1this) {
    return 0;
}

bool (*__cdecl IsShadowNCAreaPart_Original)(int a1);
bool __cdecl IsShadowNCAreaPart_Hook(int a1) {
  return false;
}

struct ACCENT_POLICY {
  int AccentState;
  int AccentFlags;
  int GradientColor;
  int AnimationId;
};

long (*__cdecl UpdateAcrylicBlurBehind24H2_Original)(LPVOID a1this, ACCENT_POLICY *a2, unsigned int a3, const double *a4);
long __cdecl UpdateAcrylicBlurBehind24H2_Hook(LPVOID a1this, ACCENT_POLICY *a2, unsigned int a3, const double *a4) {
  ACCENT_POLICY fixedPolicy;
  fixedPolicy.AccentState = a2->AccentState;
  fixedPolicy.AccentFlags = a2->AccentFlags | 2;
  fixedPolicy.GradientColor = a2->GradientColor;
  fixedPolicy.AnimationId = a2->AnimationId;
  return UpdateAcrylicBlurBehind24H2_Original(a1this, &fixedPolicy, a3, a4);
}

long (*__cdecl UpdateAcrylicBlurBehind22H2_Original)(LPVOID a1this, ACCENT_POLICY *a2, unsigned int a3, bool a4, const double *a5);
long __cdecl UpdateAcrylicBlurBehind22H2_Hook(LPVOID a1this, ACCENT_POLICY *a2, unsigned int a3, bool a4, const double *a5) {
  ACCENT_POLICY fixedPolicy;
  fixedPolicy.AccentState = a2->AccentState;
  fixedPolicy.AccentFlags = a2->AccentFlags | 2;
  fixedPolicy.GradientColor = a2->GradientColor;
  fixedPolicy.AnimationId = a2->AnimationId;
  return UpdateAcrylicBlurBehind22H2_Original(a1this, &fixedPolicy, a3, a4, a5);
}

LPBYTE shadow = NULL;

DWORD GetWinBuild() {
    DWORD WinBuild;
    ((void (WINAPI*)(DWORD*, DWORD*, DWORD*))GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlGetNtVersionNumbers"))(NULL, NULL, &WinBuild);
    WinBuild &= ~0xF0000000;
    return WinBuild;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE udwm = GetModuleHandle(L"udwm.dll");
    if (!udwm) {
        Wh_Log(L"udwm.dll isn't loaded");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: static bool __cdecl CDesktopManager::IsHighContrastMode(void))"},
            (void**)&IsHighContrastMode_Original,
            (void*)IsHighContrastMode_Hook,
        },
        {
            {LR"(private: long __cdecl CTopLevelWindow::UpdateWindowVisuals(void))"},
            (void**)&UpdateWindowVisuals_Original,
            (void*)UpdateWindowVisuals_Hook
        },
        {
            {LR"(private: long __cdecl CTopLevelWindow::UpdateNCAreaGeometry(void))"},
            (void**)&UpdateNCAreaGeometry_Original,
            (void*)UpdateNCAreaGeometry_Hook
        },
        {
            {LR"(public: long __cdecl CLegacyNonClientBackground::SetBorderColor(struct _D3DCOLORVALUE const &))"},
            (void**)&SetBorderColor_Original,
            (void*)SetBorderColor_Hook
        },
        {
            {LR"(private: enum CTopLevelWindow::BackgroundType __cdecl CTopLevelWindow::CalculateBackgroundType(void)const )"},
            (void**)&CalculateBackgroundType_Original,
            (void*)CalculateBackgroundType_Hook
        },
        {
            {LR"(public: void __cdecl CTopLevelWindow::SetSuppressBorderUpdates(bool))"},
            (void**)&SetSuppressBorderUpdates_Original
        },
        {
            {LR"(private: static class CDesktopManager * CDesktopManager::s_pDesktopManagerInstance)"},
            (void**)&s_pDesktopManagerInstance
        },
    };

    WindhawkUtils::SYMBOL_HOOK symbolHooks22H2[] = {
        {
            {LR"(public: long __cdecl CAccentAcrylicBlurBehind::UpdateAcrylicBlurBehind(struct ACCENT_POLICY const &,unsigned long,bool,double const *))"},
            (void**)&UpdateAcrylicBlurBehind22H2_Original,
            (void*)UpdateAcrylicBlurBehind22H2_Hook
        }
    };

    WindhawkUtils::SYMBOL_HOOK symbolHooks24H2[] = {
        {
            {LR"(private: static bool __cdecl CTopLevelWindow::IsShadowNCAreaPart(unsigned int))"},
            (void**)&IsShadowNCAreaPart_Original,
            (void*)IsShadowNCAreaPart_Hook
        },
        {
            {LR"(public: long __cdecl CAccentAcrylicBlurBehind::UpdateAcrylicBlurBehind(struct ACCENT_POLICY const &,unsigned long,double const *))"},
            (void**)&UpdateAcrylicBlurBehind24H2_Original,
            (void*)UpdateAcrylicBlurBehind24H2_Hook
        }
    };

    if (!HookSymbols(udwm, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Failed to hook symbols");
        return FALSE;
    }

    if (GetWinBuild() >= 26100) {
        if (!HookSymbols(udwm, symbolHooks24H2, ARRAYSIZE(symbolHooks24H2))) {
            Wh_Log(L"Failed to hook 24H2-specific symbols");
            return FALSE;
        }
    }

    if (GetWinBuild() < 26100 && GetWinBuild() > 22000) {
        if (!HookSymbols(udwm, symbolHooks22H2, ARRAYSIZE(symbolHooks22H2))) {
            Wh_Log(L"Failed to hook 22H2-specific symbols");
            return FALSE;
        }
        const auto dosHeader = (PIMAGE_DOS_HEADER)udwm;
        const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)udwm + dosHeader->e_lfanew);
        const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        for (DWORD i = 0; i < sizeOfImage; i++) {
            if (((LPBYTE)udwm)[i] == 0x8D &&
                ((LPBYTE)udwm)[i+2] == 0xEE &&
                ((LPBYTE)udwm)[i+3] == 0x83 &&
                ((LPBYTE)udwm)[i+4] == 0xF8 &&
                ((LPBYTE)udwm)[i+5] == 0x03 &&
                ((LPBYTE)udwm)[i+6] == 0x77)
            {
                shadow = ((LPBYTE)udwm) + i;
                break;
            }
        }
        if (!shadow) {
            Wh_Log(L"Failed to find shadow check");
            return FALSE;
        }
        DWORD old;
        VirtualProtect(shadow+6, 1, PAGE_EXECUTE_READWRITE, &old);
        shadow[6] = 0xEB;
        VirtualProtect(shadow+6, 1, old, &old);
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (shadow) {
        DWORD old;
        VirtualProtect(shadow+6, 1, PAGE_EXECUTE_READWRITE, &old);
        shadow[6] = 0x77;
        VirtualProtect(shadow+6, 1, old, &old);
    }
}
