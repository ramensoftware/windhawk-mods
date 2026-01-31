// ==WindhawkMod==
// @id              file-explorer-remove-suffixes
// @name            Remove Taskbar Window Suffixes
// @description     Remove suffixes from taskbar window titles for File Explorer and other programs, or configure custom text replacement rules
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Taskbar Window Suffixes

Windows appends redundant suffixes like " - File Explorer" or " - Notepad" to
window titles on the taskbar. This mod removes these suffixes to keep your
taskbar clean and easy to read.

## Suffix Removal Modes

Choose how suffixes are removed:

- **File Explorer only** (default): Removes the " - File Explorer" suffix from
  File Explorer windows.
- **Universal**: Automatically removes the last part after " - ", " — " (em dash
  with spaces), or "—" (em dash alone) from any window title. Examples:
    - "Document - Notepad" becomes "Document"
    - "Downloads - File Explorer" becomes "Downloads"
    - "Windhawk — Firefox" becomes "Windhawk"
- **Off**: Disables automatic suffix removal.

## Before and After

![Before](https://i.imgur.com/ErUN0YU.png) \
_Before: Each folder shows " - File Explorer" suffix_

![After](https://i.imgur.com/tblTr3Q.png) \
_After: Clean folder names without redundant suffixes_

## Custom Regex Rules

Additionally, you can define custom regex-based search and replace rules for
specific programs or all programs. Each rule specifies:

- **Process identifier**: Match by process name (e.g., `notepad.exe`), full path
  (e.g., `C:\Windows\notepad.exe`), or App ID for UWP apps (e.g.,
  `Microsoft.WindowsCalculator_8wekyb3d8bbwe!App`). Leave empty to match all
  processes.
- **Search pattern**: Regular expression to find in window titles. Supports
  standard regex syntax including anchors, groups, and quantifiers.
- **Replace with**: Replacement text. Can use regex capture groups ($1, $2,
  etc.) to reference matched parts. Leave empty to remove the matched text.

Multiple rules can match the same window, and all matching patterns are applied
sequentially in the order they're defined.

### Example Rules

**Remove " - Notepad" suffix from Notepad:**
- Process: `notepad.exe`
- Search: ` - Notepad$`
- Replace: (empty)

**Reorder title parts in Chrome:**
- Process: `chrome.exe`
- Search: `^(.*) - Google Chrome$`
- Replace: `Chrome: $1`

**Remove build configuration from any window:**
- Process: (empty, matches all)
- Search: ` \(Debug|Release\)$`
- Replace: (empty)

## Notes

- Changes only affect how titles appear on the taskbar, not the actual window
  titles.
- Universal mode processes titles before custom regex rules, allowing you to
  apply additional modifications after suffix removal.
- Case-insensitive process matching for better compatibility.
- Invalid regex patterns are logged but won't prevent other rules from working.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- suffixRemovalMode: fileExplorerOnly
  $name: Suffix removal mode
  $options:
  - off: Off
  - fileExplorerOnly: File Explorer only
  - universal: Universal
  $description: >-
    Controls how suffixes are removed from taskbar titles. "File Explorer only"
    only removes File Explorer suffixes. "Universal" removes the suffix from any
    title (e.g., "Document - Editor" becomes "Document").
- suffixRules:
  - - processIdentifier: ""
      $name: Process (name, path, or App ID)
      $description: >-
        Can be a process name (explorer.exe), full path
        (C:\Windows\explorer.exe), or App ID
        (Microsoft.WindowsCalculator_8wekyb3d8bbwe!App). Leave empty to match
        all processes.
    - search: ""
      $name: Search pattern (regex)
      $description: >-
        Regular expression pattern to search for in window titles. Example: " -
        Notepad$" to match " - Notepad" at the end of the title.
    - replace: ""
      $name: Replace with
      $description: >-
        Replacement text. Can use regex capture groups ($1, $2, etc.). Leave
        empty to remove the matched text.
  $name: Custom title modification rules
  $description: >-
    Define regex-based search and replace rules for window titles. Each rule
    specifies a process and a regex pattern. Multiple rules can match the same
    window, and all matching patterns are applied in order.
*/
// ==/WindhawkModSettings==

#include <psapi.h>

#include <regex>
#include <string>
#include <vector>

#include <winrt/base.h>

enum class SuffixRemovalMode {
    Off,
    FileExplorerOnly,
    Universal,
};

struct SuffixRule {
    std::wstring processIdentifier;  // Stored in uppercase, empty = match all
    std::wregex search;
    std::wstring replace;
};

struct {
    SuffixRemovalMode suffixRemovalMode;
    std::vector<SuffixRule> suffixRules;
} g_settings;

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

std::vector<const SuffixRule*> GetRulesForWindow(HWND hWnd) {
    std::vector<const SuffixRule*> matchedRules;

    if (g_settings.suffixRules.empty()) {
        return matchedRules;
    }

    // Get process path and convert to uppercase
    WCHAR resolvedWindowProcessPath[MAX_PATH];
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH];
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
                      resolvedWindowProcessPathUpper,
                      resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);
    } else {
        resolvedWindowProcessPathUpper[0] = L'\0';
    }

    // Extract process name from path
    PCWSTR programFileNameUpper =
        wcsrchr(resolvedWindowProcessPathUpper, L'\\');
    if (programFileNameUpper) {
        programFileNameUpper++;
    }

    // Get App ID once (expensive operation)
    std::wstring appId;
    bool appIdFetched = false;

    // Check each rule and collect all matches
    for (const auto& rule : g_settings.suffixRules) {
        bool matches = false;

        // Empty process identifier matches all processes
        if (rule.processIdentifier.empty()) {
            matches = true;
        }
        // Check full path match
        else if (wcscmp(resolvedWindowProcessPathUpper,
                        rule.processIdentifier.c_str()) == 0) {
            matches = true;
        }
        // Check process name match
        else if (programFileNameUpper && *programFileNameUpper &&
                 wcscmp(programFileNameUpper, rule.processIdentifier.c_str()) ==
                     0) {
            matches = true;
        }
        // Check App ID match
        else {
            if (!appIdFetched) {
                appId = GetWindowAppId(hWnd);
                if (!appId.empty()) {
                    LCMapStringEx(
                        LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, appId.data(),
                        static_cast<int>(appId.length()), appId.data(),
                        static_cast<int>(appId.length()), nullptr, nullptr, 0);
                }
                appIdFetched = true;
            }
            if (!appId.empty() &&
                wcscmp(appId.c_str(), rule.processIdentifier.c_str()) == 0) {
                matches = true;
            }
        }

        if (matches) {
            matchedRules.push_back(&rule);
        }
    }

    return matchedRules;
}

using FindResourceExW_t = decltype(&FindResourceExW);
FindResourceExW_t FindResourceExW_Original;
HRSRC WINAPI FindResourceExW_Hook(HMODULE hModule,
                                  LPCWSTR lpType,
                                  LPCWSTR lpName,
                                  WORD wLanguage) {
    if (g_settings.suffixRemovalMode != SuffixRemovalMode::Off && hModule &&
        lpType == RT_STRING && lpName == MAKEINTRESOURCE(2195) &&
        hModule == GetModuleHandle(L"explorerframe.dll")) {
        Wh_Log(L">");
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return nullptr;
    }

    return FindResourceExW_Original(hModule, lpType, lpName, wLanguage);
}

using InternalGetWindowText_t = int(WINAPI*)(HWND hWnd,
                                             LPWSTR pString,
                                             int cchMaxCount);
InternalGetWindowText_t InternalGetWindowText_Original;
int WINAPI InternalGetWindowText_Hook(HWND hWnd,
                                      LPWSTR pString,
                                      int cchMaxCount) {
    int result = InternalGetWindowText_Original(hWnd, pString, cchMaxCount);
    if (result == 0 || !pString || cchMaxCount == 0) {
        return result;
    }

    if (g_settings.suffixRemovalMode != SuffixRemovalMode::Universal &&
        g_settings.suffixRules.empty()) {
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

    Wh_Log(L"Original text: %s", pString);

    std::wstring text = pString;
    bool modified = false;

    // Apply universal mode: remove last " - <text>" or "—<text>" part
    if (g_settings.suffixRemovalMode == SuffixRemovalMode::Universal &&
        result > 0 && pString) {
        // Find the last occurrence of " - ", " — ", or "—"
        size_t lastSepPos = std::wstring::npos;

        size_t hyphenPos = text.rfind(L" - ");
        size_t emDashSpacePos = text.rfind(L" — ");
        size_t emDashPos = text.rfind(L"—");

        // Find the rightmost separator
        // Note: Check longer patterns first to avoid matching "—" that's part
        // of " — "
        if (hyphenPos != std::wstring::npos) {
            lastSepPos = hyphenPos;
        }
        if (emDashSpacePos != std::wstring::npos &&
            (lastSepPos == std::wstring::npos || emDashSpacePos > lastSepPos)) {
            lastSepPos = emDashSpacePos;
        }
        // Only use emDashPos if it's not part of " — " pattern
        if (emDashPos != std::wstring::npos &&
            (lastSepPos == std::wstring::npos || emDashPos > lastSepPos) &&
            !(emDashSpacePos != std::wstring::npos &&
              emDashPos == emDashSpacePos + 1)) {
            lastSepPos = emDashPos;
        }

        if (lastSepPos != std::wstring::npos) {
            text = text.substr(0, lastSepPos);
            modified = true;
            Wh_Log(L"Universal mode: removed suffix after last separator");
        }
    }

    // Get all matching rules for this window's process
    std::vector<const SuffixRule*> rules = GetRulesForWindow(hWnd);

    if (!rules.empty() && result > 0 && pString) {
        // Apply all matching rules in order
        for (const auto* rule : rules) {
            try {
                std::wstring newText =
                    std::regex_replace(text, rule->search, rule->replace);
                if (newText != text) {
                    text = newText;
                    modified = true;
                }
            } catch (const std::regex_error& ex) {
                Wh_Log(L"Regex replace error %08X: %S",
                       static_cast<DWORD>(ex.code()), ex.what());
            }
        }
    }

    // Update the window text if it changed
    if (modified) {
        if (text.length() < static_cast<size_t>(cchMaxCount)) {
            wcscpy_s(pString, cchMaxCount, text.c_str());
            result = static_cast<int>(text.length());
            Wh_Log(L"Modified text: %s", pString);
        } else {
            Wh_Log(L"Result too long (%zu chars), keeping original",
                   text.length());
        }
    }

    return result;
}

void LoadSettings() {
    Wh_Log(L"LoadSettings");

    // Load File Explorer suffix mode
    PCWSTR mode = Wh_GetStringSetting(L"suffixRemovalMode");
    g_settings.suffixRemovalMode = SuffixRemovalMode::FileExplorerOnly;
    if (wcscmp(mode, L"off") == 0) {
        g_settings.suffixRemovalMode = SuffixRemovalMode::Off;
    } else if (wcscmp(mode, L"universal") == 0) {
        g_settings.suffixRemovalMode = SuffixRemovalMode::Universal;
    }
    Wh_FreeStringSetting(mode);

    // Load custom suffix rules
    g_settings.suffixRules.clear();

    for (int i = 0;; i++) {
        PCWSTR processId =
            Wh_GetStringSetting(L"suffixRules[%d].processIdentifier", i);
        PCWSTR search = Wh_GetStringSetting(L"suffixRules[%d].search", i);
        PCWSTR replace = Wh_GetStringSetting(L"suffixRules[%d].replace", i);

        bool hasRule = *search;

        if (!hasRule) {
            Wh_FreeStringSetting(processId);
            Wh_FreeStringSetting(search);
            Wh_FreeStringSetting(replace);
            break;
        }

        try {
            SuffixRule rule;

            // Convert processIdentifier to uppercase (empty = match all)
            if (*processId) {
                rule.processIdentifier = processId;
                LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                              &rule.processIdentifier[0],
                              static_cast<int>(rule.processIdentifier.length()),
                              &rule.processIdentifier[0],
                              static_cast<int>(rule.processIdentifier.length()),
                              nullptr, nullptr, 0);
            }

            rule.search = std::wregex(search);
            rule.replace = replace;

            Wh_Log(L"Loaded rule for '%s': '%s' -> '%s'",
                   rule.processIdentifier.empty()
                       ? L"<all processes>"
                       : rule.processIdentifier.c_str(),
                   search, replace);

            g_settings.suffixRules.push_back(std::move(rule));
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

    LoadSettings();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

    auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                     PCSTR targetName, void* hookFunction,
                                     void** originalFunction) {
        void* targetFunction =
            (void*)GetProcAddress(kernelBaseModule, targetName);
        if (!targetFunction) {
            targetFunction = (void*)GetProcAddress(kernel32Module, targetName);
            if (!targetFunction) {
                return FALSE;
            }
        }

        return Wh_SetFunctionHook(targetFunction, hookFunction,
                                  originalFunction);
    };

    setKernelFunctionHook("FindResourceExW", (void*)FindResourceExW_Hook,
                          (void**)&FindResourceExW_Original);

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

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
