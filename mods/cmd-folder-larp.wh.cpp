// ==WindhawkMod==
// @id              cmd-folder-larp
// @name            Command Prompt Folder Larp
// @description     Make Command Prompt show different names for folders.
// @version         1.0
// @author          Isabella Lulamoon (kawapure)
// @github          https://github.com/kawapure
// @twitter         https://twitter.com/kawaipure
// @homepage        https://kawapure.github.io/
// @include         cmd.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Command Prompt Folder Larp

Make the current directory in Command Prompt appear as though it's a different folder.

This is an eyecandy tweak to make it seem like an older version of Windows, where, for example,
the "Users" folder was known as "Documents and Settings".

This mod does not currently change the behaviour of `cd` or `dir` commands.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replace_in_title: true
  $name: Replace in application title
  $description: This will allow you to change the capitalisation of "C:\Windows\System32\cmd.exe", among other uses. 
- replacements:
  - - search: C:\Users\
      $name: Path to be replaced
    - replacement: C:\Documents and Settings\
      $name: Name to be replaced with
  $name: "Paths to replace:"
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <string>
#include <algorithm>
#include <vector>

#define WHINIT_ASSERT(x)                                 \
{                                                        \
    if (!(x))                                            \
    {                                                    \
        Wh_Log(L"Startup assertion fail: %s", L## #x);   \
        return FALSE;                                    \
    }                                                    \
}

#define KAWA_ASSERT(condition) [&]() { if (!(condition)) { Wh_Log(L"Assertion fail: %s", L## #condition); return false; } return true; }()

thread_local bool g_fEnableGetCurrentDirectoryHooks = false;

struct PathRedirection
{
    WCHAR szFrom[MAX_PATH];
    WCHAR szTo[MAX_PATH];
};

bool g_fRedirectTitle = false;

int g_cRedirections = 0;
std::vector<PathRedirection> g_srgRedirections;

/**
 * @return True if replaced, false if unchanged.
 */
bool ApplyPathReplacementToString(LPWSTR szTarget, DWORD nBufferLength)
{
    for (int i = 0; i < g_cRedirections; i++)
    {
        WCHAR szBuffer[MAX_PATH] = { 0 };
        wcscpy_s(szBuffer, szTarget);

        Wh_Log(L"Looking at directories, i = %d", i);
        PathRedirection *pRedir = &g_srgRedirections[i];

        Wh_Log(L"Looking at replacement:");
        Wh_Log(L"  - Pattern: %s", pRedir->szFrom);
        Wh_Log(L"  - Replace: %s", pRedir->szTo);

        std::wstring ssTarget = szBuffer;
        LCMapStringW(
            LOCALE_INVARIANT,
            LCMAP_LOWERCASE,
            ssTarget.c_str(),
            ssTarget.size(),
            szBuffer,
            ARRAYSIZE(szBuffer)
        );
        std::wstring ssTargetLower = szBuffer;

        Wh_Log(L"ssTarget: %s", ssTarget.c_str());
        Wh_Log(L"ssTargetLower: %s", ssTargetLower.c_str());
        
        if (ssTargetLower.starts_with(pRedir->szFrom) ||
            (ssTargetLower + L"\\") == pRedir->szFrom)
        {
            Wh_Log(L"Lowercase target starts with pattern.");
            // This never has a trailing backslash.
            wcscpy(szBuffer, pRedir->szTo);

            // Anything more? We'll count the number of backslashes in the
            // strings to figure this out.
            UINT cBackslashesPattern = 0;
            UINT cBackslashesSrc = 0;
            LPCWSTR szRestOfSrc = ssTarget.c_str();
            for (LPCWSTR sz = pRedir->szFrom; *sz != 0;)
            {
                if (*sz == L'\\')
                    cBackslashesPattern++;
                sz++;
            }
            for (LPCWSTR sz = ssTarget.c_str(); *sz != 0 && cBackslashesSrc < cBackslashesPattern;)
            {
                if (*sz == L'\\')
                {
                    cBackslashesSrc++;
                    szRestOfSrc = sz;
                }
                sz++;
            }
            Wh_Log(L"Count of backslashes in pattern: %d", cBackslashesPattern);
            Wh_Log(L"Count of backslashes in source: %d", cBackslashesSrc);
            Wh_Log(L"Rest of source: %s", szRestOfSrc);

            if (cBackslashesSrc >= cBackslashesPattern)
            {
                // Append the rest of the source, including the leading "\"
                wcscat_s(szBuffer, szRestOfSrc);
            }

            ZeroMemory(szTarget, nBufferLength);
            wcscpy_s(szTarget, nBufferLength, szBuffer);
            return true;
        }
        else if (pRedir->szFrom[0] == L'\0' && pRedir->szTo[0] == L'\0')
        {
            // End of array.
            break;
        }
    }

    return false;
}

DWORD (WINAPI *GetCurrentDirectoryW_orig)(DWORD nBufferLength, LPWSTR lpBuffer);
DWORD WINAPI GetCurrentDirectoryW_hook(DWORD nBufferLength, LPWSTR lpBuffer)
{
    Wh_Log(L"Entered");
    if (g_fEnableGetCurrentDirectoryHooks)
    {
        WCHAR szBuffer[MAX_PATH] = { 0 };
        GetCurrentDirectoryW_orig(ARRAYSIZE(szBuffer), szBuffer);

        ApplyPathReplacementToString(szBuffer, ARRAYSIZE(szBuffer));

        DWORD nResultLength = wcslen(szBuffer);

        if (nResultLength + 1 > nBufferLength)
        {
            return GetCurrentDirectoryW_orig(nBufferLength, lpBuffer);
        }

        wcscpy_s(lpBuffer, nBufferLength, szBuffer);
        return nResultLength;
    }

    return GetCurrentDirectoryW_orig(nBufferLength, lpBuffer);
}

void (WINAPI *PrintPrompt_orig)();
void WINAPI PrintPrompt_hook()
{
    Wh_Log(L"Entered");
    g_fEnableGetCurrentDirectoryHooks = true;
    PrintPrompt_orig();
    g_fEnableGetCurrentDirectoryHooks = false;
}

void (WINAPI *SetConsoleTitleW_orig)(LPCWSTR szTitle);
void WINAPI SetConsoleTitleW_hook(LPCWSTR szTitle)
{
    Wh_Log(L"Entered");

    if (!g_fRedirectTitle)
        return SetConsoleTitleW_orig(szTitle);

    // The maximum width of a title in Windows is 254 characters before Windows 10,
    // 255 characters after Windows 10; 259 characters in UxTheme (MAX_PATH);
    // 255 characters in DWM. We'll go with the largest variant.
    WCHAR szBuffer[MAX_PATH];
    wcscpy_s(szBuffer, szTitle);

    Wh_Log(L"%s", szBuffer);

    ApplyPathReplacementToString(szBuffer, ARRAYSIZE(szBuffer));

    return SetConsoleTitleW_orig(szBuffer);
}

// cmd.exe
const WindhawkUtils::SYMBOL_HOOK g_rghookCmd[] = {
    {
        {
#ifdef _WIN64
            L"void __cdecl PrintPrompt(void)",
#else
            L"void __stdcall PrintPrompt(void)",
#endif
        },
        &PrintPrompt_orig,
        PrintPrompt_hook,
    },
};

void LoadSettings()
{
    constexpr UINT kReallocationThreshold = 16;
    WCHAR szBuffer[MAX_PATH] = { 0 };
    PathRedirection *pRedirectionsTemp = nullptr;

    g_fRedirectTitle = Wh_GetIntSetting(L"replace_in_title") ? true : false;

    for (int i = 0; ; i++)
    {
        g_cRedirections = i;

        if (i % kReallocationThreshold == 0)
        {
            pRedirectionsTemp = pRedirectionsTemp
                ? (PathRedirection *)realloc(pRedirectionsTemp, sizeof(PathRedirection) * (i + kReallocationThreshold))
                : (PathRedirection *)malloc(sizeof(PathRedirection) * (i + kReallocationThreshold));
            ZeroMemory(&pRedirectionsTemp[i], sizeof(PathRedirection) * (kReallocationThreshold));
        }

        LPCWSTR szSearch = Wh_GetStringSetting(L"replacements[%d].search", i);
        LPCWSTR szReplace = Wh_GetStringSetting(L"replacements[%d].replacement", i);

        LPCWSTR szEffectiveReplace = szReplace;

        if ((*szSearch && !*szReplace) || (*szReplace && !*szSearch))
        {
            Wh_Log(L"Skipping illegal setting %d (search pattern: %s, replacement: %s)", i,
                szSearch ? szSearch : L"<null>",
                szReplace ? szReplace : L"<null>"
            );
            Wh_FreeStringSetting(szSearch);
            Wh_FreeStringSetting(szReplace);
            continue;
        }
        else if (!*szReplace && !*szSearch)
        {
            // End of array.
            Wh_FreeStringSetting(szSearch);
            Wh_FreeStringSetting(szReplace);
            break;
        }

        // Remove trailing "\" if the replacement contains it.
        DWORD dwLenReplace = wcslen(szReplace);
        if (dwLenReplace && szReplace[dwLenReplace - 1] == L'\\')
        {
            szEffectiveReplace = szBuffer;
            ZeroMemory(szBuffer, sizeof(szBuffer));
            wcscpy_s(szBuffer, szReplace);
            szBuffer[dwLenReplace - 1] = 0;
        }

        // Emplace:
        wcscpy_s(pRedirectionsTemp[i].szTo, szEffectiveReplace);

        ZeroMemory(szBuffer, sizeof(szBuffer));
        LCMapStringW(
            LOCALE_INVARIANT,
            LCMAP_LOWERCASE,
            szSearch,
            wcslen(szSearch),
            szBuffer,
            ARRAYSIZE(szBuffer)
        );

        // Add trailing "\" if the pattern doesn't contain it.
        DWORD dwLenPattern = wcslen(szBuffer);
        if (dwLenPattern && szBuffer[dwLenPattern - 1] != L'\\')
        {
            wcscat_s(szBuffer, L"\\");
        }
        wcscpy_s(pRedirectionsTemp[i].szFrom, szBuffer);

        Wh_FreeStringSetting(szSearch);
        Wh_FreeStringSetting(szReplace);
    }

    // Since the matching will apply to the rest of the string, sorting by
    // descending length is necessary.
    struct SortMap
    {
        int index;
        int length;
    };
    SortMap *rgMap = new SortMap[g_cRedirections];

    for (int i = 0; i < g_cRedirections; i++)
    {
        rgMap[i].index = i;
        rgMap[i].length = wcslen(pRedirectionsTemp[i].szFrom);
    }

    std::sort(rgMap, &rgMap[g_cRedirections], [](const SortMap &a, const SortMap &b)
    {
        return a.length > b.length;
    });

    for (int i = 0; i < g_cRedirections; i++)
    {
        Wh_Log(L"Sorted %d %d %s", rgMap[i].index, rgMap[i].length, pRedirectionsTemp[rgMap[i].index].szFrom);
        g_srgRedirections.push_back(pRedirectionsTemp[rgMap[i].index]);
    }

    delete[] rgMap;
    free(pRedirectionsTemp);
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hmCmd = GetModuleHandleW(NULL);

    if (!WindhawkUtils::HookSymbols(hmCmd, g_rghookCmd, ARRAYSIZE(g_rghookCmd)))
    {
        Wh_Log(L"Failed to hook symbols");
        return FALSE;
    }

    WHINIT_ASSERT(PrintPrompt_orig != nullptr);

    HMODULE hmKernelBase = GetModuleHandleW(L"kernelbase.dll");

    if (!hmKernelBase)
    {
        Wh_Log(L"No kernel32.");
        return FALSE;
    }

    FARPROC pfnGetCurrentDirectoryW = GetProcAddress(hmKernelBase, "GetCurrentDirectoryW");
    if (!pfnGetCurrentDirectoryW)
    {
        Wh_Log(L"Could not find address of GetCurrentDirectoryW in kernel32.");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        (void *)pfnGetCurrentDirectoryW,
        (void *)GetCurrentDirectoryW_hook,
        (void **)&GetCurrentDirectoryW_orig
    ))
    {
        Wh_Log(L"Failed to hook GetCurrentDirectoryW.");
        return FALSE;
    }

    WHINIT_ASSERT(GetCurrentDirectoryW_orig != nullptr);

    if (g_fRedirectTitle)
    {
        HMODULE hmConsoleApiset = LoadLibraryW(L"api-ms-win-core-console-l2-2-0.dll");
        FARPROC pfnSetConsoleTitleW = GetProcAddress(hmConsoleApiset, "SetConsoleTitleW");

        if (!Wh_SetFunctionHook(
            (void *)pfnSetConsoleTitleW,
            (void *)SetConsoleTitleW_hook,
            (void **)&SetConsoleTitleW_orig
        ))
        {
            Wh_Log(L"Failed to hook SetConsoleTitleW.");
        }

        WHINIT_ASSERT(SetConsoleTitleW_orig != nullptr);
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    if (g_fRedirectTitle)
    {
        // Change the initial title of the console.
        WCHAR szOriginalTitle[MAX_PATH] = { 0 };

        // GetConsoleOriginalTitleW may return a path with environment variables
        // unaccounted for. For example, "%SystemRoot%\System32\cmd.exe" instead
        // of "C:\Windows\System32\cmd.exe"
        GetConsoleTitleW(szOriginalTitle, ARRAYSIZE(szOriginalTitle));

        // Caught by the SetConsoleTitleW hook.
        SetConsoleTitleW(szOriginalTitle);
    }
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged()
{
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
