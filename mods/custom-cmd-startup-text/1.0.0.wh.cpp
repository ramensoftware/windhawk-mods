// ==WindhawkMod==
// @id              custom-cmd-startup-text
// @name            Custom Command Prompt Startup Text
// @description     Replace the version and copyright text that shows 
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         cmd.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Command Prompt Startup Text
This mod allows you to change the version and copyright text displayed at the startup of
Command Prompt without messing up scripts that use the `ver` command to read OS version,
and without modifying the `cmd.exe` binary on disk.

## Supported escape sequences
- `\n` - new line
- `\\` - backslash
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- startup_text: "Microsoft Windows [Version 5.2.3790]\\n(c) Microsoft Corporation. All rights reserved.\\n\\n"
  $name: Startup text
  $description: Text to use at startup. For a list of supported escape sequences, check the README.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#ifdef _WIN64
#   define  STDCALL_STR L"__cdecl"
#else
#   define  STDCALL_STR L"__stdcall"
#endif

/* Toss out the copyright string. */
LPWSTR (__fastcall *CmdGetBrandingString_orig)(void);
LPWSTR __fastcall CmdGetBrandingString_hook(void)
{
    return nullptr;
}

bool g_fInInit = false;

/* Avoid printing extraneous new lines */
int (__fastcall *CmdPutChars_orig)(LPCWSTR String, int Length);
int __fastcall CmdPutChars_hook(LPCWSTR String, int Length)
{
    if (g_fInInit && Length == 2 && String[0] == L'\r' && String[1] == L'\n')
        return 0;
    return CmdPutChars_orig(String, Length);
}

/* Avoid printing out the new line before the prompt the first time around.
   This is to make it possible to replicate Windows Me's MS-DOS prompt, which
   had no startup text or new line before the first prompt. */
void (__fastcall *PrintPrompt_orig)(void);
void __fastcall PrintPrompt_hook(void)
{
    static bool fDoneFirstTime = false;

    if (!fDoneFirstTime)
    {
        g_fInInit = true;
    }
    PrintPrompt_orig();
    if (!fDoneFirstTime)
    {
        g_fInInit = false;
        fDoneFirstTime = true;
    }
}

void PrintModError(LPCWSTR pszFormat, ...)
{
    LPCWSTR c_szErrorHeader = L"Error in Windhawk mod Custom Command Prompt Startup Text:\r\n";
    CmdPutChars_orig(c_szErrorHeader, wcslen(c_szErrorHeader));

    va_list args;
    va_start(args, pszFormat);

    WCHAR szMessage[512];
    vswprintf_s(szMessage, pszFormat, args);

    va_end(args);

    CmdPutChars_orig(szMessage, wcslen(szMessage));
    CmdPutChars_orig(L"\r\n\r\n", 4);
}

typedef int CRTHANDLE;

int (__fastcall *PutMsg_orig)(unsigned int, CRTHANDLE, unsigned int, va_list *);
int __fastcall PutMsg_hook(
    unsigned int  MsgNum,
    CRTHANDLE     Handle,
    unsigned int  NumOfArgs,
    va_list      *arglist
)
{
    if (g_fInInit)
    {
        // Print modified version string
        if (MsgNum == 9040)
        {
            LPWSTR pszText = (LPWSTR)Wh_GetStringSetting(L"startup_text");
            if (!pszText[0])
            {
                Wh_FreeStringSetting(pszText);
                return 0;
            }

            int cchText = wcslen(pszText);
            for (int i = 0; i < cchText; i++)
            {
                if (pszText[i] == L'\\')
                {
                    if (i == cchText - 1)
                    {
                        PrintModError(L"Unterminated escape sequence at end of startup text");
                        return 0;
                    }

                    switch (pszText[i + 1])
                    {
                        case L'n':
                            pszText[i]     = L'\r';
                            pszText[i + 1] = L'\n';
                            break;
                        case L'\\':
                            wcscpy(&pszText[i + 1], &pszText[i + 2]);
                            break;
                        default:
                            PrintModError(L"Unrecognized escape sequence '\\%c' in startup text", pszText[i + 1]);
                            return 0;
                    }
                }
            }

            CmdPutChars_orig(pszText, wcslen(pszText));

            Wh_FreeStringSetting(pszText);
            return 0;
        }
        // Do not print the out of memory error for the now missing copyright text.
        else if (MsgNum == 8)
        {
            return 0;
        }
    }
    return PutMsg_orig(MsgNum, Handle, NumOfArgs, arglist);
}

int (__fastcall *Init_orig)(LPWSTR *, int);
int __fastcall Init_hook(LPWSTR *ppszArgs, int nNumArgs)
{
    g_fInInit = true;
    int nRet = Init_orig(ppszArgs, nNumArgs);
    g_fInInit = false;
    return nRet;
}

const WindhawkUtils::SYMBOL_HOOK cmdExeHooks[] = {
    {
        {
            L"unsigned short * " STDCALL_STR L" CmdGetBrandingString(unsigned short const *,int)"
        },
        &CmdGetBrandingString_orig,
        CmdGetBrandingString_hook,
        false
    },
    {
        {
            L"int " STDCALL_STR L" CmdPutChars(unsigned short const *,int)"
        },
        &CmdPutChars_orig,
        CmdPutChars_hook,
        false
    },
    {
        {
            L"void " STDCALL_STR L" PrintPrompt(void)"
        },
        &PrintPrompt_orig,
        PrintPrompt_hook,
        false
    },
    {
        {
            L"int " STDCALL_STR L" PutMsg(unsigned int,int,unsigned int,char * *)"
        },
        &PutMsg_orig,
        PutMsg_hook,
        false
    },
    {
        {
            L"int " STDCALL_STR L" Init(unsigned short * *,int)"
        },
        &Init_orig,
        Init_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        cmdExeHooks,
        ARRAYSIZE(cmdExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions");
        return FALSE;
    }

    return TRUE;
}