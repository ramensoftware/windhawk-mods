// ==WindhawkMod==
// @id              hide-insider-watermark
// @name            Hide Insider Preview Watermark
// @description     Hides the Insider Preview / Evaluation Copy watermark from the desktop corner.
// @version         1.0
// @author          Exiled Eye
// @github          https://github.com/ExiledEye
// @homepage        https://exiledeye.github.io/
// @donateUrl       https://ko-fi.com/exiled_eye
// @include         explorer.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Insider Preview Watermark

Hides the "Windows 11 Pro Insider Preview" and "Evaluation copy. Build XXXXX" text
from the bottom-right corner of the desktop on Windows Insider builds.

## How it works
Uses IAT hooking on `shell32.dll` — the same technique as Universal Watermark Disabler:
- Intercepts `LoadStringW` to suppress resource IDs 62000 and 62001 (the watermark strings loaded by shell32)
- Intercepts `ExtTextOutW` to suppress the actual GDI draw call for any watermark text
- Forces a desktop repaint on init so the watermark disappears immediately without restarting explorer

## Notes
- Should work in both Windows 10 and 11 (Windows 10 Insder Preview in 2026??).
- May need to manually restart `explorer.exe` process.

## Credits
- [pr701](https://github.com/pr701) for [universal-watermark-disabler](https://github.com/pr701/universal-watermark-disabler) used as reference.

*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <string>

// Watermaker detection helper

static const wchar_t* WATERMARK_FRAGMENTS[] = {
    L"Insider Preview",
    L"Evaluation copy",
    L"Evaluation Copy",
    L"Test Mode",
    L"Build ",
};

static bool IsWatermarkText(LPCWSTR text, UINT count) {
    if (!text || count == 0) return false;
    std::wstring s(text, count);
    for (auto& frag : WATERMARK_FRAGMENTS) {
        if (s.find(frag) != std::wstring::npos)
            return true;
    }
    return false;
}

// IAT patcher helper

static bool PatchIAT(HMODULE hModule, LPCSTR importDll, FARPROC original, FARPROC replacement) {
    auto base = (DWORD_PTR)hModule;
    auto dos  = (PIMAGE_DOS_HEADER)base;
    auto nt   = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
    auto imp  = (PIMAGE_IMPORT_DESCRIPTOR)(base +
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for (; imp->Name; imp++) {
        if (lstrcmpiA((LPCSTR)(base + imp->Name), importDll) != 0) continue;
        auto thunk = (DWORD_PTR*)(base + imp->FirstThunk);
        for (; *thunk; thunk++) {
            if (*thunk != (DWORD_PTR)original) continue;
            DWORD old;
            VirtualProtect(thunk, sizeof(DWORD_PTR), PAGE_EXECUTE_READWRITE, &old);
            *thunk = (DWORD_PTR)replacement;
            VirtualProtect(thunk, sizeof(DWORD_PTR), old, &old);
            return true;
        }
    }
    return false;
}

// Hooks

typedef int  (WINAPI* LoadStringW_t)(HINSTANCE, UINT, LPWSTR, int);
typedef BOOL (WINAPI* ExtTextOutW_t)(HDC, int, int, UINT, const RECT*, LPCWSTR, UINT, const INT*);

static LoadStringW_t orig_LoadStringW = nullptr;
static ExtTextOutW_t orig_ExtTextOutW = nullptr;

static int WINAPI Hook_LoadStringW(HINSTANCE hInst, UINT uID, LPWSTR lpBuf, int nMax) {
    // Resource IDs 62000 and 62001 are the watermark strings in shell32
    if (uID == 62000 || uID == 62001) {
        Wh_Log(L"Suppressed LoadStringW id=%u", uID);
        if (lpBuf && nMax > 0) lpBuf[0] = L'\0';
        return 0;
    }
    return orig_LoadStringW(hInst, uID, lpBuf, nMax);
}

static BOOL WINAPI Hook_ExtTextOutW(HDC hdc, int X, int Y, UINT opts,
    const RECT* lrc, LPCWSTR str, UINT count, const INT* dx)
{
    if (IsWatermarkText(str, count)) {
        Wh_Log(L"Suppressed ExtTextOutW: %.*s", count, str);
        return TRUE;
    }
    return orig_ExtTextOutW(hdc, X, Y, opts, lrc, str, count, dx);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing hide-insider-watermark v1.4");

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"shell32.dll not found");
        return FALSE;
    }

    // Set DLLs depending on the build
    static const char* loaderDlls[] = {
        "api-ms-win-core-libraryloader-l1-2-0.dll",
        "api-ms-win-core-libraryloader-l1-1-1.dll",
        "kernelbase.dll",
        "kernel32.dll",
    };
    for (auto& dll : loaderDlls) {
        HMODULE h = GetModuleHandleA(dll);
        if (!h) continue;
        FARPROC p = GetProcAddress(h, "LoadStringW");
        if (!p) continue;
        if (PatchIAT(hShell32, dll, p, (FARPROC)Hook_LoadStringW)) {
            orig_LoadStringW = (LoadStringW_t)p;
            Wh_Log(L"Patched LoadStringW via %S", dll);
            break;
        }
    }

    // ExtTextOutW may be imported from gdi32.dll or gdi32full.dll
    static const char* gdiDlls[] = { "gdi32.dll", "gdi32full.dll" };
    for (auto& dll : gdiDlls) {
        HMODULE h = GetModuleHandleA(dll);
        if (!h) continue;
        FARPROC p = GetProcAddress(h, "ExtTextOutW");
        if (!p) continue;
        if (PatchIAT(hShell32, dll, p, (FARPROC)Hook_ExtTextOutW)) {
            orig_ExtTextOutW = (ExtTextOutW_t)p;
            Wh_Log(L"Patched ExtTextOutW via %S", dll);
            break;
        }
    }

    // Force a desktop repaint so the watermark disappears immediately (hopefully)
    HWND hDesktop = GetShellWindow();
    if (hDesktop) InvalidateRect(hDesktop, nullptr, TRUE);

    Wh_Log(L"Init done — LoadStringW: %s, ExtTextOutW: %s",
        orig_LoadStringW ? L"OK" : L"not patched",
        orig_ExtTextOutW ? L"OK" : L"not patched");

    return TRUE;
}

void Wh_ModUninit() {
    // Restore IAT patches on unload so shell32 goes back to normal (explorer restart may still be required)
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) return;

    if (orig_LoadStringW) {
        static const char* loaderDlls[] = {
            "api-ms-win-core-libraryloader-l1-2-0.dll",
            "api-ms-win-core-libraryloader-l1-1-1.dll",
            "kernelbase.dll",
            "kernel32.dll",
        };
        for (auto& dll : loaderDlls)
            PatchIAT(hShell32, dll, (FARPROC)Hook_LoadStringW, (FARPROC)orig_LoadStringW);
    }

    if (orig_ExtTextOutW) {
        PatchIAT(hShell32, "gdi32.dll",     (FARPROC)Hook_ExtTextOutW, (FARPROC)orig_ExtTextOutW);
        PatchIAT(hShell32, "gdi32full.dll", (FARPROC)Hook_ExtTextOutW, (FARPROC)orig_ExtTextOutW);
    }

    Wh_Log(L"hide-insider-watermark v1.0 unloaded");
}
