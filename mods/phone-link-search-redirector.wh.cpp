// ==WindhawkMod==
// @id              phone-link-search-redirector
// @name            Phone Link Redirector
// @description     Redirect Bing searches opened by Phone Link to another search engine.
// @version         1.0.0
// @author          Rieversed
// @github          https://github.com/Rieversed
// @include         PhoneExperienceHost.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Phone Link Search Redirector

Redirects Bing searches opened by Microsoft Phone Link to another search engine.

The mod only runs inside PhoneExperienceHost.exe and only modifies command
lines containing a Bing search URL.

Supported search engines:
- DuckDuckGo
- Brave Search
- Google
- Kagi
- Ecosia
- Startpage
- Yahoo
- Custom URL template

For Custom, use {q} as the query placeholder.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- engine: duckduckgo
  $name: Search engine
  $options:
  - duckduckgo: DuckDuckGo
  - brave: Brave Search
  - google: Google
  - kagi: Kagi
  - ecosia: Ecosia
  - startpage: Startpage
  - yahoo: Yahoo
  - custom: Custom

- customTemplate: https://duckduckgo.com/?q={q}
  $name: Custom URL template
  $description: Used when Custom is selected. Use {q} as the search query placeholder.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <vector>

#include <windhawk_api.h>

static const wchar_t* g_engine = L"duckduckgo";
static const wchar_t* g_customTemplate = L"https://duckduckgo.com/?q={q}";

typedef BOOL(WINAPI* CreateProcessW_t)(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
);

static CreateProcessW_t CreateProcessW_Original = nullptr;


// ------------------------------------------------------------
// URL helpers
// ------------------------------------------------------------

static int HexValue(wchar_t c)
{
    if (c >= L'0' && c <= L'9')
        return c - L'0';

    if (c >= L'a' && c <= L'f')
        return c - L'a' + 10;

    if (c >= L'A' && c <= L'F')
        return c - L'A' + 10;

    return -1;
}

static std::wstring UrlDecode(const std::wstring& value)
{
    std::wstring result;
    result.reserve(value.size());

    for (size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == L'%' && i + 2 < value.size())
        {
            int high = HexValue(value[i + 1]);
            int low = HexValue(value[i + 2]);

            if (high >= 0 && low >= 0)
            {
                result += static_cast<wchar_t>((high << 4) | low);
                i += 2;
                continue;
            }
        }

        if (value[i] == L'+')
        {
            result += L' ';
        }
        else
        {
            result += value[i];
        }
    }

    return result;
}

static std::wstring UrlEncode(const std::wstring& value)
{
    const wchar_t* hex = L"0123456789ABCDEF";

    std::wstring result;

    for (wchar_t c : value)
    {
        // Keep normal URL-safe characters.
        if (
            (c >= L'a' && c <= L'z') ||
            (c >= L'A' && c <= L'Z') ||
            (c >= L'0' && c <= L'9') ||
            c == L'-' ||
            c == L'_' ||
            c == L'.' ||
            c == L'~'
        )
        {
            result += c;
        }
        else if (c == L' ')
        {
            result += L'+';
        }
        else
        {
            // Encode UTF-16 characters as UTF-8 first.
            char utf8[4];
            int utf8Length = WideCharToMultiByte(
                CP_UTF8,
                0,
                &c,
                1,
                utf8,
                sizeof(utf8),
                nullptr,
                nullptr
            );

            if (utf8Length > 0)
            {
                for (int i = 0; i < utf8Length; i++)
                {
                    unsigned char byte =
                        static_cast<unsigned char>(utf8[i]);

                    result += L'%';
                    result += hex[(byte >> 4) & 0xF];
                    result += hex[byte & 0xF];
                }
            }
        }
    }

    return result;
}


// ------------------------------------------------------------
// Bing URL detection
// ------------------------------------------------------------

static bool FindBingUrl(
    const std::wstring& commandLine,
    size_t& urlStart,
    size_t& urlEnd
)
{
    const wchar_t* prefixes[] =
    {
        L"https://www.bing.com/",
        L"https://bing.com/",
        L"http://www.bing.com/",
        L"http://bing.com/"
    };

    size_t found = std::wstring::npos;

    for (const wchar_t* prefix : prefixes)
    {
        size_t position = commandLine.find(prefix);

        if (position != std::wstring::npos &&
            (found == std::wstring::npos || position < found))
        {
            found = position;
        }
    }

    if (found == std::wstring::npos)
        return false;

    // Make sure this is actually a Bing search URL.
    size_t searchPosition = commandLine.find(L"/search?q=", found);

    if (searchPosition == std::wstring::npos)
        return false;

    urlStart = found;

    // Find the end of the URL.
    //
    // Phone Link normally passes it as:
    //
    // "https://www.bing.com/search?q=..."
    //
    // so the closing quote is the most reliable delimiter.

    size_t position = searchPosition;

    while (position < commandLine.size())
    {
        wchar_t c = commandLine[position];

        if (c == L'"' ||
            c == L'\'' ||
            c == L' ' ||
            c == L'\t')
        {
            break;
        }

        position++;
    }

    urlEnd = position;

    return urlEnd > urlStart;
}


static bool ExtractQueryFromBingUrl(
    const std::wstring& url,
    std::wstring& query
)
{
    size_t queryStart = url.find(L"?q=");

    if (queryStart == std::wstring::npos)
        queryStart = url.find(L"&q=");

    if (queryStart == std::wstring::npos)
        return false;

    queryStart += 3;

    size_t queryEnd = url.find(L'&', queryStart);

    if (queryEnd == std::wstring::npos)
        queryEnd = url.size();

    if (queryEnd <= queryStart)
        return false;

    std::wstring encodedQuery =
        url.substr(queryStart, queryEnd - queryStart);

    query = UrlDecode(encodedQuery);

    return !query.empty();
}


// ------------------------------------------------------------
// Search engine URL creation
// ------------------------------------------------------------

static std::wstring BuildSearchUrl(const std::wstring& query)
{
    std::wstring encodedQuery = UrlEncode(query);

    if (_wcsicmp(g_engine, L"duckduckgo") == 0)
    {
        return L"https://duckduckgo.com/?q=" + encodedQuery;
    }

    if (_wcsicmp(g_engine, L"brave") == 0)
    {
        return L"https://search.brave.com/search?q=" + encodedQuery;
    }

    if (_wcsicmp(g_engine, L"google") == 0)
    {
        return L"https://www.google.com/search?q=" + encodedQuery;
    }

    if (_wcsicmp(g_engine, L"kagi") == 0)
    {
        return L"https://kagi.com/search?q=" + encodedQuery;
    }

    if (_wcsicmp(g_engine, L"ecosia") == 0)
    {
        return L"https://www.ecosia.org/search?q=" + encodedQuery;
    }

    if (_wcsicmp(g_engine, L"startpage") == 0)
    {
        return L"https://www.startpage.com/sp/search?query=" +
               encodedQuery;
    }

    if (_wcsicmp(g_engine, L"yahoo") == 0)
    {
        return L"https://search.yahoo.com/search?p=" + encodedQuery;
    }

    if (_wcsicmp(g_engine, L"custom") == 0)
    {
        std::wstring result = g_customTemplate;

        size_t position = result.find(L"{q}");

        while (position != std::wstring::npos)
        {
            result.replace(position, 3, encodedQuery);

            position = result.find(L"{q}", position + encodedQuery.size());
        }

        return result;
    }

    // Safe fallback.
    return L"https://duckduckgo.com/?q=" + encodedQuery;
}


// ------------------------------------------------------------
// Command line rewriting
// ------------------------------------------------------------

static bool RewriteCommandLine(std::wstring& commandLine)
{
    size_t urlStart;
    size_t urlEnd;

    if (!FindBingUrl(commandLine, urlStart, urlEnd))
        return false;

    std::wstring oldUrl =
        commandLine.substr(urlStart, urlEnd - urlStart);

    std::wstring query;

    if (!ExtractQueryFromBingUrl(oldUrl, query))
        return false;

    std::wstring newUrl = BuildSearchUrl(query);

    if (newUrl.empty())
        return false;

    commandLine.replace(
        urlStart,
        urlEnd - urlStart,
        newUrl
    );

    Wh_Log(
        L"[PhoneLinkRedirector] REDIRECT: \"%s\" -> \"%s\"",
        oldUrl.c_str(),
        newUrl.c_str()
    );

    Wh_Log(
        L"[PhoneLinkRedirector] Query: \"%s\"",
        query.c_str()
    );

    Wh_Log(
        L"[PhoneLinkRedirector] New command line: %s",
        commandLine.c_str()
    );

    return true;
}


// ------------------------------------------------------------
// CreateProcessW hook
// ------------------------------------------------------------

BOOL WINAPI CreateProcessW_Hook(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
)
{
    if (!lpCommandLine)
    {
        return CreateProcessW_Original(
            lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,
            lpThreadAttributes,
            bInheritHandles,
            dwCreationFlags,
            lpEnvironment,
            lpCurrentDirectory,
            lpStartupInfo,
            lpProcessInformation
        );
    }

    std::wstring commandLine(lpCommandLine);

    if (!RewriteCommandLine(commandLine))
    {
        return CreateProcessW_Original(
            lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,
            lpThreadAttributes,
            bInheritHandles,
            dwCreationFlags,
            lpEnvironment,
            lpCurrentDirectory,
            lpStartupInfo,
            lpProcessInformation
        );
    }

    // CreateProcessW requires a writable command-line buffer.
    std::vector<wchar_t> writableCommandLine(
        commandLine.begin(),
        commandLine.end()
    );

    writableCommandLine.push_back(L'\0');

    return CreateProcessW_Original(
        lpApplicationName,
        writableCommandLine.data(),
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );
}


// ------------------------------------------------------------
// Settings
// ------------------------------------------------------------

static void LoadSettings()
{
    PCWSTR engine = Wh_GetStringSetting(L"engine");

    if (engine)
    {
        static std::wstring engineStorage;
        engineStorage = engine;

        g_engine = engineStorage.c_str();

        Wh_FreeStringSetting(engine);
    }

    PCWSTR customTemplate =
        Wh_GetStringSetting(L"customTemplate");

    if (customTemplate)
    {
        static std::wstring customTemplateStorage;
        customTemplateStorage = customTemplate;

        g_customTemplate = customTemplateStorage.c_str();

        Wh_FreeStringSetting(customTemplate);
    }

    Wh_Log(
        L"[PhoneLinkRedirector] Engine: %s",
        g_engine
    );

    Wh_Log(
        L"[PhoneLinkRedirector] Custom template: %s",
        g_customTemplate
    );
}


// ------------------------------------------------------------
// Mod lifecycle
// ------------------------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(L"[PhoneLinkRedirector] Initializing");

    LoadSettings();

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");

    if (!kernel32)
    {
        Wh_Log(
            L"[PhoneLinkRedirector] Failed to get kernel32.dll"
        );

        return FALSE;
    }

    FARPROC createProcess =
        GetProcAddress(kernel32, "CreateProcessW");

    if (!createProcess)
    {
        Wh_Log(
            L"[PhoneLinkRedirector] Failed to find CreateProcessW"
        );

        return FALSE;
    }

    Wh_Log(
        L"[PhoneLinkRedirector] Installing CreateProcessW hook"
    );

    if (!Wh_SetFunctionHook(
        reinterpret_cast<void*>(createProcess),
        reinterpret_cast<void*>(CreateProcessW_Hook),
        reinterpret_cast<void**>(&CreateProcessW_Original)
    ))
    {
        Wh_Log(
            L"[PhoneLinkRedirector] Failed to install CreateProcessW hook"
        );

        return FALSE;
    }

    Wh_Log(
        L"[PhoneLinkRedirector] CreateProcessW hook installed"
    );

    return TRUE;
}


void Wh_ModUninit()
{
    Wh_Log(
        L"[PhoneLinkRedirector] Uninitializing"
    );
}


void Wh_ModSettingsChanged()
{
    LoadSettings();

    Wh_Log(
        L"[PhoneLinkRedirector] Settings changed"
    );
}
