// ==WindhawkMod==
// @id              outlook-msedge-redirect
// @name            Outlook Edge-to-Chromium Redirector
// @description     Redirects any msedge.exe launch triggered by OUTLOOK.EXE to a custom Chromium-based browser, extracting the real target URL along the way
// @version         1.0
// @author          you
// @include         OUTLOOK.EXE
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Outlook Edge-to-Chromium Redirector

Classic Outlook hard-launches Microsoft Edge for any link you click in an
email, regardless of your system default browser. This mod hooks process
creation inside OUTLOOK.EXE and rewrites any attempt to launch `msedge.exe`
into a launch of a Chromium-based browser of your choice instead.

It handles two link-click formats that Edge/Outlook commonly produce:

- A plain `https://` / `http://` URL somewhere in the command line.
- Edge's own wrapped format, e.g.
  `microsoft-edge:///?url=https%3A%2F%2Fexample.com%2F...`, which is parsed
  and percent-decoded so the real destination URL is passed to the
  replacement browser instead of opening a blank tab.

As a backstop, `ShellExecuteExW` calls made directly against `msedge.exe`
are also redirected.

## Settings

- **Browser path**: full path to the replacement browser's executable.
- **Redirect ShellExecute calls**: enable/disable the ShellExecuteExW
  backstop hook.
- **Extra launch arguments**: any additional command-line arguments to pass
  to the replacement browser (e.g. a specific profile flag), inserted before
  the URL.

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
    Also intercept ShellExecuteExW calls that target msedge.exe directly, as
    a backstop for the main CreateProcess hook.
- extraArgs: ""
  $name: Extra launch arguments
  $description: >-
    Optional extra command-line arguments to pass to the replacement
    browser, inserted before the URL (e.g. --profile-directory="Work").
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>

#include <cwctype>
#include <string>
#include <vector>

std::wstring g_browserPath;
std::wstring g_extraArgs;
bool g_redirectShellExecute = true;

void LoadSettings() {
    PCWSTR browserPath = Wh_GetStringSetting(L"browserPath");
    g_browserPath = browserPath;
    Wh_FreeStringSetting(browserPath);

    PCWSTR extraArgs = Wh_GetStringSetting(L"extraArgs");
    g_extraArgs = extraArgs;
    Wh_FreeStringSetting(extraArgs);

    g_redirectShellExecute = Wh_GetIntSetting(L"redirectShellExecute") != 0;
}

bool IsMsEdgePath(const wchar_t* path) {
    if (!path) {
        return false;
    }

    std::wstring s(path);
    size_t pos = s.find_last_of(L"\\/");
    std::wstring fname = (pos == std::wstring::npos) ? s : s.substr(pos + 1);
    if (!fname.empty() && fname.front() == L'"') {
        fname.erase(0, 1);
    }
    for (auto& c : fname) {
        c = towlower(c);
    }

    return fname.rfind(L"msedge.exe", 0) == 0;
}

std::wstring UrlDecode(const std::wstring& in) {
    std::wstring out;
    out.reserve(in.size());

    auto hex = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };

    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == L'%' && i + 2 < in.size()) {
            int h1 = hex(in[i + 1]);
            int h2 = hex(in[i + 2]);
            if (h1 >= 0 && h2 >= 0) {
                out.push_back(static_cast<wchar_t>((h1 << 4) | h2));
                i += 2;
                continue;
            }
        } else if (in[i] == L'+') {
            out.push_back(L' ');
            continue;
        }
        out.push_back(in[i]);
    }

    return out;
}

// Extracts the real target URL from an Edge command line. Handles both a
// bare http(s):// token and Edge's
// "microsoft-edge:///?url=<percent-encoded>" wrapper.
std::wstring ExtractUrl(const std::wstring& cmd) {
    size_t schemePos = cmd.find(L"microsoft-edge:");
    if (schemePos != std::wstring::npos) {
        size_t urlParam = cmd.find(L"url=", schemePos);
        if (urlParam != std::wstring::npos) {
            size_t start = urlParam + 4;
            size_t end = cmd.size();
            for (wchar_t stopChar : {L'&', L' ', L'"'}) {
                size_t p = cmd.find(stopChar, start);
                if (p != std::wstring::npos && p < end) {
                    end = p;
                }
            }
            return UrlDecode(cmd.substr(start, end - start));
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
            if (q != std::wstring::npos) {
                end = q;
            }
        } else {
            size_t sp = cmd.find(L' ', pos);
            if (sp != std::wstring::npos) {
                end = sp;
            }
        }
        return cmd.substr(pos, end - pos);
    }

    return L"";
}

std::wstring BuildReplacementCommandLine(const std::wstring& url) {
    std::wstring newCmd = L"\"" + g_browserPath + L"\"";
    if (!g_extraArgs.empty()) {
        newCmd += L" " + g_extraArgs;
    }
    if (!url.empty()) {
        newCmd += L" \"" + url + L"\"";
    }
    return newCmd;
}

// ---- CreateProcessW hook ----
using CreateProcessW_t = decltype(&CreateProcessW);
CreateProcessW_t CreateProcessW_orig;

BOOL WINAPI CreateProcessW_hook(
    LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation) {
    bool redirect = IsMsEdgePath(lpApplicationName);
    if (!redirect && lpCommandLine) {
        redirect = IsMsEdgePath(lpCommandLine);
    }

    if (redirect && !g_browserPath.empty()) {
        std::wstring fullCmd = lpCommandLine ? lpCommandLine : L"";
        Wh_Log(L"Original edge cmdline: %s", fullCmd.c_str());

        std::wstring url = ExtractUrl(fullCmd);
        std::wstring newCmd = BuildReplacementCommandLine(url);

        Wh_Log(L"New cmdline: %s", newCmd.c_str());

        std::vector<wchar_t> buf(newCmd.begin(), newCmd.end());
        buf.push_back(0);

        return CreateProcessW_orig(
            g_browserPath.c_str(), buf.data(), lpProcessAttributes,
            lpThreadAttributes, bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory, lpStartupInfo,
            lpProcessInformation);
    }

    return CreateProcessW_orig(lpApplicationName, lpCommandLine,
                                lpProcessAttributes, lpThreadAttributes,
                                bInheritHandles, dwCreationFlags,
                                lpEnvironment, lpCurrentDirectory,
                                lpStartupInfo, lpProcessInformation);
}

// ---- ShellExecuteExW hook (backstop) ----
using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_orig;

BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pInfo) {
    if (g_redirectShellExecute && !g_browserPath.empty() &&
        IsMsEdgePath(pInfo->lpFile)) {
        std::wstring params = pInfo->lpParameters ? pInfo->lpParameters : L"";
        std::wstring url = ExtractUrl(params);

        Wh_Log(L"Redirecting ShellExecute msedge.exe -> %s",
               g_browserPath.c_str());

        SHELLEXECUTEINFOW newInfo = *pInfo;
        newInfo.lpFile = g_browserPath.c_str();
        newInfo.lpParameters = url.empty() ? nullptr : url.c_str();
        return ShellExecuteExW_orig(&newInfo);
    }

    return ShellExecuteExW_orig(pInfo);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init outlook-msedge-redirect");

    LoadSettings();

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");

    Wh_SetFunctionHook((void*)GetProcAddress(hKernel32, "CreateProcessW"),
                        (void*)CreateProcessW_hook,
                        (void**)&CreateProcessW_orig);

    Wh_SetFunctionHook((void*)GetProcAddress(hShell32, "ShellExecuteExW"),
                        (void*)ShellExecuteExW_hook,
                        (void**)&ShellExecuteExW_orig);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}
