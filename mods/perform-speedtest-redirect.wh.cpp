// ==WindhawkMod==
// @id              perform-speedtest-redirect
// @name            Perform Speed Test Redirect
// @description     Redirects the "Perform speed test" link in the taskbar network right-click menu from the default Microsoft page to a custom URL (defaults to speedtest.net).
// @version         1.2.1
// @author          mynameistito
// @github          https://github.com/mynameistito
// @license         MIT
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         RuntimeBroker.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Perform Speed Test Redirect

When you right-click the network icon in the Windows taskbar and click
**"Perform speed test"**, Windows normally opens:

```
https://go.microsoft.com/fwlink/?linkid=2324916
```

This mod intercepts that URL and replaces it with a URL of your choice.
By default it redirects to **https://www.speedtest.net** (or any URL you set).

## Settings

- **Redirect URL** – the URL to open instead of the default Microsoft speed
  test link. Change this to any speed test site you prefer, e.g.
  `https://fast.com` or `https://cloudflare.com/speed`.

## How it works

Hooks `ShellExecuteW`, `ShellExecuteExW`, and `CreateProcessW` across
`explorer.exe`, `ShellExperienceHost.exe`, and `RuntimeBroker.exe` —
covering every known code path Windows uses to open the speed test URL.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- redirectUrl: "https://www.speedtest.net/"
  $name: Redirect URL
  $description: The URL to open instead of the default Microsoft speed test link.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <algorithm>

#define LINK_ID L"linkid=2324916"

// ── helpers ──────────────────────────────────────────────────────────────────

static bool HasLinkId(LPCWSTR s) {
    if (!s) return false;
    const size_t nlen = wcslen(LINK_ID);
    const size_t hlen = wcslen(s);
    for (size_t i = 0; i + nlen <= hlen; i++) {
        if (_wcsnicmp(s + i, LINK_ID, nlen) == 0) return true;
    }
    return false;
}

// Replace the full URL that contains LINK_ID with redirectUrl.
// Returns the modified string, or empty if LINK_ID not found.
static std::wstring ReplaceUrl(LPCWSTR src) {
    if (!src) return {};
    std::wstring s(src);
    std::wstring lower(s);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

    const std::wstring needle(LINK_ID);
    size_t pos = lower.find(needle);
    if (pos == std::wstring::npos) return {};

    // Walk back to find start of "http" or "https"
    size_t urlStart = lower.rfind(L"http", pos);
    if (urlStart == std::wstring::npos) urlStart = pos;

    // Walk forward to find end of URL (whitespace, quote, or end of string)
    size_t urlEnd = s.find_first_of(L" \t\r\n\"'", pos);
    if (urlEnd == std::wstring::npos) urlEnd = s.size();

    PCWSTR redirectUrl = Wh_GetStringSetting(L"redirectUrl");
    s.replace(urlStart, urlEnd - urlStart, redirectUrl);
    Wh_FreeStringSetting(redirectUrl);
    return s;
}

// ── ShellExecuteExW hook ─────────────────────────────────────────────────────

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_Original;

BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* pei) {
    if (!pei) return ShellExecuteExW_Original(pei);

    bool inFile   = HasLinkId(pei->lpFile);
    bool inParams = HasLinkId(pei->lpParameters);

    if (inFile || inParams) {
        Wh_Log(L"[speedtest] ShellExecuteExW intercepted — file=%s params=%s",
               pei->lpFile       ? pei->lpFile       : L"(null)",
               pei->lpParameters ? pei->lpParameters : L"(null)");

        std::wstring newFile, newParams;
        SHELLEXECUTEINFOW mod = *pei;

        if (inFile) {
            newFile = ReplaceUrl(pei->lpFile);
            mod.lpFile       = newFile.c_str();
            mod.lpParameters = nullptr;
        } else {
            newParams = ReplaceUrl(pei->lpParameters);
            mod.lpParameters = newParams.c_str();
        }

        Wh_Log(L"[speedtest] Redirecting ShellExecuteExW to: %s",
               inFile ? mod.lpFile : mod.lpParameters);
        return ShellExecuteExW_Original(&mod);
    }

    return ShellExecuteExW_Original(pei);
}

// ── ShellExecuteW hook ───────────────────────────────────────────────────────

using ShellExecuteW_t = decltype(&ShellExecuteW);
ShellExecuteW_t ShellExecuteW_Original;

HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR op, LPCWSTR file,
                                    LPCWSTR params, LPCWSTR dir, INT show) {
    bool inFile   = HasLinkId(file);
    bool inParams = HasLinkId(params);

    if (inFile || inParams) {
        Wh_Log(L"[speedtest] ShellExecuteW intercepted — file=%s params=%s",
               file   ? file   : L"(null)",
               params ? params : L"(null)");

        std::wstring newFile, newParams;
        if (inFile) {
            newFile = ReplaceUrl(file);
            Wh_Log(L"[speedtest] Redirecting ShellExecuteW to: %s", newFile.c_str());
            return ShellExecuteW_Original(hwnd, op, newFile.c_str(),
                                          nullptr, dir, show);
        } else {
            newParams = ReplaceUrl(params);
            Wh_Log(L"[speedtest] Redirecting ShellExecuteW params to: %s", newParams.c_str());
            return ShellExecuteW_Original(hwnd, op, file,
                                          newParams.c_str(), dir, show);
        }
    }

    return ShellExecuteW_Original(hwnd, op, file, params, dir, show);
}

// ── CreateProcessW hook ──────────────────────────────────────────────────────
// Last-resort: catches the browser being spawned with the URL in its
// command line, regardless of which higher-level API triggered it.

using CreateProcessW_t = decltype(&CreateProcessW);
CreateProcessW_t CreateProcessW_Original;

BOOL WINAPI CreateProcessW_Hook(LPCWSTR app, LPWSTR cmd,
                                LPSECURITY_ATTRIBUTES psa, LPSECURITY_ATTRIBUTES tsa,
                                BOOL inherit, DWORD flags, LPVOID env,
                                LPCWSTR dir, LPSTARTUPINFOW si, LPPROCESS_INFORMATION pi) {
    bool inApp = HasLinkId(app);
    bool inCmd = HasLinkId(cmd);

    if (inApp || inCmd) {
        Wh_Log(L"[speedtest] CreateProcessW intercepted — app=%s cmd=%s",
               app ? app : L"(null)",
               cmd ? cmd : L"(null)");

        std::wstring newCmd;
        if (inCmd) {
            newCmd = ReplaceUrl(cmd);
            Wh_Log(L"[speedtest] New command line: %s", newCmd.c_str());
            return CreateProcessW_Original(app, newCmd.data(), psa, tsa,
                                           inherit, flags, env, dir, si, pi);
        }
        // inApp only — unlikely, but forward unchanged
    }

    return CreateProcessW_Original(app, cmd, psa, tsa, inherit, flags, env, dir, si, pi);
}

// ── Init ─────────────────────────────────────────────────────────────────────

static bool HookFn(void* target, void* hook, void** orig, const wchar_t* name) {
    if (!target) {
        Wh_Log(L"[speedtest] Could not resolve %s", name);
        return false;
    }
    if (!Wh_SetFunctionHook(target, hook, orig)) {
        Wh_Log(L"[speedtest] Hook failed for %s", name);
        return false;
    }
    Wh_Log(L"[speedtest] Hooked %s", name);
    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L"[speedtest] Init in %s", GetCommandLineW());

    HMODULE hShell32  = GetModuleHandleW(L"shell32.dll");
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");

    bool ok = true;

    ok &= HookFn(hShell32  ? (void*)GetProcAddress(hShell32,  "ShellExecuteExW") : nullptr,
                 (void*)ShellExecuteExW_Hook, (void**)&ShellExecuteExW_Original,
                 L"ShellExecuteExW");

    ok &= HookFn(hShell32  ? (void*)GetProcAddress(hShell32,  "ShellExecuteW")   : nullptr,
                 (void*)ShellExecuteW_Hook,   (void**)&ShellExecuteW_Original,
                 L"ShellExecuteW");

    ok &= HookFn(hKernel32 ? (void*)GetProcAddress(hKernel32, "CreateProcessW")  : nullptr,
                 (void*)CreateProcessW_Hook,  (void**)&CreateProcessW_Original,
                 L"CreateProcessW");

    if (!ok) {
        Wh_Log(L"[speedtest] One or more hooks failed — aborting");
        return FALSE;
    }

    Wh_Log(L"[speedtest] All hooks installed");
    return TRUE;
}
