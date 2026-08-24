// ==WindhawkMod==
// @id              start-search-bing-redirector
// @name            Start Search Bing Redirector
// @description     Redirects Bing web-result clicks in the Start menu search to Google (or another engine) and opens them in your default browser instead of Edge.
// @version         1.0.0
// @author          takattowo
// @github          https://github.com/takattowo
// @homepage        https://takattowo.github.io/
// @include         SearchHost.exe
// @include         SearchApp.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Search Bing Redirector

When you click a web result in the Windows Start menu search, Windows forces
the URL through `microsoft-edge:?url=https://www.bing.com/search?q=...` so it
always opens in Edge with Bing.

This mod intercepts that hand-off, extracts the search query, and redirects
it to your chosen engine. The `microsoft-edge:` wrapper is dropped so the
URL opens in your **default browser**.

## How it works

Hooks `WindowsCreateString` in `combase.dll` inside `SearchHost.exe`. When the
HSTRING being constructed is the click activation URL, the URL is rewritten
in place before it reaches the WinRT URI launcher.

The in-flyout result previews (the right panel) are **not** changed. Those
are rendered from Bing's JSON API and would need a full API proxy to swap.
Only the click hand-off is redirected.

## Logs

Open Windhawk's log viewer and filter for `[BingRedirector]`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- engine: google
  $name: Search engine
  $options:
    - google: Google
    - duckduckgo: DuckDuckGo
    - kagi: Kagi
    - brave: Brave Search
    - ecosia: Ecosia
    - yahoo: Yahoo
    - startpage: Startpage
    - custom: Custom (use template below)

- customTemplate: https://www.google.com/search?q={q}
  $name: Custom URL template
  $description: "Used only when engine is set to Custom. Use {q} as the query placeholder."
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <winstring.h>

#include <string>
#include <string_view>

#define LOG_TAG L"[BingRedirector] "

enum class Engine {
    Google, DuckDuckGo, Kagi, Brave, Ecosia, Yahoo, Startpage, Custom,
};

struct Settings {
    Engine engine = Engine::Google;
    std::wstring customTemplate = L"https://www.google.com/search?q={q}";
};

static Settings g_settings;
static thread_local bool g_inHook = false;

static Engine ParseEngine(PCWSTR s) {
    if (!s) return Engine::Google;
    if (_wcsicmp(s, L"duckduckgo") == 0) return Engine::DuckDuckGo;
    if (_wcsicmp(s, L"kagi") == 0)       return Engine::Kagi;
    if (_wcsicmp(s, L"brave") == 0)      return Engine::Brave;
    if (_wcsicmp(s, L"ecosia") == 0)     return Engine::Ecosia;
    if (_wcsicmp(s, L"yahoo") == 0)      return Engine::Yahoo;
    if (_wcsicmp(s, L"startpage") == 0)  return Engine::Startpage;
    if (_wcsicmp(s, L"custom") == 0)     return Engine::Custom;
    return Engine::Google;
}

static const wchar_t* EngineBaseUrl(Engine e) {
    switch (e) {
        case Engine::Google:     return L"https://www.google.com/search?q=";
        case Engine::DuckDuckGo: return L"https://duckduckgo.com/?q=";
        case Engine::Kagi:       return L"https://kagi.com/search?q=";
        case Engine::Brave:      return L"https://search.brave.com/search?q=";
        case Engine::Ecosia:     return L"https://www.ecosia.org/search?q=";
        case Engine::Yahoo:      return L"https://search.yahoo.com/search?p=";
        case Engine::Startpage:  return L"https://www.startpage.com/do/search?query=";
        case Engine::Custom:     return nullptr;
    }
    return nullptr;
}

static void LoadSettings() {
    PCWSTR engine = Wh_GetStringSetting(L"engine");
    g_settings.engine = ParseEngine(engine);
    if (engine) Wh_FreeStringSetting(engine);

    PCWSTR tmpl = Wh_GetStringSetting(L"customTemplate");
    g_settings.customTemplate = (tmpl && *tmpl)
        ? std::wstring(tmpl)
        : std::wstring(L"https://www.google.com/search?q={q}");
    if (tmpl) Wh_FreeStringSetting(tmpl);
}

static std::wstring UrlEncode(std::wstring_view in) {
    int n = WideCharToMultiByte(CP_UTF8, 0, in.data(), (int)in.size(),
                                nullptr, 0, nullptr, nullptr);
    if (n <= 0) return std::wstring(in);
    std::string utf8(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, in.data(), (int)in.size(),
                        utf8.data(), n, nullptr, nullptr);
    std::wstring out;
    out.reserve(in.size());
    char buf[8];
    for (unsigned char b : utf8) {
        if ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') ||
            (b >= '0' && b <= '9') ||
            b == '-' || b == '_' || b == '.' || b == '~') {
            out.push_back((wchar_t)b);
        } else {
            wsprintfA(buf, "%%%02X", b);
            for (int i = 0; buf[i]; ++i) out.push_back((wchar_t)buf[i]);
        }
    }
    return out;
}

static std::wstring UrlDecode(std::wstring_view in) {
    std::string utf8;
    utf8.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        wchar_t c = in[i];
        if (c == L'+') {
            utf8.push_back(' ');
        } else if (c == L'%' && i + 2 < in.size() &&
                   iswxdigit(in[i + 1]) && iswxdigit(in[i + 2])) {
            auto hex = [](wchar_t h) -> int {
                if (h >= L'0' && h <= L'9') return h - L'0';
                if (h >= L'a' && h <= L'f') return 10 + (h - L'a');
                if (h >= L'A' && h <= L'F') return 10 + (h - L'A');
                return 0;
            };
            utf8.push_back((char)((hex(in[i + 1]) << 4) | hex(in[i + 2])));
            i += 2;
        } else if (c < 128) {
            utf8.push_back((char)c);
        } else {
            char buf[8];
            int n = WideCharToMultiByte(CP_UTF8, 0, &c, 1, buf, sizeof(buf),
                                        nullptr, nullptr);
            if (n > 0) utf8.append(buf, n);
        }
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(),
                                nullptr, 0);
    if (n <= 0) return std::wstring(in);
    std::wstring out(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(),
                        out.data(), n);
    return out;
}

static std::wstring BuildEngineUrl(std::wstring_view encodedQuery) {
    if (g_settings.engine == Engine::Custom) {
        std::wstring out = g_settings.customTemplate;
        const std::wstring placeholder = L"{q}";
        size_t pos = out.find(placeholder);
        if (pos == std::wstring::npos) {
            out.append(encodedQuery);
        } else {
            out.replace(pos, placeholder.size(), encodedQuery);
        }
        return out;
    }
    std::wstring out = EngineBaseUrl(g_settings.engine);
    out.append(encodedQuery);
    return out;
}

static bool StartsWithI(std::wstring_view s, std::wstring_view prefix) {
    if (s.size() < prefix.size()) return false;
    return _wcsnicmp(s.data(), prefix.data(), prefix.size()) == 0;
}

static size_t FindI(std::wstring_view hay, std::wstring_view needle,
                    size_t from = 0) {
    if (needle.empty() || from > hay.size()) return std::wstring::npos;
    for (size_t i = from; i + needle.size() <= hay.size(); ++i) {
        if (_wcsnicmp(hay.data() + i, needle.data(), needle.size()) == 0)
            return i;
    }
    return std::wstring::npos;
}

// Find query parameter value range. Boundary-aware (key must follow ? or &).
static std::pair<size_t, size_t> FindQueryParam(std::wstring_view s,
                                                std::wstring_view key) {
    size_t from = 0;
    while (from < s.size()) {
        size_t pos = FindI(s, key, from);
        if (pos == std::wstring::npos) break;
        if ((pos == 0 || s[pos - 1] == L'?' || s[pos - 1] == L'&') &&
            pos + key.size() < s.size() && s[pos + key.size()] == L'=') {
            size_t valStart = pos + key.size() + 1;
            size_t valEnd = s.find_first_of(L"&#", valStart);
            if (valEnd == std::wstring::npos) valEnd = s.size();
            return {valStart, valEnd};
        }
        from = pos + key.size();
    }
    return {std::wstring::npos, std::wstring::npos};
}

// If url is bing.com/search?q=..., return engine URL for that query.
static std::wstring RewriteBingHttpUrl(std::wstring_view url) {
    if (FindI(url, L"bing.com/search?") == std::wstring::npos) return {};
    auto [qStart, qEnd] = FindQueryParam(url, L"q");
    if (qStart == std::wstring::npos) return {};
    std::wstring qRaw(url.substr(qStart, qEnd - qStart));
    std::wstring decoded = UrlDecode(qRaw);
    std::wstring encoded = UrlEncode(decoded);
    return BuildEngineUrl(encoded);
}

// Match the Start search click handoff:
//   microsoft-edge:?url=<URL-encoded Bing search URL>&...
// Return bare engine URL, or empty if input is not that pattern.
static std::wstring TryRewriteActivationUrl(std::wstring_view inUrl) {
    constexpr std::wstring_view edgePrefix = L"microsoft-edge:";
    if (!StartsWithI(inUrl, edgePrefix)) return {};

    std::wstring_view rest = inUrl;
    rest.remove_prefix(edgePrefix.size());
    if (rest.empty() || rest[0] != L'?') return {};

    auto [valStart, valEnd] = FindQueryParam(rest, L"url");
    if (valStart == std::wstring::npos) return {};

    std::wstring decInner = UrlDecode(rest.substr(valStart, valEnd - valStart));
    std::wstring rewritten = RewriteBingHttpUrl(decInner);
    if (rewritten.empty()) return {};

    Wh_Log(LOG_TAG L"Rewrote: %ls -> %ls",
           std::wstring(inUrl).c_str(), rewritten.c_str());
    return rewritten;
}

using WindowsCreateString_t = HRESULT(WINAPI*)(PCNZWCH, UINT32, HSTRING*);
WindowsCreateString_t WindowsCreateString_Original;

HRESULT WINAPI WindowsCreateString_Hook(PCNZWCH sourceString, UINT32 length,
                                       HSTRING* string) {
    if (!sourceString || length < 15 || g_inHook) {
        return WindowsCreateString_Original(sourceString, length, string);
    }
    // Fast filter: only consider HSTRINGs that start with microsoft-edge:.
    if (_wcsnicmp(sourceString, L"microsoft-edge:", 15) != 0) {
        return WindowsCreateString_Original(sourceString, length, string);
    }

    g_inHook = true;
    std::wstring in(sourceString, length);
    std::wstring rewritten = TryRewriteActivationUrl(in);
    g_inHook = false;

    if (!rewritten.empty()) {
        return WindowsCreateString_Original(
            rewritten.c_str(), (UINT32)rewritten.size(), string);
    }
    return WindowsCreateString_Original(sourceString, length, string);
}

BOOL Wh_ModInit() {
    Wh_Log(LOG_TAG L"Init");
    LoadSettings();

    HMODULE hCombase = GetModuleHandleW(L"combase.dll");
    if (!hCombase) hCombase = LoadLibraryW(L"combase.dll");
    if (!hCombase) {
        Wh_Log(LOG_TAG L"combase.dll not available");
        return FALSE;
    }
    auto p = (WindowsCreateString_t)GetProcAddress(hCombase,
                                                   "WindowsCreateString");
    if (!p) {
        Wh_Log(LOG_TAG L"WindowsCreateString not found");
        return FALSE;
    }
    WindhawkUtils::Wh_SetFunctionHookT(p, WindowsCreateString_Hook,
                                       &WindowsCreateString_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
