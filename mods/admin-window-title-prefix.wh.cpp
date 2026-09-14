// ==WindhawkMod==
// @id              admin-window-title-prefix
// @name            🛡️ Admin Window Title Prefix
// @description     Add a prefix symbol or "[Admin]" to elevated/admin window titles. Supports presets and custom text. Refreshes instantly.
// @version         1.0
// @author          wakhh@qq.com
// @github          https://github.com/wakhh
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds a prefix to the title of elevated (admin) windows.

Features:
- Preset prefixes: 🛡️ Shield, ⛨ BW Shield, 🔒 Lock, 👑 Crown, [Admin]
- Custom prefix input: overrides the dropdown when not empty
- Default: 🛡️ Shield
- Instant refresh on settings change
- Automatically removes all prefixes when the mod is disabled
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- prefixMode: colorShield
  $name: Prefix Style
  $description: Choose a preset prefix. Each option includes a trailing space.
  $options:
    - colorShield: "🛡️ Shield"
    - bwShield: "⛨ BW Shield"
    - lock: "🔒 Lock"
    - crown: "👑 Crown"
    - admin: "[Admin]"
- customPrefix: ""
  $name: Custom Prefix
  $description: If not empty, overrides the dropdown selection above.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <unordered_map>
#include <processthreadsapi.h>
#include <securitybaseapi.h>

static std::unordered_map<DWORD, BOOL> g_pidElevatedCache;
using InternalGetWindowText_t = int(WINAPI*)(HWND hWnd, LPWSTR pString, int cchMaxCount);
InternalGetWindowText_t pOrigInternalGetWindowText;

WCHAR g_prefix[MAX_PATH] = L"\xD83D\xDEE1 ";           // current active prefix
WCHAR g_effectiveOldPrefix[MAX_PATH] = L"\xD83D\xDEE1 "; // last active prefix for stripping
static BOOL g_unloading = FALSE;

void LoadSettings() {
    WCHAR custom[MAX_PATH] = L"";
    Wh_GetStringSetting(L"customPrefix", custom, ARRAYSIZE(custom));

    // custom non-empty → takes priority
    if (wcslen(custom) > 0) {
        wcscpy_s(g_prefix, ARRAYSIZE(g_prefix), custom);
        return;
    }

    // otherwise use dropdown
    WCHAR modeBuf[MAX_PATH] = L"";
    Wh_GetStringSetting(L"prefixMode", modeBuf, ARRAYSIZE(modeBuf));

    if (wcscmp(modeBuf, L"bwShield") == 0) {
        wcscpy_s(g_prefix, ARRAYSIZE(g_prefix), L"\x26E8 ");
    } else if (wcscmp(modeBuf, L"lock") == 0) {
        wcscpy_s(g_prefix, ARRAYSIZE(g_prefix), L"\xD83D\xDD12 ");
    } else if (wcscmp(modeBuf, L"crown") == 0) {
        wcscpy_s(g_prefix, ARRAYSIZE(g_prefix), L"\xD83D\xDC51 ");
    } else if (wcscmp(modeBuf, L"admin") == 0) {
        wcscpy_s(g_prefix, ARRAYSIZE(g_prefix), L"[Admin] ");
    } else {
        // colorShield (default)
        wcscpy_s(g_prefix, ARRAYSIZE(g_prefix), L"\xD83D\xDEE1 ");
    }
}

static BOOL IsProcessElevated(DWORD pid)
{
    auto it = g_pidElevatedCache.find(pid);
    if (it != g_pidElevatedCache.end())
        return it->second;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProc) {
        if (GetLastError() == ERROR_ACCESS_DENIED) {
            g_pidElevatedCache[pid] = TRUE;
            return TRUE;
        }
        g_pidElevatedCache[pid] = FALSE;
        return FALSE;
    }

    BOOL elevated = FALSE;
    HANDLE hToken = nullptr;
    if (OpenProcessToken(hProc, TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION te;
        DWORD cb = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &te, cb, &cb))
            elevated = te.TokenIsElevated;
        CloseHandle(hToken);
    }
    CloseHandle(hProc);
    g_pidElevatedCache[pid] = elevated;
    return elevated;
}

// Strip old prefix with exact length match
static BOOL StripOldPrefix(WCHAR* buf) {
    size_t oldLen = wcslen(g_effectiveOldPrefix);
    if (oldLen == 0) return FALSE;
    if (wcsncmp(buf, g_effectiveOldPrefix, oldLen) == 0) {
        memmove(buf, buf + oldLen, (wcslen(buf + oldLen) + 1) * sizeof(WCHAR));
        return TRUE;
    }
    return FALSE;
}

// Strip either old or current prefix
static BOOL StripAnyPrefix(WCHAR* buf) {
    size_t oldLen = wcslen(g_effectiveOldPrefix);
    size_t curLen = wcslen(g_prefix);
    if (oldLen > 0 && wcsncmp(buf, g_effectiveOldPrefix, oldLen) == 0) {
        memmove(buf, buf + oldLen, (wcslen(buf + oldLen) + 1) * sizeof(WCHAR));
        return TRUE;
    }
    if (curLen > 0 && wcsncmp(buf, g_prefix, curLen) == 0) {
        memmove(buf, buf + curLen, (wcslen(buf + curLen) + 1) * sizeof(WCHAR));
        return TRUE;
    }
    return FALSE;
}

static BOOL CALLBACK RefreshEnumProc(HWND hWnd, LPARAM lParam) {
    if (!IsWindowVisible(hWnd)) return TRUE;
    if (GetParent(hWnd) != nullptr) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    WCHAR buf[512];
    GetWindowTextW(hWnd, buf, _countof(buf));

    if (IsProcessElevated(pid)) {
        StripOldPrefix(buf);
        SetWindowTextW(hWnd, buf);
    } else {
        if (StripAnyPrefix(buf)) {
            SetWindowTextW(hWnd, buf);
        }
    }
    return TRUE;
}

static BOOL CALLBACK UninitEnumProc(HWND hWnd, LPARAM lParam) {
    if (!IsWindowVisible(hWnd)) return TRUE;
    if (GetParent(hWnd) != nullptr) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!IsProcessElevated(pid)) return TRUE;

    WCHAR buf[512] = {0};
    if (pOrigInternalGetWindowText) {
        pOrigInternalGetWindowText(hWnd, buf, _countof(buf));
    }
    if (wcslen(buf) > 0) {
        SetWindowTextW(hWnd, buf);
    }
    return TRUE;
}

int WINAPI Hook_InternalGetWindowText(HWND hWnd, LPWSTR pString, int cchMaxCount) {
    if (g_unloading) {
        return pOrigInternalGetWindowText(hWnd, pString, cchMaxCount);
    }

    int ret = pOrigInternalGetWindowText(hWnd, pString, cchMaxCount);
    if (ret <= 0 || !pString || cchMaxCount < 8)
        return ret;

    if (GetParent(hWnd) != nullptr)
        return ret;

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!IsProcessElevated(pid))
        return ret;

    if (wcsncmp(pString, g_prefix, wcslen(g_prefix)) == 0)
        return ret;

    std::wstring newTitle = g_prefix;
    newTitle += pString;

    if ((int)newTitle.size() < cchMaxCount) {
        wcscpy_s(pString, cchMaxCount, newTitle.c_str());
        return static_cast<int>(newTitle.size());
    }
    return ret;
}

BOOL Wh_ModInit() {
    LoadSettings();
    wcscpy_s(g_effectiveOldPrefix, ARRAYSIZE(g_effectiveOldPrefix), g_prefix);
    g_unloading = FALSE;

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return FALSE;

    FARPROC fp = GetProcAddress(hUser32, "InternalGetWindowText");
    if (!fp) return FALSE;

    if (!Wh_SetFunctionHook(
        reinterpret_cast<void*>(fp),
        reinterpret_cast<void*>(Hook_InternalGetWindowText),
        reinterpret_cast<void**>(&pOrigInternalGetWindowText)
    ))
        return FALSE;

    EnumWindows(RefreshEnumProc, 0);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    wcscpy_s(g_effectiveOldPrefix, ARRAYSIZE(g_effectiveOldPrefix), g_prefix);
    LoadSettings();
    g_pidElevatedCache.clear();
    EnumWindows(RefreshEnumProc, 0);
}

void Wh_ModUninit() {
    g_unloading = TRUE;
    g_pidElevatedCache.clear();
    EnumWindows(UninitEnumProc, 0);
}