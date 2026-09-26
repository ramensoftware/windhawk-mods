// ==WindhawkMod==
// @id              quick-translator-layout-switcher
// @name            Selection Layout Switcher & Translator
// @description     Fast layout corrector, in-place translator, and floating HUD tooltip.
// @version         1.0.1
// @author          zed712969-crypto
// @github          https://github.com/zed712969-crypto
// @include         windhawk.exe
// @compilerOptions -lwinhttp -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Selection Layout Switcher & Translator

A lightweight Windows utility that bridges communication between English and Russian speakers. Eliminates mistyped keyboard layout errors and allows instant two-way translation without breaking workflow.

> **Privacy Notice**: This mod optionally connects to Google Translate (`translate.googleapis.com`) over HTTPS to perform translations. Selected text is transmitted only when a translation hotkey is triggered. This feature is strictly **opt-in and disabled by default**.

## Features

- **Layout Correction (`Ctrl + Alt + L`)**: Fixes mistyped text (`ghbdtn` -> `привет` or vice versa) with full punctuation mapping and automatically updates the active system input layout.
- **In-Place Translation (`Ctrl + Alt + T`)**: Replaces selected text directly inside any editable input field using HTTPS POST requests. Supports multi-line paragraphs.
- **Floating HUD Tooltip (`Ctrl + Alt + Q`)**: Translates read-only incoming messages in a sleek DPI-aware dark tooltip positioned near your cursor.
- **Smart Auto-Dismiss**: The tooltip remains visible until you click anywhere, scroll the mouse wheel, or press navigation keys (Esc / Arrows).
- **Clipboard Preservation**: Automatically restores previous clipboard contents after performing operations.

## Default Hotkeys

- `Ctrl + Alt + L` - Switch mistyped layout (US <-> RU)
- `Ctrl + Alt + T` - In-place translation (Requires enabling online translation in Settings)
- `Ctrl + Alt + Q` - Floating HUD Tooltip translation (Requires enabling online translation in Settings)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enable_translation: false
  $name: "Enable Online Translation"
  $description: "Allow sending selected text to Google Translate over HTTPS for translation features. Disabled by default for privacy."
- hotkey_layout: "Ctrl + Alt + L"
  $name: "Layout Correction Hotkey"
  $description: "Switch mistyped layout (default: Ctrl + Alt + L). Supports combinations of Ctrl, Shift, Alt, Space, Insert, letters, digits."
- hotkey_translate: "Ctrl + Alt + T"
  $name: "In-Place Translate Hotkey"
  $description: "Translate selected text inline (default: Ctrl + Alt + T)."
- hotkey_tooltip: "Ctrl + Alt + Q"
  $name: "HUD Tooltip Translate Hotkey"
  $description: "Translate selected text in a floating tooltip (default: Ctrl + Alt + Q)."
- anim_duration: 150
  $name: "Animation Duration (ms)"
  $description: "Fade animation speed in ms (0 to disable)."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>
#include <shellapi.h>
#include <string>
#include <unordered_map>
#include <cwctype>
#include <cwchar>
#include <memory>
#include <atomic>
#include <windhawk_utils.h>

#define WM_SHOW_TOOLTIP (WM_USER + 101)
#define WM_HIDE_TOOLTIP (WM_USER + 102)
#define WM_APP_SETTINGS_CHANGED (WM_USER + 103)

#define TIMER_ANIM 2

HHOOK g_keyboardHook = NULL;
HHOOK g_mouseHook = NULL;
HANDLE g_hMainThread = NULL;
DWORD g_mainThreadId = 0;

HANDLE g_hWorkerThread = NULL;
std::atomic<bool> g_bStopRequested{ false };

HWND g_hTooltipWnd = NULL;
HFONT g_hTooltipFont = NULL;
UINT g_tooltipDpi = 96;
std::wstring g_tooltipText = L"";

std::unordered_map<wchar_t, wchar_t> g_toRuMap;
std::unordered_map<wchar_t, wchar_t> g_toEnMap;

enum AnimState { STATE_HIDDEN, STATE_FADE_IN, STATE_VISIBLE, STATE_FADE_OUT };
AnimState g_animState = STATE_HIDDEN;
DWORD g_animStartTime = 0;
int g_targetX = 0, g_targetY = 0, g_winW = 0, g_winH = 0;

bool g_bEnableTranslation = false;
int g_animDuration = 150;

struct HotkeyConfig {
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    WORD vk = 0;
};

HotkeyConfig g_hkLayout;
HotkeyConfig g_hkTranslate;
HotkeyConfig g_hkTooltip;

void AddMapping(wchar_t en, wchar_t ru) {
    g_toRuMap[en] = ru;
    g_toEnMap[ru] = en;
}

void InitMaps() {
    g_toRuMap.clear();
    g_toEnMap.clear();

    const wchar_t* enLower = L"qwertyuiop[]asdfghjkl;'zxcvbnm,./`";
    const wchar_t* ruLower = L"йцукенгшщзхъфывапролджэячсмитьбю.ё";
    for (size_t i = 0; enLower[i] && ruLower[i]; ++i) AddMapping(enLower[i], ruLower[i]);

    const wchar_t* enUpper = L"QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?~";
    const wchar_t* ruUpper = L"ЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ,Ё";
    for (size_t i = 0; enUpper[i] && ruUpper[i]; ++i) AddMapping(enUpper[i], ruUpper[i]);

    AddMapping(L'@', L'\"');
    AddMapping(L'#', L'№');
    AddMapping(L'$', L';');
    AddMapping(L'^', L':');
    AddMapping(L'&', L'?');
}

std::wstring ToLowerStr(const std::wstring& s) {
    std::wstring res = s;
    for (wchar_t& c : res) c = (wchar_t)towlower(c);
    return res;
}

void ParseHotkey(const std::wstring& str, HotkeyConfig& cfg, bool defCtrl, bool defShift, bool defAlt, WORD defVk) {
    cfg.ctrl = false;
    cfg.shift = false;
    cfg.alt = false;
    cfg.vk = 0;

    if (str.empty()) {
        cfg.ctrl = defCtrl; cfg.shift = defShift; cfg.alt = defAlt; cfg.vk = defVk;
        return;
    }

    size_t start = 0;
    while (start < str.length()) {
        size_t end = str.find(L'+', start);
        if (end == std::string::npos) end = str.length();

        std::wstring token = str.substr(start, end - start);
        size_t first = token.find_first_not_of(L" \t\r\n");
        size_t last = token.find_last_not_of(L" \t\r\n");

        if (first != std::string::npos && last != std::string::npos) {
            token = token.substr(first, last - first + 1);
            std::wstring lower = ToLowerStr(token);

            if (lower == L"ctrl" || lower == L"control") {
                cfg.ctrl = true;
            } else if (lower == L"shift") {
                cfg.shift = true;
            } else if (lower == L"alt") {
                cfg.alt = true;
            } else if (lower == L"pause" || lower == L"break") {
                cfg.vk = VK_PAUSE;
            } else if (lower == L"insert") {
                cfg.vk = VK_INSERT;
            } else if (lower == L"delete") {
                cfg.vk = VK_DELETE;
            } else if (lower == L"home") {
                cfg.vk = VK_HOME;
            } else if (lower == L"end") {
                cfg.vk = VK_END;
            } else if (lower == L"pageup" || lower == L"prior") {
                cfg.vk = VK_PRIOR;
            } else if (lower == L"pagedown" || lower == L"next") {
                cfg.vk = VK_NEXT;
            } else if (lower == L"scrolllock" || lower == L"scroll") {
                cfg.vk = VK_SCROLL;
            } else if (lower == L"~" || lower == L"`" || lower == L"tilde") {
                cfg.vk = VK_OEM_3;
            } else if (lower.length() >= 2 && lower[0] == L'f' && iswdigit(lower[1])) {
                int fnum = (int)std::wcstol(lower.c_str() + 1, nullptr, 10);
                if (fnum >= 1 && fnum <= 24) cfg.vk = VK_F1 + (fnum - 1);
            } else if (lower == L"tab") {
                cfg.vk = VK_TAB;
            } else if (lower == L"space") {
                cfg.vk = VK_SPACE;
            } else if (token.length() == 1) {
                wchar_t c = token[0];
                if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')) {
                    cfg.vk = (WORD)towupper(c);
                } else if (c >= L'0' && c <= L'9') {
                    cfg.vk = (WORD)c;
                } else if (g_toEnMap.count(c)) {
                    wchar_t en = g_toEnMap[c];
                    cfg.vk = (WORD)towupper(en);
                }
            }
        }
        start = end + 1;
    }

    if (cfg.vk == 0) {
        cfg.ctrl = defCtrl; cfg.shift = defShift; cfg.alt = defAlt; cfg.vk = defVk;
    }
}

void LoadSettings() {
    g_bEnableTranslation = Wh_GetIntSetting(L"enable_translation") != 0;

    PCWSTR sLayout = Wh_GetStringSetting(L"hotkey_layout");
    ParseHotkey(sLayout ? sLayout : L"", g_hkLayout, true, false, true, 'L');
    if (sLayout) Wh_FreeStringSetting(sLayout);

    PCWSTR sTrans = Wh_GetStringSetting(L"hotkey_translate");
    ParseHotkey(sTrans ? sTrans : L"", g_hkTranslate, true, false, true, 'T');
    if (sTrans) Wh_FreeStringSetting(sTrans);

    PCWSTR sTooltip = Wh_GetStringSetting(L"hotkey_tooltip");
    ParseHotkey(sTooltip ? sTooltip : L"", g_hkTooltip, true, false, true, 'Q');
    if (sTooltip) Wh_FreeStringSetting(sTooltip);

    g_animDuration = Wh_GetIntSetting(L"anim_duration");
    if (g_animDuration < 0) g_animDuration = 150;
}

void WaitForModifiersUp() {
    for (int i = 0; i < 30; ++i) {
        bool ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool alt   = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        if (!ctrl && !shift && !alt) break;
        Sleep(10);
    }

    INPUT inputs[3] = {};
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_MENU;    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_CONTROL; inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = VK_SHIFT;   inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(3, inputs, sizeof(INPUT));
}

void SendCtrlKey(WORD vk) {
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_MENU;    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_CONTROL; inputs[1].ki.dwFlags = 0;
    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = vk;         inputs[2].ki.dwFlags = 0;
    SendInput(3, inputs, sizeof(INPUT));

    Sleep(25);

    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = vk;         inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_CONTROL; inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
}

bool ForceCopy() {
    WaitForModifiersUp();
    DWORD seqBefore = GetClipboardSequenceNumber();

    Sleep(25);
    SendCtrlKey('C');

    for (int i = 0; i < 25; ++i) {
        if (GetClipboardSequenceNumber() != seqBefore) return true;
        Sleep(15);
    }
    return false;
}

void ForcePaste() {
    WaitForModifiersUp();
    Sleep(25);
    SendCtrlKey('V');
}

std::wstring GetClipboardText() {
    std::wstring text = L"";
    for (int i = 0; i < 15; ++i) {
        if (OpenClipboard(NULL)) {
            HANDLE hData = GetClipboardData(CF_UNICODETEXT);
            if (hData) {
                wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
                if (pszText) {
                    text = pszText;
                    GlobalUnlock(hData);
                }
            }
            CloseClipboard();
            if (!text.empty()) break;
        }
        Sleep(20);
    }
    return text;
}

void SetClipboardText(const std::wstring& text) {
    for (int i = 0; i < 12; ++i) {
        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, (text.length() + 1) * sizeof(wchar_t));
            if (hGlob) {
                void* pBuf = GlobalLock(hGlob);
                if (pBuf) {
                    memcpy(pBuf, text.c_str(), (text.length() + 1) * sizeof(wchar_t));
                    GlobalUnlock(hGlob);
                    SetClipboardData(CF_UNICODETEXT, hGlob);
                } else {
                    GlobalFree(hGlob);
                }
            }
            CloseClipboard();
            break;
        }
        Sleep(20);
    }
}

std::string UrlEncodeUtf8(const std::wstring& wstr) {
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    if (len <= 0) return "";
    std::string utf8(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), &utf8[0], len, NULL, NULL);

    static const char hexChars[] = "0123456789ABCDEF";
    std::string escaped;
    escaped.reserve(len * 3);

    for (unsigned char c : utf8) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            escaped.push_back((char)c);
        } else {
            escaped.push_back('%');
            escaped.push_back(hexChars[(c >> 4) & 0x0F]);
            escaped.push_back(hexChars[c & 0x0F]);
        }
    }
    return escaped;
}

std::wstring ParseGoogleTranslateResponse(const std::string& json) {
    std::wstring fullResult = L"";
    const std::string key = "\"trans\":\"";
    size_t pos = 0;

    while ((pos = json.find(key, pos)) != std::string::npos) {
        pos += key.length();
        std::string transUtf8 = "";

        while (pos < json.length()) {
            if (json[pos] == '\\' && pos + 1 < json.length()) {
                char next = json[pos + 1];
                if (next == '\"') { transUtf8 += '\"'; pos += 2; }
                else if (next == '\\') { transUtf8 += '\\'; pos += 2; }
                else if (next == 'n') { transUtf8 += "\r\n"; pos += 2; }
                else if (next == 'r') { pos += 2; }
                else if (next == 't') { transUtf8 += '\t'; pos += 2; }
                else if (next == 'u' && pos + 5 < json.length()) {
                    unsigned int codepoint = 0;
                    for (int k = 2; k <= 5; ++k) {
                        char h = json[pos + k];
                        codepoint <<= 4;
                        if (h >= '0' && h <= '9') codepoint |= (h - '0');
                        else if (h >= 'a' && h <= 'f') codepoint |= (h - 'a' + 10);
                        else if (h >= 'A' && h <= 'F') codepoint |= (h - 'A' + 10);
                    }
                    wchar_t wch = (wchar_t)codepoint;
                    char utf8Buf[4] = {};
                    int u8len = WideCharToMultiByte(CP_UTF8, 0, &wch, 1, utf8Buf, sizeof(utf8Buf), NULL, NULL);
                    for (int i = 0; i < u8len; ++i) transUtf8 += utf8Buf[i];
                    pos += 6;
                } else {
                    transUtf8 += json[pos];
                    pos++;
                }
            } else if (json[pos] == '\"') {
                pos++;
                break;
            } else {
                transUtf8 += json[pos];
                pos++;
            }
        }

        if (!transUtf8.empty()) {
            int wlen = MultiByteToWideChar(CP_UTF8, 0, transUtf8.c_str(), (int)transUtf8.length(), NULL, 0);
            if (wlen > 0) {
                std::wstring wpart(wlen, 0);
                MultiByteToWideChar(CP_UTF8, 0, transUtf8.c_str(), (int)transUtf8.length(), &wpart[0], wlen);
                fullResult += wpart;
            }
        }
    }
    return fullResult;
}

std::wstring FetchTranslationWinHttp(const std::wstring& text, bool toRussian) {
    if (g_bStopRequested) return L"";

    HINTERNET hSession = WinHttpOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64)", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return L"";

    WinHttpSetTimeouts(hSession, 3000, 3000, 4000, 4000);

    HINTERNET hConnect = WinHttpConnect(hSession, L"translate.googleapis.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return L"";
    }

    std::wstring path = L"/translate_a/single?client=gtx&sl=auto&tl=" +
                        std::wstring(toRussian ? L"ru" : L"en") +
                        L"&dt=t&dj=1";

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return L"";
    }

    std::string postBody = "q=" + UrlEncodeUtf8(text);
    std::wstring headers = L"Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n";

    std::string response;
    if (WinHttpSendRequest(hRequest, headers.c_str(), (DWORD)headers.length(), 
                           (LPVOID)postBody.c_str(), (DWORD)postBody.length(), (DWORD)postBody.length(), 0) &&
        WinHttpReceiveResponse(hRequest, NULL)) {
        
        char buffer[4096];
        DWORD bytesRead = 0;
        while (WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            if (g_bStopRequested) break;
            buffer[bytesRead] = '\0';
            response.append(buffer, bytesRead);
        }
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    if (g_bStopRequested) return L"";
    return ParseGoogleTranslateResponse(response);
}

std::wstring FetchTranslationForText(const std::wstring& originalText) {
    if (originalText.empty()) return L"";

    int cyrillicCount = 0;
    int latinCount = 0;
    for (wchar_t c : originalText) {
        if ((c >= 0x0400 && c <= 0x04FF) || c == L'ё' || c == L'Ё') {
            cyrillicCount++;
        } else if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')) {
            latinCount++;
        }
    }
    bool toRussian = (latinCount >= cyrillicCount);

    return FetchTranslationWinHttp(originalText, toRussian);
}

DWORD WINAPI TranslationWorker(LPVOID) {
    std::wstring backupClipboard = GetClipboardText();

    if (!ForceCopy()) {
        return 0;
    }

    std::wstring originalText = GetClipboardText();
    std::wstring translated = FetchTranslationForText(originalText);

    if (translated.empty() || g_bStopRequested) {
        if (!backupClipboard.empty()) SetClipboardText(backupClipboard);
        if (!g_bStopRequested) MessageBeep(MB_ICONHAND);
        return 0;
    }

    SetClipboardText(translated);
    Sleep(40);
    ForcePaste();

    Sleep(250);
    if (!backupClipboard.empty() && !g_bStopRequested) {
        SetClipboardText(backupClipboard);
    }
    return 0;
}

DWORD WINAPI TooltipWorker(LPVOID) {
    std::wstring backupClipboard = GetClipboardText();

    if (!ForceCopy()) {
        return 0;
    }

    std::wstring originalText = GetClipboardText();
    if (!backupClipboard.empty()) {
        SetClipboardText(backupClipboard);
    }

    std::wstring translated = FetchTranslationForText(originalText);
    if (translated.empty() || g_bStopRequested) {
        if (!g_bStopRequested) MessageBeep(MB_ICONHAND);
        return 0;
    }

    if (g_hTooltipWnd && IsWindow(g_hTooltipWnd)) {
        auto* pText = new std::wstring(std::move(translated));
        if (!PostMessage(g_hTooltipWnd, WM_SHOW_TOOLTIP, 0, (LPARAM)pText)) {
            delete pText;
        }
    }
    return 0;
}

std::wstring ConvertLayout(const std::wstring& input, bool& outWasEnglish) {
    size_t enCount = 0, ruCount = 0;
    for (wchar_t c : input) {
        if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')) {
            enCount++;
        } else if ((c >= 0x0400 && c <= 0x04FF) || c == L'ё' || c == L'Ё') {
            ruCount++;
        }
    }

    if (enCount > 0 || ruCount > 0) {
        outWasEnglish = (enCount >= ruCount);
    } else {
        for (wchar_t c : input) {
            if (g_toRuMap.count(c)) enCount++;
            if (g_toEnMap.count(c)) ruCount++;
        }
        outWasEnglish = (enCount >= ruCount);
    }

    std::wstring result = input;
    for (size_t i = 0; i < result.length(); ++i) {
        wchar_t c = result[i];
        if (outWasEnglish) {
            if (g_toRuMap.count(c)) result[i] = g_toRuMap[c];
        } else {
            if (g_toEnMap.count(c)) result[i] = g_toEnMap[c];
        }
    }
    return result;
}

DWORD WINAPI LayoutWorker(LPVOID) {
    std::wstring backupClipboard = GetClipboardText();

    if (!ForceCopy()) {
        return 0;
    }

    std::wstring originalText = GetClipboardText();
    if (originalText.empty()) return 0;

    bool wasEnglish = false;
    std::wstring convertedText = ConvertLayout(originalText, wasEnglish);
    if (convertedText == originalText) {
        if (!backupClipboard.empty()) SetClipboardText(backupClipboard);
        return 0;
    }

    SetClipboardText(convertedText);
    Sleep(40);
    ForcePaste();

    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        DWORD targetThread = GetWindowThreadProcessId(hwnd, NULL);
        DWORD curThread = GetCurrentThreadId();
        HKL targetLayout = wasEnglish ? LoadKeyboardLayoutW(L"00000419", KLF_ACTIVATE)
                                      : LoadKeyboardLayoutW(L"00000409", KLF_ACTIVATE);
        if (targetThread && targetThread != curThread) {
            AttachThreadInput(curThread, targetThread, TRUE);
            ActivateKeyboardLayout(targetLayout, KLF_SETFORPROCESS);
            PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)targetLayout);
            AttachThreadInput(curThread, targetThread, FALSE);
        } else {
            ActivateKeyboardLayout(targetLayout, KLF_SETFORPROCESS);
            PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)targetLayout);
        }
    }

    Sleep(250);
    if (!backupClipboard.empty() && !g_bStopRequested) {
        SetClipboardText(backupClipboard);
    }
    return 0;
}

void StartWorker(LPTHREAD_START_ROUTINE pfn) {
    if (g_bStopRequested) return;

    if (g_hWorkerThread) {
        if (WaitForSingleObject(g_hWorkerThread, 0) == WAIT_TIMEOUT) {
            return;
        }
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = NULL;
    }
    g_hWorkerThread = CreateThread(NULL, 0, pfn, NULL, 0, NULL);
}

void UpdateDpiFont(HWND hwnd) {
    UINT dpi = 96;
    if (hwnd) {
        dpi = GetDpiForWindow(hwnd);
    }
    if (dpi == 0) dpi = 96;

    if (g_hTooltipFont && g_tooltipDpi == dpi) return;
    g_tooltipDpi = dpi;

    if (g_hTooltipFont) {
        DeleteObject(g_hTooltipFont);
        g_hTooltipFont = NULL;
    }

    int fontHeight = MulDiv(-16, (int)g_tooltipDpi, 96);
    g_hTooltipFont = CreateFontW(fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

void TriggerFadeOut(HWND hwnd) {
    if (g_animState == STATE_HIDDEN || g_animState == STATE_FADE_OUT) return;

    if (g_animDuration <= 0) {
        ShowWindow(hwnd, SW_HIDE);
        g_animState = STATE_HIDDEN;
        return;
    }

    g_animState = STATE_FADE_OUT;
    g_animStartTime = GetTickCount();
    SetTimer(hwnd, TIMER_ANIM, 16, NULL);
}

LRESULT CALLBACK TooltipWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SHOW_TOOLTIP: {
            std::unique_ptr<std::wstring> pIncomingText((std::wstring*)lParam);
            if (pIncomingText) {
                g_tooltipText = std::move(*pIncomingText);
            }

            UpdateDpiFont(hwnd);

            HDC hdc = GetDC(hwnd);
            SelectObject(hdc, g_hTooltipFont);
            int maxTextWidth = MulDiv(440, (int)g_tooltipDpi, 96);
            RECT rc = { 0, 0, maxTextWidth, 0 };
            DrawTextW(hdc, g_tooltipText.c_str(), -1, &rc, DT_CALCRECT | DT_WORDBREAK);
            ReleaseDC(hwnd, hdc);

            int padX = MulDiv(14, (int)g_tooltipDpi, 96);
            int padY = MulDiv(12, (int)g_tooltipDpi, 96);
            int minW = MulDiv(140, (int)g_tooltipDpi, 96);

            g_winW = (rc.right - rc.left) + padX * 2;
            g_winH = (rc.bottom - rc.top) + padY * 2;
            if (g_winW < minW) g_winW = minW;

            POINT pt;
            GetCursorPos(&pt);
            g_targetX = pt.x + MulDiv(14, (int)g_tooltipDpi, 96);
            g_targetY = pt.y + MulDiv(18, (int)g_tooltipDpi, 96);

            HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfoW(hMon, &mi)) {
                if (g_targetX + g_winW > mi.rcWork.right)  g_targetX = mi.rcWork.right - g_winW - 8;
                if (g_targetY + g_winH > mi.rcWork.bottom) g_targetY = pt.y - g_winH - 8;
                if (g_targetX < mi.rcWork.left) g_targetX = mi.rcWork.left + 8;
                if (g_targetY < mi.rcWork.top)  g_targetY = mi.rcWork.top + 8;
            }

            KillTimer(hwnd, TIMER_ANIM);

            if (g_animDuration <= 0) {
                SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                SetWindowPos(hwnd, HWND_TOPMOST, g_targetX, g_targetY, g_winW, g_winH, SWP_SHOWWINDOW | SWP_NOACTIVATE);
                InvalidateRect(hwnd, NULL, TRUE);
                g_animState = STATE_VISIBLE;
            } else {
                SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                int offsetAnim = MulDiv(8, (int)g_tooltipDpi, 96);
                SetWindowPos(hwnd, HWND_TOPMOST, g_targetX, g_targetY + offsetAnim, g_winW, g_winH, SWP_SHOWWINDOW | SWP_NOACTIVATE);
                InvalidateRect(hwnd, NULL, TRUE);
                g_animState = STATE_FADE_IN;
                g_animStartTime = GetTickCount();
                SetTimer(hwnd, TIMER_ANIM, 16, NULL);
            }
            return 0;
        }

        case WM_HIDE_TOOLTIP: {
            TriggerFadeOut(hwnd);
            return 0;
        }

        case WM_TIMER: {
            if (wParam == TIMER_ANIM) {
                DWORD now = GetTickCount();
                DWORD elapsed = now - g_animStartTime;
                float t = (float)elapsed / (float)(g_animDuration > 0 ? g_animDuration : 1);
                if (t > 1.0f) t = 1.0f;

                if (g_animState == STATE_FADE_IN) {
                    float ease = t * (2.0f - t);
                    BYTE alpha = (BYTE)(255 * ease);
                    int offsetAnim = MulDiv(8, (int)g_tooltipDpi, 96);
                    int curY = g_targetY + (int)(offsetAnim * (1.0f - ease));

                    SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
                    SetWindowPos(hwnd, NULL, g_targetX, curY, g_winW, g_winH, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

                    if (t >= 1.0f) {
                        KillTimer(hwnd, TIMER_ANIM);
                        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                        SetWindowPos(hwnd, NULL, g_targetX, g_targetY, g_winW, g_winH, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                        g_animState = STATE_VISIBLE;
                    }
                } else if (g_animState == STATE_FADE_OUT) {
                    BYTE alpha = (BYTE)(255 * (1.0f - t));
                    SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);

                    if (t >= 1.0f) {
                        KillTimer(hwnd, TIMER_ANIM);
                        ShowWindow(hwnd, SW_HIDE);
                        g_animState = STATE_HIDDEN;
                    }
                }
                return 0;
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rcClient;
            GetClientRect(hwnd, &rcClient);

            HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 34));
            FillRect(hdc, &rcClient, bgBrush);
            DeleteObject(bgBrush);

            HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(62, 62, 72));
            HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
            SelectObject(hdc, oldBrush);
            SelectObject(hdc, oldPen);
            DeleteObject(borderPen);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(240, 240, 245));
            SelectObject(hdc, g_hTooltipFont);

            int padX = MulDiv(14, (int)g_tooltipDpi, 96);
            int padY = MulDiv(12, (int)g_tooltipDpi, 96);

            RECT rcText = rcClient;
            rcText.left += padX;
            rcText.top += padY;
            rcText.right -= padX;
            rcText.bottom -= padY;

            DrawTextW(hdc, g_tooltipText.c_str(), -1, &rcText, DT_WORDBREAK | DT_LEFT);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            TriggerFadeOut(hwnd);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

bool MatchHotkey(const HotkeyConfig& cfg, DWORD vkCode, bool ctrlDown, bool shiftDown, bool altDown) {
    if (cfg.vk == 0) return false;
    return (vkCode == cfg.vk &&
            ctrlDown == cfg.ctrl &&
            shiftDown == cfg.shift &&
            altDown == cfg.alt);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

        if (pKey->flags & LLKHF_INJECTED) {
            return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
        }

        if (g_hTooltipWnd && g_animState != STATE_HIDDEN) {
            if (pKey->vkCode == VK_ESCAPE || pKey->vkCode == VK_LEFT ||
                pKey->vkCode == VK_RIGHT || pKey->vkCode == VK_UP || pKey->vkCode == VK_DOWN) {
                PostMessage(g_hTooltipWnd, WM_HIDE_TOOLTIP, 0, 0);
            }
        }

        bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool ctrlDown  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool altDown   = ((pKey->flags & LLKHF_ALTDOWN) != 0) || ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);

        if (g_bEnableTranslation && MatchHotkey(g_hkTooltip, pKey->vkCode, ctrlDown, shiftDown, altDown)) {
            StartWorker(TooltipWorker);
            return 1;
        }

        if (g_bEnableTranslation && MatchHotkey(g_hkTranslate, pKey->vkCode, ctrlDown, shiftDown, altDown)) {
            StartWorker(TranslationWorker);
            return 1;
        }

        if (MatchHotkey(g_hkLayout, pKey->vkCode, ctrlDown, shiftDown, altDown)) {
            StartWorker(LayoutWorker);
            return 1;
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (g_hTooltipWnd && g_animState != STATE_HIDDEN) {
            if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || 
                wParam == WM_MBUTTONDOWN || wParam == WM_MOUSEWHEEL) {
                MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
                RECT rc;
                GetWindowRect(g_hTooltipWnd, &rc);
                if (wParam == WM_MOUSEWHEEL || !PtInRect(&rc, pMouse->pt)) {
                    PostMessage(g_hTooltipWnd, WM_HIDE_TOOLTIP, 0, 0);
                }
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

DWORD WINAPI MainThread(LPVOID) {
    HINSTANCE hInst = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&TooltipWndProc, &hInst);

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = TooltipWndProc;
    wc.hInstance = hInst ? hInst : GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkTranslatorTooltip";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);

    g_hTooltipWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        wc.lpszClassName, L"",
        WS_POPUP,
        0, 0, 0, 0,
        NULL, NULL, wc.hInstance, NULL
    );

    UpdateDpiFont(g_hTooltipWnd);

    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, wc.hInstance, 0);
    g_mouseHook    = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, wc.hInstance, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_APP_SETTINGS_CHANGED) {
            LoadSettings();
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = NULL;
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = NULL;
    }

    if (g_hTooltipWnd) {
        DestroyWindow(g_hTooltipWnd);
        g_hTooltipWnd = NULL;
    }
    if (g_hTooltipFont) {
        DeleteObject(g_hTooltipFont);
        g_hTooltipFont = NULL;
    }
    UnregisterClassW(L"WindhawkTranslatorTooltip", wc.hInstance);

    return 0;
}

BOOL WhTool_ModInit() {
    InitMaps();
    LoadSettings();
    g_bStopRequested = false;
    g_hMainThread = CreateThread(NULL, 0, MainThread, NULL, 0, &g_mainThreadId);
    return g_hMainThread != NULL;
}

void WhTool_ModSettingsChanged() {
    if (g_mainThreadId) {
        PostThreadMessage(g_mainThreadId, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_bStopRequested = true;

    if (g_mainThreadId) {
        PostThreadMessage(g_mainThreadId, WM_QUIT, 0, 0);
        if (g_hMainThread) {
            WaitForSingleObject(g_hMainThread, 1500);
            CloseHandle(g_hMainThread);
            g_hMainThread = NULL;
        }
    }

    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, 1500);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        return;
    }

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
