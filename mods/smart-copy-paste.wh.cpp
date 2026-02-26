// ==WindhawkMod==
// @id              smart-copy-paste
// @name            Smart Copy & Paste
// @description     Automatically format, clean, and enrich text instantly as you copy it to the clipboard.
// @version         1.3.0
// @author          SwiftExplorer567
// @github          https://github.com/SwiftExplorer567
// @include         *
// @compilerOptions -luser32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 📋 Smart Copy & Paste

**Enhance your everyday copy and paste experience by automatically cleaning, formatting, and upgrading text the moment you copy it.**

Whether you're pasting normally (`Ctrl+V`) or using the Windows Clipboard History (`Win+V`), your text will already be formatted exactly how you need it!

---

## ✨ Core Features

*   **⚡ Regex-based formatting:** Automatically replace text based on your custom rules. Perfect for fixing common typos or replacing specific words on the fly.
*   **🛡️ Remove Tracking Variables:** Automatically strip invasive `utm_`, `fbclid`, `gclid`, and other marketing parameters from copied URLs before sharing them.
*   **✂️ Auto-Trim Whitespace:** Instantly strip invisible leading/trailing spaces, tabs, and newlines that are accidentally included during text selection.
*   **📄 PDF Text Unwrapper:** Merge broken text strings back into fluid paragraphs. Extremely useful when copying tabular data or text from narrow PDF columns.
*   **🔠 Smart Casing:** Auto-convert copied text into `lowercase`, `UPPERCASE`, or `Title Case`.
*   **💻 Code Path Auto-Escaper:** Detects Windows file paths (e.g., `C:\Users\file.txt`) and automatically escapes the backslashes (`C:\\` or `C:/`) so they are instantly ready to paste into code.
*   **📥 Data Extractor:** Instead of copying bulk text, cleanly extract *only* the URLs or Email Addresses found within a massive block of text.
*   **📝 Markdown to Rich Text:** Type simple Markdown (like `**bold**` or `[links](url)`) and have it automatically converted into actual Rich Text (`CF_HTML`) on the clipboard.
*   **🚫 Force Plain Text:** Strip all annoying rich formatting (HTML, RTF, fonts, colors) from the source application, ensuring text always pastes matching the destination format.
*   **⌨️ Trigger Modifier Key:** Optionally restrict all these features so they *only* apply when holding a specific key (Shift/Alt) while copying.

---

### 💡 Why use this?
Instead of manually cleaning up tracking URLs, manually escaping backslashes, or continuously using *Paste as Plain Text* (Ctrl+Shift+V), this mod intercepts the Windows clipboard at the system level and sanitizes the data instantly.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TriggerModifierKey: none
  $name: Trigger modifier key
  $description: >-
    Only apply formatting if this key is held while copying. If none is selected, formatting always applies.
  $options:
  - none: None (always process)
  - shift: Shift
  - alt: Alt
- RegexReplacements:
  - - Search: apple
      $name: Search Regex/String
    - Replace: orange
      $name: Replace String
  $name: Regex text replacements
  $description: >-
    Define custom find-and-replace rules using regular expressions.
    These are applied to all copied text.
- RemoveTrackingParams: false
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
- ForcePlainText: false
  $name: Force plain text
  $description: >-
    Strip all rich formatting (HTML, RTF, images) from the source
    application so that text always pastes as plain, unformatted text.
*/
// ==/WindhawkModSettings==

#include <cwchar>
#include <cwctype>
#include <regex>
#include <string>
#include <vector>
#include <windows.h>

struct RegexReplacementItem {
  std::wregex searchRegex;
  std::wstring replaceW;
};

std::vector<RegexReplacementItem> g_regexReplacements;
bool g_removeTrackingParams = false;
bool g_autoTrimWhitespace = false;
bool g_unwrapText = false;
int g_casingMode = 0;
int g_pathEscaperMode = 0;
int g_dataExtractorMode = 0;
bool g_forcePlainText = false;
int g_triggerModifierKey = 0;

// -------------------------------------------------------------------------
// Settings Loader
// -------------------------------------------------------------------------

void LoadSettings() {
  g_regexReplacements.clear();

  g_removeTrackingParams = Wh_GetIntSetting(L"RemoveTrackingParams");
  g_autoTrimWhitespace = Wh_GetIntSetting(L"AutoTrimWhitespace");
  g_unwrapText = Wh_GetIntSetting(L"UnwrapText");
  g_forcePlainText = Wh_GetIntSetting(L"ForcePlainText");

  PCWSTR triggerKey = Wh_GetStringSetting(L"TriggerModifierKey");
  g_triggerModifierKey = 0;
  if (triggerKey) {
    if (wcscmp(triggerKey, L"shift") == 0)
      g_triggerModifierKey = 1;
    else if (wcscmp(triggerKey, L"alt") == 0)
      g_triggerModifierKey = 2;
    Wh_FreeStringSetting(triggerKey);
  }

  PCWSTR casingMode = Wh_GetStringSetting(L"CasingMode");
  g_casingMode = 0;
  if (wcscmp(casingMode, L"lowercase") == 0)
    g_casingMode = 1;
  else if (wcscmp(casingMode, L"uppercase") == 0)
    g_casingMode = 2;
  else if (wcscmp(casingMode, L"titlecase") == 0)
    g_casingMode = 3;
  Wh_FreeStringSetting(casingMode);

  PCWSTR pathMode = Wh_GetStringSetting(L"PathEscaperMode");
  g_pathEscaperMode = 0;
  if (wcscmp(pathMode, L"doubleBackslash") == 0)
    g_pathEscaperMode = 1;
  else if (wcscmp(pathMode, L"forwardSlash") == 0)
    g_pathEscaperMode = 2;
  Wh_FreeStringSetting(pathMode);

  PCWSTR extractorMode = Wh_GetStringSetting(L"DataExtractorMode");
  g_dataExtractorMode = 0;
  if (wcscmp(extractorMode, L"urls") == 0)
    g_dataExtractorMode = 1;
  else if (wcscmp(extractorMode, L"emails") == 0)
    g_dataExtractorMode = 2;
  Wh_FreeStringSetting(extractorMode);

  for (int i = 0;; i++) {
    PCWSTR search = Wh_GetStringSetting(L"RegexReplacements[%d].Search", i);
    bool hasSearch = *search;

    if (hasSearch) {
      PCWSTR replace = Wh_GetStringSetting(L"RegexReplacements[%d].Replace", i);

      try {
        g_regexReplacements.push_back(
            {std::wregex(search), std::wstring(replace)});
      } catch (const std::regex_error &) {
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

std::wstring ApplyRegexReplacements(std::wstring text) {
  for (const auto &item : g_regexReplacements) {
    text = std::regex_replace(text, item.searchRegex, item.replaceW);
  }
  return text;
}

std::wstring RemoveUrlTrackingParams(std::wstring text) {
  if (!g_removeTrackingParams)
    return text;

  if (text.find(L"http") == std::wstring::npos ||
      text.find(L"?") == std::wstring::npos) {
    return text;
  }

  static const std::wregex trackingRegex(L"([?&])(utm_[^&=]+|fbclid|gclid|igshid|mc_cid|mc_"
                            L"eid|msclkid)=[^&#\\r\\n]*(&?)",
                            std::regex_constants::icase);
  static const std::wregex ampersandMerge(L"&&+");
  static const std::wregex questionAmpersand(L"\\?&");
  static const std::wregex dangling(L"[?&](?=\\s|$)");

  std::wstring prevText;
  do {
    prevText = text;
    text = std::regex_replace(text, trackingRegex, L"$1$3");
  } while (text != prevText);

  text = std::regex_replace(text, ampersandMerge, L"&");
  text = std::regex_replace(text, questionAmpersand, L"?");
  text = std::regex_replace(text, dangling, L"");

  return text;
}

std::wstring ExtractData(const std::wstring &text) {
  if (g_dataExtractorMode == 0)
    return text;

  static const std::wregex urlPattern(L"https?://[^\\s]+", std::regex_constants::icase);
  static const std::wregex emailPattern(L"[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}", std::regex_constants::icase);

  const std::wregex* pattern = nullptr;
  if (g_dataExtractorMode == 1) {
    pattern = &urlPattern;
  } else if (g_dataExtractorMode == 2) {
    pattern = &emailPattern;
  } else {
    return text;
  }

  std::wstring result;
  auto words_begin = std::wsregex_iterator(text.begin(), text.end(), *pattern);
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

std::wstring TrimWhitespace(std::wstring text) {
  if (!g_autoTrimWhitespace)
    return text;

  auto start = text.find_first_not_of(L" \t\r\n");
  if (start == std::wstring::npos)
    return L"";

  auto end = text.find_last_not_of(L" \t\r\n");
  return text.substr(start, end - start + 1);
}

std::wstring UnwrapText(std::wstring text) {
  if (!g_unwrapText)
    return text;

  static const std::wregex winNewline(L"\r\n");
  static const std::wregex doubleNewline(L"\n\n");
  static const std::wregex singleNewline(L"\n");
  static const std::wregex placeholder(L"\x01\x01");

  text = std::regex_replace(text, winNewline, L"\n");

  // Preserve paragraph breaks (double newlines) using a placeholder
  text = std::regex_replace(text, doubleNewline, L"\x01\x01");
  text = std::regex_replace(text, singleNewline, L" ");
  text = std::regex_replace(text, placeholder, L"\r\n\r\n");

  return text;
}

std::wstring ApplyCasing(std::wstring text) {
  if (g_casingMode == 0)
    return text;

  if (g_casingMode == 1) { // Lowercase
    for (auto &c : text)
      c = std::towlower(c);
  } else if (g_casingMode == 2) { // UPPERCASE
    for (auto &c : text)
      c = std::towupper(c);
  } else if (g_casingMode == 3) { // Title Case
    bool newWord = true;
    for (auto &c : text) {
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

std::wstring ApplyPathEscaper(std::wstring text) {
  if (g_pathEscaperMode == 0)
    return text;

  if (text.find(L":\\") != std::wstring::npos || text.find(L"\\\\") == 0) {
    static const std::wregex doubleSlash(L"\\\\+");
    static const std::wregex singleSlash(L"\\\\");

    if (g_pathEscaperMode == 1) {
      text = std::regex_replace(text, doubleSlash, L"\\");
      text = std::regex_replace(text, singleSlash, L"\\\\");
    } else if (g_pathEscaperMode == 2) {
      text = std::regex_replace(text, doubleSlash, L"/");
    }
  }
  return text;
}

std::wstring CleanCopiedText(const std::wstring &originalText) {
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

std::string ConvertMarkdownToHtml(const std::wstring &text) {
  std::wstring htmlW = text;

  static const std::wregex amp(L"&");
  static const std::wregex lt(L"<");
  static const std::wregex gt(L">");
  static const std::wregex newline(L"\\r\\n|\\r|\\n");
  static const std::wregex bold1(L"\\*\\*(.*?)\\*\\*");
  static const std::wregex bold2(L"__(.*?)__");
  static const std::wregex italic1(L"\\*([^\\*]+)\\*");
  static const std::wregex italic2(L"_([^_]+)_");
  static const std::wregex link(L"\\[(.*?)\\]\\((.*?)\\)");

  htmlW = std::regex_replace(htmlW, amp, L"&amp;");
  htmlW = std::regex_replace(htmlW, lt, L"&lt;");
  htmlW = std::regex_replace(htmlW, gt, L"&gt;");

  htmlW = std::regex_replace(htmlW, newline, L"<br>\n");
  htmlW = std::regex_replace(htmlW, bold1, L"<strong>$1</strong>");
  htmlW = std::regex_replace(htmlW, bold2, L"<strong>$1</strong>");
  htmlW = std::regex_replace(htmlW, italic1, L"<em>$1</em>");
  htmlW = std::regex_replace(htmlW, italic2, L"<em>$1</em>");
  htmlW = std::regex_replace(htmlW, link, L"<a href=\"$2\">$1</a>");

  int u8Len =
      WideCharToMultiByte(CP_UTF8, 0, htmlW.c_str(), -1, NULL, 0, NULL, NULL);
  std::string htmlU8(u8Len, 0);
  WideCharToMultiByte(CP_UTF8, 0, htmlW.c_str(), -1, &htmlU8[0], u8Len, NULL,
                      NULL);

  if (!htmlU8.empty() && htmlU8.back() == '\0') {
    htmlU8.pop_back();
  }

  return htmlU8;
}

std::string GenerateClipboardHtmlPayload(const std::string &htmlBodyFragment) {
  const char *headerFormat = "Version:0.9\r\n"
                             "StartHTML:%010u\r\n"
                             "EndHTML:%010u\r\n"
                             "StartFragment:%010u\r\n"
                             "EndFragment:%010u\r\n";

  const char *htmlPrefix = "<html>\r\n"
                           "<body>\r\n"
                           "<!--StartFragment-->";

  const char *htmlSuffix = "<!--EndFragment-->\r\n"
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
           (unsigned int)startHtml, (unsigned int)endHtml,
           (unsigned int)startFragment, (unsigned int)endFragment);

  return std::string(headerBuffer) + htmlPrefixStr + htmlBodyFragment +
         htmlSuffixStr;
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------

thread_local DWORD t_lastClipboardSeq = 0;
thread_local bool t_modifiedCurrentSeq = false;
thread_local bool t_isCopyingOurFormats = false;
thread_local bool t_shouldFormatThisSeq = true;

using SetClipboardData_t = decltype(&SetClipboardData);
SetClipboardData_t pOriginalSetClipboardData;
HANDLE WINAPI SetClipboardDataHook(UINT uFormat, HANDLE hMem) {

  if (t_isCopyingOurFormats) {
    return pOriginalSetClipboardData(uFormat, hMem);
  }

  DWORD seq = GetClipboardSequenceNumber();
  if (seq != t_lastClipboardSeq) {
    t_lastClipboardSeq = seq;
    t_modifiedCurrentSeq = false;
    
    t_shouldFormatThisSeq = true;
    if (g_triggerModifierKey != 0) {
      t_shouldFormatThisSeq = false; // Require key if set
      if (g_triggerModifierKey == 1 && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) t_shouldFormatThisSeq = true;
      else if (g_triggerModifierKey == 2 && (GetAsyncKeyState(VK_MENU) & 0x8000)) t_shouldFormatThisSeq = true;
    }
  }

  if (!t_shouldFormatThisSeq) {
    return pOriginalSetClipboardData(uFormat, hMem);
  }

  static UINT cfHtml = RegisterClipboardFormatW(L"HTML Format");
  static UINT cfRtf = RegisterClipboardFormatW(L"Rich Text Format");

  if (g_forcePlainText || t_modifiedCurrentSeq) {
    if (uFormat != CF_UNICODETEXT && uFormat != CF_TEXT && uFormat != CF_OEMTEXT && uFormat != CF_LOCALE) {
      if (hMem) {
        static UINT cfDropped = RegisterClipboardFormatW(L"Windhawk_Dropped");
        return pOriginalSetClipboardData(cfDropped, hMem);
      }
      return NULL;
    }
  }

  if (uFormat == CF_UNICODETEXT && hMem != NULL) {
    SIZE_T size = GlobalSize(hMem);
    SIZE_T maxChars = size / sizeof(WCHAR);
    if (maxChars > 0 && maxChars < 5 * 1024 * 1024) { // 10MB safety limit
      LPCWSTR pData = (LPCWSTR)GlobalLock(hMem);
      if (pData) {
        SIZE_T actualLen = 0;
        while (actualLen < maxChars && pData[actualLen] != L'\0') {
          actualLen++;
        }
        std::wstring originalText(pData, actualLen);
        GlobalUnlock(hMem);

        std::wstring cleanedText = CleanCopiedText(originalText);

        if (cleanedText != originalText || g_forcePlainText) {
          t_modifiedCurrentSeq = true;
          SIZE_T allocSize = (cleanedText.length() + 1) * sizeof(WCHAR);
          HANDLE hNewMem = GlobalAlloc(GMEM_MOVEABLE, allocSize);
          if (hNewMem) {
            LPWSTR pNewData = (LPWSTR)GlobalLock(hNewMem);
            if (pNewData) {
              memcpy(pNewData, cleanedText.c_str(), allocSize);
              GlobalUnlock(hNewMem);

              HANDLE hRet = pOriginalSetClipboardData(uFormat, hNewMem);
              if (hRet) {
                GlobalFree(hMem);
                
                if (!g_forcePlainText && cfHtml) {
                    t_isCopyingOurFormats = true;

                    std::string htmlPayload = GenerateClipboardHtmlPayload(ConvertMarkdownToHtml(cleanedText));
                    HANDLE hHtmlMem = GlobalAlloc(GMEM_MOVEABLE, htmlPayload.length() + 1);
                    if (hHtmlMem) {
                        LPSTR pHtmlData = (LPSTR)GlobalLock(hHtmlMem);
                        if (pHtmlData) {
                            memcpy(pHtmlData, htmlPayload.c_str(), htmlPayload.length() + 1);
                            GlobalUnlock(hHtmlMem);
                            pOriginalSetClipboardData(cfHtml, hHtmlMem);
                        } else {
                            GlobalFree(hHtmlMem);
                        }
                    }

                    if (cfRtf) {
                        HANDLE hRtfMem = GlobalAlloc(GMEM_MOVEABLE, 1);
                        if (hRtfMem) {
                            LPSTR pRtfData = (LPSTR)GlobalLock(hRtfMem);
                            if (pRtfData) {
                                pRtfData[0] = '\0';
                                GlobalUnlock(hRtfMem);
                                pOriginalSetClipboardData(cfRtf, hRtfMem);
                            } else {
                                GlobalFree(hRtfMem);
                            }
                        }
                    }

                    t_isCopyingOurFormats = false;
                }

                return hRet;
              } else {
                GlobalFree(hNewMem);
                return NULL;
              }
            } else {
              GlobalFree(hNewMem);
            }
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

BOOL Wh_ModInit(void) {
  Wh_Log(L"Init");
  LoadSettings();

  Wh_SetFunctionHook((void *)SetClipboardData, (void *)SetClipboardDataHook,
                     (void **)&pOriginalSetClipboardData);

  return TRUE;
}

void Wh_ModUninit(void) { Wh_Log(L"Uninit"); }

void Wh_ModSettingsChanged(void) {
  Wh_Log(L"SettingsChanged");
  LoadSettings();
}