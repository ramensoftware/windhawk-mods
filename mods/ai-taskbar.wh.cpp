// ==WindhawkMod==
// @id             ai-taskbar
// @name           Modern AI Taskbar
// @description    An ai assistant built into your taskbar
// @version        1.2
// @author         Frqme
// @github         https://github.com/Frqmelikescheese
// @include        explorer.exe
// @compilerOptions -luser32 -lgdi32 -ldwmapi -lwinhttp -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- apiKey: ""
  $name: OpenRouter API Key (Required)
- model: "openai/gpt-3.5-turbo"
  $name: Model ID
- xOffset: 120
  $name: Button X Position (from Left)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>
#include <richedit.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <string>
#include <vector>

#define CHAT_W 400
#define CHAT_H 550
#define BTN_SIZE 40
#define ID_TIMER_SYNC 9001
#define ID_BTN_SEND 200
#define ID_BTN_CLEAR 201

static std::wstring g_apiKey;
static std::wstring g_model;
static int g_xOffset = 120;

static HWND g_hBtn = NULL;
static HWND g_hChat = NULL;
static HWND g_hOut = NULL;
static HWND g_hIn = NULL;
static WNDPROC g_origInputProc = NULL;

static int g_currentMode = 0; 
static BOOL g_isOpen = FALSE;
static HFONT g_hFontNormal = NULL;
static HFONT g_hFontBold = NULL;
static HBRUSH g_hBrushInput = NULL;

#define C_BG        RGB(32, 32, 32)
#define C_INPUT     RGB(50, 50, 50)
#define C_ACCENT    RGB(0, 120, 215)
#define C_TEXT      RGB(245, 245, 245)
#define C_TEXT_DIM  RGB(150, 150, 150)

void UpdatePosition() {
    if (!g_hBtn) return;
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        RECT rTask; 
        GetWindowRect(hTaskbar, &rTask);
        int yPos = rTask.top + (rTask.bottom - rTask.top - BTN_SIZE)/2;
        int xPos = rTask.left + g_xOffset;
        
        SetWindowPos(g_hBtn, HWND_TOPMOST, xPos, yPos, BTN_SIZE, BTN_SIZE, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        
        if (g_isOpen && g_hChat) {
             SetWindowPos(g_hChat, HWND_TOPMOST, xPos, yPos - CHAT_H - 10, CHAT_W, CHAT_H, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
}

std::wstring ExtractContent(std::string json) {
    std::string key = "\"content\":\"";
    size_t start = json.find(key);
    if (start == std::string::npos) return L"";
    start += key.length();
    std::string content = "";
    for (size_t i = start; i < json.length(); i++) {
        if (json[i] == '"' && json[i-1] != '\\') break;
        if (json[i] == '\\' && json[i+1] == 'n') { content += "\r\n"; i++; }
        else if (json[i] != '\\') { content += json[i]; }
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &content[0], (int)content.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &content[0], (int)content.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

struct ThreadData { std::wstring prompt; std::wstring sys; };

DWORD WINAPI ApiThread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    HINTERNET hSession = WinHttpOpen(L"WindhawkAI/4.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        delete data;
        return 0;
    }
    HINTERNET hConnect = WinHttpConnect(hSession, L"openrouter.ai", INTERNET_DEFAULT_HTTPS_PORT, 0);
    
    std::wstring headers = L"Authorization: Bearer " + g_apiKey + L"\r\nContent-Type: application/json";
    
    auto ToUtf8 = [](std::wstring s) {
        int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
        std::string str(len, 0);
        WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &str[0], len, NULL, NULL);
        if(!str.empty()) str.pop_back();
        return str;
    };
    
    std::string body = "{\"model\": \"" + ToUtf8(g_model) + "\", \"messages\": [{\"role\": \"system\", \"content\": \"" + ToUtf8(data->sys) + "\"}, {\"role\": \"user\", \"content\": \"" + ToUtf8(data->prompt) + "\"}]}";
    
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api/v1/chat/completions", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    
    if (WinHttpSendRequest(hRequest, headers.c_str(), headers.length(), (LPVOID)body.c_str(), body.length(), body.length(), 0)) {
        WinHttpReceiveResponse(hRequest, NULL);
        std::string response = "";
        DWORD dwSize = 0, dwDownloaded = 0;
        do {
            dwSize = 0;
            WinHttpQueryDataAvailable(hRequest, &dwSize);
            if (!dwSize) break;
            std::vector<char> buffer(dwSize + 1);
            if (WinHttpReadData(hRequest, &buffer[0], dwSize, &dwDownloaded)) {
                buffer[dwDownloaded] = 0;
                response += &buffer[0];
            }
        } while (dwSize > 0);
        
        std::wstring reply = ExtractContent(response);
        if(reply.empty()) reply = L"...";

        CHARRANGE cr = { -1, -1 };
        SendMessage(g_hOut, EM_EXSETSEL, 0, (LPARAM)&cr);
        
        CHARFORMAT2 cf = { sizeof(CHARFORMAT2) };
        cf.dwMask = CFM_COLOR | CFM_BOLD;
        cf.crTextColor = RGB(0, 200, 100); 
        cf.dwEffects = CFE_BOLD;
        SendMessage(g_hOut, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
        SendMessage(g_hOut, EM_REPLACESEL, 0, (LPARAM)L"\r\nAI: ");
        
        cf.dwEffects = 0;
        cf.crTextColor = C_TEXT;
        SendMessage(g_hOut, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
        SendMessage(g_hOut, EM_REPLACESEL, 0, (LPARAM)reply.c_str());
        SendMessage(g_hOut, EM_REPLACESEL, 0, (LPARAM)L"\r\n\r\n");
        SendMessage(g_hOut, WM_VSCROLL, SB_BOTTOM, 0);
    }
    
    WinHttpCloseHandle(hRequest); 
    WinHttpCloseHandle(hConnect); 
    WinHttpCloseHandle(hSession);
    delete data;
    return 0;
}

void SendChat() {
    int len = GetWindowTextLength(g_hIn);
    if (len == 0) return;
    std::vector<wchar_t> buf(len + 1);
    GetWindowText(g_hIn, &buf[0], len + 1);
    std::wstring prompt = &buf[0];
    SetWindowText(g_hIn, L"");
    
    CHARRANGE cr = { -1, -1 };
    SendMessage(g_hOut, EM_EXSETSEL, 0, (LPARAM)&cr);
    
    CHARFORMAT2 cf = { sizeof(CHARFORMAT2) };
    cf.dwMask = CFM_COLOR | CFM_BOLD;
    cf.crTextColor = C_ACCENT;
    cf.dwEffects = CFE_BOLD;
    SendMessage(g_hOut, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessage(g_hOut, EM_REPLACESEL, 0, (LPARAM)L"\r\nYou: ");
    
    cf.dwEffects = 0;
    cf.crTextColor = C_TEXT;
    SendMessage(g_hOut, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessage(g_hOut, EM_REPLACESEL, 0, (LPARAM)prompt.c_str());
    
    std::wstring sys = L"You are a helpful assistant.";
    if (g_currentMode == 1) sys = L"You are a master programmer. Code only.";
    if (g_currentMode == 2) sys = L"You are a creative writer.";
    
    HANDLE hThread = CreateThread(NULL, 0, ApiThread, new ThreadData{prompt, sys}, 0, NULL);
    if (hThread) {
        CloseHandle(hThread);
    }
}

LRESULT CALLBACK InputSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_CHAR && wParam == VK_RETURN) {
        if (GetKeyState(VK_SHIFT) < 0) return CallWindowProc(g_origInputProc, hWnd, uMsg, wParam, lParam);
        SendChat();
        return 0;
    }
    return CallWindowProc(g_origInputProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ChatProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            LoadLibrary(L"Msftedit.dll");
            g_hFontNormal = CreateFont(17,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
            g_hFontBold = CreateFont(16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
            g_hBrushInput = CreateSolidBrush(C_INPUT);

            g_hOut = CreateWindowEx(0, MSFTEDIT_CLASS, L"", 
                WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_READONLY|ES_AUTOVSCROLL, 
                15, 60, CHAT_W-30, CHAT_H-130, hWnd, NULL, GetModuleHandle(NULL), NULL);
            SendMessage(g_hOut, EM_SETBKGNDCOLOR, 0, C_BG);
            SendMessage(g_hOut, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            RECT rect = {5, 5, CHAT_W-40, CHAT_H-130};
            SendMessage(g_hOut, EM_SETRECT, 0, (LPARAM)&rect);

            g_hIn = CreateWindowEx(0, L"EDIT", L"", 
                WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL, 
                20, CHAT_H-55, CHAT_W-80, 40, hWnd, NULL, GetModuleHandle(NULL), NULL);
            SendMessage(g_hIn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            g_origInputProc = (WNDPROC)SetWindowLongPtr(g_hIn, GWLP_WNDPROC, (LONG_PTR)InputSubclass);
            
            CreateWindow(L"BUTTON", L"X", WS_CHILD|WS_VISIBLE|BS_FLAT|BS_OWNERDRAW, 
                CHAT_W-40, 10, 30, 30, hWnd, (HMENU)ID_BTN_CLEAR, GetModuleHandle(NULL), NULL);

            SendMessage(g_hOut, EM_REPLACESEL, 0, (LPARAM)L"Ready.\r\n");
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc; GetClientRect(hWnd, &rc);
            
            HBRUSH brBg = CreateSolidBrush(C_BG); FillRect(hdc, &rc, brBg); DeleteObject(brBg);
            RECT rcTop = {0,0,CHAT_W,50};
            HBRUSH brTop = CreateSolidBrush(RGB(40,40,40)); FillRect(hdc, &rcTop, brTop); DeleteObject(brTop);
            
            SetBkMode(hdc, TRANSPARENT);
            SelectObject(hdc, g_hFontBold);
            const wchar_t* modes[] = {L"Chat", L"Code", L"Creative"};
            for(int i=0; i<3; i++) {
                RECT rTab = {10 + (i*80), 0, 90 + (i*80), 50};
                if(i == g_currentMode) {
                    SetTextColor(hdc, C_ACCENT);
                    RECT rLine = {rTab.left+15, 46, rTab.right-15, 48};
                    HBRUSH brAcc = CreateSolidBrush(C_ACCENT); FillRect(hdc, &rLine, brAcc); DeleteObject(brAcc);
                } else SetTextColor(hdc, C_TEXT_DIM);
                DrawText(hdc, modes[i], -1, &rTab, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            }
            
            HBRUSH brIn = CreateSolidBrush(C_INPUT); HBRUSH old = (HBRUSH)SelectObject(hdc, brIn);
            SelectObject(hdc, GetStockObject(NULL_PEN));
            RoundRect(hdc, 10, CHAT_H-65, CHAT_W-10, CHAT_H-15, 15, 15);
            SelectObject(hdc, old); DeleteObject(brIn);
            
            EndPaint(hWnd, &ps); return 0;
        }
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT di = (LPDRAWITEMSTRUCT)lParam;
            if(di->CtlID == ID_BTN_CLEAR) {
                SetBkMode(di->hDC, TRANSPARENT);
                SetTextColor(di->hDC, RGB(200, 80, 80)); 
                DrawText(di->hDC, L"🗑", -1, &di->rcItem, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
                return TRUE;
            }
            return TRUE;
        }
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam); int y = HIWORD(lParam);
            if(y < 50) {
                if(x > 10 && x < 90) g_currentMode = 0;
                else if(x > 90 && x < 170) g_currentMode = 1;
                else if(x > 170 && x < 250) g_currentMode = 2;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam; 
            SetBkMode(hdc, TRANSPARENT); 
            SetTextColor(hdc, C_TEXT); 
            SetBkColor(hdc, C_INPUT); 
            return (LRESULT)g_hBrushInput;
        }
        case WM_COMMAND:
            if(LOWORD(wParam) == ID_BTN_CLEAR) SetWindowText(g_hOut, L"Chat Cleared.\r\n");
            break;
        case WM_DESTROY:
            if (g_hBrushInput) DeleteObject(g_hBrushInput);
            if (g_hFontNormal) DeleteObject(g_hFontNormal);
            if (g_hFontBold) DeleteObject(g_hFontBold);
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK BtnProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps); RECT rc; GetClientRect(hWnd, &rc);
            HBRUSH br = CreateSolidBrush(g_isOpen ? C_ACCENT : RGB(45,45,45)); HBRUSH old = (HBRUSH)SelectObject(hdc, br);
            SelectObject(hdc, GetStockObject(NULL_PEN)); RoundRect(hdc, 0, 0, rc.right, rc.bottom, 12, 12);
            SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, C_TEXT);
            DrawText(hdc, L"AI", -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            SelectObject(hdc, old); DeleteObject(br); EndPaint(hWnd, &ps); return 0;
        }
        case WM_LBUTTONDOWN: {
            g_isOpen = !g_isOpen;
            ShowWindow(g_hChat, g_isOpen ? SW_SHOW : SW_HIDE);
            if (g_isOpen) { UpdatePosition(); SetFocus(g_hIn); }
            InvalidateRect(hWnd, NULL, FALSE); return 0;
        }
        case WM_TIMER: {
            if (wParam == ID_TIMER_SYNC) UpdatePosition();
            break;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI MainThread(LPVOID) {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX)};
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpfnWndProc = ChatProc; wc.lpszClassName = L"WhChatV4"; RegisterClassEx(&wc);
    wc.lpfnWndProc = BtnProc; wc.lpszClassName = L"WhBtnV4"; wc.hCursor = LoadCursor(NULL, IDC_HAND); RegisterClassEx(&wc);

    g_hBtn = CreateWindowEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW, L"WhBtnV4", L"", WS_POPUP|WS_VISIBLE, 0, 0, BTN_SIZE, BTN_SIZE, NULL, NULL, GetModuleHandle(NULL), NULL);
    g_hChat = CreateWindowEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW, L"WhChatV4", L"", WS_POPUP, 0, 0, CHAT_W, CHAT_H, NULL, NULL, GetModuleHandle(NULL), NULL);
    
    DWORD attrib = DWMWCP_ROUND; DwmSetWindowAttribute(g_hChat, DWMWA_WINDOW_CORNER_PREFERENCE, &attrib, sizeof(attrib));
    
    SetTimer(g_hBtn, ID_TIMER_SYNC, 500, NULL);
    UpdatePosition(); 

    MSG msg; while(GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}

void LoadSettings() {
    g_apiKey = Wh_GetStringSetting(L"apiKey");
    g_model = Wh_GetStringSetting(L"model");
    g_xOffset = Wh_GetIntSetting(L"xOffset");
    if(g_model.empty()) g_model = L"openai/gpt-3.5-turbo";
}

BOOL Wh_ModInit() {
    LoadSettings();
    if(g_apiKey.empty()) { 
        MessageBox(NULL, L"Please set API Key", L"Windhawk", MB_OK | MB_TOPMOST); 
        return FALSE; 
    }
    CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() { 
    if (g_hBtn) SendMessage(g_hBtn, WM_CLOSE, 0, 0); 
    if (g_hChat) SendMessage(g_hChat, WM_CLOSE, 0, 0); 
}

void Wh_ModSettingsChanged() { 
    LoadSettings(); 
    UpdatePosition(); 
}
