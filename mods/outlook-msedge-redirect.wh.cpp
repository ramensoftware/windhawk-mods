// ==WindhawkMod==
// @id              outlook-msedge-redirect
// @name            Outlook Edge-to-Chromium Redirector
// @description     Redirects any msedge.exe launch or microsoft-edge: protocol activation triggered by OUTLOOK.EXE to a custom Chromium-based browser, extracting the real target URL along the way
// @version         1.1
// @author          somebudyelse-ka
// @github          https://github.com/somebudyelse-ka
// @include         OUTLOOK.EXE
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Outlook Edge-to-Chromium Redirector

Classic Outlook hard-launches Microsoft Edge for any link you click in an
email, regardless of your system default browser. This mod hooks process
creation and shell execution inside OUTLOOK.EXE and rewrites any attempt to
launch `msedge.exe` (or invoke the `microsoft-edge:` protocol) into a launch
of a Chromium-based browser of your choice instead.

It handles two link-click formats that Edge/Outlook commonly produce:

- A plain `https://` / `http://` URL somewhere in the command line.
- Edge's own wrapped format, e.g.
  `microsoft-edge:///?url=https%3A%2F%2Fexample.com%2F...`, which is parsed
  and percent-decoded (correctly, as UTF-8) so the real destination URL is
  passed to the replacement browser instead of opening a blank tab.

The extracted URL is validated (must start with `http://`/`https://`, no
quotes, no whitespace, no control characters) before being used, and is
passed to the replacement browser via `--single-argument`, the same
mechanism Edge/Chromium use internally, so it's never spliced into the
command line as free text.

Observed in testing: Outlook invokes the browser via a command line of the
form `"...\msedge.exe" --single-argument microsoft-edge:///?url=<encoded>`,
with `lpApplicationName` pointing at msedge.exe directly. Both the
`CreateProcessInternalW` hook and the `ShellExecuteExW` backstop check for
this pattern as well as a bare `microsoft-edge:` protocol invocation.

## Settings

- **Browser path**: full path to the replacement browser's executable.
- **Redirect ShellExecute calls**: enable/disable the ShellExecuteExW
  backstop hook.
- **Extra launch arguments**: any additional command-line arguments to pass
  to the replacement browser (e.g. a specific profile flag), inserted before
  the URL.

## Relationship to other mods

[pivotlink-browser-router](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/pivotlink-browser-router.wh.cpp)
hooks the same set of functions process-wide and reroutes browser launches
to whichever configured browser is currently running, by priority. This mod
differs in scope and intent: it's scoped only to `OUTLOOK.EXE`, and always
sends launches to one fixed, explicitly configured browser path rather than
picking among running browsers. Useful if you want Outlook specifically
pinned to a browser regardless of your system default or what else happens
to be open.

## Notes

This mod only intercepts calls made from within OUTLOOK.EXE's own process.
If a particular Outlook build resolves link clicks through a broker process
(such as browser_broker.exe) instead of calling CreateProcess directly, this
mod won't see that launch. Check the mod's log (via Windhawk's log viewer)
after clicking a link to confirm it's firing as expected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- browserPath: "C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe"
  $name: Browser path
  $description: >-
    Full path to the replacement Chromium-based browser's executable.
    Defaults to a system-wide Chrome install; change this if Chrome is
    installed elsewhere (e.g. per-user under
    %LocalAppData%\Google\Chrome\Application\chrome.exe) or if you're using
    a different Chromium-based browser.
- redirectShellExecute: true
  $name: Redirect ShellExecute calls
  $description: >-
    Also intercept ShellExecuteExW calls that target msedge.exe or the
    microsoft-edge: protocol directly, as a backstop for the main
    CreateProcess hook.
- extraArgs: ""
  $name: Extra launch arguments
  $description: >-
    Optional extra command-line arguments to pass to the replacement
    browser, inserted before the URL (e.g. --profile-directory="Work").
    Applied on both the CreateProcess and ShellExecute paths.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>

#include <cstring>
#include <cwctype>
#include <mutex>
#include <string>
#include <vector>

std::mutex g_settingsMutex;
std::wstring g_browserPath;
std::wstring g_extraArgs;
bool g_redirectShellExecute = true;

std::wstring ExpandEnvVars(const std::wstring& in) {
    if (in.find(L'%') == std::wstring::npos) {
        return in;
    }
    DWORD needed = ExpandEnvironmentStringsW(in.c_str(), nullptr, 0);
    if (needed == 0) {
        return in;
    }
    std::wstring out(needed, L'\0');
    DWORD written = ExpandEnvironmentStringsW(in.c_str(), out.data(), needed);
    if (written == 0) {
        return in;
    }
    // ExpandEnvironmentStringsW's return value includes the null
    // terminator; trim it from the std::wstring.
    out.resize(written - 1);
    return out;
}

void LoadSettings() {
    PCWSTR browserPath = Wh_GetStringSetting(L"browserPath");
    PCWSTR extraArgs = Wh_GetStringSetting(L"extraArgs");
    bool redirectShellExecute = Wh_GetIntSetting(L"redirectShellExecute") != 0;

    std::wstring expandedBrowserPath = ExpandEnvVars(browserPath);

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_browserPath = expandedBrowserPath;
        g_extraArgs = extraArgs;
        g_redirectShellExecute = redirectShellExecute;
    }

    Wh_FreeStringSetting(browserPath);
    Wh_FreeStringSetting(extraArgs);
}

// Extracts the first whitespace/quote-delimited token from a command line
// (i.e. the application path, honoring quoting rules).
std::wstring ExtractFirstToken(const std::wstring& cmd) {
    size_t i = 0;
    while (i < cmd.size() && cmd[i] == L' ') {
        i++;
    }
    if (i >= cmd.size()) {
        return L"";
    }
    if (cmd[i] == L'"') {
        size_t end = cmd.find(L'"', i + 1);
        return (end == std::wstring::npos) ? cmd.substr(i + 1)
                                            : cmd.substr(i + 1, end - i - 1);
    }
    size_t end = cmd.find(L' ', i);
    return (end == std::wstring::npos) ? cmd.substr(i) : cmd.substr(i, end - i);
}

bool IsEdgeExePath(const std::wstring& token) {
    if (token.empty()) {
        return false;
    }
    size_t pos = token.find_last_of(L"\\/");
    std::wstring fname = (pos == std::wstring::npos) ? token : token.substr(pos + 1);
    return _wcsicmp(fname.c_str(), L"msedge.exe") == 0;
}

bool IsEdgeProtocol(const std::wstring& token) {
    return _wcsnicmp(token.c_str(), L"microsoft-edge:", 15) == 0;
}

// Percent-decodes a URL, correctly treating the encoded bytes as UTF-8
// rather than one wchar_t per byte.
std::wstring UrlDecode(const std::wstring& in) {
    auto hex = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };

    std::string bytes;
    bytes.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == L'%' && i + 2 < in.size()) {
            int h1 = hex(in[i + 1]);
            int h2 = hex(in[i + 2]);
            if (h1 >= 0 && h2 >= 0) {
                bytes.push_back(static_cast<char>((h1 << 4) | h2));
                i += 2;
                continue;
            }
        }
        if (in[i] < 0x80) {
            bytes.push_back(static_cast<char>(in[i]));
        }
        // Non-ASCII wide chars aren't expected in an already-percent-encoded
        // query string; drop them rather than corrupt the UTF-8 byte stream.
    }

    if (bytes.empty()) {
        return L"";
    }

    int wideLen = MultiByteToWideChar(CP_UTF8, 0, bytes.data(),
                                       static_cast<int>(bytes.size()), nullptr, 0);
    if (wideLen <= 0) {
        return L"";
    }

    std::wstring out(wideLen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()),
                         out.data(), wideLen);
    return out;
}

// Requires an http(s) URL with no characters that could break out of a
// single command-line argument.
bool IsSafeUrl(const std::wstring& url) {
    bool https = url.size() >= 8 && _wcsnicmp(url.c_str(), L"https://", 8) == 0;
    bool http = url.size() >= 7 && _wcsnicmp(url.c_str(), L"http://", 7) == 0;
    if (!https && !http) {
        return false;
    }
    for (wchar_t c : url) {
        if (c == L'"' || c <= 0x20) {
            return false;
        }
    }
    return true;
}

// Finds the position of "?url=" or "&url=" (whichever comes first) at or
// after `from`, returning npos if neither is present.
size_t FindUrlParam(const std::wstring& cmd, size_t from) {
    size_t p1 = cmd.find(L"?url=", from);
    size_t p2 = cmd.find(L"&url=", from);
    if (p1 == std::wstring::npos) return p2;
    if (p2 == std::wstring::npos) return p1;
    return std::min(p1, p2);
}

// Extracts and validates the real target URL from a command line or
// microsoft-edge: protocol string. Returns an empty string if no valid
// http(s) URL is found.
std::wstring ExtractUrl(const std::wstring& cmd) {
    size_t schemePos = cmd.find(L"microsoft-edge:");
    if (schemePos != std::wstring::npos) {
        size_t paramPos = FindUrlParam(cmd, schemePos);
        if (paramPos != std::wstring::npos) {
            size_t start = paramPos + 5;  // skip "?url=" / "&url="
            size_t end = cmd.size();
            for (wchar_t stopChar : {L'&', L' ', L'"'}) {
                size_t p = cmd.find(stopChar, start);
                if (p != std::wstring::npos && p < end) {
                    end = p;
                }
            }
            std::wstring decoded = UrlDecode(cmd.substr(start, end - start));
            return IsSafeUrl(decoded) ? decoded : L"";
        }
    }

    for (const wchar_t* scheme : {L"https://", L"http://"}) {
        size_t pos = cmd.find(scheme);
        if (pos == std::wstring::npos) {
            continue;
        }
        size_t end = cmd.size();
        if (pos > 0 && cmd[pos - 1] == L'"') {
            size_t q = cmd.find(L'"', pos);
            if (q != std::wstring::npos) end = q;
        } else {
            size_t sp = cmd.find(L' ', pos);
            if (sp != std::wstring::npos) end = sp;
        }
        std::wstring candidate = cmd.substr(pos, end - pos);
        return IsSafeUrl(candidate) ? candidate : L"";
    }

    return L"";
}

// ---- CreateProcessInternalW hook (catches CreateProcessW/A from any
// caller, including code inside shell32 resolving a protocol handler) ----
using CreateProcessInternalW_t = BOOL(WINAPI*)(
    HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
    PHANDLE hNewToken);

CreateProcessInternalW_t CreateProcessInternalW_orig;

BOOL WINAPI CreateProcessInternalW_hook(
    HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
    PHANDLE hNewToken) {
    std::wstring browserPath, extraArgs;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        browserPath = g_browserPath;
        extraArgs = g_extraArgs;
    }

    std::wstring fullCmd = lpCommandLine ? lpCommandLine : L"";

    std::wstring appToken = lpApplicationName ? lpApplicationName : L"";
    if (appToken.empty() && !fullCmd.empty()) {
        appToken = ExtractFirstToken(fullCmd);
    }

    bool redirect = IsEdgeExePath(appToken) || IsEdgeProtocol(appToken);
    if (!redirect && fullCmd.find(L"microsoft-edge:") != std::wstring::npos) {
        redirect = true;
    }

    if (redirect && !browserPath.empty()) {
        Wh_Log(L"Original edge cmdline: %s", fullCmd.c_str());

        std::wstring url = ExtractUrl(fullCmd);

        std::wstring newCmd = L"\"" + browserPath + L"\"";
        if (!extraArgs.empty()) {
            newCmd += L" " + extraArgs;
        }
        if (!url.empty()) {
            newCmd += L" --single-argument " + url;
        }

        Wh_Log(L"New cmdline: %s", newCmd.c_str());

        std::vector<wchar_t> buf(newCmd.begin(), newCmd.end());
        buf.push_back(0);

        return CreateProcessInternalW_orig(
            hToken, browserPath.c_str(), buf.data(), lpProcessAttributes,
            lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
            lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
    }

    return CreateProcessInternalW_orig(
        hToken, lpApplicationName, lpCommandLine, lpProcessAttributes,
        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
        lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
}

// ---- ShellExecuteExW hook (backstop) ----
using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_orig;

BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pInfo) {
    bool redirectShellExecute;
    std::wstring browserPath, extraArgs;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        redirectShellExecute = g_redirectShellExecute;
        browserPath = g_browserPath;
        extraArgs = g_extraArgs;
    }

    if (redirectShellExecute && !browserPath.empty() && pInfo && pInfo->lpFile) {
        std::wstring file(pInfo->lpFile);
        bool isEdge = IsEdgeExePath(file) || IsEdgeProtocol(file);

        if (isEdge) {
            std::wstring params = pInfo->lpParameters ? pInfo->lpParameters : L"";
            std::wstring source = IsEdgeProtocol(file) ? file : params;
            std::wstring url = ExtractUrl(source);

            std::wstring newParams;
            if (!extraArgs.empty()) {
                newParams += extraArgs + L" ";
            }
            if (!url.empty()) {
                newParams += L"--single-argument " + url;
            } else {
                // Couldn't find/validate a URL: keep whatever the caller
                // originally passed rather than silently dropping it.
                newParams += params;
            }

            Wh_Log(L"Redirecting ShellExecute msedge -> %s", browserPath.c_str());

            SHELLEXECUTEINFOW newInfo = {};
            memcpy(&newInfo, pInfo, pInfo->cbSize);
            newInfo.lpFile = browserPath.c_str();
            newInfo.lpParameters = newParams.empty() ? nullptr : newParams.c_str();

            BOOL ret = ShellExecuteExW_orig(&newInfo);

            pInfo->hInstApp = newInfo.hInstApp;
            if (pInfo->fMask & SEE_MASK_NOCLOSEPROCESS) {
                pInfo->hProcess = newInfo.hProcess;
            }

            return ret;
        }
    }

    return ShellExecuteExW_orig(pInfo);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init outlook-msedge-redirect");

    LoadSettings();

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    void* pCreateProcessInternalW =
        hKernelBase
            ? (void*)GetProcAddress(hKernelBase, "CreateProcessInternalW")
            : nullptr;
    if (!pCreateProcessInternalW) {
        HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        pCreateProcessInternalW =
            hKernel32
                ? (void*)GetProcAddress(hKernel32, "CreateProcessInternalW")
                : nullptr;
    }

    if (pCreateProcessInternalW) {
        Wh_SetFunctionHook(pCreateProcessInternalW,
                            (void*)CreateProcessInternalW_hook,
                            (void**)&CreateProcessInternalW_orig);
    } else {
        Wh_Log(L"Failed to resolve CreateProcessInternalW");
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    void* pShellExecuteExW =
        hShell32 ? (void*)GetProcAddress(hShell32, "ShellExecuteExW") : nullptr;
    if (pShellExecuteExW) {
        Wh_SetFunctionHook(pShellExecuteExW, (void*)ShellExecuteExW_hook,
                            (void**)&ShellExecuteExW_orig);
    } else {
        Wh_Log(L"Failed to resolve ShellExecuteExW");
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}
