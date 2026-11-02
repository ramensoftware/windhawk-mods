// ==WindhawkMod==
// @id              application-windows-no-min-size
// @name            Application Windows: No Minimum Size
// @description     Allows supported application windows to resize below their standard minimum size
// @version         1.8.6
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

![Window resizing demonstration](https://i.imgur.com/iYyPe1o.gif)


## Usage

Enable the mod, then resize a supported window normally or with your window
manager. Disable other minimum-size mods that target the same applications.

The mod targets all processes by default. Use Windhawk's per-mod advanced
process inclusion/exclusion controls to restrict it to particular applications.
Explorer's desktop, taskbar, and helper windows are excluded; only its folder
window classes are eligible.

## File Pilot support

The optional **File Pilot minimum-size override** is disabled by default.
Its code pattern was verified against 64-bit File Pilot 0.8.5. Other versions
are eligible only when the same pattern matches uniquely; they have not been
runtime-tested.

Support is planned to be updated alongside File Pilot releases as new builds
are checked. The scan tolerates changed object offsets and call displacements.
Missing or ambiguous patterns leave the override disabled.

File Pilot needs specific support because it enforces its minimum through an
internal resize correction, separately from Windows' standard minimum-size
message. The optional hook suppresses only the identified correction call.
It does not patch the executable on disk.

Enable the option in the Settings tab. Other mods hooking the same function
may interfere with it.

## Disabling or updating

The mod restores normal minimum-size handling. It requests a normal close for
visible, non-minimized, non-maximized windows below their observed minimum.
Windows with no observed minimum cannot be detected by this check.

If a requested window remains open after three seconds, a popup asks you to
save your work and close the app. Save prompts and cancellation are respected.
No process is force-terminated.

Other windows are left open unless callback removal times out. Blocked
attachment or callback cleanup can also trigger a close request and a popup,
regardless of window size. The popup explains which step is blocked.

This fallback cannot detect a later wait inside Windhawk's own hook removal.
Applications may enforce their minimum again on a subsequent resize.

## Compatibility and limitations

Applications with independent layout constraints can still refuse smaller
sizes. Contents may clip, overlap, or become inaccessible in very small windows.
Exclude incompatible applications using Windhawk's per-mod advanced settings.

The mod does not make fixed-size dialogs resizable or make a window manager
manage excluded windows. Protected processes and applications excluded by
Windhawk are unaffected. Window-manager borders and padding are unchanged.

Existing eligible windows are handled at startup. Windows created through
unhooked APIs, or made eligible by a later style change, may not be handled
until their next ShowWindow call.

## Credits and license

Original mod: **Explorer No Minimum Window Size** by **Anixx**.
Adaptation: **[SaiLuminous](https://github.com/SaiLuminous)**.

[Original source](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-no-min-size.wh.cpp)

Distributed under the MIT License; the notice is included below.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- filePilotOverride: false
  $name: File Pilot minimum-size override (experimental)
  $description: Finds a unique minimum-size correction pattern in 64-bit File Pilot. Off by default. Verified against 0.8.5; missing or ambiguous patterns disable the override. Other releases are not guaranteed compatible.
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
#include <unordered_map>
#include <algorithm>
#include <vector>

static wchar_t g_processName[MAX_PATH]{};
static bool g_isExplorer = false;
static std::atomic<bool> g_stopping{false};
static std::atomic<unsigned> g_attaching{0};
static std::mutex g_windowsMutex;
static std::unordered_set<HWND> g_windows;
static std::unordered_map<HWND, SIZE> g_minimums;

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
        g_minimums.erase(hwnd);
        }
        return DefSubclassProc(hwnd, msg, wp, lp);
    }
    LRESULT result = DefSubclassProc(hwnd, msg, wp, lp);
    // Run AFTER the application so its own minimum doesn't overwrite ours.
    if (msg == WM_GETMINMAXINFO && lp && !g_stopping.load() &&
        IsApplicationWindow(hwnd)) {
        auto* limits = reinterpret_cast<MINMAXINFO*>(lp);
        {
            std::lock_guard lock(g_windowsMutex);
            g_minimums[hwnd] = SIZE{limits->ptMinTrackSize.x, limits->ptMinTrackSize.y};
        }
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
        g_minimums.erase(hwnd);
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
        RECT current{};
        if (GetWindowRect(hwnd, &current)) {
            std::lock_guard lock(g_windowsMutex);
            auto& minimum = g_minimums[hwnd];
            if (cx > current.right - current.left) minimum.cx = cx;
            if (cy > current.bottom - current.top) minimum.cy = cy;
        }
        return TRUE;
    }
    return OriginalSetWindowPos(hwnd, after, x, y, cx, cy, flags);
}

static bool InstallFilePilotHook(const wchar_t* processName) {
    if (_wcsicmp(processName, L"FPilot.exe") != 0) return true;
#ifdef _WIN64
    auto* base = reinterpret_cast<unsigned char*>(GetModuleHandleW(nullptr));
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return true;
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        return true;
    }
    // Negative entries are wildcards. Match the minimum-size clamp followed
    // by SetWindowPos(hwnd, nullptr, 0, 0, width, height, SWP_NOMOVE|SWP_NOZORDER).
    // Object-field offsets and the RIP-relative IAT displacement may change
    // between builds without changing this instruction sequence.
    static constexpr int signature[] = {
        0x41,0x8b,0x8e,-1,-1,-1,-1,0x44,0x3b,0xc1,0x7c,0x04,
        0x3b,0xd1,0x7d,0x34,0x41,0x8b,0x86,-1,-1,-1,-1,
        0x3b,0xd1,0xc7,0x44,0x24,0x30,0x06,0x00,0x00,0x00,
        0x0f,0x4f,0xca,0x44,0x3b,0xc0,0x89,0x4c,0x24,0x28,
        0x48,0x8b,0x0f,0x41,0x0f,0x4f,0xc0,0x45,0x33,0xc9,
        0x89,0x44,0x24,0x20,0x45,0x33,0xc0,0x33,0xd2,
        0xff,0x15,-1,-1,-1,-1
    };
    constexpr size_t patternLength = ARRAYSIZE(signature);
    const unsigned char* match = nullptr;
    size_t matchCount = 0;
    const auto* sections = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const auto& section = sections[i];
        if (memcmp(section.Name, ".text\0\0\0", IMAGE_SIZEOF_SHORT_NAME) != 0 ||
            !(section.Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;
        size_t start = section.VirtualAddress;
        size_t length = section.Misc.VirtualSize;
        size_t imageSize = nt->OptionalHeader.SizeOfImage;
        if (start >= imageSize || length > imageSize - start ||
            length < patternLength) continue;
        for (size_t offset = 0; offset <= length - patternLength; ++offset) {
            const auto* candidate = base + start + offset;
            bool matches = true;
            for (size_t j = 0; j < patternLength; ++j) {
                if (signature[j] >= 0 && candidate[j] != signature[j]) {
                    matches = false;
                    break;
                }
            }
            if (!matches) continue;
            match = candidate;
            if (++matchCount > 1) {
                Wh_Log(L"File Pilot override unavailable: ambiguous correction pattern");
                return true;
            }
        }
    }
    if (matchCount != 1) {
        Wh_Log(L"File Pilot override unavailable: correction pattern not found");
        return true;
    }
    // The final instruction is the six-byte indirect call. Its end is the
    // return address seen by our hook, regardless of this function's RVA.
    g_filePilotCorrectionReturn = const_cast<unsigned char*>(match + patternLength);
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
        wcscpy_s(g_processName, name ? name + 1 : processPath);
        g_isExplorer = _wcsicmp(g_processName, L"explorer.exe") == 0;
    }
    g_stopping.store(false);
    Wh_ModSettingsChanged();
    if (!InstallFilePilotHook(g_processName)) return FALSE;
    return WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWHook, &OriginalCreateW) &&
           WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateAHook, &OriginalCreateA) &&
           WindhawkUtils::SetFunctionHook(ShowWindow, ShowHook, &OriginalShow);
}
void Wh_ModAfterInit() {
    EnumWindows(EnumAttach, 0);
}
static void AskToCloseApplication(const wchar_t* reason) {
    const wchar_t* name = g_processName;
    wchar_t message[768]{};
    swprintf_s(message, LR"(Please close %s to finish cleanup for Application Windows: No Minimum Size.

%s
Save your work, finish any save prompts, and close this application manually.

No process will be force-terminated. Unloading may remain pending until it exits.)",
        *name ? name : L"This application", reason);
    MessageBoxW(nullptr, message, L"Please close the application",
                MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
}

static bool TryDetachWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) return true;
    DWORD_PTR result = 0;
    // Same protocol as WindhawkUtils::RemoveWindowSubclassFromAnyThread.
    // The LPARAM is a function identifier, not a temporary data pointer.
    return SendMessageTimeoutW(
        hwnd, WindhawkUtils::detail::GetSubclassRegisteredMsg(), FALSE,
        reinterpret_cast<LPARAM>(SubclassWndProc),
        SMTO_ABORTIFHUNG | SMTO_BLOCK, 1500, &result) != 0;
}
static bool IsUndersized(HWND hwnd) {
    // Judge the current on-screen window, not a saved restored rectangle.
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || IsZoomed(hwnd)) return false;
    SIZE minimum{};
    {
        std::lock_guard lock(g_windowsMutex);
        auto it = g_minimums.find(hwnd);
        if (it == g_minimums.end()) return false;
        minimum = it->second;
    }
    RECT rect{};
    if (!GetWindowRect(hwnd, &rect)) return false;
    return (minimum.cx > 0 && rect.right - rect.left < minimum.cx) ||
           (minimum.cy > 0 && rect.bottom - rect.top < minimum.cy);
}
void Wh_ModBeforeUninit() {
    g_stopping.store(true);
    // Finish attachments already in flight before taking the detach snapshot.
    bool prompted = false;
    ULONGLONG attachmentStart = GetTickCount64();
    while (g_attaching.load()) {
        if (!prompted && GetTickCount64() - attachmentStart >= 3000) {
            std::vector<HWND> pending;
            {
                std::lock_guard lock(g_windowsMutex);
                pending.assign(g_windows.begin(), g_windows.end());
            }
            for (HWND hwnd : pending) {
                if (IsWindowVisible(hwnd)) PostMessageW(hwnd, WM_CLOSE, 0, 0);
            }
            Sleep(1500);
            if (g_attaching.load()) {
                prompted = true;
                AskToCloseApplication(L"A window attachment is still in progress. This is a cleanup problem, not a window-size warning. A close request was sent to visible windows.");
            }
        }
        Sleep(50);
    }
    std::vector<HWND> windows;
    {
        std::lock_guard lock(g_windowsMutex);
        windows.assign(g_windows.begin(), g_windows.end());
    }
    // Never hold the tracking mutex while sending to a window thread.
    for (HWND hwnd : windows) {
        if (IsUndersized(hwnd)) {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            ULONGLONG deadline = GetTickCount64() + 3000;
            while (IsWindow(hwnd) && GetTickCount64() < deadline) Sleep(50);
            if (IsWindow(hwnd) && !prompted) {
                prompted = true;
                AskToCloseApplication(L"An undersized window was asked to close but remains open. The app may be waiting for you to save or confirm closing.");
            }
        }
        if (TryDetachWindow(hwnd)) continue;
        // Request closure only after detachment times out; preserve save prompts.
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
        Sleep(1500);
        while (!TryDetachWindow(hwnd)) {
            if (!prompted) {
                prompted = true;
                AskToCloseApplication(L"Removing a window callback timed out. This can happen at any window size. A close request was sent, but cleanup is still blocked.");
            }
            Sleep(250);
        }
    }
}
void Wh_ModUninit() {
    std::lock_guard lock(g_windowsMutex);
    g_windows.clear();
    g_minimums.clear();
}
