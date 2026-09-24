// ==WindhawkMod==
// @id              quick-translator-layout-switcher
// @name            Quick Translator & Layout Switcher
// @description     Fix mistyped keyboard layout, translate text in-place, or view translations in a floating HUD tooltip.
// @version         1.0
// @author          zed712969-crypto
// @github          https://github.com/zed712969-crypto
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Translator & Layout Switcher

A lightweight utility for Windows that bridges communication between English and Russian speakers. Eliminates mistyped keyboard layout errors and allows instant two-way translation without breaking workflow.

## Features

- **Layout Correction (`Shift + ~`)**: Fixes mistyped text (`ghbdtn` -> `привет` or vice versa) with full ANSI punctuation mapping and automatically updates the active system input layout.
- **In-Place Translation (`Ctrl + ~`)**: Replaces selected text directly inside any editable input field (chats, editors, forms) using background HTTPS POST requests. Supports multi-line paragraphs up to 4000+ characters.
- **Floating HUD Tooltip (`Alt + ~`)**: Translates read-only incoming messages (Discord, Telegram, web pages, games) in a sleek dark tooltip positioned near your cursor. Stays on screen until explicitly dismissed.
- **Manual Dismiss On Interaction**: The tooltip remains visible until you click anywhere, scroll the mouse wheel, or press navigation keys (Esc / Arrows).
- **Fully Customizable**: Change any hotkey or adjust animation speed anytime in the Windhawk Settings tab without recompiling.

## Advantages Over Standalone Translators

- **Zero Overhead**: Injected into `explorer.exe` using native `winhttp.dll` and `gdi32.dll`. No heavy Electron apps, background browsers, or RAM hogs.
- **No Focus Loss**: The HUD tooltip uses non-activating windows (`WS_EX_NOACTIVATE`), preserving active text caret and window state.
- **No Window Switching**: Never Alt+Tab away from your game or conversation just to translate text.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey_layout: "Shift + ~"
  $name: "Layout Correction"
  $description: "Switch mistyped layout (default Shift + ~)"
- hotkey_translate: "Ctrl + ~"
  $name: "In-Place Translate"
  $description: "Translate selected text inline (default Ctrl + ~)"
- hotkey_tooltip: "Alt + ~"
  $name: "HUD Tooltip Translate"
  $description: "Translate selected text in a floating tooltip (default Alt + ~)"
- anim_duration: 150
  $name: "Animation Duration (ms)"
  $description: "Fade animation speed in ms (0 to disable)"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cwctype>
#include <cstdlib>

#define WM_SHOW_TOOLTIP (WM_USER + 101)
#define WM_HIDE_TOOLTIP (WM_USER + 102)

#define TIMER_ANIM 2

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif

#ifndef LWA_ALPHA
#define LWA_ALPHA 0x00000002
#endif

typedef LPVOID HINTERNET;
typedef HINTERNET(WINAPI* pfn_WinHttpOpen)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
typedef HINTERNET(WINAPI* pfn_WinHttpConnect)(HINTERNET, LPCWSTR, WORD, DWORD);
typedef HINTERNET(WINAPI* pfn_WinHttpOpenRequest)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);
typedef BOOL(WINAPI* pfn_WinHttpSendRequest)(HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
typedef BOOL(WINAPI* pfn_WinHttpReceiveResponse)(HINTERNET, LPVOID);
typedef BOOL(WINAPI* pfn_WinHttpReadData)(HINTERNET, LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI* pfn_WinHttpCloseHandle)(HINTERNET);

typedef BOOL(WINAPI* pfn_SetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
typedef HGDIOBJ(WINAPI* pfn_SelectObject)(HDC, HGDIOBJ);
typedef HBRUSH(WINAPI* pfn_CreateSolidBrush)(COLORREF);
typedef BOOL(WINAPI* pfn_DeleteObject)(HGDIOBJ);
typedef HPEN(WINAPI* pfn_CreatePen)(int, int, COLORREF);
typedef HGDIOBJ(WINAPI* pfn_GetStockObject)(int);
typedef BOOL(WINAPI* pfn_Rectangle)(HDC, int, int, int, int);
typedef int(WINAPI* pfn_SetBkMode)(HDC, int);
typedef COLORREF(WINAPI* pfn_SetTextColor)(HDC, COLORREF);
typedef HFONT(WINAPI* pfn_CreateFontW)(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCWSTR);

pfn_SetLayeredWindowAttributes g_pSetLayeredWindowAttributes = NULL;
pfn_SelectObject     g_pSelectObject = NULL;
pfn_CreateSolidBrush g_pCreateSolidBrush = NULL;
pfn_DeleteObject     g_pDeleteObject = NULL;
pfn_CreatePen        g_pCreatePen = NULL;
pfn_GetStockObject   g_pGetStockObject = NULL;
pfn_Rectangle        g_pRectangle = NULL;
pfn_SetBkMode        g_pSetBkMode = NULL;
pfn_SetTextColor     g_pSetTextColor = NULL;
pfn_CreateFontW      g_pCreateFontW = NULL;

HHOOK g_keyboardHook = NULL;
HHOOK g_mouseHook = NULL;
HANDLE g_hMainThread = NULL;
DWORD g_mainThreadId = 0;

HWND g_hTooltipWnd = NULL;
HFONT g_hTooltipFont = NULL;
std::wstring g_tooltipText = L"";

std::unordered_map<wchar_t, wchar_t> g_toRuMap;
std::unordered_map<wchar_t, wchar_t> g_toEnMap;

enum AnimState { STATE_HIDDEN, STATE_FADE_IN, STATE_VISIBLE, STATE_FADE_OUT };
AnimState g_animState = STATE_HIDDEN;
DWORD g_animStartTime = 0;
int g_targetX = 0, g_targetY = 0, g_winW = 0, g_winH = 0;
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

void InitLibraries() {
    HMODULE hUser = GetModuleHandleA("user32.dll");
    if (hUser) {
        g_pSetLayeredWindowAttributes = (pfn_SetLayeredWindowAttributes)GetProcAddress(hUser, "SetLayeredWindowAttributes");
    }

    HMODULE hGdi = LoadLibraryA("gdi32.dll");
    if (!hGdi) return;

    g_pSelectObject     = (pfn_SelectObject)GetProcAddress(hGdi, "SelectObject");
    g_pCreateSolidBrush = (pfn_CreateSolidBrush)GetProcAddress(hGdi, "CreateSolidBrush");
    g_pDeleteObject     = (pfn_DeleteObject)GetProcAddress(hGdi, "DeleteObject");
    g_pCreatePen        = (pfn_CreatePen)GetProcAddress(hGdi, "CreatePen");
    g_pGetStockObject   = (pfn_GetStockObject)GetProcAddress(hGdi, "GetStockObject");
    g_pRectangle        = (pfn_Rectangle)GetProcAddress(hGdi, "Rectangle");
    g_pSetBkMode        = (pfn_SetBkMode)GetProcAddress(hGdi, "SetBkMode");
    g_pSetTextColor     = (pfn_SetTextColor)GetProcAddress(hGdi, "SetTextColor");
    g_pCreateFontW      = (pfn_CreateFontW)GetProcAddress(hGdi, "CreateFontW");
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
            } else if (lower == L"~" || lower == L"`" || lower == L"tilde") {
                cfg.vk = VK_OEM_3;
            } else if (lower.length() >= 2 && lower[0] == L'f' && iswdigit(lower[1])) {
                int fnum = _wtoi(lower.c_str() + 1);
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
    PCWSTR sLayout = Wh_GetStringSetting(L"hotkey_layout");
    ParseHotkey(sLayout ? sLayout : L"", g_hkLayout, false, true, false, VK_OEM_3);
    if (sLayout) Wh_FreeStringSetting(sLayout);

    PCWSTR sTranslate = Wh_GetStringSetting(L"hotkey_translate");
    ParseHotkey(sTranslate ? sTranslate : L"", g_hkTranslate, true, false, false, VK_OEM_3);
    if (sTranslate) Wh_FreeStringSetting(sTranslate);

    PCWSTR sTooltip = Wh_GetStringSetting(L"hotkey_tooltip");
    ParseHotkey(sTooltip ? sTooltip : L"", g_hkTooltip, false, false, true, VK_OEM_3);
    if (sTooltip) Wh_FreeStringSetting(sTooltip);

    g_animDuration = Wh_GetIntSetting(L"anim_duration");
    if (g_animDuration < 0) g_animDuration = 150;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void SendKey(WORD vk, bool keyUp) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    if (keyUp) input.ki.dwFlags |= KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void WaitForModifiersUp() {
    for (int i = 0; i < 25; ++i) {
        bool ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool alt   = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        if (!ctrl && !shift && !alt) break;
        Sleep(15);
    }
}

void ForceCopy() {
    WaitForModifiersUp();
    SendKey(VK_CONTROL, true);
    SendKey(VK_SHIFT, true);
    SendKey(VK_MENU, true);
    Sleep(25);

    SendKey(VK_CONTROL, false);
    SendKey('C', false);
    SendKey('C', true);
    SendKey(VK_CONTROL, true);
}

void ForcePaste() {
    WaitForModifiersUp();
    SendKey(VK_CONTROL, true);
    SendKey(VK_SHIFT, true);
    SendKey(VK_MENU, true);
    Sleep(25);

    SendKey(VK_CONTROL, false);
    SendKey('V', false);
    SendKey('V', true);
    SendKey(VK_CONTROL, true);
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
        Sleep(25);
    }
    return text;
}

void SetClipboardText(const std::wstring& text) {
    for (int i = 0; i < 12; ++i) {
        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, (text.length() + 1) * sizeof(wchar_t));
            if (hGlob) {
                memcpy(GlobalLock(hGlob), text.c_str(), (text.length() + 1) * sizeof(wchar_t));
                GlobalUnlock(hGlob);
                SetClipboardData(CF_UNICODETEXT, hGlob);
            }
            CloseClipboard();
            break;
        }
        Sleep(25);
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
    HMODULE hWinHttp = LoadLibraryA("winhttp.dll");
    if (!hWinHttp) return L"";

    auto fnWinHttpOpen = (pfn_WinHttpOpen)GetProcAddress(hWinHttp, "WinHttpOpen");
    auto fnWinHttpConnect = (pfn_WinHttpConnect)GetProcAddress(hWinHttp, "WinHttpConnect");
    auto fnWinHttpOpenRequest = (pfn_WinHttpOpenRequest)GetProcAddress(hWinHttp, "WinHttpOpenRequest");
    auto fnWinHttpSendRequest = (pfn_WinHttpSendRequest)GetProcAddress(hWinHttp, "WinHttpSendRequest");
    auto fnWinHttpReceiveResponse = (pfn_WinHttpReceiveResponse)GetProcAddress(hWinHttp, "WinHttpReceiveResponse");
    auto fnWinHttpReadData = (pfn_WinHttpReadData)GetProcAddress(hWinHttp, "WinHttpReadData");
    auto fnWinHttpCloseHandle = (pfn_WinHttpCloseHandle)GetProcAddress(hWinHttp, "WinHttpCloseHandle");

    if (!fnWinHttpOpen || !fnWinHttpConnect || !fnWinHttpOpenRequest ||
        !fnWinHttpSendRequest || !fnWinHttpReceiveResponse || !fnWinHttpReadData || !fnWinHttpCloseHandle) {
        FreeLibrary(hWinHttp);
        return L"";
    }

    HINTERNET hSession = fnWinHttpOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64)", 0, NULL, NULL, 0);
    if (!hSession) {
        FreeLibrary(hWinHttp);
        return L"";
    }

    HINTERNET hConnect = fnWinHttpConnect(hSession, L"translate.googleapis.com", 443, 0);
    if (!hConnect) {
        fnWinHttpCloseHandle(hSession);
        FreeLibrary(hWinHttp);
        return L"";
    }

    std::wstring path = L"/translate_a/single?client=gtx&sl=auto&tl=" +
                        std::wstring(toRussian ? L"ru" : L"en") +
                        L"&dt=t&dj=1";

    HINTERNET hRequest = fnWinHttpOpenRequest(hConnect, L"POST", path.c_str(), NULL, NULL, NULL, 0x00800000);
    if (!hRequest) {
        fnWinHttpCloseHandle(hConnect);
        fnWinHttpCloseHandle(hSession);
        FreeLibrary(hWinHttp);
        return L"";
    }

    std::string postBody = "q=" + UrlEncodeUtf8(text);
    std::wstring headers = L"Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n";

    std::string response;
    if (fnWinHttpSendRequest(hRequest, headers.c_str(), (DWORD)headers.length(), 
                            (LPVOID)postBody.c_str(), (DWORD)postBody.length(), (DWORD)postBody.length(), 0) &&
        fnWinHttpReceiveResponse(hRequest, NULL)) {
        
        char buffer[4096];
        DWORD bytesRead = 0;
        while (fnWinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response.append(buffer, bytesRead);
        }
    }

    fnWinHttpCloseHandle(hRequest);
    fnWinHttpCloseHandle(hConnect);
    fnWinHttpCloseHandle(hSession);
    FreeLibrary(hWinHttp);

    return ParseGoogleTranslateResponse(response);
}

DWORD WINAPI TranslationWorker(LPVOID) {
    ForceCopy();
    Sleep(90);

    std::wstring originalText = GetClipboardText();
    if (originalText.empty()) {
        MessageBeep(MB_ICONHAND);
        return 0;
    }

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

    std::wstring translated = FetchTranslationWinHttp(originalText, toRussian);
    if (translated.empty()) {
        MessageBeep(MB_ICONHAND);
        return 0;
    }

    SetClipboardText(translated);
    Sleep(50);

    ForcePaste();
    return 0;
}

DWORD WINAPI TooltipWorker(LPVOID) {
    ForceCopy();
    Sleep(90);

    std::wstring originalText = GetClipboardText();
    if (originalText.empty()) {
        MessageBeep(MB_ICONHAND);
        return 0;
    }

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

    std::wstring translated = FetchTranslationWinHttp(originalText, toRussian);
    if (translated.empty()) {
        MessageBeep(MB_ICONHAND);
        return 0;
    }

    g_tooltipText = translated;
    if (g_hTooltipWnd) {
        PostMessage(g_hTooltipWnd, WM_SHOW_TOOLTIP, 0, 0);
    }
    return 0;
}

std::wstring ConvertLayout(const std::wstring& input, bool& outWasEnglish) {
    size_t enCount = 0, ruCount = 0;
    for (wchar_t c : input) {
        if (g_toRuMap.count(c)) enCount++;
        if (g_toEnMap.count(c)) ruCount++;
    }

    outWasEnglish = (enCount >= ruCount);
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
    ForceCopy();
    Sleep(80);

    std::wstring originalText = GetClipboardText();
    if (originalText.empty()) return 0;

    bool wasEnglish = false;
    std::wstring convertedText = ConvertLayout(originalText, wasEnglish);
    if (convertedText == originalText) return 0;

    SetClipboardText(convertedText);
    Sleep(40);

    ForcePaste();

    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        HKL targetLayout = wasEnglish ? LoadKeyboardLayoutW(L"00000419", KLF_ACTIVATE)
                                      : LoadKeyboardLayoutW(L"00000409", KLF_ACTIVATE);
        PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)targetLayout);
    }
    return 0;
}

void SetWindowAlpha(HWND hwnd, BYTE alpha) {
    if (g_pSetLayeredWindowAttributes) {
        g_pSetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
    }
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
            HDC hdc = GetDC(hwnd);
            if (g_pSelectObject && g_hTooltipFont) {
                g_pSelectObject(hdc, g_hTooltipFont);
            }
            RECT rc = { 0, 0, 440, 0 };
            DrawTextW(hdc, g_tooltipText.c_str(), -1, &rc, DT_CALCRECT | DT_WORDBREAK);
            ReleaseDC(hwnd, hdc);

            int padX = 14, padY = 12;
            g_winW = (rc.right - rc.left) + padX * 2;
            g_winH = (rc.bottom - rc.top) + padY * 2;
            if (g_winW < 140) g_winW = 140;

            POINT pt;
            GetCursorPos(&pt);
            g_targetX = pt.x + 14;
            g_targetY = pt.y + 18;

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
                SetWindowAlpha(hwnd, 255);
                SetWindowPos(hwnd, HWND_TOPMOST, g_targetX, g_targetY, g_winW, g_winH, SWP_SHOWWINDOW | SWP_NOACTIVATE);
                InvalidateRect(hwnd, NULL, TRUE);
                g_animState = STATE_VISIBLE;
            } else {
                SetWindowAlpha(hwnd, 0);
                SetWindowPos(hwnd, HWND_TOPMOST, g_targetX, g_targetY + 8, g_winW, g_winH, SWP_SHOWWINDOW | SWP_NOACTIVATE);
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
                    int curY = g_targetY + (int)(8 * (1.0f - ease));

                    SetWindowAlpha(hwnd, alpha);
                    SetWindowPos(hwnd, NULL, g_targetX, curY, g_winW, g_winH, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

                    if (t >= 1.0f) {
                        KillTimer(hwnd, TIMER_ANIM);
                        SetWindowAlpha(hwnd, 255);
                        SetWindowPos(hwnd, NULL, g_targetX, g_targetY, g_winW, g_winH, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                        g_animState = STATE_VISIBLE;
                    }
                } else if (g_animState == STATE_FADE_OUT) {
                    BYTE alpha = (BYTE)(255 * (1.0f - t));
                    SetWindowAlpha(hwnd, alpha);

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

            if (g_pCreateSolidBrush && g_pDeleteObject) {
                HBRUSH bgBrush = g_pCreateSolidBrush(RGB(30, 30, 34));
                FillRect(hdc, &rcClient, bgBrush);
                g_pDeleteObject(bgBrush);
            }

            if (g_pCreatePen && g_pSelectObject && g_pGetStockObject && g_pRectangle && g_pDeleteObject) {
                HPEN borderPen = g_pCreatePen(PS_SOLID, 1, RGB(62, 62, 72));
                HPEN oldPen = (HPEN)g_pSelectObject(hdc, borderPen);
                HBRUSH oldBrush = (HBRUSH)g_pSelectObject(hdc, g_pGetStockObject(NULL_BRUSH));
                g_pRectangle(hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
                g_pSelectObject(hdc, oldBrush);
                g_pSelectObject(hdc, oldPen);
                g_pDeleteObject(borderPen);
            }

            if (g_pSetBkMode) g_pSetBkMode(hdc, TRANSPARENT);
            if (g_pSetTextColor) g_pSetTextColor(hdc, RGB(240, 240, 245));
            if (g_pSelectObject && g_hTooltipFont) g_pSelectObject(hdc, g_hTooltipFont);

            RECT rcText = rcClient;
            rcText.left += 14;
            rcText.top += 12;
            rcText.right -= 14;
            rcText.bottom -= 12;

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

        if (g_hTooltipWnd && g_animState != STATE_HIDDEN) {
            if (pKey->vkCode == VK_ESCAPE || pKey->vkCode == VK_LEFT ||
                pKey->vkCode == VK_RIGHT || pKey->vkCode == VK_UP || pKey->vkCode == VK_DOWN) {
                PostMessage(g_hTooltipWnd, WM_HIDE_TOOLTIP, 0, 0);
            }
        }

        bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool ctrlDown  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool altDown   = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

        if (MatchHotkey(g_hkTooltip, pKey->vkCode, ctrlDown, shiftDown, altDown)) {
            HANDLE h = CreateThread(NULL, 0, TooltipWorker, NULL, 0, NULL);
            if (h) CloseHandle(h);
            return 1;
        }

        if (MatchHotkey(g_hkTranslate, pKey->vkCode, ctrlDown, shiftDown, altDown)) {
            HANDLE h = CreateThread(NULL, 0, TranslationWorker, NULL, 0, NULL);
            if (h) CloseHandle(h);
            return 1;
        }

        if (MatchHotkey(g_hkLayout, pKey->vkCode, ctrlDown, shiftDown, altDown)) {
            HANDLE h = CreateThread(NULL, 0, LayoutWorker, NULL, 0, NULL);
            if (h) CloseHandle(h);
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
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = TooltipWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkTranslatorTooltip";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);

    if (g_pCreateFontW) {
        g_hTooltipFont = g_pCreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    }

    g_hTooltipWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        wc.lpszClassName, L"",
        WS_POPUP,
        0, 0, 0, 0,
        NULL, NULL, wc.hInstance, NULL
    );

    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    g_mouseHook    = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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
    if (g_hTooltipFont && g_pDeleteObject) {
        g_pDeleteObject(g_hTooltipFont);
        g_hTooltipFont = NULL;
    }
    UnregisterClassW(L"WindhawkTranslatorTooltip", GetModuleHandle(NULL));

    return 0;
}

BOOL Wh_ModInit() {
    InitMaps();
    InitLibraries();
    LoadSettings();
    g_hMainThread = CreateThread(NULL, 0, MainThread, NULL, 0, &g_mainThreadId);
    return g_hMainThread != NULL;
}

void Wh_ModUninit() {
    if (g_mainThreadId) {
        PostThreadMessage(g_mainThreadId, WM_QUIT, 0, 0);
        if (g_hMainThread) {
            WaitForSingleObject(g_hMainThread, 1000);
            CloseHandle(g_hMainThread);
            g_hMainThread = NULL;
        }
    }
}
