// ==WindhawkMod==
// @id              clipboard-history-upgrade
// @name            Clipboard History Upgrade
// @description     Enhances Windows Win+V with Regex formatting, tracking parameter removal, and Markdown to Rich Text conversion.
// @version         1.1.0
// @author          SwiftExplorer567
// @github          https://github.com/SwiftExplorer567
// @include         *
// @compilerOptions -luser32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Clipboard History Upgrade

Enhance the Windows Clipboard History (Win+V) by automatically cleaning and formatting text when you copy it.

## Features:
*   **Regex-based formatting:** Automatically replace text (e.g., replace specific words, fix common typos) based on your custom rules.
*   **Remove tracking variables:** Automatically strip `utm_`, `fbclid`, and other common tracking parameters from copied URLs.
*   **Auto-Trim Whitespace:** Strip leading/trailing spaces and newlines.
*   **Text Unwrapper:** Merge broken text (e.g., from PDFs) back into fluid paragraphs.
*   **Smart Casing:** Convert text to lowercase, UPPERCASE, or Title Case automatically.
*   **Code Path Auto-Escaper:** Automatically formats Windows file paths (`C:\`) to escaped paths (`C:\\` or `C:/`).
*   **Data Extractor:** If copying a large text block, extract *only* URLs or Email Addresses.
*   **Markdown & Rich Text:** Auto-convert simple Markdown (`**bold**`) to rich text.
*   **Force Plain Text:** Strip all rich formatting from the source application.

*Note: Changes happen at the moment of copying. The cleaned text is what gets placed into standard clipboard history.*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RegexReplacements:
  - - Search: apple
      $name: Search Regex/String
    - Replace: orange
      $name: Replace String
  $name: Regex text replacements
  $description: >-
    Define custom find-and-replace rules using regular expressions.
    These are applied to all copied text.
- RemoveTrackingParams: true
  $name: Remove tracking parameters
  $description: >-
    Automatically strip utm_source, fbclid, gclid, and other
    common tracking parameters from copied URLs.
- AutoTrimWhitespace: false
  $name: Auto-trim whitespace
  $description: >-
    Remove leading and trailing spaces, tabs, and newlines
    that are often accidentally included when selecting text.
- UnwrapText: false
  $name: Unwrap text (PDF fixer)
  $description: >-
    Merge broken lines back into flowing paragraphs.
    Useful when copying text from PDFs or narrow columns
    that insert hard line breaks mid-sentence.
    Paragraph breaks (double newlines) are preserved.
- CasingMode: none
  $name: Smart casing
  $description: >-
    Automatically convert copied text to the selected casing style.
  $options:
  - none: None (no change)
  - lowercase: lowercase
  - uppercase: UPPERCASE
  - titlecase: Title Case
- PathEscaperMode: none
  $name: Path auto-escaper
  $description: >-
    When a Windows file path is detected (e.g. C:\Users\file.txt),
    automatically escape the backslashes for use in code.
  $options:
  - none: None (no change)
  - doubleBackslash: Double backslash (C:\\Users\\file.txt)
  - forwardSlash: Forward slash (C:/Users/file.txt)
- DataExtractorMode: none
  $name: Data extractor
  $description: >-
    Instead of copying the full text, extract only the URLs
    or email addresses found within it.
  $options:
  - none: None (copy full text)
  - urls: Extract URLs only
  - emails: Extract email addresses only
- ConvertMarkdownToRichText: true
  $name: Markdown to rich text
  $description: >-
    Detect simple Markdown syntax (**bold**, *italic*, [links](url))
    and inject an HTML Format clipboard payload so that rich text
    editors render the formatting when you paste.
- ForcePlainText: false
  $name: Force plain text
  $description: >-
    Strip all rich formatting (HTML, RTF, images) from the source
    application so that text always pastes as plain, unformatted text.
    Note: This overrides "Markdown to rich text" if both are enabled.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <vector>
#include <regex>
#include <cwctype>

struct RegexReplacementItem {
    std::wregex searchRegex;
    std::wstring replaceW;
};

std::vector<RegexReplacementItem> g_regexReplacements;
bool g_removeTrackingParams = true;
bool g_autoTrimWhitespace = false;
bool g_unwrapText = false;
int g_casingMode = 0;
int g_pathEscaperMode = 0;
int g_dataExtractorMode = 0;
bool g_convertMarkdownToRichText = true;
bool g_forcePlainText = false;

// -------------------------------------------------------------------------
// Settings Loader
// -------------------------------------------------------------------------

void LoadSettings()
{
    g_regexReplacements.clear();

    g_removeTrackingParams = Wh_GetIntSetting(L"RemoveTrackingParams");
    g_autoTrimWhitespace = Wh_GetIntSetting(L"AutoTrimWhitespace");
    g_unwrapText = Wh_GetIntSetting(L"UnwrapText");
    g_convertMarkdownToRichText = Wh_GetIntSetting(L"ConvertMarkdownToRichText");
    g_forcePlainText = Wh_GetIntSetting(L"ForcePlainText");

    PCWSTR casingMode = Wh_GetStringSetting(L"CasingMode");
    g_casingMode = 0;
    if (wcscmp(casingMode, L"lowercase") == 0) g_casingMode = 1;
    else if (wcscmp(casingMode, L"uppercase") == 0) g_casingMode = 2;
    else if (wcscmp(casingMode, L"titlecase") == 0) g_casingMode = 3;
    Wh_FreeStringSetting(casingMode);

    PCWSTR pathMode = Wh_GetStringSetting(L"PathEscaperMode");
    g_pathEscaperMode = 0;
    if (wcscmp(pathMode, L"doubleBackslash") == 0) g_pathEscaperMode = 1;
    else if (wcscmp(pathMode, L"forwardSlash") == 0) g_pathEscaperMode = 2;
    Wh_FreeStringSetting(pathMode);

    PCWSTR extractorMode = Wh_GetStringSetting(L"DataExtractorMode");
    g_dataExtractorMode = 0;
    if (wcscmp(extractorMode, L"urls") == 0) g_dataExtractorMode = 1;
    else if (wcscmp(extractorMode, L"emails") == 0) g_dataExtractorMode = 2;
    Wh_FreeStringSetting(extractorMode);

    for (int i = 0;; i++) {
        PCWSTR search = Wh_GetStringSetting(L"RegexReplacements[%d].Search", i);
        bool hasSearch = *search;

        if (hasSearch) {
            PCWSTR replace = Wh_GetStringSetting(L"RegexReplacements[%d].Replace", i);

            try {
                g_regexReplacements.push_back({
                    std::wregex(search),
                    std::wstring(replace)
                });
            } catch (const std::regex_error&) {
                Wh_Log(L"Invalid regex provided in settings: %s", search);
            }
            Wh_FreeStringSetting(replace);
        }

        Wh_FreeStringSetting(search);

        if (!hasSearch) {
            break;
        }
    }
}

// -------------------------------------------------------------------------
// Text Transformations
// -------------------------------------------------------------------------

std::wstring ApplyRegexReplacements(std::wstring text)
{
    for (const auto& item : g_regexReplacements) {
        text = std::regex_replace(text, item.searchRegex, item.replaceW);
    }
    return text;
}

std::wstring RemoveUrlTrackingParams(std::wstring text)
{
    if (!g_removeTrackingParams) return text;
    
    if (text.find(L"http") == std::wstring::npos || text.find(L"?") == std::wstring::npos) {
        return text;
    }

    std::wregex trackingRegex(L"([?&])(utm_[^&=]+|fbclid|gclid|igshid|mc_cid|mc_eid|msclkid)=[^&#]*(&?)", std::regex_constants::icase);
    
    std::wstring prevText;
    do {
        prevText = text;
        text = std::regex_replace(text, trackingRegex, L"$1$3");
    } while (text != prevText);

    text = std::regex_replace(text, std::wregex(L"&&+"), L"&");
    text = std::regex_replace(text, std::wregex(L"\\?&"), L"?");
    text = std::regex_replace(text, std::wregex(L"[?&]$"), L"");

    return text;
}

std::wstring ExtractData(const std::wstring& text)
{
    if (g_dataExtractorMode == 0) return text;

    std::wregex pattern;
    if (g_dataExtractorMode == 1) {
        pattern = std::wregex(L"https?://[^\\s]+", std::regex_constants::icase);
    } else if (g_dataExtractorMode == 2) {
        pattern = std::wregex(L"[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}", std::regex_constants::icase);
    }

    std::wstring result = L"";
    auto words_begin = std::wsregex_iterator(text.begin(), text.end(), pattern);
    auto words_end = std::wsregex_iterator();

    for (std::wsregex_iterator i = words_begin; i != words_end; ++i) {
        std::wsmatch match = *i;
        result += match.str() + L"\r\n";
    }

    if (!result.empty()) {
        result.pop_back();
        result.pop_back();
    }

    return result.empty() ? text : result;
}

std::wstring TrimWhitespace(std::wstring text)
{
    if (!g_autoTrimWhitespace) return text;
    
    auto start = text.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";
    
    auto end = text.find_last_not_of(L" \t\r\n");
    return text.substr(start, end - start + 1);
}

std::wstring UnwrapText(std::wstring text)
{
    if (!g_unwrapText) return text;

    text = std::regex_replace(text, std::wregex(L"\r\n"), L"\n");

    // Preserve paragraph breaks (double newlines) using a placeholder
    text = std::regex_replace(text, std::wregex(L"\n\n"), L"\x01\x01");
    text = std::regex_replace(text, std::wregex(L"\n"), L" ");
    text = std::regex_replace(text, std::wregex(L"\x01\x01"), L"\r\n\r\n");

    return text;
}

std::wstring ApplyCasing(std::wstring text)
{
    if (g_casingMode == 0) return text;

    if (g_casingMode == 1) { // Lowercase
        for (auto& c : text) c = std::towlower(c);
    } else if (g_casingMode == 2) { // UPPERCASE
        for (auto& c : text) c = std::towupper(c);
    } else if (g_casingMode == 3) { // Title Case
        bool newWord = true;
        for (auto& c : text) {
            if (std::iswspace(c)) {
                newWord = true;
            } else if (newWord) {
                c = std::towupper(c);
                newWord = false;
            } else {
                c = std::towlower(c);
            }
        }
    }
    return text;
}

std::wstring ApplyPathEscaper(std::wstring text)
{
    if (g_pathEscaperMode == 0) return text;

    if (text.find(L":\\") != std::wstring::npos || text.find(L"\\\\") == 0) {
        if (g_pathEscaperMode == 1) {
            text = std::regex_replace(text, std::wregex(L"\\\\+"), L"\\");
            text = std::regex_replace(text, std::wregex(L"\\\\"), L"\\\\");
        } else if (g_pathEscaperMode == 2) {
            text = std::regex_replace(text, std::wregex(L"\\\\+"), L"/");
        }
    }
    return text;
}

std::wstring CleanCopiedText(const std::wstring& originalText)
{
    std::wstring text = originalText;
    
    text = ExtractData(text);
    text = RemoveUrlTrackingParams(text);
    text = ApplyRegexReplacements(text);
    text = UnwrapText(text);
    text = ApplyCasing(text);
    text = ApplyPathEscaper(text);
    text = TrimWhitespace(text);
    
    return text;
}

// -------------------------------------------------------------------------
// Markdown to HTML Format Generation
// -------------------------------------------------------------------------

std::string ConvertMarkdownToHtml(const std::wstring& text)
{
    std::wstring htmlW = text;

    htmlW = std::regex_replace(htmlW, std::wregex(L"\\r\\n|\\r|\\n"), L"<br>\n");
    htmlW = std::regex_replace(htmlW, std::wregex(L"\\*\\*(.*?)\\*\\*"), L"<strong>$1</strong>");
    htmlW = std::regex_replace(htmlW, std::wregex(L"__(.*?)__"), L"<strong>$1</strong>");
    htmlW = std::regex_replace(htmlW, std::wregex(L"\\*([^\\*]+)\\*"), L"<em>$1</em>");
    htmlW = std::regex_replace(htmlW, std::wregex(L"_([^_]+)_"), L"<em>$1</em>");
    htmlW = std::regex_replace(htmlW, std::wregex(L"\\[(.*?)\\]\\((.*?)\\)"), L"<a href=\"$2\">$1</a>");

    int u8Len = WideCharToMultiByte(CP_UTF8, 0, htmlW.c_str(), -1, NULL, 0, NULL, NULL);
    std::string htmlU8(u8Len, 0);
    WideCharToMultiByte(CP_UTF8, 0, htmlW.c_str(), -1, &htmlU8[0], u8Len, NULL, NULL);
    
    if (!htmlU8.empty() && htmlU8.back() == '\0') {
        htmlU8.pop_back();
    }

    return htmlU8;
}

std::string GenerateClipboardHtmlPayload(const std::string& htmlBodyFragment)
{
    const char* headerFormat =
        "Version:0.9\r\n"
        "StartHTML:%010u\r\n"
        "EndHTML:%010u\r\n"
        "StartFragment:%010u\r\n"
        "EndFragment:%010u\r\n";

    const char* htmlPrefix =
        "<html>\r\n"
        "<body>\r\n"
        "<!--StartFragment-->";

    const char* htmlSuffix =
        "<!--EndFragment-->\r\n"
        "</body>\r\n"
        "</html>";

    std::string htmlPrefixStr = htmlPrefix;
    std::string htmlSuffixStr = htmlSuffix;

    size_t headerLength = 105;

    size_t startHtml = headerLength;
    size_t startFragment = startHtml + htmlPrefixStr.length();
    size_t endFragment = startFragment + htmlBodyFragment.length();
    size_t endHtml = endFragment + htmlSuffixStr.length();

    char headerBuffer[128];
    snprintf(headerBuffer, sizeof(headerBuffer), headerFormat, 
        (unsigned int)startHtml, 
        (unsigned int)endHtml, 
        (unsigned int)startFragment, 
        (unsigned int)endFragment);

    return std::string(headerBuffer) + htmlPrefixStr + htmlBodyFragment + htmlSuffixStr;
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------

using SetClipboardData_t = decltype(&SetClipboardData);
SetClipboardData_t pOriginalSetClipboardData;
HANDLE WINAPI SetClipboardDataHook(UINT uFormat, HANDLE hMem)
{
    if (g_forcePlainText) {
        static UINT cfHtml = 0;
        static UINT cfRtf = 0;
        if (cfHtml == 0) cfHtml = RegisterClipboardFormatW(L"HTML Format");
        if (cfRtf == 0) cfRtf = RegisterClipboardFormatW(L"Rich Text Format");

        if (uFormat == cfHtml || uFormat == cfRtf || uFormat == CF_DIB || uFormat == CF_BITMAP) {
            if (hMem) GlobalFree(hMem);
            return NULL;
        }
    }

    if (uFormat == CF_UNICODETEXT && hMem != NULL) {
        LPCWSTR pData = (LPCWSTR)GlobalLock(hMem);
        if (pData) {
            std::wstring originalText(pData);
            GlobalUnlock(hMem);

            std::wstring cleanedText = CleanCopiedText(originalText);

            if (g_convertMarkdownToRichText && !g_forcePlainText &&
               (cleanedText.find(L"**") != std::wstring::npos || 
                cleanedText.find(L"__") != std::wstring::npos ||
                cleanedText.find(L"*") != std::wstring::npos || 
                cleanedText.find(L"_") != std::wstring::npos ||
                (cleanedText.find(L"[") != std::wstring::npos && cleanedText.find(L"](") != std::wstring::npos))) {
                
                std::string htmlUtf8 = ConvertMarkdownToHtml(cleanedText);
                std::string htmlPayload = GenerateClipboardHtmlPayload(htmlUtf8);

                UINT cfHtml = RegisterClipboardFormatW(L"HTML Format");
                if (cfHtml) {
                    HGLOBAL hMemHtml = GlobalAlloc(GMEM_MOVEABLE, htmlPayload.length() + 1);
                    if (hMemHtml) {
                        LPVOID pMemHtml = GlobalLock(hMemHtml);
                        if (pMemHtml) {
                            memcpy(pMemHtml, htmlPayload.c_str(), htmlPayload.length() + 1);
                            GlobalUnlock(hMemHtml);
                            
                            pOriginalSetClipboardData(cfHtml, hMemHtml);
                        } else {
                            GlobalFree(hMemHtml);
                        }
                    }
                }
            }

            if (cleanedText != originalText) {
                SIZE_T size = (cleanedText.length() + 1) * sizeof(WCHAR);
                HANDLE hNewMem = GlobalAlloc(GMEM_MOVEABLE, size);
                if (hNewMem) {
                    LPWSTR pNewData = (LPWSTR)GlobalLock(hNewMem);
                    if (pNewData) {
                        memcpy(pNewData, cleanedText.c_str(), size);
                        GlobalUnlock(hNewMem);
                        
                        GlobalFree(hMem);

                        return pOriginalSetClipboardData(uFormat, hNewMem);
                    } else {
                        GlobalFree(hNewMem);
                    }
                }
            }
        }
    }

    return pOriginalSetClipboardData(uFormat, hMem);
}


// -------------------------------------------------------------------------
// Init
// -------------------------------------------------------------------------

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");
    LoadSettings();

    Wh_SetFunctionHook((void*)SetClipboardData, (void*)SetClipboardDataHook, (void**)&pOriginalSetClipboardData);

    return TRUE;
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged(void)
{
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
