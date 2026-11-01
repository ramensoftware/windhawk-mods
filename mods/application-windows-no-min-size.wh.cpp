// ==WindhawkMod==
// @id              application-windows-no-min-size
// @name            Application Windows: No Minimum Size
// @description     Allows supported application windows to resize below their standard minimum size
// @version         1.8.0
// @author          SaiLuminous
// @github          https://github.com/SaiLuminous
// @include         *
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Application Windows: No Minimum Size

Resize supported application windows below their usual minimum width and height.
Useful with tiling window managers or when arranging several windows on screen.

Based on [Explorer No Minimum Window Size](https://windhawk.net/mods/explorer-no-min-size)
by [Anixx](https://github.com/Anixx). This adaptation extends the original
Explorer-only approach to eligible top-level application windows.

## Usage

Enable the mod, then resize a supported window normally or with your window
manager. Existing windows and newly created windows are supported. Disable
other minimum-size mods that target the same applications.

The mod targets all processes by default. Use Windhawk's per-mod advanced
process inclusion/exclusion controls to restrict it to particular applications.
Explorer's desktop, taskbar, and helper windows are excluded; only its folder
window classes are eligible.

## File Pilot support

The optional **File Pilot minimum-size override** currently supports the
verified 64-bit File Pilot 0.8.5 build. It is disabled by default.
Support is planned to be updated alongside File Pilot releases as new builds
are checked. Updates are not automatically compatible: an unrecognized build
is left unchanged until its override has been updated and verified.

File Pilot needs specific support because it enforces its minimum size through
an internal correction that calls SetWindowPos, separately from the standard
Windows minimum-size message. The generic override cannot prevent that
correction. The optional hook suppresses only the identified correction call,
after checking the executable build and code signature; it does not patch
the executable on disk.

Enable this option in the mod's Settings tab. Other mods hooking the same
function may interfere with it. Very small windows may expose application
layout or rendering bugs.
## Compatibility and limitations

The standard override changes Windows' minimum tracking size. Applications
with independent layout constraints can still refuse smaller sizes. Contents
may clip, overlap, or become inaccessible in very small windows.

This mod does not make fixed-size dialogs resizable or make a window manager
manage excluded windows. Protected processes and applications excluded by
Windhawk are unaffected. Window-manager borders and padding are unchanged.

Disabling restores normal minimum-size handling without requiring you to resize
or close windows first. It does not actively resize or close windows.
Applications can apply their minimum on a subsequent resize.

Other applications may enforce independent minimum sizes that the standard override does not bypass. Very small windows can cause clipping or
application-specific rendering problems. Exclude incompatible applications
using Windhawk's per-mod advanced settings.
## Credits and license

Original mod: **Explorer No Minimum Window Size** by **Anixx**.
Adaptation: **[SaiLuminous](https://github.com/SaiLuminous)**.

Original source:
https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-no-min-size.wh.cpp

Distributed under the MIT License; the notice is included below.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- filePilotOverride: false
  $name: File Pilot minimum-size override (experimental)
  $description: Suppress the separate size-correction call in the inspected File Pilot 0.8.5 x64 build. Off by default. Other builds are left unchanged. Support is planned to be updated alongside File Pilot releases. Very small windows may clip content or expose application bugs.
*/
// ==/WindhawkModSettings==
// MIT License
//
// Copyright (c) Anixx
// Copyright (c) 2026 SaiLuminous
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#include <windhawk_utils.h>
#include <windows.h>

#include <atomic>
#include <cwchar>
#include <cstring>
#include <mutex>
#include <unordered_set>
#include <vector>

static bool g_isExplorer = false;
static std::atomic<bool> g_stopping{false};
static std::atomic<unsigned> g_attaching{0};
static std::mutex g_windowsMutex;
static std::unordered_set<HWND> g_windows;

static bool IsApplicationWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_NOACTIVATE)) return false;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;
    wchar_t cls[256] = {};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return false;
    // explorer.exe also hosts the desktop and long-lived invisible helpers.
    // Only its actual folder window classes belong in this mod.
    if (g_isExplorer && _wcsicmp(cls, L"CabinetWClass") != 0 &&
        _wcsicmp(cls, L"ExploreWClass") != 0) return false;
    const wchar_t* excluded[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"#32768", L"tooltips_class32", L"SysShadow", L"IME", L"MSCTFIME UI"
    };
    for (const auto* name : excluded) {
        if (_wcsicmp(cls, name) == 0) return false;
    }
    // Avoid non-interactive popup helpers; allow borderless main app windows.
    if ((exStyle & WS_EX_TOOLWINDOW) && !(style & (WS_CAPTION | WS_THICKFRAME)))
        return false;
    return true;
}

static LRESULT CALLBACK SubclassWndProc(HWND hwnd, UINT msg, WPARAM wp,
                                        LPARAM lp, DWORD_PTR) {
    if (msg == WM_NCDESTROY) {
        {
            std::lock_guard lock(g_windowsMutex);
            g_windows.erase(hwnd);
        }
        return DefSubclassProc(hwnd, msg, wp, lp);
    }
    LRESULT result = DefSubclassProc(hwnd, msg, wp, lp);
    // Run AFTER the application so its own minimum doesn't overwrite ours.
    if (msg == WM_GETMINMAXINFO && lp && !g_stopping.load() &&
        IsApplicationWindow(hwnd)) {
        auto* limits = reinterpret_cast<MINMAXINFO*>(lp);
        limits->ptMinTrackSize.x = 1;
        limits->ptMinTrackSize.y = 1;
    }

    return result;
}

static void Attach(HWND hwnd) {
    struct InFlight {
        InFlight() { ++g_attaching; }
        ~InFlight() { --g_attaching; }
    } inFlight;
    if (g_stopping.load() || !IsApplicationWindow(hwnd)) return;
    {
        std::lock_guard lock(g_windowsMutex);
        if (!g_windows.insert(hwnd).second) return;
    }
    // Never hold a mutex while synchronously sending to a window thread.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, SubclassWndProc, 0)) {
        std::lock_guard lock(g_windowsMutex);
        g_windows.erase(hwnd);
    }
}
static BOOL CALLBACK EnumAttach(HWND hwnd, LPARAM) {
    Attach(hwnd);
    return TRUE;
}
static decltype(&CreateWindowExW) OriginalCreateW;
static decltype(&CreateWindowExA) OriginalCreateA;
static decltype(&ShowWindow) OriginalShow;

static HWND WINAPI CreateWHook(DWORD ex, LPCWSTR cls, LPCWSTR title, DWORD style,
    int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    HWND hwnd = OriginalCreateW(ex, cls, title, style, x, y, w, h, parent, menu, instance, param);
    Attach(hwnd);
    return hwnd;
}
static HWND WINAPI CreateAHook(DWORD ex, LPCSTR cls, LPCSTR title, DWORD style,
    int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    HWND hwnd = OriginalCreateA(ex, cls, title, style, x, y, w, h, parent, menu, instance, param);
    Attach(hwnd);
    return hwnd;
}
static BOOL WINAPI ShowHook(HWND hwnd, int command) {
    Attach(hwnd);
    return OriginalShow(hwnd, command);
}
static std::atomic<bool> g_filePilotOverride{false};
static void* g_filePilotCorrectionReturn = nullptr;
static decltype(&SetWindowPos) OriginalSetWindowPos;

static BOOL WINAPI FilePilotSetWindowPosHook(HWND hwnd, HWND after,
                                            int x, int y, int cx, int cy, UINT flags) {
    // Caller-specific: other SetWindowPos calls and all external tiler calls pass through.
    if (g_filePilotOverride.load() && !g_stopping.load() &&
        __builtin_return_address(0) == g_filePilotCorrectionReturn &&
        flags == (SWP_NOMOVE | SWP_NOZORDER) && !after && x == 0 && y == 0 &&
        cx > 0 && cy > 0 && IsApplicationWindow(hwnd) &&
        !IsIconic(hwnd) && !IsZoomed(hwnd)) {
        return TRUE;
    }
    return OriginalSetWindowPos(hwnd, after, x, y, cx, cy, flags);
}

static bool InstallFilePilotHook() {
#ifdef _WIN64
    wchar_t path[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) return true;
    const wchar_t* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    if (_wcsicmp(name, L"FPilot.exe") != 0) return true;
    auto* base = reinterpret_cast<unsigned char*>(GetModuleHandleW(nullptr));
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return true;
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    // Fail closed on another build, even if it advertises the same version.
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        nt->FileHeader.TimeDateStamp != 0x6a9571db ||
        nt->OptionalHeader.SizeOfImage != 0x279000) {
        Wh_Log(L"File Pilot override unavailable: executable build does not match");
        return true;
    }
    static constexpr unsigned char signature[] = {
        0x41,0x8b,0x8e,0xa4,0x08,0x00,0x00,0x44,0x3b,0xc1,0x7c,0x04,
        0x3b,0xd1,0x7d,0x34,0x41,0x8b,0x86,0xa0,0x08,0x00,0x00,
        0x3b,0xd1,0xc7,0x44,0x24,0x30,0x06,0x00,0x00,0x00,
        0x0f,0x4f,0xca,0x44,0x3b,0xc0,0x89,0x4c,0x24,0x28,
        0x48,0x8b,0x0f,0x41,0x0f,0x4f,0xc0,0x45,0x33,0xc9,
        0x89,0x44,0x24,0x20,0x45,0x33,0xc0,0x33,0xd2,
        0xff,0x15,0xff,0x26,0x01,0x00
    };
    if (memcmp(base + 0x206f8d, signature, sizeof(signature)) != 0) {
        Wh_Log(L"File Pilot override unavailable: correction code does not match");
        return true;
    }
    g_filePilotCorrectionReturn = base + 0x206fd1;
    if (!WindhawkUtils::SetFunctionHook(SetWindowPos, FilePilotSetWindowPosHook,
                                        &OriginalSetWindowPos)) {
        Wh_Log(L"File Pilot correction hook failed");
        return false;
    }
    Wh_Log(L"File Pilot correction hook ready; setting enabled=%d",
           g_filePilotOverride.load());
#endif
    return true;
}

void Wh_ModSettingsChanged() {
    if (g_stopping.load()) return;
    g_filePilotOverride.store(Wh_GetIntSetting(L"filePilotOverride") != 0);
}
BOOL Wh_ModInit() {
    wchar_t processPath[MAX_PATH]{};
    DWORD pathLength = GetModuleFileNameW(nullptr, processPath, ARRAYSIZE(processPath));
    if (pathLength && pathLength < ARRAYSIZE(processPath)) {
        const wchar_t* name = wcsrchr(processPath, L'\\');
        g_isExplorer = _wcsicmp(name ? name + 1 : processPath, L"explorer.exe") == 0;
    }
    g_stopping.store(false);
    Wh_ModSettingsChanged();
    if (!InstallFilePilotHook()) return FALSE;
    return WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWHook, &OriginalCreateW) &&
           WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateAHook, &OriginalCreateA) &&
           WindhawkUtils::SetFunctionHook(ShowWindow, ShowHook, &OriginalShow);
}
void Wh_ModAfterInit() {
    EnumWindows(EnumAttach, 0);
}
void Wh_ModBeforeUninit() {
    g_stopping.store(true);
    // Finish attachments already in flight before taking the detach snapshot.
    while (g_attaching.load()) Sleep(1);
    std::vector<HWND> windows;
    {
        std::lock_guard lock(g_windowsMutex);
        windows.assign(g_windows.begin(), g_windows.end());
    }
    // Never hold the tracking mutex while sending to a window thread.
    for (HWND hwnd : windows) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, SubclassWndProc);
    }
}
void Wh_ModUninit() {
    std::lock_guard lock(g_windowsMutex);
    g_windows.clear();
}
