// ==WindhawkMod==
// @id              perform-speedtest-redirect
// @name            Perform Speed Test Redirect
// @description     Redirects the "Perform speed test" link in the taskbar network right-click menu from the default Microsoft page to a custom URL (defaults to speedtest.net).
// @version         1.2.2
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

#define LINK_ID L"linkid=2324916"
#define LOG(fmt, ...) Wh_Log(L"[speedtest] " fmt, ##__VA_ARGS__)

// ── helpers ──────────────────────────────────────────────────────────────────

static bool HasLinkId(LPCWSTR s) {
    return s && wcsstr(s, LINK_ID);
}

// Replace the full URL that contains LINK_ID with redirectUrl.
// Returns the modified string, or empty if LINK_ID not found.
static std::wstring ReplaceUrl(LPCWSTR src) {
    if (!src) return {};
    std::wstring s(src);

    const wchar_t* hit = wcsstr(src, LINK_ID);
    if (!hit) return {};

    size_t pos      = hit - src;
    size_t urlStart = s.rfind(L"http", pos);
    if (urlStart == std::wstring::npos) urlStart = pos;
    size_t urlEnd   = s.find_first_of(L" \t\r\n\"'", pos);
    if (urlEnd   == std::wstring::npos) urlEnd   = s.size();

    PCWSTR url = Wh_GetStringSetting(L"redirectUrl");
    s.replace(urlStart, urlEnd - urlStart, url);
    Wh_FreeStringSetting(url);
    return s;
}

// ── ShellExecuteExW hook ─────────────────────────────────────────────────────

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_Original;

BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* pei) {
    if (!pei) return ShellExecuteExW_Original(pei);

    if (HasLinkId(pei->lpFile) || HasLinkId(pei->lpParameters)) {
        std::wstring newFile, newParams;
        SHELLEXECUTEINFOW mod = *pei;

        if (HasLinkId(pei->lpFile)) {
            newFile = ReplaceUrl(pei->lpFile);
            mod.lpFile       = newFile.c_str();
            mod.lpParameters = nullptr;
        } else {
            newParams = ReplaceUrl(pei->lpParameters);
            mod.lpParameters = newParams.c_str();
        }

        LOG(L"ShellExecuteExW → %s", mod.lpFile ? mod.lpFile : mod.lpParameters);
        return ShellExecuteExW_Original(&mod);
    }

    return ShellExecuteExW_Original(pei);
}

// ── ShellExecuteW hook ───────────────────────────────────────────────────────

using ShellExecuteW_t = decltype(&ShellExecuteW);
ShellExecuteW_t ShellExecuteW_Original;

HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR op, LPCWSTR file,
                                    LPCWSTR params, LPCWSTR dir, INT show) {
    if (HasLinkId(file)) {
        std::wstring newFile = ReplaceUrl(file);
        LOG(L"ShellExecuteW → %s", newFile.c_str());
        return ShellExecuteW_Original(hwnd, op, newFile.c_str(), nullptr, dir, show);
    }
    if (HasLinkId(params)) {
        std::wstring newParams = ReplaceUrl(params);
        LOG(L"ShellExecuteW params → %s", newParams.c_str());
        return ShellExecuteW_Original(hwnd, op, file, newParams.c_str(), dir, show);
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
    if (HasLinkId(cmd)) {
        std::wstring newCmd = ReplaceUrl(cmd);
        LOG(L"CreateProcessW → %s", newCmd.c_str());
        return CreateProcessW_Original(app, newCmd.data(), psa, tsa,
                                       inherit, flags, env, dir, si, pi);
    }

    return CreateProcessW_Original(app, cmd, psa, tsa, inherit, flags, env, dir, si, pi);
}

// ── Init ─────────────────────────────────────────────────────────────────────

static bool HookFn(void* target, void* hook, void** orig, const wchar_t* name) {
    if (!target) {
        LOG(L"Could not resolve %s", name);
        return false;
    }
    if (!Wh_SetFunctionHook(target, hook, orig)) {
        LOG(L"Hook failed for %s", name);
        return false;
    }
    LOG(L"Hooked %s", name);
    return true;
}

BOOL Wh_ModInit() {
    LOG(L"Init in %s", GetCommandLineW());

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
        LOG(L"One or more hooks failed — aborting");
        return FALSE;
    }

    LOG(L"All hooks installed");
    return TRUE;
}
