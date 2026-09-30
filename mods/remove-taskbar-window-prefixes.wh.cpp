// ==WindhawkMod==
// @id              remove-taskbar-window-prefixes
// @name            Remove Taskbar Window Prefixes
// @description     Remove prefixes such as "ChatGPT - " from taskbar window titles, with built-in and custom per-app rules
// @version         0.1.5
// @author          MrSpice
// @github          https://github.com/ryanspice
// @homepage        https://ryanspice.com/
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Taskbar Window Prefixes



Windows taskbar buttons can waste useful label space when apps put the app name
first, for example:

- `ChatGPT - Android S...`
- `ChatGPT - Needle - ...`
- `App Name - Useful Document Title`

This mod changes only the taskbar label that Explorer displays. It does **not**
change the real window title.

This is a prefix-oriented companion to m417z's suffix-removal mod idea.
It reuses the same practical Windhawk taskbar-label pattern: hook Explorer's
taskbar title read path, then rewrite only the label Explorer displays. It is
intended for Progressive Web Apps and programs that put a repeated app name at
the beginning of every title.

## Prefix Removal Modes

- **ChatGPT only** (default): Removes a leading `ChatGPT - ` prefix from taskbar labels.
- **Universal**: Removes the first title part before an exact spaced hyphen separator, ` - `, from every matching taskbar title.
- **Off**: Disables built-in prefix removal. Custom rules can still run.

## Simple Prefix Rules

Use simple prefix rules when you just want to remove a literal app prefix.

Each rule has:

- **Process**: Optional. Use a process name such as `msedge_proxy.exe`,
  `chrome_proxy.exe`, `msedge.exe`, a full path, or an App ID. Leave empty to
  match all programs.
- **Prefix text**: Literal prefix to remove before a separator.

Examples:

**Remove ChatGPT from all matching taskbar titles:**

- Process: empty
- Prefix text: `ChatGPT`

**Remove ChatGPT only from Edge PWA windows:**

- Process: `msedge_proxy.exe`
- Prefix text: `ChatGPT`

## Advanced Regex Rules

Use regex rules when you need more control. Rules are applied after the built-in
mode and simple prefix rules.

Example:

- Process: empty
- Search: `^ChatGPT\\\\s+-\\\\s+(.+)$`
- Replace: `$1`

## Notes

- Changes only affect how titles appear on the taskbar.
- Rules can match by process name or full path. App ID matching is available only when enabled in settings.
- Empty process means all processes.
- Process matching is case-insensitive.
- Invalid regex patterns are logged but won't prevent other rules from working.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- prefixRemovalMode: chatgptOnly
  $name: Prefix removal mode
  $options:
  - off: Off
  - chatgptOnly: ChatGPT only
  - universal: Universal
  $description: >-
    Controls built-in prefix removal. "ChatGPT only" removes leading ChatGPT
    prefixes. "Universal" removes the first title part before an exact ` - ` separator.
    "Off" disables built-in removal while keeping custom rules available.
- enableAppIdMatching: false
  $name: Enable App ID matching
  $description: >-
    Allows process rules to match by Windows App ID in addition to process name
    and full path. Disabled by default to avoid COM/AppResolver work in
    Explorer's taskbar title path.
- simplePrefixRules:
  - - processIdentifier: ""
      $name: Process (name, path, or App ID)
      $description: >-
        Can be a process name (msedge_proxy.exe), full path (C:\\Program Files\\...\\msedge_proxy.exe), or App ID when App ID matching is enabled. Leave empty to match all processes.
    - prefix: ""
      $name: Prefix text to remove
      $description: >-
        Literal prefix text to remove when followed by an exact ` - ` separator. Example:
        ChatGPT removes "ChatGPT - " from taskbar labels.
  $name: Simple prefix removal rules
  $description: >-
    Define literal prefix-removal rules. Multiple matching rules are applied in
    order. Leave Process empty to apply a prefix rule to all programs.
- regexRules:
  - - processIdentifier: ""
      $name: Process (name, path, or App ID)
      $description: >-
        Can be a process name (msedge_proxy.exe), full path, or App ID when App ID matching is enabled. Leave
        empty to match all processes.
    - search: ""
      $name: Search pattern (regex)
      $description: >-
        Regular expression pattern to search for in taskbar titles. Example:
        ^ChatGPT\\\\s+-\\\\s+(.+)$
    - replace: ""
      $name: Replace with
      $description: >-
        Replacement text. Can use regex capture groups ($1, $2, etc.).
  $name: Advanced regex title modification rules
  $description: >-
    Define regex-based search and replace rules for taskbar titles. Rules are
    applied after the built-in mode and simple prefix rules.
*/
// ==/WindhawkModSettings==

#include <psapi.h>

#include <cwctype>
#include <regex>
#include <string>
#include <vector>

#include <winrt/base.h>

enum class PrefixRemovalMode {
    Off,
    ChatGptOnly,
    Universal,
};

struct SimplePrefixRule {
    std::wstring processIdentifier;  // Stored in uppercase, empty = match all
    std::wstring prefix;
};

struct RegexRule {
    std::wstring processIdentifier;  // Stored in uppercase, empty = match all
    std::wregex search;
    std::wstring replace;
};

struct {
    PrefixRemovalMode prefixRemovalMode;
    bool enableAppIdMatching;    std::vector<SimplePrefixRule> simplePrefixRules;
    std::vector<RegexRule> regexRules;
} g_settings;
volatile LONG g_refreshWorkerQueued = 0;
volatile LONG g_unloading = 0;
HANDLE g_refreshThread = nullptr;
HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

HWND GetTaskBandWnd() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        return (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    }

    return nullptr;
}

std::wstring UppercaseCopy(std::wstring value) {
    if (!value.empty()) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, value.data(),
                      static_cast<int>(value.length()), value.data(),
                      static_cast<int>(value.length()), nullptr, nullptr, 0);
    }

    return value;
}

// https://gist.github.com/m417z/451dfc2dad88d7ba88ed1814779a26b4
std::wstring GetWindowAppId(HWND hWnd) {
    // {c8900b66-a973-584b-8cae-355b7f55341b}
    constexpr winrt::guid CLSID_StartMenuCacheAndAppResolver{
        0x660b90c8,
        0x73a9,
        0x4b58,
        {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};

    // {de25675a-72de-44b4-9373-05170450c140}
    constexpr winrt::guid IID_IAppResolver_8{
        0xde25675a,
        0x72de,
        0x44b4,
        {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

    struct IAppResolver_8 : public IUnknown {
       public:
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut() = 0;
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject() = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForWindow(HWND hWnd,
                          WCHAR** pszAppId,
                          void* pUnknown1,
                          void* pUnknown2,
                          void* pUnknown3) = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForProcess(DWORD dwProcessId,
                           WCHAR** pszAppId,
                           void* pUnknown1,
                           void* pUnknown2,
                           void* pUnknown3) = 0;
    };

    HRESULT hr;
    std::wstring result;

    winrt::com_ptr<IAppResolver_8> appResolver;
    hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                          CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                          IID_IAppResolver_8, appResolver.put_void());
    if (SUCCEEDED(hr)) {
        WCHAR* pszAppId;
        hr = appResolver->GetAppIDForWindow(hWnd, &pszAppId, nullptr, nullptr,
                                            nullptr);
        if (SUCCEEDED(hr)) {
            result = pszAppId;
            CoTaskMemFree(pszAppId);
        }
    }

    return result;
}

struct WindowProcessIdentity {
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH] = {};
    PCWSTR programFileNameUpper = nullptr;
    std::wstring appIdUpper;
    bool appIdFetched = false;
};

WindowProcessIdentity GetWindowProcessIdentity(HWND hWnd) {
    WindowProcessIdentity identity;

    WCHAR resolvedWindowProcessPath[MAX_PATH];
    DWORD resolvedWindowProcessPathLen = 0;

    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        HANDLE hProcess =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
        if (hProcess) {
            DWORD dwSize = ARRAYSIZE(resolvedWindowProcessPath);
            if (QueryFullProcessImageName(hProcess, 0,
                                          resolvedWindowProcessPath, &dwSize)) {
                resolvedWindowProcessPathLen = dwSize;
            }
            CloseHandle(hProcess);
        }
    }

    if (resolvedWindowProcessPathLen > 0) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      resolvedWindowProcessPath,
                      resolvedWindowProcessPathLen + 1,
                      identity.resolvedWindowProcessPathUpper,
                      resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);
    } else {
        identity.resolvedWindowProcessPathUpper[0] = L'\0';
    }

    identity.programFileNameUpper =
        wcsrchr(identity.resolvedWindowProcessPathUpper, L'\\');
    if (identity.programFileNameUpper) {
        identity.programFileNameUpper++;
    }

    return identity;
}

bool ProcessIdentifierMatches(HWND hWnd,
                              WindowProcessIdentity& identity,
                              const std::wstring& processIdentifier) {
    if (processIdentifier.empty()) {
        return true;
    }

    if (wcscmp(identity.resolvedWindowProcessPathUpper,
               processIdentifier.c_str()) == 0) {
        return true;
    }

    if (identity.programFileNameUpper && *identity.programFileNameUpper &&
        wcscmp(identity.programFileNameUpper, processIdentifier.c_str()) == 0) {
        return true;
    }

    // App ID matching can require COM/AppResolver work inside Explorer. Keep it
    // opt-in for stability, and only attempt it for App-ID-shaped identifiers.
    if (!g_settings.enableAppIdMatching ||
        processIdentifier.find(L'!') == std::wstring::npos) {
        return false;
    }

    if (!identity.appIdFetched) {
        identity.appIdUpper = UppercaseCopy(GetWindowAppId(hWnd));
        identity.appIdFetched = true;
    }

    return !identity.appIdUpper.empty() &&
           wcscmp(identity.appIdUpper.c_str(), processIdentifier.c_str()) == 0;
}
bool StartsWithInsensitive(const std::wstring& text,
                           const std::wstring& prefix) {
    if (prefix.empty() || text.length() <= prefix.length()) {
        return false;
    }

    return _wcsnicmp(text.c_str(), prefix.c_str(), prefix.length()) == 0;
}

bool IsExactPrefixSeparatorAt(const std::wstring& text, size_t pos) {
    return pos + 3 <= text.length() && text[pos] == L' ' &&
           text[pos + 1] == L'-' && text[pos + 2] == L' ';
}

bool PrefixCandidateLooksSafe(const std::wstring& prefixCandidate) {
    if (prefixCandidate.empty() || prefixCandidate.length() > 64) {
        return false;
    }

    // Avoid treating unsaved document titles or file paths as app prefixes.
    if (prefixCandidate[0] == L'*' ||
        prefixCandidate.find_first_of(L"\\/:") != std::wstring::npos) {
        return false;
    }

    return true;
}

bool RemoveLiteralPrefix(std::wstring& text, const std::wstring& prefix) {
    if (!StartsWithInsensitive(text, prefix)) {
        return false;
    }

    size_t separatorPos = prefix.length();
    if (!IsExactPrefixSeparatorAt(text, separatorPos)) {
        return false;
    }

    size_t titleStart = separatorPos + 3;
    if (titleStart >= text.length()) {
        return false;
    }

    text = text.substr(titleStart);
    return true;
}

bool FindFirstSeparator(const std::wstring& text,
                        size_t* separatorPos,
                        size_t* separatorLen) {
    size_t pos = text.find(L" - ");
    if (pos == std::wstring::npos) {
        return false;
    }

    std::wstring prefixCandidate = text.substr(0, pos);
    if (!PrefixCandidateLooksSafe(prefixCandidate)) {
        return false;
    }

    *separatorPos = pos;
    *separatorLen = 3;
    return true;
}
bool RemoveUniversalPrefix(std::wstring& text) {
    size_t separatorPos = std::wstring::npos;
    size_t separatorLen = 0;

    if (!FindFirstSeparator(text, &separatorPos, &separatorLen)) {
        return false;
    }

    size_t titleStart = separatorPos + separatorLen;
    while (titleStart < text.length() && iswspace(text[titleStart])) {
        titleStart++;
    }

    if (titleStart >= text.length()) {
        return false;
    }

    text = text.substr(titleStart);
    return true;
}

using InternalGetWindowText_t = int(WINAPI*)(HWND hWnd,
                                             LPWSTR pString,
                                             int cchMaxCount);
InternalGetWindowText_t InternalGetWindowText_Original;
int WINAPI InternalGetWindowText_Hook(HWND hWnd,
                                      LPWSTR pString,
                                      int cchMaxCount) {
    int result = InternalGetWindowText_Original(hWnd, pString, cchMaxCount);
    if (result <= 0 || !pString || cchMaxCount <= 0 ||
        result >= cchMaxCount) {
        return result;
    }

    try {
        if (g_settings.prefixRemovalMode == PrefixRemovalMode::Off &&
            g_settings.simplePrefixRules.empty() &&
            g_settings.regexRules.empty()) {
            return result;
        }

        void* retAddress = __builtin_return_address(0);

        HMODULE taskbarModule = GetModuleHandle(L"taskbar.dll");
        if (!taskbarModule) {
            return result;
        }

        HMODULE module;
        if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               (PCWSTR)retAddress, &module) ||
            module != taskbarModule) {
            return result;
        }

        std::wstring text(pString, static_cast<size_t>(result));
        bool modified = false;

        if (g_settings.prefixRemovalMode == PrefixRemovalMode::ChatGptOnly) {
            if (RemoveLiteralPrefix(text, L"ChatGPT")) {
                modified = true;
            }
        } else if (g_settings.prefixRemovalMode == PrefixRemovalMode::Universal) {
            if (RemoveUniversalPrefix(text)) {
                modified = true;
            }
        }

        WindowProcessIdentity identity;
        bool identityLoaded = false;

        for (const auto& rule : g_settings.simplePrefixRules) {
            std::wstring candidateText = text;
            if (!RemoveLiteralPrefix(candidateText, rule.prefix)) {
                continue;
            }

            if (!identityLoaded) {
                identity = GetWindowProcessIdentity(hWnd);
                identityLoaded = true;
            }

            if (ProcessIdentifierMatches(hWnd, identity,
                                         rule.processIdentifier)) {
                text = candidateText;
                modified = true;
            }
        }

        for (const auto& rule : g_settings.regexRules) {
            try {
                std::wstring newText =
                    std::regex_replace(text, rule.search, rule.replace);
                if (newText == text) {
                    continue;
                }

                if (!identityLoaded) {
                    identity = GetWindowProcessIdentity(hWnd);
                    identityLoaded = true;
                }

                if (!ProcessIdentifierMatches(hWnd, identity,
                                             rule.processIdentifier)) {
                    continue;
                }

                text = newText;
                modified = true;
            } catch (const std::regex_error& ex) {
                Wh_Log(L"Regex replace error %08X: %S",
                       static_cast<DWORD>(ex.code()), ex.what());
            }
        }

        if (modified) {
            if (text.length() < static_cast<size_t>(cchMaxCount)) {
                wcscpy_s(pString, cchMaxCount, text.c_str());
                result = static_cast<int>(text.length());
            } else {
                Wh_Log(L"Result too long (%zu chars), keeping original",
                       text.length());
            }
        }
    } catch (...) {
        Wh_Log(L"Unexpected error while rewriting taskbar title");
    }

    return result;
}
bool ApplySettings() {
    HWND hTaskBandWnd = GetTaskBandWnd();
    if (!hTaskBandWnd) {
        return false;
    }

    static const UINT WM_SHELLHOOK = RegisterWindowMessage(L"SHELLHOOK");

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            if (IsWindowVisible(hWnd)) {
                PostMessage(reinterpret_cast<HWND>(lParam), WM_SHELLHOOK,
                            HSHELL_REDRAW, reinterpret_cast<LPARAM>(hWnd));
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(hTaskBandWnd));

    return true;
}

DWORD WINAPI RefreshTaskbarThreadProc(LPVOID) {
    for (int i = 0; i < 12; i++) {
        if (InterlockedCompareExchange(&g_unloading, 0, 0) != 0) {
            break;
        }

        if (ApplySettings()) {
            break;
        }

        Sleep(250);
    }

    InterlockedExchange(&g_refreshWorkerQueued, 0);
    return 0;
}

void QueueApplySettings() {
    if (InterlockedCompareExchange(&g_unloading, 0, 0) != 0) {
        return;
    }

    if (InterlockedCompareExchange(&g_refreshWorkerQueued, 1, 0) != 0) {
        return;
    }

    HANDLE hThread =
        CreateThread(nullptr, 0, RefreshTaskbarThreadProc, nullptr, 0, nullptr);
    if (hThread) {
        if (g_refreshThread) {
            CloseHandle(g_refreshThread);
        }

        g_refreshThread = hThread;
    } else {
        InterlockedExchange(&g_refreshWorkerQueued, 0);
        ApplySettings();
    }
}
void LoadSettings() {
    Wh_Log(L"LoadSettings");

    PCWSTR mode = Wh_GetStringSetting(L"prefixRemovalMode");
    g_settings.prefixRemovalMode = PrefixRemovalMode::ChatGptOnly;
    if (wcscmp(mode, L"off") == 0) {
        g_settings.prefixRemovalMode = PrefixRemovalMode::Off;
    } else if (wcscmp(mode, L"universal") == 0) {
        g_settings.prefixRemovalMode = PrefixRemovalMode::Universal;
    }
    Wh_FreeStringSetting(mode);

    g_settings.enableAppIdMatching =
        Wh_GetIntSetting(L"enableAppIdMatching") != 0;

    g_settings.simplePrefixRules.clear();

    for (int i = 0;; i++) {
        PCWSTR processId =
            Wh_GetStringSetting(L"simplePrefixRules[%d].processIdentifier", i);
        PCWSTR prefix = Wh_GetStringSetting(L"simplePrefixRules[%d].prefix", i);

        bool hasRule = *prefix;

        if (!hasRule) {
            Wh_FreeStringSetting(processId);
            Wh_FreeStringSetting(prefix);
            break;
        }

        SimplePrefixRule rule;
        rule.processIdentifier = UppercaseCopy(processId);
        rule.prefix = prefix;

        Wh_Log(L"Loaded simple prefix rule for '%s': '%s'",
               rule.processIdentifier.empty() ? L"<all processes>"
                                              : rule.processIdentifier.c_str(),
               rule.prefix.c_str());

        g_settings.simplePrefixRules.push_back(std::move(rule));

        Wh_FreeStringSetting(processId);
        Wh_FreeStringSetting(prefix);
    }

    g_settings.regexRules.clear();

    for (int i = 0;; i++) {
        PCWSTR processId =
            Wh_GetStringSetting(L"regexRules[%d].processIdentifier", i);
        PCWSTR search = Wh_GetStringSetting(L"regexRules[%d].search", i);
        PCWSTR replace = Wh_GetStringSetting(L"regexRules[%d].replace", i);

        bool hasRule = *search;

        if (!hasRule) {
            Wh_FreeStringSetting(processId);
            Wh_FreeStringSetting(search);
            Wh_FreeStringSetting(replace);
            break;
        }

        try {
            RegexRule rule;
            rule.processIdentifier = UppercaseCopy(processId);
            rule.search = std::wregex(search);
            rule.replace = replace;

            Wh_Log(L"Loaded regex rule for '%s': '%s' -> '%s'",
                   rule.processIdentifier.empty() ? L"<all processes>"
                                                  : rule.processIdentifier.c_str(),
                   search, replace);

            g_settings.regexRules.push_back(std::move(rule));
        } catch (const std::regex_error& ex) {
            Wh_Log(L"Invalid regex pattern '%s': %S (code %08X)", search,
                   ex.what(), static_cast<DWORD>(ex.code()));
        }

        Wh_FreeStringSetting(processId);
        Wh_FreeStringSetting(search);
        Wh_FreeStringSetting(replace);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    InterlockedExchange(&g_unloading, 0);

    LoadSettings();

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    if (user32Module) {
        void* pInternalGetWindowText =
            (void*)GetProcAddress(user32Module, "InternalGetWindowText");
        if (pInternalGetWindowText) {
            Wh_SetFunctionHook(pInternalGetWindowText,
                               (void*)InternalGetWindowText_Hook,
                               (void**)&InternalGetWindowText_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    QueueApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    InterlockedExchange(&g_unloading, 1);

    if (g_refreshThread) {
        WaitForSingleObject(g_refreshThread, 3000);
        CloseHandle(g_refreshThread);
        g_refreshThread = nullptr;
    }

    ApplySettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    QueueApplySettings();
}





