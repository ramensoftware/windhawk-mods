// ==WindhawkMod==
// @id              windows-search-default-browser
// @name            Windows Search - Search Engine Redirect
// @description     Redirects Windows Search web-result clicks to a configurable search engine and the registered HTTPS handler.
// @version         1.1.0
// @github          https://github.com/XRedxFoxx
// @author          XRedxFoxx
// @homepage         https://github.com/XRedxFoxx/windows-search-default-browser
// @include          SearchHost.exe
// @include          SearchApp.exe
// @architecture     x86-64
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- engine: google
  $name: Search engine
  $options:
  - google: Google
  - bing: Bing
  - duckduckgo: DuckDuckGo
  - yahoo: Yahoo
  - brave: Brave Search
  - ecosia: Ecosia
  - startpage: Startpage
  - custom: Custom
- customTemplate: "https://www.google.com/search?q={query}"
  $name: Custom search URL template
  $description: "Used only with Custom. Include {query}; it is replaced with the URL-encoded search query."
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Windows Search - Search Engine Redirect

Extracts the search query from supported Bing web-result activations and
constructs a URL for the selected search engine. The resulting ordinary
HTTP(S) URL is passed back to Windows Search's activation flow and should open
with the registered handler (normally the user's default browser).

Supported engines: Google, Bing, DuckDuckGo, Yahoo, Brave Search, Ecosia,
Startpage, and Custom. Custom templates must contain `{query}`.

This changes clicked web-result destinations only. It does not replace Bing's
suggestions, previews, or results displayed inside the Search flyout.

Unsupported activation formats are passed through unchanged.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <winstring.h>

#include <string>
#include <string_view>
#include <utility>
#include <mutex>
#include <shared_mutex>

namespace {

constexpr std::wstring_view kEdgePrefix = L"microsoft-edge:";

enum class SearchEngine { Google, Bing, DuckDuckGo, Yahoo, Brave, Ecosia, Startpage, Custom };
struct Settings {
    SearchEngine engine = SearchEngine::Google;
    std::wstring customTemplate = L"https://www.google.com/search?q={query}";
};
Settings g_settings;
std::shared_mutex g_settingsMutex;

using WindowsCreateString_t = HRESULT(WINAPI*)(PCNZWCH, UINT32, HSTRING*);
WindowsCreateString_t g_windowsCreateStringOriginal = nullptr;
thread_local bool g_insideHook = false;

bool StartsWithInsensitive(std::wstring_view value,
                           std::wstring_view prefix) {
    return value.size() >= prefix.size() &&
           _wcsnicmp(value.data(), prefix.data(), prefix.size()) == 0;
}

int HexValue(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    return -1;
}

// Decodes one query-string layer. '+' represents a space at this outer layer.
// Percent-encoded bytes are interpreted as UTF-8.
bool UrlDecodeOnce(std::wstring_view input, std::wstring* output) {
    std::string utf8;
    utf8.reserve(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        const wchar_t c = input[i];
        if (c == L'%') {
            if (i + 2 >= input.size()) return false;
            const int hi = HexValue(input[i + 1]);
            const int lo = HexValue(input[i + 2]);
            if (hi < 0 || lo < 0) return false;
            utf8.push_back(static_cast<char>((hi << 4) | lo));
            i += 2;
        } else if (c == L'+') {
            utf8.push_back(' ');
        } else if (c <= 0x7F) {
            utf8.push_back(static_cast<char>(c));
        } else {
            // Preserve unescaped Unicode by converting the current UTF-16
            // code point (including a possible surrogate pair) to UTF-8.
            wchar_t chars[2] = {c, 0};
            int charCount = 1;
            if (c >= 0xD800 && c <= 0xDBFF && i + 1 < input.size() &&
                input[i + 1] >= 0xDC00 && input[i + 1] <= 0xDFFF) {
                chars[1] = input[++i];
                charCount = 2;
            }

            char buffer[8] = {};
            const int bytes = WideCharToMultiByte(
                CP_UTF8, WC_ERR_INVALID_CHARS, chars, charCount,
                buffer, static_cast<int>(sizeof(buffer)), nullptr, nullptr);
            if (bytes <= 0) return false;
            utf8.append(buffer, bytes);
        }
    }

    if (utf8.empty()) {
        output->clear();
        return true;
    }

    const int wideLength = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(),
        static_cast<int>(utf8.size()), nullptr, 0);
    if (wideLength <= 0) return false;

    output->resize(wideLength);
    return MultiByteToWideChar(
               CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(),
               static_cast<int>(utf8.size()), output->data(), wideLength) > 0;
}

// Finds a named parameter in a URI query string. The returned view ends at
// the next '&' or '#', so encoded inner URL separators (%26) remain intact.
std::wstring_view FindParameter(std::wstring_view query,
                                std::wstring_view name) {
    size_t position = 0;
    while (position <= query.size()) {
        const size_t end = query.find_first_of(L"&#", position);
        const size_t partEnd = end == std::wstring_view::npos
                                   ? query.size()
                                   : end;
        const std::wstring_view part = query.substr(position, partEnd - position);
        const size_t equals = part.find(L'=');
        if (equals != std::wstring_view::npos && equals == name.size() &&
            _wcsnicmp(part.data(), name.data(), name.size()) == 0) {
            return part.substr(equals + 1);
        }
        if (end == std::wstring_view::npos || query[end] == L'#') break;
        position = end + 1;
    }
    return {};
}

bool IsHttpUrl(std::wstring_view url) {
    return StartsWithInsensitive(url, L"https://") ||
           StartsWithInsensitive(url, L"http://");
}

SearchEngine ParseEngine(PCWSTR value) {
    if (!value) return SearchEngine::Google;
    if (_wcsicmp(value, L"bing") == 0) return SearchEngine::Bing;
    if (_wcsicmp(value, L"duckduckgo") == 0) return SearchEngine::DuckDuckGo;
    if (_wcsicmp(value, L"yahoo") == 0) return SearchEngine::Yahoo;
    if (_wcsicmp(value, L"brave") == 0) return SearchEngine::Brave;
    if (_wcsicmp(value, L"ecosia") == 0) return SearchEngine::Ecosia;
    if (_wcsicmp(value, L"startpage") == 0) return SearchEngine::Startpage;
    if (_wcsicmp(value, L"custom") == 0) return SearchEngine::Custom;
    return SearchEngine::Google;
}

void LoadSettings() {
    Settings updated;
    PCWSTR engine = Wh_GetStringSetting(L"engine");
    updated.engine = ParseEngine(engine);
    if (engine) Wh_FreeStringSetting(engine);

    PCWSTR customTemplate = Wh_GetStringSetting(L"customTemplate");
    if (customTemplate && *customTemplate) updated.customTemplate = customTemplate;
    if (customTemplate) Wh_FreeStringSetting(customTemplate);

    std::unique_lock lock(g_settingsMutex);
    g_settings = std::move(updated);
}

std::wstring UrlEncodeQuery(std::wstring_view input) {
    if (input.empty()) return {};
    const int bytesNeeded = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
        input.data(), static_cast<int>(input.size()), nullptr, 0, nullptr, nullptr);
    if (bytesNeeded <= 0) return {};
    std::string utf8(bytesNeeded, '\0');
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, input.data(),
        static_cast<int>(input.size()), utf8.data(), bytesNeeded, nullptr, nullptr) <= 0) return {};

    constexpr wchar_t hex[] = L"0123456789ABCDEF";
    std::wstring encoded;
    encoded.reserve(utf8.size() * 3);
    for (unsigned char c : utf8) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded.push_back(static_cast<wchar_t>(c));
        } else {
            encoded.push_back(L'%');
            encoded.push_back(hex[c >> 4]);
            encoded.push_back(hex[c & 0x0F]);
        }
    }
    return encoded;
}

bool IsValidTemplate(std::wstring_view value) {
    return IsHttpUrl(value) && value.find(L"{query}") != std::wstring_view::npos &&
           value.find_first_of(L"\r\n\t") == std::wstring_view::npos;
}

std::wstring BuildSearchUrl(std::wstring_view query) {
    Settings settings;
    {
        std::shared_lock lock(g_settingsMutex);
        settings = g_settings;
    }

    std::wstring url;
    switch (settings.engine) {
        case SearchEngine::Google: url = L"https://www.google.com/search?q={query}"; break;
        case SearchEngine::Bing: url = L"https://www.bing.com/search?q={query}"; break;
        case SearchEngine::DuckDuckGo: url = L"https://duckduckgo.com/?q={query}"; break;
        case SearchEngine::Yahoo: url = L"https://search.yahoo.com/search?p={query}"; break;
        case SearchEngine::Brave: url = L"https://search.brave.com/search?q={query}"; break;
        case SearchEngine::Ecosia: url = L"https://www.ecosia.org/search?q={query}"; break;
        case SearchEngine::Startpage: url = L"https://www.startpage.com/sp/search?query={query}"; break;
        case SearchEngine::Custom: url = settings.customTemplate; break;
    }

    if (!IsValidTemplate(url)) {
        Wh_Log(L"[SearchDefaultBrowser] Invalid custom template; using Google");
        url = L"https://www.google.com/search?q={query}";
    }

    const std::wstring encoded = UrlEncodeQuery(query);
    const std::wstring placeholder = L"{query}";
    size_t pos = 0;
    while ((pos = url.find(placeholder, pos)) != std::wstring::npos) {
        url.replace(pos, placeholder.size(), encoded);
        pos += encoded.size();
    }
    return url;
}

bool IsBingSearchUrl(std::wstring_view url) {
    if (!IsHttpUrl(url)) return false;
    const size_t scheme = url.find(L"://");
    if (scheme == std::wstring_view::npos) return false;
    const size_t hostStart = scheme + 3;
    const size_t pathStart = url.find_first_of(L"/?#", hostStart);
    const std::wstring_view host = url.substr(hostStart,
        pathStart == std::wstring_view::npos ? url.size() - hostStart : pathStart - hostStart);
    if (_wcsicmp(std::wstring(host).c_str(), L"www.bing.com") != 0 &&
        _wcsicmp(std::wstring(host).c_str(), L"bing.com") != 0) return false;
    if (pathStart == std::wstring_view::npos || url[pathStart] != L'/') return false;
    const size_t pathEnd = url.find_first_of(L"?#", pathStart);
    const std::wstring_view path = url.substr(pathStart,
        pathEnd == std::wstring_view::npos ? url.size() - pathStart : pathEnd - pathStart);
    return _wcsicmp(std::wstring(path).c_str(), L"/search") == 0;
}

// Extract the query from a supported Bing search URL and build the selected
// engine's URL. The original query is decoded once, then encoded once for the
// destination engine (UTF-8 / RFC 3986).
bool TryRewriteEdgeUrl(std::wstring_view activationUrl, std::wstring* newUrl) {
    if (!StartsWithInsensitive(activationUrl, kEdgePrefix)) return false;

    const std::wstring_view remainder = activationUrl.substr(kEdgePrefix.size());
    const size_t question = remainder.find(L'?');
    if (question == std::wstring_view::npos) return false;

    const std::wstring_view parameters = remainder.substr(question + 1);
    std::wstring_view rawValue;

    // Prefer the common `?url=https://...` form. When the embedded URL is
    // already readable, its own '&' characters belong to that URL, not to
    // the outer microsoft-edge URI.
    if (StartsWithInsensitive(parameters, L"url=")) {
        rawValue = parameters.substr(4);
    } else {
        rawValue = FindParameter(parameters, L"url");
    }
    if (rawValue.empty()) return false;

    // Strip only an outer fragment. Do not split a readable HTTPS URL at '&'
    // because those characters may delimit its own query parameters.
    const size_t fragment = rawValue.find(L'#');
    if (fragment != std::wstring_view::npos) {
        rawValue = rawValue.substr(0, fragment);
    }

    std::wstring originalUrl;
    if (IsHttpUrl(rawValue)) {
        originalUrl.assign(rawValue);
    } else {
        // Encoded wrapper form: decode the wrapper once. In this form an
        // unescaped '&' ends the outer parameter value.
        const size_t outerEnd = rawValue.find(L'&');
        if (outerEnd != std::wstring_view::npos) {
            rawValue = rawValue.substr(0, outerEnd);
        }
        if (!UrlDecodeOnce(rawValue, &originalUrl) || !IsHttpUrl(originalUrl)) {
            return false;
        }
    }

    // Keep the original mod's broad behavior: every supported HTTP(S) URL
    // is unwrapped so Windows can activate it through the registered handler.
    // Search-engine rewriting is an additional step only for a recognized
    // Bing search URL with an extractable q parameter.
    if (IsBingSearchUrl(originalUrl)) {
        const size_t queryMark = originalUrl.find(L'?');
        if (queryMark != std::wstring::npos) {
            const std::wstring_view rawQuery =
                FindParameter(std::wstring_view(originalUrl).substr(queryMark + 1), L"q");
            if (!rawQuery.empty()) {
                std::wstring query;
                if (UrlDecodeOnce(rawQuery, &query)) {
                    std::wstring rewritten = BuildSearchUrl(query);
                    if (IsHttpUrl(rewritten)) {
                        *newUrl = std::move(rewritten);
                        return true;
                    }
                }
            }
        }
    }

    // Crucial fallback: preserve the original HTTPS destination rather than
    // passing microsoft-edge: through to Windows Search (which launches Edge).
    *newUrl = std::move(originalUrl);
    return true;
}


}  // namespace

HRESULT WINAPI WindowsCreateString_Hook(PCNZWCH sourceString,
                                        UINT32 length,
                                        HSTRING* string) {
    if (!g_windowsCreateStringOriginal) {
        return E_FAIL;
    }

    if (!sourceString || length < kEdgePrefix.size() || g_insideHook ||
        _wcsnicmp(sourceString, kEdgePrefix.data(),
                  kEdgePrefix.size()) != 0) {
        return g_windowsCreateStringOriginal(sourceString, length, string);
    }

    g_insideHook = true;
    HRESULT result = E_FAIL;

    try {
        const std::wstring incoming(sourceString, length);
        std::wstring unwrapped;
        if (TryRewriteEdgeUrl(incoming, &unwrapped)) {
            Wh_Log(L"[SearchDefaultBrowser] Rewrote Windows Search URL: %ls",
                   unwrapped.c_str());
            result = g_windowsCreateStringOriginal(
                unwrapped.c_str(), static_cast<UINT32>(unwrapped.size()), string);
        } else {
            Wh_Log(L"[SearchDefaultBrowser] Unsupported Edge/Bing activation URL; passing through");
            result = g_windowsCreateStringOriginal(sourceString, length, string);
        }
    } catch (...) {
        Wh_Log(L"[SearchDefaultBrowser] Exception while processing activation URL; passing through");
        result = g_windowsCreateStringOriginal(sourceString, length, string);
    }

    g_insideHook = false;
    return result;
}

BOOL Wh_ModInit() {
    Wh_Log(L"[SearchDefaultBrowser] Initializing");
    LoadSettings();

    HMODULE combase = GetModuleHandleW(L"combase.dll");
    if (!combase) {
        combase = LoadLibraryW(L"combase.dll");
    }
    if (!combase) {
        Wh_Log(L"[SearchDefaultBrowser] Failed to load combase.dll");
        return FALSE;
    }

    const auto target = reinterpret_cast<WindowsCreateString_t>(
        GetProcAddress(combase, "WindowsCreateString"));
    if (!target) {
        Wh_Log(L"[SearchDefaultBrowser] WindowsCreateString export not found");
        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(
        target, WindowsCreateString_Hook, &g_windowsCreateStringOriginal);

    Wh_Log(L"[SearchDefaultBrowser] Hook installed");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"[SearchDefaultBrowser] Settings updated");
}
