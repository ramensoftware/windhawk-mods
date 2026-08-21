// ==WindhawkMod==
// @id              microslop
// @name            Microslop
// @description     Replaces "Microsoft" with "Microslop" in system apps
// @version         1.0
// @author          gameknight963
// @github          https://github.com/Gameknight963
// @include         winver.exe
// @include         explorer.exe
// @include         SystemSettings.exe
// @include         StartMenuExperienceHost.exe
// @include         Taskmgr.exe
// @compilerOptions -lgdi32 -ldwrite
// ==/WindhawkMod==

// DWrite vtable hooking technique permanently borrowed from m417z's text-replace mod
// https://windhawk.net/mods/text-replace

// ==WindhawkModReadme==
/*
  ## Microslop

  Replace Microsoft with Microslop because funny

  Settings has tons of awesome replacement options as well.

  DWrite vtable hooking technique borrowed (forever) from m417z's 
  [text-replace](https://windhawk.net/mods/text-replace) mod

  ### best screenshots (all options enabled)

  ![settings page](https://i.imgur.com/eh5yaz1.png)

  ![winver](https://i.imgur.com/oXbFoBF.png)

  ![start menu](https://i.imgur.com/Iz1viC9.png)

  _note: much of the Start Menu is web-based in 
Windows 11, and that's not possible to replace._

  ![context mnenu](https://i.imgur.com/P6I0GxD.png)

_not really the intention I had lol_
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- behaviour:
  - skipFilePaths: true
    $name: Skip file paths
    $description: Tries to skip strings that look like file paths. Not foolproof though.

  - enableExplorer: false
    $name: Replace in Explorer
    $description: Additionally replaces text in explorer.exe, but can make file paths confusing. 
  $name: Behaviour

- replacements:
  - microsoft: true
    $name: Microsoft
    $description: Replaces "Microsoft" with "Microslop"

  - windows: false
    $name: Windows
    $description: Replaces "Windows" with "Winslop"

  - msEdge: false
    $name: Microsoft Edge
    $description: Replaces "Microsoft Edge" with "Microsoft Edging"

  - msDefender: false
    $name: Microsoft Defender
    $description: Replaces "Microsoft Defender" with "Microsoft Offender"

  - winSecurity: false
    $name: Windows Security
    $description: Replaces "Windows Security" with "Windows's Insecurities"

  - security: false
    $name: Security
    $description: Replaces "Security" with "Illusion"

  - recommended: false
    $name: Recommended
    $description: Replaces "Recommended" with "Forced"

  $name: Replacements
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <algorithm>
#include <string>
#include <d2d1.h>
#include <dwrite.h>

static bool g_skipFilePaths = true;
static bool g_enableExplorer = false;
static bool g_replaceMicrosoft = true;
static bool g_replaceWindows = false;
static bool g_replaceMicrosoftEdge = false;
static bool g_replaceMsDefender = false;
static bool g_replaceWinSecurity = false;
static bool g_replaceSecurity = false;
static bool g_replaceRecommended = false;

static bool g_isExplorer = false; 

static void LoadSettings() {
    g_enableExplorer = Wh_GetIntSetting(L"behaviour.enableExplorer");
    g_skipFilePaths = Wh_GetIntSetting(L"behaviour.skipFilePaths");
    g_replaceMicrosoft = Wh_GetIntSetting(L"replacements.microsoft");
    g_replaceWindows = Wh_GetIntSetting(L"replacements.windows");
    g_replaceMicrosoftEdge = Wh_GetIntSetting(L"replacements.msEdge");
    g_replaceMsDefender = Wh_GetIntSetting(L"replacements.msDefender");
    g_replaceWinSecurity = Wh_GetIntSetting(L"replacements.winSecurity");
    g_replaceSecurity = Wh_GetIntSetting(L"replacements.security");
    g_replaceRecommended = Wh_GetIntSetting(L"replacements.recommended");
}

static bool LooksLikeFilePath(const std::wstring& s) {
    // Drive letter path C:\ or C:/
    if (s.size() >= 3 && s[1] == L':' && (s[2] == L'\\' || s[2] == L'/'))
        return true;
    // UNC path \\server or //server
    if (s.size() >= 2 && ((s[0] == L'\\' && s[1] == L'\\') || (s[0] == L'/' && s[1] == L'/')))
        return true;
    // Backslash sequence that looks like a path segment
    if (s.find(L":\\") != std::wstring::npos || s.find(L"C:\\") != std::wstring::npos)
        return true;
    return false;
}

static std::wstring PatchString(const wchar_t* str, size_t len = (size_t)-1) {
    if (!str) return {};
    if (len == (size_t)-1) len = wcslen(str);
    std::wstring s(str, len);

    if (g_isExplorer && !g_enableExplorer) return s;

    if (g_skipFilePaths && LooksLikeFilePath(s)) return s;

    auto replace = [&](const std::wstring& from, const std::wstring& to) {
        size_t pos = 0;
        while (pos < s.size()) {
            auto it = std::search(s.begin() + pos, s.end(), from.begin(), from.end(),
                [](wchar_t a, wchar_t b) { return towlower(a) == towlower(b); });
            if (it == s.end()) break;
            pos = it - s.begin();
            s.replace(pos, from.size(), to);
            pos += to.size();
        }
    };

    if (g_replaceMicrosoftEdge) replace(L"Microsoft Edge", L"Microsoft Edging");
    if (g_replaceMsDefender) replace(L"Microsoft Defender", L"Microsoft Offender");
    if (g_replaceMicrosoft) replace(L"Microsoft", L"Microslop");
    if (g_replaceWinSecurity) replace(L"Windows Security", L"Windows's Insecurities");
    if (g_replaceSecurity) replace(L"Security", L"Illusion");
    if (g_replaceSecurity) replace(L"Recommended", L"Forced");
    if (g_replaceWindows) replace(L"Windows",   L"Winslop");

    return s;
}

using SetWindowTextW_t = decltype(&SetWindowTextW);
SetWindowTextW_t pOrigSetWindowTextW;
BOOL WINAPI SetWindowTextWHook(HWND hWnd, LPCWSTR s) {
    if (s) { auto p = PatchString(s); return pOrigSetWindowTextW(hWnd, p.c_str()); }
    return pOrigSetWindowTextW(hWnd, s);
}

using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t pOrigDrawTextW;
int WINAPI DrawTextWHook(HDC hdc, LPCWSTR s, int c, LPRECT r, UINT fmt) {
    if (s) { auto p = PatchString(s, c); int l = p.size(); if (fmt & DT_MODIFYSTRING) p.resize(l+4); return pOrigDrawTextW(hdc, p.c_str(), l, r, fmt); }
    return pOrigDrawTextW(hdc, s, c, r, fmt);
}

using ExtTextOutW_t = decltype(&ExtTextOutW);
ExtTextOutW_t pOrigExtTextOutW;
BOOL WINAPI ExtTextOutWHook(HDC hdc, int x, int y, UINT opt, CONST RECT* r, LPCWSTR s, UINT c, CONST INT* dx) {
    if (!(opt & ETO_GLYPH_INDEX) && s) { auto p = PatchString(s, c); return pOrigExtTextOutW(hdc, x, y, opt, r, p.c_str(), p.size(), p.size() != c ? nullptr : dx); }
    return pOrigExtTextOutW(hdc, x, y, opt, r, s, c, dx);
}

using IDWriteFactory_CreateTextLayout_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteFactory*, const WCHAR*, UINT32, IDWriteTextFormat*,
    FLOAT, FLOAT, IDWriteTextLayout**);
IDWriteFactory_CreateTextLayout_t pOrigCreateTextLayout;

HRESULT STDMETHODCALLTYPE CreateTextLayoutHook(
    IDWriteFactory* pThis, const WCHAR* s, UINT32 len,
    IDWriteTextFormat* fmt, FLOAT w, FLOAT h, IDWriteTextLayout** out) {
    if (s) { auto p = PatchString(s, len); return pOrigCreateTextLayout(pThis, p.c_str(), p.size(), fmt, w, h, out); }
    return pOrigCreateTextLayout(pThis, s, len, fmt, w, h, out);
}

static void HookDWrite() {
    HMODULE hDWrite = LoadLibraryExW(L"dwrite.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hDWrite) return;

    using DWriteCreateFactory_t = HRESULT(WINAPI*)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
    auto pCreate = (DWriteCreateFactory_t)GetProcAddress(hDWrite, "DWriteCreateFactory");
    if (!pCreate) return;

    IDWriteFactory* pFactory = nullptr;
    if (FAILED(pCreate(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&pFactory))) return;

    void** vtable = *(void***)pFactory;
    Wh_SetFunctionHook(vtable[18], (void*)CreateTextLayoutHook, (void**)&pOrigCreateTextLayout);
    pFactory->Release();
}

BOOL Wh_ModInit() {
    Wh_Log(L"Microslop init");
    LoadSettings();

    WCHAR path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    g_isExplorer = !!wcsstr(path, L"explorer.exe");
    
    Wh_SetFunctionHook((void*)SetWindowTextW, (void*)SetWindowTextWHook, (void**)&pOrigSetWindowTextW);
    Wh_SetFunctionHook((void*)DrawTextW, (void*)DrawTextWHook, (void**)&pOrigDrawTextW);
    Wh_SetFunctionHook((void*)ExtTextOutW, (void*)ExtTextOutWHook, (void**)&pOrigExtTextOutW);

    HookDWrite();

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}