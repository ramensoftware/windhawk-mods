// ==WindhawkMod==
// @id           gemini-quick-assist
// @name         Gemini AI Assistant
// @description  Press Ctrl+Shift+G to send selected text to Gemini AI.
// @version      1.4.0
// @author       Coeus Institute
// @github          https://github.com/CoeusInstitute
// @twitter         https://x.com/CoeusInstitute
// @homepage        https://coeus.institute
// @include      explorer.exe
// @compilerOptions -lwinhttp -luser32 -lkernel32 -lgdi32 -ldwmapi -luxtheme -lcomctl32 -std=c++20
// @license MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Gemini AI Assistant

This mod integrates Google's Gemini AI directly into your Windows workflow.

## Features

- **Modern UI:** Uses a custom dark-mode window with Rich Text formatting.
- **Loading Indicator:** Animated overlay shows API activity so you always know it's working.
- **Smart Parsing:** Renders headers, bold text, italics, lists, and code blocks beautifully.
- **Auto-Replace:** "Fix Grammar" mode instantly replaces text in your document.
- **Resizable Window:** Drag to resize the result window for longer responses.
- **Keyboard Shortcuts:** Escape to close, Ctrl+A to select all in the result window.

## Configuration

- **Required:** Get an API key from [Google AI Studio](https://aistudio.google.com/) and paste it into the Settings tab.
- **Best Practice:** In the "Advanced" tab of the Mod, scroll to the bottom and turn on "Consider inclusion list patterns for critical system processes"

## Changelog (v1.4.0)
- Added animated loading indicator at cursor position
- Result window is now resizable with enforced minimum size
- Added Escape key to close and Ctrl+A to select all
- Improved markdown rendering: numbered lists, italic text, nested bold/italic
- Better code block rendering with distinct background color via indentation
- Improved error messages and timeout handling
- Visual refinements and spacing improvements
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ApiKey: ""
  $name: Gemini API Key
  $description: Your API Key from aistudio.google.com (Required)
- ModelName: "gemini-flash-latest"
  $name: Model Name
  $description: The model version to use.
- SystemPrompt: "You are a helpful AI assistant integrated into Windows 11. The user is selecting text from applications on their local workstation. Provide concise, accurate responses. Format responses with markdown for clarity."
  $name: System Prompt
  $description: Context and instructions sent with every request to guide the AI's behavior.
- CustomPrompts: "Translate the following text to Esperanto; Generate a witty response to this text"
  $name: Custom Prompts
  $description: Add your own menu options here, separated by semicolons (;).
*/
// ==/WindhawkModSettings==

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <winhttp.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <richedit.h>

// ---------------------------------------------------------------------------
// Globals & Constants
// ---------------------------------------------------------------------------
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;
HWND g_hHelperWindow = NULL;
HMODULE g_hRichEditMod = NULL;

// GUI Resources
HFONT g_hFontTitle = NULL;
HFONT g_hFontSubtitle = NULL;
HFONT g_hFontBody = NULL;
HFONT g_hFontButton = NULL;
HBRUSH g_hBrushBg = NULL;
HBRUSH g_hBrushEditBg = NULL;
HBRUSH g_hBrushButtonNormal = NULL;
HBRUSH g_hBrushButtonHover = NULL;
HBRUSH g_hBrushAccent = NULL;

// Loading Indicator
HWND g_hLoadingWindow = NULL;
int g_loadingFrame = 0;
const int LOADING_TIMER_ID = 42;
const int LOADING_WINDOW_SIZE = 64;

// UI Control IDs
#define ID_RICH_TEXT     101
#define ID_BTN_COPY      102
#define ID_BTN_CLOSE     103
#define ID_STATIC_TITLE  104
#define ID_STATIC_ACTION 105

// Modern Dark Theme Colors
namespace Colors {
    const COLORREF Background      = RGB(24, 24, 28);
    const COLORREF SurfaceLight    = RGB(44, 45, 52);
    const COLORREF Border          = RGB(58, 60, 68);
    const COLORREF TextPrimary     = RGB(235, 235, 240);
    const COLORREF TextSecondary   = RGB(160, 162, 170);
    const COLORREF TextHeader      = RGB(130, 170, 255);
    const COLORREF TextCode        = RGB(206, 145, 120);
    const COLORREF TextItalic      = RGB(200, 200, 210);
    const COLORREF Accent          = RGB(130, 170, 255);
    const COLORREF AccentHover     = RGB(160, 190, 255);
    const COLORREF ButtonBg        = RGB(55, 58, 68);
    const COLORREF ButtonHover     = RGB(70, 73, 85);
    const COLORREF LoadingBg       = RGB(32, 33, 38);
    const COLORREF LoadingDot      = RGB(130, 170, 255);
    const COLORREF LoadingDotDim   = RGB(55, 65, 90);
    const COLORREF CodeBlockBg     = RGB(35, 36, 42);
}

// Window dimensions
const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 580;
const int WINDOW_MIN_WIDTH = 450;
const int WINDOW_MIN_HEIGHT = 350;
const int PADDING = 24;
const int BUTTON_HEIGHT = 42;
const int BUTTON_WIDTH = 160;
const int TITLE_HEIGHT = 32;
const int SUBTITLE_HEIGHT = 24;

std::wstring g_currentAction = L"Response";

// ---------------------------------------------------------------------------
// DWM Dark Mode Support
// ---------------------------------------------------------------------------
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

void EnableDarkTitleBar(HWND hwnd) {
    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
    
    #ifndef DWMWA_WINDOW_CORNER_PREFERENCE
    #define DWMWA_WINDOW_CORNER_PREFERENCE 33
    #endif
    int cornerPref = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));
}

// ---------------------------------------------------------------------------
// Helper: String Conversions
// ---------------------------------------------------------------------------
std::wstring ToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string ToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// ---------------------------------------------------------------------------
// Helper: JSON Processing
// ---------------------------------------------------------------------------
std::string EscapeJSON(const std::string& input) {
    std::ostringstream ss;
    for (unsigned char c : input) {
        switch (c) {
            case '"':  ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:
                if (c < 0x20) ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                else ss << c;
        }
    }
    return ss.str();
}

std::wstring ParseJSONResponse(const std::string& json) {
    std::string textKey = "\"text\": \"";
    size_t startPos = json.find(textKey);
    
    if (startPos == std::string::npos) {
        // Try alternate key format without space
        textKey = "\"text\":\"";
        startPos = json.find(textKey);
    }
    
    if (startPos == std::string::npos) {
        std::string errorKey = "\"message\": \"";
        size_t errPos = json.find(errorKey);
        if (errPos != std::string::npos) {
            startPos = errPos;
            textKey = errorKey;
        } else {
            return L"⚠ API Error (Unexpected Format):\n\n" + ToWString(json.substr(0, 500));
        }
    }
    
    startPos += textKey.length();
    std::string result;
    bool escaped = false;
    
    for (size_t i = startPos; i < json.length(); i++) {
        char c = json[i];
        if (escaped) {
            switch (c) {
                case 'n':  result += '\n'; break;
                case 't':  result += '\t'; break;
                case 'r':  break;
                case '"':  result += '"'; break;
                case '\\': result += '\\'; break;
                case '/':  result += '/'; break;
                default:   result += c; break;
            }
            escaped = false;
        } else {
            if (c == '\\') escaped = true;
            else if (c == '"') break;
            else result += c;
        }
    }
    return ToWString(result);
}

// ---------------------------------------------------------------------------
// Logic: Clipboard Operations
// ---------------------------------------------------------------------------
BOOL SetClipboardText(const std::wstring& text) {
    if (!OpenClipboard(NULL)) return FALSE;
    EmptyClipboard();
    size_t size = (text.length() + 1) * sizeof(wchar_t);
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hGlob) { CloseClipboard(); return FALSE; }
    memcpy(GlobalLock(hGlob), text.c_str(), size);
    GlobalUnlock(hGlob);
    SetClipboardData(CF_UNICODETEXT, hGlob);
    CloseClipboard();
    return TRUE;
}

std::wstring GetSelectedText() {
    if (OpenClipboard(NULL)) { EmptyClipboard(); CloseClipboard(); }
    
    std::vector<INPUT> inputs;
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    
    // Release modifier keys first
    input.ki.wVk = VK_SHIFT;   input.ki.dwFlags = KEYEVENTF_KEYUP; inputs.push_back(input);
    input.ki.wVk = VK_CONTROL; input.ki.dwFlags = KEYEVENTF_KEYUP; inputs.push_back(input);
    input.ki.wVk = VK_MENU;    input.ki.dwFlags = KEYEVENTF_KEYUP; inputs.push_back(input);
    
    // Send Ctrl+C
    input.ki.wVk = VK_CONTROL; input.ki.dwFlags = 0;               inputs.push_back(input);
    input.ki.wVk = 'C';        input.ki.dwFlags = 0;               inputs.push_back(input);
    input.ki.wVk = 'C';        input.ki.dwFlags = KEYEVENTF_KEYUP; inputs.push_back(input);
    input.ki.wVk = VK_CONTROL; input.ki.dwFlags = KEYEVENTF_KEYUP; inputs.push_back(input);
    
    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
    
    std::wstring result;
    for (int i = 0; i < 20; i++) {
        Sleep(40);
        if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
            if (OpenClipboard(NULL)) {
                HANDLE hData = GetClipboardData(CF_UNICODETEXT);
                if (hData) {
                    wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
                    if (pszText) { result = pszText; GlobalUnlock(hData); }
                }
                CloseClipboard();
            }
            if (!result.empty()) break;
        }
    }
    return result;
}

void SendPaste() {
    Sleep(50);
    std::vector<INPUT> inputs;
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VK_CONTROL; input.ki.dwFlags = 0;               inputs.push_back(input);
    input.ki.wVk = 'V';        input.ki.dwFlags = 0;               inputs.push_back(input);
    input.ki.wVk = 'V';        input.ki.dwFlags = KEYEVENTF_KEYUP; inputs.push_back(input);
    input.ki.wVk = VK_CONTROL; input.ki.dwFlags = KEYEVENTF_KEYUP; inputs.push_back(input);
    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

// ---------------------------------------------------------------------------
// Loading Indicator: Animated Spinner Window
// ---------------------------------------------------------------------------
LRESULT CALLBACK LoadingWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            EnableDarkTitleBar(hwnd);
            SetTimer(hwnd, LOADING_TIMER_ID, 120, NULL);
            g_loadingFrame = 0;
            return 0;
        }
        case WM_TIMER: {
            if (wParam == LOADING_TIMER_ID) {
                g_loadingFrame = (g_loadingFrame + 1) % 8;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            
            // Double buffer
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
            
            // Fill background with rounded rect appearance
            HBRUSH hBgBrush = CreateSolidBrush(Colors::LoadingBg);
            FillRect(hdcMem, &rc, hBgBrush);
            DeleteObject(hBgBrush);
            
            // Draw border
            HPEN hPen = CreatePen(PS_SOLID, 1, Colors::Border);
            HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPen);
            HBRUSH hNull = (HBRUSH)GetStockObject(NULL_BRUSH);
            SelectObject(hdcMem, hNull);
            RoundRect(hdcMem, 0, 0, rc.right, rc.bottom, 12, 12);
            SelectObject(hdcMem, hOldPen);
            DeleteObject(hPen);
            
            // Draw 3 bouncing dots
            int centerX = rc.right / 2;
            int centerY = rc.bottom / 2;
            int dotRadius = 5;
            int dotSpacing = 18;
            
            for (int i = 0; i < 3; i++) {
                // Each dot bounces with a phase offset
                int phase = (g_loadingFrame + (i * 2)) % 8;
                int yOffset = 0;
                
                // Simple bounce: 0,1,2,3 go up; 4,5,6,7 come back
                if (phase < 4) yOffset = -phase * 3;
                else yOffset = -(8 - phase) * 3;
                
                // Brighten the dot that's at peak
                bool isActive = (phase == 2 || phase == 3);
                COLORREF dotColor = isActive ? Colors::LoadingDot : Colors::LoadingDotDim;
                
                HBRUSH hDot = CreateSolidBrush(dotColor);
                SelectObject(hdcMem, hDot);
                SelectObject(hdcMem, (HPEN)GetStockObject(NULL_PEN));
                
                int x = centerX + (i - 1) * dotSpacing;
                int y = centerY + yOffset;
                Ellipse(hdcMem, x - dotRadius, y - dotRadius, x + dotRadius, y + dotRadius);
                DeleteObject(hDot);
            }
            
            // Blit to screen
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, LOADING_TIMER_ID);
            g_hLoadingWindow = NULL;
            return 0;
        }
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

void ShowLoadingIndicator() {
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = LoadingWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"GeminiLoadingHUD";
        wc.hCursor = LoadCursor(NULL, IDC_WAIT);
        wc.style = CS_DROPSHADOW;
        RegisterClassW(&wc);
        classRegistered = true;
    }
    
    // Position near cursor
    POINT pt;
    GetCursorPos(&pt);
    int x = pt.x + 16;
    int y = pt.y + 16;
    
    // Keep on screen
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    if (x + LOADING_WINDOW_SIZE > screenW) x = pt.x - LOADING_WINDOW_SIZE - 16;
    if (y + LOADING_WINDOW_SIZE > screenH) y = pt.y - LOADING_WINDOW_SIZE - 16;
    
    g_hLoadingWindow = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"GeminiLoadingHUD", L"",
        WS_POPUP | WS_VISIBLE,
        x, y, LOADING_WINDOW_SIZE, LOADING_WINDOW_SIZE,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    
    // Force repaint to start animation
    if (g_hLoadingWindow) {
        UpdateWindow(g_hLoadingWindow);
    }
}

void HideLoadingIndicator() {
    if (g_hLoadingWindow) {
        DestroyWindow(g_hLoadingWindow);
        g_hLoadingWindow = NULL;
    }
}

// Pump messages briefly so the loading window animates during API call
void PumpMessages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// ---------------------------------------------------------------------------
// Network: Call Gemini API (with loading indicator pumping)
// ---------------------------------------------------------------------------
std::wstring CallGemini(const std::wstring& prompt, const std::wstring& contextText) {
    Wh_Log(L"Sending request to Gemini...");
    
    PCWSTR settingApiKey = Wh_GetStringSetting(L"ApiKey");
    std::wstring apiKey = (settingApiKey && settingApiKey[0] != L'\0') ? settingApiKey : L"";
    if (apiKey.empty()) {
        Wh_FreeStringSetting(settingApiKey);
        return L"⚠ Error: API Key Missing\n\nPlease enter your Gemini API key in the mod settings.\n\n"
               L"1. Go to https://aistudio.google.com/ to get a free key\n"
               L"2. Open Windhawk → Gemini AI Assistant → Settings\n"
               L"3. Paste your key into the API Key field";
    }
    Wh_FreeStringSetting(settingApiKey);
    
    PCWSTR settingModel = Wh_GetStringSetting(L"ModelName");
    std::wstring modelName = (settingModel && settingModel[0] != L'\0') ? settingModel : L"gemini-flash-latest";
    Wh_FreeStringSetting(settingModel);
    
    PCWSTR settingSystemPrompt = Wh_GetStringSetting(L"SystemPrompt");
    std::wstring systemPrompt = (settingSystemPrompt && settingSystemPrompt[0] != L'\0') 
        ? settingSystemPrompt 
        : L"You are a helpful AI assistant integrated into Windows 11. Format responses with markdown.";
    Wh_FreeStringSetting(settingSystemPrompt);
    
    std::string jsonBody = R"({
        "system_instruction": { "parts": [{"text": ")" + EscapeJSON(ToString(systemPrompt)) + R"("}] },
        "contents": [{ "parts": [{"text": ")" + EscapeJSON(ToString(prompt) + ":\n\n" + ToString(contextText)) + R"("}] }]
    })";
    
    HINTERNET hSession = WinHttpOpen(L"WindhawkGeminiMod/1.4.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return L"⚠ Network Error: Failed to initialize HTTP session.\n\nCheck your internet connection and try again.";
    
    DWORD timeout = 45000; // Increased to 45s for longer responses
    WinHttpSetOption(hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hSession, WINHTTP_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hSession, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    
    HINTERNET hConnect = WinHttpConnect(hSession, L"generativelanguage.googleapis.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return L"⚠ Connection Error: Could not reach Gemini servers.\n\nCheck your internet connection and firewall settings.";
    }
    
    std::wstring path = L"/v1beta/models/" + modelName + L":generateContent?key=" + apiKey;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return L"⚠ Request Error: Failed to create HTTP request.";
    }
    
    std::wstring headers = L"Content-Type: application/json";
    BOOL bResults = WinHttpSendRequest(hRequest, headers.c_str(), (DWORD)headers.length(), (LPVOID)jsonBody.c_str(), (DWORD)jsonBody.length(), (DWORD)jsonBody.length(), 0);
    
    if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);
    
    std::wstring responseText;
    DWORD dwStatusCode = 0, dwSize = sizeof(dwStatusCode);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);
    
    if (bResults) {
        std::string buffer;
        DWORD dwDownloaded = 0;
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;
            std::vector<char> chunk(dwSize + 1, 0);
            if (WinHttpReadData(hRequest, chunk.data(), dwSize, &dwDownloaded)) buffer.append(chunk.data(), dwDownloaded);
            PumpMessages(); // Keep loading animation alive during download
        } while (dwSize > 0);
        
        std::wstring parsed = ParseJSONResponse(buffer);
        if (dwStatusCode == 200) {
            responseText = parsed;
        } else if (dwStatusCode == 429) {
            responseText = L"⚠ Rate Limited (429)\n\nYou've sent too many requests. Please wait a moment and try again.";
        } else if (dwStatusCode == 401 || dwStatusCode == 403) {
            responseText = L"⚠ Authentication Error (" + std::to_wstring(dwStatusCode) + L")\n\nYour API key may be invalid or expired. Please check your key in the mod settings.";
        } else {
            responseText = L"⚠ API Error " + std::to_wstring(dwStatusCode) + L"\n\n" + parsed;
        }
    } else {
        DWORD err = GetLastError();
        if (err == ERROR_WINHTTP_TIMEOUT) {
            responseText = L"⚠ Timeout: The request took too long.\n\nThe selected text may be too large, or the server is slow. Try selecting less text.";
        } else {
            responseText = L"⚠ Network Error: No response received (Error " + std::to_wstring(err) + L").\n\nCheck your internet connection.";
        }
    }
    
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    return responseText;
}

// ---------------------------------------------------------------------------
// Rich Text Logic
// ---------------------------------------------------------------------------
void AppendText(HWND hEdit, const std::wstring& text, COLORREF color, int size, bool bold, bool italic = false, bool code = false) {
    CHARFORMAT2 cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_WEIGHT | CFM_FACE | CFM_ITALIC;
    cf.crTextColor = color;
    cf.yHeight = size * 20; // Twips
    cf.wWeight = bold ? FW_BOLD : FW_NORMAL;
    cf.dwEffects = italic ? CFE_ITALIC : 0;
    
    if (code) wcscpy_s(cf.szFaceName, L"Consolas");
    else wcscpy_s(cf.szFaceName, L"Segoe UI");

    int len = GetWindowTextLength(hEdit);
    SendMessage(hEdit, EM_SETSEL, len, len);
    SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)text.c_str());
}

void SetParagraphFormat(HWND hEdit, bool bullet, bool indented = false) {
    PARAFORMAT2 pf;
    ZeroMemory(&pf, sizeof(pf));
    pf.cbSize = sizeof(pf);
    pf.dwMask = PFM_NUMBERING | PFM_OFFSET | PFM_STARTINDENT | PFM_SPACEAFTER;
    pf.dySpaceAfter = 60;
    if (bullet) {
        pf.wNumbering = PFN_BULLET;
        pf.dxOffset = 100;
        pf.dxStartIndent = 100;
    } else if (indented) {
        pf.wNumbering = 0;
        pf.dxOffset = 0;
        pf.dxStartIndent = 200; // Indent for code blocks
    } else {
        pf.wNumbering = 0;
        pf.dxOffset = 0;
        pf.dxStartIndent = 0;
    }
    SendMessage(hEdit, EM_SETPARAFORMAT, SCF_SELECTION, (LPARAM)&pf);
}

// Parse inline formatting within a line, handling **bold**, *italic*, and `code`
void RenderInlineFormatting(HWND hEdit, const std::wstring& line) {
    size_t pos = 0;
    while (pos < line.length()) {
        // Find next token
        size_t nextBold = line.find(L"**", pos);
        size_t nextCode = line.find(L"`", pos);
        size_t nextItalic = std::wstring::npos;
        
        // Check for single * that is NOT **
        size_t searchFrom = pos;
        while (true) {
            size_t candidate = line.find(L"*", searchFrom);
            if (candidate == std::wstring::npos) break;
            // Make sure it's not part of **
            if (candidate + 1 < line.length() && line[candidate + 1] == L'*') {
                searchFrom = candidate + 2;
                continue;
            }
            if (candidate > 0 && line[candidate - 1] == L'*') {
                searchFrom = candidate + 1;
                continue;
            }
            nextItalic = candidate;
            break;
        }
        
        // Determine which token comes first
        size_t nextToken = std::wstring::npos;
        int tokenType = 0; // 1=Bold, 2=Code, 3=Italic

        // Find minimum
        if (nextBold != std::wstring::npos) { nextToken = nextBold; tokenType = 1; }
        if (nextCode != std::wstring::npos && (nextToken == std::wstring::npos || nextCode < nextToken)) { nextToken = nextCode; tokenType = 2; }
        if (nextItalic != std::wstring::npos && (nextToken == std::wstring::npos || nextItalic < nextToken)) { nextToken = nextItalic; tokenType = 3; }

        if (nextToken != std::wstring::npos) {
            // Print plain text before token
            if (nextToken > pos) {
                AppendText(hEdit, line.substr(pos, nextToken - pos), Colors::TextPrimary, 11, false);
            }
            
            if (tokenType == 1) { // **Bold**
                size_t endBold = line.find(L"**", nextToken + 2);
                if (endBold != std::wstring::npos) {
                    AppendText(hEdit, line.substr(nextToken + 2, endBold - (nextToken + 2)), Colors::TextPrimary, 11, true);
                    pos = endBold + 2;
                } else {
                    AppendText(hEdit, line.substr(nextToken), Colors::TextPrimary, 11, false);
                    pos = line.length();
                }
            } else if (tokenType == 2) { // `Code`
                size_t endCode = line.find(L"`", nextToken + 1);
                if (endCode != std::wstring::npos) {
                    AppendText(hEdit, line.substr(nextToken + 1, endCode - (nextToken + 1)), Colors::TextCode, 10, false, false, true);
                    pos = endCode + 1;
                } else {
                    AppendText(hEdit, line.substr(nextToken), Colors::TextPrimary, 11, false);
                    pos = line.length();
                }
            } else if (tokenType == 3) { // *Italic*
                size_t endItalic = std::wstring::npos;
                size_t search = nextToken + 1;
                while (true) {
                    size_t candidate = line.find(L"*", search);
                    if (candidate == std::wstring::npos) break;
                    if (candidate + 1 < line.length() && line[candidate + 1] == L'*') { search = candidate + 2; continue; }
                    if (candidate > 0 && line[candidate - 1] == L'*') { search = candidate + 1; continue; }
                    endItalic = candidate;
                    break;
                }
                if (endItalic != std::wstring::npos) {
                    AppendText(hEdit, line.substr(nextToken + 1, endItalic - (nextToken + 1)), Colors::TextItalic, 11, false, true);
                    pos = endItalic + 1;
                } else {
                    AppendText(hEdit, line.substr(nextToken), Colors::TextPrimary, 11, false);
                    pos = line.length();
                }
            }
        } else {
            // No more tokens, output rest as plain text
            AppendText(hEdit, line.substr(pos), Colors::TextPrimary, 11, false);
            pos = line.length();
        }
    }
}

void RenderMarkdown(HWND hEdit, const std::wstring& markdown) {
    std::wstringstream ss(markdown);
    std::wstring line;
    bool inCodeBlock = false;
    
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        
        // 1. Code Block Toggle (```)
        if (line.find(L"```") == 0) {
            if (!inCodeBlock) {
                inCodeBlock = true;
                // Optional: show language label
                std::wstring lang = line.substr(3);
                if (!lang.empty()) {
                    // Trim whitespace
                    size_t start = lang.find_first_not_of(L" \t");
                    if (start != std::wstring::npos) {
                        lang = lang.substr(start);
                        SetParagraphFormat(hEdit, false, true);
                        AppendText(hEdit, lang + L"\r\n", Colors::TextSecondary, 9, false, true, true);
                    }
                }
            } else {
                inCodeBlock = false;
                // Add spacing after code block
                SetParagraphFormat(hEdit, false);
                AppendText(hEdit, L"\r\n", Colors::TextPrimary, 6, false);
            }
            continue;
        }
        
        // 2. Inside code block - render as code
        if (inCodeBlock) {
            SetParagraphFormat(hEdit, false, true);
            AppendText(hEdit, line + L"\r\n", Colors::TextCode, 10, false, false, true);
            continue;
        }
        
        // 3. Headers (### ## #)
        if (line.rfind(L"###", 0) == 0 || line.rfind(L"##", 0) == 0 || line.rfind(L"#", 0) == 0) {
            size_t hashCount = 0;
            while (hashCount < line.length() && line[hashCount] == L'#') hashCount++;
            size_t firstChar = line.find_first_not_of(L"# ");
            if (firstChar != std::wstring::npos) line = line.substr(firstChar);
            SetParagraphFormat(hEdit, false);
            int fontSize = (hashCount == 1) ? 15 : (hashCount == 2) ? 14 : 13;
            AppendText(hEdit, line + L"\r\n", Colors::TextHeader, fontSize, true);
            continue;
        }
        
        // 4. Horizontal rule (---, ***)
        if (line == L"---" || line == L"***" || line == L"___") {
            SetParagraphFormat(hEdit, false);
            AppendText(hEdit, L"────────────────────────────\r\n", Colors::Border, 10, false);
            continue;
        }
        
        // 5. Bullets (* or -)
        if (line.rfind(L"* ", 0) == 0 || line.rfind(L"- ", 0) == 0) {
            line = line.substr(2);
            SetParagraphFormat(hEdit, true);
            RenderInlineFormatting(hEdit, line);
            AppendText(hEdit, L"\r\n", Colors::TextPrimary, 11, false);
            continue;
        }
        
        // 6. Numbered lists (1. 2. etc.)
        if (line.length() >= 3) {
            size_t dotPos = line.find(L". ");
            if (dotPos != std::wstring::npos && dotPos <= 3) {
                bool isNumbered = true;
                for (size_t i = 0; i < dotPos; i++) {
                    if (!iswdigit(line[i])) { isNumbered = false; break; }
                }
                if (isNumbered) {
                    std::wstring prefix = line.substr(0, dotPos + 2);
                    line = line.substr(dotPos + 2);
                    SetParagraphFormat(hEdit, false);
                    AppendText(hEdit, prefix, Colors::Accent, 11, true);
                    RenderInlineFormatting(hEdit, line);
                    AppendText(hEdit, L"\r\n", Colors::TextPrimary, 11, false);
                    continue;
                }
            }
        }

        // 7. Regular paragraph with inline formatting
        SetParagraphFormat(hEdit, false);
        RenderInlineFormatting(hEdit, line);
        AppendText(hEdit, L"\r\n", Colors::TextPrimary, 11, false);
    }
}

// ---------------------------------------------------------------------------
// GUI: Modern Custom Button Drawing
// ---------------------------------------------------------------------------
struct ButtonState { bool isHovered; bool isPressed; bool isPrimary; };
std::map<HWND, ButtonState> g_buttonStates;

LRESULT CALLBACK ButtonSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    ButtonState& state = g_buttonStates[hwnd];
    switch (msg) {
        case WM_MOUSEMOVE:
            if (!state.isHovered) {
                state.isHovered = true;
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        case WM_MOUSELEAVE: state.isHovered = false; state.isPressed = false; InvalidateRect(hwnd, NULL, FALSE); break;
        case WM_LBUTTONDOWN: state.isPressed = true; InvalidateRect(hwnd, NULL, FALSE); break;
        case WM_LBUTTONUP: state.isPressed = false; InvalidateRect(hwnd, NULL, FALSE); break;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            
            // Double buffer the button
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
            
            COLORREF bgColor = state.isPrimary 
                ? (state.isPressed ? Colors::Accent : (state.isHovered ? Colors::AccentHover : Colors::Accent)) 
                : (state.isPressed ? Colors::SurfaceLight : (state.isHovered ? Colors::ButtonHover : Colors::ButtonBg));
            COLORREF textColor = state.isPrimary ? RGB(10, 10, 15) : Colors::TextPrimary;
            
            // Fill parent bg first for clean edges
            HBRUSH hBgFill = CreateSolidBrush(Colors::Background);
            FillRect(hdcMem, &rc, hBgFill);
            DeleteObject(hBgFill);
            
            HBRUSH hBrush = CreateSolidBrush(bgColor);
            SelectObject(hdcMem, hBrush);
            SelectObject(hdcMem, (HPEN)GetStockObject(NULL_PEN));
            RoundRect(hdcMem, rc.left, rc.top, rc.right, rc.bottom, 10, 10);
            DeleteObject(hBrush);
            
            SetBkMode(hdcMem, TRANSPARENT);
            SetTextColor(hdcMem, textColor);
            SelectObject(hdcMem, g_hFontButton);
            wchar_t text[256]; GetWindowTextW(hwnd, text, 256);
            DrawTextW(hdcMem, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCDESTROY: g_buttonStates.erase(hwnd); RemoveWindowSubclass(hwnd, ButtonSubclassProc, uIdSubclass); break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// GUI: Modern Result Window (Resizable)
// ---------------------------------------------------------------------------
HWND g_hResultWindow = NULL;
HWND g_hEditControl = NULL;

void LayoutResultWindow(HWND hwnd) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int clientW = clientRect.right - clientRect.left;
    int clientH = clientRect.bottom - clientRect.top;
    int contentW = clientW - (PADDING * 2);
    
    // Reposition title and subtitle
    HWND hTitle = GetDlgItem(hwnd, ID_STATIC_TITLE);
    HWND hAction = GetDlgItem(hwnd, ID_STATIC_ACTION);
    if (hTitle) SetWindowPos(hTitle, NULL, PADDING, PADDING, contentW, TITLE_HEIGHT, SWP_NOZORDER);
    if (hAction) SetWindowPos(hAction, NULL, PADDING, PADDING + TITLE_HEIGHT + 4, contentW, SUBTITLE_HEIGHT, SWP_NOZORDER);
    
    int editTop = PADDING + TITLE_HEIGHT + 4 + SUBTITLE_HEIGHT + 16;
    int buttonY = clientH - PADDING - BUTTON_HEIGHT;
    int editHeight = buttonY - editTop - 12;
    
    if (g_hEditControl) SetWindowPos(g_hEditControl, NULL, PADDING, editTop, contentW, editHeight, SWP_NOZORDER);
    
    HWND hBtnCopy = GetDlgItem(hwnd, ID_BTN_COPY);
    HWND hBtnClose = GetDlgItem(hwnd, ID_BTN_CLOSE);
    if (hBtnCopy) SetWindowPos(hBtnCopy, NULL, PADDING, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, SWP_NOZORDER);
    if (hBtnClose) SetWindowPos(hBtnClose, NULL, clientW - PADDING - BUTTON_WIDTH, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, SWP_NOZORDER);
    
    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT CALLBACK ResultWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            EnableDarkTitleBar(hwnd);
            RECT clientRect; GetClientRect(hwnd, &clientRect);
            int clientW = clientRect.right - clientRect.left, clientH = clientRect.bottom - clientRect.top;
            int contentW = clientW - (PADDING * 2), yPos = PADDING;
            
            HWND hTitle = CreateWindowW(L"STATIC", L"Gemini Assistant", WS_CHILD | WS_VISIBLE | SS_LEFT, PADDING, yPos, contentW, TITLE_HEIGHT, hwnd, (HMENU)ID_STATIC_TITLE, NULL, NULL);
            SendMessage(hTitle, WM_SETFONT, (WPARAM)g_hFontTitle, TRUE);
            yPos += TITLE_HEIGHT + 4;
            
            HWND hAction = CreateWindowW(L"STATIC", g_currentAction.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, PADDING, yPos, contentW, SUBTITLE_HEIGHT, hwnd, (HMENU)ID_STATIC_ACTION, NULL, NULL);
            SendMessage(hAction, WM_SETFONT, (WPARAM)g_hFontSubtitle, TRUE);
            yPos += SUBTITLE_HEIGHT + 16;
            
            int editHeight = clientH - yPos - BUTTON_HEIGHT - 32;
            g_hEditControl = CreateWindowExW(0, MSFTEDIT_CLASS, L"", 
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                PADDING, yPos, contentW, editHeight, hwnd, (HMENU)ID_RICH_TEXT, NULL, NULL);
            SendMessage(g_hEditControl, EM_SETBKGNDCOLOR, 0, Colors::SurfaceLight);
            SendMessage(g_hEditControl, EM_SETEVENTMASK, 0, ENM_LINK);
            
            // Set margins inside the rich edit for better readability
            RECT editMargin = { 10, 8, 10, 8 };
            SendMessage(g_hEditControl, EM_SETRECT, 0, 0); // Reset first
            SendMessage(g_hEditControl, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(10, 10));
            
            SetWindowTheme(g_hEditControl, L"DarkMode_Explorer", NULL);

            int buttonY = clientH - PADDING - BUTTON_HEIGHT;
            HWND hBtnCopy = CreateWindowW(L"BUTTON", L"Copy to Clipboard", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, PADDING, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)ID_BTN_COPY, NULL, NULL);
            g_buttonStates[hBtnCopy] = { false, false, true }; SetWindowSubclass(hBtnCopy, ButtonSubclassProc, 0, 0);
            
            HWND hBtnClose = CreateWindowW(L"BUTTON", L"Close  (Esc)", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, clientW - PADDING - BUTTON_WIDTH, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, (HMENU)ID_BTN_CLOSE, NULL, NULL);
            g_buttonStates[hBtnClose] = { false, false, false }; SetWindowSubclass(hBtnClose, ButtonSubclassProc, 0, 0);
            return 0;
        }
        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = WINDOW_MIN_WIDTH;
            mmi->ptMinTrackSize.y = WINDOW_MIN_HEIGHT;
            return 0;
        }
        case WM_SIZE: {
            if (wParam != SIZE_MINIMIZED) {
                LayoutResultWindow(hwnd);
            }
            return 0;
        }
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }
        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam; SetBkMode(hdc, TRANSPARENT);
            int id = GetDlgCtrlID((HWND)lParam);
            SetTextColor(hdc, (id == ID_STATIC_ACTION) ? Colors::Accent : Colors::TextPrimary);
            return (LRESULT)g_hBrushBg;
        }
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam; RECT rc; GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, g_hBrushBg);
            if (g_hEditControl) {
                RECT r; GetWindowRect(g_hEditControl, &r); MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&r, 2);
                InflateRect(&r, 1, 1);
                HBRUSH br = CreateSolidBrush(Colors::Border); FrameRect(hdc, &r, br); DeleteObject(br);
            }
            return 1;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BTN_COPY) {
                int len = GetWindowTextLength(g_hEditControl);
                if (len > 0) {
                    std::vector<wchar_t> buf(len + 1);
                    GetWindowText(g_hEditControl, buf.data(), len + 1);
                    if (SetClipboardText(buf.data())) {
                        HWND hBtn = GetDlgItem(hwnd, ID_BTN_COPY);
                        SetWindowTextW(hBtn, L"✓ Copied!"); InvalidateRect(hBtn, NULL, FALSE);
                        MessageBeep(MB_OK);
                        SetTimer(hwnd, 1, 1500, NULL);
                    }
                }
            } else if (LOWORD(wParam) == ID_BTN_CLOSE) DestroyWindow(hwnd);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == 1) {
                KillTimer(hwnd, 1);
                HWND hBtn = GetDlgItem(hwnd, ID_BTN_COPY);
                if (hBtn) { SetWindowTextW(hBtn, L"Copy to Clipboard"); InvalidateRect(hBtn, NULL, FALSE); }
            }
            return 0;
        }
        case WM_DESTROY: g_hResultWindow = NULL; g_hEditControl = NULL; PostQuitMessage(0); return 0;
        default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ShowResultWindow(const std::wstring& rawMarkdown, const std::wstring& actionName) {
    g_currentAction = actionName;
    
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = ResultWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"GeminiResultHUD_v4";
        wc.hbrBackground = g_hBrushBg;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.style = CS_DROPSHADOW;
        RegisterClassW(&wc);
        classRegistered = true;
    }
    
    int w = WINDOW_WIDTH, h = WINDOW_HEIGHT;
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
    
    // WS_THICKFRAME enables resizing, WS_MAXIMIZEBOX allows maximize
    g_hResultWindow = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_APPWINDOW,
        L"GeminiResultHUD_v4", L"Gemini Assistant",
        WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME,
        x, y, w, h,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    
    if (g_hEditControl) RenderMarkdown(g_hEditControl, rawMarkdown);
    SetForegroundWindow(g_hResultWindow);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        // Handle Escape key globally for this window
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            if (g_hResultWindow) { DestroyWindow(g_hResultWindow); continue; }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// ---------------------------------------------------------------------------
// Main Logic: Hotkey Handler
// ---------------------------------------------------------------------------
void OnHotkeyTriggered() {
    Wh_Log(L"Hotkey triggered.");
    std::wstring selectedText = GetSelectedText();
    if (selectedText.empty()) {
        MessageBeep(MB_ICONWARNING);
        return;
    }
    
    std::vector<std::wstring> customPrompts;
    PCWSTR settingPrompts = Wh_GetStringSetting(L"CustomPrompts");
    if (settingPrompts) {
        std::wstringstream ss(settingPrompts); std::wstring seg;
        while (std::getline(ss, seg, L';')) {
            size_t start = seg.find_first_not_of(L" \t");
            if (start != std::wstring::npos) customPrompts.push_back(seg.substr(start));
        }
        Wh_FreeStringSetting(settingPrompts);
    }
    
    POINT pt; GetCursorPos(&pt);
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, 1, L"📝  Summarize");
    AppendMenuW(hMenu, MF_STRING, 2, L"💡  Explain");
    AppendMenuW(hMenu, MF_STRING, 3, L"✏️  Fix Grammar (Auto-Replace)");
    AppendMenuW(hMenu, MF_STRING, 4, L"💻  Explain Code");
    AppendMenuW(hMenu, MF_STRING, 5, L"🔄  Rewrite Professionally");
    AppendMenuW(hMenu, MF_STRING, 6, L"🔍  Key Points & Takeaways");
    if (!customPrompts.empty()) {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        for (size_t i = 0; i < customPrompts.size(); i++) {
            AppendMenuW(hMenu, MF_STRING, 100 + i, (L"⚡  " + customPrompts[i]).c_str());
        }
    }
    
    SetForegroundWindow(g_hHelperWindow);
    int selection = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, g_hHelperWindow, NULL);
    PostMessage(g_hHelperWindow, WM_NULL, 0, 0); DestroyMenu(hMenu);
    if (selection == 0) return;
    
    std::wstring prompt, actionName;
    bool isAutoReplace = (selection == 3);
    if (selection >= 100) {
        prompt = customPrompts[selection - 100];
        actionName = customPrompts[selection - 100];
    } else {
        switch (selection) {
            case 1: prompt = L"Summarize concisely";                                 actionName = L"Summary"; break;
            case 2: prompt = L"Explain simply";                                       actionName = L"Explanation"; break;
            case 3: prompt = L"Fix grammar, return only corrected text";             actionName = L"Grammar Fix"; break;
            case 4: prompt = L"Explain code";                                         actionName = L"Code Explanation"; break;
            case 5: prompt = L"Rewrite this text in a clear, professional tone";     actionName = L"Professional Rewrite"; break;
            case 6: prompt = L"Extract the key points and main takeaways as a list"; actionName = L"Key Points"; break;
        }
    }
    
    // Show loading indicator
    ShowLoadingIndicator();
    
    // Make the API call, pumping messages periodically so the spinner animates
    // We run the call synchronously but the loading window still animates via PumpMessages() inside CallGemini
    std::wstring rawResponse = CallGemini(prompt, selectedText);
    
    // Hide loading indicator
    HideLoadingIndicator();
    
    if (isAutoReplace) {
        if (SetClipboardText(rawResponse)) {
            SendPaste();
            Wh_Log(L"Auto-replaced.");
        }
    } else {
        ShowResultWindow(rawResponse, actionName);
    }
}

LRESULT CALLBACK HelperWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI HotkeyThread(LPVOID lpParam) {
    // Load Rich Edit DLL
    g_hRichEditMod = LoadLibrary(L"Msftedit.dll");
    
    // Create GDI Resources
    g_hBrushBg = CreateSolidBrush(Colors::Background);
    g_hBrushEditBg = CreateSolidBrush(Colors::SurfaceLight);
    g_hBrushButtonNormal = CreateSolidBrush(Colors::ButtonBg);
    g_hBrushButtonHover = CreateSolidBrush(Colors::ButtonHover);
    g_hBrushAccent = CreateSolidBrush(Colors::Accent);
    
    g_hFontTitle = CreateFontW(24, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_SWISS, L"Segoe UI");
    g_hFontSubtitle = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_SWISS, L"Segoe UI");
    g_hFontBody = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_MODERN, L"Consolas");
    g_hFontButton = CreateFontW(15, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_SWISS, L"Segoe UI");
    
    // Create invisible helper window for hotkey & menu ownership
    WNDCLASSW wc = {}; wc.lpfnWndProc = HelperWndProc; wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkGeminiHelper_v4"; RegisterClassW(&wc);
    g_hHelperWindow = CreateWindowW(wc.lpszClassName, L"GeminiHelper", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);
    
    if (!RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_SHIFT, 0x47)) {
        Wh_Log(L"Failed to register hotkey Ctrl+Shift+G. It may already be in use by another application.");
        return 1;
    }
    
    Wh_Log(L"Gemini AI Assistant ready. Press Ctrl+Shift+G to activate.");
    
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) OnHotkeyTriggered();
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnregisterHotKey(NULL, 1);
    DestroyWindow(g_hHelperWindow);
    if (g_hLoadingWindow) DestroyWindow(g_hLoadingWindow);
    if (g_hRichEditMod) FreeLibrary(g_hRichEditMod);
    DeleteObject(g_hBrushBg); DeleteObject(g_hBrushEditBg); DeleteObject(g_hBrushButtonNormal);
    DeleteObject(g_hBrushButtonHover); DeleteObject(g_hBrushAccent);
    DeleteObject(g_hFontTitle); DeleteObject(g_hFontSubtitle); DeleteObject(g_hFontBody); DeleteObject(g_hFontButton);
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Gemini AI Assistant v1.4.0 Loading...");
    g_hThread = CreateThread(NULL, 0, HotkeyThread, NULL, 0, &g_dwThreadId);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hThread) {
        if (g_dwThreadId) PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, 2000);
        CloseHandle(g_hThread);
    }
}
