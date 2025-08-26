// ==WindhawkMod==
// @id              per-app-ui-language
// @name            Per-Application UI Language
// @description     Set the Windows UI language per application.
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Per-Application UI Language
This mod allows you to set the list of preferred UI languages for each application.
This is mainly useful if you wish to use or test another language for an application
that does not have a built-in language option (such as applications that use the built-in
MUI system).

**Example**: Japanese [OpenWithEx](https://github.com/aubymori/OpenWithEx) on an English system

![Example](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/per-app-ui-language-example.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- langs:
  - - path: notepad.exe
      $name: Application name or path
    - lang: ja-JP,en-US
      $name: Languages
      $description: List of language identifiers to use. Separate with commas and no spaces.
  $name: Definitions
*/
// ==/WindhawkModSettings==

#include <stdio.h>

bool g_fLangSet = false;

void UpdateUILanguage(void)
{
    LPWSTR pszLanguages = nullptr;

    WCHAR szAppPath[MAX_PATH];
    GetModuleFileNameW(GetModuleHandleW(NULL), szAppPath, ARRAYSIZE(szAppPath));

    for (int i = 0;; i++)
    {
        LPCWSTR pszPath = Wh_GetStringSetting(L"langs[%d].path", i);
        if (!*pszPath)
        {
            Wh_FreeStringSetting(pszPath);
            break;
        }

        WCHAR szExpandedPath[MAX_PATH];
        ExpandEnvironmentStringsW(pszPath, szExpandedPath, ARRAYSIZE(szExpandedPath));
        Wh_FreeStringSetting(pszPath);

        WCHAR *pchBackslash = wcsrchr(szAppPath, L'\\');
        if (!wcsicmp(szAppPath, szExpandedPath)
        || (pchBackslash && !wcsicmp(pchBackslash + 1, szExpandedPath)))
        {
            LPCWSTR pszLang = Wh_GetStringSetting(L"langs[%d].lang", i);
            if (*pszLang)
            {
                Wh_Log(L"Application \"%s\" using languages %s", szAppPath, pszLang);

                size_t cchLang = wcslen(pszLang) + 1;
                size_t cchLanguages = cchLang + 1; // add one for double null terminator

                pszLanguages = (LPWSTR)LocalAlloc(LPTR, cchLanguages * sizeof(WCHAR));
                if (!pszLanguages)
                {
                    Wh_Log(L"Failed to allocate sufficient buffer, skipping");
                    Wh_FreeStringSetting(pszLang);
                    break;
                }

                // Copy langs over
                for (size_t i = 0; i < cchLang; i++)
                {
                    // LPTR in LocalAlloc contains the flag LMEM_ZEROINIT,
                    // so we don't need to copy over any null characters
                    if (pszLang[i] != L',')
                        pszLanguages[i] = pszLang[i];
                }
                
                Wh_FreeStringSetting(pszLang);
                break;
            }

            Wh_FreeStringSetting(pszLang);
        }
    }

    if ((pszLanguages || g_fLangSet) && !SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, pszLanguages, nullptr))
    {
        DWORD dwErr = GetLastError();
        if (dwErr != ERROR_CANNOT_IMPERSONATE)
        {
            WCHAR szError[256];
            FormatMessageW(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
                nullptr,
                dwErr,
                0,
                szError,
                ARRAYSIZE(szError),
                nullptr
            );

            WCHAR szMessage[256];
            swprintf_s(
                szMessage, L"Failed to set the UI language.\n\nError: \"%s\" (%d)",
                szError,
                dwErr
            );

            MessageBoxW(
                NULL,
                szMessage,
                L"Windhawk: Per-Application UI Language",
                MB_ICONERROR
            );
        }
    }

    g_fLangSet = (pszLanguages != nullptr);

    if (pszLanguages)
        LocalFree(pszLanguages);
}

void Wh_ModSettingsChanged(void)
{
    UpdateUILanguage();
}

BOOL Wh_ModInit(void)
{
    UpdateUILanguage();
    return TRUE;
}

void Wh_ModUninit(void)
{
    if (g_fLangSet)
        SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, nullptr, nullptr);
}